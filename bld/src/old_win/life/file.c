#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <malloc.h>
#include <sys/stat.h>
#include "life.h"
#include "filedlg.h"

BOOL GetFileName( char *name, int type, char *ext,
                  char *buff, int len )
/**************************************

    Get the name of a pattern for either loading or saving
*/
{
    FILEOPEN            of;
    HANDLE              dll;
#if defined(__WINDOWS_386__)
    HINDIR              h;
#else
    BOOL                (PASCAL FAR *gsn)( LPFILEOPEN );
#endif
    FARPROC             farp;
    DWORD               a1,a2,a3;
    BOOL                rc;

    dll = LoadLibrary( "filedlg.dll" );
    if( dll < 32 ) {
        MessageBox( NULL, "Could not find filedlg.dll!", "Life", MB_OK );
        return( FALSE );
    }
    farp = (FARPROC) GetProcAddress( dll, "GetFileName" );
#if defined(__WINDOWS_386__)
    h = GetIndirectFunctionHandle( farp, INDIR_PTR, INDIR_ENDLIST );
#else
    gsn = (void FAR *) farp;
#endif

    of.hwnd = WinHandle;
    buff[0] = 0;
    a1 = AllocAlias16( buff );
    of.name = (LPSTR) a1;
    of.namelen = len;
    a2 = AllocAlias16( name );
    of.title = (LPSTR) a2;
    of.type = type;
    a3 = AllocAlias16( ext );
    of.ext = (LPSTR) a3;
#if defined(__WINDOWS_386__)
    rc = InvokeIndirectFunction( h, &of );
#else
    rc = gsn( &of );
#endif
    if( rc ) rc = TRUE;
    else rc = FALSE;
    FlushMouse();
    FreeAlias16( a1 );
    FreeAlias16( a2 );
    FreeAlias16( a3 );
    FreeLibrary( dll );
    return( rc );
}

int CheckHeap()
{
        return( _heapchk() );

}
