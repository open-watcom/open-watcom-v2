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


/*
  OBJIO -- Object file i/o interface routines

*/

#include <stdio.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "objio.h"
#include "objcache.h"

extern bool CacheOpen( file_list *list )
/**************************************/
{
    infilelist *file;

    if( list == NULL ) return TRUE;
    file = list->file;
    if( file->flags & INSTAT_IOERR ) return FALSE;
    if( DoObjOpen( file ) ) {
        file->flags |= INSTAT_IN_USE;
    } else {
        file->flags |= INSTAT_IOERR;
        return( FALSE );
    }
    if( file->len == 0 ) {
        file->len = QFileSize( file->handle );
        if( file->len == 0 ) {
            LnkMsg( ERR+MSG_BAD_OBJECT, "s", file->name );
            file->flags |= INSTAT_IOERR;
            return( FALSE );
        }
    }
    if( file->cache == NULL ) {
        _ChkAlloc( file->cache, file->len );
        if( file->currpos != 0 ) {
            QLSeek( file->handle, 0, SEEK_SET, file->name );
        }
        QRead( file->handle, file->cache, file->len, file->name );
        file->currpos = file->len;
    }
    return TRUE;
}

extern void CacheClose( file_list *list, unsigned pass )
/******************************************************/
{
    infilelist *file;

    if( list == NULL ) return;
    file = list->file;
    file->flags &= ~INSTAT_IN_USE;
    switch( pass ) {
    case 1: /* first pass */
    case 2: /* second pass */
        if( !(file->flags & INSTAT_LIBRARY) ) {
            _LnkFree( file->cache );
            file->cache = NULL;
        }
        break;
    case 3: /* freeing structure */
        if( file->handle != NIL_HANDLE ) {
            QClose( file->handle, file->name );
            file->handle = NIL_HANDLE;
        }
        if( file->cache != NULL ) {
            _LnkFree( file->cache );
            file->cache = NULL;
        }
        break;
    }
}

extern void * CachePermRead( file_list * list, unsigned long pos, unsigned len )
/******************************************************************************/
{
    return CacheRead( list, pos, len );
}

extern void * CacheRead( file_list * list, unsigned long pos, unsigned len )
/**************************************************************************/
/* read len bytes out of the cache. */
// NYI: will need to make this more sophisticated for libraries
{
    if( pos + len > list->file->len ) return NULL;
    return (char *)list->file->cache + pos;
}

extern bool CacheEnd( file_list * list, unsigned long pos )
/*********************************************************/
{
    return pos >= list->file->len;
}

extern void CacheFini( void )
/***************************/
{
}

extern void CacheFree( file_list * list, void * blk )
/***************************************************/
{
    list = list;
    blk = blk;
}

extern void FreeObjCache( file_list *list )
/*****************************************/
{
    list = list;
}

extern bool DumpObjCache( void )
/******************************/
{
    return FALSE;
}
