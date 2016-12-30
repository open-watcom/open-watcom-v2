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
* Description:  Client callback routines for wres library.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "bool.h"
#include "wressetr.h"
#include "reserr.h"


static WResFileID res_open( const char *name, wres_open_mode omode )
{
    FILE    *fp;

    omode=omode;
    fp = fopen( name, "rb" );
    if( fp == NULL ) {
        WRES_ERROR( WRS_OPEN_FAILED );
    }
    return( WRES_FH2FID( fp ) );
}

static bool res_close( WResFileID fid )
{
    return( fclose( WRES_FID2FH( fid ) ) != 0 );
}

static size_t res_read( WResFileID fid, void *buf, size_t size )
{
    return( fread( buf, 1, size, WRES_FID2FH( fid ) ) );
}

static size_t res_write( WResFileID fid, const void *buf, size_t size )
{
    return( fwrite( buf, 1, size, WRES_FID2FH( fid ) ) );
}

static bool res_seek( WResFileID fid, WResFileOffset pos, int where )
{
    if( where == SEEK_SET ) {
        /* fool the wres library into thinking that the resource information starts at offset 0 */
        return( fseek( WRES_FID2FH( fid ), pos + WResFileShift, where ) != 0 );
    }
    return( fseek( WRES_FID2FH( fid ), pos, where ) != 0 );
}

static WResFileOffset res_tell( WResFileID fid )
{
    return( ftell( WRES_FID2FH( fid ) ) );
}

static bool res_ioerr( WResFileID fid, size_t rc )
{
    rc = rc;
    return( ferror( WRES_FID2FH( fid ) ) != 0 );
}

WResSetRtns( res_open, res_close, res_read, res_write, res_seek, res_tell, res_ioerr, malloc, free );
