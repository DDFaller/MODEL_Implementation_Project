/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>
#include <string.h>

#define MPFR_PRECISION 256

static void fill_mpfr_pattern_a(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        double v = 0.1 * (double)(i + 1);
        if (i % 4 == 0) v = 0.0;
        poly[i] = (i % 2 == 0) ? v : -v;
    }
}

static void fill_mpfr_pattern_b(double *poly, int n) {
    for (int i = 0; i < n; i++) {
        double v = 1.0 + 0.05 * (double)(n - i);
        if (i % 5 == 0) v = 0.0;
        poly[i] = (i % 3 == 0) ? v : -v;
    }
}

static void assert_mpfr_vs_naive(TestResult *result, const char *name,
                                 int n1, int n2, double *poly1, double *poly2) {
    const int out_len = n1 + n2 - 1;
    double *expected = malloc((size_t)out_len * sizeof(double));
    double *actual = malloc((size_t)out_len * sizeof(double));

    if (!expected || !actual) {
        result->failed++;
        snprintf(result->last_error, sizeof(result->last_error), "Allocation failed in %s", name);
        free(expected);
        free(actual);
        return;
    }

    naive(n1, poly1, n2, poly2, expected);
    naive_mpfr_from_double(n1, poly1, n2, poly2, actual, MPFR_PRECISION);

    assert_polynomial_equal(result, name, out_len, expected, actual);
    free(expected);
    free(actual);
}

TestResult test_naive_mpfr_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Naive MPFR Multiplication:\n");
    
    // Test 1: Sizes above Karatsuba cutoff (mixed signs)
    {
        const int n1 = KARATSUBA_CUTOFF + 5;
        const int n2 = KARATSUBA_CUTOFF + 2;
        double *poly1 = malloc((size_t)n1 * sizeof(double));
        double *poly2 = malloc((size_t)n2 * sizeof(double));

        fill_mpfr_pattern_a(poly1, n1);
        fill_mpfr_pattern_b(poly2, n2);

        assert_mpfr_vs_naive(&result, "mpfr mixed signs > cutoff", n1, n2, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    // Test 2: Sparse polynomials with leading and trailing zeros
    {
        const int n1 = 9;
        const int n2 = 12;
        double poly1[9];
        double poly2[12];

        memset(poly1, 0, sizeof(poly1));
        memset(poly2, 0, sizeof(poly2));
        poly1[2] = 3.5;
        poly1[5] = -1.25;
        poly1[8] = 2.0;
        poly2[1] = -2.0;
        poly2[7] = 4.0;
        poly2[11] = -0.5;

        assert_mpfr_vs_naive(&result, "sparse with zeros", n1, n2, poly1, poly2);
    }
    
    // Test 3: Fractional coefficients and cancellations
    {
        const int n1 = 7;
        const int n2 = 7;
        double poly1[7] = {0.125, -0.5, 0.75, -1.0, 0.0, 0.25, -0.125};
        double poly2[7] = {-0.25, 0.0, 0.5, -0.75, 1.0, -0.5, 0.25};

        assert_mpfr_vs_naive(&result, "fractional cancellations", n1, n2, poly1, poly2);
    }
    
    print_test_summary("Naive MPFR Multiplication", &result);
    return result;
}

