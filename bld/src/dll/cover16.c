/*
 *  COVER16.C
 */

#include <stdio.h>
#include <windows.h>    /* required for all Windows applications */

typedef long (FAR PASCAL *FPROC)();

FPROC DLL_1;
FPROC DLL_2;

long FAR PASCAL __export Function1( short var1,
                                    long var2,
                                    short var3 )
{
    return( (long) DLL_1( var1, var2, var3 ) );
}

long FAR PASCAL __export Function2( long var1, short var2 )
{
    return( (long) DLL_2( var1, var2 ) );
}

#pragma off (unreferenced);
BOOL FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                         WORD wHeapSize, LPSTR lpszCmdLine )
#pragma on (unreferenced);
{
    HANDLE hlib;

    /* Do our DLL initialization */
    hlib = LoadLibrary( "vbdll32.dll" );
    if( hlib < 32 ) {
        MessageBox( NULL,
                    "Make sure your PATH contains VBDLL32.DLL",
                    "COVER16", MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    DLL_1 = (FPROC) GetProcAddress( hlib, "DLL1" );
    DLL_2 = (FPROC) GetProcAddress( hlib, "DLL2" );
    return( TRUE );
}
