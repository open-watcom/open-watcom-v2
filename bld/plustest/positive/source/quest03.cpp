// This tests nasty interactions between destruction, ?:, and ellipsis functions
//
#include "fail.h"

#include <stdarg.h>

int scount = 0;


struct S {
    unsigned a[3];
    ~S()
    {
        --scount;
    }
    S( const char * str )
    {
        ++ scount;
        a[0] = str[0];
        a[1] = str[1];
        a[2] = str[2];
    }
    S( const S & src )
    {
        ++ scount;
        a[0] = src.a[0];
        a[1] = src.a[1];
        a[2] = src.a[2];
    }
};

int foo( int a, ...  )
{
    va_list list;
    va_start( list, a );
    S sv = va_arg( list, S );
    va_end( list );
    return a + sv.a[ 1 ];
}

int ack(int i, S *x) {
    return foo( i, i ? "56" : *x );
}

int ack(int i, S *x, S *y) {
    return foo( i, i ? *y : *x );
}

int ack( S *x ) {
    return foo( 3, *x );
}


int main()
{
    {
        S sv1 = "87";
        S sv2 = "89";
        if( 3 + '9' != ack( &sv2 ) ) fail(__LINE__);
        if( 3 + '6' != ack( 3, &sv1 ) ) fail(__LINE__);
        if( 0 + '7' != ack( 0, &sv1 ) ) fail(__LINE__);
        if( 5 + '9' != ack( 5, &sv1, &sv2 ) ) fail(__LINE__);
        if( 0 + '7' != ack( 0, &sv1, &sv2 ) ) fail(__LINE__);
    }
    if( 0 != scount ) fail(__LINE__);
    _PASS;
}

