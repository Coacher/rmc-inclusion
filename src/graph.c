/* functions related to printing Ms, Rads and RMs inclusion graphs */
#include <stdlib.h>
#include <string.h>

#include "graph.h"

#define MAX_LABEL_LENGTH 512

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads, unsigned int m_weight, unsigned int r_weight) {
    unsigned long long i, j;

    fprintf(out, "digraph M_Rad_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tgraph [nodesep=\"3.0\",\n");
    fprintf(out, "\t\tranksep=\"0.35\",\n");
    fprintf(out, "\t\tsplines=ortho\n");
    fprintf(out, "\t];\n");


    /* label Ms chain; it is proven that only these equalities hold:
     *  M_pi(m, 0) == Rad^(nilindex - 1)
     *  M_pi(m, numofMs - 2) == Rad^1 == Rad
     *  M_pi(m, numofMs - 1) == Rad^0 == QH
     */
    fprintf(out, "\tM_%llu_%lu_%u [label = \"M_%llu(%lu,%u) = Rad^%llu\"];\n", pi, m, 0, pi, m, 0, nilindex - 1);
    for (i = 1; i < numofMs - 2; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", pi, m, i, pi, m, i);
    }
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", pi, m, numofMs - 1, pi, m, numofMs - 1, 0);

    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, m_weight*numofMs);
    }


    /* label Rads chain (except for those in equalities mentioned above) */
    for (i = 2; i < nilindex - 1; ++i) {
        fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
    }

    /* construct Rads chain (except for those in equalities mentioned above) */
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu [weight = %llu];\n", i, i + 1, r_weight*nilindex);
    }


    /* carcass is ready, fill it with inclusion links */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each Rad create all needed links between Rad and Ms */
    for (j = 2; j < nilindex - 1; ++j) {

        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(Rads[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%llu;\n", pi, m, i, j);
                /* if M_pi[i] > Rad^j, then M_pi[i] > Rad^j > Rad^(j+1) > ... */
                break;
            }
        }

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu;\n", j, pi, m, i);
                /* if Rad^j > M_pi[i], then ... > Rad^(j-1) > Rad^j > M_pi[i] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

static int append_to_label(char** label, char* s) {
    char* p;

    if (s == NULL)
        return 1;

    if ((*label) == NULL) {
        p = (char*) malloc((strlen(s) + 1)*sizeof(char));
        strcpy(p, s);
        *label = p;
    } else {
        /* p must have place for a previous label,
         * additional part of label and terminating `\0` */
        p = (char*) malloc((strlen(*label) + strlen(s) + 1)*sizeof(char));
        sprintf(p, "%s%s", (*label), s);

        free(*label);
        *label = p;
    }

    return 0;
}

void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs, unsigned int m_weight) {
    unsigned long long i, j;
    unsigned char was_collision = 0;
    unsigned long long previous;

    char buf[MAX_LABEL_LENGTH];
    char** labels;

    labels = (char**) malloc(numofMs*sizeof(char*));
    if (labels == NULL) {
        fprintf(stderr, "Unable to allocate memory for labels.\n");
        return;
    }

    for (i = 0; i < numofMs; ++i)
        labels[i] = NULL;


    fprintf(out, "digraph M_RM_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tgraph [nodesep=\"3.0\",\n");
    fprintf(out, "\t\tranksep=\"0.35\",\n");
    fprintf(out, "\t\tsplines=ortho\n");
    fprintf(out, "\t];\n");


    /* label RMs taking collisions into account */
    for (i = 0; i < numofMs; ++i) {
        if (RMs[i] == NULL)
            continue;

        was_collision = 0;
        for (j = i + 1; j < numofMs; ++j) {
            if (RMs[j] != NULL && ideal_isequal(RMs[i], RMs[j])) {
                if (!was_collision) {
                    was_collision = 1;

                    sprintf(buf, "Rad*M_%llu(%lu,%llu) = Rad*M_%llu(%lu,%llu)", pi, m, i, pi, m, j);
                    append_to_label(labels + i, buf);
                    dbg_msg_l(3, "%llu'th label: \"%s\"\n", i, labels[i]);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                } else {
                    sprintf(buf, " = Rad*M_%llu(%lu,%llu)", pi, m, j);
                    append_to_label(labels + i, buf);
                    dbg_msg_l(3, "%llu'th label: \"%s\"\n", i, labels[i]);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                }
            }
        }

        if (!was_collision) {
            sprintf(buf, "Rad*M_%llu(%lu,%llu)", pi, m, i);
            append_to_label(labels + i, buf);
            dbg_msg_l(3, "%llu'th label: \"%s\"\n", i, labels[i]);
        }
    }

    /* label Ms chain taking collisions with RMs into account */
    for (i = 0; i < numofMs; ++i) {
        was_collision = 0;

        for (j = 1; j < numofMs; ++j) {
            if (RMs[j] != NULL && ideal_isequal(Ms[i], RMs[j])) {
                was_collision = 1;
                fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = %s\"];\n", \
                        pi, m, i, pi, m, i, labels[j]);

                ideal_free(RMs[j]);
                RMs[j] = NULL;
                break;
            }
        }

        if (!was_collision)
            fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", pi, m, i, pi, m, i);
    }

    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, m_weight*numofMs);
    }

    /* label RMs chain */
    for (i = 0; i < numofMs; ++i) {
        if (RMs[i] == NULL)
            continue;

        fprintf(out, "\tRM_%llu_%lu_%llu [label = \"%s\"];\n", \
                pi, m, i, labels[i]);
    }

    /* it is proven that M_pi(m,0) == Rad*M_pi(m,1)
     * and Rad*M_pi(m,0) is smaller than M_pi(m,0)
     * so it will not collide with either any of Ms or RMs
     * that's why it is safe to create this link here */
    fprintf(out, "\tM_%llu_%lu_%u -> RM_%llu_%lu_%u;\n", pi, m, 0, pi, m, 0);

    /* construct RMs chain */
    for (i = 1; i < numofMs; ++i) {
        if (RMs[i] == NULL)
            continue;

        for (j = i + 1; j < numofMs; ++j) {
            if (RMs[j] != NULL) {
                fprintf(out, "\tRM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, j, pi, m, i);
                /* if j > i, then ... >= RM_pi[j+1] >= RM_pi[j] > RM_pi[i],
                 * because always RM_pi[j] >= RM_pi[i]
                 * and we've already handled all collisions */
                break;
            }
        }
    }


    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each RM create all needed links between RM and Ms
     * things that were already handled:
     *  Rad*M_pi(m,0)
     *  M_pi(m,0) == Rad*M_pi(m,1)
     *  M_pi(m, numofMs - 2) == Rad*M_pi(m, numofMs - 1) == Rad*QH == Rad */
    previous = 0;
    for (j = 2; j < numofMs - 1; ++j) {
        if (RMs[j] == NULL)
            continue;

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], RMs[j])) {
                if (previous < i) {
                    fprintf(out, "\tRM_%llu_%lu_%llu -> M_%llu_%lu_%llu;\n", pi, m, j, pi, m, i);
                    previous = i;
                }
                /* if RM_pi[j] > M_pi[i], then RM_pi[j] > M_pi[i] > M_pi[i-1] > ... */
                break;
            }
        }
    }

    previous = numofMs - 2;
    for (j = numofMs - 2; j >= 2; --j) {
        if (RMs[j] == NULL)
            continue;

        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(RMs[j], Ms[i])) {
                if (previous > i) {
                    fprintf(out, "\tM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, i, pi, m, j);
                    previous = i;
                }
                /* if M_pi[i] > RM_pi[j], then ... > M_pi[i+1] > M_pi[i] > RM_pi[j] */
                break;
            }
        }
    }

    fprintf(out, "}\n");

    for (i = 0; i < numofMs; ++i) {
        if (labels[i])
            free(labels[i]);
    }
    free(labels);
}
