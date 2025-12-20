/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "multiplication_methods.h"
#include "utils/arena_allocator.h"
#include <stdlib.h>
#include <string.h>

static int max_int(int x, int y) { return (x > y) ? x : y; }
static int min_int(int x, int y) { return (x < y) ? x : y; }

/*
  Upper bound on the maximum arena usage (in doubles) along a recursion path,
  for no-padding Karatsuba with split m = floor(min(nA,nB)/2) and cutoff.
*/
static size_t kara_ws_needed(int nA, int nB, int cutoff) {
    if (nA <= cutoff || nB <= cutoff) return 0;

    int t = min_int(nA, nB);
    int m = t / 2;
    if (m <= 0) return 0;

    // A0 length = m, A1 length = nA - m; same for B
    int a1 = nA - m;
    int b1 = nB - m;

    // sum lengths
    int sa = max_int(m, a1);
    int sb = max_int(m, b1);

    // local scratch: sumA[sa] + sumB[sb] + z1[sa+sb-1] + z0_temp[2*m-1] + z2_temp[nA+nB-2*m-1]
    size_t z0_temp_size = (size_t)(2 * m - 1);
    size_t z2_temp_size = (size_t)(nA + nB - 2 * m - 1);
    size_t local = (size_t)sa + (size_t)sb + (size_t)(sa + sb - 1) + z0_temp_size + z2_temp_size;

    // children
    size_t w0 = kara_ws_needed(m, m, cutoff);
    size_t w2 = kara_ws_needed(a1, b1, cutoff);
    size_t w1 = kara_ws_needed(sa, sb, cutoff);

    size_t wchild = w0;
    if (w2 > wchild) wchild = w2;
    if (w1 > wchild) wchild = w1;

    return local + wchild;
}

/* ---------------- No-padding Karatsuba recursion ---------------- */
/*
  Computes C = A * B
  A length = nA, B length = nB, C length = nA + nB - 1

  Split point:
    m = floor(min(nA,nB)/2)
    A = A0 + x^m A1  where |A0|=m, |A1|=nA-m
    B = B0 + x^m B1  where |B0|=m, |B1|=nB-m

  Identity:
    z0 = A0*B0
    z2 = A1*B1
    z1 = (A0+A1)*(B0+B1)
    C  = z0 + x^m (z1 - z0 - z2) + x^(2m) z2
*/
static int karatsuba_rec_np(
    const double *A, int nA,
    const double *B, int nB,
    int cutoff,
    double *C,
    Arena *arena
) {
    const int outLen = nA + nB - 1;

    // Base case (also handles very unbalanced inputs)
    if (nA <= cutoff || nB <= cutoff) {
        memset(C, 0, (size_t)outLen * sizeof(double));
        naive(nA, (double*)A, nB, (double*)B, C);
        return STATUS_OK;
    }

    int t = min_int(nA, nB);
    int m = t / 2;
    if (m <= 0) {
        // Fallback (should be rare if cutoff >= 2)
        memset(C, 0, (size_t)outLen * sizeof(double));
        naive(nA, (double*)A, nB, (double*)B, C);
        return STATUS_OK;
    }

    const double *A0 = A;
    const double *A1 = A + m;
    const double *B0 = B;
    const double *B1 = B + m;

    const int nA0 = m;
    const int nB0 = m;
    const int nA1 = nA - m;
    const int nB1 = nB - m;

    const int z0_len = nA0 + nB0 - 1;          // 2m - 1
    const int z2_len = nA1 + nB1 - 1;          // nA+nB-2m-1

    // sum sizes for z1
    const int sa = max_int(nA0, nA1);
    const int sb = max_int(nB0, nB1);
    const int z1_len = sa + sb - 1;

    memset(C, 0, (size_t)outLen * sizeof(double));

    // Mark arena usage for this frame
    size_t mark = arena_mark(arena);

    // Allocate locals from arena
    double *sumA = arena_alloc(arena, (size_t)sa);
    double *sumB = arena_alloc(arena, (size_t)sb);
    double *z1   = arena_alloc(arena, (size_t)z1_len);
    double *z0_temp = arena_alloc(arena, (size_t)z0_len);
    double *z2_temp = arena_alloc(arena, (size_t)z2_len);

    if (!sumA || !sumB || !z1 || !z0_temp || !z2_temp) {
        // Not enough scratch: restore and signal error
        arena_restore(arena, mark);
        return -1;
    }

    // --- z0 into temp buffer and C ---
    {
        int st = karatsuba_rec_np(A0, nA0, B0, nB0, cutoff, z0_temp, arena);
        if (st != STATUS_OK) { arena->used = mark; return st; }
        // Copy z0 to final position
        for (int i = 0; i < z0_len; i++) {
            C[i] = z0_temp[i];
        }
    }

    // --- z2 into temp buffer and C ---
    {
        int st = karatsuba_rec_np(A1, nA1, B1, nB1, cutoff, z2_temp, arena);
        if (st != STATUS_OK) { arena->used = mark; return st; }
        // Copy z2 to final position
        for (int i = 0; i < z2_len; i++) {
            C[2*m + i] = z2_temp[i];
        }
    }

    // --- sumA = A0 + A1 (aligned at index 0) ---
    for (int i = 0; i < sa; i++) {
        double v = 0.0;
        if (i < nA0) v += A0[i];
        if (i < nA1) v += A1[i];
        sumA[i] = v;
    }

    // --- sumB = B0 + B1 ---
    for (int i = 0; i < sb; i++) {
        double v = 0.0;
        if (i < nB0) v += B0[i];
        if (i < nB1) v += B1[i];
        sumB[i] = v;
    }

    // --- z1 = sumA * sumB (into temp z1 buffer) ---
    {
        int st = karatsuba_rec_np(sumA, sa, sumB, sb, cutoff, z1, arena);
        if (st != STATUS_OK) { arena->used = mark; return st; }
    }

    // --- Compute middle = z1 - z0 - z2 and add to C at offset m ---
    for (int i = 0; i < z1_len; i++) {
        double mid = z1[i];
        
        // Subtract z0 from temp buffer
        if (i < z0_len) {
            mid -= z0_temp[i];
        }
        
        // Subtract z2 from temp buffer
        if (i < z2_len) {
            mid -= z2_temp[i];
        }

        // Add the middle term at position m+i
        int idx = m + i;
        if (idx >= 0 && idx < outLen) {
            C[idx] += mid;
        }
    }

    // Restore arena for caller
    arena_restore(arena, mark);
    return STATUS_OK;
}

/* ---------------- Public entry point (no padding) ---------------- */

int karatsuba(int n1, const double *poly1, int n2, const double *poly2, double *product) {
    if (!poly1 || !poly2 || !product || n1 <= 0 || n2 <= 0) return -1;

    const int cutoff = KARATSUBA_CUTOFF;

    size_t ws_doubles = kara_ws_needed(n1, n2, cutoff);
    double *ws = (ws_doubles > 0) ? (double*)malloc(ws_doubles * sizeof(double)) : NULL;

    Arena arena = { ws, ws_doubles, 0 };

    int st = karatsuba_rec_np(poly1, n1, poly2, n2, cutoff, product, &arena);

    free(ws);
    return st;
}

