#include "fail.h"

//#define DBGING

#ifdef DBGING
#   include <stdio.h>
#   define DbgPrintf( a, b ) printf( a, b )
#else
#   define DbgPrintf( a, b )
#endif


static int count_down;          // triggers throw
static int ctored;              // # outstanding objects to dtor
static int temps;               // # outstanding Temp objects to CTOR
static int args;                // # outstanding Arg objects to DTOR

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
    int order;
    Temp( int );
    Temp( Temp const & );
    ~Temp();
};

Temp::Temp( int v ) : value( v ), order( temps )
{
    DbgPrintf( " Temp[%d]", value );
    CountDown();
    ++ ctored;
    ++ temps;
}

Temp::Temp( Temp const &src ) : value( src.value ), order( -1 )
{
    DbgPrintf( " Temp-copy[%d]", value );
    CountDown();
    ++ ctored;
}

Temp::~Temp( void )
{
    -- ctored;
    if( order != -1 ) {
        -- temps;
        if( temps != order ) fail( __LINE__ );
    }
    DbgPrintf( " ~Temp[%d]", value );
    CountDown();
}

struct Arg {
    int value;
    int order;
    Arg( Temp );
    Arg( Arg const & );
    ~Arg();
};

Arg::Arg( Temp t ) : value( t.value ), order( t.order )
{
    DbgPrintf( " Arg[%d]", value );
    CountDown();
    ++ ctored;
    ++ args;
}

Arg::Arg( Arg const & src ) : value( src.value ), order( -1 )
{
    DbgPrintf( " Arg-copy[%d]", value );
    CountDown();
    ++ ctored;
    ++ args;
}

Arg::~Arg( void )
{
    -- ctored;
    -- args;
    if( order != -1 ) {
        if( temps < order ) fail( __LINE__ );
    }
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
    int iv = ( foo( Arg( Temp(1) )
             , Arg( Temp(2) ) )
             , args );
    return iv;
}

int main()
{
    int done = 0;
    for( unsigned ctr = 0; !done; ++ctr ) {
        temps = 0;
        ctored = 0;
        count_down = ctr;
        DbgPrintf( "%3d", ctr );
        try {
            int left = poo();
            DbgPrintf( " left(%d)", left );
            done = 1;
        } catch( ... ) {
        }
        if( 0 != ctored ) fail( __LINE__ );
        DbgPrintf( "\n", 0 );
    }
    _PASS;
}
