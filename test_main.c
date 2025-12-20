/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "src/tests/test_framework.h"
#include "src/multiplication_methods.h"
#include "src/utils/utils.h"
#include "src/mpfr_poly.h"

void print_polynomial_formatted(int n, double *coeffs, const char *name) {
    printf("%s(x) = ", name);
    for (int i = n - 1; i >= 0; i--) {
        if (i == n - 1) {
            printf("%.3f", coeffs[i]);
        } else {
            if (coeffs[i] >= 0) {
                printf(" + %.3f", coeffs[i]);
            } else {
                printf(" - %.3f", -coeffs[i]);
            }
        }
        if (i > 1) printf("x^%d", i);
        else if (i == 1) printf("x");
    }
    printf("\n");
}

static void fill_ramp(double *poly, int n, double start, double step) {
    for (int i = 0; i < n; i++) {
        poly[i] = start + step * (double)i;
    }
}

static void fill_alternating(double *poly, int n, double base) {
    for (int i = 0; i < n; i++) {
        double v = base * (1.0 + 0.1 * (double)(i % 7));
        poly[i] = (i % 2 == 0) ? v : -v;
        if (i % 5 == 0) poly[i] = 0.0;
    }
}

static void fill_pattern_a(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        if (i % 6 == 0) {
            poly[i] = 0.0;
        } else if (i % 3 == 0) {
            poly[i] = 1.25;
        } else if (i % 3 == 1) {
            poly[i] = -0.75 * (double)(i + 1);
        } else {
            poly[i] = 0.5 * (double)(i + 1);
        }
    }
}

static void fill_pattern_b(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        double v = 0.2 * (double)(n - i);
        if (i % 4 == 0) v = 0.0;
        poly[i] = (i % 2 == 0) ? v : -v;
    }
}

static void fill_pattern_c(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        double v = 0.15 * (double)(i + 1);
        if (i % 7 == 0) v = 0.0;
        poly[i] = (i % 2 == 0) ? v : -v;
    }
}

static void fill_pattern_d(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        double v = 1.0 + 0.05 * (double)(n - i);
        if (i % 5 == 0) v = 0.0;
        poly[i] = (i % 3 == 0) ? v : -v;
    }
}

static void run_all_algorithms(int n1, int n2, const double *poly1, const double *poly2) {
    const int out_len = n1 + n2 - 1;
    double *naive_result = malloc((size_t)out_len * sizeof(double));
    double *karatsuba_result = malloc((size_t)out_len * sizeof(double));
    double *tom_result = malloc((size_t)out_len * sizeof(double));
    double *tom4_result = malloc((size_t)out_len * sizeof(double));

    if (!naive_result || !karatsuba_result || !tom_result || !tom4_result) {
        printf("Allocation failed.\n");
        free(naive_result);
        free(karatsuba_result);
        free(tom_result);
        free(tom4_result);
        return;
    }

    naive(n1, (double *)poly1, n2, (double *)poly2, naive_result);
    karatsuba(n1, poly1, n2, poly2, karatsuba_result);

    int max_size = (n1 > n2) ? n1 : n2;
    double *a = calloc((size_t)max_size, sizeof(double));
    double *b = calloc((size_t)max_size, sizeof(double));
    double *tom_full_result = calloc((size_t)max_size * 2, sizeof(double));
    double *tom4_full_result = calloc((size_t)max_size * 2, sizeof(double));

    if (!a || !b || !tom_full_result || !tom4_full_result) {
        printf("Allocation failed.\n");
        free(naive_result);
        free(karatsuba_result);
        free(tom_result);
        free(tom4_result);
        free(a);
        free(b);
        free(tom_full_result);
        free(tom4_full_result);
        return;
    }

    for (int i = 0; i < n1; i++) a[i] = poly1[i];
    for (int i = 0; i < n2; i++) b[i] = poly2[i];

    tom(a, b, max_size, tom_full_result);
    tom4(a, b, max_size, tom4_full_result);

    for (int i = 0; i < out_len; i++) {
        tom_result[i] = tom_full_result[i];
        tom4_result[i] = tom4_full_result[i];
    }

    printf("\n--- INPUT POLYNOMIALS ---\n");
    print_polynomial_formatted(n1, (double *)poly1, "P1");
    print_polynomial_formatted(n2, (double *)poly2, "P2");

    printf("\n--- RESULTS ---\n");
    print_polynomial_formatted(out_len, naive_result, "Naive");
    print_polynomial_formatted(out_len, karatsuba_result, "Karatsuba");
    print_polynomial_formatted(out_len, tom_result, "Toom-Cook 3");
    print_polynomial_formatted(out_len, tom4_result, "Toom-Cook 4");

    printf("\n--- VERIFICATION ---\n");
    double error_k = 0.0, error_t = 0.0, error_t4 = 0.0;
    for (int i = 0; i < out_len; i++) {
        error_k += fabs(naive_result[i] - karatsuba_result[i]);
        error_t += fabs(naive_result[i] - tom_result[i]);
        error_t4 += fabs(naive_result[i] - tom4_result[i]);
    }

    printf("Karatsuba vs Naive error: %.2e\n", error_k);
    printf("Toom-Cook 3 vs Naive error: %.2e\n", error_t);
    printf("Toom-Cook 4 vs Naive error: %.2e\n", error_t4);

    if (error_k < 1e-10 && error_t < 1e-10 && error_t4 < 1e-10) {
        printf("OK: All algorithms agree.\n");
    } else {
        printf("WARN: Difference between algorithms TOLERANCE 1e-10.\n");
    }

    free(naive_result);
    free(karatsuba_result);
    free(tom_result);
    free(tom4_result);
    free(a);
    free(b);
    free(tom_full_result);
    free(tom4_full_result);
}

void interactive_test() {
    printf("\n=== INTERACTIVE TEST ===\n");

    int choice;
    do {
        printf("\nChoose an option:\n");
        printf("1. Karatsuba cutoff+1 (odd length)\n");
        printf("2. Toom-3 cutoff boundary\n");
        printf("3. Toom-3 cutoff+2 (padding path)\n");
        printf("4. Toom-4 cutoff+1 (padding path)\n");
        printf("5. Unbalanced sparse polynomials\n");
        printf("6. Enter custom polynomials\n");
        printf("7. Random polynomials with custom size\n");
        printf("0. Back\n");
        printf("Option: ");
        scanf("%d", &choice);

        double *poly1 = NULL;
        double *poly2 = NULL;
        int n1, n2;

        switch(choice) {
            case 1:
                n1 = n2 = KARATSUBA_CUTOFF + 1;
                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));
                fill_ramp(poly1, n1, 1.0, 0.75);
                fill_alternating(poly2, n2, 2.0);
                break;

            case 2:
                n1 = n2 = TOOM3_CUTOFF;
                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));
                fill_pattern_a(poly1, n1);
                fill_pattern_b(poly2, n2);
                break;

            case 3:
                n1 = n2 = TOOM3_CUTOFF + 2;
                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));
                fill_pattern_a(poly1, n1);
                fill_pattern_b(poly2, n2);
                break;

            case 4:
                n1 = n2 = TOOM4_CUTOFF + 1;
                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));
                fill_pattern_c(poly1, n1);
                fill_pattern_d(poly2, n2);
                break;

            case 5:
                n1 = KARATSUBA_CUTOFF + 9;
                n2 = KARATSUBA_CUTOFF + 3;
                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));
                memset(poly1, 0, (size_t)n1 * sizeof(double));
                memset(poly2, 0, (size_t)n2 * sizeof(double));
                if (n1 > 0) poly1[0] = 1.0;
                if (n1 > 3) poly1[3] = -2.5;
                if (n1 > 7) poly1[7] = 4.0;
                if (n1 > 0) poly1[n1 - 1] = 3.25;
                fill_ramp(poly2, n2, -3.0, 0.4);
                break;

            case 6:
                printf("Size of first polynomial: ");
                scanf("%d", &n1);
                printf("Size of second polynomial: ");
                scanf("%d", &n2);

                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));

                printf("Coefficients of first polynomial (from lowest to highest degree):\n");
                for (int i = 0; i < n1; i++) {
                    printf("coeff[%d]: ", i);
                    scanf("%lf", &poly1[i]);
                }

                printf("Coefficients of second polynomial (from lowest to highest degree):\n");
                for (int i = 0; i < n2; i++) {
                    printf("coeff[%d]: ", i);
                    scanf("%lf", &poly2[i]);
                }
                break;

            case 7:
                printf("Size of polynomials (both will have same size): ");
                scanf("%d", &n1);
                n2 = n1;

                poly1 = malloc((size_t)n1 * sizeof(double));
                poly2 = malloc((size_t)n2 * sizeof(double));

                int degree;
                generate_random_polynomial(n1, poly1, &degree);
                generate_random_polynomial(n2, poly2, &degree);

                printf("Generated random polynomials of size %d\n", n1);
                break;

            case 0:
                return;

            default:
                printf("Invalid option!\n");
                continue;
        }

        if (choice >= 1 && choice <= 7) {
            run_all_algorithms(n1, n2, poly1, poly2);
        }

        free(poly1);
        free(poly2);

    } while (choice != 0);
}

int main(void) {
    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("  POLYNOMIAL MULTIPLICATION UNIT TESTS\n");
    printf("========================================\n");

    int all_passed = run_all_tests();

    printf("========================================\n");
    if (all_passed) {
        printf("ALL TESTS PASSED. System is ready.\n");
    } else {
        printf("SOME TESTS FAILED. Check implementation.\n");
    }

    int choice;
    do {
        printf("\n=== MAIN MENU ===\n");
        printf("1. Run unit tests again\n");
        printf("2. Interactive test with visualization\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("\n");
                run_all_tests();
                break;
            case 2:
                interactive_test();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid option!\n");
        }
    } while (choice != 0);

    return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
