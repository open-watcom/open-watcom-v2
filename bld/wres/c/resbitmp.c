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
#include "resbitmp.h"
#include "reserr.h"
#include "wresrtns.h"
#include "write.h"

bool ResWriteBitmapInfoHeader( BitmapInfoHeader *head, FILE *fp )
/***************************************************************/
{
    if( WRESWRITE( fp, head, sizeof( BitmapInfoHeader ) ) != sizeof( BitmapInfoHeader ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteWinOldBitmapHeader( BitmapInfoHeader *head, FILE *fp ) {
    bool error;

    error = ResWriteUint8( 0x02, fp ); // rnType
    if( !error )
        error = ResWriteUint8( 0x00, fp );
    if( !error )
        error = ResWriteUint16( 0x0000, fp ); // bmType
    if( !error )
        error = ResWriteUint16( head->Width, fp ); // bmWidth
    if( !error )
        error = ResWriteUint16( head->Height, fp ); // bmHeight
    if( !error )
        error = ResWriteUint16( (((head->Width*head->BitCount+15u)&(~15u))/8u)/*WORD align*/, fp ); // bmWidthBytes
    if( !error )
        error = ResWriteUint8( 1, fp ); // bmPlanes
    if( !error )
        error = ResWriteUint8( head->BitCount, fp ); // bmBitsPixel
    if( !error )
        error = ResWriteUint32( 0, fp ); // unknown zero field

    return( error );
}

