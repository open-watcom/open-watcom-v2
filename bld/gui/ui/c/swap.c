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
* Description:  Save/restore screen.
*
****************************************************************************/


#include "guiwind.h"
#include <windows.h>
#include <dos.h>
#include "dpmi.h"
#define __WANTALL__
#include "swap.h"

static BOOL             isGraphical;
static BOOL             isFast;

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

extern int              SwapScrnLines( void );

static void doAnInt10( void )
{
    rmRegs.ss = rmRegs.sp = 0;
    DPMISimulateRealModeInterrupt( 0x10,0,0, &rmRegs );
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
    _disablev( VIDCOLRINDXREG + 6 );
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
    MyMoveData( swapSeg, 0, (WORD) _A000h, 0, pageSize );
    _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
    MyMoveData( swapSeg, pageSize, (WORD) _A000h, 0, pageSize );
    _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
    MyMoveData( swapSeg, pageSize*2, (WORD) _A000h, 0, FONT_SIZE );

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
    MyMoveData( (WORD) _A000h, 0, swapSeg, 0, pageSize );
    _graph_write( GRA_READ_MAP, RMS_MAP_1 );
    MyMoveData( (WORD) _A000h, 0, swapSeg, pageSize, pageSize );
    _graph_write( GRA_READ_MAP, RMS_MAP_2 );
    MyMoveData( (WORD) _A000h, 0, swapSeg, pageSize * 2, FONT_SIZE );
    _graph_write( GRA_READ_MAP, RMS_MAP_0 );

    /*
     * blank regen area (attributes)
     */
    _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
    Fillb( (WORD) _A000h, 0, 0, pageSize );

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

    pageSize = 25 * (80*2) + 256;

    rmRegs.eax = 0xf00;         /* get current video mode */
    doAnInt10();
    saveMode = rmRegs.eax & 0xff;
    cols = (rmRegs.eax >> 8 ) & 0xff;
    disppage = rmRegs.ebx &0xff;

    rmRegs.eax = 0x1c00;        /* get state buffer size */
    rmRegs.ecx = VID_STATE;
    doAnInt10();                /* bx contains number of 64 byte blocks */

    bytes = 64 * (WORD) rmRegs.ebx;
    dosMem = GlobalDosAlloc( (DWORD) bytes );
    size = pageSize*2 + FONT_SIZE;
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
    cnt = ShowCursor( FALSE );
    oldCursorCount = cnt + 1;
    while( cnt >= 0 ) {
        ShowCursor( FALSE );
        cnt--;
    }

}

static void restoreCursor( void )
{
    while( ShowCursor( TRUE ) != oldCursorCount );
    SetCursorPos( oldCursorPos.x, oldCursorPos.y );
}

void ToGraphical( void )
{
    if( isGraphical ) return;
    if( isFast ) {
        toGraphicalFast();
    } else {
        toGraphicalW();
    }
    isGraphical = TRUE;
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

    if( !isGraphical ) return;
    saveCursor();
    if( isFast ) {
        toCharacterFast();
    } else {
        toCharacterW();
    }
    isGraphical = FALSE;
    if( SwapScrnLines() >=43 ) {
        SetCharSet( DOUBLE_DOT_CHR_SET );
    } else if( SwapScrnLines() >= 28 ) {
        SetCharSet( COMPRESSED_CHR_SET );
    }
}

void InitSwapper( BOOL wantfast )
{
    isFast = wantfast;
    if( isFast ) {
        initSwapperFast();
    } else {
        initSwapperW();
    }
    isGraphical = TRUE;
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
