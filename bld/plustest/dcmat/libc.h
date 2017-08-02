#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static const char *form( const char *format, ... )
{
    va_list args;
    static char buff[1024];

    va_start( args, format );
    vsprintf( buff, format, args );
    va_end( args );
    return buff;
}
