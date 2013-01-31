/* a number of small useful functions */
#ifndef _COMMON_H
#define _COMMON_H

/* calculates (pi)-adic weight */
unsigned long weight(unsigned long x, unsigned long pi);

/* binomial coefficient */
unsigned long bin_coeff(unsigned long n, unsigned long m);

/* Q-dimension of M_pi(m,k) */
unsigned long m_k(unsigned long k, unsigned long pi, unsigned long m);
#endif
