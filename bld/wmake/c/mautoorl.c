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
* Description:  Autodepend section processing using ORL.
*
****************************************************************************/


#if !defined( _M_I86 ) && !defined( BOOTSTRAP )

#include "make.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mautodep.h"
#include "orl.h"
#include "autodep.h"

#include "clibext.h"


typedef struct orl_info {
    int                 handle;
    orl_file_handle     file;
    UINT8               *buffer;
    DepInfo             *curr;
}                       orl_info;

static orl_info         orlInfo;
static orl_sec_handle   orlDependsInfo;
static orl_handle       orlHandle;
static char             *orlBuffer;
static size_t           orlFilePosition;
static size_t           orlFileSize;

// note: this should match name in rscobj.c in codegen
static const char       *dependSectionName = ".depend";


static size_t fileSize( int file_descriptor )
{
    long    old;
    size_t  size;

    old = tell( file_descriptor );
    lseek( file_descriptor, 0, SEEK_END );
    size = tell( file_descriptor );
    lseek( file_descriptor, old, SEEK_SET );
    if( size == 0 ) {
        // MallocSafe returns NULL for size == 0
        ++size;
    }
    return( size );
}


static void bufferInit( void )
/****************************/
{
    orlFilePosition = 0;
    orlFileSize = 0;
    orlBuffer = NULL;
}


static void *orlRead( void *file_handle, size_t bytes )
/*****************************************************/
{
    size_t  n;
    size_t  old_pos;

    if( orlBuffer == NULL ) {
        orlFileSize = fileSize( (int)(pointer_int)file_handle );
        orlBuffer = MallocSafe( orlFileSize );
        // just suck it right in :)
        n = read( (int)(pointer_int)file_handle, orlBuffer, orlFileSize );
        if( n != orlFileSize ) {
            return( NULL );
        }
    }
    if( (orlFilePosition + bytes) <= orlFileSize ) {
        old_pos = orlFilePosition;
        orlFilePosition += bytes;
        return( &orlBuffer[old_pos] );
    }
    return( NULL );
}


static size_t orlSeek( void *file_handle, size_t offset, int mode )
/*****************************************************************/
{
    (void)file_handle; // Unused
    switch( mode ) {
    case SEEK_SET:
        orlFilePosition = offset;
        break;
    case SEEK_CUR:
        orlFilePosition += offset;
        break;
    case SEEK_END:
        abort();       // not used by ORL - cheesy, I know
    }
    return( orlFilePosition );
}


static void AutoORLInit( void )
/*****************************/
{
    static orl_funcs    funcs = {
        orlRead,
        (long (*)( void *, long, int ))orlSeek, // cast as specified externally
        MallocSafe,
        FreeSafe
    };

    orlHandle = ORLInit( &funcs );
}


static orl_return findDependInfo( orl_sec_handle section )
/********************************************************/
{
    char    *name;

    name = ORLSecGetName( section );
    if( !strcmp( name, dependSectionName ) ) {
        orlDependsInfo = section;
        return( ORL_FALSE );
    }
    return( ORL_TRUE );
}


static UINT8 *orlGetDependsInfo( orl_file_handle file )
/*****************************************************/
{
    UINT8   *buffer;

    orlDependsInfo = NULL;
    buffer = NULL;
    ORLFileScan( file, NULL, findDependInfo );
    if( orlDependsInfo != NULL ) {
        ORLSecGetContents( orlDependsInfo, &buffer );
    }
    return( buffer );
}


static handle AutoORLFileInit( const char *name )
/***********************************************/
{
    orl_file_format type;
    orl_file_handle file;
    int             file_handle;
    UINT8           *buffer;

    bufferInit();
    file_handle = open( name, O_RDONLY | O_BINARY );
    if( file_handle != -1 ) {
        orlInfo.handle = file_handle;
        type = ORLFileIdentify( orlHandle, (void *)(pointer_int)file_handle );
        switch( type ) {
        case ORL_COFF:
        case ORL_ELF:
            file = ORLFileInit( orlHandle, (void *)(pointer_int)file_handle, type );
            if( file != NULL ) {
                orlInfo.file = file;
                buffer = orlGetDependsInfo( file );
                if( buffer != NULL ) {
                    orlInfo.buffer = buffer;
                    return( &orlInfo );
                }
            }
            break;
        case ORL_OMF:
        case ORL_UNRECOGNIZED_FORMAT:
            break;
        }
        if( orlBuffer != NULL ) {
            FreeSafe( orlBuffer );
            orlBuffer = NULL;
        }
        close( file_handle );
    }
    return( NULL );
}


static dep_handle AutoORLFirstDep( handle hdl )
/*********************************************/
{
    orl_info *hndl = hdl;

    hndl->curr = (void *)hndl->buffer;
    if( hndl->curr->len != 0 ) {
        return( hdl );
    }
    return( NULL );
}


static void AutoORLTransDep( dep_handle hdl, char **name, time_t *stamp )
/***********************************************************************/
{
    DepInfo *curr = ((orl_info *)hdl)->curr;

    *name = curr->name;
    *stamp = curr->time;
}


static dep_handle AutoORLNextDep( dep_handle hndl )
/*************************************************/
{
    DepInfo     *p;
    orl_info    *hdl = hndl;

    p = hdl->curr;
    p = (void *)((char *)p + sizeof( DepInfo ) + p->len - 1);
    if( p->len == 0 ) {
        hdl->curr = NULL;
        return( NULL );
    }
    hdl->curr = p;
    return( hdl );
}


static void AutoORLFileFini( handle hdl )
/***************************************/
{
    ORLFileFini( ((orl_info*)hdl)->file );
    close( ((orl_info*)hdl)->handle );
    FreeSafe( orlBuffer );
    orlBuffer = NULL;
}


static void AutoORLFini( void )
/*****************************/
{
    ORLFini( orlHandle );
}


const auto_dep_info ORLAutoDepInfo = {
    AutoORLInit,
    AutoORLFileInit,
    AutoORLFirstDep,
    AutoORLTransDep,
    AutoORLNextDep,
    AutoORLFileFini,
    AutoORLFini,
};

#endif
