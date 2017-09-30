#include "fail.h"

//#define DBGING

#ifdef DBGING
#   include <stdio.h>
#   define DbgPrintf( a, b ) printf( a, b )
#else
#   define DbgPrintf( a, b )
#endif


static int count_down;
static int ctored;

void CountDown()
{
    if( count_down > 1000 ) fail( __LINE__ );
    if( count_down < 0 ) fail( __LINE__ );
    if( 0 == count_down ) {
        count_down = 1000;
        DbgPrintf( " throw", 0 );
        throw 1234;
    }
    -- count_down;
}


struct Temp {
    int value;
    Temp( int );
    Temp( Temp const & );
    ~Temp();
};

Temp::Temp( int v ) : value( v )
{
    DbgPrintf( " Temp[%d]", value );
    CountDown();
    ++ ctored;
}

Temp::Temp( Temp const &src ) : value( src.value )
{
    DbgPrintf( " Temp-copy[%d]", value );
    CountDown();
    ++ ctored;
}

Temp::~Temp( void )
{
    -- ctored;
    DbgPrintf( " ~Temp[%d]", value );
    CountDown();
}

struct Arg {
    int value;
    Arg( Temp );
    Arg( Arg const & );
    ~Arg();
};

Arg::Arg( Temp t ) : value( t.value )
{
    DbgPrintf( " Arg[%d]", value );
    CountDown();
    ++ ctored;
}

Arg::Arg( Arg const & src ) : value( src.value )
{
    DbgPrintf( " Arg-copy[%d]", value );
    CountDown();
    ++ ctored;
}

Arg::~Arg( void )
{
    -- ctored;
    DbgPrintf( " ~Arg[%d]", value );
    CountDown();
}

int foo( Arg, Arg )
{
    DbgPrintf( " foo", 0 );
    CountDown();
    return 97;
}

int poo()
{
    int iv = foo( Arg( Temp(1) ), Arg( Temp(2) ) );
    return iv;
}

int main()
{
    int done = 0;
    for( unsigned ctr = 0; !done; ++ctr ) {
        ctored = 0;
        count_down = ctr;
        DbgPrintf( "%3d", ctr );
        try {
            poo();
            done = 1;
        } catch( ... ) {
        }
        if( 0 != ctored ) fail( __LINE__ );
        DbgPrintf( "\n", 0 );
    }
    _PASS;
}
