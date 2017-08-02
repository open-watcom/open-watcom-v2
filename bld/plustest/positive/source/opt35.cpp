#include "fail.h"

int foo( signed char x ) {
    return x & 0x80;
}
int bar( signed char x ) {
    return ((int)x) & ((int)0x80);
}

int main() {
#if __WATCOM_REVISION__ >= 8
    if( foo( -1 ) != 0x0080 ) _fail;
#endif
    if( bar( -1 ) != 0x0080 ) _fail;
    _PASS;
}
