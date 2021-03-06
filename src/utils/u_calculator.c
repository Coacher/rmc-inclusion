/* A small utility to multiply two u_s elements */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/common.h"
#include "rmc/ideals.h"
#include "rmc/constants.h"

const char* package = "u_s calculator";
const char* version = "1.2.0";
const char* progname = NULL;

/* indices of two elements to multiply */
unsigned long long i, j;

/* global debug level */
unsigned int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    /* index of result */
    unsigned long long delta;

    /* u_delta coefficient */
    mpz_t coeff;

    unsigned long long div1, div2;
    unsigned long digit1, digit2;
    char sign = 1;

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    init_constants();

    if ( !(i < q) || !(j < q) ) {
        fprintf(stderr, "Indices must be lesser than q = p^l.\n");
        exit(EXIT_FAILURE);
    }

    /* ensure i is a maximum of {i, j} */
    if (j > i) {
        delta = i; i = j; j = delta;
    }

    /* calculate index of u_i * u_j */
    /* same as i + j == 2*(q - 1), but no overflow happens */
    if ( (j == (q - 1)) && (i == (q - 1)) ) {
        fprintf(stdout, "result: - u_0 - u_%llu\n", q - 1);
        return 0;
    /* same as i + j <= q - 2, but no overflow happens */
    } else if ( i <= (q - 2 - j) ) {
        fprintf(stdout, "result: 0\n");
        return 0;
    /* the only possible option left for i, j is (i + j > q - 2) && (i + j < 2*(q - 1))
     * since i + j >= q - 1, we have max{i,j} >= (q - 1) / 2
     * since i is a maximum of {i,j}, we get i >= (q - 1) / 2 */
    } else {
        /* same as delta = i + j - (q - 1), but no overflow happens */
        delta = i - ((q - 1) >> 1);
        delta += j;
        delta -= ((q - 1) >> 1);
        /* p == 2 implies that q is odd and (q - 1) / 2 == (q - 2) / 2
         * thus we need to substract additional 1 */
        if (p == 2) --delta;
    }

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

    if (digit1 < digit2) {
        fprintf(stdout, "result: 0 * u_%llu == 0\n", delta);
        return 0;
    }

    /* coefficient is non-zero so compute it */
    mpz_init(coeff);

    bin_coeff(coeff, i, delta);
    /* since i is a maximum of {i,j} we have that (i - delta) is positive */
    sign = ((i - delta) & 1) ? 1 : -1;

    if (sign < 0)
        mpz_mul_si(coeff, coeff, -1);

    gmp_fprintf(stdout, "result: %Zd * u_%llu\n", coeff, delta);

    mpz_clear(coeff);

    return 0;
}

/*
 * Parse command line arguments
 */
static int handle_cmdline(int *argc, char ***argv) {
    struct option opts[] = {
        {"char", 1, 0, 'p'},
        {"exp", 1, 0, 'l'},
        {"lambda", 1, 0, 'L'},
        {"i", 1, 0, 'i'},
        {"j", 1, 0, 'j'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies order of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent, except for 1.",
        "Specifies index of the first element to multiply. Default 0.",
        "Specifies index of the second element to multiply. Default 0.",
        "Increase debugging level.",
        "Print version information.",
        "Print this message.",
    };
    struct option *opt;
    const char **hlp;
    int max, size;

    for (;;) {
        int k;
        k = getopt_long(*argc, *argv,
            "p:l:L:i:j:Dvh", opts, NULL);
        if (k == -1) {
            break;
        }
        switch (k) {
        case 'p':
            sscanf(optarg, "%u", &p);
            break;
        case 'l':
            sscanf(optarg, "%u", &l);
            break;
        case 'L':
            sscanf(optarg, "%u", &lambda);
            break;
        case 'i':
            sscanf(optarg, "%llu", &i);
            break;
        case 'j':
            sscanf(optarg, "%llu", &j);
            break;
        case 'D':
            debug++;
            break;
        case 'v':
            printf("%s - %s\n", package, version);
            exit(EXIT_SUCCESS);
        case 'h':
        default:
            fprintf(stdout, "Usage: %s [OPTIONS]\n", progname);
            max = 0;
            for (opt = opts; opt->name; opt++) {
                size = strlen(opt->name);
                if (size > max)
                    max = size;
            }
            for (opt = opts, hlp = opts_help;
                 opt->name;
                 opt++, hlp++)
            {
                fprintf(stdout, "  -%c, --%s",
                    opt->val, opt->name);
                size = strlen(opt->name);
                for (; size < max; size++)
                    fprintf(stdout, " ");
                fprintf(stdout, "  %s\n", *hlp);
            }
            exit(EXIT_SUCCESS);
            break;
        }
    }

    *argc -= optind;
    *argv += optind;

    if (p == 0 || l == 0 || lambda == 0) {
        fprintf(stderr, "You must specify at least p, l and L options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    if (l % lambda || lambda == 1) {
        fprintf(stderr, "(L)ambda must be a factor of l, except for 1. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
