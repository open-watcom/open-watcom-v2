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


#include "watcom.h"

#if defined(__OSI__)
 extern  void   __Int10();
 #define _INT_10        "call __Int10"
#else
 #define _INT_10        "int 0x10"
#endif


#define VIDMONOINDXREG                  0x03b4
#define VIDCOLRINDXREG                  0x03d4
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
    VID_STATE_ALL           = 0x7
};


extern void BIOSSetPage( unsigned_8 pagenb );
#pragma aux BIOSSetPage =                               \
        " push   ebp            "                       \
        " mov    ah, 5          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ al ]                                     \
        modify exact [ ah ];

extern unsigned_8 BIOSGetPage( void );
#pragma aux BIOSGetPage =                               \
        " push   ebp            "                       \
        " mov    ah, 0fh        "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        modify exact [ ax bh ]                          \
        value [ bh ];

extern unsigned_8 BIOSGetMode( void );
#pragma aux BIOSGetMode =                               \
        " push   ebp            "                       \
        " mov    ah, 0fh        "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        modify exact [ ax bh ];

extern void BIOSSetMode( unsigned_8 mode );
#pragma aux BIOSSetMode =                               \
        " push   ebp            "                       \
        " mov    ah, 0          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ al ]                                     \
        modify exact [ ah ];

extern unsigned_16 BIOSGetCurPos( unsigned_8 pagenb );
#pragma aux BIOSGetCurPos =                             \
        " push   ebp            "                       \
        " mov    ah, 3          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ bh ]                                     \
        modify exact [ cx dx ]                          \
        value [ dx ];

extern void BIOSSetCurPos( unsigned_16 rowcol, unsigned_8 pagenb );
#pragma aux BIOSSetCurPos =                             \
        " push   ebp            "                       \
        " mov    ah, 2          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ dx ] [ bh ]                              \
        modify exact [ ah ];

extern unsigned_16 BIOSGetCurTyp( unsigned_8 pagenb );
#pragma aux BIOSGetCurTyp =                             \
        " push   ebp            "                       \
        " mov    ah, 3          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ bh ]                                     \
        modify exact [ ah cx dx ]                       \
        value [ cx ];

extern void BIOSSetCurTyp( unsigned_16 startend );
#pragma aux BIOSSetCurTyp =                             \
        " push   ebp            "                       \
        " mov    ah, 1          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ cx ]                                     \
        modify exact [ ah ];

extern unsigned_8 BIOSGetAttr( unsigned_8 pagenb );
#pragma aux BIOSGetAttr =                               \
        " push   ebp            "                       \
        " mov    ah, 8          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm  [ bh ]                                    \
        modify exact [ ax ]                             \
        value [ ah ];

extern void BIOSSetAttr( unsigned_8 attr );
#pragma aux BIOSSetAttr =                               \
        " push   ebp            "                       \
        " mov    cx, 0          "                       \
        " mov    dx, 3250h      "                       \
        " mov    ax, 600h       "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ bh ]                                     \
        modify exact [ ax cx dx ];

extern uint_8 _ReadCRTCReg( uint_16 vidport, uint_8 regnb );
#pragma aux _ReadCRTCReg =                      \
        "out    dx, al          ",              \
        "inc    dx              ",              \
        "in     al, dx          "               \
        parm [ dx ] [ al ]                      \
        modify exact [ al dx ];

extern void _WriteCRTCReg( uint_16 vidport, uint_8 regnb, uint_8 value );
#pragma aux _WriteCRTCReg =                     \
        "out    dx, al          ",              \
        "inc    dx              ",              \
        "mov    al, ah          ",              \
        "out    dx, al          "               \
        parm [ dx ] [ al ] [ ah ]               \
        modify exact [ al dx ];

#if 0

extern unsigned_8 VIDGetRow( unsigned_16 vidport );
#pragma aux VIDGetRow =                                 \
        " mov    al, 0fh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         "                       \
        parm [ dx ]                                     \
        modify exact [ al dx ];

extern void VIDSetRow( unsigned_16 vidport, unsigned_8 row );
#pragma aux VIDSetRow =                                 \
        " mov    al, 0fh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " mov    al, ah         ",                      \
        " out    dx, al         "                       \
        parm [ dx ] [ ah ]                              \
        modify exact [ ax dx ];

extern void VIDSetPos( unsigned_16 vidport, unsigned_16 cursorpos );
#pragma aux VIDSetPos =                                 \
        " mov    al, 0fh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " mov    al, bl         ",                      \
        " out    dx, al         ",                      \
        " dec    dx             ",                      \
        " mov    al, 0eh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " mov    al, bh         ",                      \
        " out    dx, al         "                       \
        parm [ dx ] [ bx ]                              \
        modify exact [ al dx ];

extern unsigned_16 VIDGetPos( unsigned_16 vidport );
#pragma aux VIDGetPos =                                 \
        " mov    al, 0fh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         ",                      \
        " mov    ah, al         ",                      \
        " dec    dx             ",                      \
        " mov    al, 0eh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         ",                      \
        " xchg   ah, al         "                       \
        parm [ dx ]                                     \
        modify exact [ ax dx ];

extern unsigned_8 VIDGetCol( unsigned_16 vidport );
#pragma aux VIDGetCol =                                 \
        " mov    al, 0eh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         "                       \
        parm [ dx ]                                     \
        modify exact [ al dx ];

extern void VIDSetCurTyp( unsigned_16 vidport, unsigned_16 cursortyp );
#pragma aux VIDSetCurTyp =                              \
        " push   ax             ",                      \
        " mov    al, 0ah        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " mov    al, ah         ",                      \
        " out    dx, al         ",                      \
        " dec    dx             ",                      \
        " mov    al, 0bh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " pop    ax             ",                      \
        " out    dx, al         "                       \
        parm [ dx ] [ ax ]                              \
        modify exact [ al dx ];

extern unsigned_16 VIDGetCurTyp( unsigned_16 vidport );
#pragma aux VIDGetCurTyp =                              \
        " mov    al, 0bh        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         ",                      \
        " mov    ah, al         ",                      \
        " dec    dx             ",                      \
        " mov    al, 0ah        ",                      \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         ",                      \
        " xchg   ah, al         "                       \
        parm [ dx ]                                     \
        modify exact [ ax dx ];

#endif

extern void VIDWait( void );
#pragma aux VIDWait =                                   \
        " jmp  x                "                       \
        " x:                    "                       \
        modify exact [ ];

extern unsigned BIOSEGAInfo( void );
#pragma aux BIOSEGAInfo =                               \
        " push   ebp            "                       \
        " mov    ah, 12h        "                       \
        " mov    bx, 0ff10h     "                       \
        _INT_10                                         \
        " shl    ebx, 10h       "                       \
        " mov    bx, cx         "                       \
        " pop    ebp            "                       \
        modify exact [ ah ebx cx ]                      \
        value [ ebx ];

extern unsigned_8 BIOSGetRows( void );
#pragma aux BIOSGetRows =                               \
        " push  es              "                       \
        " push  ebp             "                       \
        " mov   ax, 1130h       "                       \
        " mov   bh, 0           "                       \
        _INT_10                                         \
        " inc   dl              "                       \
        " pop   ebp             "                       \
        " pop   es              "                       \
/*      modify exact [ ax bh cx dl ] */                 \
        modify exact [ ax ebx ecx edx edi ] /* workaround bug in DOS4G */\
        value [ dl ];

extern unsigned_16 BIOSGetPoints( void );
#pragma aux BIOSGetPoints =                             \
        " push  es              "                       \
        " push  ebp             "                       \
        " mov   ax, 1130h       "                       \
        " mov   bh, 0           "                       \
        _INT_10                                         \
        " pop   ebp             "                       \
        " pop   es              "                       \
/*      modify exact [ ax bh cx dl ] */                 \
        modify exact [ ax ebx ecx edx edi ] /* workaround bug in DOS4G */\
        value [ cx ];

extern void BIOSEGAChrSet( unsigned_8 vidroutine );
#pragma aux BIOSEGAChrSet =                             \
        " push   ebp            "                       \
        " mov    ah, 11h        "                       \
        " mov    bl, 0          "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        parm [ al ]                                     \
        modify exact [ ah bl ];

extern void _ega_write( unsigned_16, unsigned_8, unsigned_8 );
#pragma aux _ega_write =                                \
        " out    dx, ax         "                       \
        parm [ dx ] [ al ] [ ah ]                       \
        modify exact [ ];

extern unsigned_8 _vga_read( unsigned_16, unsigned_8 );
#pragma aux _vga_read =                                 \
        " out    dx, al         ",                      \
        " inc    dx             ",                      \
        " in     al, dx         "                       \
        parm [ dx ] [ al ]                              \
        modify exact [ al dx ];

extern void _disablev( unsigned_16 );
#pragma aux _disablev =                                 \
        " again:                "                       \
        " in     al, dx         ",                      \
        " test   al, 8          ",                      \
        " je     again          ",                      \
        " mov    dx, 3c0h       ",                      \
        " mov    al, 11h        ",                      \
        " out    dx, al         ",                      \
        " mov    al, 0          ",                      \
        " out    dx, al         "                       \
        parm [ dx ]                                     \
        modify exact [ al dx ];

extern void _enablev( unsigned_16 );
#pragma aux _enablev =                                  \
        " again:                "                       \
        " in     al, dx         ",                      \
        " test   al, 8          ",                      \
        " je     again          ",                      \
        " mov    dx, 3c0h       ",                      \
        " mov    al, 31h        ",                      \
        " out    dx, al         ",                      \
        " mov    al, 0          ",                      \
        " out    dx, al         "                       \
        parm [ dx ]                                     \
        modify exact [ al dx ];

extern unsigned_16 _vidstatesize( unsigned_16 requestedstate );
#pragma aux _vidstatesize =                             \
        " mov    ax, 1c00h      "                       \
        _INT_10                                         \
        " cmp    al, 1ch        "                       \
        " je     end            "                       \
        " mov    bx, 0          "                       \
        " end:                  "                       \
        parm [ cx ]                                     \
        modify exact [ ax bx ]                          \
        value [ bx ];

extern void Fillb( void __far *dst, unsigned_8 value, unsigned_16 count );
#pragma aux Fillb =                                     \
        "db 0x65"                                       \
        " rep   stosb           "                       \
        parm [ gs edi ] [ al ] [ ecx ]                  \
        modify exact [ edi ecx ];
