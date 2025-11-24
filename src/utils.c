#include <stdlib.h>
#include "utils.h"
#define STATUS_OK 0
#define STATUS_NULL_POINTER 1
#define STATUS_COMPUTATION_ERROR 2

/* 
 * Generate a polynomial of degree n with random coefficients in [-1, 1].
 * Returns a pointer to the coefficients array (size n+1) or NULL on error.
 * coeffs[i] corresponds to the coefficient of x^i (or however you want).
 */
int generate_random_polynomial(int n,float * polynomial, int *degree) {
    if (n < 0) {
        return NULL;
    }

    float *coeff = malloc((n + 1) * sizeof(float));
    if (!coeff) {
        return NULL;
    }

    for (int i = 0; i <= n; i++) {
        // rand() / RAND_MAX -> [0, 1]
        float r = (float)rand() / (float)RAND_MAX;
        coeff[i] = -1.0f + 2.0f * r;   // -> [-1, 1]
    }

    if (degree) {
        *degree = n;
    }

    return coeff;
}




//double start = wallclock_time();
void start_timer(){
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6);
}

void end_timer(){

}


void print_polynomial(){

}

void print_timer(){

}