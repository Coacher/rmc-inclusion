/* This program will gather and print all needed info about M_pi's, Rads and Rad*M_pi's structure
 * It is also able to construct and print inclusion graph in dot format */
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
#include "info.h"
#include "graph.h"

#define MAX_FILENAME_LEN 128

#define WITH_INFO       1
#define WITH_GRAPH      (1 << 1)
#define WITH_RM_GRAPH   (1 << 2)

#ifdef WITH_MPI
char* package = "MPI Reed-Muller codes calculator";
#else
char* package = "Reed-Muller codes calculator";
#endif
char* version = "0.3.0";
char* progname = NULL;
unsigned char use_stdout = 0;
unsigned char output_control = 0;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i;
    FILE *info_out, *graph_out, *rm_graph_out;

    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** Rads;
    IDEAL** RMs;

    char     info_outname[MAX_FILENAME_LEN];
    char    graph_outname[MAX_FILENAME_LEN];
    char rm_graph_outname[MAX_FILENAME_LEN];

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

    /* prepare output files */
    if (!use_stdout) {
        if (output_control & WITH_INFO) {
            sprintf(info_outname, "codes_info_%lu-%lu-%lu.txt", p, l, lambda);
            info_out = fopen(info_outname, "w");

            if (!info_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", info_outname);
                exit(EXIT_FAILURE);
            }
        }

        if (output_control & WITH_GRAPH) {
            sprintf(graph_outname, "inclusion_tree_%lu-%lu-%lu.gv", p, l, lambda);
            graph_out = fopen(graph_outname, "w");

            if (!graph_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", graph_outname);
                exit(EXIT_FAILURE);
            }
        }

        if (output_control & WITH_RM_GRAPH) {
            sprintf(rm_graph_outname, "rm_inclusion_tree_%lu-%lu-%lu.gv", p, l, lambda);
            rm_graph_out = fopen(rm_graph_outname, "w");

            if (!rm_graph_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", rm_graph_outname);
                exit(EXIT_FAILURE);
            }
        }
    } else {
        if (weight(output_control, 2) != 1) {
            fprintf(stderr, "You must specify exactly one of the available outputs in conjuction with stdout option.\n");
            exit(EXIT_FAILURE);
        }

        info_out = stdout;
        graph_out = stdout;
        rm_graph_out = stdout;
    }

    /* initializing needed things */
    init_constants(p, l, lambda);

    Ms   = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    Rads = (IDEAL**) malloc(nilindex*sizeof(IDEAL*));
    RMs  = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));

    if (Ms == NULL || Rads == NULL || RMs == NULL) {
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

    dbg_msg("Computing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_product(pp, Rads[1], Ms[i], p);
        RMs[i] = pp;
    }

    /* print out info */
    if (output_control & WITH_INFO) {
        dbg_msg("Printing out info...\n");
        print_info(info_out, Ms, Rads, RMs);
        fclose(info_out);
    }

    /* print out graph */
    if (output_control & WITH_GRAPH) {
        dbg_msg("Printing out graph...\n");
        print_graph(graph_out, Ms, Rads);
        fclose(graph_out);
    }

    /* print out rm_graph */
    if (output_control & WITH_RM_GRAPH) {
        dbg_msg("Printing out rm_graph...\n");
//        print_rm_graph(rm_graph_out, RMs, Rads);
        fclose(rm_graph_out);
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

    dbg_msg_l(5, "Freeing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(RMs[i]);
    }
    free(RMs);

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
        {"with_info", 0, 0, 'I'},
        {"with_graph", 0, 0, 'G'},
        {"with_rm_graph", 0, 0, 'R'},
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
        "Enable info output.",
        "Enable graph output.",
        "Enable rm_graph output.",
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
            "p:l:L:cIGRDvh", opts, NULL);
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
        case 'I':
            output_control |= WITH_INFO;
            break;
        case 'G':
            output_control |= WITH_GRAPH;
            break;
        case 'R':
            output_control |= WITH_RM_GRAPH;
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

    if (!output_control) {
        fprintf(stderr, "You must specify at least one of I, G or R options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
