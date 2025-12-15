#include "multiplication_methods.h"
#include <stdlib.h>


#define USE_NAIVE 4

void Karatsuba (double * P1, double * P2, double * res, int d1, int d2, int k) {
   /* The algorithm must rely on the naive algorithm when polynomials have size at most k. */
    if (d1 < k || d2 < k) {
        naive(d1, P1, d2, P2, res);
    } else {
        int m = (d1 > d2 ? d1 : d2) / 2; // could it be (d1 + d2) / 2 ?
        double * P1_ = malloc((m + 1) * sizeof(double));
        double * P2_ = malloc((m + 1) * sizeof(double));

        for (int i = 0; i < m; i++) {
            P1_[i] = (i < d1 ? P1[i] : 0) + (m + i < d1 ? P1[m + i] : 0);
            P2_[i] = (i < d2 ? P2[i] : 0) + (m + i < d2 ? P2[m + i] : 0);
        }
        P1_[m] = 0;
        P2_[m] = 0;

        double * R1 = malloc((2 * m) * sizeof(double));
        double * R2 = malloc((2 * m) * sizeof(double));
        double * R3 = malloc((2 * (m + 1)) * sizeof(double));

        Karatsuba(P1, P2, R1, m, m, k);
        Karatsuba(P1 + m, P2 + m, R2, d1 - m, d2 - m, k);
        Karatsuba(P1_, P2_, R3, m + 1, m + 1, k);

        double * R4 = malloc((2 * (m + 1)) * sizeof(double));
        for (int i = 0; i < 2 * m; i++) {
            R4[i] = R3[i] - R1[i] - R2[i];
        }
        for (int i = 0; i < d1 + d2 - 1; i++) {
            if (i < 2 * m) {
                res[i] = R1[i];
            } else {
                res[i] = 0;
            }
            if (i - m >= 0 && i - m < 2 * m) {
                res[i] += R4[i - m];
            }
            if (i - 2 * m >= 0 && i - 2 * m < 2 * m) {
                res[i] += R2[i - 2 * m];
            }
        }
        
        free(P1_);
        free(P2_);
        free(R1);
        free(R2);
        free(R3);
        free(R4);
    }
}

int karatsuba(int n1, const double *poly1, int n2, const double *poly2, double *product) {
    Karatsuba((double*)poly1, (double*)poly2, product, n1, n2, USE_NAIVE);
    return STATUS_OK;
}