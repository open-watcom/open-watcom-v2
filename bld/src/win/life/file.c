#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#define INCLUDE_COMMDLG_H       1
#include <windows.h>
#include "life.h"
#include "win1632.h"

BOOL GetFileName( char *name, BOOL save, char *ext,
                  char *buff, int len )
/**************************************/
{
    OPENFILENAME        of;
    char                filter[30];
    char                *ptr;
    BOOL                rc;

    buff[0] = '\0';
    ptr = filter;
    strcpy( ptr, ext );
    ptr += strlen( ptr ) + 1;
    strcpy( ptr, ext );
    ptr += strlen( ptr ) + 1;
    *ptr = '\0';
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = NULL;
    of.lpstrFilter = (LPSTR) filter;
    of.lpstrDefExt = "";
    of.nFilterIndex = 1L;
    of.lpstrFile = buff;
    of.nMaxFile = len;
    of.lpstrTitle = name;
    of.Flags = OFN_HIDEREADONLY;
    if( save ) {
        rc = GetSaveFileName( &of );
    } else {
        rc = GetOpenFileName( &of );
    }
    return( rc );
}

int CheckHeap()
{
        return( _heapchk() );

}
