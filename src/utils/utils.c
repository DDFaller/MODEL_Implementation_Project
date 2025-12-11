
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "typedefs.h"


#define TOL_ERROR 1e-6
/* 
 * Generate a polynomial of degree n with random coefficients in [-1, 1].
 * Returns a pointer to the coefficients array (size n+1) or NULL on error.
 * coeffs[i] corresponds to the coefficient of x^i (or however you want).
 */
int generate_random_polynomial(int n, double *polynomial, int *degree) {
    if (n <= 0 || polynomial == NULL) {
        return STATUS_NULL_POINTER;
    }

    for (int i = 0; i < n; i++) {
        // rand() / RAND_MAX -> [0, 1]
        double r = (double)rand() / (double)RAND_MAX;
        polynomial[i] = -1.0 + 2.0 * r;   // -> [-1, 1]
    }

    if (degree != NULL) {
        *degree = n - 1;
    }

    return STATUS_OK;
}


int calculate_error(int poly_size, const double *A, const double *B, double *param_sum_error) {
    double sum_error = 0.0;

    for (int i = 0; i < poly_size; i++) {
        double curr_error = fabs(A[i] - B[i]);
        sum_error += curr_error;

        if (curr_error >= TOL_ERROR) { 
            printf("Potential problem at index %d: error = %f\n", i, curr_error);
            return STATUS_COMPUTATION_ERROR;
        }
    }

    printf("Accumulated error: %f\n", sum_error);
    *param_sum_error = sum_error;
    return STATUS_OK;
}

void print_polynomial(int n, double *coeffs){
    for (int i = 0; i < n; i++) {
        printf("coeff[%d] = %f\n", i, coeffs[i]);
    }
}
