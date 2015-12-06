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


#include    <dos.h>
#include    <stdlib.h>
#include    <string.h>
#include    "uidef.h"
#include    "charmap.h"
#include    "uigchar.h"

static unsigned char UiMapChar[] = {
    #define MAPCHARS
    #define pick(enum,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) charmap,
    #include "_mapchar.h"
    #undef pick
    #undef MAPCHARS
};

static unsigned char MappingData[][16] = {
    #define MAPCHARS
    #define pick(enum,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) {d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df},
    #include "_mapchar.h"
    #undef pick
    #undef MAPCHARS
};

static void MapCharacter( unsigned char ch, unsigned char data[16] )
{
    unsigned short              s,  points;
    int                         j;
    unsigned char               temp;

    points = _POINTS;

    s = ch * 32;

    for( j = 0; j < points; j++ ) {
        temp = _peekb( 0xA000, s );
        _pokeb( 0xA000, s++, data[j] );
        data[j] = temp;
    }
}

bool UIMapCharacters( unsigned char mapchar[], unsigned char mapdata[][16] )
{
    int         i;

    if( ( UIData->colour == M_EGA || UIData->colour == M_VGA )
                && !UIData->desqview
                && !UIData->no_graphics
                && UIData->height == 25 ) {
        SetSequencer();
        SetWriteMap();

        for( i = 0; i < sizeof( UiMapChar ); i++ ) {
            MapCharacter( mapchar[i], mapdata[i] );
        }
        ResetSequencer();
        return( TRUE );
    }
    return( FALSE );
}

bool FlipCharacterMap( void )
{
    ATTR        old;
    ATTR        new;

    if( UIMapCharacters( UiMapChar, MappingData ) ) {
        memcpy( UiGChar, UiMapChar, sizeof( UiMapChar ) );
        /* swap fore and back color for Dialog attr so you get nice title */
        old = UIData->attrs[ATTR_DIAL_FRAME];
        new = 0;
        new |= ( old >> 4 ) & 0x0f;
        new |= ( old << 4 ) & 0xf0;
        UIData->attrs[ATTR_DIAL_FRAME] = new;
        return( TRUE );
    }
    return( FALSE );
}
