#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>

TestResult test_karatsuba_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Karatsuba Multiplication:\n");
    
    // Test 1: Compare with naive for [1, 2] * [3, 4]
    {
        double poly1[] = {1.0, 2.0};
        double poly2[] = {3.0, 4.0};
        double naive_result[3];
        double karatsuba_result[3];
        
        naive(2, poly1, 2, poly2, naive_result);
        karatsuba(2, poly1, 2, poly2, karatsuba_result);
        
        assert_polynomial_equal(&result, "2x2 vs naive", 3, naive_result, karatsuba_result);
    }
    
    // Test 2: Larger polynomial [1, 2, 3, 4] * [5, 6, 7, 8]
    {
        double poly1[] = {1.0, 2.0, 3.0, 4.0};
        double poly2[] = {5.0, 6.0, 7.0, 8.0};
        double naive_result[7];
        double karatsuba_result[7];
        
        naive(4, poly1, 4, poly2, naive_result);
        karatsuba(4, poly1, 4, poly2, karatsuba_result);
        
        assert_polynomial_equal(&result, "4x4 vs naive", 7, naive_result, karatsuba_result);
    }
    
    // Test 3: Power of 2 size [1, 1, 1, 1, 1, 1, 1, 1] * [2, 2, 2, 2, 2, 2, 2, 2]
    {
        double poly1[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        double poly2[8] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
        double naive_result[15];
        double karatsuba_result[15];
        
        naive(8, poly1, 8, poly2, naive_result);
        karatsuba(8, poly1, 8, poly2, karatsuba_result);
        
        assert_polynomial_equal(&result, "8x8 vs naive", 15, naive_result, karatsuba_result);
    }
    
    // Test 4: Random coefficients size 16
    {
        double poly1[16], poly2[16];
        double naive_result[31];
        double karatsuba_result[31];
        
        // Initialize with simple pattern
        for (int i = 0; i < 16; i++) {
            poly1[i] = (double)(i + 1);
            poly2[i] = (double)(16 - i);
        }
        
        naive(16, poly1, 16, poly2, naive_result);
        karatsuba(16, poly1, 16, poly2, karatsuba_result);
        
        assert_polynomial_equal(&result, "16x16 vs naive", 31, naive_result, karatsuba_result);
    }
    
    // Test 5: Edge case - single element
    {
        double poly1[] = {7.0};
        double poly2[] = {3.0};
        double naive_result[1];
        double karatsuba_result[1];
        
        naive(1, poly1, 1, poly2, naive_result);
        karatsuba(1, poly1, 1, poly2, karatsuba_result);
        
        assert_polynomial_equal(&result, "1x1 vs naive", 1, naive_result, karatsuba_result);
    }
    
    print_test_summary("Karatsuba Multiplication", &result);
    return result;
}