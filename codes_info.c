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
#include "log.h"

#define MAX_FILENAME_LEN 128

#ifdef WITH_MPI
char* package = "MPI Reed-Muller codes calculator";
#else
char* package = "Reed-Muller codes calculator";
#endif
char* version = "0.0.1";
char* progname = NULL;
char use_stdout = 0;
unsigned long p, l, lambda;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long i, j;
    FILE *out;
    IDEAL* Ms;
    IDEAL* Rads;
    IDEAL* tmp;

    unsigned long q, m, pi;
    unsigned long nilindex;
    unsigned long numofMs;

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
    pi = pow_ul(p, lambda);
    m  = l / lambda;
    nilindex = l*(p-1) + 1;
    numofMs = m*(pi-1) + 1;

    Ms   = (IDEAL*) malloc(numofMs*sizeof(IDEAL));
    Rads = (IDEAL*) malloc(nilindex*sizeof(IDEAL));

    if (Ms == NULL || Rads == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideals arrays.\n");
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

    fprintf(out, "Info for graph with parameters: p = %lu, l = %lu, lambda = %lu.\n", p, l, lambda);
    fprintf(out, "q = %lu, pi = %lu, m = %lu\n\n", q, pi, m);

    fprintf(out, "Total number of (M_pi)-s: %lu\n", numofMs);
    fprintf(out, "Total number of (M_p)-s:  %lu\n", nilindex);
    fprintf(out, "Total number of different non-zero radical powers: %lu\n\n", nilindex - 1);

    fprintf(out, "m*(pi-1) = %-10lu m*(pi-1) - 1 = %-lu\n", numofMs - 1, numofMs - 2);
    fprintf(out, "l*(p-1) =  %-10lu l*(p-1) - 1 =  %-lu\n\n", nilindex - 1, nilindex - 2);

    fprintf(out, "Total number of Ms is %lu:\n", numofMs);
    for (i = 0; i < numofMs; ++i)
        fprintf(out, "M_%lu(%lu,%lu):\t\tdim = %lu\n", pi, m, i, m_k(pi, m, i));
    fprintf(out, "\n");

    fprintf(out, "Total number of Radical powers is %lu:\n", nilindex);
    for (i = 0; i < nilindex; ++i)
        fprintf(out, "Rad^%lu = M_%lu(%lu,%lu):\tdim = %lu\n", l*(p - 1) - i, p, l, i, m_k(p, l, i));
    fprintf(out, "\n");

    dbg_msg("Computing Ms...\n");
    for (i = 1; i < numofMs; ++i) {
        tmp = ideal_create(q);
        ideal_init(tmp, pi, m, i);
        Ms[i] = *tmp;
    }

    dbg_msg("Computing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        tmp = ideal_create(q);
        ideal_init(tmp, p, l, l*(p - 1) - i);
        Rads[i] = *tmp;
    }

    fprintf(out, "Detected (M_pi)-s <-> Rads equalities:\n");
    for (i = 0; i <= numofMs; ++i) {
        for (j = 0; j <= nilindex; ++j) {
            if (ideal_isequal(&Ms[i], &Rads[j])) {
                fprintf(out, "Rad^%lu == M_%lu(%lu,%lu)\n", j, pi, m, i);
            }
        }
    }

    /* do cleanup */

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
        "Specifies characteristic of Q, must be a prime.",
        "Specifies size of Q as an exponent of p.",
        "Specifies the series of ideals, must be a factor of l.",
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

    return 0;
}
