/* functions related to printing Ms, Rads and RMs inclusion graphs */
#include <stdlib.h>
#include <string.h>

#include "graph.h"

#define MAX_LABEL_LENGTH 512

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads) {
    unsigned long long i, j;

    fprintf(out, "digraph M_Rad_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tsplines=\"ortho\";\n");
    fprintf(out, "\tnodesep=\"3.0\";\n");
    fprintf(out, "\tranksep=\"0.35\";\n");


    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, 1000*numofMs);
    }

    /* label Ms chain; it is proven that only these equalities hold:
     *  M_pi(m, 0) = Rad^(nilindex - 1)
     *  M_pi(m, numofMs - 2) = Rad^1 = Rad
     *  M_pi(m, numofMs - 1) = Rad^0 = QH
     */
    fprintf(out, "\tM_%llu_%lu_%i [label = \"M_%llu(%lu,%i) = Rad^%llu\"];\n", pi, m, 0, pi, m, 0, nilindex - 1);
    for (i = 1; i < numofMs - 2; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", pi, m, i, pi, m, i);
    }
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%i\"];\n", pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%i\"];\n", pi, m, numofMs - 1, pi, m, numofMs - 1, 0);


    /* construct Rads (except mentioned above) chain and
     * embed Rads chain into appropriate place in Ms chain */
    fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%i;\n", pi, m, numofMs - 2, 2);
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu [weight = %llu];\n", i, i + 1, 1000*nilindex);
    }
    fprintf(out, "\tRad_%llu -> M_%llu_%lu_%i;\n", nilindex - 2, pi, m, 0);

    /* label them */
    for (i = 2; i < nilindex - 1; ++i) {
        fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
    }


    /* carcass is ready, fill it with inclusion links */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");
    for (i = 1; i < numofMs - 2; ++i) {
        for (j = 2; j < nilindex - 1; ++j) {
            if (ideal_issubset(Rads[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%llu;\n", pi, m, i, j);
                /* if M_pi[i] > Rad^j, then M_pi[i] > Rad^j > Rad^(j+1) > ... */
                break;
            }
        }

        for (j = nilindex - 2; j >= 2; --j) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu;\n", j, pi, m, i);
                /* if Rad^j > M_pi[i], then ... > Rad^(j-1) > Rad^j > M_pi[i] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

int append_to_label(char** label, char* s) {
    if (s == NULL)
        return 1;

    char* p;

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

void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs) {
    unsigned long long i, j;
    unsigned char was_collision = 0;

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
    fprintf(out, "\tsplines=\"ortho\";\n");
    fprintf(out, "\tnodesep=\"3.0\";\n");
    fprintf(out, "\tranksep=\"0.35\";\n");


    /* label RMs taking collisions into account */
    for (i = 0; i < numofMs; ++i) {
        was_collision = 0;
        if (RMs[i] == NULL)
            continue;

        for (j = i + 1; j < numofMs; ++j) {
            if (RMs[j] == NULL) {
                continue;
            } else if (ideal_isequal(RMs[i], RMs[j])) {
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

    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, 1000*numofMs);
    }

    /* label Ms chain taking collisions with RMs into account */
    for (i = 0; i < numofMs; ++i) {
        was_collision = 0;

        for (j = 1; j < numofMs; ++j) {
            if (RMs[j] == NULL) {
                continue;
            } else if (ideal_isequal(Ms[i], RMs[j])) {
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

    /* construct RMs chain */
    for (i = 0; i < numofMs; ++i) {
        if (RMs[i] == NULL)
            continue;

        for (j = i + 1; j < numofMs; ++j) {
            if (RMs[j] == NULL) {
                continue;
            } else {
                fprintf(out, "\tRM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, j, pi, m, i);
                /* if j > i, then ... >= RM_pi[j+1] >= RM_pi[j] > RM_pi[i] */
                break;
            }
        }
    }

    /* label RMs chain */
    for (i = 0; i < numofMs; ++i) {
        if (RMs[i] == NULL)
            continue;

        fprintf(out, "\tRM_%llu_%lu_%llu [label = \"%s\"];\n", \
                pi, m, i, labels[i]);
    }


    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each RM create all needed links with Ms and RMs */
    for (j = 0; j < numofMs; ++j) {
        if (RMs[j] == NULL)
            continue;

        /* create all needed links with Ms */
        /* M_pi(m,0) == Rad*M_pi(m,1) */
        /* M_pi(m, numofMs - 2) == Rad*M_pi(m, numofMs - 1) == Rad*QH == Rad */
        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(RMs[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, i, pi, m, j);
                /* if M_pi[i] > RM_pi[j], then ... > M_pi[i+1] > M_pi[i] > RM_pi[j] */
                break;
            }
        }

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], RMs[j])) {
                fprintf(out, "\tRM_%llu_%lu_%llu -> M_%llu_%lu_%llu;\n", pi, m, j, pi, m, i);
                /* if RM_pi[j] > M_pi[i], then RM_pi[j] > M_pi[i] > M_pi[i-1] > ... */
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
