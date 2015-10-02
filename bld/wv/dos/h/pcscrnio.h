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


#pragma aux BIOSSetPage =       \
        "push   bp"             \
        "mov    ah,5"           \
        "int    10h"            \
        "pop    bp"             \
    parm caller [ax];


#pragma aux BIOSGetPage =                                       \
        "push   bp"             \
        "mov    ah,0fH"         \
        "int    10h"            \
        "mov    al,bh"          \
        "xor    ah,ah"          \
        "pop    bp"             \
    parm caller [ax] modify [bx];


#pragma aux BIOSGetMode =                                       \
        "push   bp"             \
        "mov    ah,0fH"         \
        "int    10h"            \
        "pop    bp"             \
    parm caller [ax] modify [bx];


#pragma aux BIOSSetMode =                                       \
        "push   bp"             \
        "xor    ah,ah"          \
        "int    10h"            \
        "pop    bp"             \
    parm caller [ax];


#pragma aux BIOSGetCurPos =                                     \
        "push   bp"             \
        "mov    ah,3"           \
        "int    10h"            \
        "pop    bp"             \
    parm caller [bx] value [dx] modify [bx cx dx];


#pragma aux BIOSSetCurPos =                                     \
        "push   bp"             \
        "mov    ah,2"           \
        "int    10h"            \
        "pop    bp"             \
    parm caller [dx] [bx] modify [bx cx dx];


#pragma aux BIOSGetCurTyp =                                     \
        "push   bp"             \
        "mov    ah,3"           \
        "int    10h"            \
        "pop    bp"             \
    parm caller [bx] value [cx] modify [bx cx dx];


#pragma aux BIOSSetCurTyp =                                     \
        "push   bp"             \
        "mov    ah,1"           \
        "int    10h"            \
        "pop    bp"             \
    parm caller [cx] modify [cx];


#pragma aux BIOSGetAttr =                                       \
        "push   bp"             \
        "mov    ah,8"           \
        "int    10h"            \
        "pop    bp"             \
    parm [bh] value [ah];


#pragma aux BIOSSetAttr =                                       \
        "push   bp"             \
        "sub    cx,cx"          \
        "mov    dx,3250h"       \
        "mov    ax,600h"        \
        "int    10h"            \
        "pop    bp"             \
    parm caller [bh];


#pragma aux VIDGetRow =                                         \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm caller [dx];


#pragma aux VIDSetRow =                                         \
        "mov    ah,al"          \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "mov    al,ah"          \
        "out    dx,al"          \
    parm caller [dx] [ax];


#pragma aux VIDSetPos =                                         \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "mov    al,bl"          \
        "out    dx,al"          \
        "dec    dx"             \
        "mov    al,0eh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "mov    al,bh"          \
        "out    dx,al"          \
    parm caller [dx] [bx];


#pragma aux VIDGetPos =                                         \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
        "mov    ah,al"          \
        "dec    dx"             \
        "mov    al,0eh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
        "xchg   ah,al"          \
    parm caller [dx] value [ax];


#pragma aux VIDGetCol =                                         \
        "mov    al,0eh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm caller [dx];


#pragma aux VIDWait =                                           \
0XEB 0X00       /* jmp    ip                            */      \
        parm caller;


#pragma aux VIDSetCurTyp =                                      \
        "push   ax"             \
        "mov    al,0ah"         \
        "out    dx,al"          \
        "inc    dx"             \
        "mov    al,ah"          \
        "out    dx,al"          \
        "dec    dx"             \
        "mov    al,0bh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "pop    ax"             \
        "out    dx,al"          \
    parm caller [dx] [ax];


#pragma aux VIDGetCurTyp =                                      \
        "mov    al,0bh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
        "mov    ah,al"          \
        "dec    dx"             \
        "mov    al,0ah"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
        "xchg   ah,al"          \
    parm caller [dx] value [ax];


#pragma aux BIOSEGAInfo =                                       \
        "push   bp"             \
        "mov    ah,12h"         \
        "mov    bx,0ff0ah"      \
        "int    10h"            \
        "mov    ax,bx"          \
        "mov    dx,cx"          \
        "pop    bp"             \
    parm modify [bx cx];


#pragma aux BIOSGetRows =                                       \
        "push   es"             \
        "push   bp"             \
        "mov    ah,1130h"       \
        "xor    bh,bh"          \
        "int    10h"            \
        "inc    dl"             \
        "pop    bp"             \
        "pop    es"             \
    parm value [dl] modify [ax bx cx];


#pragma aux BIOSGetPoints =                                     \
        "push   es"             \
        "push   bp"             \
        "mov    ah,1130h"       \
        "xor    bh,bh"          \
        "int    10h"            \
        "pop    bp"             \
        "pop    es"             \
    parm value [cx] modify [ax bx dx];


#pragma aux BIOSEGAChrSet =                                     \
        "push   bp"             \
        "mov    ah,11h"         \
        "xor    bl,bl"          \
        "int    10h"            \
        "pop    bp"             \
    parm [al] modify [ax bx];


#pragma aux BIOSCharSet =                                       \
        "xchg   bp,si"          \
        "mov    ah,11h"         \
        "xor    bl,bl"          \
        "int    10h"            \
        "xchg   bp,si"          \
    parm [al] [bh] [cx] [dx] [es] [si] modify [ax bx cx dx];

enum ega_seqencer {
        SEQ_PORT        = 0x3c4,
        SEQ_RESET       = 0,
        SEQ_CLOCK_MODE  = 1,
        SEQ_MAP_MASK    = 2,
        SEQ_CHAR_MAP_SEL= 3,
        SEQ_MEM_MODE    = 4,
        /* reset register */
        RES_NOT_ASYNCH  = 0x01,
        RES_NOT_SYNCH   = 0x02,
        /* clock mode register */
        CLK_8_DOTS      = 0x01,
        CLK_SHIFT_LOAD  = 0x04,
        CLK_DOT_CLOCK   = 0x08,
        /* map mask register */
        MSK_MAP_0       = 0x01,
        MSK_MAP_1       = 0x02,
        MSK_MAP_2       = 0x04,
        MSK_MAP_3       = 0x08,
        /* character map register */
        CHR_MAPA_0      = 0x00,
        CHR_MAPA_1      = 0x01,
        CHR_MAPA_2      = 0x02,
        CHR_MAPA_3      = 0x03,
        CHR_MAPB_0      = 0x00,
        CHR_MAPB_1      = 0x04,
        CHR_MAPB_2      = 0x08,
        CHR_MAPB_3      = 0x0c,
        /* memory mode register */
        MEM_ALPHA       = 0x01,
        MEM_EXTENDED    = 0x02,
        MEM_NOT_ODD_EVEN= 0x04
};

enum ega_graphics_controller {
        GRA_PORT        = 0x3ce,
        GRA_SET_RESET   = 0,
        GRA_ENABLE_SR   = 1,
        GRA_COLOR_COMP  = 2,
        GRA_DATA_ROT    = 3,
        GRA_READ_MAP    = 4,
        GRA_GRAPH_MODE  = 5,
        GRA_MISC        = 6,
        GRA_COLOR_CARE  = 7,
        GRA_BIT_MASK    = 8,
        /* set/reset register */
        SR_MAP_0        = 0x01,
        SR_MAP_1        = 0x02,
        SR_MAP_2        = 0x04,
        SR_MAP_3        = 0x08,
        /* enable set/reset register */
        ESR_MAP_0       = 0x01,
        ESR_MAP_1       = 0x02,
        ESR_MAP_2       = 0x04,
        ESR_MAP_3       = 0x08,
        /* colour compare register */
        COL_MAP_0       = 0x01,
        COL_MAP_1       = 0x02,
        COL_MAP_2       = 0x04,
        COL_MAP_3       = 0x08,
        /* data rotate register */
        /* bottom three bits are the right rotate count */
        ROT_UNMOD       = 0x00,
        ROT_AND         = 0x08,
        ROT_OR          = 0x10,
        ROT_XOR         = 0x18,
        /* read map select register */
        RMS_MAP_0       = 0x00,
        RMS_MAP_1       = 0x01,
        RMS_MAP_2       = 0x02,
        RMS_MAP_3       = 0x03,
        /* graphics mode register */
        GRM_EN_ROT      = 0x00,
        GRM_SYS_LATCH   = 0x01,
        GRM_BIT_PLANE   = 0x02,
        GRM_ILL         = 0x03,
        GRM_TEST        = 0x04,
        GRM_READ_MODE   = 0x08,
        GRM_ODD_EVEN    = 0x10,
        GRM_SHIFT       = 0x20,
        /* miscellaneous register */
        MIS_GRAPH_MODE  = 0x01,
        MIS_CHAIN       = 0x02,
        MIS_A000_128    = 0x00,
        MIS_A000_64     = 0x04,
        MIS_B000_32     = 0x08,
        MIS_B800_32     = 0x0c,
        /* colour don't care register */
        CDC_CARE_MAP_0  = 0x01,
        CDC_CARE_MAP_1  = 0x02,
        CDC_CARE_MAP_2  = 0x04,
        CDC_CARE_MAP_3  = 0x08
        /* bit mask register */
        /* bit N set to one causes that bit in each plane not to be written */
};

/* write ega/vga registers */
#pragma aux     _ega_write =    \
        "out    dx,ax"          \
    parm [dx] [al] [ah] modify exact [];

/* read vga registers */
#pragma aux     _vga_read =                     \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm [dx] [al] value [al];

#define _seq_write( reg, val )          _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )        _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )                _vga_read( SEQ_PORT, reg )
#define _graph_read( reg )              _vga_read( GRA_PORT, reg )


/* disable video */
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


enum vid_state_info {
        VID_STATE_HARDWARE      = 0x1,
        VID_STATE_BIOS          = 0x2,
        VID_STATE_DAC_N_COLOURS = 0x4,
        VID_STATE_ALL           = 0x7
};

/* get video save size */
#pragma aux     _vidstatesize = \
        "mov    ax,1c00h"       \
        "int    10h"            \
        "cmp    al,1ch"         \
        "je short L1"           \
        "xor    bx,bx"          \
    "L1:"                       \
    parm [cx] value [bx] modify exact [ax bx];

/* save video state */
#pragma aux     _vidstatesave = \
        "mov    ax,1c01h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];

/* restore video state */
#pragma aux     _vidstaterestore = \
        "mov    ax,1c02h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];




#pragma aux Fillb =                                             \
/* Fillb( toseg, tooff, val, len ); */ \
        "rep stosb"             \
    parm caller [es] [di] [ax] [cx] modify [di es];



#define VIDMONOINDXREG  0X03B4
#define VIDCOLRINDXREG  0X03D4



extern char        BIOSGetMode();
extern int         BIOSGetPage();
extern signed long BIOSEGAInfo();
extern void        BIOSSetPage();
extern void        BIOSSetMode();
extern void        BIOSEGAChrSet();
extern void        BIOSEGALoadChrSet();
extern int         BIOSGetCurPos();
extern void        BIOSSetCurPos();
extern int         BIOSGetCurTyp();
extern void        BIOSSetCurTyp();
extern char        BIOSGetRows();
extern unsigned    BIOSGetPoints();
extern char        BIOSGetAttr();
extern void        BIOSSetAttr( unsigned char );
extern void        BIOSCharSet();
extern char        VIDGetCol();
extern char        VIDGetRow();
extern void        VIDWait();
extern void        VIDSetPos();
extern void        VIDSetRow();
extern void        VIDSetCol();
extern void        VIDSetCurTyp();
extern unsigned    VIDGetCurTyp();


extern void        Fillb( unsigned, unsigned, unsigned, unsigned );
extern void        _ega_write( unsigned, char, char );
extern char        _vga_read( unsigned, char );
extern void        _disablev( unsigned );
extern void        _enablev( unsigned );
extern unsigned    _vidstatesize( unsigned );
extern void        _vidstatesave( unsigned, unsigned, unsigned );
extern void        _vidstaterestore( unsigned, unsigned, unsigned );
