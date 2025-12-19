#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <math.h>
#include "../utils/typedefs.h"

#define TEST_TOLERANCE 1e-10
#define MAX_TEST_SIZE 64

// Test result structure
typedef struct {
    int passed;
    int failed;
    char last_error[256];
} TestResult;

// Test framework functions
void init_test_result(TestResult *result);
int compare_polynomials(int size, const double *expected, const double *actual, double tolerance);
void assert_polynomial_equal(TestResult *result, const char *test_name, 
                           int size, const double *expected, const double *actual);
void print_test_summary(const char *suite_name, const TestResult *result);

// Test suite declarations
TestResult test_naive_multiplication(void);
TestResult test_karatsuba_multiplication(void);
TestResult test_toom_cook_multiplication(void);
TestResult test_tom4_multiplication(void);
TestResult test_naive_mpfr_multiplication(void);

// Main test runner
int run_all_tests(void);

#endif /* TEST_FRAMEWORK_H */