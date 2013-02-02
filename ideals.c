/* code for M_pi(m,k) ideals and related functions */
#include <stdlib.h>
#include <stdio.h>

#include "ideals.h"

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

int ideal_init(IDEAL* M, unsigned long pi, unsigned long m, unsigned long k) {
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

int ideal_isequal(IDEAL* M, IDEAL* N) {
    unsigned long long i;

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

    if (M->q != N->q)
        return 0;

    for (i = 0; i < M->q; ++i) {
        if (M->u_s[i] > N->u_s[i]) {
            return 0;
        }
    }

    return 1;
}

int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long long j, unsigned long p) {
    unsigned long long q, i, delta;
    unsigned long long div1, div2;
    unsigned long digit1, digit2;

    if (M == NULL || res == NULL)
        return -1;

    if (M->q != res->q)
        return -2;

    q = M->q;
    for (i = 0; i < q; ++i) {
        if (M->u_s[i]) {
            if ( (i + j > q - 2) && (i + j < 2*(q - 1)) ) {
                delta = i + j - (q - 1);

                if (res->u_s[delta]) {
                    continue;
                } else {
                    digit1 = i % p;
                    digit2 = delta % p;
                    div1 = i;
                    div2 = delta;

                    do {
                        if (digit2 > digit1)
                            break;

                        div1 /= p;
                        digit1 = div1 % p;

                        div2 /= p;
                        digit2 = div2 % p;
                    } while (div2);

                    res->u_s[delta] = (digit2 < digit1);
                }
            } else if ((i + j) == 2*(q - 1)) {
                res->u_s[0] = 1;
                res->u_s[q-1] = 1;
            }
        }
    }

    return 0;
}

int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N, unsigned long p) {
    unsigned long long q, j;
    int ret;

    if (M == NULL || N == NULL || res == NULL)
        return -1;

    if (M->q != N->q || M->q != res->q)
        return -2;

    q = N->q;
    for (j = 0; j < q; ++j) {
        if (N->u_s[j]) {
            if (ret = ideal_multiplyby_u(res, M, j, p))
                return ret;
        }
    }

    return 0;
}

