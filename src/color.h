/* functions for colored output */
#ifndef _COLOR_H
#define _COLOR_H

#include "rmc/ideals.h"

/* colored printf
 * first argument specifies color, the rest is any arguments acceptable by printf */
int color_printf(unsigned int color, const char *fmt, ...);

/* colored ideal_print with conditional function
 * first argument specifies color, the second one specifies ideal
 * and the third argument specifies the condition function.
 * colored printing happens when conditional function applied to
 * index returns non-zero. */
void color_ideal_print(unsigned int color, IDEAL* M, \
        int (*condition)(unsigned long long));

/* colored ideal_print_verbose with conditional function
 * first argument specifies color, the second one specifies ideal
 * and the third argument specifies the condition function.
 * colored printing happens when conditional function applied to
 * index returns non-zero. */
void color_ideal_print_verbose(unsigned int color, IDEAL* M, \
        int (*condition)(unsigned long long));
#endif
