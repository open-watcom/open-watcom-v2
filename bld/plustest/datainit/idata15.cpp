#include <stdio.h>
#include "dump.h"
__wmi

struct B {
    int b1, b2, b3;
    B();
    B( int );
};
static B* pp = 0;
B::B()
{
    b1=1; b2=2; b3=3;
    if( pp == 0 ) {
	printf( "CTOR1\n" );
	pp = this;
    } else {
	printf( "CTOR1:%x\n", this - pp );
    }
}
B::B( int a )
{
    b1=a; b2=a; b3=a;
    if( pp == 0 ) {
	printf( "CTOR2\n" );
	pp = this;
    } else {
	printf( "CTOR2:%x\n", this - pp );
    }
}

#define MAX_1 3
#define MAX_2 9
B b1[MAX_1] = { B( 1 ), B( 2 ) };	// should pad out with B();
struct Q {
    Q() {
	pp = 0;
    }
} clear_pp;
B b2[MAX_2] = { B( 1 ), B( 2 ) };	// should pad out with runtime routine

void dump( B const &b )
{
    printf( "%d %d %d\n", b.b1, b.b2, b.b3 );
}

int main( void )
{
    int i;
    for(i = 0 ; i < MAX_1 ; i++ )
    {
	dump( b1[i] );
    }
    for(i = 0 ; i < MAX_2 ; i++ )
    {
	dump( b2[i] );
    }
    return 0;
}
