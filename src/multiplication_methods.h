#ifndef TIMER_H
#define MULTIPLICATIONS_METHODS_H
#include <stdio.h>
#include <stdlib.h>
#include "utils/typedefs.h"

// Cutoff values for each algorithm
extern const int KARATSUBA_CUTOFF;
extern const int TOOM3_CUTOFF;
extern const int TOOM4_CUTOFF;

/*
 * Naive polynomial multiplication.
 * A has size n1, B has size n2, result has size (n1 + n2 - 1).
 */
int naive(int n1, double *poly1,
        int n2, double *poly2,
        double *product);

/*
 * Karatsuba polynomial multiplication.
 * A has size n1, B has size n2, result has size (n1 + n2 - 1) returned in product parameter.
 */
int karatsuba(int n1, const double *poly1,
                int n2, const double *poly2,
                double *product);

void Karatsuba(double * P1, double * P2, int d1, int d2, int k, double * res);

/*
 * Toom-Cook 3 polynomial multiplication.
 * A has size tLen, B has size tLen, result has size (tLen * 2).
 */
int tom(double *a, double *b, int tLen, double *z);

/*
 * Toom-Cook 4 polynomial multiplication.
 * A has size tLen, B has size tLen, result has size (tLen * 2).
 */
int tom4(double *a, double *b, int tLen, double *z);

/*
 * MPFR high-precision naive multiplication
 */
int naive_mpfr(int n1, const char **poly1_str, int n2, const char **poly2_str, char **product_str, int precision);

int naive_mpfr_from_double(int n1, double *poly1_d, int n2, double *poly2_d, double *product_d, int precision);

#endif /* TIMER_H */