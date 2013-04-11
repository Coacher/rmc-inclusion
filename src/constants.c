/* needed GLOBAL variables */

#include "constants.h"

void init_constants(void) {
    q  = pow_ul(p, l);
    pi = pow_ul(p, lambda);
    m  = l / lambda;
    numofMs  = m*(pi - 1) + 1;
    nilindex = l*(p - 1)  + 1;
}
