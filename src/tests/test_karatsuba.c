/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>
#include <string.h>

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

static void fill_sparse(double *poly, int n) {
    memset(poly, 0, (size_t)n * sizeof(double));
    if (n > 0) poly[0] = 1.0;
    if (n > 3) poly[3] = -2.5;
    if (n > 7) poly[7] = 4.0;
    if (n > 11) poly[11] = -1.75;
    if (n > 0) poly[n - 1] = 3.25;
}

static void assert_karatsuba_vs_naive(TestResult *result, const char *name,
                                      int n1, int n2, const double *poly1, const double *poly2) {
    const int out_len = n1 + n2 - 1;
    double *naive_result = malloc((size_t)out_len * sizeof(double));
    double *karatsuba_result = malloc((size_t)out_len * sizeof(double));

    if (!naive_result || !karatsuba_result) {
        result->failed++;
        snprintf(result->last_error, sizeof(result->last_error), "Allocation failed in %s", name);
        free(naive_result);
        free(karatsuba_result);
        return;
    }

    naive(n1, (double *)poly1, n2, (double *)poly2, naive_result);
    karatsuba(n1, poly1, n2, poly2, karatsuba_result);

    assert_polynomial_equal(result, name, out_len, naive_result, karatsuba_result);
    free(naive_result);
    free(karatsuba_result);
}

TestResult test_karatsuba_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Karatsuba Multiplication:\n");
    
    // Test 1: Just above cutoff, odd length
    {
        const int n = KARATSUBA_CUTOFF + 1;
        double *poly1 = malloc((size_t)n * sizeof(double));
        double *poly2 = malloc((size_t)n * sizeof(double));

        fill_ramp(poly1, n, 1.0, 0.75);
        fill_alternating(poly2, n, 2.0);

        assert_karatsuba_vs_naive(&result, "cutoff+1 odd length", n, n, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    // Test 2: Unbalanced sizes, both above cutoff
    {
        const int n1 = KARATSUBA_CUTOFF + 9;
        const int n2 = KARATSUBA_CUTOFF + 3;
        double *poly1 = malloc((size_t)n1 * sizeof(double));
        double *poly2 = malloc((size_t)n2 * sizeof(double));

        fill_sparse(poly1, n1);
        fill_ramp(poly2, n2, -3.0, 0.4);

        assert_karatsuba_vs_naive(&result, "unbalanced > cutoff", n1, n2, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    // Test 3: Power-of-two length with cancellations
    {
        const int n = 32;
        double poly1[32];
        double poly2[32];

        fill_alternating(poly1, n, 1.0);
        fill_alternating(poly2, n, 0.5);

        assert_karatsuba_vs_naive(&result, "32x32 alternating", n, n, poly1, poly2);
    }
    
    // Test 4: Trailing zeros in higher degrees
    {
        const int n1 = KARATSUBA_CUTOFF + 6;
        const int n2 = KARATSUBA_CUTOFF + 7;
        double *poly1 = malloc((size_t)n1 * sizeof(double));
        double *poly2 = malloc((size_t)n2 * sizeof(double));

        fill_ramp(poly1, n1, 0.5, 0.25);
        fill_ramp(poly2, n2, -1.0, 0.3);

        for (int i = 0; i < 3; i++) {
            poly1[n1 - 1 - i] = 0.0;
            poly2[n2 - 1 - i] = 0.0;
        }

        assert_karatsuba_vs_naive(&result, "trailing zeros", n1, n2, poly1, poly2);
        free(poly1);
        free(poly2);
    }
    
    print_test_summary("Karatsuba Multiplication", &result);
    return result;
}

