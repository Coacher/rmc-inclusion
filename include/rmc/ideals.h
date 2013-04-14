/* code for M_pi(m,k) ideals and related functions */
#ifndef _IDEALS_H
#define _IDEALS_H

#include "common.h"
#include "log.h"

/*
 * Structure representing M_pi(m, k) ideal in a group algebra QH,
 * where Q is a finite field with characteristic p and size q = p^l
 * and H is a group isomorphic to (Q, +) - additive group of field Q.
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
    unsigned long pi;
    unsigned long m;
    unsigned long k;
    /* string with 'coordinates' of a given ideal in the basis of Qu_i's */
    unsigned char* u_s;
} IDEAL;

IDEAL* ideal_create(unsigned long long q);
int ideal_init(IDEAL* M, unsigned long pi, unsigned long m, unsigned long k);
void ideal_free(IDEAL* M);

/* returns Q-dimension of M_pi(m,k) */
void m_k(mpz_t rop, unsigned long long pi, unsigned long m, unsigned long k);

/* returns non-zero when M is equal to N */
int ideal_isequal(IDEAL* M, IDEAL* N);
/* returns non-zero when M is a subset of N */
int ideal_issubset(IDEAL* M, IDEAL* N);
/* sets res to M \ N. N must be a subset of M */
int ideal_diff(IDEAL* res, IDEAL* M, IDEAL* N);

/* sets res to M * (Qu_j) */
int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long long j, unsigned long p);
/* sets res to M * N */
int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N, unsigned long p);

/* prints out the set of u_s which form the M */
void ideal_print(IDEAL* M);

/*
 * We use notation from [1] below.
 *
 * [1]: unfortunately this article is still prepared for printing
 *      feel free to contact me directly if you need any help
 *      you can find my e-mail in README file
 *
 */

/* returns for the given j a minimum k such that P_j \subset \Pi_k */
unsigned long long minimum_Pi_for_P(unsigned long long j, \
        unsigned long p, unsigned long m, unsigned long lambda);

/* for the given j there is a maximum k' among all those k
 * for which j is a minimum such that \Pi_k \subset P_j
 *
 * returns k' for the given j */
unsigned long long max_minimum_P_for_Pi(unsigned long long j, \
        unsigned long p, unsigned long m);
#endif
