/* logging facility */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef ENABLE_GMP
#include <gmp.h>
#endif

#include "rmc/log.h"

#ifdef WITH_MPI
#define MAX_OUTPUT_STR_LENGTH 1024

extern int mpi_me, mpi_total;

static int ret;
static char buf[MAX_OUTPUT_STR_LENGTH];
#endif

/* write message to stderr if debug >= 0 */
void dbg_msg(const char *fmt, ...)
{
#ifdef ENABLE_DEBUG
    if (debug) {
        va_list args;

        va_start(args, fmt);
#ifdef WITH_MPI
        ret = sprintf(buf, "%i'th thread says: ", mpi_me);
        vsprintf(buf + ret, fmt, args);
        fprintf(stderr, "%s", buf);
#else
        vfprintf(stderr, fmt, args);
#endif
        va_end(args);
    }
#endif
}

/* write message to stderr if debug >= level */
void dbg_msg_l(unsigned int level, const char *fmt, ...)
{
#ifdef ENABLE_DEBUG
    if (debug >= level) {
        va_list args;

        va_start(args, fmt);
#ifdef WITH_MPI
        ret = sprintf(buf, "%i'th thread says: ", mpi_me);
        vsprintf(buf + ret, fmt, args);
        fprintf(stderr, "%s", buf);
#else
        vfprintf(stderr, fmt, args);
#endif
        va_end(args);
    }
#endif
}

/* gmp analogs */
/* write message to stderr if debug >= 0 */
void gmp_dbg_msg(const char *fmt, ...)
{
#if defined ENABLE_DEBUG && defined ENABLE_GMP
    if (debug) {
        va_list args;

        va_start(args, fmt);
#ifdef WITH_MPI
        ret = sprintf(buf, "%i'th thread says: ", mpi_me);
        gmp_vsprintf(buf + ret, fmt, args);
        fprintf(stderr, "%s", buf);
#else
        gmp_vfprintf(stderr, fmt, args);
#endif
        va_end(args);
    }
#endif
}

/* write message to stderr if debug >= level */
void gmp_dbg_msg_l(unsigned int level, const char *fmt, ...)
{
#if defined ENABLE_DEBUG && defined ENABLE_GMP
    if (debug >= level) {
        va_list args;

        va_start(args, fmt);
#ifdef WITH_MPI
        ret = sprintf(buf, "%i'th thread says: ", mpi_me);
        gmp_vsprintf(buf + ret, fmt, args);
        fprintf(stderr, "%s", buf);
#else
        gmp_vfprintf(stderr, fmt, args);
#endif
        va_end(args);
    }
#endif
}
