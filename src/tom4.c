#include "multiplication_methods.h"
#include "utils/arena_allocator.h"
#include <stdlib.h>
#include <string.h>

static size_t tom4_ws_needed(int tLen, int cutoff) {
    if (tLen <= cutoff) return 0;
    int k = (tLen + 3) / 4;
    int wLen = 2 * k - 1;
    size_t local = (size_t)(8 * k + 14 * k + 14 * wLen + 7 * wLen);
    size_t child = tom4_ws_needed(k, cutoff);
    return local + 7 * child;
}

static int tom4_rec(double *a, double *b, int tLen, double *z, int cutoff, Arena *arena) {
    if (!a || !b || !z || tLen <= 0) return -1;

    if (tLen <= cutoff) {
        return tom(a, b, tLen, z);
    }

    const int outLen = 2 * tLen - 1;
    memset(z, 0, (size_t)outLen * sizeof(double));

    const int k = (tLen + 3) / 4;
    const int wLen = 2 * k - 1;
    
    size_t mark = arena_mark(arena);

    // Allocate from arena
    double *a0 = arena_alloc(arena, k);
    double *a1 = arena_alloc(arena, k);
    double *a2 = arena_alloc(arena, k);
    double *a3 = arena_alloc(arena, k);

    double *b0 = arena_alloc(arena, k);
    double *b1 = arena_alloc(arena, k);
    double *b2 = arena_alloc(arena, k);
    double *b3 = arena_alloc(arena, k);

    double *A0 = arena_alloc(arena, k);
    double *A1 = arena_alloc(arena, k);
    double *Am1 = arena_alloc(arena, k);
    double *A2 = arena_alloc(arena, k);
    double *Am2 = arena_alloc(arena, k);
    double *Ah = arena_alloc(arena, k);
    double *Ainf = arena_alloc(arena, k);

    double *B0 = arena_alloc(arena, k);
    double *B1 = arena_alloc(arena, k);
    double *Bm1 = arena_alloc(arena, k);
    double *B2 = arena_alloc(arena, k);
    double *Bm2 = arena_alloc(arena, k);
    double *Bh = arena_alloc(arena, k);
    double *Binf = arena_alloc(arena, k);

    double *W0 = arena_alloc(arena, wLen);
    double *W1 = arena_alloc(arena, wLen);
    double *Wm1 = arena_alloc(arena, wLen);
    double *W2 = arena_alloc(arena, wLen);
    double *Wm2 = arena_alloc(arena, wLen);
    double *Wh = arena_alloc(arena, wLen);
    double *Winf = arena_alloc(arena, wLen);

    double *C0 = arena_alloc(arena, wLen);
    double *C1 = arena_alloc(arena, wLen);
    double *C2 = arena_alloc(arena, wLen);
    double *C3 = arena_alloc(arena, wLen);
    double *C4 = arena_alloc(arena, wLen);
    double *C5 = arena_alloc(arena, wLen);
    double *C6 = arena_alloc(arena, wLen);

    if (!a0 || !a1 || !a2 || !a3 || !b0 || !b1 || !b2 || !b3 ||
        !A0 || !A1 || !Am1 || !A2 || !Am2 || !Ah || !Ainf ||
        !B0 || !B1 || !Bm1 || !B2 || !Bm2 || !Bh || !Binf ||
        !W0 || !W1 || !Wm1 || !W2 || !Wm2 || !Wh || !Winf ||
        !C0 || !C1 || !C2 || !C3 || !C4 || !C5 || !C6) {
        arena_restore(arena, mark);
        return -1;
    }

    // Initialize arrays
    memset(a0, 0, k * sizeof(double));
    memset(a1, 0, k * sizeof(double));
    memset(a2, 0, k * sizeof(double));
    memset(a3, 0, k * sizeof(double));
    memset(b0, 0, k * sizeof(double));
    memset(b1, 0, k * sizeof(double));
    memset(b2, 0, k * sizeof(double));
    memset(b3, 0, k * sizeof(double));

    // Copy with bounds
    for (int i = 0; i < k; i++) {
        int idx;
        idx = i;
        if (idx < tLen) a0[i] = a[idx];
        idx = i + k;
        if (idx < tLen) a1[i] = a[idx];
        idx = i + 2*k;
        if (idx < tLen) a2[i] = a[idx];
        idx = i + 3*k;
        if (idx < tLen) a3[i] = a[idx];

        idx = i;
        if (idx < tLen) b0[i] = b[idx];
        idx = i + k;
        if (idx < tLen) b1[i] = b[idx];
        idx = i + 2*k;
        if (idx < tLen) b2[i] = b[idx];
        idx = i + 3*k;
        if (idx < tLen) b3[i] = b[idx];
    }

    // Evaluate A(t) and B(t)
    for (int i = 0; i < k; i++) {
        const double x0 = a0[i], x1 = a1[i], x2 = a2[i], x3 = a3[i];
        const double y0 = b0[i], y1 = b1[i], y2 = b2[i], y3 = b3[i];

        A0[i] = x0;                 B0[i] = y0;
        A1[i] = x0 + x1 + x2 + x3;  B1[i] = y0 + y1 + y2 + y3;
        Am1[i] = x0 - x1 + x2 - x3; Bm1[i] = y0 - y1 + y2 - y3;
        A2[i] = x0 + 2*x1 + 4*x2 + 8*x3;  B2[i] = y0 + 2*y1 + 4*y2 + 8*y3;
        Am2[i] = x0 - 2*x1 + 4*x2 - 8*x3; Bm2[i] = y0 - 2*y1 + 4*y2 - 8*y3;
        Ah[i] = x0 + 0.5*x1 + 0.25*x2 + 0.125*x3;
        Bh[i] = y0 + 0.5*y1 + 0.25*y2 + 0.125*y3;
        Ainf[i] = x3;               Binf[i] = y3;
    }

    int st = STATUS_OK;
    st = tom4_rec(A0, B0, k, W0, cutoff, arena);    if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(A1, B1, k, W1, cutoff, arena);    if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(Am1, Bm1, k, Wm1, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(A2, B2, k, W2, cutoff, arena);    if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(Am2, Bm2, k, Wm2, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(Ah, Bh, k, Wh, cutoff, arena);    if (st != STATUS_OK) { arena_restore(arena, mark); return st; }
    st = tom4_rec(Ainf, Binf, k, Winf, cutoff, arena); if (st != STATUS_OK) { arena_restore(arena, mark); return st; }

    // Interpolation matrix
    static const double Vinv[7][7] = {
        {   1.0,    0.0,     0.0,      0.0,      0.0,       0.0,   0.0 },
        {  -2.0, -2.0/3.0, -2.0/9.0,  1.0/36.0, 1.0/60.0, 128.0/45.0, -2.0 },
        { -5.0/4.0, 2.0/3.0, 2.0/3.0, -1.0/24.0, -1.0/24.0, 0.0,  4.0 },
        {  5.0/2.0, 3.0/2.0, -7.0/18.0, -1.0/18.0, 0.0, -32.0/9.0, 5.0/2.0 },
        {  1.0/4.0, -1.0/6.0, -1.0/6.0, 1.0/24.0, 1.0/24.0, 0.0, -5.0 },
        { -1.0/2.0, -1.0/3.0, 1.0/9.0, 1.0/36.0, -1.0/60.0, 32.0/45.0, -1.0/2.0 },
        {   0.0,    0.0,     0.0,      0.0,      0.0,       0.0,   1.0 }
    };

    // Interpolate
    for (int i = 0; i < wLen; i++) {
        const double wv[7] = { W0[i], W1[i], Wm1[i], W2[i], Wm2[i], Wh[i], Winf[i] };
        double *Cv[7] = { C0, C1, C2, C3, C4, C5, C6 };
        for (int r = 0; r < 7; r++) {
            double s = 0.0;
            for (int c = 0; c < 7; c++) s += Vinv[r][c] * wv[c];
            Cv[r][i] = s;
        }
    }

    // Recompose
    double *Cv[7] = { C0, C1, C2, C3, C4, C5, C6 };
    for (int j = 0; j < 7; j++) {
        const int shift = j * k;
        for (int i = 0; i < wLen; i++) {
            const int idx = i + shift;
            if (idx >= 0 && idx < outLen) z[idx] += Cv[j][i];
        }
    }

    arena_restore(arena, mark);
    return st;
}

int tom4(double *a, double *b, int tLen, double *z) {
    const int cutoff = TOOM4_CUTOFF;
    size_t ws_doubles = tom4_ws_needed(tLen, cutoff);
    double *ws = (ws_doubles > 0) ? malloc(ws_doubles * sizeof(double)) : NULL;
    
    Arena arena = { ws, ws_doubles, 0 };
    int st = tom4_rec(a, b, tLen, z, cutoff, &arena);
    
    free(ws);
    return st;
}