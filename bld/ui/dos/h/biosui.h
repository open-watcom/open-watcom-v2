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


#ifndef _BIOSUI_H_
#define _BIOSUI_H_

#ifdef __386__
extern LP_VOID firstmeg( unsigned seg, unsigned offset );
#else
#define firstmeg(a,b) MK_FP((a),(b))
#endif

extern void IdleInterrupt(void);

#if defined(__OSI__)
 extern  void   __Int10();
 extern  void   __Int16();
 extern  void   __Int21();
 #define _INT_10        "call __Int10"
 #define _INT_16        "call __Int16"
 #define _INT_21        "call __Int21"
#else
 #define _INT_10        "int 0x10"
 #define _INT_16        "int 0x16"
 #define _INT_21        "int 0x21"
#endif

#define BIOS_VIDEO      0x10
#define BIOS_MOUSE      0x33
#define BIOS_PAGE       0x40

#define SCREEN_OFFSET   0x4e
#define SYSTEM_CLOCK    0x6c

#define REAL_SEGMENT    0x34

extern short MouseInt( unsigned short, unsigned short, unsigned short,
                       unsigned short );
#pragma aux MouseInt = 0xcd BIOS_MOUSE parm [ax] [bx] [cx] [dx];

extern unsigned char BIOSGetPage(void);
#pragma aux BIOSGetPage =       \
    "push   bp"                 \
    "mov    ah,0fh"             \
    _INT_10                     \
    "mov    al,bh"              \
    "mov    ah,0"               \
    "pop    bp"                 \
    parm caller [ ax ]          \
    value [bh]                  \
    modify [ bx ];

extern void BIOSSetMode(unsigned);
#pragma aux BIOSSetMode =                                       \
0X55            /* push   bp                            */      \
0XB4 0X00       /* mov    ah,0                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ];

extern unsigned char BIOSGetMode(void);
#pragma aux BIOSGetMode =                                       \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];

extern unsigned char BIOSGetColumns(void);
#pragma aux BIOSGetColumns =                                    \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        value       [ah]                                        \
        modify [ bx ];

extern unsigned char BIOSGetRows(void);
#pragma aux BIOSGetRows =                                       \
0X06            /* push   es                            */      \
0X55            /* push   bp                            */      \
0XB8 0X30 0X11  /* mov    ax,1130                       */      \
0XB7 0X00       /* mov    bh,0                          */      \
_INT_10                                                         \
0XFE 0XC2       /* inc    dl                            */      \
0X5D            /* pop    bp                            */      \
0X07            /* pop    es                            */      \
        parm value [ dl ]                                       \
        modify [ ax bx cx ];

struct ega_info {
    unsigned char   mem;
    unsigned char   mono;
    unsigned char   switches;
    unsigned char   adapter_bits;
};

extern struct ega_info BIOSEGAInfo( void );
#ifdef __386__
/* note : first 5 lines work but the the corresponding assembler code
          doesn't work so the comments must not match the code */
#pragma aux BIOSEGAInfo =                       \
0X55            /* push   bp                            */      \
0XB4 0X12       /* mov    ah,12h                        */      \
0XB3 0X10       /* mov    bl,10h                        */      \
0XB7 0XFF       /* mov    bh,ff                         */      \
_INT_10                                                         \
    "xor    eax, eax"                                           \
    "mov    ax,cx"                                              \
    "shl    eax,10h"                                            \
    "or     ax,bx"                                              \
    "pop    ebp"                                                \
        parm modify [ cx bx ];
#else
#pragma aux BIOSEGAInfo =                                       \
0X55            /* push   bp                            */      \
0XB4 0X12       /* mov    ah,12h                        */      \
0XB3 0X10       /* mov    bl,10h                        */      \
0XB7 0XFF       /* mov    bh,ff                         */      \
_INT_10                                                         \
0X89 0XD8       /* mov    ax,bx                         */      \
0X89 0XCA       /* mov    dx,cx                         */      \
0X5D            /* pop    bp                            */      \
        parm modify [ cx bx ];
#endif

extern unsigned char BIOSSumming( char );
#pragma aux BIOSSumming =                                 \
0X55            /* push   bp                            */      \
0XB4 0X12       /* mov    ah,12                         */      \
0XB3 0X33       /* mov    bl,33                         */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm [al] value [ al ] modify [ cx bx dx ];

extern unsigned short BIOSGetKeyboard( char );
#pragma aux  BIOSGetKeyboard =  \
_INT_16                         \
 parm[ah] value[ax];


extern unsigned short BIOSKeyboardHit( char );
#ifdef __386__
#pragma aux BIOSKeyboardHit = \
_INT_16                                 \
 0x74 0x06              /* jz      foo1 */ \
 0x66 0xB8 0x01 0x00    /* mov     ax,1 */ \
 0xEB 0x04              /* jmp     short foo2 */ \
 0x66 0xB8 0x00 0x00    /* foo1:   mov     ax,0 */ \
 parm[ah] value[ax];
#else
#pragma aux BIOSKeyboardHit = \
_INT_16                                 \
 0x74 0x05              /* jz      foo1 */ \
 0xB8 0x01 0x00         /* mov     ax,1 */ \
 0xEB 0x03              /* jmp     short foo2 */ \
 0xB8 0x00 0x00         /* foo1:   mov     ax,0 */ \
 parm[ah] value[ax];
#endif

extern unsigned short BIOSTestKeyboard(void);
#ifdef __386__
#pragma aux BIOSTestKeyboard = \
 0x66 0xB8 0xff 0x12    /* mov     ax,012ffh */ \
_INT_16                                         \
 value[ax];
#else
#pragma aux BIOSTestKeyboard = \
 0xB8 0xff 0x12         /* mov     ax,012ffh */ \
_INT_16                                         \
 value[ax];
#endif

extern unsigned short BIOSGetCharAttr( unsigned char page );
#pragma aux  BIOSGetCharAttr =  \
    "push   bp"                 \
    "mov    ah,8"               \
    _INT_10                     \
    "pop    bp"                 \
    parm    caller  [bh]        \
    value   [ax];

extern void BIOSSetCharAttr( unsigned short char_attr, unsigned char page );
#pragma aux BIOSSetCharAttr =   \
    "push   bp"                 \
    "push   cx"                 \
    "push   bx"                 \
    "mov    bl,ah"              \
    "mov    cx,1"               \
    "mov    ah,9"               \
    _INT_10                     \
    "pop    bx"                 \
    "pop    cx"                 \
    "pop    bp"                 \
    parm caller [ax] [bh];

struct cursor_pos {
    unsigned char   col;
    unsigned char   row;
};

extern struct cursor_pos  BIOSGetCurPos(unsigned char page);
#pragma aux               BIOSGetCurPos =                       \
0X55            /* push   bp                            */      \
0XB4 0X03       /* mov    ah,3                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ bh ]                                      \
        value [ dx ]                                            \
        modify [ bx cx dx ];


extern void BIOSSetCurPos(unsigned char row, unsigned char col, unsigned char page);
#pragma aux BIOSSetCurPos =                                     \
0X55            /* push   bp                            */      \
0XB4 0X02       /* mov    ah,2                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ dh ] [dl] [ bh ]                          \
        modify [ bx cx dx ];


extern unsigned short BIOSGetCurTyp(unsigned char page);
#pragma aux     BIOSGetCurTyp =                                 \
0X55            /* push   bp                            */      \
0XB4 0X03       /* mov    ah,3                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ bh ]                                      \
        value [ cx ]                                            \
        modify [ bx cx dx ];


extern void BIOSSetCurTyp(unsigned char top_line, unsigned char bot_line);
#pragma aux BIOSSetCurTyp =                                     \
0X55            /* push   bp                            */      \
0XB4 0X01       /* mov    ah,1                          */      \
_INT_10                                                         \
0X5D            /* pop    bp                            */      \
        parm caller [ ch ] [cl]                                 \
        modify [ cx ];

#endif // _BIOSUI_H_
