#include "dump.h"

template<class T> void foo( T, T ) GOOD;

void foo( int, int );

void bar( float f, int i, char c ) {
    foo( f, f );
    foo( i, i );
    foo( c, c );
    foo( i, c );
    CHECK_GOOD( 44 );
}
int main( void ) {
    bar( 1.0, 2, 3 );
    return errors != 0;
}

void foo( int, int ) GOOD
