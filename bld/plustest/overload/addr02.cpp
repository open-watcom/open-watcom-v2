#include "dump.h"

void g( char ) BAD;
void g( short ) GOOD;

void f1( auto void (*fn)( short ) ) GOOD;
void f1( char ) BAD;
void f2( auto void (*fn)( char ) ) GOOD;
void f2( short ) BAD;

void foo1( void )
{
    f1( &g );
}

void foo2( void )
{
    f2( g );	// has to have & ?
}

void foo3( void )
{
    auto void (*a)( short );
    a = &g;
    a( 1 );
    f1( a );
}
int main( void ) {
    foo1();
    foo2();
    foo3();
    CHECK_GOOD( 24 );
    return errors != 0;
}
