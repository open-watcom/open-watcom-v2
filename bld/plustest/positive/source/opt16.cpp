#include "fail.h"
#include <varargs.h>

void vararg_fn( int num, va_alist )
va_dcl
{
    va_list args;

    va_start( args );
    if( num != 10 ) fail(__LINE__);
    if( va_arg( args, void * ) != 0 ) fail(__LINE__);
    if( va_arg( args, double ) != 2.0 ) fail(__LINE__);
    if( va_arg( args, int ) != 3 ) fail(__LINE__);
    if( va_arg( args, double ) != 4.0 ) fail(__LINE__);
    if( va_arg( args, int ) != 5 ) fail(__LINE__);
    if( va_arg( args, double ) != 6.0 ) fail(__LINE__);
    if( va_arg( args, int ) != 7 ) fail(__LINE__);
    if( va_arg( args, double ) != 8.0 ) fail(__LINE__);
    if( va_arg( args, int ) != 9 ) fail(__LINE__);
    if( va_arg( args, double ) != 10.0 ) fail(__LINE__);
    if( va_arg( args, double ) != -1.25 ) fail(__LINE__);
    va_end( args );
}

int main() {
    vararg_fn( 10, 0, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0, -1.25 );
    _PASS;
}
