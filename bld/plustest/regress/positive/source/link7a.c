#include "fail.h"
#if defined( M_I86 ) || defined( M_I386 )

#include <stdio.h>

// this needs to keep working in 10.0
#pragma aux name_ack "__S__ack_v_i";
#pragma aux name_foo "__S__foo_v_i";
#pragma aux name_bar "__S__bar_v_i";

struct S {
    int __pragma("name_ack") ack();
    virtual int __pragma("name_foo") foo();
    static int __pragma("name_bar") bar();
};

struct T : S {
};

void foo( S *p )
{
    if( p->ack() != 'a' ) fail(__LINE__);
    if( p->foo() != 'f' ) fail(__LINE__);
    if( p->bar() != 'b' ) fail(__LINE__);
}

int main()
{
    T x;

    foo( &x );
    _PASS;
}

#else

ALWAYS_PASS

#endif
