#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>

TestResult test_naive_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Naive Multiplication:\n");
    
    // Test 1: Simple multiplication [1, 2] * [3, 4] = [3, 10, 8]
    {
        double poly1[] = {1.0, 2.0};
        double poly2[] = {3.0, 4.0};
        double expected[] = {3.0, 10.0, 8.0};
        double actual[3];
        
        naive(2, poly1, 2, poly2, actual);
        assert_polynomial_equal(&result, "Simple 2x2 multiplication", 3, expected, actual);
    }
    
    // Test 2: Multiplication with zero [1, 0] * [2, 3] = [2, 3, 0]
    {
        double poly1[] = {1.0, 0.0};
        double poly2[] = {2.0, 3.0};
        double expected[] = {2.0, 3.0, 0.0};
        double actual[3];
        
        naive(2, poly1, 2, poly2, actual);
        assert_polynomial_equal(&result, "Multiplication with zero", 3, expected, actual);
    }
    
    // Test 3: Single coefficient [5] * [3] = [15]
    {
        double poly1[] = {5.0};
        double poly2[] = {3.0};
        double expected[] = {15.0};
        double actual[1];
        
        naive(1, poly1, 1, poly2, actual);
        assert_polynomial_equal(&result, "Single coefficient multiplication", 1, expected, actual);
    }
    
    // Test 4: Different sizes [1, 2, 3] * [4, 5] = [4, 13, 22, 15]
    {
        double poly1[] = {1.0, 2.0, 3.0};
        double poly2[] = {4.0, 5.0};
        double expected[] = {4.0, 13.0, 22.0, 15.0};
        double actual[4];
        
        naive(3, poly1, 2, poly2, actual);
        assert_polynomial_equal(&result, "Different size polynomials", 4, expected, actual);
    }
    
    // Test 5: Negative coefficients [-1, 2] * [3, -4] = [-3, 10, -8]
    {
        double poly1[] = {-1.0, 2.0};
        double poly2[] = {3.0, -4.0};
        double expected[] = {-3.0, 10.0, -8.0};
        double actual[3];
        
        naive(2, poly1, 2, poly2, actual);
        assert_polynomial_equal(&result, "Negative coefficients", 3, expected, actual);
    }
    
    print_test_summary("Naive Multiplication", &result);
    return result;
}