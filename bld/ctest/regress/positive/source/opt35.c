#include "fail.h"

int foo( signed char x ) {
    return x & 0x80;
}
int bar( signed char x ) {
    return ((int)x) & ((int)0x80);
}

int main() {
    if( foo( -1 ) != 0x0080 ) _fail;
    if( bar( -1 ) != 0x0080 ) _fail;
    _PASS;
}
