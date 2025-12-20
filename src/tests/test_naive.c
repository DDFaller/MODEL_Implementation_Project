/*
 * MACHADO CARNEIRO FALLER Daniel (21400117)
 * LY Laura (21500152)
 * CRIVELLARO Federico (21502450)
 */

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

    // Test 6: Larger sparse polynomials with far-apart terms
    {
        double poly1[12] = {0};
        double poly2[9] = {0};
        double expected[20] = {0};
        double actual[20];

        poly1[0] = 1.5;
        poly1[11] = -2.0;
        poly2[2] = 3.0;
        poly2[8] = -1.0;

        expected[2] = 4.5;
        expected[8] = -1.5;
        expected[13] = -6.0;
        expected[19] = 2.0;

        naive(12, poly1, 9, poly2, actual);
        assert_polynomial_equal(&result, "Sparse large polynomials", 20, expected, actual);
    }
    
    print_test_summary("Naive Multiplication", &result);
    return result;
}

