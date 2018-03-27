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
