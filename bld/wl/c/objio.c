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
* Description:  Object file i/o interface routines
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "linkstd.h"
#include "pcobj.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "objio.h"


typedef struct {
    void *              buffer;
    unsigned long       pos;
    infilelist *        currfile;
} bufferedfile;

infilelist *    CachedLibFiles;
infilelist *    CachedFiles;

void ResetObjIO( void )
/****************************/
{
    CachedFiles = NULL;
    CachedLibFiles = NULL;
}

static infilelist * AllocEntry( char *name, path_entry *path )
/************************************************************/
{
    infilelist *        entry;

    _PermAlloc( entry, sizeof(infilelist) );
    entry->name = AddStringStringTable( &PermStrings, name );
    entry->path_list = path;
    entry->prefix = NULL;
    entry->handle = NIL_FHANDLE;
    entry->cache = NULL;
    entry->len = 0;
    entry->flags = 0;
    return entry;
}

infilelist * AllocFileEntry( char *name, path_entry * path )
/*****************************************************************/
{
    infilelist *        entry;

    entry = AllocEntry( name, path );
    entry->next = CachedFiles;
    CachedFiles = entry;
    return entry;
}

infilelist * AllocUniqueFileEntry( char *name, path_entry *path )
/**********************************************************************/
{
    infilelist *        entry;

    for( entry = CachedLibFiles; entry != NULL; entry = entry->next ) {
        if( FNAMECMPSTR( entry->name, name ) == 0 ) {
            return entry;       // we found 1 with the same name.
        }
    }
    entry = AllocEntry( name, path );   // didn't find one, so allocate a new 1
    if( CachedLibFiles == NULL ) {      // add libraries to the end of the
        CachedLibFiles = entry;         // regular cached files list.
        LinkList( &CachedFiles, entry );
    } else {
        LinkList( &CachedLibFiles, entry );
    }
    return entry;
}

bool CleanCachedHandles( void )
/************************************/
{
    infilelist *list;

    for( list = CachedFiles; list != NULL; list = list->next ) {
        if( !(list->flags & INSTAT_IN_USE) && list->handle != NIL_FHANDLE )break;
    }
    if( list == NULL ) return( FALSE );
    QClose( list->handle, list->name );
    list->handle = NIL_FHANDLE;
    return( TRUE );
}

#define LIB_SEARCH (INSTAT_USE_LIBPATH | INSTAT_LIBRARY)

static f_handle PathObjOpen( char * path_ptr, char *name, char *new_name,
                             infilelist *list )
/************************************************************************/
{
    f_handle    fp;

    fp = NIL_FHANDLE;
    for(;;) {
        list->prefix = path_ptr;
        if( !QMakeFileName( &path_ptr, name, new_name ) )
            break;
        fp = QObjOpen( new_name );
        if( fp != NIL_FHANDLE ) {
            break;
        }
    }
    return( fp );
}

bool DoObjOpen( infilelist *list )
/***************************************/
{
    char *      name;
    f_handle    fp;
    unsigned    err;
    char *      path_ptr;
    char        new_name[ PATH_MAX ];
    path_entry *searchpath;
    bool        haspath;

    name = list->name;
    if( list->handle != NIL_FHANDLE )
        return( TRUE );
    list->currpos = 0;
    haspath = QHavePath( name );
    if( haspath ) {                         // has path defined
        list->path_list = NULL;
        fp = QObjOpen( name );
    } else if( list->prefix != NULL ) {     // already searched path
        path_ptr = list->prefix;
        QMakeFileName( &path_ptr, name, new_name );
        fp = QObjOpen( new_name );
    } else {                                // new, no searched path
        fp = NIL_FHANDLE;
        if( (list->flags & LIB_SEARCH) || list->path_list == NULL ) {
            /* try in current directory */
            fp = QObjOpen( name );
        }
        if( fp == NIL_FHANDLE ) {
            for( searchpath = list->path_list; searchpath != NULL; searchpath = searchpath->next ) {
                fp = PathObjOpen( searchpath->name, name, new_name, list );
                if( fp != NIL_FHANDLE ) {
                    break;
                }
            }
            if( fp == NIL_FHANDLE && (list->flags & INSTAT_USE_LIBPATH) ) {
                fp = PathObjOpen( GetEnvString("LIB"), name, new_name, list );
            }
        }
    }
    if( fp != NIL_FHANDLE ) {
        if( !(list->flags & INSTAT_GOT_MODTIME) ) {
            list->modtime = QFModTime( fp );
        }
        list->handle = fp;
        return( TRUE );
    } else if( !(list->flags & INSTAT_NO_WARNING) ) {
        err = ( list->flags & INSTAT_OPEN_WARNING ) ?
                                        WRN+MSG_CANT_OPEN : ERR+MSG_CANT_OPEN;
        PrintIOError( err, "12", name );
        list->prefix = NULL;
        list->handle = NIL_FHANDLE;
    }
    return( FALSE );
}

unsigned_16 CalcAlign( unsigned_32 pos, unsigned_16 align )
/****************************************************************/
/* align file */
{
    unsigned_16 modulus;

    modulus = pos % align;
    if( modulus != 0 ) {
        modulus = align - modulus;  // go to boundary.
    }
    return( modulus );
}

void InitTokBuff( void )
/*****************************/
{
    TokSize = MAX_HEADROOM;
    _ChkAlloc( TokBuff, MAX_HEADROOM );
}

void FreeTokBuffs( void )
/******************************/
{
    if( TokBuff != NULL ) {
        _LnkFree( TokBuff );
        TokBuff = NULL;
    }
}

void BadObject( void )
/***************************/
{
    CurrMod->f.source->file->flags |= INSTAT_IOERR;
    LnkMsg( LOC+ERR+MSG_OBJ_FILE_ATTR, NULL );
}

void EarlyEOF( void )
/**************************/
{
    CurrMod->f.source->file->flags |= INSTAT_IOERR;
    Locator( CurrMod->f.source->file->name, NULL, 0 );
    LnkMsg( ERR+MSG_EARLY_EOF, NULL );
}
