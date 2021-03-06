/* M_pi(m,k) ideals and related functions */
#include <stdlib.h>
#include <stdio.h>

#include "rmc/ideals.h"

IDEAL* ideal_create(unsigned long long q) {
    IDEAL* M;

    M = (IDEAL*) malloc(sizeof(IDEAL));
    if (M == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideal.\n");
        return NULL;
    }

    M->q = q;

    M->u_s = (unsigned char*) calloc(q, sizeof(unsigned char));
    if (M->u_s == NULL) {
        fprintf(stderr, "Unable to allocate memory for u_s.\n");
        free(M);
        return NULL;
    }

    return M;
}

int ideal_init(IDEAL* M,
        unsigned long long pi, unsigned int m, unsigned long long k) {

    unsigned long long i;

    if (M == NULL)
        return 1;

    M->pi = pi;
    M->m = m;
    M->k = k;

    for (i = 0; i < M->q; ++i) {
        if (weight(i, pi) <= k) {
            M->u_s[i] = 1;
        }
    }

    return 0;
}

void ideal_free(IDEAL* M) {
    if (M) {
        if (M->u_s) {
            free(M->u_s);
        }
        free(M);
    }
}

#ifdef ENABLE_GMP
void m_k(mpz_t rop,
        unsigned long long pi, unsigned int m, unsigned long long k) {

    unsigned int j;
    mpz_t tmp1, tmp2;

    mpz_init(tmp1);
    mpz_init(tmp2);

    mpz_set_ui(rop, 0);

    for (j = 0; j <= m; ++j) {
        bin_coeff(tmp1, m, j);
        /* next call needs special attention as parameters can overflow */
        bin_coeff(tmp2, m + k - pi*j, k - pi*j);
        mpz_mul(tmp1, tmp1, tmp2);
        if (j & 1)
            mpz_mul_si(tmp1, tmp1, -1);

        mpz_add(rop, rop, tmp1);
    }

    mpz_clear(tmp1);
    mpz_clear(tmp2);
}
#endif

int ideal_isequal(IDEAL* M, IDEAL* N) {
    unsigned long long i;

    if (M == NULL || N == NULL)
        return 0;

    if (M->q != N->q)
        return 0;

    for (i = 0; i < M->q; ++i) {
        if (M->u_s[i] != N->u_s[i]) {
            return 0;
        }
    }

    return 1;
}

int ideal_issubset(IDEAL* M, IDEAL* N) {
    unsigned long long i;

    if (M == NULL || N == NULL)
        return 0;

    if (M->q != N->q)
        return 0;

    for (i = 0; i < M->q; ++i) {
        if (M->u_s[i] > N->u_s[i]) {
            return 0;
        }
    }

    return 1;
}

int ideal_diff(IDEAL* res, IDEAL* M, IDEAL* N) {
    unsigned long long i;

    if (M == NULL || N == NULL || res == NULL) {
        dbg_msg("ideal_diff: incorrect input parameters\n");
        return -1;
    }

    if (M->q != N->q || M->q != res->q) {
        dbg_msg("ideal_diff: incorrect input parameters\n");
        return -2;
    }

    for (i = 0; i < res->q; ++i) {
        if (M->u_s[i] == 1 && N->u_s[i] == 1) {
            res->u_s[i] = 0;
        } else {
            res->u_s[i] = M->u_s[i];
        }
    }

    return 0;
}

int ideal_intersect(IDEAL* res, IDEAL* M, IDEAL* N) {
    unsigned long long i;

    if (M == NULL || N == NULL || res == NULL) {
        dbg_msg("ideal_intersect: incorrect input parameters\n");
        return -1;
    }

    if (M->q != N->q || M->q != res->q) {
        dbg_msg("ideal_intersect: incorrect input parameters\n");
        return -2;
    }

    for (i = 0; i < res->q; ++i) {
        res->u_s[i] = ((M->u_s[i] == 1) && (N->u_s[i] == 1));
    }

    return 0;
}

int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long long j,
        unsigned int p) {

    unsigned long long i, q;
    unsigned long long delta;
    unsigned long long div1, div2;
    unsigned int digit1, digit2;

    if (M == NULL || res == NULL) {
        dbg_msg("ideal_multiplyby_u: incorrect input parameters\n");
        return -1;
    }

    if (M->q != res->q) {
        dbg_msg("ideal_multiplyby_u: incorrect input parameters\n");
        return -2;
    }

    q = M->q;
    for (i = 0; i < q; ++i) {
        if (M->u_s[i]) {
            /* same as i + j == 2*(q - 1), but no overflow happens */
            if ( (i == (q - 1)) && (j == (q - 1)) ) {
                res->u_s[0] = 1;
                res->u_s[q - 1] = 1;
            /* same as i + j <= q - 2, but no overflow happens */
            } else if ( i <= (q - 2 - j) ) {
                continue;
            /* the only possible option left for i, j is (i + j > q - 2) && (i + j < 2*(q - 1))
             * since i + j >= q - 1, we have max{i,j} >= (q - 1) / 2 */
            } else {
                /* same as delta = i + j - (q - 1), but no overflow happens */
                if (i > j) {
                    /* i >= (q - 1) / 2 */
                    delta = i - ((q - 1) >> 1);
                    delta += j;
                } else {
                    /* j >= (q - 1) / 2 */
                    delta = j - ((q - 1) >> 1);
                    delta += i;
                }
                delta -= ((q - 1) >> 1);
                /* p == 2 implies that q is odd and (q - 1) / 2 == (q - 2) / 2
                 * thus we need to substract additional 1 */
                if (p == 2) --delta;

                if (res->u_s[delta]) {
                    /* Qu_delta already in res so don't bother */
                    continue;
                } else {
                    /* u_i * u_j is non-zero first, when i + j > q - 2,
                     * second, when \binom{i}{delta} != 0 mod p;
                     * we use Lucas theorem to avoid computation of \binom here
                     * and compare only p-coordinates of i and delta */
                    digit1 = i % p;
                    digit2 = delta % p;
                    div1 = i;
                    div2 = delta;

                    while ((digit1 >= digit2) && div2) {
                        div1 /= p;
                        digit1 = div1 % p;

                        div2 /= p;
                        digit2 = div2 % p;
                    };

                    res->u_s[delta] = (digit1 >= digit2);
                }
            }
        }
    }

    return 0;
}

int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N,
        unsigned int p) {

    unsigned long long j;
    int ret;

    if (M == NULL || N == NULL || res == NULL) {
        dbg_msg("ideal_product: incorrect input parameters\n");
        return -1;
    }

    if (M->q != N->q || M->q != res->q) {
        dbg_msg("ideal_product: incorrect input parameters\n");
        return -2;
    }

    for (j = 0; j < N->q; ++j) {
        if (N->u_s[j]) {
            if ((ret = ideal_multiplyby_u(res, M, j, p)))
                return ret;
        }
    }

    return 0;
}

void ideal_print(IDEAL* M) {
    unsigned long long i;

    if (M == NULL)
        return;

    fprintf(stdout, "[");

    for (i = 0; i < (M->q - 1); ++i)
        fprintf(stdout, " %u", M->u_s[i]);

    /* i = M->q - 1 */
    fprintf(stdout, " %u ]\n", M->u_s[i]);
}

void ideal_print_verbose(IDEAL* M) {
    unsigned long long i;

    if (M == NULL)
        return;

    for (i = 0; i < (M->q - 1); ++i) {
        if (M->u_s[i]) {
            fprintf(stdout, "u_%llu ", i);
        }
    }

    /* i = M->q - 1 */
    if (!M->u_s[i]) {
        /* only the whole group algebra QH contains u_(q-1) */
        fprintf(stdout, "\b\n");
    } else {
        fprintf(stdout, "u_%llu\n", i);
    }
}

unsigned long long minimum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m, unsigned int lambda) {

    unsigned long long i;
    unsigned int theta, tau;
    unsigned long long sum = 0;

    theta = j / (m*(p - 1));
    tau = j % (m*(p - 1));

    for(i = 0; i < theta; ++i) {
        sum += m*(p - 1)*pow_ull(p, lambda - 1 - i);
    }

    sum += tau*pow_ull(p, lambda - 1 - theta);

    return sum;
}

static unsigned long long lift(unsigned long long t,
        unsigned int p, unsigned int m) {
    return p*(t + 1) + m*(p - 1) - 1;
}

unsigned long long maximum_Pi_for_P(unsigned long long j,
        unsigned int p, unsigned int m) {

    unsigned long long i;
    unsigned int theta, tau;
    unsigned long long ret = 0;

    if (j < m*(p - 1)) {
        return j;
    } else {
        theta = j / (m*(p - 1));
        tau = j % (m*(p - 1));

        ret = lift(tau, p, m);

        for(i = 1; i < theta; ++i) {
            ret = lift(ret, p, m);
        }
    }

    return ret;
}

unsigned long long minimum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m) {

    unsigned long long j, k_j;

    j = k_j = 0;

    while (k_j < k) {
        ++j;
        k_j = maximum_Pi_for_P(j, p, m);
    }

    return j;
}

unsigned long long maximum_P_for_Pi(unsigned long long k,
        unsigned int p, unsigned int m,
        unsigned int l, unsigned int lambda,
        unsigned long long upper_bound) {

    unsigned long long j, k_j;

    if ((k_j = (upper_bound - k)) <= 1)
        return l*(p - 1) - k_j;

    j = l*(p - 1) - 1;

    while (k < k_j) {
        --j;
        k_j = minimum_Pi_for_P(j, p, m, lambda);
    }

    return j;
}
