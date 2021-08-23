#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "pctcp.h"

int sock_printf( sock_type *sk, const char *fmt, ... )
{
    char    buf[tcp_MaxBufSize];
    int     len;
    va_list args;

    va_start (args, fmt);
    len = vsnprintf( buf, sizeof( buf ), fmt, args );
    if( len < 0 || len > sizeof( buf ) - 1 ) {
        outsnl( _LANG( "ERROR: sock_printf() overrun" ) );
    }
    sock_puts(sk, (const BYTE *)buf);
    va_end( args );
    return( len );
}

