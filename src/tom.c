#include "multiplication_methods.h"
#include "utils/arena_allocator.h"
#include <stdlib.h>
#include <string.h>

static size_t tom_ws_needed(int tLen, int cutoff) {
    if (tLen <= cutoff) return 0;
    int k = (tLen + 2) / 3;
    int wLen = 2 * k - 1;
    size_t local = (size_t)(6 * k + 10 * k + 5 * wLen); // all arrays needed
    size_t child = tom_ws_needed(k, cutoff);
    return local + 5 * child; // 5 recursive calls
}

static int tom_rec(double *a, double *b, int tLen, double *z, int cutoff, Arena *arena) {
    
    if (tLen <= TOOM3_CUTOFF) {
        return karatsuba(tLen, a, tLen, b, z);
    }
    
    size_t mark = arena_mark(arena);
    const int outLen = 2 * tLen - 1;
    memset(z, 0, (size_t)outLen * sizeof(double));
    
    int k = (tLen + 2) / 3;
    
    // Allocate from arena
    double *a0 = arena_alloc(arena, k);
    double *a1 = arena_alloc(arena, k);
    double *a2 = arena_alloc(arena, k);
    double *b0 = arena_alloc(arena, k);
    double *b1 = arena_alloc(arena, k);
    double *b2 = arena_alloc(arena, k);
    
    double *u0 = arena_alloc(arena, k);
    double *u1 = arena_alloc(arena, k);
    double *u_1 = arena_alloc(arena, k);
    double *u_2 = arena_alloc(arena, k);
    double *u_inf = arena_alloc(arena, k);
    
    double *v0 = arena_alloc(arena, k);
    double *v1 = arena_alloc(arena, k);
    double *v_1 = arena_alloc(arena, k);
    double *v_2 = arena_alloc(arena, k);
    double *v_inf = arena_alloc(arena, k);
    
    const int wLen = 2 * k - 1;
    double *w0 = arena_alloc(arena, wLen);
    double *w1 = arena_alloc(arena, wLen);
    double *w_1 = arena_alloc(arena, wLen);
    double *w_2 = arena_alloc(arena, wLen);
    double *w_inf = arena_alloc(arena, wLen);
    
    if (!a0 || !a1 || !a2 || !b0 || !b1 || !b2 || !u0 || !u1 || !u_1 || !u_2 || !u_inf ||
        !v0 || !v1 || !v_1 || !v_2 || !v_inf || !w0 || !w1 || !w_1 || !w_2 || !w_inf) {
        arena_restore(arena, mark);
        return -1;
    }
    
    // Initialize arrays
    memset(a0, 0, k * sizeof(double));
    memset(a1, 0, k * sizeof(double));
    memset(a2, 0, k * sizeof(double));
    memset(b0, 0, k * sizeof(double));
    memset(b1, 0, k * sizeof(double));
    memset(b2, 0, k * sizeof(double));
    
    // Copy coefficients to split polynomials
    for (int i = 0; i < k && i < tLen; i++) a0[i] = a[i];
    for (int i = 0; i < k && i + k < tLen; i++) a1[i] = a[i + k];
    for (int i = 0; i < k && i + 2*k < tLen; i++) a2[i] = a[i + 2*k];
    for (int i = 0; i < k && i < tLen; i++) b0[i] = b[i];
    for (int i = 0; i < k && i + k < tLen; i++) b1[i] = b[i + k];
    for (int i = 0; i < k && i + 2*k < tLen; i++) b2[i] = b[i + 2*k];
    
    // Evaluate at 5 points
    for (int i = 0; i < k; i++) {
        // u(0) = a0, v(0) = b0
        u0[i] = a0[i];
        v0[i] = b0[i];
        
        // u(1) = a0 + a1 + a2, v(1) = b0 + b1 + b2
        u1[i] = a0[i] + a1[i] + a2[i];
        v1[i] = b0[i] + b1[i] + b2[i];
        
        // u(-1) = a0 - a1 + a2, v(-1) = b0 - b1 + b2
        u_1[i] = a0[i] - a1[i] + a2[i];
        v_1[i] = b0[i] - b1[i] + b2[i];
        
        // u(-2) = a0 - 2*a1 + 4*a2, v(-2) = b0 - 2*b1 + 4*b2
        u_2[i] = a0[i] - 2.0*a1[i] + 4.0*a2[i];
        v_2[i] = b0[i] - 2.0*b1[i] + 4.0*b2[i];
        
        // u(inf) = a2, v(inf) = b2
        u_inf[i] = a2[i];
        v_inf[i] = b2[i];
    }
    
    // Multiply at evaluation points
    int st;
    st = tom_rec(u0, v0, k, w0, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom_rec(u1, v1, k, w1, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom_rec(u_1, v_1, k, w_1, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom_rec(u_2, v_2, k, w_2, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom_rec(u_inf, v_inf, k, w_inf, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    
    // Interpolate using standard Toom-3 formulas
    for (int i = 0; i < wLen; i++) {
        double r0 = w0[i];
        double r1 = w1[i];
        double r_1 = w_1[i];
        double r_2 = w_2[i];
        double r_inf = w_inf[i];
        
        // Standard Toom-3 interpolation
        double c0 = r0;
        double c4 = r_inf;

        // c2 = (w(1)+w(-1))/2 - c0 - c4
        double c2 = 0.5 * (r1 + r_1) - c0 - c4;

        // d1 = (w(1)-w(-1))/2 = c1 + c3
        double d1 = 0.5 * (r1 - r_1);

        // t = w(-2) - c0 - 4*c2 - 16*c4 = -2*c1 - 8*c3
        double t = r_2 - c0 - 4.0 * c2 - 16.0 * c4;

        // resolve:
        // c3 = (-t - 2*d1)/6
        double c3 = (-t - 2.0 * d1) / 6.0;

        // c1 = d1 - c3
        double c1 = d1 - c3;

        // Add to result with proper shifts and bounds checking
        int idx;
        idx = i;
        if (idx >= 0 && idx < outLen) z[idx] += c0;
        idx = i + k;
        if (idx >= 0 && idx < outLen) z[idx] += c1;
        idx = i + 2*k;
        if (idx >= 0 && idx < outLen) z[idx] += c2;
        idx = i + 3*k;
        if (idx >= 0 && idx < outLen) z[idx] += c3;
        idx = i + 4*k;
        if (idx >= 0 && idx < outLen) z[idx] += c4;
    }
    
    arena_restore(arena, mark);
    return STATUS_OK;
}

int tom(double *a, double *b, int tLen, double *z) {
    const int cutoff = TOOM3_CUTOFF;
    size_t ws_doubles = tom_ws_needed(tLen, cutoff);
    double *ws = (ws_doubles > 0) ? malloc(ws_doubles * sizeof(double)) : NULL;
    
    Arena arena = { ws, ws_doubles, 0 };
    int st = tom_rec(a, b, tLen, z, cutoff, &arena);
    
    free(ws);
    return st;
}