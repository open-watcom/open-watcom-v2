#include "pch05.h"

int main() {
    if( da1() != 4 ) fail(__LINE__);
    if( da2() != 4.875 ) fail(__LINE__);
    if( da3() != 7 ) fail(__LINE__);
    if( da4() != 12 ) fail(__LINE__);
    if( da5() != 4 ) fail(__LINE__);
    _PASS;
}
