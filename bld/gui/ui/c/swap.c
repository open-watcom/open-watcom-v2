/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Save/restore screen
*               (character mode 16-bit Windows code only)
*
****************************************************************************/


#include "guiwind.h"
#include <windows.h>
#include <dos.h>
#include "dpmi.h"
#include "swapline.h"
#include "swap.h"


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
    DISP_MODEL30_COLOUR
} hw_display_type;


typedef struct {
    hw_display_type active;
    hw_display_type alt;
} display_configuration;

extern display_configuration BIOSDevCombCode( void );
#pragma aux BIOSDevCombCode = \
        "push bp"       \
        "mov  ax,1a00h" \
        "int 10h"       \
        "cmp  al,1ah"   \
        "jz short L1"   \
        "sub  bx,bx"    \
    "L1: pop  bp"       \
    __parm              [] \
    __value             [__bx] \
    __modify __exact    [__ax __bx]

extern char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "push bp"       \
        "mov  ah,0fh"   \
        "int 10h"       \
        "pop  bp"       \
    __parm              [] \
    __value             [__al] \
    __modify __exact    [__ax __bh]

extern signed long BIOSEGAInfo( void );
#pragma aux BIOSEGAInfo = \
        "push bp"       \
        "mov  ah,12h"   \
        "mov  bl,10h"   \
        "mov  bh,0ffh"  \
        "int 10h"       \
        "mov  ax,bx"    \
        "mov  dx,cx"    \
        "pop  bp"       \
    __parm              [] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx]

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
extern void _ega_write( unsigned, char, char );
#pragma aux _ega_write = \
        "out dx,ax"     \
    __parm              [__dx] [__al] [__ah] \
    __value             \
    __modify __exact    []

/* read vga registers */
extern char _vga_read( unsigned, char );
#pragma aux _vga_read = \
        "out dx,al"     \
        "inc dx"        \
        "in  al,dx"     \
    __parm              [__dx] [__al] \
    __value             [__al] \
    __modify __exact    [__al __dx]

extern void Fillb( unsigned, unsigned, unsigned, unsigned );
#pragma aux Fillb =     \
        "rep stosb"     \
    __parm __caller     [__es] [__di] [__ax] [__cx] \
    __value             \
    __modify __exact    [__cx __di]

#define _seq_write( reg, val )          _ega_write( SEQ_PORT, reg, val )
#define _graph_write( reg, val )        _ega_write( GRA_PORT, reg, val )
#define _seq_read( reg )                _vga_read( SEQ_PORT, reg )
#define _graph_read( reg )              _vga_read( GRA_PORT, reg )

/* disable video */
extern void _disable_video( unsigned );
#pragma aux _disable_video = \
    "L1: in al,dx"      \
        "test al,8"     \
        "jz short L1"   \
        "mov dx,3c0h"   \
        "mov al,11h"    \
        "out dx,al"     \
        "mov al,0"      \
        "out dx,al"     \
    __parm              [__dx] \
    __value             \
    __modify __exact    [__al __dx]

/* enable video  */
extern void _enable_video( unsigned );
#pragma aux _enable_video = \
    "L1: in al,dx"      \
        "test al,8"     \
        "jz short L1"   \
        "mov dx,3c0h"   \
        "mov al,31h"    \
        "out dx,al"     \
        "mov al,0"      \
        "out dx,al"     \
    __parm              [__dx] \
    __value             \
    __modify __exact    [__al __dx]

enum vid_state_info {
    VID_STATE_HARDWARE      = 0x1,
    VID_STATE_BIOS          = 0x2,
    VID_STATE_DAC_N_COLOURS = 0x4,
    VID_STATE_ALL           = 0x7
};


extern char _A000h[];
extern void FAR PASCAL Death( HDC );
extern void FAR PASCAL Resurrection( HDC, WORD w1, WORD w2, WORD w3, WORD w4, WORD w5, WORD w6 );

extern void MyMoveData( unsigned srcseg, unsigned srcoff, unsigned destseg, unsigned destoff, unsigned size );
#pragma aux MyMoveData = \
        "push   ds"     \
        "mov    ds,dx"  \
        "rep    movsb"  \
        "pop    ds"     \
    __parm              [__dx] [__si] [__es] [__di] [__cx] \
    __value             \
    __modify __exact    [__cx __di __si]

#define VID_STATE       5
#define FONT_SIZE       8 * 1024

static bool             isGraphical;
static bool             isFast;

static DWORD            dosMem;
static WORD             pageSize;
static WORD             swapSeg;
static GLOBALHANDLE     swapHandle;
static WORD             saveMode;
static WORD             scanLines;
static WORD             fontType;

static rm_call_struct   rmRegs;
static int              screenX;
static int              screenY;
static HBITMAP          screenBitmap;
static HDC              screenDC;
static HDC              screenMemDC;

static int              oldCursorCount;
static POINT            oldCursorPos;

static void doAnInt10( void )
{
    rmRegs.ss = rmRegs.sp = 0;
    DPMISimulateRealModeInterrupt( 0x10, 0, 0, &rmRegs );
}


/*
 * toGraphicalW - switch to graphical screen, using windows only
 */
static void toGraphicalW( void )
{
    HBITMAP     old;

    Resurrection( screenDC, 0, 0, 0, 0, 0, 0 );

    old = SelectObject( screenMemDC, screenBitmap );
    BitBlt( screenDC, 0, 0, screenX, screenY, screenMemDC, 0, 0, SRCCOPY );
    SelectObject( screenMemDC, old );

    DeleteObject( screenBitmap );
    DeleteDC( screenMemDC );
    ReleaseDC( NULL, screenDC );

} /* toGraphicalW */

/*
 * toCharacterW - switch to character mode screen, using windows only
 */
static void toCharacterW( void )
{
    HBITMAP     old;


    screenDC = GetDC( NULL );
    screenMemDC = CreateCompatibleDC( NULL );
    screenBitmap = CreateCompatibleBitmap( screenDC, screenX, screenY );

    old = SelectObject( screenMemDC, screenBitmap );
    BitBlt( screenMemDC, 0, 0, screenX, screenY, screenDC, 0, 0, SRCCOPY );
    SelectObject( screenMemDC, old );

    Death( screenDC );

} /* toCharacterW */

/*
 * initSwapperW - get set up for screen swapping, using windows only
 */
static void initSwapperW( void )
{
    screenX = GetSystemMetrics( SM_CXSCREEN );
    screenY = GetSystemMetrics( SM_CYSCREEN );

} /* initSwapperW */

/*
 * finiSwapperW - done screen swapping, using windows only
 */
static void finiSwapperW( void )
{
} /* finiSwapperW */

/*
 * setRegenClear - mark regen buffer as clear
 */
static void setRegenClear( void )
{
    unsigned char regen;

    GetBIOSData( BD_VID_CTRL1, regen );
    regen &= 0x7f;
    regen |= saveMode & 0x80;
    SetBIOSData( BD_VID_CTRL1, regen );

} /* setREgenClear */

/*
 * setupEGA - set up for accessing EGA
 */
static void setupEGA( void )
{
    _disable_video( VIDCOLRINDXREG + 6 );
    _seq_write( SEQ_MEM_MODE, MEM_NOT_ODD_EVEN );
    _graph_write( GRA_MISC, MIS_A000_64 | MIS_GRAPH_MODE );
    _graph_write( GRA_ENABLE_SR, 0 );
    _graph_write( GRA_DATA_ROT, ROT_UNMOD | 0 );
    _graph_write( GRA_GRAPH_MODE, GRM_EN_ROT );

} /* setupEGA */

/*
 * toGraphicalFast - switch to graphical mode, using direct method
 */
static void toGraphicalFast( void )
{
    rmRegs.eax = 0x1201;
    rmRegs.ebx = 0x36;
    doAnInt10();

    setupEGA();

    _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
    MyMoveData( swapSeg, 0, (WORD)_A000h, 0, pageSize );
    _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
    MyMoveData( swapSeg, pageSize, (WORD)_A000h, 0, pageSize );
    _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
    MyMoveData( swapSeg, pageSize * 2, (WORD)_A000h, 0, FONT_SIZE );

    rmRegs.eax = saveMode | 0x80;
    doAnInt10();

    setRegenClear();

    rmRegs.eax = 0x1200;
    rmRegs.ebx = 0x36;
    doAnInt10();
    rmRegs.es = HIWORD( dosMem );
    rmRegs.ebx = 0;
    rmRegs.ecx = VID_STATE;
    rmRegs.eax = 0x1c02;                /* restore state info */
    doAnInt10();
    rmRegs.es = HIWORD( dosMem );
    rmRegs.ebx = 0;
    rmRegs.ecx = VID_STATE;
    rmRegs.eax = 0x1c01;                /* save state info */
    doAnInt10();

} /* toGraphicalFast */

/*
 * toCharacterFast - switch to character mode screen, using direct method
 */
static void toCharacterFast( void )
{
    rmRegs.es = HIWORD( dosMem );
    rmRegs.ebx = 0;
    rmRegs.ecx = VID_STATE;
    rmRegs.eax = 0x1c01;        /* copy state info */
    doAnInt10();

    rmRegs.eax = 0x1201;        /* ah=12,bl=36: video refresh ctl; al=01: disable */
    rmRegs.ebx = 0x36;
    doAnInt10();

    setupEGA();
    _graph_write( GRA_READ_MAP, RMS_MAP_0 );
    MyMoveData( (WORD)_A000h, 0, swapSeg, 0, pageSize );
    _graph_write( GRA_READ_MAP, RMS_MAP_1 );
    MyMoveData( (WORD)_A000h, 0, swapSeg, pageSize, pageSize );
    _graph_write( GRA_READ_MAP, RMS_MAP_2 );
    MyMoveData( (WORD)_A000h, 0, swapSeg, pageSize * 2, FONT_SIZE );
    _graph_write( GRA_READ_MAP, RMS_MAP_0 );

    /*
     * blank regen area (attributes)
     */
    _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
    Fillb( (WORD)_A000h, 0, 0, pageSize );

    rmRegs.eax = scanLines;
    rmRegs.ebx= 0x30;
    doAnInt10();

    rmRegs.eax = 0x83;          /* ah=0: set video mode, al=83 */
    doAnInt10();
    setRegenClear();

    rmRegs.eax = fontType;
    rmRegs.ebx = 0;
    doAnInt10();

} /* toCharacterFast */

/*
 * finiSwapperFast - done screen swapping, using direct method
 */
static void finiSwapperFast( void )
{
    GlobalDosFree( LOWORD( dosMem ) );
    if( swapHandle != 0 ) {
        GlobalUnlock( swapHandle );
        GlobalFree( swapHandle );
    }

} /* finiSwapperFast */

/*
 * initSwapperFast- get set up for screen swapping, using direct method
 */
static void initSwapperFast( void )
{
    void        *tmp;
    int         cols,disppage;
    WORD        bytes,size;

    pageSize = 25 * ( 80 * 2 ) + 256;

    rmRegs.eax = 0xf00;         /* get current video mode */
    doAnInt10();
    saveMode = rmRegs.eax & 0xff;
    cols = ( rmRegs.eax >> 8 ) & 0xff;
    disppage = rmRegs.ebx & 0xff;

    rmRegs.eax = 0x1c00;        /* get state buffer size */
    rmRegs.ecx = VID_STATE;
    doAnInt10();                /* bx contains number of 64 byte blocks */

    bytes = 64 * (WORD)rmRegs.ebx;
    dosMem = GlobalDosAlloc( (DWORD)bytes );
    size = pageSize * 2 + FONT_SIZE;
    swapHandle = GlobalAlloc( GMEM_FIXED, size );
    tmp = GlobalLock( swapHandle );
    swapSeg = FP_SEG( tmp );

    fontType = 0x1114;
    scanLines = 0x1202;

} /* initSwapperFast */

static void saveCursor( void )
{
    int cnt;

    GetCursorPos( &oldCursorPos );
    cnt = ShowCursor( false );
    oldCursorCount = cnt + 1;
    while( cnt >= 0 ) {
        ShowCursor( false );
        cnt--;
    }

}

static void restoreCursor( void )
{
    while( ShowCursor( true ) != oldCursorCount );
    SetCursorPos( oldCursorPos.x, oldCursorPos.y );
}

void ToGraphical( void )
{
    if( isGraphical )
        return;
    if( isFast ) {
        toGraphicalFast();
    } else {
        toGraphicalW();
    }
    isGraphical = true;
    restoreCursor();
}

#define DOUBLE_DOT_CHR_SET      0x12
#define COMPRESSED_CHR_SET      0x11
#define USER_CHR_SET            0x00

static void SetCharSet( unsigned char c )
{
    rmRegs.eax = 0x1100 + c;
    rmRegs.ebx = 0;
    doAnInt10();
}

void ToCharacter( void )
{

    if( !isGraphical )
        return;
    saveCursor();
    if( isFast ) {
        toCharacterFast();
    } else {
        toCharacterW();
    }
    isGraphical = false;
    if( SwapScrnLines() >=43 ) {
        SetCharSet( DOUBLE_DOT_CHR_SET );
    } else if( SwapScrnLines() >= 28 ) {
        SetCharSet( COMPRESSED_CHR_SET );
    }
}

void InitSwapper( bool wantfast )
{
    isFast = wantfast;
    if( isFast ) {
        initSwapperFast();
    } else {
        initSwapperW();
    }
    isGraphical = true;
}

void FiniSwapper( void )
{
    ToCharacter();
    rmRegs.eax = 0x0003;
    doAnInt10();
    ToGraphical();
    if( isFast ) {
        finiSwapperFast();
    } else {
        finiSwapperW();
    }
}
