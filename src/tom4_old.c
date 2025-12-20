/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "multiplication_methods.h"
#include <stdlib.h>
#include <string.h>


// Toom-Cook 4-way using points: 0, 1, -1, 2, -2, 1/2, inf
// Order of W vector must be: W0, W1, Wm1, W2, Wm2, Wh, Winf

int tom4(double *a, double *b, int tLen, double *z) {
    if (tLen <= 12) {
        return naive(tLen, a, tLen, b, z);
    }

    int quarter = tLen / 4;
    int m = quarter;

    // Split polynomials into 4 parts (views via pointers)
    double *a0 = &a[0];
    double *a1 = &a[m];
    double *a2 = &a[2 * m];
    double *a3 = &a[3 * m];

    double *b0 = &b[0];
    double *b1 = &b[m];
    double *b2 = &b[2 * m];
    double *b3 = &b[3 * m];

    // Result blocks: C(x) = sum_{k=0..6} x^(k*m) * Ck
    double *c0 = &z[0 * m];
    double *c2 = &z[2 * m];
    double *c4 = &z[4 * m];
    double *c6 = &z[6 * m];

    // Odd blocks kept temporary (they overlap when shifted)
    double *c1 = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *c3 = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *c5 = (double*)calloc((size_t)(2 * m), sizeof(double));

    // Evaluation arrays (length m)
    double *a_0   = (double*)calloc((size_t)m, sizeof(double));
    double *a_1   = (double*)calloc((size_t)m, sizeof(double));
    double *a_m1  = (double*)calloc((size_t)m, sizeof(double));
    double *a_2   = (double*)calloc((size_t)m, sizeof(double));
    double *a_m2  = (double*)calloc((size_t)m, sizeof(double));
    double *a_h   = (double*)calloc((size_t)m, sizeof(double)); // 1/2
    double *a_inf = (double*)calloc((size_t)m, sizeof(double)); // inf

    double *b_0   = (double*)calloc((size_t)m, sizeof(double));
    double *b_1   = (double*)calloc((size_t)m, sizeof(double));
    double *b_m1  = (double*)calloc((size_t)m, sizeof(double));
    double *b_2   = (double*)calloc((size_t)m, sizeof(double));
    double *b_m2  = (double*)calloc((size_t)m, sizeof(double));
    double *b_h   = (double*)calloc((size_t)m, sizeof(double));
    double *b_inf = (double*)calloc((size_t)m, sizeof(double));

    // Products at points (length 2m)
    double *w_0   = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_1   = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_m1  = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_2   = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_m2  = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_h   = (double*)calloc((size_t)(2 * m), sizeof(double));
    double *w_inf = (double*)calloc((size_t)(2 * m), sizeof(double));

    // ---- Evaluation: A(t) = a0 + t a1 + t^2 a2 + t^3 a3  (same for B)
    for (int i = 0; i < m; ++i) {
        const double A0 = a0[i], A1 = a1[i], A2 = a2[i], A3 = a3[i];
        const double B0 = b0[i], B1 = b1[i], B2 = b2[i], B3 = b3[i];

        // t = 0
        a_0[i] = A0;
        b_0[i] = B0;

        // t = 1
        a_1[i] = A0 + A1 + A2 + A3;
        b_1[i] = B0 + B1 + B2 + B3;

        // t = -1
        a_m1[i] = A0 - A1 + A2 - A3;
        b_m1[i] = B0 - B1 + B2 - B3;

        // t = 2
        a_2[i] = A0 + 2.0*A1 + 4.0*A2 + 8.0*A3;
        b_2[i] = B0 + 2.0*B1 + 4.0*B2 + 8.0*B3;

        // t = -2
        a_m2[i] = A0 - 2.0*A1 + 4.0*A2 - 8.0*A3;
        b_m2[i] = B0 - 2.0*B1 + 4.0*B2 - 8.0*B3;

        // t = 1/2
        a_h[i] = A0 + 0.5*A1 + 0.25*A2 + 0.125*A3;
        b_h[i] = B0 + 0.5*B1 + 0.25*B2 + 0.125*B3;

        // t = inf -> highest block
        a_inf[i] = A3;
        b_inf[i] = B3;
    }

    // ---- Recursive multiplications at points
    tom4(a_0,   b_0,   m, w_0);
    tom4(a_1,   b_1,   m, w_1);
    tom4(a_m1,  b_m1,  m, w_m1);
    tom4(a_2,   b_2,   m, w_2);
    tom4(a_m2,  b_m2,  m, w_m2);
    tom4(a_h,   b_h,   m, w_h);
    tom4(a_inf, b_inf, m, w_inf);

    // ---- Interpolation via Vinv (hardcoded, from Mathematica)
    // Vinv rows = C0..C6, cols = [W0, W1, W(-1), W2, W(-2), W(1/2), W(inf)]
    static const double Vinv[7][7] = {
        {   1.0,    0.0,     0.0,      0.0,      0.0,       0.0,   0.0 },
        {  -2.0, -2.0/3.0, -2.0/9.0,  1.0/36.0, 1.0/60.0, 128.0/45.0, -2.0 },
        { -5.0/4.0, 2.0/3.0, 2.0/3.0, -1.0/24.0, -1.0/24.0, 0.0,  4.0 },
        {  5.0/2.0, 3.0/2.0, -7.0/18.0, -1.0/18.0, 0.0, -32.0/9.0, 5.0/2.0 },
        {  1.0/4.0, -1.0/6.0, -1.0/6.0, 1.0/24.0, 1.0/24.0, 0.0, -5.0 },
        { -1.0/2.0, -1.0/3.0, 1.0/9.0, 1.0/36.0, -1.0/60.0, 32.0/45.0, -1.0/2.0 },
        {   0.0,    0.0,     0.0,      0.0,      0.0,       0.0,   1.0 }
    };

    // Fill even blocks directly in z (no overlap among even shifts)
    for (int i = 0; i < 2 * m; ++i) {
        const double W0   = w_0[i];
        const double W1   = w_1[i];
        const double Wm1  = w_m1[i];
        const double W2   = w_2[i];
        const double Wm2  = w_m2[i];
        const double Wh   = w_h[i];
        const double Winf = w_inf[i];

        // C0 and C6 are trivial with this Vinv, but we keep the pattern consistent
        c0[i] = Vinv[0][0]*W0 + Vinv[0][1]*W1 + Vinv[0][2]*Wm1 + Vinv[0][3]*W2 + Vinv[0][4]*Wm2 + Vinv[0][5]*Wh + Vinv[0][6]*Winf;
        c2[i] = Vinv[2][0]*W0 + Vinv[2][1]*W1 + Vinv[2][2]*Wm1 + Vinv[2][3]*W2 + Vinv[2][4]*Wm2 + Vinv[2][5]*Wh + Vinv[2][6]*Winf;
        c4[i] = Vinv[4][0]*W0 + Vinv[4][1]*W1 + Vinv[4][2]*Wm1 + Vinv[4][3]*W2 + Vinv[4][4]*Wm2 + Vinv[4][5]*Wh + Vinv[4][6]*Winf;
        c6[i] = Vinv[6][0]*W0 + Vinv[6][1]*W1 + Vinv[6][2]*Wm1 + Vinv[6][3]*W2 + Vinv[6][4]*Wm2 + Vinv[6][5]*Wh + Vinv[6][6]*Winf;

        // Odd blocks -> temporary (will be added with shifts)
        c1[i] = Vinv[1][0]*W0 + Vinv[1][1]*W1 + Vinv[1][2]*Wm1 + Vinv[1][3]*W2 + Vinv[1][4]*Wm2 + Vinv[1][5]*Wh + Vinv[1][6]*Winf;
        c3[i] = Vinv[3][0]*W0 + Vinv[3][1]*W1 + Vinv[3][2]*Wm1 + Vinv[3][3]*W2 + Vinv[3][4]*Wm2 + Vinv[3][5]*Wh + Vinv[3][6]*Winf;
        c5[i] = Vinv[5][0]*W0 + Vinv[5][1]*W1 + Vinv[5][2]*Wm1 + Vinv[5][3]*W2 + Vinv[5][4]*Wm2 + Vinv[5][5]*Wh + Vinv[5][6]*Winf;
    }

    // ---- Recomposition: add odd blocks with shifts (overlaps handled by +=)
    for (int i = 0; i < 2 * m; ++i) z[i + 1 * m] += c1[i];  // + x^m   * C1
    for (int i = 0; i < 2 * m; ++i) z[i + 3 * m] += c3[i];  // + x^(3m)* C3
    for (int i = 0; i < 2 * m; ++i) z[i + 5 * m] += c5[i];  // + x^(5m)* C5

    // Free
    free(c1); free(c3); free(c5);

    free(a_0); free(a_1); free(a_m1); free(a_2); free(a_m2); free(a_h); free(a_inf);
    free(b_0); free(b_1); free(b_m1); free(b_2); free(b_m2); free(b_h); free(b_inf);
    free(w_0); free(w_1); free(w_m1); free(w_2); free(w_m2); free(w_h); free(w_inf);

    return STATUS_OK;
}
