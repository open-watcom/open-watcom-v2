/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
#include "write.h"

bool ResWriteIconCurDirHeader( const IconCurDirHeader *head, FILE *fp )
/*********************************************************************/
{
    if( WRESWRITE( fp, head, sizeof( IconCurDirHeader ) ) != sizeof( IconCurDirHeader ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResReadIconCurDirHeader( IconCurDirHeader *head, FILE *fp )
/**************************************************************/
{
    size_t      numread;

    if( (numread = WRESREAD( fp, head, sizeof( IconCurDirHeader ) )) != sizeof( IconCurDirHeader ) )
        return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
    return( false );
}

bool ResWriteIconDirEntry( const IconDirEntry *entry, FILE *fp )
/**************************************************************/
{
    if( WRESWRITE( fp, entry, sizeof( IconDirEntry ) ) != sizeof( IconDirEntry ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResReadIconDirEntry( IconDirEntry *entry, FILE *fp )
/*******************************************************/
{
    size_t      numread;

    if( (numread = WRESREAD( fp, entry, sizeof( IconDirEntry ) )) != sizeof( IconDirEntry ) )
        return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
    return( false );
}

bool ResWriteCurDirEntry( const CurDirEntry *entry, FILE *fp )
/************************************************************/
{
    if( WRESWRITE( fp, entry, sizeof( CurDirEntry ) ) != sizeof( CurDirEntry ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResReadCurDirEntry( CurDirEntry *entry, FILE *fp )
/*****************************************************/
{
    size_t      numread;

    if( (numread = WRESREAD( fp, entry, sizeof( CurDirEntry ) )) != sizeof( CurDirEntry ) )
        return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
    return( false );
}

bool ResWriteCurHotspot( const CurHotspot *hotspot, FILE *fp )
/************************************************************/
{
    if( WRESWRITE( fp, hotspot, sizeof( CurHotspot ) ) != sizeof( CurHotspot ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteWinOldIconHeader( const IconDirEntry *entry, FILE *fp )
/******************************************************************/
{
    bool error;

    error = ResWriteUint8( fp, 0x01 ); // rnType
    if( !error )
        error = ResWriteUint8( fp, 0x01 ); // rnFlags. Magic undocumented bit 0 must be set or Windows 2.x shows the icon at full scale and randomly corrupts it.
    if( !error )
        error = ResWriteUint16( fp, 0x0000 ); // rnZero
    if( !error )
        error = ResWriteUint16( fp, 0x0000 ); // bmType
    if( !error )
        error = ResWriteUint16( fp, entry->Info.Width ); // bmWidth
    if( !error )
        error = ResWriteUint16( fp, entry->Info.Height ); // bmHeight
    if( !error )
        error = ResWriteUint16( fp, (((entry->Info.Width*entry->Info.BitCount+15u)&(~15u))/8u)/*WORD align*/ ); // bmWidthBytes
    if( !error )
        error = ResWriteUint8( fp, entry->Info.BitCount != 1 ? 1 : 0 ); // bmPlanes
    if( !error )
        error = ResWriteUint8( fp, entry->Info.BitCount != 1 ? entry->Info.BitCount : 0 ); // bmBitsPixel

    return( error );
}

bool ResWriteWinOldCursorHeader( const CurDirEntry *entry, const CurHotspot *hotspot, FILE *fp )
{
    bool error;

    error = ResWriteUint8( fp, 0x03 ); // rnType
    if( !error )
        error = ResWriteUint8( fp, 0x01 ); // rnFlags. Magic undocumented bit 0 must be set or Windows 2.x shows the icon at full scale and randomly corrupts it.
    if( !error )
        error = ResWriteUint16( fp, hotspot->X ); // rnZero
    if( !error )
        error = ResWriteUint16( fp, hotspot->Y ); // bmType
    if( !error )
        error = ResWriteUint16( fp, entry->Width ); // bmWidth
    if( !error )
        error = ResWriteUint16( fp, entry->Height ); // bmHeight
    if( !error )
        error = ResWriteUint16( fp, (((entry->Width*entry->BitCount+15u)&(~15u))/8u)/*WORD align*/ ); // bmWidthBytes
    if( !error )
        error = ResWriteUint8( fp, entry->BitCount != 1 ? 1 : 0 ); // bmPlanes
    if( !error )
        error = ResWriteUint8( fp, entry->BitCount != 1 ? entry->BitCount : 0 ); // bmBitsPixel

    return( error );
}

