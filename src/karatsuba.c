#include "multiplication_methods.h"

void Karatsuba (double * P1, double * P2, double * res, int d1, int d2, int k) {
   /* The algorithm must rely on the naive algorithm when polynomials have size at most k. */
    if (d1 < k || d2 < k) {
        naive(P1, P2, d1, d2, res);
    } else {
        int m = (d1 > d2 ? d1 : d2) / 2; // could it be (d1 + d2) / 2 ?
        double * P1_ = malloc((m) * sizeof(double));
        double * P2_ = malloc((m) * sizeof(double));

        for (int i = 0; i < m; i++) {
            P1_[i] = (P1[i] + P1[m + i]);
            P2_[i] = (P2[i] + P2[m + i]);
        }
        if (d1 > 2*m - 1) {
            P1_[m] = P1[d1];
        } else {
            P1_[m] = 0;
        }
        if (d2 > 2*m - 1) {
            P2_[m] = P2[d2];
        } else {
            P2_[m] = 0;
        }

        double * R1 = malloc((2 * (m-1)) * sizeof(double));
        double * R2 = malloc((2 * (m-1)) * sizeof(double));
        double * R3 = malloc((2 * m) * sizeof(double));

        Karatsuba(P1, P2, R1, m, m, k);
        Karatsuba(P1 + m, P2 + m, R2, d1 - m, d2 - m, k);
        Karatsuba(P1_, P2_, R3, m + 1, m + 1, k);

        double * R4 = malloc((2 * m) * sizeof(double));
        for (int i = 0; i < 2 * (m - 1); i++) {
            R4[i] = R3[i] - R1[i] - R2[i];
        }
        for (int i = 0; i < 2 * (d1 + d2 - 1); i++) {
            if (i < 2 * (m - 1)) {
                res[i] = R1[i];
            } else {
                res[i] = 0;
            }
            if (i - m >= 0 && i - m < 2 * m) {
                res[i] += R4[i - m];
            }
            if (i - 2 * m >= 0 && i - 2 * m < 2 * (m - 1)) {
                res[i] += R2[i - 2 * m];
            }
        }
    }

}