#include "multiplication_methods.h"
#include <mpfr.h>

int naive_mpfr(int n1, const char **poly1_str, int n2, const char **poly2_str, char **product_str, int precision) {
    int nProd = n1 + n2 - 1;
    
    mpfr_set_default_prec(precision);
    
    mpfr_t *poly1 = malloc(n1 * sizeof(mpfr_t));
    mpfr_t *poly2 = malloc(n2 * sizeof(mpfr_t));
    mpfr_t *product = malloc(nProd * sizeof(mpfr_t));
    mpfr_t temp, sum;
    
    mpfr_init(temp);
    mpfr_init(sum);
    
    for (int i = 0; i < n1; i++) {
        mpfr_init(poly1[i]);
        mpfr_set_str(poly1[i], poly1_str[i], 10, MPFR_RNDN);
    }
    
    for (int i = 0; i < n2; i++) {
        mpfr_init(poly2[i]);
        mpfr_set_str(poly2[i], poly2_str[i], 10, MPFR_RNDN);
    }
    
    // Initialize product array
    for (int i = 0; i < nProd; i++) {
        mpfr_init(product[i]);
        mpfr_set_ui(product[i], 0, MPFR_RNDN);
    }
    
    // Naive multiplication
    for (int i = 0; i < nProd; i++) {
        for (int j = 0; j < n1; j++) {
            int k = i - j;
            if (k >= 0 && k < n2) {
                mpfr_mul(temp, poly1[j], poly2[k], MPFR_RNDN);
                mpfr_add(product[i], product[i], temp, MPFR_RNDN);
            }
        }
    }
    
    for (int i = 0; i < nProd; i++) {
        product_str[i] = malloc(100);
        mpfr_sprintf(product_str[i], "%.50Rf", product[i]);
    }
    
    for (int i = 0; i < n1; i++) mpfr_clear(poly1[i]);
    for (int i = 0; i < n2; i++) mpfr_clear(poly2[i]);
    for (int i = 0; i < nProd; i++) mpfr_clear(product[i]);
    mpfr_clear(temp);
    mpfr_clear(sum);
    
    free(poly1);
    free(poly2);
    free(product);
    
    return STATUS_OK;
}

int naive_mpfr_from_double(int n1, double *poly1_d, int n2, double *poly2_d, double *product_d, int precision) {
    int nProd = n1 + n2 - 1;
    
    // Convert doubles to strings
    char **poly1_str = malloc(n1 * sizeof(char*));
    char **poly2_str = malloc(n2 * sizeof(char*));
    char **product_str = malloc(nProd * sizeof(char*));
    
    for (int i = 0; i < n1; i++) {
        poly1_str[i] = malloc(50);
        sprintf(poly1_str[i], "%.15g", poly1_d[i]);
    }
    
    for (int i = 0; i < n2; i++) {
        poly2_str[i] = malloc(50);
        sprintf(poly2_str[i], "%.15g", poly2_d[i]);
    }
    
    // Call MPFR naive
    int status = naive_mpfr(n1, (const char**)poly1_str, n2, (const char**)poly2_str, product_str, precision);
    
    // Convert back to doubles
    for (int i = 0; i < nProd; i++) {
        product_d[i] = atof(product_str[i]);
    }
    
    // Free memory
    for (int i = 0; i < n1; i++) free(poly1_str[i]);
    for (int i = 0; i < n2; i++) free(poly2_str[i]);
    for (int i = 0; i < nProd; i++) free(product_str[i]);
    free(poly1_str);
    free(poly2_str);
    free(product_str);
    
    return status;
}