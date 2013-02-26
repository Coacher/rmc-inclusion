/* code for M_pi(m,k) ideals and related functions */
#ifndef _IDEALS_H
#define _IDEALS_H

#include "common.h"

/*
 * Structure representing M_pi(m, k) ideal in a group algebra QH,
 * where Q is a finite field with characteristic p and size q = p^l
 * and H is a group isomorphic to (Q, +) - additive group of field Q.
 * It is known (see [0]), that such ideals are Reed-Muller codes
 * when Q is a prime field.
 * In this representation we heavily use the fact that these ideals
 * are composed from basis elements of a special form -
 * left principal ideals Qu_i's (see [1]).
 *
 * [0]: E.F. Assmus Jr., J.D. Key, “Polynomial Codes and Finite Geometries”,
 * Handbook of Coding Theory, Vera S. Pless and W. Cary Huffman (eds.),
 * Elsevier (1998), 1269-1343.
 *
 * [1]: E. Couselo, S. Gonzalez, V. Markov, C. Martinez, A. Nechaev,
 * “Ideal representation of Reed-Solomon and Reed-Muller codes”,
 * Algebra and Logic, 51, 3 (2012), 195-212.
 */
typedef struct IDEAL_t {
    /* parameters of ideal */
    unsigned long long q;
    unsigned long pi;
    unsigned long m;
    unsigned long k;
    /* string with 'coordinates' of a given ideal in a basis of Qu_i's */
    unsigned char* u_s;
} IDEAL;

IDEAL* ideal_create(unsigned long long q);
int ideal_init(IDEAL* M, unsigned long pi, unsigned long m, unsigned long k);
void ideal_free(IDEAL* M);

/* returns non-zero when M is equal to N */
int ideal_isequal(IDEAL* M, IDEAL* N);
/* returns non-zero when M is a subset of N */
int ideal_issubset(IDEAL* M, IDEAL* N);
/* sets res to M * (Qu_t) */
int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long long t, unsigned long p);
/* sets res to M * N */
int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N, unsigned long p);
/* prints out set of u_s */
void ideal_print(IDEAL* M);
#endif
