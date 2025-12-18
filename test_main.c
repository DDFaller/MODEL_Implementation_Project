#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/tests/test_framework.h"
#include "src/multiplication_methods.h"
#include "src/utils/utils.h"

void print_polynomial_formatted(int n, double *coeffs, const char *name) {
    printf("%s(x) = ", name);
    for (int i = n-1; i >= 0; i--) {
        if (i == n-1) {
            printf("%.3f", coeffs[i]);
        } else {
            if (coeffs[i] >= 0) {
                printf(" + %.3f", coeffs[i]);
            } else {
                printf(" - %.3f", -coeffs[i]);
            }
        }
        if (i > 1) printf("x^%d", i);
        else if (i == 1) printf("x");
    }
    printf("\n");
}

void interactive_test() {
    printf("\n=== INTERACTIVE TEST ===\n");
    
    int choice;
    do {
        printf("\nChoose an option:\n");
        printf("1. Simple test [1,2] * [3,4]\n");
        printf("2. Test with zeros [1,0,3] * [2,1]\n");
        printf("3. Larger test [1,2,3,4] * [5,6,7,8]\n");
        printf("4. Enter custom polynomials\n");
        printf("5. Random polynomials with custom size\n");
        printf("0. Back\n");
        printf("Option: ");
        scanf("%d", &choice);
        
        double *poly1, *poly2, *naive_result, *karatsuba_result, *tom_result, *tom4_result;
        int n1, n2;
        
        switch(choice) {
            case 1:
                n1 = n2 = 2;
                poly1 = malloc(n1 * sizeof(double));
                poly2 = malloc(n2 * sizeof(double));
                poly1[0] = 1; poly1[1] = 2;
                poly2[0] = 3; poly2[1] = 4;
                break;
                
            case 2:
                n1 = 3; n2 = 2;
                poly1 = malloc(n1 * sizeof(double));
                poly2 = malloc(n2 * sizeof(double));
                poly1[0] = 1; poly1[1] = 0; poly1[2] = 3;
                poly2[0] = 2; poly2[1] = 1;
                break;
                
            case 3:
                n1 = n2 = 4;
                poly1 = malloc(n1 * sizeof(double));
                poly2 = malloc(n2 * sizeof(double));
                poly1[0] = 1; poly1[1] = 2; poly1[2] = 3; poly1[3] = 4;
                poly2[0] = 5; poly2[1] = 6; poly2[2] = 7; poly2[3] = 8;
                break;
                
            case 4:
                printf("Size of first polynomial: ");
                scanf("%d", &n1);
                printf("Size of second polynomial: ");
                scanf("%d", &n2);
                
                poly1 = malloc(n1 * sizeof(double));
                poly2 = malloc(n2 * sizeof(double));
                
                printf("Coefficients of first polynomial (from lowest to highest degree):\n");
                for (int i = 0; i < n1; i++) {
                    printf("coeff[%d]: ", i);
                    scanf("%lf", &poly1[i]);
                }
                
                printf("Coefficients of second polynomial (from lowest to highest degree):\n");
                for (int i = 0; i < n2; i++) {
                    printf("coeff[%d]: ", i);
                    scanf("%lf", &poly2[i]);
                }
                break;
                
            case 5:
                printf("Size of polynomials (both will have same size): ");
                scanf("%d", &n1);
                n2 = n1;
                
                poly1 = malloc(n1 * sizeof(double));
                poly2 = malloc(n2 * sizeof(double));
                
                int degree;
                generate_random_polynomial(n1, poly1, &degree);
                generate_random_polynomial(n2, poly2, &degree);
                
                printf("Generated random polynomials of size %d\n", n1);
                break;
                
            case 0:
                return;
                
            default:
                printf("Invalid option!\n");
                continue;
        }
        
        if (choice >= 1 && choice <= 5) {
            // Allocate results
            naive_result = malloc((n1 + n2 - 1) * sizeof(double));
            karatsuba_result = malloc((n1 + n2 - 1) * sizeof(double));
            tom_result = malloc((n1 + n2 - 1) * sizeof(double));
            tom4_result = malloc((n1 + n2 - 1) * sizeof(double));
            
            // Execute multiplications
            naive(n1, poly1, n2, poly2, naive_result);
            karatsuba(n1, poly1, n2, poly2, karatsuba_result);
            
            // For Toom-Cook 3
            int max_size = (n1 > n2) ? n1 : n2;
            double *a = calloc(max_size, sizeof(double));
            double *b = calloc(max_size, sizeof(double));
            double *tom_full_result = calloc(max_size * 2, sizeof(double));
            
            for (int i = 0; i < n1; i++) a[i] = poly1[i];
            for (int i = 0; i < n2; i++) b[i] = poly2[i];
            
            tom(a, b, max_size, tom_full_result);
            
            for (int i = 0; i < n1 + n2 - 1; i++) {
                tom_result[i] = tom_full_result[i];
            }
            
            // For Toom-Cook 4
            double *a4 = calloc(max_size, sizeof(double));
            double *b4 = calloc(max_size, sizeof(double));
            double *tom4_full_result = calloc(max_size * 2, sizeof(double));
            
            for (int i = 0; i < n1; i++) a4[i] = poly1[i];
            for (int i = 0; i < n2; i++) b4[i] = poly2[i];
            
            tom4(a4, b4, max_size, tom4_full_result);
            
            for (int i = 0; i < n1 + n2 - 1; i++) {
                tom4_result[i] = tom4_full_result[i];
            }
            
            // Show results
            printf("\n--- INPUT POLYNOMIALS ---\n");
            print_polynomial_formatted(n1, poly1, "P1");
            print_polynomial_formatted(n2, poly2, "P2");
            
            printf("\n--- RESULTS ---\n");
            print_polynomial_formatted(n1 + n2 - 1, naive_result, "Naive");
            print_polynomial_formatted(n1 + n2 - 1, karatsuba_result, "Karatsuba");
            print_polynomial_formatted(n1 + n2 - 1, tom_result, "Toom-Cook 3");
            print_polynomial_formatted(n1 + n2 - 1, tom4_result, "Toom-Cook 4");
            
            // Check differences
            printf("\n--- VERIFICATION ---\n");
            double error_k = 0, error_t = 0, error_t4 = 0;
            for (int i = 0; i < n1 + n2 - 1; i++) {
                error_k += fabs(naive_result[i] - karatsuba_result[i]);
                error_t += fabs(naive_result[i] - tom_result[i]);
                error_t4 += fabs(naive_result[i] - tom4_result[i]);
            }
            
            printf("Karatsuba vs Naive error: %.2e\n", error_k);
            printf("Toom-Cook 3 vs Naive error: %.2e\n", error_t);
            printf("Toom-Cook 4 vs Naive error: %.2e\n", error_t4);
            
            if (error_k < 1e-10 && error_t < 1e-10 && error_t4 < 1e-10) {
                printf("✓ All algorithms agree!\n");
            } else {
                printf("⚠ There are differences between algorithms!\n");
            }
            
            // Free memory
            free(poly1); free(poly2);
            free(naive_result); free(karatsuba_result); free(tom_result); free(tom4_result);
            free(a); free(b); free(tom_full_result);
            free(a4); free(b4); free(tom4_full_result);
        }
        
    } while (choice != 0);
}

int main(void) {
    srand((unsigned int)time(NULL));
    
    printf("========================================\n");
    printf("  POLYNOMIAL MULTIPLICATION UNIT TESTS\n");
    printf("========================================\n");
    
    int all_passed = run_all_tests();
    
    printf("========================================\n");
    if (all_passed) {
        printf("🎉 ALL TESTS PASSED! System is ready.\n");
    } else {
        printf("❌ SOME TESTS FAILED! Check implementation.\n");
    }
    
    // Interactive menu
    int choice;
    do {
        printf("\n=== MAIN MENU ===\n");
        printf("1. Run unit tests again\n");
        printf("2. Interactive test with visualization\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf("\n");
                run_all_tests();
                break;
            case 2:
                interactive_test();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid option!\n");
        }
    } while (choice != 0);
    
    return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}