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


#include "layer0.h"
#include "resiccu.h"
#include "reserr.h"
#include "wresrtns.h"

bool ResWriteIconCurDirHeader( const IconCurDirHeader *head, WResFileID handle )
/******************************************************************************/
{
    if( WRESWRITE( handle, head, sizeof( IconCurDirHeader ) ) != sizeof( IconCurDirHeader ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResReadIconCurDirHeader( IconCurDirHeader *head, WResFileID handle )
/***********************************************************************/
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, head, sizeof( IconCurDirHeader ) );
    if( numread != sizeof( IconCurDirHeader ) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteIconDirEntry( const IconDirEntry *entry, WResFileID handle )
/***********************************************************************/
{
    if( WRESWRITE( handle, entry, sizeof( IconDirEntry ) ) != sizeof( IconDirEntry ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResReadIconDirEntry( IconDirEntry *entry, WResFileID handle )
/****************************************************************/
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, entry, sizeof( IconDirEntry ) );
    if( numread != sizeof( IconDirEntry ) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteCurDirEntry( const CurDirEntry *entry, WResFileID handle )
/*********************************************************************/
{
    if( WRESWRITE( handle, entry, sizeof( CurDirEntry ) ) != sizeof( CurDirEntry ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResReadCurDirEntry( CurDirEntry *entry, WResFileID handle )
/**************************************************************/
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, entry, sizeof( CurDirEntry ) );
    if( numread != sizeof( CurDirEntry ) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteCurHotspot( const CurHotspot *hotspot, WResFileID handle )
/*********************************************************************/
{
    if( WRESWRITE( handle, hotspot, sizeof( CurHotspot ) ) != sizeof( CurHotspot ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}
