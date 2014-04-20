/* This program gathers and prints all needed info about Ms, Rads and RadMs structure.
 * It is also able to print Ms/Rads and Ms/RadMs inclusion graphs in dot format. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/common.h"
#include "rmc/ideals.h"
#include "rmc/constants.h"
#include "info.h"
#include "graph.h"

#define MAX_FILENAME_LEN 128

#define WITH_INFO           1
#define WITH_GRAPH          (1 << 1)
#define WITH_RADM_GRAPH     (1 << 2)

const char* package = "Basic Reed-Muller codes plotter";
const char* version = "3.1.5";
const char* progname = NULL;
unsigned char use_stdout = 0;
unsigned char output_control = 0;
unsigned int m_weight = 1000;
unsigned int r_weight = 1000;
unsigned int o_weight = 10;
unsigned char use_groups = 0;

/* global debug level */
unsigned int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i;
    FILE* info_out       = NULL;
    FILE* graph_out      = NULL;
    FILE* radm_graph_out = NULL;

    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** Rads;
    IDEAL** RadMs;

    char       info_outname[MAX_FILENAME_LEN];
    char      graph_outname[MAX_FILENAME_LEN];
    char radm_graph_outname[MAX_FILENAME_LEN];

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* prepare output files */
    if (!use_stdout) {
        if (output_control & WITH_INFO) {
            sprintf(info_outname, "codes_info_%u-%u-%u.txt", p, l, lambda);
            info_out = fopen(info_outname, "w");

            if (!info_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", info_outname);
                exit(EXIT_FAILURE);
            }
        }

        if (output_control & WITH_GRAPH) {
            sprintf(graph_outname, "inclusion_tree_%u-%u-%u.gv", p, l, lambda);
            graph_out = fopen(graph_outname, "w");

            if (!graph_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", graph_outname);
                exit(EXIT_FAILURE);
            }
        }

        if (output_control & WITH_RADM_GRAPH) {
            sprintf(radm_graph_outname, "radm_inclusion_tree_%u-%u-%u.gv", p, l, lambda);
            radm_graph_out = fopen(radm_graph_outname, "w");

            if (!radm_graph_out) {
                fprintf(stderr, "Error opening \"%s\" file\n", radm_graph_outname);
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
        radm_graph_out = stdout;
    }

    /* initializing needed things */
    init_constants();

    Ms    = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    Rads  = (IDEAL**) malloc(nilindex*sizeof(IDEAL*));
    RadMs = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));

    if (Ms == NULL || Rads == NULL || RadMs == NULL) {
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

    dbg_msg("Computing RadMs...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_product(pp, Rads[1], Ms[i], p);
        RadMs[i] = pp;
    }

    /* print out info */
    if (output_control & WITH_INFO) {
        dbg_msg("Printing out info...\n");
        print_info(info_out, Ms, Rads, RadMs);
        fclose(info_out);
    }

    /* print out graph */
    if (output_control & WITH_GRAPH) {
        dbg_msg("Printing out graph...\n");
        print_graph(graph_out, Ms, Rads, m_weight, r_weight, o_weight, use_groups);
        fclose(graph_out);
    }

    /* print out radm_graph */
    if (output_control & WITH_RADM_GRAPH) {
        dbg_msg("Printing out radm_graph...\n");
        print_radm_graph(radm_graph_out, Ms, RadMs, m_weight);
        fclose(radm_graph_out);
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

    dbg_msg_l(5, "Freeing RadMs...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(RadMs[i]);
    }
    free(RadMs);

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
        {"with_radm_graph", 0, 0, 'R'},
        {"m_weight", 1, 0, 'm'},
        {"r_weight", 1, 0, 'r'},
        {"o_weight", 1, 0, 'o'},
        {"use_groups", 0, 0, 'g'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies order of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent, except for 1.",
        "Write output to stdout.",
        "Enable info output.",
        "Enable graph output.",
        "Enable radm_graph output.",
        "Specifies weight to use for Ms arcs in graph construction. Default 1000.",
        "Specifies weight to use for Rads arcs in graph construction. Default 1000.",
        "Specifies weight to use for Rads<->Ms arcs in graph construction. Default 10.",
        "Enable grouping of Rads and Ms when plotting Ms/Rads inclusion graph.",
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
            "p:l:L:cIGRm:r:o:gDvh", opts, NULL);
        if (i == -1) {
            break;
        }
        switch (i) {
        case 'p':
            sscanf(optarg, "%u", &p);
            break;
        case 'l':
            sscanf(optarg, "%u", &l);
            break;
        case 'L':
            sscanf(optarg, "%u", &lambda);
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
            output_control |= WITH_RADM_GRAPH;
            break;
        case 'm':
            sscanf(optarg, "%u", &m_weight);
            break;
        case 'r':
            sscanf(optarg, "%u", &r_weight);
            break;
        case 'o':
            sscanf(optarg, "%u", &o_weight);
            break;
        case 'g':
            use_groups = 1;
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

    if (!output_control) {
        fprintf(stderr, "You must specify at least one of I, G or R options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
