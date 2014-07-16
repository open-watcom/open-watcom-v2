#include "fail.h"
#include <float.h>

struct FP {
    static double eps() { return DBL_EPSILON; }
    static double min() { return DBL_MIN; }
    static double max() { return DBL_MAX; }
};

int main() {
    if( FP::eps() != DBL_EPSILON ) fail(__LINE__);
    if( FP::min() != DBL_MIN ) fail(__LINE__);
    if( FP::max() != DBL_MAX ) fail(__LINE__);
    _PASS;
}
