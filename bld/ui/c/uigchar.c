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
* Description:  Graphic character definitions
*
****************************************************************************/


#include <string.h>
#include "uibox.h"
#include "uichars.h"
#include "uidef.h"
#include "uigchar.h"

#ifdef __LINUX__

void intern DBCSCharacterMap( void ) {}

unsigned char UiGChar[] = {
    #define pick(enum,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) linux,
    #include "_mapchar.h"
    #undef pick
};

#else

/*
    The order of items in this table is position dependent.
    The first "n" entries in this table are remapped on EGA/VGA devices
    supporting character remapping (see UIMapCharacters in UIMAPCH.C).
*/

unsigned char UiGChar[] = {
    #define pick(enum,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) others,
    #include "_mapchar.h"
    #undef pick
};

extern char VertScrollFrame[2];
extern char HorzScrollFrame[2];
extern char SliderChar[2];
extern char LeftPoint[2];
extern char RightPoint[2];
extern char UpPoint[2];
extern char DownPoint[2];

unsigned char UiDBCSChar[] = {
    #define pick(enum,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) dbcs,
    #include "_mapchar.h"
    #undef pick
};

void intern DBCSCharacterMap( void )
{
    if ( uiisdbcs() ) {
        memcpy( UiGChar, UiDBCSChar, sizeof( UiDBCSChar ) );
        VertScrollFrame[0]  = 0x1A;
        HorzScrollFrame[0]  = 0x1A;
        SliderChar[0]       = 0x14;
        LeftPoint[0]        = '<';
        RightPoint[0]       = '>';
        UpPoint[0]          = '^';
        DownPoint[0]        = 'v';
    }
}

#endif
