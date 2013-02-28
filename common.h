/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

#include <gmp.h>

/* constants that define our behaviour:
 *  p - prime integer
 *  l - any positive integer
 *  lambda - any divisor of l, except 1
 *
 * other constants:
 *  q  = p^l
 *  pi = p^lambda
 *  m = l / lambda
 *  numofMs - number of M_pi ideals
 *  nilindex - number of M_p ideals (radical powers)
 */
unsigned long p, l, lambda, m;
unsigned long long q, pi;
unsigned long long numofMs, nilindex;

/* simple double-and-add integer pow */
unsigned long long pow_ul(unsigned long base, unsigned long power);

/* init the rest of constants according to given p, l, lambda */
void init_constants(unsigned long p, unsigned long l, unsigned long lambda);

/* calculates (pi)-adic weight of x */
unsigned long weight(unsigned long long x, unsigned long pi);

/* binomial coefficient \binom{n}{m} */
void bin_coeff(mpz_t rop, long long n, long long m);

/* Q-dimension of M_pi(m,k) */
void m_k(mpz_t rop, unsigned long long pi, unsigned long m, unsigned long k);
#endif
