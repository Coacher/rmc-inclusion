/* functions related to printing Ms, Rads and RMs inclusion graphs */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>

#include "common.h"
#include "ideals.h"

/* print out Ms and Rads inclusion graph */
void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads);

/* print out Ms and RMs inclusion graph */
void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs);
#endif
