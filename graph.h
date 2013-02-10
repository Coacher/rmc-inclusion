/* functions related to printing M_pi's, Rads and Rad*M_pi's inclusion graphs */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>

#include "common.h"
#include "ideals.h"

/* print out M_pi's and Rads inclusion graph */
void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads);

/* print out M_pi's and Rad*M_pi's inclusion graph */
void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs);
#endif
