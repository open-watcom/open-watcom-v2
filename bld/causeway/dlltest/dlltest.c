//
//
// A short and hopefully easy to understand demonstration of CauseWay DLL
// usage with Watcom.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "cwdllfnc.h"

extern char *_LpPgmName;

// Name the module. DLLS for stack version and DLLR for register
#ifdef __SW_3S
char ModuleName[]={"DLLS"};
#else
char ModuleName[]={"DLLR"};
#endif

//
//The actual do something code.
//
int main( void )
{
    unsigned char *DLL;

    void _cdecl (*DLLFunction)(char *);

    printf( "EXE File name: %s\n", GetModuleFileName( _psp ) );
    printf( "Program name: %s\n", _LpPgmName );
    printf( "Load module: %s\n", ModuleName );
    // Try and load the module.
    DLL = LoadModule( ModuleName );
    if( DLL ) {

        printf( "Module %s loaded sucessfully\n", ModuleName );

        // Fetch the test function address
        DLLFunction = GetProcAddress( DLL, "_SayHello" );

        if( DLLFunction ) {
            // Give the test function a shout
            DLLFunction( "Hello World!" );
        } else {
            printf( "Failed to GetProcAddress\n" );
        }

        printf( "Free module: %s\n", ModuleName );
        // Lose the module again
        FreeModule( DLL );
        printf( "Module %s discarded\n", ModuleName );

    } else {
        printf( "Failed to load %s module...\n", ModuleName );
    }
    return( 0 );
}
