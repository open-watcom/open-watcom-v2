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


#include <stdui.h>

#pragma aux BIOSSetPos =                                         \
        "mov ah, 13h"\
        "int 18h"\
        modify [ ah ] \
        parm caller [ dx ];

extern void        BIOSSetPos( unsigned );

#pragma aux BIOSSetBlink = \
        "mov ax, 1000h"\
        "int 18h"\
        modify [ ax ];
extern void BIOSSetBlink();

#pragma aux BIOSSetNoBlink = \
        "mov ax, 1001h"\
        "int 18h"\
        modify [ ax ];
extern void BIOSSetNoBlink();

#pragma aux BIOSOffCursor = \
        "mov ah, 12h" \
        "int 18h"\
        modify [ah];
extern void BIOSOffCursor();

#pragma aux BIOSOnCursor =\
        "mov ah, 11h"\
        "int 18h"\
        modify [ah];
extern void BIOSOnCursor();

static char OldRow, OldCol, OldTyp;

void uiinitcursor()
{
    OldTyp = C_OFF;
    uioffcursor();
}

#pragma off( unreferenced );
void uisetcursor( ORD row, ORD col, int typ, int attr )
#pragma off( unreferenced );
{
    if( typ == C_OFF ) {
        uioffcursor();
    } else {
        if( row == OldRow && col == OldCol ) return;
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        BIOSSetPos( ( ( ( row * UIData->width ) + col ) * 2 )  );
        /*
         * We multiply by 2 there since the NEC's screen VRAM is twice as
         * large. ( 2 bytes for every character )
         */
        if( typ == C_INSERT ) {
            BIOSSetBlink();
        } else {
            BIOSSetNoBlink();
        }
        BIOSOnCursor(); //Dammit
    }
}


void uioffcursor()
{
    OldTyp = C_OFF;
    BIOSOffCursor();
}

void uiswapcursor()
{
}

void uifinicursor()
{
}
