/* functions for printing Ms/Rads and Ms/RadMs inclusion graphs */
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>

#include "rmc/ideals.h"
#include "rmc/constants.h"

/* prints out Ms and Rads inclusion graph */
void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads,
        unsigned int m_weight, unsigned int r_weight, unsigned int o_weight,
        unsigned int use_groups);

/* prints out Ms and RadMs inclusion graph */
void print_radm_graph(FILE* out, IDEAL** Ms, IDEAL** RadMs,
        unsigned int m_weight);

/* prints out beautiful Ms and Rads inclusion graph
 *
 * this function doesn't require any ideals to be computed,
 * but uses theoretical results to print out only
 * the meaningful part of inclusion graph */
void print_graph_beautiful(FILE* out,
        unsigned int m_weight, unsigned int r_weight, unsigned int o_weight,
        unsigned int use_groups);
#endif
