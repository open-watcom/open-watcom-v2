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


#include <windows.h>
#include <string.h>

#include "wrglbl.h"
#include "wrmem.h"
#include "wresall.h"
#include "wrutil.h"
#include "wrnamoor.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

// UNICODE strings are always compacted
ResNameOrOrdinal *WR32Mem2NameOrOrdinal( void *data )
{
    ResNameOrOrdinal    *new;
    uint_16             *data16;
    char                *str;
    int                 stringlen;

    if( data == NULL ) {
        return( NULL );
    }

    data16 = (uint_16 *)data;

    if( *data16 == 0xffff ) {
        new = (ResNameOrOrdinal *)WRMemAlloc( sizeof( ResNameOrOrdinal ) );
        if( new == NULL ) {
            return( NULL );
        }
        new->ord.fFlag = 0xff;
        new->ord.wOrdinalID = data16[1];
    } else {
        str = NULL;
        WRunicode2mbcs( (char *)data, &str, &stringlen );
        new = (ResNameOrOrdinal *)str;
    }

    return( new );
}

ResNameOrOrdinal *WR16Mem2NameOrOrdinal( void *data )
{
    ResNameOrOrdinal    *new;
    uint_8              *data8;
    int                 stringlen;
    int                 len;

    if( data == NULL ) {
        return( NULL );
    }

    data8 = (uint_8 *)data;

    if( *data8 == 0xff ) {
        stringlen = 0;
        len = sizeof( ResNameOrOrdinal );
    } else {
        stringlen = strlen( (char *)data );
        len = stringlen + 1;
    }

    new = (ResNameOrOrdinal *)WRMemAlloc( max( len, sizeof( ResNameOrOrdinal ) ) );
    if( new == NULL ) {
        return( NULL );
    }

    if( *data8 != 0xff ) {
        memcpy( &new->name[0], data, len );
    } else {
        memcpy( new, data, sizeof( ResNameOrOrdinal ) );
    }

    return( new );
}

int WRNameOrOrd2Mem16( ResNameOrOrdinal *name, void **data, int *size )
{
    int         len;
    int         stringlen;

    if( name == NULL || data == NULL || size == NULL ) {
        return( FALSE );
    }

    if( name->ord.fFlag == 0xff ) {
        len = sizeof( ResNameOrOrdinal );
        stringlen = 0;
    } else {
        stringlen = strlen( &name->name[0] );
        len = stringlen + 1;
    }

    *data = WRMemAlloc( len );
    if( *data == NULL ) {
        return( FALSE );
    }

    if( name->ord.fFlag != 0xff ) {
        memcpy( *data, &name->name[0], len );
    } else {
        memcpy( *data, name, len );
    }

    *size = len;

    return( TRUE );
}

int WRNameOrOrd2Mem32( ResNameOrOrdinal *name, void **data, int *size )
{
    uint_16     *data16;
    char        *uni_str;

    if( name == NULL || data == NULL || size == NULL ) {
        return( FALSE );
    }

    if( name->ord.fFlag == 0xff ) {
        *size = sizeof( uint_16 ) + sizeof( uint_16 );
        *data = WRMemAlloc( *size );
    } else {
        uni_str = NULL;
        WRmbcs2unicode( &name->name[0], (char **)data, size );
    }

    if( *data == NULL ) {
        return( FALSE );
    }

    if( name->ord.fFlag == 0xff ) {
        data16 = (uint_16 *)*data;
        data16[0] = 0xffff;
        data16[1] = name->ord.wOrdinalID;
    }

    return( TRUE );
}

ResNameOrOrdinal * WR_EXPORT WRMem2NameOrOrdinal( void *data, int is32bit )
{
    ResNameOrOrdinal    *n;

    if( is32bit ) {
        n = WR32Mem2NameOrOrdinal( data );
    } else {
        n = WR16Mem2NameOrOrdinal( data );
    }

    return( n );
}

int WR_EXPORT WRNameOrOrd2Mem( ResNameOrOrdinal *name, int is32bit, void **data, int *size )
{
    if( is32bit ) {
        return( WRNameOrOrd2Mem32( name, data, size ) );
    } else {
        return( WRNameOrOrd2Mem16( name, data, size ) );
    }
}
