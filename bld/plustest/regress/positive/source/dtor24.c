#include "fail.h"
#include <stdio.h>

#if 0
    #define dbg_printf printf
#else
    void dbg_printf( ... ) {}
#endif


struct A {
    A();
    ~A();
    A( const A& );
};

const int MAX_CTORS = 100;

A* ctored[ MAX_CTORS ];
int index = 0;

void ERR( char* msg )
{
    printf( "FAILED -- %s\n", msg );
}

void CTOR( A* this_ptr )
{
    dbg_printf( "CTOR: %x\n", this_ptr );
    if( index >= MAX_CTORS ) {
        ERR( "overflow" );
	fail(__LINE__);
    } else {
        ctored[ index++ ] = this_ptr;
    }
}

void DTOR( A* this_ptr )
{
    dbg_printf( "DTOR: %x\n", this_ptr );
    if( index == 0 ) {
        ERR( "underflow" );
	fail(__LINE__);
    } else {
        if( ctored[ --index ] != this_ptr ) {
            ERR( "dtor out of order" );
	    fail(__LINE__);
        }
    }
}

A::A()
{
    CTOR( this );
}

A::A( const A&)
{
    CTOR( this );
}

A::~A()
{
    DTOR( this );
}



int foo( A )
{
    return sizeof( A );
}

int goo()
{
    return 3;
}


inline int inl2( A a )
{
    ( goo() ) ? goo() : foo( a );
    return sizeof( A );
}


inline int inl( A a )
{
    inl2( a );
    ( goo() ) ? goo() : foo( a );
    return sizeof( A );
}


int out( A a )
{
    inl( a );
    ( goo() ) ? goo() : foo( a );
    return sizeof( A );
}


int main()
{
    {   A av;
        out( av );
        inl( av );
        int i;
        for( i = 0; i < 2; ++i ) {
            i ? goo() : foo( av );
        }
    }
    if( index != 0 ) {
        ERR( "missed DTOR" );
	fail(__LINE__);
    }
    _PASS;
}
