#include <stdio.h>
#include <stdlib.h>
#include "cwdll.h"

extern char *_LpPgmName;

int DllMain( int reason, char **dummy )
{
    int result;

    if( !reason ) {

        /*
        ** DLL initialisation.
        */
        printf( "DLL startup...\n" );
        printf( "DLL File name: %s\n", GetModuleFileName( _psp ) );
        printf( "Program name: %s\n", _LpPgmName );

        /* return zero to let the load continue */
        result = 0;

    } else {

        /*
        ** DLL clean up.
        */
        printf( "DLL shutdown...\n" );

        result = 0;
    }
    return( result );
}


void __export _cdecl SayHello( char *message )
{
    printf( "Recieved DLL Message: %s\n", message );
}
