/* a number of small useful functions */
#include <gmp.h>

#include "common.h"

unsigned long pow_ul(unsigned long p, unsigned long l) {
    unsigned long ret = 1;

    while (l) {
        if (l & 1)
            ret *= p;

        p *= p;
        l >>= 1;
    }

    return ret;
}

unsigned long weight(unsigned long x, unsigned long pi) {
    unsigned long weight = 0;

    while (x) {
        weight += x % pi;
        x /= pi;
    }

    return weight;
}

/* calculates \binom{n}{m} using gmp */
unsigned long bin_coeff(long n, long m) {
    unsigned long ret = 1;
    mpz_t binc;

    mpz_init(binc);

    if (m < 0)
        return 0;

    if (n < 0) {
        n = m - n - 1;
        ret *= ((m & 1) ? -1 : 1);
    }

    mpz_bin_uiui(binc, n, m);

    ret *= mpz_get_ui(binc);
    mpz_clear(binc);

    return ret;
}

unsigned long m_k(unsigned long pi, unsigned long m, unsigned long k) {
    unsigned long ret = 0;
    unsigned long j;
    for (j = 0; j <= m; ++j) {
        ret += ((j & 1) ? -1 : 1) * bin_coeff(m, j) * bin_coeff(m + k - pi*j, k - pi*j);
    }

    return ret;
}
