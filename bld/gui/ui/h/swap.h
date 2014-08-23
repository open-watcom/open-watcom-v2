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


void ToGraphical( void );
void ToCharacter( void );
void InitSwapper( bool );
void FiniSwapper( void );

#ifdef __WANTALL__
enum {
        BD_SEG          = 0x40,
        BD_EQUIP_LIST   = 0x10,
        BD_CURR_MODE    = 0x49,
        BD_REGEN_LEN    = 0x4c,
        BD_CURPOS       = 0x50,
        BD_MODE_CTRL    = 0x65,
        BD_VID_CTRL1    = 0x87
};
#define GetBIOSData( offset, var ) \
    MyMoveData( BD_SEG, offset, FP_SEG( &var ), FP_OFF( &var ), sizeof( var ) );
#define SetBIOSData( offset, var ) \
    MyMoveData( FP_SEG( &var ), FP_OFF( &var ), BD_SEG, offset, sizeof( var ) );

#define VIDCOLRINDXREG  0x03D4

typedef enum {
        DISP_NONE,
        DISP_MONOCHROME,
        DISP_CGA,
        DISP_RESERVED1,
        DISP_EGA_COLOUR,
        DISP_EGA_MONO,
        DISP_PGA,
        DISP_VGA_MONO,
        DISP_VGA_COLOUR,
        DISP_RESERVED2,
        DISP_RESERVED3,
        DISP_MODEL30_MONO,
        DISP_MODEL30_COLOUR } hw_display_type;


typedef struct {
     hw_display_type active;
     hw_display_type alt;
} display_configuration;

extern display_configuration BIOSDevCombCode();
#pragma aux BIOSDevCombCode =                                   \
0X55            /* push   bp                            */      \
0XB8 0X00 0X1A  /* mov    ax,1a00                       */      \
0XCD 0X10       /* int    10                            */      \
0X3C 0X1A       /* cmp    al,1a                         */      \
0X74 0X02       /* jz     *+2                           */      \
0X29 0XDB       /* sub    bx,bx                         */      \
0X5D            /* pop    bp                            */      \
                value   [ bx ]                                  \
                modify  [ ax ];

extern char        BIOSGetMode();
#pragma aux BIOSGetMode =                                       \
0X55            /* push   bp                            */      \
0XB4 0X0F       /* mov    ah,f                          */      \
0XCd 0X10       /* int    10                            */      \
0X5D            /* pop    bp                            */      \
        parm caller [ ax ]                                      \
        modify [ bx ];

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
        parm modify [ bx cx ];

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

#pragma aux     _ega_write =            /* write ega/vga registers */   \
                0xef                    /* out dx,ax */                 \
                parm [dx] [al] [ah]                                     \
                modify exact [];

#pragma aux     _vga_read =             /* read vga registers */        \
                0xee                    /* out dx,al    */              \
                0x42                    /* inc    dx    */              \
                0xec                    /* in al,dx     */              \
                parm [dx] [al]                                          \
                value [al];

#pragma aux Fillb =                                             \
/*      Fillb( toseg, tooff, val, len );                */      \
0XF3            /* rep                                  */      \
0XAA            /* stosb                                */      \
        parm    caller  [ es ] [ di ] [ ax ] [ cx ]             \
        modify  [ di es ];


extern void Fillb( unsigned, unsigned, unsigned, unsigned );
extern void _ega_write( unsigned, char, char );
extern char _vga_read( unsigned, char );
extern void _disablev( unsigned );
extern void _enablev( unsigned );

#define _seq_write( reg, val )          _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )        _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )                _vga_read( SEQ_PORT, reg )
#define _graph_read( reg )              _vga_read( GRA_PORT, reg )


#pragma aux     _disablev =     /* disable video */     \
                0xec            /* in al,dx     */      \
                0xa8 0x08       /* test al,8    */      \
                0x74 0xfb       /* jz -5        */      \
                0xba 0xc0 0x03  /* mov dx,03c0  */      \
                0xb0 0x11       /* mov al,11    */      \
                0xee            /* out dx,al    */      \
                0xb0 0x00       /* mov al,0     */      \
                0xee            /* out dx,al    */      \
                parm [dx]                                       \
                modify [ax dx];


#pragma aux     _enablev =              /* enable video  */     \
                    0xec                /* in al,dx     */      \
                    0xa8 0x08           /* test al,8    */      \
                    0x74 0xfb           /* jz -5        */      \
                    0xba 0xc0 0x03      /* mov dx,03c0  */      \
                    0xb0 0x31           /* mov al,31    */      \
                    0xee                /* out dx,al    */      \
                    0xb0 0x00           /* mov al,0     */      \
                    0xee                /* out dx,al    */      \
                    parm [dx]                                   \
                    modify [ax dx];


enum vid_state_info {
        VID_STATE_HARDWARE      = 0x1,
        VID_STATE_BIOS          = 0x2,
        VID_STATE_DAC_N_COLOURS = 0x4,
        VID_STATE_ALL           = 0x7
};


extern char _A000h[];
extern void FAR PASCAL Death( HDC );
extern void FAR PASCAL Resurrection( HDC, WORD w1, WORD w2, WORD w3, WORD w4,
                                WORD w5, WORD w6 );

extern void MyMoveData( int srcseg, int srcoff, int destseg, int destoff, int size );
#pragma aux MyMoveData = \
        "push   ds" \
        "mov    ds,dx" \
        "rep    movsb" \
        "pop    ds" \
        parm [dx] [si] [es] [di] [cx];

#define VID_STATE       5
#define FONT_SIZE       8*1024

#endif
