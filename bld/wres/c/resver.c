/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <time.h>
#include "layer0.h"
#include "filefmt.h"
#include "write.h"
#include "resver.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

bool ResWriteVerBlockHeader( VerBlockHeader * head, bool use_unicode, WResTargetOS res_os, FILE *fp )
/***************************************************************************************************/
/* Writes the header, correcting it for 32 bit alligning */
{
    bool        error;

    error = ResWriteUint16( head->Size, fp );
    if( !error ) {
        error = ResWriteUint16( head->ValSize, fp );
    }
    if( !error && res_os == WRES_OS_WIN32 ) {
        error = ResWriteUint16( head->Type, fp );
    }
    if( !error ) {
        error = ResWriteString( head->Key, use_unicode, fp );
    }
    if( !error ) {
        error = ResWritePadDWord( fp );
    }

    return( error );
}

size_t ResSizeVerBlockHeader( VerBlockHeader *head, bool use_unicode, WResTargetOS res_os )
/*****************************************************************************************/
{
    size_t      key_size;
    size_t      padding;
    size_t      fixed_size;

    key_size = strlen( head->Key ) + 1;
    if( use_unicode )
        key_size *= 2;
    if( res_os == WRES_OS_WIN32 ) {
        /* the NT key field begins 2 bytes from a 32 bit boundary */
        padding = RES_PADDING_DWORD( key_size + 2 );
        fixed_size = 3 * sizeof( uint_16 );
    } else {
        padding = RES_PADDING_DWORD( key_size );
        fixed_size = 2 * sizeof( uint_16 );
    }
    return( fixed_size + key_size + padding );
}

bool ResWriteVerValueItem( VerValueItem *item, bool use_unicode, FILE *fp )
/*************************************************************************/
{
    bool            error;

    error = false;
    if( item->IsNum ) {
        error = ResWriteUint16( item->Value.Num, fp );
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            error = ResWriteString( item->Value.String, use_unicode, fp );
        } else {
            error = ResWriteStringLen( item->Value.String, use_unicode, fp, item->strlen );
        }
    }
    return( error );
}

size_t ResSizeVerValueItem( VerValueItem * item, bool use_unicode )
/*****************************************************************/
{
    size_t  size;

    if( item->IsNum ) {
        size = sizeof( uint_16 );
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            size = strlen( item->Value.String ) + 1;
        } else {
            size = item->strlen + 1;
        }
        if( use_unicode ) {
            size = ConvToUnicode( size, item->Value.String, NULL );
        } else {
            size = ConvToMultiByte( size, item->Value.String, NULL );
        }
    }
    return( size );
}

bool ResWriteVerFixedInfo( VerFixedInfo *fixed, FILE *fp )
/********************************************************/
{
    fixed->Signature = VER_FIXED_SIGNATURE;
    fixed->StructVer = VER_FIXED_STRUCT_VER;
    fixed->FileDateLow = (uint_32)time( NULL );
    if( WRESWRITE( fp, fixed, sizeof( VerFixedInfo ) ) != sizeof( VerFixedInfo ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}
