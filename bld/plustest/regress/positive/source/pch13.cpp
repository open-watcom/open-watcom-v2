#if __WATCOM_REVISION__ >= 8
#include "pch13.h"
#else
#define A 1
#define B 2
#define C 3
#endif
#include "fail.h"

int main() {
    if( A != 1 ) _fail;
    if( B != 2 ) _fail;
    if( C != 3 ) _fail;
    _PASS;
}
