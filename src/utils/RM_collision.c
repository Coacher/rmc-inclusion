/* A small utility to detect RMs collisions and print them out */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/ideals.h"
#include "constants.h"

const char* package = "RMs collision detector";
const char* version = "1.1.2";
const char* progname = NULL;

/* global debug level */
unsigned int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i, j;
    unsigned char was_collision = 0;

    IDEAL* Rad;
    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** RMs;

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    init_constants();

    Ms   = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    RMs  = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));

    if (Ms == NULL || RMs == NULL) {
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

    dbg_msg("Computing Rad...\n");
    pp = ideal_create(q);
    ideal_init(pp, p, l, l*(p - 1) - 1);
    Rad = pp;

    dbg_msg("Computing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_product(pp, Rad, Ms[i], p);
        RMs[i] = pp;
    }

    /* do collision test */
    for (i = 0; i < numofMs; ++i) {
        was_collision = 0;
        if (RMs[i] == NULL)
            continue;

        for (j = i + 1; j < numofMs; ++j) {
            if (RMs[j] == NULL) {
                continue;
            } else if (ideal_isequal(RMs[i], RMs[j])) {
                if (!was_collision) {
                    was_collision = 1;

                    fprintf(stdout, "Rad*M_%llu(%u,%llu)", pi, m, i);
                    fprintf(stdout, " == Rad*M_%llu(%u,%llu)", pi, m, j);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                } else {
                    fprintf(stdout, " == Rad*M_%llu(%u,%llu)", pi, m, j);

                    ideal_free(RMs[j]);
                    RMs[j] = NULL;
                }
            }
        }
        if (was_collision)
            fprintf(stdout, "\n");
    }

    /* do cleanup */
    dbg_msg_l(5, "Freeing Ms...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(Ms[i]);
    }
    free(Ms);

    dbg_msg_l(5, "Freeing Rad...\n");
    ideal_free(Rad);

    dbg_msg_l(5, "Freeing RMs...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(RMs[i]);
    }
    free(RMs);

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
            sscanf(optarg, "%u", &p);
            break;
        case 'l':
            sscanf(optarg, "%u", &l);
            break;
        case 'L':
            sscanf(optarg, "%u", &lambda);
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
