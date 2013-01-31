/* a number of small useful functions */
#include <gmp.h>

#include "common.h"

unsigned long weight(unsigned long x, unsigned long pi) {
    unsigned long div;
    unsigned long weight = 0;

    div = x / pi;
    weight = x % pi;

    while (!div) {
        weight += div % pi;
        div /= pi;
    }

    return weight;
}

/* calculates \binom{m}{n} using gmp */
unsigned long bin_coeff(unsigned long n, unsigned long m) {
    unsigned long ret;
    mpz_t binc;
    mpz_init(binc);

    mpz_bin_uiui(binc, n, m);

    ret = mpz_get_ui(binc);
    mpz_clear(binc);

    return ret;
}

unsigned long m_k(unsigned long k, unsigned long pi, unsigned long m) {
    unsigned long ret = 0;
    unsigned long j;
    for (j = 0; j <= m; ++j) {
        ret += ((j % 2) ? -1 : 1) * bin_coeff(m, j) * bin_coeff(m + k - pi*j, k - pi*j);
    }

    return ret;
}
