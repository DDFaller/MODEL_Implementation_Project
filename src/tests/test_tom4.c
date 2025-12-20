/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>
#include <string.h>

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

static void assert_tom4_vs_naive(TestResult *result, const char *name,
                                 int n, const double *poly1, const double *poly2) {
    const int out_len = 2 * n - 1;
    double *naive_result = malloc((size_t)out_len * sizeof(double));
    double *tom4_result = calloc((size_t)(2 * n), sizeof(double));
    double *a = malloc((size_t)n * sizeof(double));
    double *b = malloc((size_t)n * sizeof(double));

    if (!naive_result || !tom4_result || !a || !b) {
        result->failed++;
        snprintf(result->last_error, sizeof(result->last_error), "Allocation failed in %s", name);
        free(naive_result);
        free(tom4_result);
        free(a);
        free(b);
        return;
    }

    memcpy(a, poly1, (size_t)n * sizeof(double));
    memcpy(b, poly2, (size_t)n * sizeof(double));

    naive(n, (double *)poly1, n, (double *)poly2, naive_result);
    tom4(a, b, n, tom4_result);

    assert_polynomial_equal(result, name, out_len, naive_result, tom4_result);
    free(naive_result);
    free(tom4_result);
    free(a);
    free(b);
}

TestResult test_tom4_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Toom-Cook 4 Multiplication:\n");
    
    // Test 1: Cutoff boundary (delegates to Toom-3)
    {
        const int n = TOOM4_CUTOFF;
        double *poly1 = malloc((size_t)n * sizeof(double));
        double *poly2 = malloc((size_t)n * sizeof(double));

        fill_pattern_c(poly1, n);
        fill_pattern_d(poly2, n);

        assert_tom4_vs_naive(&result, "cutoff boundary", n, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    // Test 2: Just above cutoff (forces Toom-4 path)
    {
        const int n = TOOM4_CUTOFF + 1;
        double *poly1 = malloc((size_t)n * sizeof(double));
        double *poly2 = malloc((size_t)n * sizeof(double));

        fill_pattern_c(poly1, n);
        fill_pattern_d(poly2, n);

        assert_tom4_vs_naive(&result, "cutoff+1 padding", n, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    print_test_summary("Toom-Cook 4 Multiplication", &result);
    return result;
}

