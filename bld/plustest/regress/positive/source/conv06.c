#include "fail.h"

#if __WATCOM_REVISION__ >= 8
void foo( const char *p )
{
    if( *p != 'a' ) fail(__LINE__);
}

void foo( const char * const *p )
{
    foo( *p );
}

void foo( const char * const * const *p )
{
    foo( *p );
}

void foo( const char * const * const * const * p )
{
    foo( *p );
}

void foo( const char * const * const * const * const * p )
{
    foo( *p );
}

void foo( const char * const * const * const * const * const * p )
{
    foo( *p );
}
char c;
char *pc = &c;
char **ppc = &pc;
char ***pppc = &ppc;
char ****ppppc = &pppc;
char *****pppppc = &ppppc;
char ******ppppppc = &pppppc;

void test( char x )
{
    c = x;
    foo( pc );
    foo( ppc );
    foo( pppc );
    foo( ppppc );
    foo( pppppc );
    foo( ppppppc );
    char const*cpc = pc;
    char const*const*cppc = ppc;
    char const*const*const*cpppc = pppc;
    char const*const*const*const*cppppc = ppppc;
    char const*const*const*const*const*cpppppc = pppppc;
    char const*const*const*const*const*const*cppppppc = ppppppc;
    foo( cpc );
    foo( cppc );
    foo( cpppc );
    foo( cppppc );
    foo( cpppppc );
    foo( cppppppc );
}
#else
#define test( a )
#endif

int main() {
    test( 'a' );
    _PASS;
}
