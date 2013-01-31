/* a number of small useful functions */
#include <gmp.h>

#include "common.h"

unsigned long long pow_ul(unsigned long p, unsigned long l) {
    unsigned long long ret = 1;

    while (l) {
        if (l & 1)
            ret *= p;

        p *= p;
        l >>= 1;
    }

    return ret;
}

unsigned long weight(unsigned long long x, unsigned long pi) {
    unsigned long long weight = 0;

    while (x) {
        weight += x % pi;
        x /= pi;
    }

    return (unsigned long) weight;
}

/* calculates \binom{n}{m} using gmp */
void bin_coeff(mpz_t rop, long n, long m) {
    if (m < 0) {
        mpz_set_ui(rop, 0);
        return;
    }

    if (n < 0) {
        n = m - n - 1;
        (m & 1) ? mpz_set_si(rop, -1) : mpz_set_ui(rop, 1);
    }

    mpz_bin_uiui(rop, n, m);
}

void m_k(mpz_t rop, unsigned long pi, unsigned long m, unsigned long k) {
    unsigned long j;
    mpz_t tmp1, tmp2;

    mpz_init(tmp1);
    mpz_init(tmp2);

    mpz_set_ui(rop, 0);

    for (j = 0; j <= m; ++j) {
        bin_coeff(tmp1, m, j);
        bin_coeff(tmp2, m + k - pi*j, k - pi*j);
        mpz_mul(tmp1, tmp1, tmp2);
        mpz_mul_si(tmp1, tmp1, (j & 1) ? -1 : 1);
        mpz_add(rop, rop, tmp1);
    }

    mpz_clear(tmp1);
    mpz_clear(tmp2);
}
