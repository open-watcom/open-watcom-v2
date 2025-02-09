/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Librarian interface to ORL.
*
****************************************************************************/


#include "wlib.h"
#include <errno.h>

#include "clibext.h"


struct orl_io_struct {
    obj_file            ofile;
};

static orl_handle       ORLHnd;

static void *ORLObjRead( struct orl_io_struct *orlio, size_t len )
/****************************************************************/
{
    buf_list    *buf;

    buf = MemAlloc( len + sizeof( buf_list ) - 1 );
    if( LibRead( orlio->ofile.io, buf->buf, len ) != len ) {
        MemFree( buf );
        return( NULL );
    }
    buf->next = orlio->ofile.buflist;
    orlio->ofile.buflist = buf;
    return( buf->buf );
}

static int ORLObjSeek( struct orl_io_struct *orlio, long pos, int where )
/***********************************************************************/
{
    switch( where ) {
    case SEEK_SET:
        pos += orlio->ofile.offset;
        break;
    case SEEK_CUR:
        break;
    }
    LibSeek( orlio->ofile.io, pos, where );
    return( 0 );
}

static void *ORLObjAlloc( size_t size )
/*************************************/
{
    if( size == 0 )
        size = 1;
    return( MemAlloc( size ) );
}

static void ORLObjFree( void *ptr )
/*********************************/
{
    MemFree( ptr );
}

void FiniORLObj( void )
/*********************/
{
    ORLFini( ORLHnd );
}

void InitORLObj( void )
/*********************/
{
    ORLSetFuncs( orl_cli_funcs, ORLObjRead, ORLObjSeek, ORLObjAlloc, ORLObjFree );

    ORLHnd = ORLInit( &orl_cli_funcs );
    if( ORLHnd == NULL ) {
        longjmp( Env, 1 );
    }
}

static obj_file *DoOpenORLObjFile( libfile io, long offset, const char *name )
/****************************************************************************/
{
    struct orl_io_struct    *orlio;
    orl_file_format         format;

    orlio = MemAlloc( sizeof( *orlio ) );
    orlio->ofile.io = io;
    orlio->ofile.buflist = NULL;
    orlio->ofile.offset = offset;
    format = ORLFileIdentify( ORLHnd, orlio );
    switch( format ) {
    case ORL_COFF:
    case ORL_ELF:
        orlio->ofile.orl = ORLFileInit( ORLHnd, orlio, format );
        if( Options.libtype == WL_LTYPE_MLIB ) {
            if( (ORLFileGetFlags( orlio->ofile.orl ) & VALID_ORL_FLAGS) != VALID_ORL_FLAGS ) {
                FatalError( ERR_NOT_LIB, "64-bit or big-endian", ctext_WL_LTYPE_MLIB );
            }
        }
        if( orlio->ofile.orl == NULL ) {
            FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
        }
        break;
    case ORL_OMF:
    default:
        /*
         * unrecognized or ORL unused format
         */
        orlio->ofile.orl = NULL;
        break;
    }
    return( &orlio->ofile );
}

obj_file *OpenORLObjFile( const char *name )
/******************************************/
{
    libfile     io;

    io = LibOpen( name, LIBOPEN_READ );
    return( DoOpenORLObjFile( io, 0, name ) );
}

obj_file *OpenORLLibFile( libfile io, const char *name )
/******************************************************/
{
    return( DoOpenORLObjFile( io, LibTell( io ), name ) );
}

static void DoCloseORLObjFile( obj_file *ofile )
/**********************************************/
{
    buf_list    *list;

    if( ofile->orl != NULL ) {
        ORLFileFini( ofile->orl );
    }
    while( (list = ofile->buflist) != NULL ) {
        ofile->buflist = list->next;
        MemFree( list );
    }
}

void CloseORLObjFile( obj_file *ofile )
/*************************************/
{
    libfile     io;

    io = ofile->io;
    DoCloseORLObjFile( ofile );
    LibClose( io );
}

void CloseORLLibFile( obj_file *ofile )
/*************************************/
{
    DoCloseORLObjFile( ofile );
}
