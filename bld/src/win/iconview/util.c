#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "view.h"

/*
 * UtilMalloc - keeps trying to allocate memory until either the malloc
 * succeeds or the user kills the icon viewer from the task manager.
 */

void *UtilMalloc( unsigned long bytes )
{
    void        *mem;
    char        buff[ 80 ];

    mem = NULL;
    while( ( mem = malloc( bytes ) ) == NULL ) {
        sprintf( buff,
           "Memory allocation failed - need %lu bytes.\nTry again?", bytes );
        MessageBox( FrameWindow, buff, NULL, MB_OK | MB_ICONQUESTION );
    }
    return( mem );
}

/*
 * UtilFree - does nothing, but is included in case UtilMalloc changes.
 */

void UtilFree( void *mem )
{
    free( mem );
}
