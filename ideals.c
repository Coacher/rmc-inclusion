/* code for M_pi(m,k) ideals and related functions */
#include <stdlib.h>
#include <stdio.h>

#include "ideals.h"

IDEAL* ideal_init(unsigned long q, unsigned long pi, unsigned long m, unsigned long k) {
    unsigned long i;
    IDEAL* M;

    M = (IDEAL*) malloc(sizeof(IDEAL));
    if (!M) {
        fprintf(stderr, "Unable to allocate memory for ideal\n");
        return NULL;
    }

    M->q = q;
    M->pi = pi;
    M->m = m;
    M->k = k;

    M->u_s = (char*) calloc(q, sizeof(char));
    if (!M->u_s) {
        fprintf(stderr, "Unable to allocate memory for u_s\n");
        return NULL;
    }

    for (i = 0; i < q; ++i) {
        if (weight(i, pi) <= k) {
            M->u_s[i] = 1;
        }
    }

    return M;
}

void ideal_free(IDEAL* M) {
    if (M) {
        if (M->u_s) {
            free(M->u_s);
        }
        free(M);
    }
}

int ideal_issubset(IDEAL* M, IDEAL* N) {
    unsigned long i;

    if (M->q != N->q)
        return 0;

    for (i = 0; i < M->q; ++i) {
        if (M->u_s[i] > N->u_s[i]) {
            return 0;
        }
    }

    return 1;
}

int ideal_product(IDEAL* res, IDEAL* M, IDEAL* N, unsigned long p) {
    unsigned long i, j, delta;
    unsigned long q;
    unsigned long div1, div2, digit1, digit2;

    if ((M == NULL) || (N == NULL) || (res == NULL))
        return -1;

    if ((M->q != N->q) || (M->q != res->q))
        return -1;

    q = M->q;
    for (i = 0; i < q; ++i) {
        for (j = 0; j < q; ++j) {
            if (M->u_s[i] && N->u_s[j]) {

                if ((i + j > q - 2) && (i + j < 2*(q - 1))) {
                    delta = i + j - (q - 1);

                    if (res->u_s[delta]) {
                        continue;
                    } else {
                        digit1 = i % p;
                        digit2 = delta % p;

                        do {
                            if (digit2 > digit1)
                                break;

                            div1 = i / p;
                            digit1 = i % p;

                            div2 = delta / p;
                            digit2 = delta % p;
                        } while (div1 || div2);

                        res->u_s[delta] = (digit2 > digit1) ? 1: 0;
                    }
                } else if ((i + j) == 2*(q - 1)) {
                    res->u_s[0] = 1;
                    res->u_s[q-1] = 1;
                }
            }
        }
    }

    return 0;
}

int ideal_multiplyby_u(IDEAL* res, IDEAL* M, unsigned long j, unsigned long p) {
    unsigned long i, delta;
    unsigned long q;
    unsigned long div1, div2, digit1, digit2;

    if ((M == NULL) || (res == NULL))
        return -1;

    if (M->q != res->q)
        return -1;

    q = M->q;
    for (i = 0; i < q; ++i) {
        if (M->u_s[i]) {

            if ((i + j > q - 2) && (i + j < 2*(q - 1))) {
                delta = i + j - (q - 1);

                if (res->u_s[delta]) {
                    continue;
                } else {
                    digit1 = i % p;
                    digit2 = delta % p;

                    do {
                        if (digit2 > digit1)
                            break;

                        div1 = i / p;
                        digit1 = i % p;

                        div2 = delta / p;
                        digit2 = delta % p;
                    } while (div1 || div2);

                    res->u_s[delta] = (digit2 > digit1) ? 1: 0;
                }
            } else if ((i + j) == 2*(q - 1)) {
                res->u_s[0] = 1;
                res->u_s[q-1] = 1;
            }
        }
    }

    return 0;
}
