#include "fail.h"

__int64 foo( __int64 x ) {
    return x * 2;
}
__int64 bar( __int64 x ) {
    return x / 3;
}

int main() {
    if( foo( 2 ) != 4 ) _fail;
    if( bar( 9 ) != 3 ) _fail;
    _PASS;
}
