#include "fail.h"
#include <stdlib.h>
#include <stdio.h>

template <class T,int y,int*z>
    struct S;

template <class T,int y,int*z>
    struct W;

int a;

template <class X>
    void foo( S<W<X,2,&a>,1,&a> * );

template <class Z>
    void foo( S<W<Z,2,&a>,1,&a> * ) {}

void ack( S<W<int,2,&a>,1,&a> *p )
{
    foo( p );
}

void ack( S<W<double,2,&a>,1,&a> *p )
{
    foo( p );
}

int main()
{
    ack( ( S<W<int,2,&a>,1,&a> * ) 0 );
    ack( ( S<W<double,2,&a>,1,&a> * ) 0 );
    _PASS;
}
