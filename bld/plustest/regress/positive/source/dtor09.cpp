#include "fail.h"
// verify destruction of catch variables

// #pragma on (dump_emit_ic)

int ctored = 0;
int caught = 0;

#define myfail( m ) ( puts( m ), fail(__LINE__) )

struct X {
    X()             { ++ctored; }
    X( const X& )   { ++ctored; }
    ~X()            { --ctored; }
};


int main()
{
//  printf( "start\n" );

    try {
        throw X();
    } catch( X x ) {
        int j = sizeof( x );
        ++ caught;
    } catch( ... ) {
        myfail( "missed catch of named item" );
    }
    if( ctored != 0 ) {
        myfail( "missed destruction after try[1]" );
        ctored = 0;
    }
    if( caught != 1 ) {
        myfail( "missed try[1]" );
    }

    try {
        throw X();
    } catch( X ) {
        ++ caught;
    } catch( ... ) {
        myfail( "missed catch of unnamed item" );
    }
    if( ctored != 0 ) {
        myfail( "missed destruction after try[2]" );
        ctored = 0;
    }
    if( caught != 2 ) {
        myfail( "missed try[2]" );
    }

    _PASS;
}
