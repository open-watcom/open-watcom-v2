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


#define BIOS_VIDEO      0x10
#define BIOS_MOUSE      0x33
#define BIOS_PAGE       0x40

#define SCREEN_OFFSET   0x4e
#define SYSTEM_CLOCK    0x6c



#pragma aux BIOSGetPage =                                       \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
0XCD 0X10       /* int    10                            */      \
0X8A 0XC7       /* mov    al,bh                         */      \
0x30 0xE4       /* xor    ah,ah                         */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];

extern unsigned BIOSGetPage(void);

#pragma aux BIOSSetMode =                                       \
0X55            /* push   bp                            */      \
0x30 0xE4       /* xor    ah,ah                         */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ];

extern void        BIOSSetMode(unsigned);


#pragma aux BIOSGetMode =                                       \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];

extern char        BIOSGetMode(void);


#pragma aux BIOSGetColumns =                                    \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        value       [ah]                                        \
        modify [ bx ];

extern char        BIOSGetColumns(void);


#pragma aux BIOSGetRows =                                       \
0X06            /* push   es                            */      \
0X55            /* push   bp                            */      \
0XB8 0X30 0X11  /* mov    ax,1130                       */      \
0x30 0xFF       /* xor    bh,bh                         */      \
0XCD 0X10       /* int    10                            */      \
0XFE 0XC2       /* inc    dl                            */      \
0X5D            /* pop    bp                            */      \
0X07            /* pop    es                            */      \
        parm value [ dl ] modify [ ax bx cx ];

extern char        BIOSGetRows(void);


#pragma aux BIOSEGAInfo =                                       \
0X55            /* push   bp                            */      \
0XB4 0X12       /* mov    ah,12                         */      \
0X66, 0XBB, 0X0A, 0XFF       /* mov    bx,0FF0Ah                     */      \
0XCD 0X10       /* int    10                            */      \
0X89 0XD8       /* mov    ax,bx                         */      \
0X89 0XCA       /* mov    dx,cx                         */      \
0X5D            /* pop    bp                            */      \
        parm modify [ cx bx ];

struct ega_info {
    unsigned char   mem;
    unsigned char   mono;
    unsigned char   switches;
    unsigned char   adapter_bits;
};

extern struct ega_info BIOSEGAInfo( void );



#pragma aux BIOSGetCurPos =                                     \
0X55            /* push   bp                            */      \
0XB4 0X03       /* mov    ah,3                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ bx ]                                      \
        value [ dx ]                                            \
        modify [ bx cx dx ];


#pragma aux BIOSSetCurPos =                                     \
0X55            /* push   bp                            */      \
0XB4 0X02       /* mov    ah,2                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ dh ] [dl] [ bx ]                          \
        modify [ bx cx dx ];


#pragma aux BIOSGetCurTyp =                                     \
0X55            /* push   bp                            */      \
0XB4 0X03       /* mov    ah,3                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ bx ]                                      \
        value [ cx ]                                            \
        modify [ bx cx dx ];


#pragma aux BIOSSetCurTyp =                                     \
0X55            /* push   bp                            */      \
0XB4 0X01       /* mov    ah,1                          */      \
0XCD 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ch ] [cl]                                 \
        modify [ cx ];

struct cursor_pos {
    unsigned char   col;
    unsigned char   row;
};

extern struct cursor_pos    BIOSGetCurPos(unsigned);
extern void        BIOSSetCurPos(unsigned char, unsigned char, unsigned);
extern unsigned    BIOSGetCurTyp(unsigned);
extern void        BIOSSetCurTyp(unsigned char, unsigned char);

