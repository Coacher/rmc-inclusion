/* functions related to printing M_pi's, Rads and Rad*M_pi's structure info */
#ifndef _INFO_H
#define _INFO_H

#include <stdio.h>

#include "common.h"
#include "ideals.h"

/* print out all needed info */
void print_info(FILE* out, IDEAL** Ms, IDEAL** Rads, IDEAL** RMs);
#endif
