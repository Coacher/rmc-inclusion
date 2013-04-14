/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

#include <gmp.h>

/* simple double-and-add integer pow */
unsigned long long pow_ul(unsigned long base, unsigned long power);

/* calculates (pi)-adic weight of x */
unsigned long weight(unsigned long long x, unsigned long pi);

/* binomial coefficient \binom{n}{m}
 * this version handles negative inputs */
void bin_coeff(mpz_t rop, long long n, long long m);
#endif
