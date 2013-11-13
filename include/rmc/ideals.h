/* code for M_pi(m,k) ideals and related functions */
#ifndef _IDEALS_H
#define _IDEALS_H

#ifdef ENABLE_GMP
#include <gmp.h>
#endif

#include "common.h"
#include "log.h"

/*
 * Structure representing a M_pi(m, k) ideal in a group algebra QH,
 * where Q is a finite field of characteristic p and order q = p^l
 * and H is a group isomorphic to (Q, +) - additive group of the field Q.
 *
 * In this representation we rely on the fact that these ideals
 * are composed from basis elements of a special form -
 * left principal ideals Qu_i's. See [0] for more details.
 *
 * [0]: E. Couselo, S. Gonzalez, V. Markov, C. Martinez, A. Nechaev,
 * “Ideal representation of Reed-Solomon and Reed-Muller codes”,
 * Algebra and Logic, 51, 3 (2012), 195-212.
 *
 */
typedef struct IDEAL_t {
    /* parameters of ideal */
    unsigned long long q;
    unsigned long long pi;
    unsigned int m;
    unsigned long long k;
    /* string with 'coordinates' of a given ideal in the basis of Qu_i's */
    unsigned char* u_s;
} IDEAL;

IDEAL* ideal_create(unsigned long long q);
int ideal_init(IDEAL* M,
        unsigned long long pi, unsigned int m, unsigned long long k);
void ideal_free(IDEAL* M);

#ifdef ENABLE_GMP
/* returns Q-dimension of M_pi(m,k) */
void m_k(mpz_t rop,
        unsigned long long pi, unsigned int m, unsigned long long k);
#endif

/* returns non-zero when M is equal to N */
int ideal_isequal(IDEAL* M, IDEAL* N);
/* returns non-zero when M is a subset of N */
int ideal_issubset(IDEAL* M, IDEAL* N);
/* sets res to M \ N. N must be a subset of M */
int ideal_diff(IDEAL* res, IDEAL* M, IDEAL* N);
/* sets res to M \cap N. */
int ideal_intersect(IDEAL* res, IDEAL* M, IDEAL* N);

/* sets res to M * (Qu_j) */
/* NOTE: res->u_s must be filled with zeros as this function
 * works more like res += M * (Qu_j) */
int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long long j,
        unsigned int p);
/* sets res to M * N */
/* NOTE: res->u_s must be filled with zeros as this function
 * works more like res += M * (Qu_j) */
int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N,
        unsigned int p);

/* prints out the set of u_s which form the M as an array of bits */
void ideal_print(IDEAL* M);
/* prints out the set of u_s which form the M in a verbose way*/
void ideal_print_verbose(IDEAL* M);

/*
 * We use notation from [1] below.
 *
 * [1]: unfortunately this article is still being prepared for printing
 *      feel free to contact me directly if you need any help
 *      you can find my e-mail in README file
 *
 */

/* returns for the given j a minimum k such that P_j \subset \Pi_k */
unsigned long long minimum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m, unsigned int lambda);

/* returns for the given j a maximum k such that \Pi_k \subset P_j */
unsigned long long maximum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m);

/* returns for the given k a minimum j such that \Pi_k \subset P_j */
/* uses lookup method */
unsigned long long minimum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m);

/* returns for the given k a maximum j such that P_j \subset \Pi_k */
/* uses lookup method */
/* upper_bound is an index of the biggest M_pi ideal (which is QH)
 * i.e. upper_bound = numofMs - 1 */
unsigned long long maximum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m,
        unsigned int l, unsigned int lambda,
        unsigned long long upper_bound);
#endif
