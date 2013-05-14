/* This program will visualize diff between two given ideals: M_pi(m, i) and Rad^j.
 * Given two indexes i and j it will print the diff between M_pi(m, i) and Rad^j.
 * If only one index is specified it will construct the shortest chain consisting of
 * the specified ideal and two closest ideals from the other family of ideals. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "rmc/log.h"
#include "rmc/ideals.h"
#include "constants.h"
#include "color.h"

#define WAS_SPECIFIED_M             1
#define WAS_SPECIFIED_Rad           (1 << 1)

#define SPECIAL_COLOR   35 /* magenta */

#define MAX_LABEL_LENGTH    512

const char* package = "Utility to visualize diff between M_pi(m,k) and Rad^j";
const char* version = "1.0.0";
const char* progname = NULL;
unsigned char output_control = 0;
unsigned char use_colors = 0;

/* indexes of two ideals to compare */
unsigned long long i, j;

/* global debug level */
int debug = 0;

static int handle_cmdline(int *argc, char ***argv);

/* returns non-zero when s is a special number */
static int isspecial_number(unsigned long long s);

/* a function that does printing job for two given ideals M, N
 * N must be a subset of M, res is the same as in ideal_diff */
static void print_diff(IDEAL*res, IDEAL* M, IDEAL* N, \
        char* M_name, char* N_name, int with_colors);

int main(int argc, char **argv) {
    IDEAL* pp;
    IDEAL** Ms;
    IDEAL** Rads;

    char M_name[MAX_LABEL_LENGTH];
    char N_name[MAX_LABEL_LENGTH];

    /* learn who we really are */
    progname = (const char *)strrchr(argv[0], '/');
    progname = progname ? (progname + 1) : argv[0];

    /* handle cmdline */
    handle_cmdline(&argc, &argv);

    /* initializing needed things */
    init_constants();

    if ( !(i < numofMs) || !(j < nilindex) ) {
        fprintf(stderr, "i must be less than numofMs = m*(pi - 1) + 1, "
                        "j must be less than nilindex = l*(p - 1) + 1.\n");
        exit(EXIT_FAILURE);
    }

    /* we need at most 2 IDEALs per array */
    Ms   = (IDEAL**) malloc(2*sizeof(IDEAL*));
    Rads = (IDEAL**) malloc(2*sizeof(IDEAL*));

    if (Ms == NULL || Rads == NULL) {
        fprintf(stderr, "Unable to allocate memory for ideals' arrays.\n");
        exit(EXIT_FAILURE);
    }

    /* do the job */
    if ((output_control & WAS_SPECIFIED_M) && (output_control & WAS_SPECIFIED_Rad)) {
        /* both M_pi(m, i) and Rad^j were specified, process only them*/
        pp = ideal_create(q);
        ideal_init(pp, pi, m, i);
        Ms[0] = pp;

        pp = ideal_create(q);
        ideal_init(pp, p, l, l*(p - 1) - j);
        Rads[0] = pp;

        sprintf(M_name, "M_%llu(%lu,%llu)", pi, m, i);
        sprintf(N_name, "Rad^%llu", j);

        if (ideal_issubset(Rads[0], Ms[0])) {
            print_diff(Ms[0], Ms[0], Rads[0], M_name, N_name, use_colors);

            if (debug >= 2) {
                fprintf(stdout, "%s\t\t=\n", N_name);
                if (use_colors) {
                    color_ideal_print(SPECIAL_COLOR, Rads[0], isspecial_number);
                } else {
                    ideal_print(Rads[0]);
                }
                fprintf(stdout, "\n");
            }
        } else if (ideal_issubset(Ms[0], Rads[0])) {
            print_diff(Rads[0], Rads[0], Ms[0], N_name, M_name, use_colors);

            if (debug >= 2) {
                fprintf(stdout, "%s\t\t=\n", M_name);
                if (use_colors) {
                    color_ideal_print(SPECIAL_COLOR, Ms[0], isspecial_number);
                } else {
                    ideal_print(Ms[0]);
                }
                fprintf(stdout, "\n");
            }
        } else {
            /* we will store intersection here */
            pp = ideal_create(q);
            Ms[1] = pp;

            if (debug >= 2) {
                fprintf(stdout, "%s\t\t=\n", M_name);
                if (use_colors) {
                    color_ideal_print(SPECIAL_COLOR, Ms[0], isspecial_number);
                } else {
                    ideal_print(Ms[0]);
                }
                fprintf(stdout, "\n");
            }

            if (debug >= 2) {
                fprintf(stdout, "%s\t\t=\n", N_name);
                if (use_colors) {
                    color_ideal_print(SPECIAL_COLOR, Rads[0], isspecial_number);
                } else {
                    ideal_print(Rads[0]);
                }
                fprintf(stdout, "\n");
            }

            ideal_intersect(Ms[1], Ms[0], Rads[0]);

            sprintf(N_name, "(M_%llu(%lu,%llu) /\\ Rad^%llu)", pi, m, i, j);

            sprintf(M_name, "M_%llu(%lu,%llu)", pi, m, i);
            print_diff(Ms[0], Ms[0], Ms[1], M_name, N_name, use_colors);

            sprintf(M_name, "Rad^%llu", j);
            print_diff(Rads[0], Rads[0], Ms[1], M_name, N_name, use_colors);
        }
    } else if (output_control & WAS_SPECIFIED_Rad) {
        /* only Rad^j was specified, find sup and inf M_pi(m, k) and process all three of them */
        unsigned long long Mpi_to_Rad, Rad_to_Mpi;

        Rad_to_Mpi = maximum_Pi_for_P(l*(p - 1) - j, p, m);
        Mpi_to_Rad = minimum_Pi_for_P(l*(p - 1) - j, p, m, lambda);

        if (Mpi_to_Rad == Rad_to_Mpi) {
            fprintf(stdout, "Rad^%llu == M_%llu(%lu,%llu)\n\n",
                    j, pi, m, Rad_to_Mpi);
            goto end;
        }

        pp = ideal_create(q);
        ideal_init(pp, pi, m, Rad_to_Mpi);
        Ms[0] = pp;

        pp = ideal_create(q);
        ideal_init(pp, pi, m, Mpi_to_Rad);
        Ms[1] = pp;

        pp = ideal_create(q);
        ideal_init(pp, p, l, l*(p - 1) - j);
        Rads[0] = pp;

        fprintf(stdout, "M_%llu(%lu,%llu) --> Rad^%llu --> M_%llu(%lu,%llu)\n\n",
                pi, m, Mpi_to_Rad, j, pi, m, Rad_to_Mpi);

        sprintf(N_name, "Rad^%llu", j);

        sprintf(M_name, "M_%llu(%lu,%llu)", pi, m, Mpi_to_Rad);
        print_diff(Ms[1], Ms[1], Rads[0], M_name, N_name, use_colors);

        sprintf(M_name, "M_%llu(%lu,%llu)", pi, m, Rad_to_Mpi);
        print_diff(Ms[0], Rads[0], Ms[0], N_name, M_name, use_colors);

        if (debug >= 2) {
            fprintf(stdout, "%s\t\t=\n", N_name);
            if (use_colors) {
                color_ideal_print(SPECIAL_COLOR, Rads[0], isspecial_number);
            } else {
                ideal_print(Rads[0]);
            }
            fprintf(stdout, "\n");
        }
    } else if (output_control & WAS_SPECIFIED_M) {
        /* only M_pi(m, i) was specified, find sup and inf Rad^j and process all three of them */
        unsigned long long Mpi_to_Rad, Rad_to_Mpi;

        Rad_to_Mpi = minimum_P_for_Pi(i, p, m);
        Mpi_to_Rad = maximum_P_for_Pi(i, p, m, l, lambda, numofMs - 1);

        if (Mpi_to_Rad == Rad_to_Mpi) {
            fprintf(stdout, "M_%llu(%lu,%llu) == Rad^%llu\n\n",
                    pi, m, i, l*(p - 1) - Rad_to_Mpi);
            goto end;
        }

        pp = ideal_create(q);
        ideal_init(pp, p, l, Mpi_to_Rad);
        Rads[0] = pp;

        pp = ideal_create(q);
        ideal_init(pp, p, l, Rad_to_Mpi);
        Rads[1] = pp;

        pp = ideal_create(q);
        ideal_init(pp, pi, m, i);
        Ms[0] = pp;

        fprintf(stdout, "Rad^%llu --> M_%llu(%lu,%llu) --> Rad^%llu\n\n",
                l*(p - 1) - Rad_to_Mpi, pi, m, i, l*(p - 1) - Mpi_to_Rad);

        sprintf(M_name, "M_%llu(%lu,%llu)", pi, m, i);

        sprintf(N_name, "Rad^%llu", l*(p - 1) - Rad_to_Mpi);
        print_diff(Rads[1], Rads[1], Ms[0], N_name, M_name, use_colors);

        sprintf(N_name, "Rad^%llu", l*(p - 1) - Mpi_to_Rad);
        print_diff(Rads[0], Ms[0], Rads[0], M_name, N_name, use_colors);

        if (debug >= 2) {
            fprintf(stdout, "%s\t\t=\n", M_name);
            if (use_colors) {
                color_ideal_print(SPECIAL_COLOR, Ms[0], isspecial_number);
            } else {
                ideal_print(Ms[0]);
            }
            fprintf(stdout, "\n");
        }
    } else {
        fprintf(stderr, "Incorrect usage. See --help.\n");
    }

end:
    /* do cleanup */
    dbg_msg_l(5, "Freeing Ms...\n");
    for (i = 0; i < 2; ++i) {
        ideal_free(Ms[i]);
    }
    free(Ms);

    dbg_msg_l(5, "Freeing Rads...\n");
    for (i = 0; i < 2; ++i) {
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
        {"i", 1, 0, 'i'},
        {"j", 1, 0, 'j'},
        {"color", 0, 0, 'C'},
        {"debug", 0, 0, 'D'},
        {"version", 0, 0, 'v'},
        {"help", 0, 0, 'h'},
        {NULL, 0, 0, 0},
    };
    const char *opts_help[] = {
        "Specifies characteristic of field, must be a prime.",
        "Specifies size of field as an exponent of characteristic.",
        "Specifies series of ideals, can be any factor of exponent.",
        "Specifies index of the M_pi(m, k) ideal. If none will compare against closest.",
        "Specifies power of the Rad. If none will compare against closest.",
        "Use colors in output.",
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
            "p:l:L:i:j:CDvh", opts, NULL);
        if (k == -1) {
            break;
        }
        switch (k) {
        case 'p':
            sscanf(optarg, "%lu", &p);
            break;
        case 'l':
            sscanf(optarg, "%lu", &l);
            break;
        case 'L':
            sscanf(optarg, "%lu", &lambda);
            break;
        case 'i':
            sscanf(optarg, "%llu", &i);
            output_control |= WAS_SPECIFIED_M;
            break;
        case 'j':
            sscanf(optarg, "%llu", &j);
            output_control |= WAS_SPECIFIED_Rad;
            break;
        case 'C':
            use_colors = 1;
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
        fprintf(stderr, "You must specify at least one of i or j options. See --help.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

static int isspecial_number(unsigned long long s) {
    int isspecial = 1;
    unsigned long long res = 0;

    if (!s) return 0;

    while(s && isspecial) {
        res = s % pi;
        s /= pi;
        isspecial &= ((res % p) == (p - 1));
    }

    return isspecial;
}

static void print_diff(IDEAL* res, IDEAL* M, IDEAL* N, \
        char* M_name, char* N_name, int with_colors) {
    ideal_diff(res, M, N);
    fprintf(stdout, "%s \\ %s\t\t=", M_name, N_name);

    /* if debug is non-zero print also u_s array of diff */
    if (debug) {
        fprintf(stdout, "\n");

        if (with_colors) {
            color_ideal_print(SPECIAL_COLOR, res, isspecial_number);
        } else {
            ideal_print(res);
        }

        fprintf(stdout, "Indexes in diff:");
    }

    if (with_colors) {
        color_ideal_print_verbose(SPECIAL_COLOR, res, isspecial_number);
    } else {
        ideal_print_verbose(res);
    }
    fprintf(stdout, "\n");
}
