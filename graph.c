/* functions related to printing M_pi's, Rads and Rad*M_pi's inclusion graphs */

#include "graph.h"

void print_graph(FILE* out, IDEAL** Ms, IDEAL** Rads) {
    unsigned long long i, j;

    fprintf(out, "digraph M_Rad_inclusion {\n");


    /* contrsuct M_pi's chain */
    for (i = 1; i < numofMs; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu -> M_%llu_%lu_%llu\n", pi, m, i, pi, m, i - 1);
    }

    /* label M_pi's chain; it is proven that only these equalities hold:
     *  M_pi(m, 0) = Rad^(nilindex - 1)
     *  M_pi(m, numofMs - 2) = Rad^1 = Rad
     *  M_pi(m, numofMs - 1) = Rad^0 = QH
     */
    fprintf(out, "\tM_%llu_%lu_%i [label = \"M_%llu(%lu,%i) = Rad^%llu\"]\n", pi, m, 0, pi, m, 0, nilindex - 1);
    for (i = 1; i < numofMs - 2; ++i) {
        fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu)\"]\n", pi, m, i, pi, m, i);
    }
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%i\"]\n", pi, m, numofMs - 2, pi, m, numofMs - 2, 1);
    fprintf(out, "\tM_%llu_%lu_%llu [label = \"M_%llu(%lu,%llu) = Rad^%i\"]\n", pi, m, numofMs - 1, pi, m, numofMs - 1, 0);


    /* construct Rads (except mentioned above) chain and
     * embed Rads chain into appropriate place in M_pi's chain */
    fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%i\n", pi, m, numofMs - 2, 2);
    for (i = 2; i < nilindex - 2; ++i) {
        fprintf(out, "\tRad_%llu -> Rad_%llu\n", i, i + 1);
    }
    fprintf(out, "\tRad_%llu -> M_%llu_%lu_%i\n", nilindex - 2, pi, m, 0);

    /* label them */
    for (i = 2; i < nilindex - 1; ++i) {
        fprintf(out, "\tRad_%llu [label = \"Rad^%llu\"]\n", i, i);
    }


    /* carcass is ready, fill it with inclusion links */
    fprintf(out, "# The rest of the file is generated in an automated manner and not meant to be read by human\n");
    for (i = 1; i < numofMs - 2; ++i) {
        for (j = 2; j < nilindex - 1; ++j) {
            if (ideal_issubset(Ms[i], Rads[j])) {
                fprintf(out, "\tRad_%llu -> M_%llu_%lu_%llu\n", j, pi, m, i);
            } else if (ideal_issubset(Rads[j], Ms[i])) {
                fprintf(out, "\tM_%llu_%lu_%llu -> Rad_%llu\n", pi, m, i, j);
            }
        }
    }

    fprintf(out, "}\n");
}
