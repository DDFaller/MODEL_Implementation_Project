/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <stddef.h>



/* 
 * Generate a polynomial of degree n with random coefficients in [-1, 1].
 * 
 * Parameters:
 *   n         - degree of the polynomial (must be >= 0)
 *   out_degree - if not NULL, *out_degree is set to n
 *
 * Returns:
 *   Pointer to an array of (n + 1) floats (coeffs[0]..coeffs[n])
 *   or NULL if allocation fails or n < 0.
 *
 * Note:
 *   The caller is responsible for calling free() on the returned pointer.
 */
int generate_random_polynomial(int n, double * polynomial,int *out_degree);

int calculate_error(int poly_size, const double *A, const double *B, double *param_sum_error);

void print_polynomial(int n, double *coeffs);

#endif /* POLYNOMIAL_H */

