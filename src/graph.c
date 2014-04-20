/* functions for printing Ms/Rads and Ms/RadMs inclusion graphs */
#include <stdlib.h>
#include <string.h>

#include "graph.h"

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads,
        unsigned int m_weight, unsigned int r_weight, unsigned int o_weight,
        unsigned int use_groups) {

    unsigned long long i, j;


    fprintf(out, "digraph M_Rad_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tgraph [nodesep=\"3.0\",\n");
    fprintf(out, "\t\tranksep=\"0.35\",\n");
    fprintf(out, "\t\tsplines=ortho\n");
    fprintf(out, "\t];\n");


    /* label Ms chain
     * it is proven that only these equalities hold:
     *   M_pi(m, 0) == Rad^(nilindex - 1)
     *   M_pi(m, numofMs - 2) == Rad^1 == Rad
     *   M_pi(m, numofMs - 1) == Rad^0 == QH */
    if (use_groups) {
        fprintf(out, "\tM_%llu_%u_%u [label = \"M_%llu(%u,%u) = Rad^%llu\", group = \"Ms\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        for (i = 1; i < numofMs - 2; ++i) {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\", group = \"Ms\"];\n", \
                    pi, m, i, pi, m, i);
        }
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    } else {
        fprintf(out, "\tM_%llu_%u_%u [label = \"M_%llu(%u,%u) = Rad^%llu\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        for (i = 1; i < numofMs - 2; ++i) {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
                    pi, m, i, pi, m, i);
        }
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    }

    /* construct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                pi, m, i, pi, m, i - 1, m_weight*numofMs);
    }


    /* label Rads chain (except for those in the equalities mentioned above) */
    for (i = 2; i < nilindex - 1; ++i) {
        if (use_groups) {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\", group = \"Rads\"];\n", i, i);
        } else {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
        }
    }

    /* construct Rads chain (except for those in the equalities mentioned above) */
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu [weight = %llu];\n", i, i + 1, r_weight*nilindex);
    }


    /* carcass is ready, fill it with inclusion arcs */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each Rad create all needed arcs between Rad and Ms */
    for (j = 2; j < nilindex - 1; ++j) {

        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(Rads[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%u_%llu -> Rad_%llu [weight = %llu];\n", \
                        pi, m, i, j, o_weight*nilindex);
                /* if M_pi[i] > Rad^j, then M_pi[i] > Rad^j > Rad^(j+1) > ... */
                break;
            }
        }

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                        j, pi, m, i, o_weight*nilindex);
                /* if Rad^j > M_pi[i], then ... > Rad^(j-1) > Rad^j > M_pi[i] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

void print_radm_graph(FILE* out, IDEAL** Ms, IDEAL** RadMs,
        unsigned int m_weight) {

    unsigned long long i, j;
    unsigned char equality = 0;
    unsigned long long previous;


    fprintf(out, "digraph M_RadM_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tgraph [nodesep=\"3.0\",\n");
    fprintf(out, "\t\tranksep=\"0.35\",\n");
    fprintf(out, "\t\tsplines=ortho\n");
    fprintf(out, "\t];\n");


    /* label Ms chain taking equalities with RadMs into account */
    /* It is proven that all RadMs are different when lambda != l, i.e. m != 1.
     * It is proven that only the equalities Rad*M_pi(m,k + 1) == Rad*M_pi(m,k),
     * where k = (p - 1) mod p, hold when lambda == l, i.e. m == 1. */
    /* We always have that Rad == Rad*M_pi(m,numofMs-1) is a subset of M_pi(m,numofMs-1),
     * since the radical is not the whole group algebra. */
    /* We also always have that Rad == Rad*M_pi(m,numofMs-1) == M_pi(m,numofMs-2). */
    /* We also always have that Rad == Rad*M_pi(m,numofMs-1) != Rad*M_pi(m,numofMs-2) == Rad^2,
     * since nilindex >= 3 */
    fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
            pi, m, numofMs - 1, pi, m, numofMs - 1);
    fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu)\"];\n", \
            pi, m, numofMs - 2, pi, m, numofMs - 2, pi, m, numofMs - 1);
    ideal_free(RadMs[numofMs - 1]);
    RadMs[numofMs - 1] = NULL;

    if (m != 1) {
        for (i = 0; i < numofMs - 2; ++i) {
            equality = 0;

            for (j = 1; j < numofMs - 1; ++j) {
                if (ideal_isequal(Ms[i], RadMs[j])) {
                    equality = 1;
                    fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu)\"];\n", \
                            pi, m, i, pi, m, i, pi, m, j);

                    ideal_free(RadMs[j]);
                    RadMs[j] = NULL;
                    break;
                }
            }

            if (!equality)
                fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
                        pi, m, i, pi, m, i);
        }
    } else {
        for (j = p; j < numofMs; j += p) {
            ideal_free(RadMs[j]);
            RadMs[j] = NULL;
        }

        for (i = 0; i < numofMs - 2; ++i) {
            equality = 0;

            for (j = 1; j < numofMs - 1; ++j) {
                if (RadMs[j] != NULL && ideal_isequal(Ms[i], RadMs[j])) {
                    equality = 1;
                    if ((j + 1) % p != 0) {
                        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu)\"];\n", \
                                pi, m, i, pi, m, i, pi, m, j);
                    } else {
                        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu)\"];\n", \
                                pi, m, i, pi, m, i, pi, m, j, pi, m, j + 1);
                    }

                    ideal_free(RadMs[j]);
                    RadMs[j] = NULL;
                    break;
                }
            }

            if (!equality)
                fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
                        pi, m, i, pi, m, i);
        }
    }

    /* construct Ms chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                pi, m, i, pi, m, i - 1, m_weight*numofMs);
    }


    /* label RadMs chain */
    if (m != 1) {
        for (i = 0; i < numofMs - 1; ++i) {
            if (RadMs[i] != NULL) {
                fprintf(out, "\tRadM_%llu_%u_%llu [label = \"Rad*M_%llu(%u,%llu)\"];\n", \
                        pi, m, i, pi, m, i);
            } else {
                continue;
            }
        }
    } else {
        for (i = 0; i < numofMs - 1; ++i) {
            if (RadMs[i] != NULL) {
                if ((i + 1) % p != 0) {
                    fprintf(out, "\tRadM_%llu_%u_%llu [label = \"Rad*M_%llu(%u,%llu)\"];\n", \
                            pi, m, i, pi, m, i);
                } else {
                    fprintf(out, "\tRadM_%llu_%u_%llu [label = \"Rad*M_%llu(%u,%llu) = Rad*M_%llu(%u,%llu)\"];\n", \
                            pi, m, i, pi, m, i, pi, m, i + 1);
                }
            } else {
                continue;
            }
        }
    }

    /* it is proven that M_pi(m,0) == Rad*M_pi(m,1)
     * and Rad*M_pi(m,0) is smaller than M_pi(m,0)
     * thus, Rad*M_pi(m,0) will not collide with either any of Ms or RadMs
     * that's why it is safe to create this arc here */
    fprintf(out, "\tM_%llu_%u_%u -> RadM_%llu_%u_%u;\n", pi, m, 0, pi, m, 0);

    /* construct RadMs chain */
    for (i = 1; i < numofMs - 1; ++i) {
        if (RadMs[i] == NULL)
            continue;

        for (j = i + 1; j < numofMs - 1; ++j) {
            if (RadMs[j] != NULL) {
                fprintf(out, "\tRadM_%llu_%u_%llu -> RadM_%llu_%u_%llu;\n", pi, m, j, pi, m, i);
                /* if j > i, then ... >= RadM_pi[j+1] >= RadM_pi[j] > RadM_pi[i]
                 * indeed, we always have RadM_pi[j] >= RadM_pi[i]
                 * and we have already handled all collisions */
                break;
            }
        }
    }


    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each RadM create all needed arcs between RadM and Ms
     * the following cases have already been handled:
     *   Rad*M_pi(m,0)
     *   Rad*M_pi(m,1) == M_pi(m,0)
     *   Rad*M_pi(m, numofMs - 1) == M_pi(m, numofMs - 2) == Rad*QH == Rad */
    previous = 0;
    for (j = 2; j < numofMs - 1; ++j) {
        if (RadMs[j] == NULL)
            continue;

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], RadMs[j])) {
                if (previous < i) {
                    fprintf(out, "\tRadM_%llu_%u_%llu -> M_%llu_%u_%llu;\n", pi, m, j, pi, m, i);
                    previous = i;
                }
                /* if RadM_pi[j] > M_pi[i], then RadM_pi[j] > M_pi[i] > M_pi[i-1] > ... */
                break;
            }
        }
    }

    previous = numofMs - 2;
    for (j = numofMs - 2; j >= 2; --j) {
        if (RadMs[j] == NULL)
            continue;

        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(RadMs[j], Ms[i])) {
                if (previous > i) {
                    fprintf(out, "\tM_%llu_%u_%llu -> RadM_%llu_%u_%llu;\n", pi, m, i, pi, m, j);
                    previous = i;
                }
                /* if M_pi[i] > RadM_pi[j], then ... > M_pi[i+1] > M_pi[i] > RadM_pi[j] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

void print_graph_beautiful(FILE* out,
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


    /* label Ms chain
     * it is proven that only these equalities hold:
     *   M_pi(m, 0) == Rad^(nilindex - 1)
     *   M_pi(m, numofMs - 2) == Rad^1 == Rad
     *   M_pi(m, numofMs - 1) == Rad^0 == QH */
    if (use_groups) {
        fprintf(out, "\tM_%llu_%u_%u [label = \"M_%llu(%u,%u) = Rad^%llu\", group = \"Ms\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\", group = \"Ms\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    } else {
        fprintf(out, "\tM_%llu_%u_%u [label = \"M_%llu(%u,%u) = Rad^%llu\"];\n", \
                pi, m, 0, pi, m, 0, nilindex - 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
        fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu) = Rad^%u\"];\n", \
                pi, m, numofMs - 1, pi, m, numofMs - 1, 0);
    }
    fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", pi, m, numofMs - 1, pi, m, numofMs - 2, m_weight*numofMs);


    /* label Rads chain (except for those in the equalities mentioned above) */
    for (i = 2; i < nilindex - 1; ++i) {
        if (use_groups) {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\", group = \"Rads\"];\n", i, i);
        } else {
            fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"];\n", i, i);
        }
    }

    /* construct Rads chain (except for those in the equalities mentioned above) */
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu [weight = %llu];\n", i, i + 1, r_weight*nilindex);
    }


    /* fill, label and create arcs for Mpi_to_Rad array */
    for (i = 2; i < nilindex - 1; ++i) {
        Mpi_to_Rad[i] = minimum_Pi_for_P(l*(p - 1) - i, p, m, lambda);
        if (use_groups) {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\", group = \"Ms\"];\n", \
                    pi, m, Mpi_to_Rad[i], pi, m, Mpi_to_Rad[i]);
        } else {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
                    pi, m, Mpi_to_Rad[i], pi, m, Mpi_to_Rad[i]);
        }
        fprintf(out, "\tM_%llu_%u_%llu -> Rad_%llu [weight = %llu];\n", pi, m, Mpi_to_Rad[i], i, o_weight*nilindex);
    }

    /* fill, label and create arcs for Rad_to_Mpi array */
    for (i = 2; i < nilindex - 1; ++i) {
        Rad_to_Mpi[i] = maximum_Pi_for_P(l*(p - 1) - i, p, m);
        if (use_groups) {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\", group = \"Ms\"];\n", \
                    pi, m, Rad_to_Mpi[i], pi, m, Rad_to_Mpi[i]);
        } else {
            fprintf(out, "\tM_%llu_%u_%llu [label = \"M_%llu(%u,%llu)\"];\n", \
                    pi, m, Rad_to_Mpi[i], pi, m, Rad_to_Mpi[i]);
        }
        fprintf(out, "\tRad_%llu -> M_%llu_%u_%llu [weight = %llu];\n", i, pi, m, Rad_to_Mpi[i], o_weight*nilindex);
    }


    /* carcass is ready, fill it with inclusion arcs */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* construct Ms chain */
    i = j = nilindex - 2;
    previous = 0;
    while(i >= 2 && j >= 2) {
    /* obviously, Mpi_to_Rad[t] > Rad_to_Mpi[t] for any t
     * thus we won't have the situation when i > 2 && j == 2,
     * but we keep both ">=" checks for sanity anyway */
        if (Rad_to_Mpi[i] < Mpi_to_Rad[j]) {
            /*
             * we don't need to check whether previous == Rad_to_Mpi[i] here
             * indeed, assume Rad_to_Mpi[i] == previous
             * and indices' values are (i,j)
             *
             * we can obtain indices' values (i,j) in 3 ways:
             * 1. (i+1,j+1) -> (i,j)
             *   in this case Rad_to_Mpi[i + 1] == Mpi_to_Rad[j + 1] == previous
             *   therefore, Rad_to_Mpi[i] == Rad_to_Mpi[i + 1]
             *   since all elements in Rad_to_Mpi array are different,
             *   we obtain contradiction
             * 2. (i,j+1) -> (i,j)
             *   in this case previous == Mpi_to_Rad[j + 1] == Rad_to_Mpi[i]
             *   therefore, Rad_to_Mpi[i] < Rad_to_Mpi[i], which again
             *   leads to contradiction
             * 3. (i+1,j) -> (i,j)
             *   in this case previous == Rad_to_Mpi[i + 1] == Rad_to_Mpi[i]
             *   since all elements in Rad_to_Mpi array are different,
             *   we obtain contradiction
             */
            if (previous + 1 == Rad_to_Mpi[i]) {
                fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            } else {
                fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu, style = \"dashed\"];\n", \
                        pi, m, Rad_to_Mpi[i], pi, m, previous, m_weight*numofMs);
            }
            previous = Rad_to_Mpi[i];
            --i;
        } else if (Rad_to_Mpi[i] > Mpi_to_Rad[j]) {
            /* if previous == Rad_to_Mpi[j], then we don't need to create
             * Mpi_to_Rad[j] -> Rad_to_Mpi[j] arc as it will duplicate
             * an already created chain Mpi_to_Rad[j] -> Rad_j -> Rad_to_Mpi[j] */
            if (previous != Rad_to_Mpi[j]) {
                if (previous + 1 == Mpi_to_Rad[j]) {
                    fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                } else {
                    fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu, style = \"dashed\"];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                }
            }
            previous = Mpi_to_Rad[j];
            --j;
        } else {
            /* it is possible to have Rad_to_Mpi[i] == Mpi_to_Rad[j] for some i and j
             * however, all elements in Rad_to_Mpi array are different as well as in Mpi_to_Rad array
             * thus it is impossible to have Rad_to_Mpi[i] == Mpi_to_Rad[j] == previous */
            /* if previous == Rad_to_Mpi[j], then we don't need to create
             * Mpi_to_Rad[j] -> Rad_to_Mpi[j] arc as it will duplicate
             * an already created chain Mpi_to_Rad[j] -> Rad_j -> Rad_to_Mpi[j] */
            if (previous != Rad_to_Mpi[j]) {
                if (previous + 1 == Mpi_to_Rad[j]) {
                    fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                } else {
                    fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu, style = \"dashed\"];\n", \
                            pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
                }
            }
            previous = Mpi_to_Rad[j];
            --i;
            --j;
        }
    }

    /* this block handles situation when i == j == 2 after the previous while
     * when this happens we don't need to create
     * Mpi_to_Rad[j] -> Rad_to_Mpi[j] arc as it will duplicate
     * an already created chain Mpi_to_Rad[j] -> Rad_j -> Rad_to_Mpi[j] */
    if (j == 2) {
        previous = Mpi_to_Rad[j];
        --j;
    }

    /* obviously, Mpi_to_Rad[t] > Rad_to_Mpi[t] for any t
     * thus we won't have the situation when i > 2 && j == 2 in the previous while */
    for (; j >= 2; --j) {
        if (previous + 1 == Mpi_to_Rad[j]) {
            fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                    pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
        } else {
            fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu, style = \"dashed\"];\n", \
                    pi, m, Mpi_to_Rad[j], pi, m, previous, m_weight*numofMs);
        }
        previous = Mpi_to_Rad[j];
    }

    /*
     * Mpi_to_Rad[2] never equals to M_pi(m, numofMs - 2) == Rad
     * Indeed, for any j < l(p - 1) there exists k such that
     * k is a minimal integer for which P_j \susbet \Pi_k.
     * There exists an integer t such that weight(t, p) == j, weight(t, pi) == k
     * and weight(t + 1, p) == j + 1, weight(t + 1, pi) == k + 1.
     * Therefore, if Mpi_to_Rad[2] == M_pi(m, numofMs - 2), then there exists
     * an integer t such that weight(t, p) == l(p - 1) - 2, weight(t, pi) == numofMs - 2
     * and weight(t + 1, p) = l(p - 1) - 1, weight(t + 1, pi) == numofMs - 1.
     * Since Rad == M_pi(m, numofMs - 2) and weight(t + 1, p) == l(p - 1) - 1,
     * we have weight(t + 1, pi) <= numofMs - 2, which contradicts
     * the equality weight(t + 1, pi) == numofMs - 1
     */
    if (previous + 1 == numofMs - 2) {
        fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu];\n", \
                pi, m, numofMs - 2, pi, m, previous, m_weight*numofMs);
    } else {
        fprintf(out, "\tM_%llu_%u_%llu -> M_%llu_%u_%llu [weight = %llu, style = \"dashed\"];\n", \
                pi, m, numofMs - 2, pi, m, previous, m_weight*numofMs);
    }

    fprintf(out, "}\n");

    free(Rad_to_Mpi);
    free(Mpi_to_Rad);
}
