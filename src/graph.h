/* functions related to printing Ms/Rads and Ms/RMs inclusion graphs */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>

#include "rmc/log.h"
#include "rmc/common.h"
#include "rmc/ideals.h"
#include "constants.h"

/* prints out Ms and Rads inclusion graph */
void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads, unsigned int m_weight, unsigned int r_weight);

/* prints out Ms and RMs inclusion graph */
void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs, unsigned int m_weight);

/* prints out Ms and Rads beautiful inclusion graph
 *
 * this function doesn't even calculate any ideals,
 * but uses theoretical results to print out only
 * meaningful part of inclusion graph
 * therefore it is much faster and uses much less memory */
void print_graph_beautiful(FILE* out, unsigned int m_weight, unsigned int r_weight);
#endif
