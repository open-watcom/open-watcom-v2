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

#include "clibext.h"


infilelist      *CachedLibFiles;
infilelist      *CachedFiles;

void ResetObjIO( void )
/****************************/
{
    CachedFiles = NULL;
    CachedLibFiles = NULL;
}

static infilelist *AllocEntry( const char *name, const path_entry *path )
/***********************************************************************/
{
    infilelist  *infile;

    _PermAlloc( infile, sizeof( infilelist ) );
    infile->name.u.ptr = AddStringStringTable( &PermStrings, name );
    infile->path_list = path;
    infile->prefix = NULL;
    infile->handle = NIL_FHANDLE;
    infile->cache = NULL;
    infile->len = 0;
    infile->status = 0;
    return( infile );
}

infilelist *AllocFileEntry( const char *name, const path_entry *path )
/********************************************************************/
{
    infilelist  *infile;

    infile = AllocEntry( name, path );
    infile->next = CachedFiles;
    CachedFiles = infile;
    return( infile );
}

infilelist *AllocUniqueFileEntry( const char *name, const path_entry *path )
/**************************************************************************/
{
    infilelist  *infile;

    for( infile = CachedLibFiles; infile != NULL; infile = infile->next ) {
        if( FNAMECMPSTR( infile->name.u.ptr, name ) == 0 ) {
            return( infile );    // we found 1 with the same name.
        }
    }
    infile = AllocEntry( name, path );   // didn't find one, so allocate a new 1
    if( CachedLibFiles == NULL ) {      // add libraries to the end of the
        CachedLibFiles = infile;         // regular cached files list.
        LinkList( &CachedFiles, infile );
    } else {
        LinkList( &CachedLibFiles, infile );
    }
    return( infile );
}

bool CleanCachedHandles( void )
/************************************/
{
    infilelist *infile;

    for( infile = CachedFiles; infile != NULL; infile = infile->next ) {
        if( (infile->status & INSTAT_IN_USE) == 0 && infile->handle != NIL_FHANDLE ) {
            break;
        }
    }
    if( infile == NULL )
        return( false );
    QClose( infile->handle, infile->name.u.ptr );
    infile->handle = NIL_FHANDLE;
    return( true );
}

char *MakePath( char *fullname, const char **path_list )
/******************************************************/
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
        c = p[-1];
        if( !IS_PATH_SEP( c ) ) {
            *p++ = DIR_SEP;
        }
    }
    return( p );
}

char *MakeFileName( infilelist *infile, char *fullname )
/******************************************************/
{
    const char  *path = infile->prefix;

    strcpy( MakePath( fullname, &path ), infile->name.u.ptr );
    return( (char *)path );
}

bool MakeFileNameFromList( const char **path_list, char *name, char *fullname )
/*****************************************************************************/
{
    if( *path_list != NULL && **path_list != '\0' ) {
        strcpy( MakePath( fullname, path_list ), name );
        return( true );
    }
    return( false );
}

static f_handle PathObjOpen( const char *path_ptr, char *name, char *new_name, infilelist *infile )
/*************************************************************************************************/
{
    f_handle    fp;

    fp = NIL_FHANDLE;
    for( ;; ) {
        infile->prefix = path_ptr;
        if( !MakeFileNameFromList( &path_ptr, name, new_name ) ) {
            infile->prefix = NULL;
            break;
        }
        fp = QObjOpen( new_name );
        if( fp != NIL_FHANDLE ) {
            break;
        }
    }
    return( fp );
}

bool DoObjOpen( infilelist *infile )
/**********************************/
{
    char                *name;
    f_handle            fp;
    unsigned            err;
    char                new_name[PATH_MAX];
    const path_entry    *searchpath;

    name = infile->name.u.ptr;
    if( infile->handle != NIL_FHANDLE )
        return( true );
    infile->currpos = 0;
    if( HAS_PATH( name ) ) {   // has path defined
        infile->path_list = NULL;
        fp = QObjOpen( name );
    } else if( infile->prefix != NULL ) {     // already searched path
        MakeFileName( infile, new_name );
        fp = QObjOpen( new_name );
    } else {                                // new, no searched path
        fp = NIL_FHANDLE;
        if( (infile->status & INSTAT_LIB_SEARCH) || infile->path_list == NULL ) {
            /* try in current directory */
            fp = QObjOpen( name );
        }
        if( fp == NIL_FHANDLE ) {
            for( searchpath = infile->path_list; searchpath != NULL; searchpath = searchpath->next ) {
                fp = PathObjOpen( searchpath->name, name, new_name, infile );
                if( fp != NIL_FHANDLE ) {
                    break;
                }
            }
            if( fp == NIL_FHANDLE && (infile->status & INSTAT_USE_LIBPATH) ) {
                if( LibPath != NULL ) {
                    fp = PathObjOpen( LibPath, name, new_name, infile );
                }
            }
        }
    }
    if( fp != NIL_FHANDLE ) {
        if( (infile->status & INSTAT_GOT_MODTIME) == 0 ) {
            infile->modtime = QFModTime( fp );
        }
        infile->handle = fp;
        return( true );
    } else if( (infile->status & INSTAT_NO_WARNING) == 0 ) {
        err = ( infile->status & INSTAT_OPEN_WARNING ) ? WRN+MSG_CANT_OPEN : ERR+MSG_CANT_OPEN;
        PrintIOError( err, "12", name );
        infile->prefix = NULL;
        infile->handle = NIL_FHANDLE;
    }
    return( false );
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
    CurrMod->f.source->infile->status |= INSTAT_IOERR;
    LnkMsg( LOC+ERR+MSG_OBJ_FILE_ATTR, NULL );
}

void EarlyEOF( void )
/**************************/
{
    CurrMod->f.source->infile->status |= INSTAT_IOERR;
    Locator( CurrMod->f.source->infile->name.u.ptr, NULL, 0 );
    LnkMsg( ERR+MSG_EARLY_EOF, NULL );
}
