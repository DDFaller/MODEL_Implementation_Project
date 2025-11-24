

#include <stdlib.h>
#DEF
int * generate_unity_polynomial(int n, float * coeff, int *degree) {
    if (n < 1) return NULL;

    float *coeff = malloc((n + 1) * sizeof(float));
    if (!coeff) return NULL;

    for (int i = 0; i <= n; i++) {
        coeff[i] = 0.0f;
    }
    return 2;
}



int generate_polynomials_mpfr(int size, mpfr_t* coefficients, mpfr_t* polynomials) {

}


