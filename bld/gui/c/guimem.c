/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef TRMEM
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/stat.h>
 #include "trmemcvr.h"
#endif

/*
 * GUIAlloc
 */

void * GUIAlloc( unsigned size )
{
#ifdef TRMEM
    return( TRMemAlloc( size ) );
#else
    return( malloc( size ) );
#endif
}

/*
 * GUIFree
 */

void GUIFree( void * ptr )
{
#ifdef TRMEM
    TRMemFree( ptr );
#else
    free( ptr );
#endif
}

/*
 * GUIMemOpen
 */

void GUIMemOpen()
{
#ifdef TRMEM
    static bool GUIMemOpened = FALSE;
    char * tmpdir;

    if( !GUIMemOpened ) {
        TRMemOpen();
        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TRMemRedirect( open( tmpdir, O_RDWR+O_CREAT+O_TRUNC+O_BINARY,
                           S_IWUSR+S_IRUSR ) );
        }
        GUIMemOpened = TRUE;
    }
#endif
}

/*
 * GUIMemClose
 */

void GUIMemClose( void )
{
#ifdef TRMEM
    TRMemPrtList();
    TRMemClose();
#endif
}

/*
 * GUIMemRealloc
 */

void * GUIRealloc( void * ptr, unsigned size )
{
#ifdef TRMEM
    return( TRMemRealloc( ptr, size) );
#else
    return( realloc( ptr, size ) );
#endif
}


/*
 * GUIMemReport
 */

void GUIMemPrtUsage( void )
{
#ifdef TRMEM
    TRMemPrtUsage();
#endif
}
