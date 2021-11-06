/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#define _1k                         1024UL
#define _64k                        (64 * _1k)
#define RoundTo1K(s)                (((s) + ( _1k - 1 )) & ~( _1k - 1 ))

#define FONT_TABLE_SIZE             (8 * _1k)

#define ISTEXTMODE( mode )          ((mode) < 4 || (mode) == 7)
#define ISMONOMODE( mode )          ((mode) == 7 || (mode) == 15)

#define CURS_LOCATION_LOW           0x0f
#define CURS_LOCATION_HI            0x0e
#define CURS_START_SCANLINE         0x0a
#define CURS_END_SCANLINE           0x0b

#define _seq_write( reg, val )      _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )    _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )            _ReadCRTCReg( SEQ_PORT, reg )
#define _graph_read( reg )          _ReadCRTCReg( GRA_PORT, reg )

#define VIDGetRow( vidport )        _ReadCRTCReg( vidport, CURS_LOCATION_LOW )
#define VIDSetRow( vidport, row )   _WriteCRTCReg( vidport, CURS_LOCATION_LOW, row )
#define VIDGetCol( vidport )        _ReadCRTCReg( vidport, CURS_LOCATION_HI )
#define VIDSetCol( vidport, col )   _WriteCRTCReg( vidport, CURS_LOCATION_HI, col )

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

typedef enum {
    FLIP_OVERWRITE,
    FLIP_SWAP,
    FLIP_PAGE,
    FLIP_TWO,
    FLIP_CHEAPSWAP
} flip_types;

typedef enum {
    MD_DEFAULT,
    MD_MONO,
    MD_COLOUR,
    MD_EGA,
    MD_HERC
} mode_types;

typedef enum {
    VID_STATE_HARDWARE      = 0x1,
    VID_STATE_BIOS          = 0x2,
    VID_STATE_DAC_N_COLOURS = 0x4,
    VID_STATE_ALL           = 0x7,
    VID_STATE_SWAP          = VID_STATE_ALL
} vid_state_info;

typedef enum {
    ADAPTER_MONO = -1,  // -1
    ADAPTER_NONE,       // 0
    ADAPTER_COLOUR      // 1
} adapter_type;

#define DISP_TYPES() \
    pick( DISP_NONE,            ADAPTER_NONE ) \
    pick( DISP_MONOCHROME,      ADAPTER_MONO ) \
    pick( DISP_CGA,             ADAPTER_COLOUR ) \
    pick( DISP_RESERVED1,       ADAPTER_NONE ) \
    pick( DISP_EGA_COLOUR,      ADAPTER_COLOUR ) \
    pick( DISP_EGA_MONO,        ADAPTER_MONO ) \
    pick( DISP_PGA,             ADAPTER_COLOUR ) \
    pick( DISP_VGA_MONO,        ADAPTER_COLOUR ) \
    pick( DISP_VGA_COLOUR,      ADAPTER_COLOUR ) \
    pick( DISP_RESERVED2,       ADAPTER_NONE ) \
    pick( DISP_RESERVED3,       ADAPTER_NONE ) \
    pick( DISP_MODEL30_MONO,    ADAPTER_COLOUR ) \
    pick( DISP_MODEL30_COLOUR,  ADAPTER_COLOUR )

typedef enum {
    #define pick(dt,at)     dt,
        DISP_TYPES()
    #undef pick
} hw_display_type;

typedef struct {
     hw_display_type active;
     hw_display_type alt;
} display_config;

#ifdef __WINDOWS__
#define SCREEN_OPTS() \
    pick( "Monochrome", OPT_MONO        ) \
    pick( "Color",      OPT_COLOR       ) \
    pick( "Colour",     OPT_COLOUR      ) \
    pick( "Ega43",      OPT_EGA43       ) \
    pick( "FAstswap",   OPT_FASTSWAP    ) \
    pick( "Vga50",      OPT_VGA50       ) \
    pick( "Overwrite",  OPT_OVERWRITE   ) \
    pick( "Page",       OPT_PAGE        ) \
    pick( "Swap",       OPT_SWAP        ) \
    pick( "Two",        OPT_TWO         )
#else
#define SCREEN_OPTS() \
    pick( "Monochrome", OPT_MONO        ) \
    pick( "Color",      OPT_COLOR       ) \
    pick( "Colour",     OPT_COLOUR      ) \
    pick( "Ega43",      OPT_EGA43       ) \
    pick( "Vga50",      OPT_VGA50       ) \
    pick( "Overwrite",  OPT_OVERWRITE   ) \
    pick( "Page",       OPT_PAGE        ) \
    pick( "Swap",       OPT_SWAP        ) \
    pick( "Two",        OPT_TWO         )
#endif

typedef enum {
    #define pick(t,e)   e,
        SCREEN_OPTS()
    #undef pick
} screen_opt;

#ifdef _M_I86
extern void Fillb( unsigned, unsigned, unsigned char, unsigned );
#pragma aux Fillb = \
        "rep stosb"     \
    __parm              [__es] [__di] [__al] [__cx] \
    __value             \
    __modify __exact    [__cx __di]
#else
extern void Fillb( void __far *dst, unsigned char, unsigned );
#pragma aux Fillb = \
        "rep stosb"     \
    __parm              [__es __edi] [__al] [__ecx] \
    __value             \
    __modify __exact    [__ecx __edi]
#endif

extern void VIDWait( void );
#pragma aux VIDWait = "jmp short L1" "L1:"

extern unsigned char _ReadCRTCReg( unsigned short vidport, unsigned char regnb );
#pragma aux _ReadCRTCReg = \
        "out  dx,al"    \
        "inc  dx"       \
        "in   al,dx"    \
    __parm              [__dx] [__al] \
    __value             [__al] \
    __modify __exact    [__al __dx]

extern void _WriteCRTCReg( unsigned short vidport, unsigned char regnb, unsigned char value );
#pragma aux _WriteCRTCReg = \
        "out  dx,al"        \
        "inc  dx"           \
        "mov  al,ah"        \
        "out  dx,al"        \
    __parm              [__dx] [__al] [__ah] \
    __value             \
    __modify __exact    [__al __dx]

/* write ega/vga registers */
extern void _ega_write( unsigned short, unsigned char, unsigned char );
#pragma aux _ega_write = \
        "out  dx,ax"        \
    __parm              [__dx] [__al] [__ah] \
    __value             \
    __modify __exact    []

/* read vga registers */
extern unsigned char _vga_read( unsigned short, unsigned char );
#pragma aux _vga_read = \
        "out  dx,al"        \
        "inc  dx"           \
        "in   al,dx"        \
    __parm              [__dx] [__al] \
    __value             [__al] \
    __modify __exact    [__al __dx]

extern void _disablev( unsigned short );
#pragma aux _disablev = \
    "L1: in   al,dx"        \
        "test al,8"         \
        "jz short L1"       \
        "mov  dx,3c0h"      \
        "mov  al,11h"       \
        "out  dx,al"        \
        "xor  al,al"        \
        "out  dx,al"        \
    __parm              [__dx] \
    __value             \
    __modify __exact    [__al __dx]

extern void _enablev( unsigned short );
#pragma aux _enablev = \
    "L1: in   al,dx"        \
        "test al,8"         \
        "jz short L1"       \
        "mov  dx,3c0h"      \
        "mov  al,31h"       \
        "out  dx,al"        \
        "xor  al,al"        \
        "out  dx,al"        \
    __parm              [__dx] \
    __value             \
    __modify __exact    [__al __dx]
