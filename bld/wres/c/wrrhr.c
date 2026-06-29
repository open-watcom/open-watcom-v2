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
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"


bool WResReadHeader( WResHeader *header, FILE *fp )
/*************************************************/
{
    char        tmp[sizeof( WResHeader_X )];
    char        *p;
    size_t      numread;

    if( WRESSEEK( fp, 0, SEEK_SET ) )
        return( WRES_ERROR( WRS_SEEK_FAILED ) );
    if( (numread = WRESREAD( fp, tmp, sizeof( tmp ) )) != sizeof( tmp ) )
        return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
    p = tmp;
    header->Magic[0] = MGET_LE_U32( p );
    p += sizeof( uint_32 );
    header->Magic[1] = MGET_LE_U32( p );
    p += sizeof( uint_32 );
    header->DirOffset = MGET_LE_U32( p );
    p += sizeof( uint_32 );
    header->NumResources = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    header->NumTypes = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    header->WResVer = MGET_LE_U16( p );
    if( WRESSEEK( fp, 0, SEEK_SET ) )
        return( WRES_ERROR( WRS_SEEK_FAILED ) );
    return( false );
}

bool WResReadExtHeader( WResExtHeader *extheader, FILE *fp )
/**********************************************************/
{
    char        tmp[sizeof( WResExtHeader )];
    char        *p;
    size_t      numread;

    if( (numread = WRESREAD( fp, tmp, sizeof( tmp ) )) != sizeof( tmp ) )
        return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
    p = tmp;
    extheader->TargetOS = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    extheader->reserved[0] = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    extheader->reserved[1] = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    extheader->reserved[2] = MGET_LE_U16( p );
    p += sizeof( uint_16 );
    extheader->reserved[3] = MGET_LE_U16( p );
    if( WRESSEEK( fp, 0, SEEK_SET ) )
        return( WRES_ERROR( WRS_SEEK_FAILED ) );
    return( false );
}
