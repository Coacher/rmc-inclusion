/* M_pi(m,k) ideals and related functions */
#ifndef _IDEALS_H
#define _IDEALS_H

#ifdef ENABLE_GMP
#include <gmp.h>
#endif

#include "common.h"
#include "log.h"

/*
 * This structure represents a M_pi(m, k) ideal in a group algebra QH,
 * where Q is a finite field of characteristic p and order q = p^l
 * and H is a group isomorphic to the additive group of the said field (Q, +).
 *
 * In this representation we rely on the fact that these ideals
 * are defined as sums of Qu_s. u_s elements, where 0 <= s <= q - 1,
 * are a basis of QH. See [0] for more details.
 *
 * [0]: E. Couselo, S. Gonzalez, V. Markov, C. Martinez, A. Nechaev,
 * “Ideal representation of Reed-Solomon and Reed-Muller codes”,
 * Algebra and Logic, 51, 3 (2012), 195-212.
 *
 */
typedef struct IDEAL_t {
    /* parameters of the ideal */
    unsigned long long q;
    unsigned long long pi;
    unsigned int m;
    unsigned long long k;
    /* `bit` array of length q representing Qu_i summands that form the ideal */
    unsigned char* u_s;
} IDEAL;

IDEAL* ideal_create(unsigned long long q);
int ideal_init(IDEAL* M,
        unsigned long long pi, unsigned int m, unsigned long long k);
void ideal_free(IDEAL* M);

#ifdef ENABLE_GMP
/* returns a Q-dimension of M_pi(m,k) */
void m_k(mpz_t rop,
        unsigned long long pi, unsigned int m, unsigned long long k);
#endif

/* returns a non-zero value iff M is equal to N */
int ideal_isequal(IDEAL* M, IDEAL* N);
/* returns a non-zero value iff M is a subset of N */
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
 * works more like res += M * N */
int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N,
        unsigned int p);

/* prints M->u_s as an array of bits */
void ideal_print(IDEAL* M);
/* prints M->u_s in a verbose way */
void ideal_print_verbose(IDEAL* M);

/*
 * We use the notation from [1] below.
 *
 * [1]: unfortunately, this article is still being prepared for printing
 *      feel free to contact me directly if you need any help
 *      you can find my e-mail in README file
 *
 */

/* returns a minimal k such that P_j \subset \Pi_k for the given j */
unsigned long long minimum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m, unsigned int lambda);

/* returns a maximal k such that \Pi_k \subset P_j for the given j */
unsigned long long maximum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m);

/* returns a minimal j such that \Pi_k \subset P_j for the given k */
/* uses lookup method */
unsigned long long minimum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m);

/* returns a maximal j such that P_j \subset \Pi_k for the given k */
/* uses lookup method */
/* upper_bound is the index of the biggest M_pi ideal (which is QH)
 * i.e. upper_bound = numofMs - 1 */
unsigned long long maximum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m,
        unsigned int l, unsigned int lambda,
        unsigned long long upper_bound);
#endif
