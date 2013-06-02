/* a number of small useful functions */

#include "rmc/common.h"

unsigned long long pow_ull(unsigned int p, unsigned int l) {
    unsigned long long ret = 1;

    while (l) {
        if (l & 1)
            ret *= p;

        p *= p;
        l >>= 1;
    }

    return ret;
}

unsigned long long weight(unsigned long long x, unsigned long long pi) {
    unsigned long long ret = 0;

    while (x) {
        ret += x % pi;
        x /= pi;
    }

    return ret;
}

#ifdef ENABLE_GMP
void bin_coeff(mpz_t rop, long long n, long long m) {
    char sign = 1;

    if ((m < 0) || ( (n < m) && (0 <= n) )) {
        mpz_set_ui(rop, 0);
        return;
    }

    if (n < 0) {
        /* \binom{n}{m} = (-1)^m*\binom{m-n-1}{m}, when n < 0 <= m */
        /* won't overflow in our use cases */
        n = m - n - 1;
        sign = (m & 1) ? -1 : 1;
    }

    mpz_bin_uiui(rop, n, m);

    if (sign < 0)
        mpz_mul_si(rop, rop, sign);
}
#endif
