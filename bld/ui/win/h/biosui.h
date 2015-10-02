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



extern unsigned BIOSGetPage(void);
#pragma aux BIOSGetPage =                                       \
        "push   bp"             \
        "mov    ah,0fh"         \
        _INT_10                 \
        "mov    al,bh"          \
        "xor    ah,ah"          \
        "pop    bp"             \
    parm caller [ax] modify [bx];


extern void        BIOSSetMode(unsigned);
#pragma aux BIOSSetMode =                                       \
        "push   bp"             \
        "xor    ah,ah"          \
        _INT_10                 \
        "pop    bp"             \
    parm caller [ax];



#pragma aux BIOSGetMode =                                       \
        "push   bp"             \
        "mov    ah,0fh"         \
        _INT_10                 \
        "pop    bp"             \
        parm caller [ ax ]                                      \
        modify [ bx ];

extern char        BIOSGetMode(void);


extern char        BIOSGetColumns(void);
#pragma aux BIOSGetColumns =                                    \
        "push   bp"             \
        "mov    ah,0fh"         \
        _INT_10                 \
        "pop    bp"             \
    parm caller [ax] value [ah] modify [bx];

extern char        BIOSGetRows(void);
#pragma aux BIOSGetRows =                                       \
        "push   es"             \
        "push   bp"             \
        "mov    ax,1130h"       \
        "xor    bh,bh"          \
        _INT_10                 \
        "inc    dl"             \
        "pop    bp"             \
        "pop    es"             \
    parm value [dl] modify [ax bx cx];



#pragma aux BIOSEGAInfo =                                       \
        "push   bp"             \
        "mov    ah,12h"         \
        "mov    bx,0ff0ah"      \
        _INT_10                 \
        "mov    ax,bx"          \
        "mov    dx,cx"          \
        "pop    bp"             \
    parm modify [cx bx];

struct ega_info {
    unsigned char   mem;
    unsigned char   mono;
    unsigned char   switches;
    unsigned char   adapter_bits;
};

extern struct ega_info BIOSEGAInfo( void );



#pragma aux BIOSGetCurPos =                                     \
        "push   bp"             \
        "mov    ah,3"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [bx] value [dx] modify [bx cx dx];


#pragma aux BIOSSetCurPos =                                     \
        "push   bp"             \
        "mov    ah,2"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [dh] [dl] [bx] modify [bx cx dx];


#pragma aux BIOSGetCurTyp =                                     \
        "push   bp"             \
        "mov    ah,3"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [bx] value [cx] modify [ bx cx dx ];


#pragma aux BIOSSetCurTyp =                                     \
        "push   bp"             \
        "mov    ah,1"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [ch] [cl] modify [cx];

struct cursor_pos {
    unsigned char   col;
    unsigned char   row;
};

extern struct cursor_pos    BIOSGetCurPos(unsigned);
extern void        BIOSSetCurPos(unsigned char, unsigned char, unsigned);
extern unsigned    BIOSGetCurTyp(unsigned);
extern void        BIOSSetCurTyp(unsigned char, unsigned char);
extern void        WindowsMouseEvent( unsigned event, unsigned info );
