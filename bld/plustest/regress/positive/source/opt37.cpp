#include "fail.h"

int add( short *x, short *y ) {
    return *x + *y;
}
int twice_add( short x, short y ) {
    return add(&x, &y) + add( &x, &y);
}

int main() {
    if( twice_add( 1, 8 ) != 18 ) _fail;
    _PASS;
}
