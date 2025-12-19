#include "test_framework.h"
#include <string.h>

void init_test_result(TestResult *result) {
    result->passed = 0;
    result->failed = 0;
    strcpy(result->last_error, "");
}

int compare_polynomials(int size, const double *expected, const double *actual, double tolerance) {
    for (int i = 0; i < size; i++) {
        if (fabs(expected[i] - actual[i]) > tolerance) {
            return 0; // Not equal
        }
    }
    return 1; // Equal within tolerance
}

void assert_polynomial_equal(TestResult *result, const char *test_name, 
                           int size, const double *expected, const double *actual) {
    if (compare_polynomials(size, expected, actual, TEST_TOLERANCE)) {
        result->passed++;
        printf("  ✓ %s\n", test_name);
    } else {
        result->failed++;
        printf("  ✗ %s\n", test_name);
        snprintf(result->last_error, sizeof(result->last_error), 
                "Test '%s' failed: polynomials differ beyond tolerance", test_name);
    }
}

void print_test_summary(const char *suite_name, const TestResult *result) {
    printf("\n%s Results: %d passed, %d failed\n", 
           suite_name, result->passed, result->failed);
    if (result->failed > 0) {
        printf("Last error: %s\n", result->last_error);
    }
}

int run_all_tests(void) {
    printf("Running Unit Tests...\n");
    printf("====================\n");
    

    TestResult naive_result = test_naive_multiplication();
    TestResult karatsuba_result = test_karatsuba_multiplication();
    TestResult toom_result = test_toom_cook_multiplication();
    TestResult tom4_result = test_tom4_multiplication();
    
    TestResult naive_mpfr_result = test_naive_mpfr_multiplication();

    int total_passed = naive_result.passed + karatsuba_result.passed + toom_result.passed + tom4_result.passed;
    int total_failed = naive_result.failed + karatsuba_result.failed + toom_result.failed + tom4_result.failed;
    


    
    
    printf("\n====================\n");
    printf("Overall Results: %d passed, %d failed\n", total_passed, total_failed);
    printf("MPFR comparison results: %d passed, %d failed\n", naive_mpfr_result.passed, naive_mpfr_result.failed);
    
    if (total_failed == 0) {
        printf("All tests PASSED! ✓\n\n");
        return 1;
    } else {
        printf("Some tests FAILED! ✗\n\n");
        return 0;
    }
}