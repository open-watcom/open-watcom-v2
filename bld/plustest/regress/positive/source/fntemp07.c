#include "fail.h"
#include <stdio.h>
#include <string.h>

// 94/08/05 -- J.W.Welch        -- was zapping a literal

template <class T>
    T twice( T x )
    {
        return x + x;
    }

int far *fp;

void x9()
{
    int x;

    x = 28;
    fp = &x;
    if( twice( *fp ) != (2*28) ) fail(__LINE__);
}

template <class T>
    void call_any_one_parm( T *p )
    {
        p( 'a' );
    }

void one_int( int v )
{
    if( v != 'a' ) fail(__LINE__);
}

void one_char( char v )
{
    if( v != 'a' ) fail(__LINE__);
}

void one_schar( signed char v )
{
    if( v != 'a' ) fail(__LINE__);
}

void x42()
{
    call_any_one_parm( one_int );
    call_any_one_parm( one_char );
    call_any_one_parm( &one_schar );
}

template <class T>
    T *zap( T *p )
    {
        *p = '@';
        return( p );
    }

void x63()
{
    int a[10];
    char temp[10];

    strcpy( temp, "asdf" );
    if( strcmp( zap( temp ), "@sdf" ) != 0 ) fail(__LINE__);
    memset( a, 0, sizeof( a ) );
    if( zap( a )[0] != '@' ) fail(__LINE__);
}

int main()
{
    x9();
    x42();
    x63();
    _PASS;
}
