#include "dump.h"


int near a;
int far b;
int huge c;

void f1( int near * ) GOOD;
void f1( int far * ) GOOD;

void g1( void )
{
    f1( &a );
    f1( &b );
    f1( &c );
}

void f2( int near & ) GOOD;
void f2( int huge & ) GOOD;

void g2( void )
{
    f2( a );
    f2( b );
    f2( c );
}

int main( void ) {
    g1();
    g2();
    CHECK_GOOD( 82 );
    return errors != 0;
}
