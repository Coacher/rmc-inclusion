/* functions related to printing Ms/Rads and Ms/RMs inclusion graphs */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>

#include "rmc/ideals.h"
#include "constants.h"

/* prints out Ms and Rads inclusion graph */
void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads, unsigned int m_weight, unsigned int r_weight);

/* prints out Ms and RMs inclusion graph */
void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs, unsigned int m_weight);

/* prints out Ms and Rads beautiful inclusion graph
 *
 * this function doesn't require any ideals to be computed,
 * but uses theoretical results to print out only
 * meaningful part of inclusion graph */
void print_graph_beautiful(FILE* out, unsigned int m_weight, unsigned int r_weight, unsigned int o_weight);
#endif
