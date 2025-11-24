#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <stddef.h>

#define STATUS_OK 0
#define STATUS_NULL_POINTER 1
#define STATUS_COMPUTATION_ERROR 2

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
int generate_random_polynomial(int n, float * polynomial,int *out_degree);

#endif /* POLYNOMIAL_H */
