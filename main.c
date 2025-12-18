#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "src/utils/typedefs.h"
#include "src/utils/utils.h"
#include "src/utils/timer.h"
#include "src/utils/timer_log.h"
#include "src/multiplication_methods.h"

#define DEBUG 1

int main(void) {
    srand((unsigned int)time(NULL));

    if (timer_log_init("timings.csv") != 0) {
        fprintf(stderr, "Warning: could not open timings.csv for writing. Timing data will not be saved.\n");
    }

    int size_of_polynomials[] = {4, 16, 30, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int amount_of_sizes = sizeof(size_of_polynomials) / sizeof(size_of_polynomials[0]);

    for (int i = 0; i < amount_of_sizes; i++) {
        int n = size_of_polynomials[i]; 

        double *coeffs         = (double*) malloc(n * sizeof(double));
        double *coeffs2        = (double*) malloc(n * sizeof(double));
        double *naive_product  = (double*) calloc((2 * n), sizeof(double));
        double *karatsuba_product = (double*) calloc((2 * n), sizeof(double));
        double *tom_product = (double*) calloc((2 * n), sizeof(double));
        double *tom4_product = (double*) calloc((2 * n), sizeof(double));

        memset(naive_product,     0, 2*n*sizeof(double));
        memset(karatsuba_product, 0, 2*n*sizeof(double));
        memset(tom_product,       0, 2*n*sizeof(double));
        memset(tom4_product,      0, 2*n*sizeof(double));

        double naive_timer = 0.0;
        double karatsuba_timer = 0.0;
        double tom_timer = 0.0;
        double tom4_timer = 0.0;

        if (!coeffs || !coeffs2 || !naive_product || !karatsuba_product || !tom_product) {
            fprintf(stderr, "Error: could not allocate polynomial coefficients (n = %d).\n", n);
            free(coeffs);
            free(coeffs2);
            free(naive_product);
            free(karatsuba_product);
            free(tom_product);
            timer_log_close();
            return STATUS_COMPUTATION_ERROR;
        }

        int degree;
        generate_random_polynomial(n, coeffs,  &degree);
        generate_random_polynomial(n, coeffs2, &degree);

        if (DEBUG && n <= 32) {
            printf("\n=== n = %d ===\n", n);
            printf("--- Polynomial 1 ---\n");
            print_polynomial(n, coeffs);
            printf("--- Polynomial 2 ---\n");
            print_polynomial(n, coeffs2);
        }

        int status;
        // ==== NAIVE ====
        start_timer();
        status = naive(n, coeffs, n, coeffs2, naive_product);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing naive multiplication (status = %d)\n", status);
        }
        end_timer();
        naive_timer = save_timer();
        timer_log_write("naive", n, naive_timer);

        // ==== KARATSUBA ====
        start_timer();
        status = karatsuba(n, coeffs, n, coeffs2, karatsuba_product);
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing karatsuba multiplication (status = %d)\n", status);
        }
        end_timer();
        karatsuba_timer = save_timer();
        timer_log_write("karatsuba", n, karatsuba_timer);

        // ==== TOOM-COOK 3 ====
        start_timer();
        int max_size = n;
        double *a = calloc(max_size, sizeof(double));
        double *b = calloc(max_size, sizeof(double));
        double *result = calloc(max_size * 2, sizeof(double));
        
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
        
        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing tom multiplication (status = %d)\n", status);
        }
        end_timer();
        tom_timer = save_timer();
        timer_log_write("tom3", n, tom_timer);


        // ==== TOOM-COOK 4 ====
        start_timer();
        int max_size4 = n; // deve essere multiplo di 4
        double *a4 = calloc(max_size4, sizeof(double));
        double *b4 = calloc(max_size4, sizeof(double));
        double *result4 = calloc(max_size4 * 2, sizeof(double)); // NOTA: 2*n (non 2*n-1)

        if (!a4 || !b4 || !result4) {
            fprintf(stderr, "Error: could not allocate Toom4 buffers (n=%d).\n", n);
            free(a4); free(b4); free(result4);
            // gestisci come preferisci: return o status error
            timer_log_close();
            return STATUS_COMPUTATION_ERROR;
        }

        for (int j = 0; j < n; j++) {
            a4[j] = coeffs[j];
            b4[j] = coeffs2[j];
        }

        // chiama tom4
        status = tom4(a4, b4, max_size4, result4);

        // copia solo i primi 2*n-1 coefficienti “veri”
        for (int j = 0; j < 2 * n - 1; j++) {
            tom4_product[j] = result4[j];
        }

        free(a4);
        free(b4);
        free(result4);

        if (status != STATUS_OK) {
            fprintf(stderr, "Error processing tom4 multiplication (status = %d)\n", status);
        }
        end_timer();
        tom4_timer = save_timer();
        timer_log_write("tom4", n, tom4_timer);


        if (n == 16) {
            for (int j=0; j<10; j++)
                printf("j=%d naive=% .6f kara=% .6f diff=% .6f\n",
                    j, naive_product[j], karatsuba_product[j],
                    naive_product[j]-karatsuba_product[j]);
        }


        // ==== ERROR COMPARISON ====
        if (n <= 64) {
            double error_sum = 0.0;
            printf("--- Comparing Naive vs Karatsuba (n=%d) ---\n", n);
            status = calculate_error(2 * n - 1, naive_product, karatsuba_product, &error_sum);
            if (status != STATUS_OK) {
                printf("WARNING: Significant error detected between naive and karatsuba results!\n");
            } else {
                printf("Karatsuba results match within tolerance.\n");
            }


            printf("--- Comparing Naive vs Toom-Cook 3 (n=%d) ---\n", n);
            status = calculate_error(2 * n - 1, naive_product, tom_product, &error_sum);
            if (status != STATUS_OK) {
                printf("WARNING: Significant error detected between naive and tom3 results!\n");
            } else {
                printf("Toom-Cook 3 results match within tolerance.\n");
            }


            printf("--- Comparing Naive vs Toom-Cook 4 (n=%d) ---\n", n);
            status = calculate_error(2 * n - 1, naive_product, tom4_product, &error_sum);
            if (status != STATUS_OK) {
                printf("WARNING: Significant error detected between naive and tom4 results!\n");
            } else {
                printf("Toom-Cook 4 results match within tolerance.\n");
            }
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

    timer_log_close();
    
    // Generate plots
    printf("\nGenerating performance plots...\n");
    system("python plot_timings.py");
    
    return STATUS_OK;
}