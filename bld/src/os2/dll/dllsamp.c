#include <stdio.h>
#include <os2.h>

#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

unsigned APIENTRY LibMain( unsigned hmod, unsigned termination )
{
    if( termination ) {
        /* DLL is detaching from process */
    } else {
        /* DLL is attaching to process */
    }
    return( 1 );
}

EXTERNC void dll_entry_1( void )
{
    printf( "Hi from dll entry #1\n" );
}

EXTERNC void dll_entry_2( void )
{
    printf( "Hi from dll entry #2\n" );
}
