/* This program will construct and print inclusion graph for Ms and Rads in dot format
 * using advanced techniques provided by theoretical background
 * i.e. no direct operations with ideals are performed */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/common.h"
#include "rmc/ideals.h"
#include "constants.h"
#include "graph.h"

#define MAX_FILENAME_LEN 128

const char* package = "Basic Reed-Muller codes beautiful graph generator";
const char* version = "0.0.1";
char* progname = NULL;
unsigned char use_stdout = 0;
unsigned int m_weight = 1000;
unsigned int r_weight = 1000;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    FILE *graph_out;

    char graph_outname[MAX_FILENAME_LEN];

    /* learn who we really are */
    progname = (char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* prepare output files */
    if (!use_stdout) {
        sprintf(graph_outname, "inclusion_tree_%lu-%lu-%lu.gv", p, l, lambda);
        graph_out = fopen(graph_outname, "w");

        if (!graph_out) {
            fprintf(stderr, "Error opening \"%s\" file\n", graph_outname);
            exit(EXIT_FAILURE);
        }
    } else {
        graph_out = stdout;
    }

    /* initializing needed things */
    init_constants();

    /* print out graph */
    dbg_msg("Printing out graph...\n");
    print_graph_beautiful(graph_out, m_weight, r_weight);
    fclose(graph_out);

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
        {"m_weight", 1, 0, 'm'},
        {"r_weight", 1, 0, 'r'},
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
        "Specifies weight to use for Ms links in graph construction. Default 1000.",
        "Specifies weight to use for Rads links in graph construction. Default 1000.",
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
            "p:l:L:cIGRm:r:Dvh", opts, NULL);
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
        case 'm':
            sscanf(optarg, "%u", &m_weight);
            break;
        case 'r':
            sscanf(optarg, "%u", &r_weight);
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
