#include "fail.h"
#include <stdio.h>

#include "exc01.h"

int wrong = 1;

int main( int argc, char *argv[] ) {
    try {
        C n;
    }
    catch (...) {
        -- wrong;
    }
    if( wrong != 0 ) fail(__LINE__);
    _PASS;
}
