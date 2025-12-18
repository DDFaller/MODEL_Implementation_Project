#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "src/utils/typedefs.h"
#include "src/utils/utils.h"
#include "src/utils/timer.h"
#include "src/utils/timer_log.h"
#include "src/multiplication_methods.h"
#include "src/tests/test_framework.h"

#define DEBUG 1
#define SAMPLE_SIZE 50

int main(void) {
    // Run unit tests first
    printf("=== POLYNOMIAL MULTIPLICATION TESTING ===\n");
    if (!run_all_tests()) {
        printf("Unit tests failed! Aborting performance measurements.\n");
        return STATUS_COMPUTATION_ERROR;
    }

    printf("=== PERFORMANCE MEASUREMENTS ===\n");
    srand((unsigned int)time(NULL));

    if (timer_log_init("csvs/timings.csv") != 0) {
        fprintf(stderr, "Warning: could not open timings.csv for writing. Timing data will not be saved.\n");
    }

    // Karatsuba vs all {2, 4, 8, 12, 16, 24, 32} //16 - 64
    // Toom3 vs all {48, 64, 81, 96, 128, 162, 192, 243, 256}// 128 - 256
    // Toom4 vs all {256, 300, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 4448, 5120}// 2048
    int size_of_polynomials[] = {2};
    int Ks[] = {4,8,16,32,64,96,128,256,512};
    int amount_of_sizes = (int)(sizeof(size_of_polynomials) / sizeof(size_of_polynomials[0]));

    for (int i = 0; i < amount_of_sizes; i++) {
        const int n = size_of_polynomials[i];

        for (int sample = 0; sample < SAMPLE_SIZE; sample++) {

            double *coeffs            = (double*)malloc((size_t)n * sizeof(double));
            double *coeffs2           = (double*)malloc((size_t)n * sizeof(double));
            double *naive_product     = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));
            double *karatsuba_product = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));
            double *tom_product       = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));
            double *tom4_product      = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));

            double naive_timer = 0.0;
            double karatsuba_timer = 0.0;
            double tom_timer = 0.0;
            double tom4_timer = 0.0;

            if (!coeffs || !coeffs2 || !naive_product || !karatsuba_product || !tom_product || !tom4_product) {
                fprintf(stderr, "Error: could not allocate polynomial buffers (n = %d).\n", n);
                free(coeffs);
                free(coeffs2);
                free(naive_product);
                free(karatsuba_product);
                free(tom_product);
                free(tom4_product);
                timer_log_close();
                return STATUS_COMPUTATION_ERROR;
            }

            int degree;
            generate_random_polynomial(n, coeffs, &degree);
            generate_random_polynomial(n, coeffs2, &degree);

            if (DEBUG && n <= 16) {
                printf("\n=== n = %d ===\n", n);
                printf("--- Polynomial 1 ---\n");
                print_polynomial(n, coeffs);
                printf("--- Polynomial 2 ---\n");
                print_polynomial(n, coeffs2);
            }

            int status;

            // ==== NAIVE ====
            memset(naive_product, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            status = naive(n, coeffs, n, coeffs2, naive_product);
            end_timer();
            naive_timer = save_timer();
            timer_log_write("naive", n, naive_timer);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error processing naive multiplication (status = %d)\n", status);
            }

            // ==== KARATSUBA ====
            memset(karatsuba_product, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            status = karatsuba(n, coeffs, n, coeffs2, karatsuba_product);
            end_timer();
            karatsuba_timer = save_timer();
            timer_log_write("karatsuba", n, karatsuba_timer);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error processing karatsuba multiplication (status = %d)\n", status);
            }

            // ==== TOOM-COOK 3 ====
            memset(tom_product, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            {
                int max_size = n;
                double *a = (double*)calloc((size_t)max_size, sizeof(double));
                double *b = (double*)calloc((size_t)max_size, sizeof(double));
                double *result = (double*)calloc((size_t)(2 * max_size), sizeof(double)); // safe length

                if (!a || !b || !result) {
                    fprintf(stderr, "Error: could not allocate Toom3 buffers (n=%d).\n", n);
                    free(a); free(b); free(result);
                    free(coeffs); free(coeffs2);
                    free(naive_product); free(karatsuba_product); free(tom_product); free(tom4_product);
                    timer_log_close();
                    return STATUS_COMPUTATION_ERROR;
                }

                for (int j = 0; j < n; j++) {
                    a[j] = coeffs[j];
                    b[j] = coeffs2[j];
                }

                status = tom(a, b, max_size, result);

                for (int j = 0; j < 2 * n - 1; j++) {
                    tom_product[j] = result[j];
                }

                free(a);
                free(b);
                free(result);
            }
            end_timer();
            tom_timer = save_timer();
            timer_log_write("tom3", n, tom_timer);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error processing tom3 multiplication (status = %d)\n", status);
            }

            // ==== TOOM-COOK 4 ====
            memset(tom4_product, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            {
                int max_size4 = n;
                double *a4 = (double*)calloc((size_t)max_size4, sizeof(double));
                double *b4 = (double*)calloc((size_t)max_size4, sizeof(double));
                double *result4 = (double*)calloc((size_t)(2 * max_size4), sizeof(double)); // safe length

                if (!a4 || !b4 || !result4) {
                    fprintf(stderr, "Error: could not allocate Toom4 buffers (n=%d).\n", n);
                    free(a4); free(b4); free(result4);
                    free(coeffs); free(coeffs2);
                    free(naive_product); free(karatsuba_product); free(tom_product); free(tom4_product);
                    timer_log_close();
                    return STATUS_COMPUTATION_ERROR;
                }

                for (int j = 0; j < n; j++) {
                    a4[j] = coeffs[j];
                    b4[j] = coeffs2[j];
                }

                status = tom4(a4, b4, max_size4, result4);

                for (int j = 0; j < 2 * n - 1; j++) {
                    tom4_product[j] = result4[j];
                }

                free(a4);
                free(b4);
                free(result4);
            }
            end_timer();
            tom4_timer = save_timer();
            timer_log_write("tom4", n, tom4_timer);
            if (status != STATUS_OK) {
                fprintf(stderr, "Error processing tom4 multiplication (status = %d)\n", status);
            }

            // ==== ERROR COMPARISON ====
            if (n <= 64) {
                double error_sum = 0.0;

                printf("--- Comparing Naive vs Karatsuba (n=%d) ---\n", n);
                status = calculate_error(2 * n - 1, naive_product, karatsuba_product, &error_sum);
                if (status != STATUS_OK) printf("WARNING: Significant error detected between naive and karatsuba results!\n");
                else printf("Karatsuba results match within tolerance.\n");

                printf("--- Comparing Naive vs Toom-Cook 3 (n=%d) ---\n", n);
                status = calculate_error(2 * n - 1, naive_product, tom_product, &error_sum);
                if (status != STATUS_OK) printf("WARNING: Significant error detected between naive and tom3 results!\n");
                else printf("Toom-Cook 3 results match within tolerance.\n");

                printf("--- Comparing Naive vs Toom-Cook 4 (n=%d) ---\n", n);
                status = calculate_error(2 * n - 1, naive_product, tom4_product, &error_sum);
                if (status != STATUS_OK) printf("WARNING: Significant error detected between naive and tom4 results!\n");
                else printf("Toom-Cook 4 results match within tolerance.\n");
            }

            if (DEBUG && n <= 16) {
                printf("Naive time: %f s\n", naive_timer);
                printf("Karatsuba time: %f s\n", karatsuba_timer);
                printf("Toom-Cook 3 time: %f s\n", tom_timer);
                printf("Toom-Cook 4 time: %f s\n", tom4_timer);
            } else {
                printf("n=%d: Naive=%.6fs, Karatsuba=%.6fs, Tom3=%.6fs, Tom4=%.6fs\n",
                       n, naive_timer, karatsuba_timer, tom_timer, tom4_timer);
            }

            free(coeffs);
            free(coeffs2);
            free(naive_product);
            free(karatsuba_product);
            free(tom_product);
            free(tom4_product);
        }
    }


    if (timer_log_init("csvs/timings_k.csv") != 0) {
        fprintf(stderr, "Warning: could not open timings.csv for writing. Timing data will not be saved.\n");
    }

    for (int k_index = 0; k_index < (int)(sizeof(Ks) / sizeof(Ks[0])); k_index++) {
        int current_k = Ks[k_index];
        KARATSUBA_CUTOFF = current_k;
        TOOM3_CUTOFF = current_k;
        TOOM4_CUTOFF = current_k;
        for (int sample = 0; sample < SAMPLE_SIZE; sample++) {

            int n = 4096 + k_index+sample;

            double *P1 = (double*)malloc((size_t)n * sizeof(double));
            double *P2 = (double*)malloc((size_t)n * sizeof(double));
            double *karatsuba_res = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));
            double *toom3_res = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));
            double *toom4_res = (double*)malloc((size_t)(2 * n - 1) * sizeof(double));

            double karatsuba_timer = 0.0;
            double toom3_timer = 0.0;
            double toom4_timer = 0.0;

            if (!P1 || !P2) {
                fprintf(stderr, "Error: could not allocate Karatsuba buffers (n = %d).\n", n);
                free(P1);
                free(P2);
                free(karatsuba_res);
                free(toom3_res);
                free(toom4_res);
                timer_log_close();
                return STATUS_COMPUTATION_ERROR;
            }

            int degree;
            generate_random_polynomial(n, P1, &degree);
            generate_random_polynomial(n, P2, &degree);

            // ==== KARATSUBA ====
            memset(karatsuba_res, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            karatsuba(P1, P2, n, n, karatsuba_res);
            end_timer();
            karatsuba_timer = save_timer();
            timer_log_write_cutoff("karatsuba_k", n, karatsuba_timer, current_k);

            printf("k=%d (n=%d): Karatsuba=%.6fs\n", current_k, n, karatsuba_timer);

            // ==== TOOM-COOK 3 ====
            memset(toom3_res, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            tom(P1, P2, n, toom3_res);    
            end_timer();
            toom3_timer = save_timer();
            timer_log_write_cutoff("toom3_k", n, toom3_timer, current_k);

            printf("k=%d (n=%d): Toom3=%.6fs\n", current_k, n, toom3_timer);

            // ==== TOOM-COOK 4 ====
            memset(toom4_res, 0, (size_t)(2 * n - 1) * sizeof(double));
            start_timer();
            tom(P1, P2, n, toom4_res);    
            end_timer();
            toom4_timer = save_timer();
            timer_log_write_cutoff("toom4_k", n, toom4_timer, current_k);

            printf("k=%d (n=%d): Toom4=%.6fs\n", current_k, n, toom4_timer);




            free(P1);
            free(P2);
            free(karatsuba_res);
            free(toom3_res);
            free(toom4_res);
        }
    }

    timer_log_close();

    return STATUS_OK;
}
