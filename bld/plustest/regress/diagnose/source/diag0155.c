struct A;

struct S {
    A foo( int );
};

struct A {
    virtual void foo() = 0;
};

void foo( S *p )
{
    p->foo( 1 );
}

A S::foo( int x )
{
    A y;
    return y;
}

#include <assert.h>

int x24()
{
    return 1;
    assert(0);
}

int x30,y30,z30;

void x32()
{
    sizeof( ++x30 ? y30 : z30 );
    sizeof( x30 ? ++y30 : z30 );
    sizeof( x30 ? y30 : ++z30 );
}

int (*x39)( int, int, ... );
int (*x40)( int, int, int );
void x41()
{
    x39( );
    x39( 1 );
    x39( 1, 2 );
    x39( 1, 2, 3 );
    x39( 1, 2, 3, 4 );
    x39( 1, 2, 3, 4, 5 );
    x40( );
    x40( 1 );
    x40( 1, 2 );
    x40( 1, 2, 3 );
    x40( 1, 2, 3, 4 );
    x40( 1, 2, 3, 4, 5 );
}

float x57[] = { 0129.5, 0128e5, 0129E5, 012.5, 012e5, 012E5, 01239, 000000 };

struct X59 {
    void foo();
};

void X59::foo()
{
    if( !(1.0) );
    if( -(1.0) );
    if( +(1.0) );
    if( ~(1) );
    if( !(1) );
    if( -(1) );
    if( +(1) );
    if( 1+1 );
    if( 1-1 );
    if( 1*1 );
    if( 1/1 );
    if( 1%2 );
    if( 1<<1 );
    if( 1<1 );
    if( this != 0 );
    if( this == 0 );
    if( (char) 0 );
    if( 1 , this );
    if( 1 ? this : this );
    if( 0 ? this : this );
}
