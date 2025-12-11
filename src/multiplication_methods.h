#ifndef TIMER_H
#define MULTIPLICATIONS_METHODS_H
#include <stdio.h>
#include <stdlib.h>
#include "utils/typedefs.h"

/*
 * Naive polynomial multiplication.
 * A has size n1, B has size n2, result has size (n1 + n2 - 1).
 */
int naive(int n1, double *poly1,
        int n2, double *poly2,
        double *product);

int karatsuba(int n1, const double *poly1,
                int n2, const double *poly2,
                double *product);

void Karatsuba(double * P1, double * P2, double * res, int d1, int d2, int k);
 


        /*
 * You can declare karatsuba and tom here too if you want:
 *

 * int tom(int n1, const double *poly1,
 *              int n2, const double *poly2,
 *              double *product);
 */


#endif /* TIMER_H */
