
#include "multiplication_methods.h"



int naive(
    int n1,double *poly1,
    int n2,double *poly2,
    double *product) 
{
    // The result has size n1 + n2 - 1
    int nProd = n1 + n2 - 1;

    for (int i = 0; i < nProd; i++) {
        // Coefficient of x^i
        product[i] = 0.0f;

        // Sum over all j such that 0 <= j < n1 and 0 <= i - j < n2
        for (int j = 0; j < n1; j++) {
            int k = i - j;  // index in poly2

            if (k >= 0 && k < n2) {
                product[i] += poly1[j] * poly2[k];
            }
        }
    }
    return STATUS_OK; // STATUS_OK
}