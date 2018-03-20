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


#include "pcscrnio.h"


extern unsigned BIOSDevCombCode( void );
#pragma aux BIOSDevCombCode =                                   \
0X55            /* push   bp                            */      \
0XB8 0X00 0X1A  /* mov    ax,1a00                       */      \
0XCD 0X10       /* int    10                            */      \
0X3C 0X1A       /* cmp    al,1a                         */      \
0X74 0X02       /* jz     *+2                           */      \
0X29 0XDB       /* sub    bx,bx                         */      \
0X5D            /* pop    bp                            */      \
                value   [bx]                                  \
                modify  [ax];

extern char        BIOSGetMode( void );
#pragma aux BIOSGetMode =                                       \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
0XCd 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ax]                                      \
        modify [bx];

extern signed long BIOSEGAInfo( void );
#pragma aux BIOSEGAInfo =                                       \
0X55            /* push   bp                            */      \
0XB4 0X12       /* mov    ah,12                         */      \
0XB3 0X10       /* mov    bl,10                         */      \
0XB7 0XFF       /* mov    bh,ff                         */      \
0XCd 0X10       /* int    10                            */      \
0X89 0XD8       /* mov    ax,bx                         */      \
0X89 0XCA       /* mov    dx,cx                         */      \
0X5D            /* pop    bp                            */      \
        parm modify [bx cx];

extern void        _ega_write( unsigned, char, char );
#pragma aux     _ega_write =            /* write ega/vga registers */   \
                0xef                    /* out dx,ax */                 \
                parm [dx] [al] [ah]                                     \
                modify exact [];

extern char        _vga_read( unsigned, char );
#pragma aux     _vga_read =             /* read vga registers */        \
                0xee                    /* out dx,al    */              \
                0x42                    /* inc    dx    */              \
                0xec                    /* in al,dx     */              \
                parm [dx] [al]                                          \
                value [al];

extern void        Fillb( unsigned, unsigned, unsigned, unsigned );
#pragma aux Fillb =                                             \
/*      Fillb( toseg, tooff, val, len );                */      \
0XF3            /* rep                                  */      \
0XAA            /* stosb                                */      \
        parm    caller  [es] [di] [ax] [cx]             \
        modify  [di es];


/* disable video */
extern void        _disablev( unsigned );
#pragma aux     _disablev = \
    "L1: in   al,dx"    \
        "test al,8"     \
        "jz short L1"   \
        "mov  dx,03c0h" \
        "mov  al,11h"   \
        "out  dx,al"    \
        "xor  al,al"    \
        "out  dx,al"    \
    parm [dx] modify [ax dx];

/* enable video  */
extern void        _enablev( unsigned );
#pragma aux     _enablev = \
    "L1: in   al,dx"    \
        "test al,8"     \
        "jz short L1"   \
        "mov  dx,03c0h" \
        "mov  al,31h"   \
        "out  dx,al"    \
        "xor  al,al"    \
        "out  dx,al"    \
    parm [dx] modify [ax dx];


extern unsigned char    VIDGetRow( unsigned );
#pragma aux VIDGetRow = \
        "mov    al,0fh" \
        "out    dx,al"  \
        "inc    dx"     \
        "in     al,dx"  \
    parm [dx] value [al] modify exact [al dx]


extern void     VIDSetRow( unsigned, unsigned char );
#pragma aux VIDSetRow = \
        "mov    al,0fh" \
        "out    dx,al"  \
        "inc    dx"     \
        "mov    al,ah"  \
        "out    dx,al"  \
    parm [dx] [ah] modify exact [al dx]

extern void     VIDWait( void );
#pragma aux VIDWait =           \
        "jmp short L1" "L1:"    \
    modify exact
