#include "pch07.h"

int main() {
    int *p = x();
    if( p == NULL ) fail(__LINE__);
    p[0] = -1;
    p[22] = -1;
    free( malloc( 1024 ) );
    _PASS;
}
