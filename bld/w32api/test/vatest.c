#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/* Exercise va_list usage. Functions which take an argument of type va_list
 * (but not *va_list) need special handling if va_list is an array type.
 */
 
void test_printf( char *buf, char *format, ... )
{
    va_list     args;

    va_start( args, format );
    wvsprintf( buf, format, args );
    va_end( args );
}

void test_fmtmsg( char *buf, char *format, ... )
{
    va_list     args;

    va_start( args, format );
    FormatMessage( FORMAT_MESSAGE_FROM_STRING, format, 0L, 0L, buf, 1024, &args );
    va_end( args );
}

int main( void )
{
    char        buf1[1024];
    char        buf2[1024];
    
    test_printf( buf1, "%%%u%hx%c%s", 1, (short)2, '3', "4" );
    test_fmtmsg( buf2, "%%%1!u!%2!hx!%3!c!%4", 1, (short)2, '3', "4" );
#if 0
    puts( buf1 );
    puts( buf2 );
#endif
    if( strcmp( buf1, buf2 ) ) {
        puts( "FAILED!" );
	return( EXIT_FAILURE );
    }
    puts( "SUCCESS" );
    return( EXIT_SUCCESS );
}
