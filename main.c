#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "src/typedefs.h"
#include "src/utils.h"
#include "src/timer.h"
#include "src/timer_log.h"
#include "src/multiplication_methods.h"

#define DEBUG 1

int main(void) {
    // Seed RNG once at the start of the program
    srand((unsigned int)time(NULL));

    // Initialize CSV log file for timings
    if (timer_log_init("timings.csv") != 0) {
        fprintf(stderr, "Warning: could not open timings.csv for writing. Timing data will not be saved.\n");
    }

    // Different polynomial sizes to test
    int size_of_polynomials[] = {4};
    int amount_of_sizes = sizeof(size_of_polynomials) / sizeof(size_of_polynomials[0]);

    for (int i = 0; i < amount_of_sizes; i++) {
        int n = size_of_polynomials[i];   // number of coefficients in each polynomial

        // Allocate memory for the polynomials and their product
        double *coeffs         = (double*) malloc(n * sizeof(double));
        double *coeffs2        = (double*) malloc(n * sizeof(double));
        double *coeffs_product = (double*) malloc((2 * n - 1) * sizeof(double));

        double naive_timer    = 0.0;
        double karatsuba_timer = 0.0;
        double tom_timer       = 0.0;

        if (!coeffs || !coeffs2 || !coeffs_product) {
            fprintf(stderr, "Error: could not allocate polynomial coefficients (n = %d).\n", n);
            free(coeffs);
            free(coeffs2);
            free(coeffs_product);
            timer_log_close();
            return STATUS_COMPUTATION_ERROR;
        }

        int degree; // degree of the polynomial (not strictly needed here, but kept for clarity)

        // Generate random polynomials
        generate_random_polynomial(n, coeffs,  &degree);
        generate_random_polynomial(n, coeffs2, &degree);

        if (DEBUG) {
            printf("\n=== n = %d ===\n", n);
            printf("--- Polynomial 1 ---\n");
            print_polynomial(n, coeffs);
            printf("--- Polynomial 2 ---\n");
            print_polynomial(n, coeffs2);
        }

        int status;
        // ==== NAIVE ====
        start_timer();
        status = naive(n, coeffs, n, coeffs2, coeffs_product);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing naive multiplication (status = %d)\n", status);
        }
        end_timer();
        naive_timer = save_timer();  // elapsed time in seconds

        // Log result to CSV: function_name, poly_size, time_seconds
        timer_log_write("naive", n, naive_timer);

        // ==== KARATSUBA ====
        /*
        start_timer();
        status = karatsuba(n, coeffs, n, coeffs2, coeffs_product);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing karatsuba multiplication (status = %d)\n", status);
        }
        end_timer();
        karatsuba_timer = save_timer();
        timer_log_write("karatsuba", n, karatsuba_timer);
        */

        // ==== TOOM-COOK ====
        /*
        start_timer();
        status = tom(n, coeffs, n, coeffs2, coeffs_product);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing tom multiplication (status = %d)\n", status);
        }
        end_timer();
        tom_timer = save_timer();
        timer_log_write("tom", n, tom_timer);
        */

        if (DEBUG) {
            printf("--- Product (naive) ---\n");
            print_polynomial(2 * n - 1, coeffs_product);
            printf("Naive time: %f s\n", naive_timer);
            // printf("Karatsuba time: %f s\n", karatsuba_timer);
            // printf("Tom time: %f s\n", tom_timer);
        }

        // Free memory for this polynomial size
        free(coeffs);
        free(coeffs2);
        free(coeffs_product);
    }

    // Close CSV log file
    timer_log_close();

    return STATUS_OK;
}
