/* functions for colored output */
#include <stdio.h>
#include <stdarg.h>

#include "color.h"

int color_printf(unsigned int color, const char *fmt, ...) {
    int ret;
    va_list args;

    va_start(args, fmt);

    /* 0x1B = 27 - ASCII Escape code,
     * 0 - no special attributes (like bold or underlined),
     * color - foreground color */
    printf("%c[%d;%dm", 0x1B, 0, color);

    ret = vfprintf(stdout, fmt, args);

    /* disable coloring */
    printf("%c[%dm", 0x1B, 0);

    va_end(args);

    return ret;
}

void color_ideal_print(unsigned int color, IDEAL* M, \
        int (*condition)(unsigned long long)) {

    if (condition == NULL) {
        ideal_print(M);
        return;
    }

    /* internals of ideal_print */
    unsigned long long i;

    if (M == NULL)
        return;

    fprintf(stdout, "[");

    for (i = 0; i < (M->q - 1); ++i) {
        if (condition(i)) {
            color_printf(color, " %u", M->u_s[i]);
        } else {
            fprintf(stdout, " %u", M->u_s[i]);
        }
    }

    /* i = M->q - 1 */
    if (condition(i)) {
        color_printf(color, " %u ]\n", M->u_s[i]);
    } else {
        fprintf(stdout, " %u ]\n", M->u_s[i]);
    }
}

void color_ideal_print_verbose(unsigned int color, IDEAL* M, \
        int (*condition)(unsigned long long)) {

    if (condition == NULL) {
        ideal_print_verbose(M);
        return;
    }

    /* internals of ideal_print_verbose */
    unsigned long long i;

    if (M == NULL)
        return;

    for (i = 0; i < (M->q - 1); ++i) {
        if (M->u_s[i]) {
            if (condition(i)) {
                color_printf(color, " u_%llu", i);
            } else {
                fprintf(stdout, " u_%llu", i);
            }
        }
    }

    /* i = M->q - 1 */
    if (!M->u_s[i]) {
        /* only the whole group algebra contains u_(q-1) */
        fprintf(stdout, "\n");
    } else {
        if (condition(i)) {
            color_printf(color, " u_%llu\n", i);
        } else {
            fprintf(stdout, " u_%llu\n", i);
        }
    }
}
