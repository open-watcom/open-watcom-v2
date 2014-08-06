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


#include <string.h>
#include <time.h>
#include "layer0.h"
#include "filefmt.h"
#include "write.h"
#include "resver.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

extern int ResWriteVerBlockHeader( VerBlockHeader * head, uint_8 use_unicode,
                                        uint_8 os, WResFileID handle )
/***************************************************************************/
/* Writes the header, correcting it for 32 bit alligning */
{
    int         error;
    uint_16     tmp16;

    tmp16 = head->Size;
    error = ResWriteUint16( &tmp16, handle );
    if( !error ) {
        tmp16 = head->ValSize;
        error = ResWriteUint16( &tmp16, handle );
    }
    if( !error && os == WRES_OS_WIN32 ) {
        tmp16 = head->Type;
        error = ResWriteUint16( &tmp16, handle );
    }
    if( !error ) {
        error = ResWriteString( head->Key, use_unicode, handle );
    }
    if( !error ) {
        error = ResPadDWord( handle );
    }

    return( error );
}

extern uint_16 ResSizeVerBlockHeader( VerBlockHeader * head,
                                 uint_8 use_unicode, uint_8 os )
/**************************************************************/
{
    uint_16     key_size;
    uint_16     padding;
    uint_16     fixed_size;

    key_size = strlen( head->Key ) + 1;
    if( use_unicode ) key_size *= 2;
    if( os == WRES_OS_WIN32 ) {
        /* the NT key field begins 2 bytes from a 32 bit boundary */
        padding = RES_PADDING( key_size + 2, sizeof(uint_32) );
        fixed_size = 3 * sizeof( uint_16 );
    } else {
        padding = RES_PADDING( key_size, sizeof(uint_32) );
        fixed_size = 2 * sizeof( uint_16 );
    }
    return( fixed_size + key_size + padding );
}

extern int ResWriteVerValueItem( VerValueItem * item, uint_8 use_unicode,
                                            WResFileID handle )
/***********************************************************************/
{
    int             error;
    char            *convbuf;
    int             len;
    int             numwrote;
    uint_16         tmp16;

    error = FALSE;
    if( item->IsNum ) {
        tmp16 = item->Value.Num;
        error = ResWriteUint16( &tmp16, handle );
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            error = ResWriteString( item->Value.String, use_unicode, handle );
        } else {
            if( use_unicode ) {
                convbuf = WRESALLOC( 2 * item->strlen );
                len = (ConvToUnicode)( item->strlen, item->Value.String,
                                        convbuf );
            } else {
                len = item->strlen;
                convbuf = item->Value.String;
            }
            numwrote = WRESWRITE( handle, convbuf, len );
            if( numwrote != len ) {
                error = TRUE;
                WRES_ERROR( WRS_WRITE_FAILED );
            }
            if( use_unicode ) WRESFREE( convbuf );
        }
    }
    return( error );
}

extern uint_16 ResSizeVerValueItem( VerValueItem * item, uint_8 use_unicode )
/***************************************************************************/
{
    uint_16     size;

    if( item->IsNum ) {
        size = sizeof(uint_16);
    } else {
        if( item->strlen == VER_CALC_SIZE ) {
            size = strlen( item->Value.String ) + 1;
        } else {
            size = item->strlen;
        }
        if( use_unicode ) {
            size = (*ConvToUnicode)( size, item->Value.String, NULL);
        }
    }
    return( size );
}

extern int ResWriteVerFixedInfo( VerFixedInfo *fixed, WResFileID handle )
/***********************************************************************/
{
    int     numwrote;

    fixed->Signature = VER_FIXED_SIGNATURE;
    fixed->StructVer = VER_FIXED_STRUCT_VER;
    fixed->FileDateLow = time( NULL );
    numwrote = WRESWRITE( handle, fixed, sizeof(VerFixedInfo) );
    if( numwrote != sizeof(VerFixedInfo) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( TRUE );
    } else {
        return( FALSE );
    }
}
