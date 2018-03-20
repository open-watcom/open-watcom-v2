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

#if defined(__OSI__)
 extern  void   __Int10();
 #define _INT_10        "call __Int10"
#else
 #define _INT_10        "int 10h"
#endif

#ifdef _M_I86
#define CALL_INT10(n)   "push bp" INSTR( mov ah, n ) "int 10h" "pop bp"
#else
#define CALL_INT10(n)   "push ebp" INSTR( mov ah, n ) _INT_10 "pop ebp"
#endif

#ifdef _M_I86
#define RealModeSegmPtr( segm )         MK_FP( segm, 0 )
#define RealModeData( segm, off, type ) *(type __far *)MK_FP( segm, off )
#else
#define RealModeSegmPtr( segm )         EXTENDER_RM2PM( segm, 0 )
#define RealModeData( segm, off, type ) *(type __far *)EXTENDER_RM2PM( segm, off )
#endif
#define BIOSData( off, type )           RealModeData( 0x0040, off, type )

#define VIDMONOINDXREG  0x03B4
#define VIDCOLRINDXREG  0x03D4

#define _seq_write( reg, val )          _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )        _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )                _vga_read( SEQ_PORT, reg )
#define _graph_read( reg )              _vga_read( GRA_PORT, reg )

#define CURSOR_REG2INS(r)   (((r + 0x100)/2 + 0x100) & 0xff00) + (r & 0x00ff)

#define MSMOUSE_VECTOR      0x33

#define IRET                0xCF

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

enum vid_state_info {
    VID_STATE_HARDWARE      = 0x1,
    VID_STATE_BIOS          = 0x2,
    VID_STATE_DAC_N_COLOURS = 0x4,
    VID_STATE_ALL           = 0x7,
    VID_STATE_SWAP          = VID_STATE_ALL
};

enum {
    BD_EQUIP_LIST   = 0x10,
    BD_CURR_MODE    = 0x49,
    BD_REGEN_LEN    = 0x4c,
    BD_CURPOS       = 0x50,
    BD_MODE_CTRL    = 0x65,
    BD_VID_CTRL1    = 0x87,
};

typedef enum {
    ADAPTER_MONO = -1,  // -1
    ADAPTER_NONE,       // 0
    ADAPTER_COLOUR      // 1
} adapter_type;

#define DISP_TYPES() \
    pick_disp( DISP_NONE,           ADAPTER_NONE ) \
    pick_disp( DISP_MONOCHROME,     ADAPTER_MONO ) \
    pick_disp( DISP_CGA,            ADAPTER_COLOUR ) \
    pick_disp( DISP_RESERVED1,      ADAPTER_NONE ) \
    pick_disp( DISP_EGA_COLOUR,     ADAPTER_COLOUR ) \
    pick_disp( DISP_EGA_MONO,       ADAPTER_MONO ) \
    pick_disp( DISP_PGA,            ADAPTER_COLOUR ) \
    pick_disp( DISP_VGA_MONO,       ADAPTER_COLOUR ) \
    pick_disp( DISP_VGA_COLOUR,     ADAPTER_COLOUR ) \
    pick_disp( DISP_RESERVED2,      ADAPTER_NONE ) \
    pick_disp( DISP_RESERVED3,      ADAPTER_NONE ) \
    pick_disp( DISP_MODEL30_MONO,   ADAPTER_COLOUR ) \
    pick_disp( DISP_MODEL30_COLOUR, ADAPTER_COLOUR )

typedef enum {
    #define pick_disp(e,t) e,
        DISP_TYPES()
    #undef pick_disp
} hw_display_type;

typedef struct {
     hw_display_type active;
     hw_display_type alt;
} display_config;

#ifdef __WINDOWS__
#define SCREEN_OPTS() \
    pick_opt( OPT_MONO,         "Monochrome" ) \
    pick_opt( OPT_COLOR,        "Color" ) \
    pick_opt( OPT_COLOUR,       "Colour" ) \
    pick_opt( OPT_EGA43,        "Ega43" ) \
    pick_opt( OPT_FASTSWAP,     "FAstswap" ) \
    pick_opt( OPT_VGA50,        "Vga50" ) \
    pick_opt( OPT_OVERWRITE,    "Overwrite" ) \
    pick_opt( OPT_PAGE,         "Page" ) \
    pick_opt( OPT_SWAP,         "Swap" ) \
    pick_opt( OPT_TWO,          "Two" )
#else
#define SCREEN_OPTS() \
    pick_opt( OPT_MONO,         "Monochrome" ) \
    pick_opt( OPT_COLOR,        "Color" ) \
    pick_opt( OPT_COLOUR,       "Colour" ) \
    pick_opt( OPT_EGA43,        "Ega43" ) \
    pick_opt( OPT_VGA50,        "Vga50" ) \
    pick_opt( OPT_OVERWRITE,    "Overwrite" ) \
    pick_opt( OPT_PAGE,         "Page" ) \
    pick_opt( OPT_SWAP,         "Swap" ) \
    pick_opt( OPT_TWO,          "Two" )
#endif

typedef enum {
    #define pick_opt(e,t) e,
        SCREEN_OPTS()
    #undef pick_opt
} screen_opt;

