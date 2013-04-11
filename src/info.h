/* functions related to printing Ms, Rads and RMs structure info */
#ifndef _INFO_H
#define _INFO_H

#include <stdio.h>

#include "rmc/common.h"
#include "rmc/ideals.h"
#include "constants.h"

/* print out all needed info */
void print_info(FILE* out, IDEAL** Ms, IDEAL** Rads, IDEAL** RMs);
#endif
