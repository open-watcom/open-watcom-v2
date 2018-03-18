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


#define STR(...)    #__VA_ARGS__
#define INSTR(...)  STR(__VA_ARGS__)

#define CALL_INT10(n)   "push bp" INSTR( mov ah, n ) "int 10h" "pop bp"

#define VIDMONOINDXREG  0x03B4
#define VIDCOLRINDXREG  0x03D4

#define _seq_write( reg, val )          _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )        _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )                _vga_read( SEQ_PORT, reg )
#define _graph_read( reg )              _vga_read( GRA_PORT, reg )

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

enum vid_state_info {
    VID_STATE_HARDWARE      = 0x1,
    VID_STATE_BIOS          = 0x2,
    VID_STATE_DAC_N_COLOURS = 0x4,
    VID_STATE_ALL           = 0x7,
    VID_STATE_SWAP          = VID_STATE_ALL
};

extern void        BIOSSetPage( unsigned char );
#pragma aux BIOSSetPage =       \
        CALL_INT10( 5 )         \
    parm [al] modify exact [ah];


extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage =       \
        CALL_INT10( 15 )        \
    value [bh] modify exact [ax bh];


extern unsigned char    BIOSGetMode( void );
#pragma aux BIOSGetMode =       \
        CALL_INT10( 15 )        \
    value [al] modify exact [ax bh];


extern void     BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode =       \
        CALL_INT10( 0 )         \
    parm [al] modify exact [ax];


extern unsigned BIOSGetCurPos( unsigned char );
#pragma aux BIOSGetCurPos =     \
        CALL_INT10( 3 )         \
    parm [bh] value [dx] modify exact [ax cx dx];


extern void     BIOSSetCurPos( unsigned, unsigned char );
#pragma aux BIOSSetCurPos =     \
        CALL_INT10( 2 )         \
    parm [dx] [bh] modify [bx cx dx];


extern unsigned BIOSGetCurTyp( unsigned char );
#pragma aux BIOSGetCurTyp =     \
        CALL_INT10( 3 )         \
    parm [bh] value [cx] modify exact [ax cx dx];


extern void        BIOSSetCurTyp();
#pragma aux BIOSSetCurTyp =     \
        CALL_INT10( 1 )         \
    parm [cx] modify [cx];


extern unsigned char    BIOSGetAttr();
#pragma aux BIOSGetAttr =       \
        CALL_INT10( 8 )         \
    parm [bh] value [ah];


extern void        BIOSSetAttr( unsigned char );
#pragma aux BIOSSetAttr =       \
        "sub    cx,cx"          \
        "mov    dx,3250h"       \
        "xor    al,al"          \
        CALL_INT10( 6 )         \
    parm [bh];


extern unsigned char    VIDGetRow( unsigned );
#pragma aux VIDGetRow =         \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm [dx] modify exact [al dx]


extern void     VIDSetRow( unsigned, unsigned char );
#pragma aux VIDSetRow =         \
        "mov    al,0fh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "mov    al,ah"          \
        "out    dx,al"          \
    parm [dx] [ah] modify exact [al dx]


extern void        VIDSetPos( unsigned, unsigned );
#pragma aux VIDSetPos =         \
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
    parm [dx] [bx] modify exact [al dx]


extern unsigned VIDGetPos( unsigned );
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
    parm [dx] value [ax] modify exact [ax dx]


extern unsigned char    VIDGetCol( unsigned );
#pragma aux VIDGetCol =         \
        "mov    al,0eh"         \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm [dx] value [al] modify exact [al dx]


extern void        VIDWait( void );
#pragma aux VIDWait = "jmp short L1" "L1:" \
    parm [] modify exact []


extern void        VIDSetCurTyp();
#pragma aux VIDSetCurTyp =      \
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
    parm [dx] [ax];


extern unsigned    VIDGetCurTyp();
#pragma aux VIDGetCurTyp =      \
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
    parm [dx] value [ax];


extern signed long BIOSEGAInfo();
#pragma aux BIOSEGAInfo =       \
        "mov    bx,0ff0ah"      \
        CALL_INT10( 18 )         \
        "mov    ax,bx"          \
        "mov    dx,cx"          \
    parm modify [bx cx];


extern unsigned char    BIOSGetRows();
#pragma aux BIOSGetRows =       \
        "push   es"             \
        "mov    al,30h"       \
        "xor    bh,bh"          \
        CALL_INT10( 17 )         \
        "inc    dl"             \
        "pop    es"             \
    parm value [dl] modify [ax bx cx];


extern unsigned    BIOSGetPoints();
#pragma aux BIOSGetPoints =     \
        "push   es"             \
        "mov    al,30h"       \
        "xor    bh,bh"          \
        CALL_INT10( 17 )         \
        "pop    es"             \
    parm value [cx] modify [ax bx dx];


extern void        BIOSEGAChrSet();
#pragma aux BIOSEGAChrSet =     \
        "xor    bl,bl"          \
        CALL_INT10( 17 )         \
    parm [al] modify [ax bx];


extern void        BIOSCharSet();
#pragma aux BIOSCharSet =       \
        "xchg   bp,si"          \
        "mov    ah,11h"         \
        "xor    bl,bl"          \
        "int    10h"            \
        "xchg   bp,si"          \
    parm [al] [bh] [cx] [dx] [es] [si] modify [ax bx cx dx];

/* write ega/vga registers */
extern void     _ega_write( unsigned, unsigned char, unsigned char );
#pragma aux     _ega_write =    \
        "out    dx,ax"          \
    parm [dx] [al] [ah] modify exact [];

/* read vga registers */
extern unsigned char    _vga_read( unsigned, unsigned char );
#pragma aux     _vga_read =                     \
        "out    dx,al"          \
        "inc    dx"             \
        "in     al,dx"          \
    parm [dx] [al] value [al];

/* disable video */
extern void     _disablev( unsigned );
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
extern void     _enablev( unsigned );
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


/* get video save size */
extern unsigned _vidstatesize( unsigned );
#pragma aux     _vidstatesize = \
        "mov    ax,1c00h"       \
        "int    10h"            \
        "cmp    al,1ch"         \
        "je short L1"           \
        "xor    bx,bx"          \
    "L1:"                       \
    parm [cx] value [bx] modify exact [ax bx];

/* save video state */
extern void     _vidstatesave( unsigned, unsigned, unsigned );
#pragma aux     _vidstatesave = \
        "mov    ax,1c01h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];

/* restore video state */
extern void     _vidstaterestore( unsigned, unsigned, unsigned );
#pragma aux     _vidstaterestore = \
        "mov    ax,1c02h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];

extern void     Fillb( unsigned, unsigned, unsigned char, unsigned );
#pragma aux Fillb = "rep stosb" \
    parm [es] [di] [al] [cx] modify exact [cx di]
