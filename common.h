/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

/* simple double-and-add integer pow */
unsigned long pow_ul(unsigned long base, unsigned long power);

/* calculates (pi)-adic weight of x */
unsigned long weight(unsigned long x, unsigned long pi);

/* binomial coefficient \binom{n}{m} */
unsigned long bin_coeff(long n, long m);

/* Q-dimension of M_pi(m,k) */
unsigned long m_k(unsigned long pi, unsigned long m, unsigned long k);
#endif
