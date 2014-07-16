#include "pch03.h"

int main() {
    if( doo( 1 ) != -34 ) fail(__LINE__);
    if( doo( 9 ) != 1 ) fail(__LINE__);
    if( _CD_count != 2 ) fail(__LINE__);
    _PASS;
}
