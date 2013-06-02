/* a number of small useful functions */

#include "rmc/common.h"

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
    unsigned long ret = 0;

    while (x) {
        ret += x % pi;
        x /= pi;
    }

    return ret;
}

void bin_coeff(mpz_t rop, long long n, long long m) {
#ifdef ENABLE_GMP
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
#else
    dbg_msg("librmc built without GMP support. bin_coeff won't work.\n");
#endif
}
