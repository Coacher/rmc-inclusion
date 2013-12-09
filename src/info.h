/* functions for printing Ms, Rads and RadMs structure info */
#ifndef _INFO_H
#define _INFO_H

#include <stdio.h>

#include "rmc/ideals.h"
#include "rmc/constants.h"

/* print out all needed info */
void print_info(FILE* out, IDEAL** Ms, IDEAL** Rads, IDEAL** RadMs);
#endif
