#include "fail.h"

struct V {
    double d;
    V() : d(3.14) {
    }
};

struct D : virtual V {
    double e;
    D() : e(2.71) {
    }
} b;

double *d = &b.d;
double *e = &b.e;

int main() {
    if( *d < 3.135 || *d > 3.145 ) fail(__LINE__);
    if( *e < 2.705 || *e > 2.715 ) fail(__LINE__);
    _PASS;
}
