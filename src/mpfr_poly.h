#ifndef MPFR_POLY_H
#define MPFR_POLY_H

#include <stddef.h>
#include <mpfr.h>


extern mpfr_prec_t MPFR_REF_PREC; /* usado na ref (Cref) */
extern mpfr_prec_t MPFR_ERR_PREC;

typedef struct {
    double max_abs_err;
    double l2_abs_err;
} error_stats_t;

typedef struct {
    size_t n;
    mpfr_t *c;
} mpfr_poly_t;

typedef struct {
    //Naive
    error_stats_t  naive;

    //Karatsuba
    error_stats_t karatsuba;
    
    //Toom3
    error_stats_t toom3;

    //Toom4
    error_stats_t toom4;
}product_error_stats_t;

/* public API */
int  mpfr_poly_init(mpfr_poly_t *p, size_t n, mpfr_prec_t prec_bits);
void mpfr_poly_clear(mpfr_poly_t *p);

void mpfr_poly_set_from_double(mpfr_poly_t *dst, const double *src);

void mpfr_poly_mul_naive(mpfr_poly_t *C,
                         const mpfr_poly_t *A,
                         const mpfr_poly_t *B,
                         mpfr_rnd_t rnd);

error_stats_t compare_double_to_mpfr_abs_error(const double *approx,
                                               const mpfr_poly_t *ref,
                                               mpfr_prec_t work_prec_bits);

product_error_stats_t run_one_case(const double *A_d, size_t nA,
                                   const double *B_d, size_t nB,
                                   const double *C_naive,
                                   const double *C_karatsuba,
                                   const double *C_toom3,
                                   const double *C_toom4);

#endif
