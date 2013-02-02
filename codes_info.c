/* This program will gather and print out all needed info about M_pi's and Rad's structure */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#ifdef WITH_MPI
#include <mpi.h>

int mpi_me, mpi_total;
#endif

#include "common.h"
#include "ideals.h"
#include "list.h"
#include "log.h"

#define MAX_FILENAME_LEN 128

#ifdef WITH_MPI
char* package = "MPI Reed-Muller codes calculator";
#else
char* package = "Reed-Muller codes calculator";
#endif
char* version = "0.2.0";
char* progname = NULL;
char use_stdout = 0;
unsigned long p, l, lambda;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i, j;
    FILE *out;
    IDEAL** Ms;
    IDEAL** Rads;
    IDEAL** RMs;
    IDEAL* pp;

    unsigned long long q;
    unsigned long long numofMs;
    unsigned long nilindex;
    unsigned long m, pi;
    mpz_t dim;

    char outname[MAX_FILENAME_LEN];

#ifdef WITH_MPI
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_me);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_total);
#endif

    /* learn who we really are */
    progname = (char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    q  = pow_ul(p, l);
    pi = (unsigned long) pow_ul(p, lambda);
    m  = l / lambda;
    nilindex = l*(p-1) + 1;
    numofMs = m*(pi-1) + 1;
    mpz_init(dim);

    Ms   = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    Rads = (IDEAL**) malloc(nilindex*sizeof(IDEAL*));
    RMs  = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));

    if (Ms == NULL || Rads == NULL || RMs == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideals' arrays.\n");
        exit(EXIT_FAILURE);
    }

    /* do the job */
    if (!use_stdout) {
        sprintf(outname, "codes_info_%lu-%lu-%lu.txt", p, l, lambda);
        out = fopen(outname, "w");
        if (!out) {
            fprintf(stderr, "Error opening \"%s\" file\n", outname);
            exit(EXIT_FAILURE);
        }
    } else {
        out = stdout;
    }

    dbg_msg("Computing Ms...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_init(pp, pi, m, i);
        Ms[i] = pp;
    }

    dbg_msg("Computing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        pp = ideal_create(q);
        ideal_init(pp, p, l, l*(p - 1) - i);
        Rads[i] = pp;
    }

    dbg_msg("Computing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_product(pp, Rads[1], Ms[i], p);
        RMs[i] = pp;
    }

    /* hardest part of the work is done, spit out results */
    fprintf(out, "Input parameters:       p = %-10lu l  = %-10lu lambda = %-lu\n", p, l, lambda);
    fprintf(out, "Additional parameters:  q = %-10llu pi = %-10lu m = %-lu\n\n", q, pi, m);

    fprintf(out, "Extra info:  m*(pi - 1) = %-10llu m*(pi - 1) - 1 = %-llu\n",  numofMs - 1,  numofMs - 2);
    fprintf(out, "             l*(p - 1)  = %-10lu l*(p - 1) - 1  = %-lu\n\n", nilindex - 1, nilindex - 2);

    fprintf(out, "Total number of M_pi's: %-llu\n", numofMs);
    fprintf(out, "Total number of M_p's:  %-lu\n\n", nilindex);

    fprintf(out, "M_pi's dimensions:\n");
    for (i = 0; i < numofMs; ++i) {
        m_k(dim, pi, m, i);
        gmp_fprintf(out, "M_%lu(%lu,%lu):\t\tdim = %Zd\n", pi, m, i, dim);
    }
    fprintf(out, "\n");

    fprintf(out, "M_p's dimensions:\n");
    for (i = 0; i < nilindex; ++i) {
        m_k(dim, p, l, i);
        gmp_fprintf(out, "Rad^%lu = M_%lu(%lu,%lu):\tdim = %Zd\n", l*(p - 1) - i, p, l, i, dim);
    }
    fprintf(out, "\n");

    fprintf(out, "Detected Ms <-> Rads equalities:\n");
    for (i = 0; i < numofMs; ++i) {
        for (j = 0; j < nilindex; ++j) {
            if (ideal_isequal(Ms[i], Rads[j])) {
                fprintf(out, "Rad^%lu == M_%lu(%lu,%llu)\n", (unsigned long) j, pi, m, i);
            }
        }
    }

    fprintf(out, "\n");
    fprintf(out, "Detected Ms <-> RMs equalities:\n");
    for (i = 0; i < numofMs; ++i) {
        for (j = 0; j < numofMs; ++j) {
            if (ideal_isequal(Ms[i], RMs[j])) {
                fprintf(out, "Rad*M_%lu(%lu,%llu) == M_%lu(%lu,%llu)\n", pi, m, j, pi, m, i);
            }
        }
    }

    /* do cleanup */
    fclose(out);
    dbg_msg_l(5, "Freeing Ms...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(Ms[i]);
    }

    dbg_msg_l(5, "Freeing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        ideal_free(Rads[i]);
    }

    dbg_msg_l(5, "Freeing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(RMs[i]);
    }
    free(Ms);
    free(Rads);
    free(RMs);
    mpz_clear(dim);

#ifdef WITH_MPI
    MPI_Finalize();
#endif

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
        {"stdout", 0, 0, 'c'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies size of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent, except for 1.",
        "Write output to stdout.",
        "Increase debugging level.",
        "Print version information.",
        "Print this message.",
    };
    struct option *opt;
    const char **hlp;
    int max, size;

    for (;;) {
        int i;
        i = getopt_long(*argc, *argv,
            "p:l:L:cDvh", opts, NULL);
        if (i == -1) {
            break;
        }
        switch (i) {
        case 'p':
            sscanf(optarg, "%lu", &p);
            break;
        case 'l':
            sscanf(optarg, "%lu", &l);
            break;
        case 'L':
            sscanf(optarg, "%lu", &lambda);
            break;
        case 'c':
            use_stdout = 1;
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

    if (p == 0 || l == 0 || lambda == 0) {
        fprintf(stderr, "You must specify at least p, l and L options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    if (l % lambda) {
        fprintf(stderr, "(L)ambda must be a factor of l, except for 1. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
