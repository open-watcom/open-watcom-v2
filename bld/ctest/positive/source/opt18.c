#include "fail.h"
#include <limits.h>

unsigned long n;

void test() {
    if( (( n << 1 ) >> 1 ) != 0 ) fail(__LINE__);
}

int main() {
    n = LONG_MIN;
    test();
    n = 0;
    test();
    _PASS;
}
