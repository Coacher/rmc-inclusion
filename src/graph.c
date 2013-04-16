/* functions related to printing Ms/Rads and Ms/RMs inclusion graphs */
#include <stdlib.h>
#include <string.h>

#include "graph.h"

#define MAX_LABEL_LENGTH 512

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads, \
        unsigned int m_weight, unsigned int r_weight, unsigned int o_weight, \
        unsigned int use_groups) {
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
    if (use_groups) {
        fprintf(out, "\tM_%llu_%lu_%u [label = \"M_%llu(%lu,%u) = Rad^%llu\", group = \"Ms\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        for (i = 1; i < numofMs - 2; ++i) {
            fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\", group = \"Ms\"];\n", \
                    pi, m, i, pi, m, i);
        }
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    } else {
        fprintf(out, "\tM_%llu_%lu_%u [label = \"M_%llu(%lu,%u) = Rad^%llu\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        for (i = 1; i < numofMs - 2; ++i) {
            fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", \
                    pi, m, i, pi, m, i);
        }
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    }

    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                pi, m, i, pi, m, i - 1, m_weight*numofMs);
    }


    /* label Rads chain (except for those in equalities mentioned above) */
    for (i = 2; i < nilindex - 1; ++i) {
        if (use_groups) {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\", group = \"Rads\"];\n", i, i);
        } else {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
        }
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
                fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%llu [weight = %llu];\n", \
                        pi, m, i, j, o_weight*nilindex);
                /* if M_pi[i] > Rad^j, then M_pi[i] > Rad^j > Rad^(j+1) > ... */
                break;
            }
        }

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                        j, pi, m, i, o_weight*nilindex);
                /* if Rad^j > M_pi[i], then ... > Rad^(j-1) > Rad^j > M_pi[i] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

static int append_to_label(char** label, char* s) {
    char* pp;

    if (s == NULL)
        return 1;

    if ((*label) == NULL) {
        pp = (char*) malloc((strlen(s) + 1)*sizeof(char));
        strcpy(pp, s);
        *label = pp;
    } else {
        /* pp must have place for a previous label,
         * additional part of label and terminating `\0` */
        pp = (char*) malloc((strlen(*label) + strlen(s) + 1)*sizeof(char));
        sprintf(pp, "%s%s", (*label), s);

        free(*label);
        *label = pp;
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

                    sprintf(buf, "Rad*M_%llu(%lu,%llu) = Rad*M_%llu(%lu,%llu)", \
                            pi, m, i, pi, m, j);
                    append_to_label(labels + i, buf);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                } else {
                    sprintf(buf, " = Rad*M_%llu(%lu,%llu)", pi, m, j);
                    append_to_label(labels + i, buf);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                }
            }
        }

        if (!was_collision) {
            sprintf(buf, "Rad*M_%llu(%lu,%llu)", pi, m, i);
            append_to_label(labels + i, buf);
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
            fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", \
                    pi, m, i, pi, m, i);
    }

    /* contrsuct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                pi, m, i, pi, m, i - 1, m_weight*numofMs);
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

void print_graph_beautiful(FILE* out, \
        unsigned int m_weight, unsigned int r_weight, unsigned int o_weight,
        unsigned int use_groups) {
    unsigned long long i, j, previous;

    /* arrays to store all needed values of k */
    unsigned long long *Mpi_to_Rad;
    unsigned long long *Rad_to_Mpi;

    Mpi_to_Rad = (unsigned long long*) malloc(nilindex*sizeof(unsigned long long));
    Rad_to_Mpi = (unsigned long long*) malloc(nilindex*sizeof(unsigned long long));

    if (Mpi_to_Rad == NULL || Rad_to_Mpi == NULL) {
        fprintf(stderr, "Unable to allocate memory for arrays of k.\n");
        return;
    }


    fprintf(out, "digraph M_Rad_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tgraph [nodesep=\"3.0\",\n");
    fprintf(out, "\t\tranksep=\"0.35\",\n");
    fprintf(out, "\t\tsplines=ortho\n");
    fprintf(out, "\t];\n");


    /* label Ms/RMs equalities; it is proven that only these equalities hold:
     *  M_pi(m, 0) == Rad^(nilindex - 1)
     *  M_pi(m, numofMs - 2) == Rad^1 == Rad
     *  M_pi(m, numofMs - 1) == Rad^0 == QH
     */
    if (use_groups) {
        fprintf(out, "\tM_%llu_%lu_%u [label = \"M_%llu(%lu,%u) = Rad^%llu\", group = \"Ms\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    } else {
        fprintf(out, "\tM_%llu_%lu_%u [label = \"M_%llu(%lu,%u) = Rad^%llu\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    }
    fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, numofMs - 1, pi, m, numofMs - 2, m_weight*numofMs);


    /* label Rads chain (except for those in equalities mentioned above) */
    for (i = 2; i < nilindex - 1; ++i) {
        if (use_groups) {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\", group = \"Rads\"];\n", i, i);
        } else {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
        }
    }

    /* construct Rads chain (except for those in equalities mentioned above) */
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu [weight = %llu];\n", i, i + 1, r_weight*nilindex);
    }


    /* fill, label and link Mpi_to_Rad array */
    for (i = 2; i < nilindex - 1; ++i) {
        Mpi_to_Rad[i] = minimum_Pi_for_P(l*(p - 1) - i, p, m, lambda);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\", group = \"Ms\"];\n", pi, m, Mpi_to_Rad[i], pi, m, Mpi_to_Rad[i]);
        fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%llu [weight = %llu];\n", pi, m, Mpi_to_Rad[i], i, o_weight*nilindex);
    }

    /* fill, label and link Rad_to_Mpi array */
    for (i = 2; i < nilindex - 1; ++i) {
        Rad_to_Mpi[i] = max_minimum_P_for_Pi(l*(p - 1) - i, p, m);
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\", group = \"Ms\"];\n", pi, m, Rad_to_Mpi[i], pi, m, Rad_to_Mpi[i]);
        fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", i, pi, m, Rad_to_Mpi[i], o_weight*nilindex);
    }


    /* carcass is ready, fill it with inclusion links */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* contrsuct Ms chain */
    i = j = nilindex - 2;
    previous = 0;
    while(i >= 2 && j >= 2) {
        if (Rad_to_Mpi[i] < Mpi_to_Rad[j]) {
            /*
             * we don't need to check whether previous == Rad_to_Mpi[i] here
             * assume Rad_to_Mpi[i] == previous and we have (i,j) values of indexes
             * we can get into (i,j) state in 3 ways:
             * 1. (i+1, j+1) --> (i,j)
             *  then Rad_to_Mpi[i + 1] == Mpi_to_Rad[j + 1] == previous,
             *  therefore Rad_to_Mpi[i] == Rad_to_Mpi[i + 1] which cannot be since
             *  all elements in Rad_to_Mpi array are different
             * 2. (i, j+1) --> (i,j)
             *  then previous == Mpi_to_Rad[j + 1] == Rad_to_Mpi[i] < Rad_to_Mpi[i]
             *  therefore we get contradiction
             * 3. (i+1, j) --> (i,j)
             *  then previous == Rad_to_Mpi[i+1] == Rad_to_Mpi[i] which again
             *  cannot be as all elements in Rad_to_Mpi array are different
             */
            if (previous + 1 == Rad_to_Mpi[i]) {
                fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            } else {
                fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu, style = \"dashed\"];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            }
            previous = Rad_to_Mpi[i];
            --i;
        } else if (Rad_to_Mpi[i] > Mpi_to_Rad[j]) {
            /* if previous == Rad_to_Mpi[j] then we don't want to create
             * Mpi_to_Rad[j] -> Rad_to_Mpi[j] link as it will duplicate
             * already created chain Mpi_to_Rad[j] -> Rad_j -> Rad_to_Mpi[j] */
            if (previous != Rad_to_Mpi[j]) {
                if (previous + 1 == Mpi_to_Rad[j]) {
                    fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                } else {
                    fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu, style = \"dashed\"];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                }
            }
            previous = Mpi_to_Rad[j];
            --j;
        } else {
            /* it is possible to have Rad_to_Mpi[i] == Mpi_to_Rad[j] for some i and j
             * however, all elements in Rad_to_Mpi array are different as well as in Mpi_to_Rad array
             * so, it is impossible to have Rad_to_Mpi[i] == Mpi_to_Rad[j] == previous */
            if (previous + 1 == Rad_to_Mpi[i]) {
                fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            } else {
                fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu, style = \"dashed\"];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            }
            previous = Rad_to_Mpi[i];
            --i;
            --j;
        }
    }

    /* obviously Mpi_to_Rad[t] > Rad_to_Mpi[t] for any t
     * so, in previous while we won't get situation where i > 2 and j == 2 */
    for (; j >= 2; --j) {
        if (previous + 1 == Mpi_to_Rad[j]) {
            fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", \
                    pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
        } else {
            fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu, style = \"dashed\"];\n", \
                    pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
        }
        previous = Mpi_to_Rad[j];
    }

    /* Mpi_to_Rad[2] is never equal to M_pi(m, numofMs - 2) == Rad because
     * for any j < l(p - 1) and k such that k is minimum such that P_j \susbet \Pi_k
     * there exists t such that weight(t, p) == j, weight(t, pi) == k and
     * weight(t + 1, p) == j + 1, weight(t + 1, pi) == k + 1.
     * Therefore if Mpi_to_Rad[2] == M_pi(m, numofMs - 2) then there exists
     * t such that weight(t, p) == l(p - 1) - 2, weight(t, pi) == numofMs - 2
     * and weight(t + 1, p) = l(p - 1) - 1, weight(t + 1, pi) == numofMs - 1.
     * As Rad == M_pi(m, numofMs - 2) and weight(t + 1, p) == l(p - 1) - 1
     * it follows that weight(t + 1, pi) <= numofMs - 2,
     * which contradicts weight(t + 1, pi) == numofMs - 1 */
    fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu, style = \"dashed\"];\n", \
            pi, m, numofMs - 2, pi, m, previous, m_weight*numofMs);

    fprintf(out, "}\n");

    free(Rad_to_Mpi);
    free(Mpi_to_Rad);
}
