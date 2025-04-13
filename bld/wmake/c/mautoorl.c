/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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

struct orl_io_struct {
    orl_info            orlInfo;
};

static struct orl_io_struct orlIO;
static orl_sec_handle       orlDependsInfo;
static orl_handle           orlHandle;
static char                 *orlBuffer;
static size_t               orlFilePosition;
static size_t               orlFileSize;

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

static void *orlRead( struct orl_io_struct *orlio, size_t bytes )
/***************************************************************/
{
    size_t  n;
    size_t  old_pos;

    if( orlBuffer == NULL ) {
        orlFileSize = (size_t)fileSize( orlio->orlInfo.fp );
        orlBuffer = MallocSafe( orlFileSize );
        // just suck it right in :)
        n = fread( orlBuffer, 1, orlFileSize, orlio->orlInfo.fp );
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

static int orlSeek( struct orl_io_struct *orlio, long offset, int mode )
/**********************************************************************/
{
    /* unused parameters */ (void)orlio;

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


static UINT8 *orlGetDependsInfo( struct orl_io_struct *orlio )
/************************************************************/
{
    UINT8   *buffer;

    orlDependsInfo = NULL;
    buffer = NULL;
    ORLFileScan( orlio->orlInfo.orl_handle, NULL, findDependInfo );
    if( orlDependsInfo != NULL ) {
        ORLSecGetContents( orlDependsInfo, &buffer );
    }
    return( buffer );
}


static handle AutoORLFileInit( const char *name )
/***********************************************/
{
    orl_file_format         type;
    orl_file_handle         orl_handle;
    UINT8                   *buffer;

    bufferInit();
    orlIO.orlInfo.fp = fopen( name, "rb" );
    if( orlIO.orlInfo.fp != NULL ) {
        type = ORLFileIdentify( orlHandle, &orlIO );
        switch( type ) {
        case ORL_COFF:
        case ORL_ELF:
            orl_handle = ORLFileInit( orlHandle, &orlIO, type );
            if( orl_handle != NULL ) {
                orlIO.orlInfo.orl_handle = orl_handle;
                buffer = orlGetDependsInfo( &orlIO );
                if( buffer != NULL ) {
                    orlIO.orlInfo.buffer = buffer;
                    return( &orlIO );
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
        fclose( orlIO.orlInfo.fp );
    }
    return( NULL );
}


static dep_handle AutoORLFirstDep( handle hdl )
/*********************************************/
{
    (void)hdl;
    orlIO.orlInfo.curr = (void *)orlIO.orlInfo.buffer;
    if( orlIO.orlInfo.curr->len != 0 ) {
        return( &orlIO );
    }
    return( NULL );
}


static void AutoORLTransDep( dep_handle hdl, char **name, time_t *stamp )
/***********************************************************************/
{
    (void)hdl;
    *name = orlIO.orlInfo.curr->name;
    *stamp = orlIO.orlInfo.curr->time;
}


static dep_handle AutoORLNextDep( dep_handle hdl )
/************************************************/
{
    DepInfo     *p;

    (void)hdl;
    p = orlIO.orlInfo.curr;
    p = (void *)( (char *)p + offsetof( DepInfo, name ) + p->len );
    if( p->len == 0 ) {
        orlIO.orlInfo.curr = NULL;
        return( NULL );
    }
    orlIO.orlInfo.curr = p;
    return( &orlIO );
}


static void AutoORLFileFini( handle hdl )
/***************************************/
{
    (void)hdl;
    ORLFileFini( orlIO.orlInfo.orl_handle );
    fclose( orlIO.orlInfo.fp );
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
