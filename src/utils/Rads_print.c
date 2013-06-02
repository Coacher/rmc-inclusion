/* A small utility to visualize Rads structure */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/ideals.h"
#include "constants.h"

const char* package = "Rads structure visualizer";
const char* version = "1.1.1";
const char* progname = NULL;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

int main(int argc, char **argv) {
    unsigned long long i;

    IDEAL* pp;
    IDEAL** Rads;

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    init_constants();

    Rads = (IDEAL**) malloc(nilindex*sizeof(IDEAL*));

    if (Rads == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideals' arrays.\n");
        exit(EXIT_FAILURE);
    }

    /* do the job */
    dbg_msg("Computing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        pp = ideal_create(q);
        ideal_init(pp, p, l, i);
        Rads[i] = pp;
    }

    for (i = 0; i < nilindex; ++i) {
        if (i)
            fprintf(stdout, "\n");

        fprintf(stdout, "M_%u(%u,%llu) = Rad^%llu\t=\n", p, l, i, l*(p - 1) - i);
        ideal_print(Rads[i]);
        if (debug && i) {
            ideal_diff(Rads[i - 1], Rads[i], Rads[i - 1]);
            fprintf(stdout, "M_%u(%u,%llu) \\ M_%u(%u,%llu)\t\t=", p, l, i, p, l, i - 1);
            ideal_print_verbose(Rads[i - 1]);
        }
    }

    /* do cleanup */
    dbg_msg_l(5, "Freeing Rads...\n");
    for (i = 0; i < nilindex; ++i) {
        ideal_free(Rads[i]);
    }
    free(Rads);

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
        "Specifies series of ideals, can be any factor of exponent.",
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

    if (l % lambda) {
        fprintf(stderr, "(L)ambda must be a factor of l. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
