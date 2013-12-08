/* needed GLOBAL variables */

#ifdef ENABLE_DEBUG
/* used for overflow/wrap checks */
#include <stdlib.h>
#include <limits.h>

#include "rmc/log.h"
#endif

#include "rmc/constants.h"

void init_constants(void) {
#ifdef ENABLE_DEBUG
    unsigned long long res;
    unsigned int i;

    /* p, l, lambda must be initialized with non-zero values at this point
     * applications must enforce this themselves */

    /* res = p^l */
    res = p;
    for (i = 1; i < l; ++i) {
        if (res >= ULLONG_MAX / p) {
            dbg_msg_l(0, "Warning! Value of q = p^l is greater than ULLONG_MAX.\n");
            exit(EXIT_FAILURE);
        }
        res *= p;
    }

    /* res = p^lambda */
    /* paranoid check because if q = p^l fits in ull then pi = p^lambda would too */
    res = p;
    for (i = 1; i < lambda; ++i) {
        if (res >= ULLONG_MAX / p) {
            dbg_msg_l(0, "Warning! Value of pi = p^lambda is greater than ULLONG_MAX.\n");
            exit(EXIT_FAILURE);
        }
        res *= p;
    }

    /* res = numofMs */
    m = l / lambda;
    if ((m == 0) || (res - 1 >= ULLONG_MAX / m)) {
        dbg_msg_l(0, "Warning! Value of numofMs - 1 = m*(pi - 1) is greater than ULLONG_MAX.\n");
        exit(EXIT_FAILURE);
    }
    res = m*(res - 1);
    if (res >= ULLONG_MAX - 1) {
        dbg_msg_l(0, "Warning! Value of numofMs = m*(pi - 1) + 1 is greater than ULLONG_MAX.\n");
        exit(EXIT_FAILURE);
    }

    /* res = nilindex - 1 */
    /* paranoid check because if numofMs fits in ull then nilindex would too */
    if (p - 1 >= ULLONG_MAX / l) {
        dbg_msg_l(0, "Warning! Value of nilindex - 1 = l*(p - 1) is greater than ULLONG_MAX.\n");
        exit(EXIT_FAILURE);
    }
    res = l*(p - 1);
    if (res >= ULLONG_MAX - 1) {
        dbg_msg_l(0, "Warning! Value of nilindex = l*(p - 1) + 1 is greater than ULLONG_MAX.\n");
        exit(EXIT_FAILURE);
    }

#ifdef ENABLE_GMP
    /* bin_coeff and m_k functions are vulnerable to overflows/wraps if not used with care */

    /* bin_coeff must work correctly with all possible u_s indices
     * therefore the greatest index must fit in long long
     * moreover bin_coeff uses mpz_bin_uiui internally so the greatest index must fit in ul as well */

    /* res = numofMs - 1 */
    res = pow_ull(p, lambda);
    res = m*(res - 1);
    if (res >= ULONG_MAX || res >= LLONG_MAX) {
        dbg_msg_l(0, "Warning! bin_coeff will not work correctly with the given p, l, lambda values. You should decrease some of them.\n");
        exit(EXIT_FAILURE);
    }

    /* m_k must work correctly with all possible u_s indices
     * input parameters' limits are already handled, but m_k uses bin_coeff internally
     * so some additional checks should be made:
     * 1. m + k - pi*j >= 0 && k - pi*j >= 0
     *   m + k - pi*j must fit in long long and ul
     *   therefore pi*m must satisfy the same conditions
     *   in this case k - pi*j will fit in long long and ul automatically
     * 2. m + k - pi*j < 0 && k - pi*j >= 0
     *   impossible since m >= 0
     * 3. k - pi*j < 0
     *   k - pi*j must fit in long long
     *   therefore -pi*m must satisfy the same conditions */
    if (res >= ULONG_MAX - m || res >= LLONG_MAX - m || res >= -(LLONG_MIN + m)) {
        dbg_msg_l(0, "Warning! m_k will not work correctly with the given p, l, lambda values. You should decrease some of them.\n");
        exit(EXIT_FAILURE);
    }
#endif

#endif

    q  = pow_ull(p, l);
    pi = pow_ull(p, lambda);
    m  = l / lambda;
    numofMs  = m*(pi - 1) + 1;
    nilindex = l*(p - 1)  + 1;
}
