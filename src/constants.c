/* needed GLOBAL variables */

#include "constants.h"

void init_constants(void) {
    q  = pow_ull(p, l);
    pi = pow_ull(p, lambda);
    m  = l / lambda;
    numofMs  = m*(pi - 1) + 1;
    nilindex = l*(p - 1)  + 1;
}
