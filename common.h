/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

#include <gmp.h>

/* simple double-and-add integer pow */
unsigned long long pow_ul(unsigned long base, unsigned long power);

/* calculates (pi)-adic weight of x */
unsigned long weight(unsigned long long x, unsigned long pi);

/* binomial coefficient \binom{n}{m} */
void bin_coeff(mpz_t rop, long n, long m);

/* Q-dimension of M_pi(m,k) */
void m_k(mpz_t rop, unsigned long pi, unsigned long m, unsigned long k);
#endif
