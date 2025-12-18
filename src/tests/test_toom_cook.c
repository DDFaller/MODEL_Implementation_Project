#include "test_framework.h"
#include "../multiplication_methods.h"
#include <stdlib.h>

TestResult test_toom_cook_multiplication(void) {
    TestResult result;
    init_test_result(&result);
    
    printf("\nTesting Toom-Cook 3 Multiplication:\n");
    
    // Test 1: Small polynomial that will use naive base case
    {
        double poly1[] = {1.0, 2.0, 3.0};
        double poly2[] = {4.0, 5.0, 6.0};
        double naive_result[5];
        double tom_result[5] = {0};
        
        // Prepare arrays for tom function
        double a[3] = {1.0, 2.0, 3.0};
        double b[3] = {4.0, 5.0, 6.0};
        double result_array[6] = {0};
        
        naive(3, poly1, 3, poly2, naive_result);
        tom(a, b, 3, result_array);
        
        // Copy relevant part
        for (int i = 0; i < 5; i++) {
            tom_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "3x3 vs naive", 5, naive_result, tom_result);
    }
    
    // Test 2: Size 9 polynomial (will use Toom-Cook)
    {
        double poly1[9], poly2[9];
        double naive_result[17];
        double tom_result[17] = {0};
        
        // Initialize with simple pattern
        for (int i = 0; i < 9; i++) {
            poly1[i] = (double)(i + 1);
            poly2[i] = (double)(9 - i);
        }
        
        double a[9], b[9];
        double result_array[18] = {0};
        
        for (int i = 0; i < 9; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(9, poly1, 9, poly2, naive_result);
        tom(a, b, 9, result_array);
        
        // Copy relevant part
        for (int i = 0; i < 17; i++) {
            tom_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "9x9 vs naive", 17, naive_result, tom_result);
    }
    
    // Test 3: Size 12 polynomial
    {
        double poly1[12], poly2[12];
        double naive_result[23];
        double tom_result[23] = {0};
        
        // Initialize with alternating pattern
        for (int i = 0; i < 12; i++) {
            poly1[i] = (i % 2 == 0) ? 1.0 : -1.0;
            poly2[i] = (double)(i + 1) * 0.5;
        }
        
        double a[12], b[12];
        double result_array[24] = {0};
        
        for (int i = 0; i < 12; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(12, poly1, 12, poly2, naive_result);
        tom(a, b, 12, result_array);
        
        // Copy relevant part
        for (int i = 0; i < 23; i++) {
            tom_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "12x12 vs naive", 23, naive_result, tom_result);
    }
    
    // Test 4: Size 15 polynomial
    {
        double poly1[15], poly2[15];
        double naive_result[29];
        double tom_result[29] = {0};
        
        // Initialize with sequential values
        for (int i = 0; i < 15; i++) {
            poly1[i] = (double)(i * 2 + 1);
            poly2[i] = (double)(15 - i);
        }
        
        double a[15], b[15];
        double result_array[30] = {0};
        
        for (int i = 0; i < 15; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(15, poly1, 15, poly2, naive_result);
        tom(a, b, 15, result_array);
        
        // Copy relevant part
        for (int i = 0; i < 29; i++) {
            tom_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "15x15 vs naive", 29, naive_result, tom_result);
    }
    
    // Test 5: Edge case - exactly divisible by 3
    {
        double poly1[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
        double poly2[6] = {6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
        double naive_result[11];
        double tom_result[11] = {0};
        
        double a[6], b[6];
        double result_array[12] = {0};
        
        for (int i = 0; i < 6; i++) {
            a[i] = poly1[i];
            b[i] = poly2[i];
        }
        
        naive(6, poly1, 6, poly2, naive_result);
        tom(a, b, 6, result_array);
        
        // Copy relevant part
        for (int i = 0; i < 11; i++) {
            tom_result[i] = result_array[i];
        }
        
        assert_polynomial_equal(&result, "6x6 vs naive", 11, naive_result, tom_result);
    }
    
    print_test_summary("Toom-Cook 3 Multiplication", &result);
    return result;
}