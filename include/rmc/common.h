/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

#ifdef ENABLE_GMP
#include <gmp.h>
#endif

/* simple double-and-add integer pow */
unsigned long long pow_ull(unsigned int base, unsigned int power);

/* returns the (pi)-adic weight of x */
unsigned long long weight(unsigned long long x, unsigned long long pi);

#ifdef ENABLE_GMP
/* binomial coefficient \binom{n}{m}
 * this version handles negative inputs */
void bin_coeff(mpz_t rop, long long n, long long m);
#endif

#endif
