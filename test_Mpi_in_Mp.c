/* A test that checks whether all M_pi lay in M_p with same indexes going from 1 up to nilindex */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#ifdef WITH_MPI
#include <mpi.h>

int mpi_me, mpi_total;
#endif

#include "log.h"
#include "common.h"
#include "ideals.h"

char* package = "Check whether all M_pi lay in M_p with same indexes.";
char* version = "1.0.0";
char* progname = NULL;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned char ret = 1;

    unsigned long long i;

    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** Rads;

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
    init_constants(p, l, lambda);

    Ms   = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    Rads = (IDEAL**) malloc(nilindex*sizeof(IDEAL*));

    if (Ms == NULL || Rads == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideals' arrays.\n");
        exit(EXIT_FAILURE);
    }

    /* do the job */
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

    dbg_msg("Checking hypothesis...\n");
    /* The following statements are proved to be true:
     *   M_pi(0) == M_p(0) == Qu_0
     *   M_pi(1) \subset M_p(1)
     */
    for (i = 2; ret && (i < nilindex); ++i) {
        ret &= ideal_issubset(Ms[i], Rads[l*(p - 1) - i]);
    }

    /* do cleanup */
    dbg_msg_l(5, "Freeing Ms...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(Ms[i]);
    }
    free(Ms);

    dbg_msg_l(5, "Freeing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        ideal_free(Rads[i]);
    }
    free(Rads);

#ifdef WITH_MPI
    MPI_Finalize();
#endif

    if (ret) {
        dbg_msg("Hypothesis is TRUE.\n");
    } else {
        dbg_msg("Hypothesis is FALSE.\n");
    }

    return ret;
}

/*
 * Parse command line arguments
 */
static int handle_cmdline(int *argc, char ***argv) {
    struct option opts[] = {
        {"char", 1, 0, 'p'},
        {"exp", 1, 0, 'l'},
        {"lambda", 1, 0, 'L'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies size of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent, except for 1.",
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
            "p:l:L:Dvh", opts, NULL);
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

    if (l % lambda || lambda == 1) {
        fprintf(stderr, "(L)ambda must be a factor of l, except for 1. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
