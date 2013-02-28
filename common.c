/* a number of small useful functions */

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

void init_constants(unsigned long p, unsigned long l, unsigned long lambda) {
    q  = pow_ul(p, l);
    pi = pow_ul(p, lambda);
    m  = l / lambda;
    numofMs  = m*(pi-1) + 1;
    nilindex = l*(p-1)  + 1;
}

unsigned long weight(unsigned long long x, unsigned long pi) {
    unsigned long weight = 0;

    while (x) {
        weight += x % pi;
        x /= pi;
    }

    return weight;
}

void bin_coeff(mpz_t rop, long long n, long long m) {
    char sign = 1;

    if (m < 0) {
        mpz_set_ui(rop, 0);
        return;
    }

    if (n < 0) {
        /* \binom{n}{k} = (-1)^k*\binom{m-n-1}{m}, when n < 0 <= m */
        n = m - n - 1;
        sign = (m & 1) ? -1 : 1;
    }

    mpz_bin_uiui(rop, n, m);

    if (sign < 0)
        mpz_mul_si(rop, rop, sign);
}

void m_k(mpz_t rop, unsigned long long pi, unsigned long m, unsigned long k) {
    unsigned long j;
    mpz_t tmp1, tmp2;

    mpz_init(tmp1);
    mpz_init(tmp2);

    mpz_set_ui(rop, 0);

    for (j = 0; j <= m; ++j) {
        bin_coeff(tmp1, m, j);
        bin_coeff(tmp2, m + k - pi*j, k - pi*j);
        mpz_mul(tmp1, tmp1, tmp2);
        if (j & 1)
            mpz_mul_si(tmp1, tmp1, -1);

        mpz_add(rop, rop, tmp1);
    }

    mpz_clear(tmp1);
    mpz_clear(tmp2);
}
