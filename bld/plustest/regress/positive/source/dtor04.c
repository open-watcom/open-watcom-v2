#include "fail.h"
// test detection of impossible calling conventions for R/T system 
//
// 93/08/19 -- J.W.Welch        -- defined
// 95/06/22 -- J.W.Welch        -- made pragmas intel-specific

#include <stdarg.h>
#include <stdio.h>

#if defined( M_I86 ) || defined( M_I386 )

#pragma aux REVERSED parm reverse;
#define __reversed __pragma( "REVERSED" )

#else

#define __reversed

#endif

#define MAX_SIZE 1000
void *ctored[MAX_SIZE];
unsigned index = 0;

void ctor( void* new_element )
{
    if( index >= MAX_SIZE ) {
        printf( "ERROR -- overflow\n" );
        fail(__LINE__);
    }
    ctored[ index++ ] = new_element;
}

void dtor( void* new_element )
{
    if( index == 0 ) {
        printf( "ERROR -- underflow\n" );
        fail(__LINE__);
    } else {
        --index;
    }
    if( new_element != ctored[ index ] ) {
        printf( "ERROR -- invalid dtor, this=%x, index=%i\n"
              , new_element
              , index );
        fail(__LINE__);
    }
}



struct SELL
{
    SELL( int = 1 ... );
#if 0
    SELL( SELL const & ... );       // not implemented yet
#else
    SELL( SELL const & );
#endif
    ~SELL();
};

SELL::SELL( int ... )
{
    ctor( (void*)this );
};

#if 0                               // not implemented yet
SELL::SELL( SELL const & ... )
#else
SELL::SELL( SELL const & )
#endif
{
    ctor( (void*)this );
};

SELL::~SELL()
{
    dtor( (void*)this );
};

struct SREV
{
    __reversed SREV();
    __reversed SREV( SREV const & );
    __reversed ~SREV();
};

SREV::SREV()
{
    ctor( (void*)this );
};

SREV::SREV( SREV const & )
{
    ctor( (void*)this );
};

SREV::~SREV()
{
    dtor( (void*)this );
};

struct SPAS
{
    __pascal SPAS();
    __pascal SPAS( SPAS const & );
    __pascal ~SPAS();
};

SPAS::SPAS( )
{
    ctor( (void*)this );
};

SPAS::SPAS( SPAS const & )
{
    ctor( (void*)this );
};

SPAS::~SPAS()
{
    dtor( (void*)this );
};

int main()
{
    SELL sell;
    SREV srev;
    SPAS spas;

    SELL sell1(sell);
    SREV srev1(srev);
    SPAS spas1(spas);

    SELL* psell = new SELL[5];
    SREV* psrev = new SREV[7];
    SPAS* pspas = new SPAS[9];

    delete[] pspas;
    delete[] psrev;
    delete[] psell;

    _PASS;
}
