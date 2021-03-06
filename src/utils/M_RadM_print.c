/* A small utility to visualize Ms and RadMs structure */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/ideals.h"
#include "rmc/constants.h"

#define WITH_Ms       1
#define WITH_RADMs    (1 << 1)

const char* package = "Ms and RadMs structure visualizer";
const char* version = "1.2.1";
const char* progname = NULL;
unsigned char output_control = 0;

/* global debug level */
unsigned int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i;

    IDEAL* Rad;
    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** RadMs;

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    init_constants();

    Ms    = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));
    RadMs = (IDEAL**) malloc(numofMs*sizeof(IDEAL*));

    if (Ms == NULL || RadMs == NULL) {
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

    dbg_msg("Computing RadMs...\n");
    for (i = 0; i < numofMs; ++i) {
        pp = ideal_create(q);
        ideal_product(pp, Rad, Ms[i], p);
        RadMs[i] = pp;
    }

    for (i = 0; i < numofMs; ++i) {
        if (i)
            fprintf(stdout, "\n");

        if (output_control & WITH_Ms) {
            fprintf(stdout, "M_%llu(%u,%llu)\t\t=\n", pi, m, i);
            ideal_print(Ms[i]);
            if (debug && i) {
                ideal_diff(Ms[i - 1], Ms[i], Ms[i - 1]);
                fprintf(stdout, "M_%llu(%u,%llu) \\ M_%llu(%u,%llu)\t\t= ", pi, m, i, pi, m, i - 1);
                ideal_print_verbose(Ms[i - 1]);
            }
        }

        if (output_control & WITH_RADMs) {
            fprintf(stdout, "Rad*M_%llu(%u,%llu)\t\t=\n", pi, m, i);
            ideal_print(RadMs[i]);
            if (debug && i) {
                ideal_diff(RadMs[i - 1], RadMs[i], RadMs[i - 1]);
                fprintf(stdout, "Rad*M_%llu(%u,%llu) \\ Rad*M_%llu(%u,%llu)\t\t= ", pi, m, i, pi, m, i - 1);
                ideal_print_verbose(RadMs[i - 1]);
            }
        }
    }

    /* do cleanup */
    dbg_msg_l(5, "Freeing Ms...\n");
    for (i = 0; i < numofMs; ++i) {
        ideal_free(Ms[i]);
    }
    free(Ms);

    dbg_msg_l(5, "Freeing Rad...\n");
    ideal_free(Rad);

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
        {"with_Ms", 0, 0, 'M'},
        {"with_RadMs", 0, 0, 'R'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies order of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent.",
        "Enable Ms output.",
        "Enable RadMs output.",
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
            "p:l:L:MRDvh", opts, NULL);
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
        case 'M':
            output_control |= WITH_Ms;
            break;
        case 'R':
            output_control |= WITH_RADMs;
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

    if (l % lambda) {
        fprintf(stderr, "(L)ambda must be a factor of l. See --help.\n");
        exit(EXIT_FAILURE);
    }

    if (!output_control) {
        fprintf(stderr, "You must specify at least one of M or R options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
