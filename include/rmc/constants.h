/* needed GLOBAL variables */
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include "rmc/common.h"

/*
 * primary constants:
 *   p - any prime integer
 *   l - any integer such that l >= 2
 *   lambda - any positive divisor of l, but generally lambda != 1
 *
 * secondary constants:
 *   q  = p^l
 *   pi = p^lambda
 *   m  = l / lambda
 *   numofMs - the number of M_pi ideals
 *   nilindex - the number of M_p ideals (radical powers)
 *
 */

unsigned int p, l, lambda, m;
unsigned long long q, pi;
unsigned long long numofMs, nilindex;

/* init secondary constants according to the values of primary ones
 * primary constants must be already set prior to this function call */
void init_constants(void);
#endif
