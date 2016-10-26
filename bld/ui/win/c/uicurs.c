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

#pragma aux VIDSetPos =                                         \
0XB0 0X0F       /* mov    al,f                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0X88 0XD8       /* mov    al,bl                         */      \
0XEE            /* out    dx,al                         */      \
0X4A            /* dec    dx                            */      \
0XB0 0X0E       /* mov    al,e                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0X8A 0XC7       /* mov    al,bh                         */      \
0XEE            /* out    dx,al                         */      \
        parm caller [ dx ] [ bx ];

extern void        VIDSetPos( unsigned, unsigned );


#pragma aux VIDSetCurTyp =                                      \
0X50            /* push   ax                            */      \
0XB0 0X0A       /* mov    al,a                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0X8A 0XC4       /* mov    al,ah                         */      \
0XEE            /* out    dx,al                         */      \
0X4A            /* dec dx                               */      \
0XB0 0X0B       /* mov    al,b                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0X58            /* pop    ax                            */      \
0XEE            /* out    dx,al                         */      \
        parm caller [ dx ] [ ax ];

extern void VIDSetCurTyp( unsigned, unsigned );


#pragma aux VIDGetCurTyp =                                      \
0XB0 0X0B       /* mov    al,b                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0XEC            /* in     al,dx                         */      \
0X88 0XC4       /* mov    ah,al                         */      \
0X4A            /* dec    dx                            */      \
0XB0 0X0A       /* mov    al,a                          */      \
0XEE            /* out    dx,al                         */      \
0X42            /* inc    dx                            */      \
0XEC            /* in     al,dx                         */      \
0X86 0XE0       /* xchg   ah,al                         */      \
        parm caller [ dx ] value [ ax ];

extern unsigned         VIDGetCurTyp( unsigned );

enum {
        BD_SEG          = 0x40,
        BD_EQUIP_LIST   = 0x10,
        BD_CURR_MODE    = 0x49,
        BD_REGEN_LEN    = 0x4c,
        BD_CURPOS       = 0x50,
        BD_MODE_CTRL    = 0x65,
        BD_VID_CTRL1    = 0x87,
};

#define VIDMONOINDXREG  0X03B4
#define VIDCOLRINDXREG  0X03D4

unsigned int  VIDPort=VIDMONOINDXREG;

static unsigned RegCur;
static unsigned InsCur;

#define NoCur   0x2000

static char OldRow, OldCol, OldTyp;

void uiinitcursor( void )
{
    OldTyp = C_OFF;
    if( UIData->height == 25 ) {
        RegCur = 0x0b0c;
    } else {
        RegCur = 0x0607;
    }
    InsCur = ( ((RegCur + 0x100) >> 1 & 0xFF00) + 0x100 ) | ( RegCur & 0x00FF );
}

#pragma off( unreferenced );
void uisetcursor( ORD row, ORD col, int typ, int attr )
#pragma off( unreferenced );
{
    if( typ == C_OFF ) {
        uioffcursor();
    } else {
        if( row == OldRow && col == OldCol && typ == OldTyp ) return;
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        VIDSetPos( VIDPort, row * UIData->width + col );
        VIDSetCurTyp( VIDPort, typ == C_INSERT ? InsCur : RegCur );
    }
}


void uioffcursor( void )
{
    OldTyp = C_OFF;
    VIDSetCurTyp( VIDPort, NoCur );
}

void uiswapcursor( void )
{
}

void uifinicursor( void )
{
}
