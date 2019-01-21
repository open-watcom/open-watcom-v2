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


typedef struct orl_info {
    FILE                *fp;
    orl_file_handle     orl_handle;
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


static unsigned long fileSize( FILE *fp )
{
    unsigned long   old;
    unsigned long   size;

    old = ftell( fp );
    fseek( fp, 0, SEEK_END );
    size = ftell( fp );
    fseek( fp, old, SEEK_SET );
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

static void *orlRead( FILE *fp, size_t bytes )
/********************************************/
{
    size_t  n;
    size_t  old_pos;

    if( orlBuffer == NULL ) {
        orlFileSize = (size_t)fileSize( fp );
        orlBuffer = MallocSafe( orlFileSize );
        // just suck it right in :)
        n = fread( orlBuffer, 1, orlFileSize, fp );
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

static int orlSeek( FILE *fp, long offset, int mode )
/***************************************************/
{
    /* unused parameters */ (void)fp;

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
    return( 0 );
}


static void AutoORLInit( void )
/*****************************/
{
    ORLSetFuncs( orl_cli_funcs, orlRead, orlSeek, MallocSafe, FreeSafe );

    orlHandle = ORLInit( &orl_cli_funcs );
}


static orl_return findDependInfo( orl_sec_handle section )
/********************************************************/
{
    const char  *name;

    name = ORLSecGetName( section );
    if( strcmp( name, dependSectionName ) == 0 ) {
        orlDependsInfo = section;
        return( ORL_FALSE );
    }
    return( ORL_TRUE );
}


static UINT8 *orlGetDependsInfo( orl_file_handle orl_handle )
/***********************************************************/
{
    UINT8   *buffer;

    orlDependsInfo = NULL;
    buffer = NULL;
    ORLFileScan( orl_handle, NULL, findDependInfo );
    if( orlDependsInfo != NULL ) {
        ORLSecGetContents( orlDependsInfo, &buffer );
    }
    return( buffer );
}


static handle AutoORLFileInit( const char *name )
/***********************************************/
{
    orl_file_format type;
    orl_file_handle orl_handle;
    FILE            *fp;
    UINT8           *buffer;

    bufferInit();
    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        orlInfo.fp = fp;
        type = ORLFileIdentify( orlHandle, fp );
        switch( type ) {
        case ORL_COFF:
        case ORL_ELF:
            orl_handle = ORLFileInit( orlHandle, fp, type );
            if( orl_handle != NULL ) {
                orlInfo.orl_handle = orl_handle;
                buffer = orlGetDependsInfo( orl_handle );
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
        fclose( fp );
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
    ORLFileFini( ((orl_info*)hdl)->orl_handle );
    fclose( ((orl_info*)hdl)->fp );
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
    AUTO_DEP_ORL
};

#endif
