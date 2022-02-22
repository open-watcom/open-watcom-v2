/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include "wresall.h"
#include "wrutili.h"


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
static ResNameOrOrdinal *WRNameOrOrdinalFromData32( const void *data )
{
    ResNameOrOrdinal    *new;
    const uint_16       *data16;
    char                *str;
    size_t              stringlen;

    if( data == NULL ) {
        return( NULL );
    }

    data16 = (const uint_16 *)data;

    if( *data16 == 0xffff ) {
        new = (ResNameOrOrdinal *)MemAlloc( sizeof( ResNameOrOrdinal ) );
        if( new == NULL ) {
            return( NULL );
        }
        new->ord.fFlag = 0xff;
        new->ord.wOrdinalID = data16[1];
    } else {
        str = NULL;
        WRunicode2mbcs( data, &str, &stringlen );
        new = (ResNameOrOrdinal *)str;
    }

    return( new );
}

static ResNameOrOrdinal *WRNameOrOrdinalFromData16( const void *data )
{
    ResNameOrOrdinal    *new;
    const uint_8        *data8;
    size_t              stringlen;
    size_t              len;
    size_t              len1;

    if( data == NULL ) {
        return( NULL );
    }

    data8 = (const uint_8 *)data;

    if( *data8 == 0xff ) {
        stringlen = 0;
        len = sizeof( ResNameOrOrdinal );
    } else {
        stringlen = strlen( (const char *)data );
        len = stringlen + 1;
    }

    len1 = len;
    if( len1 < sizeof( ResNameOrOrdinal ) )
        len1 = sizeof( ResNameOrOrdinal );
    new = (ResNameOrOrdinal *)MemAlloc( len1 );
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

static bool WRDataFromNameOrOrdinal16( ResNameOrOrdinal *name, void **data, size_t *size )
{
    size_t      len;
    size_t      stringlen;

    if( name == NULL || data == NULL || size == NULL ) {
        return( false );
    }

    if( name->ord.fFlag == 0xff ) {
        len = sizeof( ResNameOrOrdinal );
        stringlen = 0;
    } else {
        stringlen = strlen( &name->name[0] );
        len = stringlen + 1;
    }

    *data = MemAlloc( len );
    if( *data == NULL ) {
        return( false );
    }

    if( name->ord.fFlag != 0xff ) {
        memcpy( *data, &name->name[0], len );
    } else {
        memcpy( *data, name, len );
    }

    *size = len;

    return( true );
}

static bool WRDataFromNameOrOrdinal32( ResNameOrOrdinal *name, void **data, size_t *size )
{
    uint_16     *data16;
    char        *uni_str;

    if( name == NULL || data == NULL || size == NULL ) {
        return( false );
    }

    if( name->ord.fFlag == 0xff ) {
        *size = sizeof( uint_16 ) + sizeof( uint_16 );
        *data = MemAlloc( *size );
    } else {
        uni_str = NULL;
        WRmbcs2unicode( &name->name[0], (char **)data, size );
    }

    if( *data == NULL ) {
        return( false );
    }

    if( name->ord.fFlag == 0xff ) {
        data16 = (uint_16 *)*data;
        data16[0] = 0xffff;
        data16[1] = name->ord.wOrdinalID;
    }

    return( true );
}

ResNameOrOrdinal * WRAPI WRNameOrOrdinalFromData( const void *data, bool is32bit )
{
    ResNameOrOrdinal    *n;

    if( is32bit ) {
        n = WRNameOrOrdinalFromData32( data );
    } else {
        n = WRNameOrOrdinalFromData16( data );
    }

    return( n );
}

int WRAPI WRDataFromNameOrOrdinal( ResNameOrOrdinal *name, bool is32bit, void **data, size_t *size )
{
    if( is32bit ) {
        return( WRDataFromNameOrOrdinal32( name, data, size ) );
    } else {
        return( WRDataFromNameOrOrdinal16( name, data, size ) );
    }
}
