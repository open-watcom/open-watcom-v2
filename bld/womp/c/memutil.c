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


#include <malloc.h>
#include <stdlib.h>
#include "watcom.h"
#include "womp.h"
#include "genutil.h"
#include "memutil.h"

#ifdef  TRACK
#include <malloc.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "trmem.h"

STATIC _trmem_hdl   memHandle;
STATIC int          memFile;     /* file handle we'll write() to */

STATIC void memLine( void *fh, const char *buf, unsigned size ) {

    write( 2, "***", 3 );
    write( 2, buf, size );
    if( *(int *)fh != -1 ) {
        write( *(int *)fh, buf, size );
    }
}
#endif  /* TRACK */

void MemInit( void ) {

#ifdef TRACK
    memFile = open( "mem.trk", O_WRONLY | O_CREAT | O_TRUNC, 0 );
    memHandle = _trmem_open( malloc, free, realloc, _TRMEM_NO_REALLOC,
        &memFile, memLine,
        _TRMEM_ALLOC_SIZE_0 |
        _TRMEM_FREE_NULL |
        _TRMEM_OUT_OF_MEMORY |
        _TRMEM_CLOSE_CHECK_FREE
    );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void MemFini( void ) {

#ifdef TRACK
    if( memHandle != NULL ) {
        _trmem_prt_list( memHandle );
        _trmem_close( memHandle );
        if( memFile != -1 ) {
            close( memFile );
        }
        memHandle = NULL;
    }
#endif
}

void *MemAlloc( size_t size ) {
/***************************/
    void *ptr;

#ifdef TRACK
    ptr = _trmem_alloc( size, _trmem_guess_who(), memHandle );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
    return( ptr );
}

void *MemRealloc( void *ptr, size_t size ) {
/****************************************/
    void *new;

#ifdef TRACK
    new = _trmem_realloc( ptr, size, _trmem_guess_who(), memHandle );
#else
    new = realloc( ptr, size );
#endif
    if( new == NULL && size != 0 ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
    return( new );
}

void MemFree( void *ptr ) {
/***********************/
#ifdef TRACK
    _trmem_free( ptr, _trmem_guess_who(), memHandle );
#else
    free( ptr );
#endif
}
