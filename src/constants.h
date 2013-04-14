/* needed GLOBAL variables */
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include "rmc/common.h"

/*
 * constants that define our behaviour:
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
 *
 */

unsigned long p, l, lambda, m;
unsigned long long q, pi;
unsigned long long numofMs, nilindex;

/* init the rest of constants according to already set values of p, l, lambda */
void init_constants(void);
#endif
