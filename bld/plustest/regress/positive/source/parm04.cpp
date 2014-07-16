#include "fail.h"
#include <stdarg.h>

void foo( int x, ... )
{
    va_list args;
    char *p;

    va_start( args, x );
    p = va_arg( args, char * );
    va_end( args );
    *p = 'b';
}

int main()
{
    char d;

    d = 'a';
    // near ptrs must be promoted to far pointers for large data models
    foo( 0, &d, 0, 0, 0, 0, 0, 0 );
    if( d != 'b' ) fail(__LINE__);
    _PASS;
}
