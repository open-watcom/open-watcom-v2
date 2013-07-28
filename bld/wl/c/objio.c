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
    void            *buffer;
    unsigned long   pos;
    infilelist      *currfile;
} bufferedfile;

infilelist      *CachedLibFiles;
infilelist      *CachedFiles;

void ResetObjIO( void )
/****************************/
{
    CachedFiles = NULL;
    CachedLibFiles = NULL;
}

static infilelist *AllocEntry( char *name, path_entry *path )
/***********************************************************/
{
    infilelist  *entry;

    _PermAlloc( entry, sizeof( infilelist ) );
    entry->name = AddStringStringTable( &PermStrings, name );
    entry->path_list = path;
    entry->prefix = NULL;
    entry->handle = NIL_FHANDLE;
    entry->cache = NULL;
    entry->len = 0;
    entry->flags = 0;
    return entry;
}

infilelist *AllocFileEntry( char *name, path_entry *path )
/********************************************************/
{
    infilelist  *entry;

    entry = AllocEntry( name, path );
    entry->next = CachedFiles;
    CachedFiles = entry;
    return entry;
}

infilelist *AllocUniqueFileEntry( char *name, path_entry *path )
/**************************************************************/
{
    infilelist  *entry;

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
        if( !(list->flags & INSTAT_IN_USE) && list->handle != NIL_FHANDLE ) {
            break;
        }
    }
    if( list == NULL ) 
        return( FALSE );
    QClose( list->handle, list->name );
    list->handle = NIL_FHANDLE;
    return( TRUE );
}

char *MakePath( char *fullname, char **path_list )
/************************************************/
{
    char        *p;
    char        c;

    p = fullname;
    if( *path_list != NULL && (c = **path_list) != '\0' ) {
        do {
            ++(*path_list);
            if( IS_PATH_LIST_SEP( c ) ) {
                break;
            }
            *p++ = c;
        } while( (c = **path_list) != '\0' );
        if( p != fullname ) {
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
        }
    }
    return( p );
}

char *MakeFileName( infilelist *file, char *fullname )
/****************************************************/
{
    char    *path = file->prefix;

    strcpy( MakePath( fullname, &path ), file->name );
    return( path );
}

int MakeFileNameFromList( char **path_list, char *name, char *fullname )
/**********************************************************************/
{
    if( *path_list != NULL && **path_list != '\0' ) {
        strcpy( MakePath( fullname, path_list ), name );
        return( 1 );
    }
    return( 0 );
}

#define LIB_SEARCH (INSTAT_USE_LIBPATH | INSTAT_LIBRARY)

static f_handle PathObjOpen( char *path_ptr, char *name, char *new_name, infilelist *file )
/*****************************************************************************************/
{
    f_handle    fp;

    fp = NIL_FHANDLE;
    for( ;; ) {
        file->prefix = path_ptr;
        if( !MakeFileNameFromList( &path_ptr, name, new_name ) ) {
            file->prefix = NULL;
            break;
        }
        fp = QObjOpen( new_name );
        if( fp != NIL_FHANDLE ) {
            break;
        }
    }
    return( fp );
}

bool DoObjOpen( infilelist *file )
/********************************/
{
    char *      name;
    f_handle    fp;
    unsigned    err;
    char        new_name[ PATH_MAX ];
    path_entry *searchpath;

    name = file->name;
    if( file->handle != NIL_FHANDLE )
        return( TRUE );
    file->currpos = 0;
    if( HAS_PATH( name ) ) {   // has path defined
        file->path_list = NULL;
        fp = QObjOpen( name );
    } else if( file->prefix != NULL ) {     // already searched path
        MakeFileName( file, new_name );
        fp = QObjOpen( new_name );
    } else {                                // new, no searched path
        fp = NIL_FHANDLE;
        if( (file->flags & LIB_SEARCH) || file->path_list == NULL ) {
            /* try in current directory */
            fp = QObjOpen( name );
        }
        if( fp == NIL_FHANDLE ) {
            for( searchpath = file->path_list; searchpath != NULL; searchpath = searchpath->next ) {
                fp = PathObjOpen( searchpath->name, name, new_name, file );
                if( fp != NIL_FHANDLE ) {
                    break;
                }
            }
            if( fp == NIL_FHANDLE && (file->flags & INSTAT_USE_LIBPATH) ) {
                if( LibPath != NULL ) {
                    fp = PathObjOpen( LibPath, name, new_name, file );
                }
            }
        }
    }
    if( fp != NIL_FHANDLE ) {
        if( !(file->flags & INSTAT_GOT_MODTIME) ) {
            file->modtime = QFModTime( fp );
        }
        file->handle = fp;
        return( TRUE );
    } else if( !(file->flags & INSTAT_NO_WARNING) ) {
        err = ( file->flags & INSTAT_OPEN_WARNING ) ? WRN+MSG_CANT_OPEN : ERR+MSG_CANT_OPEN;
        PrintIOError( err, "12", name );
        file->prefix = NULL;
        file->handle = NIL_FHANDLE;
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
