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


#include <wlib.h>

static orl_handle       ORLHnd;
static orl_funcs        OrlFuncs;

static void * ObjRead( obj_file *file, int len )
/************************************************/
{
    buf_list *  buf;

    buf = MemAlloc( len + sizeof(buf_list) - 1 );
    if( LibRead( file->hdl, buf->buf, len ) != len ) {
        MemFree( buf );
        return( NULL );
    }
    buf->next = file->buflist;
    file->buflist = buf;
    return( buf->buf );
}

static long ObjSeek( obj_file *file, long pos, int where )
/***********************************************************/
{
    switch( where ) {
    case SEEK_SET:
        where += file->offset;
        break;
    case SEEK_CUR:
        break;
    }
    LibSeek( file->hdl, pos, where );
    return( pos - file->offset );
}

static void *ObjAlloc( size_t size )
/**********************************/
{
    if( size == 0 ) size = 1;
    return( MemAllocGlobal( size ) );
}

static void ObjFree( void *ptr )
{
    MemFreeGlobal( ptr );
}

void FiniObj( void )
/******************/
{
    ORLFini( ORLHnd );
}

void InitObj( void )
/******************/
{

    OrlFuncs.read = &ObjRead;
    OrlFuncs.seek = &ObjSeek;
    OrlFuncs.alloc = &ObjAlloc;
    OrlFuncs.free = &ObjFree;
    ORLHnd = ORLInit( &OrlFuncs );
    if( ORLHnd == NULL ) {
        longjmp( Env , 1);
    }
}


static obj_file *DoOpenObjFile( char *name, libfile hdl, long offset )
/************************************************************************/
{
    obj_file            *file;
    orl_file_format     format;

    file = MemAlloc( sizeof( *file ) );
    file->hdl = hdl;
    file->buflist = NULL;
    file->offset = offset;
    format = ORLFileIdentify( ORLHnd, file );
    switch( format ) {
        case ORL_COFF: case ORL_ELF:
            file->orl = ORLFileInit( ORLHnd, file, format );
            if( Options.libtype == WL_TYPE_MLIB ) {
                if( (ORLFileGetFlags( file->orl ) & VALID_ORL_FLAGS) != VALID_ORL_FLAGS ) {
                    FatalError( ERR_NOT_LIB, "64-bit or big-endian" );
                }
            }
            if( !file->orl ) {
                FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
            }
            break;

        default: // case ORL_UNRECOGNIZED_FORMAT:
            file->orl = NULL;
            break;
    }
    return( file );
}

obj_file *OpenObjFile( char *name )
{
    libfile     hdl;

    hdl = LibOpen( name, LIBOPEN_BINARY_READ );
    return( DoOpenObjFile( name, hdl, 0 ) );
}

obj_file *OpenLibFile( char *name, libfile hdl )
{
    return( DoOpenObjFile( name, hdl, LibTell( hdl ) ) );
}

static void DoCloseObjFile( obj_file *file )
{
    buf_list    *list,*next;

    if( file->orl != NULL ) {
        ORLFileFini( file->orl );
    }
    for( list = file->buflist; list != NULL; list = next ) {
        next = list->next;
        MemFree( list );
    }
    MemFree( file );
}

void CloseObjFile( obj_file *file )
{
    libfile     hdl;

    hdl = file->hdl;
    DoCloseObjFile( file );
    LibClose( hdl );
}

void CloseLibFile( obj_file *file )
{
    DoCloseObjFile( file );
}
