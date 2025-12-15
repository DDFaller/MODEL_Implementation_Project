#include "multiplication_methods.h"
#include <stdlib.h>
#include <string.h>

int tom(double *a, double *b, int tLen, double *z) {
    if (tLen <= 9) {
        return naive(tLen, a, tLen, b, z);
    }
    
    int third = tLen / 3;
    
    // Split polynomials into 3 parts
    double *a0 = &a[0];
    double *a1 = &a[third];
    double *a2 = &a[third * 2];
    double *b0 = &b[0];
    double *b1 = &b[third];
    double *b2 = &b[third * 2];
    
    // Result coefficients
    double *c0 = &z[third * 0];
    double *c2 = &z[third * 2];
    double *c4 = &z[third * 4];
    
    // Allocate temporary arrays
    double *c1 = calloc(third * 2, sizeof(double));
    double *c3 = calloc(third * 2, sizeof(double));
    
    double *a_m2 = calloc(third, sizeof(double));
    double *a_m1 = calloc(third, sizeof(double));
    double *a_0 = calloc(third, sizeof(double));
    double *a_1 = calloc(third, sizeof(double));
    double *a_inf = calloc(third, sizeof(double));
    
    double *b_m2 = calloc(third, sizeof(double));
    double *b_m1 = calloc(third, sizeof(double));
    double *b_0 = calloc(third, sizeof(double));
    double *b_1 = calloc(third, sizeof(double));
    double *b_inf = calloc(third, sizeof(double));
    
    double *c_m2 = calloc(third * 2, sizeof(double));
    double *c_m1 = calloc(third * 2, sizeof(double));
    double *c_0 = calloc(third * 2, sizeof(double));
    double *c_1 = calloc(third * 2, sizeof(double));
    double *c_inf = calloc(third * 2, sizeof(double));
    
    // ==== a(-2) = 4 * a2 - 2 * a1 + a0, b(-2) = 4 * b2 - 2 * b1 + b0
    for(int i = 0; i < third; i++) {
        a_m2[i] = 4.0 * a2[i] - 2.0 * a1[i] + a0[i];
        b_m2[i] = 4.0 * b2[i] - 2.0 * b1[i] + b0[i];
    }
    
    // ==== a(-1) = a2 - a1 + a0, b(-1) = b2 - b1 + b0
    for(int i = 0; i < third; i++) {
        a_m1[i] = a2[i] - a1[i] + a0[i];
        b_m1[i] = b2[i] - b1[i] + b0[i];
    }
    
    // ==== a(0) = a0, b(0) = b0
    for(int i = 0; i < third; i++) {
        a_0[i] = a0[i];
        b_0[i] = b0[i];
    }
    
    // ==== a(1) = a2 + a1 + a0, b(1) = b2 + b1 + b0
    for(int i = 0; i < third; i++) {
        a_1[i] = a2[i] + a1[i] + a0[i];
        b_1[i] = b2[i] + b1[i] + b0[i];
    }
    
    // ==== a(inf) = a2, b(inf) = b2
    for(int i = 0; i < third; i++) {
        a_inf[i] = a2[i];
        b_inf[i] = b2[i];
    }
    
    // ==== c(-2) = a(-2) * b(-2)
    tom(a_m2, b_m2, third, c_m2);
    // ==== c(-1) = a(-1) * b(-1)
    tom(a_m1, b_m1, third, c_m1);
    // ==== c(0) = a(0) * b(0)
    tom(a_0, b_0, third, c_0);
    // ==== c(1) = a(1) * b(1)
    tom(a_1, b_1, third, c_1);
    // ==== c(inf) = a(inf) * b(inf)
    tom(a_inf, b_inf, third, c_inf);
    
    // ==== c4 = c(inf)
    for(int i = 0; i < third * 2; i++)
        c4[i] = c_inf[i];
    
    // ==== c3 = -c(-2) + 3 * c(-1) - 3 * c(0) + c(1) + 12 * c(inf) / 6
    for(int i = 0; i < third * 2; i++) {
        c3[i] = -c_m2[i];
        c3[i] += 3.0 * c_m1[i];
        c3[i] -= 3.0 * c_0[i];
        c3[i] += c_1[i];
        c3[i] += 12.0 * c_inf[i];
        c3[i] /= 6.0;
    }
    
    // ==== c2 = 3 * c(-1) - 6 * c(0) + 3 * c(1) - 6 * c(inf) / 6
    for(int i = 0; i < third * 2; i++) {
        c2[i] = 3.0 * c_m1[i];
        c2[i] -= 6.0 * c_0[i];
        c2[i] += 3.0 * c_1[i];
        c2[i] -= 6.0 * c_inf[i];
        c2[i] /= 6.0;
    }
    
    // ==== c1 = c(-2) - 6 * c(-1) + 3 * c(0) + 2 * c(1) - 12 * c(inf) / 6
    for(int i = 0; i < third * 2; i++) {
        c1[i] = c_m2[i];
        c1[i] -= 6.0 * c_m1[i];
        c1[i] += 3.0 * c_0[i];
        c1[i] += 2.0 * c_1[i];
        c1[i] -= 12.0 * c_inf[i];
        c1[i] /= 6.0;
    }
    
    // ==== c0 = c(0)
    for(int i = 0; i < third * 2; i++)
        c0[i] = c_0[i];
    
    // ==== z = c4 * x^4 + c3 * x^3 + c2 * x^2 + c1 * x + c0
    for(int i = 0; i < third * 2; i++) z[i + third] += c1[i];
    for(int i = 0; i < third * 2; i++) z[i + third * 3] += c3[i];
    
    // Free memory
    free(c1); free(c3);
    free(a_m2); free(a_m1); free(a_0); free(a_1); free(a_inf);
    free(b_m2); free(b_m1); free(b_0); free(b_1); free(b_inf);
    free(c_m2); free(c_m1); free(c_0); free(c_1); free(c_inf);
    
    return STATUS_OK;
}