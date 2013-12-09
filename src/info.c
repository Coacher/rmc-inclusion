/* functions for printing Ms, Rads and RadMs structure info */
#ifdef ENABLE_GMP
#include <gmp.h>
#endif

#include "info.h"

void print_info(FILE* out, IDEAL** Ms, IDEAL** Rads, IDEAL** RadMs) {
    unsigned long long i, j;

#ifdef ENABLE_GMP
    mpz_t dim;
    mpz_init(dim);
#endif

    fprintf(out, "Input parameters:       p = %-10u l  = %-10u lambda = %-u\n",   p,  l, lambda);
    fprintf(out, "Additional parameters:  q = %-10llu pi = %-10llu m = %-u\n\n",  q, pi, m);

    fprintf(out, "Extra info:  m*(pi - 1) = %-10llu m*(pi - 1) - 1 = %-llu\n",    numofMs - 1,  numofMs - 2);
    fprintf(out, "             l*(p - 1)  = %-10llu l*(p - 1) - 1  = %-llu\n\n", nilindex - 1, nilindex - 2);

    fprintf(out, "Total number of M_pi's: %-llu\n",    numofMs);
    fprintf(out, "Total number of M_p's:  %-llu\n\n", nilindex);

#ifdef ENABLE_GMP
    fprintf(out, "(M_pi)s dimensions:\n");
    for (i = 0; i < numofMs; ++i) {
        m_k(dim, pi, m, i);
        gmp_fprintf(out, "M_%llu(%u,%lu):\t\tdim = %Zd\n", pi, m, i, dim);
    }
    fprintf(out, "\n");

    fprintf(out, "(M_p)s dimensions:\n");
    for (i = 0; i < nilindex; ++i) {
        m_k(dim, p, l, i);
        gmp_fprintf(out, "Rad^%lu == M_%lu(%u,%lu):\tdim = %Zd\n", l*(p - 1) - i, p, l, i, dim);
    }
    fprintf(out, "\n");
#endif

    fprintf(out, "Detected Ms <-> Rads equalities:\n");
    for (i = 0; i < numofMs; ++i) {
        for (j = 0; j < nilindex; ++j) {
            if (ideal_isequal(Ms[i], Rads[j])) {
                fprintf(out, "Rad^%llu == M_%llu(%u,%llu)\n", j, pi, m, i);
            }
        }
    }

    fprintf(out, "\n");
    fprintf(out, "Detected Ms <-> RadMs equalities:\n");
    for (i = 0; i < numofMs; ++i) {
        for (j = 0; j < numofMs; ++j) {
            if (ideal_isequal(Ms[i], RadMs[j])) {
                fprintf(out, "Rad*M_%llu(%u,%llu) == M_%llu(%u,%llu)\n", pi, m, j, pi, m, i);
            }
        }
    }

#ifdef ENABLE_GMP
    mpz_clear(dim);
#endif
}
