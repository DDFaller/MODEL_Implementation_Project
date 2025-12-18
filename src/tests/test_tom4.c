#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>

TestResult test_tom4_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Toom-Cook 4 Multiplication:\n");
    
    // Test 1: Small polynomial that will use naive base case
    {
        double poly1[] = {1.0, 2.0, 3.0, 4.0};
        double poly2[] = {5.0, 6.0, 7.0, 8.0};
        double naive_result[7];
        double tom4_result[7] = {0};
        
        double a[4] = {1.0, 2.0, 3.0, 4.0};
        double b[4] = {5.0, 6.0, 7.0, 8.0};
        double result_array[8] = {0};
        
        naive(4, poly1, 4, poly2, naive_result);
        tom4(a, b, 4, result_array);
        
        for (int i = 0; i < 7; i++) {
            tom4_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "4x4 vs naive", 7, naive_result, tom4_result);
    }
    
    // Test 2: Size 12 polynomial (will use Toom-Cook 4)
    {
        double poly1[12], poly2[12];
        double naive_result[23];
        double tom4_result[23] = {0};
        
        for (int i = 0; i < 12; i++) {
            poly1[i] = (double)(i + 1);
            poly2[i] = (double)(12 - i);
        }
        
        double a[12], b[12];
        double result_array[24] = {0};
        
        for (int i = 0; i < 12; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(12, poly1, 12, poly2, naive_result);
        tom4(a, b, 12, result_array);
        
        for (int i = 0; i < 23; i++) {
            tom4_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "12x12 vs naive", 23, naive_result, tom4_result);
    }
    
    // Test 3: Size 16 polynomial
    {
        double poly1[16], poly2[16];
        double naive_result[31];
        double tom4_result[31] = {0};
        
        for (int i = 0; i < 16; i++) {
            poly1[i] = (i % 2 == 0) ? 1.0 : -1.0;
            poly2[i] = (double)(i + 1) * 0.5;
        }
        
        double a[16], b[16];
        double result_array[32] = {0};
        
        for (int i = 0; i < 16; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(16, poly1, 16, poly2, naive_result);
        tom4(a, b, 16, result_array);
        
        for (int i = 0; i < 31; i++) {
            tom4_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "16x16 vs naive", 31, naive_result, tom4_result);
    }
    
    print_test_summary("Toom-Cook 4 Multiplication", &result);
    return result;
}