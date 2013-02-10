/* functions related to printing M_pi's, Rads and Rad*M_pi's inclusion graphs */

#include "graph.h"

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads) {
    unsigned long long i, j;

    fprintf(out, "digraph M_Rad_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tsplines=\"ortho\";\n");
    fprintf(out, "\tnodesep=\"3.0\";\n");
    fprintf(out, "\tranksep=\"0.35\";\n");


    /* contrsuct M_pi's chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, 1000*numofMs);
    }

    /* label M_pi's chain; it is proven that only these equalities hold:
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
     * embed Rads chain into appropriate place in M_pi's chain */
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
                /* if M_pi[i] >= Rad^j, then M_pi[i] >= Rad^j >= Rad^(j+1) >= ... */
                break;
            }
        }

        for (j = nilindex - 2; j >= 2; --j) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu;\n", j, pi, m, i);
                /* if Rad^j >= M_pi[i], then ... >= Rad^(j-1) >= Rad^j >= M_pi[i] */
                break;
            }
        }
    }

    fprintf(out, "}\n");
}

void print_rm_graph(FILE* out, IDEAL** Ms, IDEAL** RMs) {
    unsigned long long i, j, k;
    unsigned char previous_is_M = 0;

    /* since we don't know anything for sure about RMs do this collision test */
    for (i = 0; i < numofMs; ++i) {
        for (j = i + 1; j < numofMs; ++j) {
            if (ideal_isequal(RMs[i], RMs[j])) {
                dbg_msg_l(0, "Warning! RMs collision detected: RMs[%llu] == RMs[%llu]\n", i, j);
                dbg_msg_l(0, "All produced results are most possibly invalid!\n");
            }
        }
    }

    fprintf(out, "digraph M_RM_inclusion {\n");

    /* global graph attributes */
    fprintf(out, "\tsplines=\"ortho\";\n");
    fprintf(out, "\tnodesep=\"3.0\";\n");
    fprintf(out, "\tranksep=\"0.35\";\n");


    /* contrsuct M_pi's chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu [weight = %llu];\n", pi, m, i, pi, m, i - 1, 1000*numofMs);
    }

    /* label M_pi's chain; it is proven that these equalities hold:
     *  M_pi(m, 0) = Rad*M_pi(m,1)
     *  M_pi(m, numofMs - 2) = Rad*M_pi(m, numofMs - 1) = Rad*QH = Rad
     */
    fprintf(out, "\tM_%llu_%lu_%i [label = \"M_%llu(%lu,%i) = Rad*M_%llu(%lu,%i)\"];\n", pi, m, 0, pi, m, 0, pi, m, 1);
    for (i = 1; i < numofMs - 2; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", pi, m, i, pi, m, i);
    }
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad*M_%llu(%lu,%llu)\"];\n", \
            pi, m, numofMs - 2, pi, m, numofMs - 2, pi, m, numofMs - 1);
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"];\n", pi, m, numofMs - 1, pi, m, numofMs - 1);


    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");

    /* for each RM create all needed links with Ms and RMs */
    for (j = 2; j < numofMs - 1;) {
        /* first check if current RM is in Ms
         * if it is true, then all Ms links were processed above already
         * and all RMs links were or will be processed during steps with `pure` RMs,
         * but we need to update label and flag */
        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_isequal(Ms[i], RMs[j])) {
                previous_is_M = 1;
                fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad*M_%llu(%lu,%llu)\"];\n", \
                        pi, m, i, pi, m, i, pi, m, j);
                goto end;
            }
        }

        /* if we are here then current RM is not in Ms */
        /* create all needed links with Ms */
        for (i = 1; i < numofMs - 2; ++i) {
            if (ideal_issubset(RMs[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, i, pi, m, j);
                /* if M_pi[i] >= RM_pi[j], then ... >= M_pi[i+1] >= M_pi[i] >= RM_pi[j] */
                break;
            }
        }

        for (i = numofMs - 3; i >= 1; --i) {
            if (ideal_issubset(Ms[i], RMs[j])) {
                fprintf(out, "\tRM_%llu_%lu_%llu -> M_%llu_%lu_%llu;\n", pi, m, j, pi, m, i);
                /* if RM_pi[j] >= M_pi[i], then RM_pi[j] >= M_pi[i] >= M_pi[i-1] >= ... */
                break;
            }
        }

        /* create all possible links with RMs*/
        if (previous_is_M) {
            /* previous RM was in Ms, so there is no need to create link to it from current RM
             * as it was already created in a previous for-block,
             * but we need to create a label */
            fprintf(out, "\tRM_%llu_%lu_%llu [label = \"Rad*M_%llu(%lu,%llu)\"];\n", pi, m, j, pi, m, j);
        } else {
            /* previous RM wasn't in Ms, therefore we need to create link to it from current RM
             * and create a label as well */
            fprintf(out, "\tRM_%llu_%lu_%llu [label = \"Rad*M_%llu(%lu,%llu)\"];\n", pi, m, j, pi, m, j);
            fprintf(out, "\tRM_%llu_%lu_%llu -> RM_%llu_%lu_%llu;\n", pi, m, j, pi, m, j - 1);
        }

        previous_is_M = 0;
end:
        ++j;
    }

    fprintf(out, "}\n");
}
