#include "fail.h"

#if defined( _M_IX86 )

struct S {
    int s;
};

typedef struct S __based(__segname("_STACK")) *SS_PTR;

void set_to_one( SS_PTR p )
{
    p->s = 1;
}

#ifdef _M_I86
struct Q {
    int __based((__segment)__self)  *head;
    int __based((__segment)__self)  *tail;
};

#define TheQ1 ((struct Q __far *) 0x12348008)
#define TheQ2 ((struct Q __far *) 0x1234:>0x8008)

struct Q __far *fn1a( void )
{
    return( TheQ1 );
}

struct Q __far *fn1b( void )
{
    return( TheQ1 );
}

struct Q __far *fn2a( void )
{
    return( TheQ2 + 1 );
}

struct Q __far *fn2b( void )
{
    return( TheQ2 + 1 );
}
#endif

int main( void )
{
    struct S x;

    x.s = 0;
    set_to_one( &x );
    if( x.s != 1 ) fail(__LINE__);
#ifdef _M_I86
    if( fn1a() != fn1b() ) fail(__LINE__);
    if( fn1a() != (void __far *)0x12348008 ) fail(__LINE__);
    if( fn2a() != fn2b() ) fail(__LINE__);
    if( fn2a() != (void __far *)0x1234800C ) fail(__LINE__);
#endif
    _PASS;
}

#else

ALWAYS_PASS

#endif
