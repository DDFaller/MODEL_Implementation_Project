#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "src/utils.h"


int main(void) {
    // Seed RNG once at the start of the program
    srand((unsigned int)time(NULL));

    int degree;
    int n = 4;  // degree of the polynomial
    float *coeffs;
    generate_random_polynomial(n,coeffs, &degree);
    if (!coeffs) {
        fprintf(stderr, "Error: could not allocate polynomial coefficients.\n");
        return STATUS_COMPUTATION_ERROR;
    }

    printf("Generated polynomial of degree %d:\n", degree);
    // Here coeffs[i] is the coefficient of x^i (you can choose any convention)
    for (int i = 0; i <= degree; i++) {
        printf("coeff[%d] = %f\n", i, coeffs[i]);
    }

    // Example: evaluate at x = 1.0 just to show usage
    float x = 1.0f;
    float value = 0.0f;
    float x_power = 1.0f;  // x^0

    for (int i = 0; i <= degree; i++) {
        value += coeffs[i] * x_power;
        x_power *= x;
    }

    printf("P(%f) = %f\n", x, value);

    free(coeffs);
    return STATUS_OK;
}
