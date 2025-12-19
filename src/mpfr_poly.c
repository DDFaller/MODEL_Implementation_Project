#include "mpfr_poly.h"
#include <stdlib.h>
#include <stdio.h>

int mpfr_poly_init(mpfr_poly_t *p, size_t n, mpfr_prec_t prec_bits) {
    p->n = n;
    p->c = (mpfr_t*)malloc(n * sizeof(mpfr_t));
    if (!p->c) return -1;

    for (size_t i = 0; i < n; i++) {
        mpfr_init2(p->c[i], prec_bits);
        mpfr_set_zero(p->c[i], 0);
    }
    return 0;
}

void mpfr_poly_clear(mpfr_poly_t *p) {
    if (!p || !p->c) return;
    for (size_t i = 0; i < p->n; i++) mpfr_clear(p->c[i]);
    free(p->c);
    p->c = NULL;
    p->n = 0;
}

void mpfr_poly_set_from_double(mpfr_poly_t *dst, const double *src) {
    for (size_t i = 0; i < dst->n; i++) {
        mpfr_set_d(dst->c[i], src[i], MPFR_RNDN);
    }
}

void mpfr_poly_mul_naive(mpfr_poly_t *C,
                         const mpfr_poly_t *A,
                         const mpfr_poly_t *B,
                         mpfr_rnd_t rnd)
{
    for (size_t i = 0; i < C->n; i++) mpfr_set_zero(C->c[i], 0);

    mpfr_t prod;
    mpfr_init2(prod, mpfr_get_prec(C->c[0]));

    for (size_t i = 0; i < A->n; i++) {
        for (size_t j = 0; j < B->n; j++) {
            mpfr_mul(prod, A->c[i], B->c[j], rnd);
            mpfr_add(C->c[i + j], C->c[i + j], prod, rnd);
        }
    }

    mpfr_clear(prod);
}

error_stats_t compare_double_to_mpfr_abs_error(const double *approx,
                                               const mpfr_poly_t *ref,
                                               mpfr_prec_t work_prec_bits)
{
    error_stats_t st = {0.0, 0.0};

    mpfr_t diff, absdiff, maxerr, sumsq, tmp;
    mpfr_init2(diff,    work_prec_bits);
    mpfr_init2(absdiff, work_prec_bits);
    mpfr_init2(maxerr,  work_prec_bits);
    mpfr_init2(sumsq,   work_prec_bits);
    mpfr_init2(tmp,     work_prec_bits);

    mpfr_set_zero(maxerr, 0);
    mpfr_set_zero(sumsq,  0);

    for (size_t i = 0; i < ref->n; i++) {
        mpfr_set(diff, ref->c[i], MPFR_RNDN);
        mpfr_sub_d(diff, diff, approx[i], MPFR_RNDN);

        mpfr_abs(absdiff, diff, MPFR_RNDN);

        if (mpfr_greater_p(absdiff, maxerr)) {
            mpfr_set(maxerr, absdiff, MPFR_RNDN);
        }

        mpfr_mul(tmp, absdiff, absdiff, MPFR_RNDN);
        mpfr_add(sumsq, sumsq, tmp, MPFR_RNDN);
    }

    mpfr_sqrt(tmp, sumsq, MPFR_RNDN);

    st.max_abs_err = mpfr_get_d(maxerr, MPFR_RNDN);
    st.l2_abs_err  = mpfr_get_d(tmp,    MPFR_RNDN);

    mpfr_clear(diff);
    mpfr_clear(absdiff);
    mpfr_clear(maxerr);
    mpfr_clear(sumsq);
    mpfr_clear(tmp);

    return st;
}

product_error_stats_t run_one_case(const double *A_d, size_t nA,
                  const double *B_d, size_t nB,
                  const double *C_naive,
                  const double *C_karatsuba,
                  const double *C_toom3,
                  const double *C_toom4)
{
    size_t nC = nA + nB - 1;

    mpfr_prec_t ref_prec = 4096;
    mpfr_prec_t err_prec = 256;

    mpfr_poly_t A, B, Cref;

    product_error_stats_t products_error;
    if (mpfr_poly_init(&A, nA, ref_prec) != 0) { fprintf(stderr,"alloc A failed\n"); exit(1); }
    if (mpfr_poly_init(&B, nB, ref_prec) != 0) { fprintf(stderr,"alloc B failed\n"); exit(1); }
    if (mpfr_poly_init(&Cref, nC, ref_prec) != 0) { fprintf(stderr,"alloc C failed\n"); exit(1); }

    mpfr_poly_set_from_double(&A, A_d);
    mpfr_poly_set_from_double(&B, B_d);

    mpfr_poly_mul_naive(&Cref, &A, &B, MPFR_RNDN);

    error_stats_t en  = compare_double_to_mpfr_abs_error(C_naive    , &Cref, err_prec);
    error_stats_t eK  = compare_double_to_mpfr_abs_error(C_karatsuba, &Cref, err_prec);
    error_stats_t eT3 = compare_double_to_mpfr_abs_error(C_toom3,     &Cref, err_prec);
    error_stats_t eT4 = compare_double_to_mpfr_abs_error(C_toom4,     &Cref, err_prec);

    products_error.naive    = en;
    products_error.karatsuba = eK;
    products_error.toom3     = eT3;
    products_error.toom4     = eT4;
    printf("Naive: max_abs=%.3e  l2_abs=%.3e\n", 0.0, 0.0);
    printf("Karatsuba: max_abs=%.3e  l2_abs=%.3e\n", eK.max_abs_err,  eK.l2_abs_err);
    printf("Toom-3   : max_abs=%.3e  l2_abs=%.3e\n", eT3.max_abs_err, eT3.l2_abs_err);
    printf("Toom-4   : max_abs=%.3e  l2_abs=%.3e\n", eT4.max_abs_err, eT4.l2_abs_err);

    mpfr_poly_clear(&A);
    mpfr_poly_clear(&B);
    mpfr_poly_clear(&Cref);

    return products_error;
}
