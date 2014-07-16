// Test for correct dtor order of temporaries

// #define DBG

#include "fail.h"

#define fail_here() fail( __LINE__ );

int order = 0;


struct INNER
{
    int val;
    INNER( int );
    ~INNER();
    INNER( INNER const & );
};

INNER::INNER( int v ) : val(v)
{
    #ifdef DBG
        printf( "INNER %d\n", val );
    #endif
    if( order != 0 || val != 17 ) fail_here();
    ++ order;
}

INNER::INNER( INNER const &s ) : val(s.val+1)
{
    #ifdef DBG
        printf( "INNER-COPY %d\n", val );
    #endif
    if( order != 1 || val != 18 ) fail_here();
    ++ order;
}

INNER::~INNER()
{
    #ifdef DBG
        printf( "~INNER %d\n", val );
    #endif
    switch( order ) {
      case 4:
        if( val != 18 ) fail_here();
        ++ order;
        break;
      case 5:
        if( val != 17 ) fail_here();
        ++ order;
        break;
      default :
        fail_here();
        break;
    }
}

struct OUTER
{
    INNER val;
    OUTER( INNER const & );
    ~OUTER();
};

OUTER::OUTER( INNER const &v )
    : val( v )
{
    #ifdef DBG
        printf( "OUTER %d\n", val.val );
    #endif
    if( order != 2 || val.val != 18 ) fail_here();
    ++ order;
}

OUTER::~OUTER()
{
    #ifdef DBG
        printf( "~OUTER %d\n", val.val );
    #endif
    if( order != 3 || val.val != 18 ) fail_here();
    ++ order;
}

void foo( OUTER const & o )
{
    #ifdef DBG
        printf( "foo %d\n", o.val.val );
    #else
        (void)&o;
    #endif
}

int main()
{
    foo( OUTER( INNER(17) ) );
    _PASS
}


//#pragma on ( dump_cg  dump_stab )
//#pragma on ( dump_exec_ic dump_stab )
