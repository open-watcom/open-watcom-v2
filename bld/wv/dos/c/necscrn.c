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


#include "necscrio.h"
#include "dbgdefn.h"
#include "dbgmem.h"
#include <io.h>
#include <stdui.h>
#include <dos.h>
#include <string.h>
#include "dbgscrn.h"
#include "tinyio.h"
#include "dbgswtch.h"
#include "dbginstr.h"

extern unsigned inp(unsigned __port);
extern unsigned outp(unsigned __port, unsigned __value);
extern void WndDirty(void);
extern void StartupErr(char *);
extern void uisetcurrpage(int);

extern screen_state     ScrnState;

flip_types              FlipMech;

static addr_seg         SwapSeg;
static char             SavePage;
static char             SaveMode;
static unsigned int     PageSize;
static int              SavCurPos;

extern bool             SavTextScrnOn = TRUE; /* assume screen on initially */
extern bool             SavGrScrnOn   = FALSE; /*assume graphic off initially*/
extern bool             SavCurBlinkOn = TRUE;/*assume cursor blinks initially*/
extern bool             SavCursorOn   = TRUE; /*assume cursor on initially */

void GetGdcCursor( int *CurPos )
/***********************************************************************
 GetGdcCursor: ( STATIC )
 ------------
    Input:      &CurPos : pointer to CurPos
    Output:     NONE

    This function uses NEC GDC to get current cursor location regardless
    of current page and put the value at input position.
************************************************************************/
{
    int     i;
    short   ead;

    for(;;) {                                   /* read gdc status register */
        GdcMask();                              /* disable interrupts */
        if( (inp( 0x60 ) & 0x04) ) break;       /* quit if fifo empty */
        GdcDelay();
        GdcUnMask();                            /* restore interrupts */
    }
    GdcDelay();
    outp( 0x62, 0xE0 );                         /* gdc CURD command */
    for(;;) {                                   /* read gdc status register */
        GdcDelay();
        if( (inp( 0x60 ) & 0x01) ) break;       /* quit if data ready */
    }
    GdcDelay();
    ead = inp( 0x62 );                          /* read gdc EAD low */
    GdcDelay();
    ead |= (inp( 0x62 ) << 8);                  /* read gdc EAD high */
    for( i = 0; i < 3; i++ ) {                  /* skip next 3 bytes includ'g */
        GdcDelay();                             /* 2 highest bits of EAD */
        inp( 0x62 );                            /* read gdc */
    }
    GdcUnMask();                                /* restore interrupts */

    /* change to relative address from 0xA000 */
    *CurPos = ead * 2;

}


static void SaveBIOSSettings( void )
/***********************************************************************
 SaveBIOSSettings: ( STATIC )
 ----------------
    Input:      pointer to the mouse position storage variable
    Output:     NONE

    This function saves current BIOS settings.  Currently I only worry
    about cursor location and current page number, but for enhancement
    we have to care about cursor states ( on, off ), cursor shape, font
    set(?), current screen mode etc.

    Note: There is no easy way to figure out the state of the textscreen,
    graphic screen, the cursor type and the on or off of the cursor. I have
    assumed some initial conditions for them. See declarations of variables
    SavTextScrnOn etc.
************************************************************************/
{
/*    SaveSwtchs = GetSwtchs();         no dual-monitor */
    SaveMode = BIOSGetMode();
    GetGdcCursor( &SavCurPos );
    if ( SavCurPos < PageSize ) {
        SavePage = 0;
    } else {
        SavePage = 1;
    }
}


static void SetPage( char PageNum )
/***********************************************************************
 SetPage: ( STATIC )
 -------
    Input:      New Page Number
    Output:     NONE

    This function set current page to "PageNum" and also reset CurrentPage
    so that UI Layer can write to new location.
************************************************************************/
{
    BIOSSetPage( PageNum );
    uisetcurrpage( PageNum );
}

void NecSysSetPage( int PgNum )
/***********************************************************************
 NecSysSetPage: ( EXTERNAL )
 -------------
    Input:      New Page Number
    Output:     NONE

    This function set current page to "PageNum". NEC DOS always writes
    to page 0. Therefore, we have to swap out the user screen for the
    "system" command. And swap it back in when "system" is done. This
    only applies to the "page" option.
************************************************************************/
{
    if( FlipMech == FLIP_PAGE ) {
        if( PgNum == 0 ) {
            AllocSwapSeg();
            movedata( 0xA000, 0, SwapSeg, 0, PageSize );
            movedata( 0xA200, 0, SwapSeg, PageSize, PageSize );
        } else {
            movedata( SwapSeg, 0, 0xA000, 0, PageSize );
            movedata( SwapSeg, PageSize, 0xA200, 0, PageSize );
            DeallocSwapSeg();
        }
        SetPage( PgNum );
    }
}


void RingBell()
/***********************************************************************
 RingBell: ( EXTERNAL )
 --------
    Input:      NONE
    Output:     NONE

    This function suppose to ring a bell but NEC BIOS book lied to me,
    this function doesn't work as book described...

    Well, I changed it so now I think it ought to work -
    RingBell calls RING_BELL_, a macro defined in necscrio.h, which calls
    the aux DoRingBell, which writes a 7h to stderr directly.
************************************************************************/
{
    RING_BELL_
}

unsigned ConfigScreen()
/***********************************************************************
 ConfigScreen: ( EXTERNAL )
 ------------
    Input:      NONE
    Output:     NONE

    This function figure out screen configuration we're going to use.
************************************************************************/
{
    return( 0 );
}


static void DeallocSwapSeg()
/***********************************************************************
 AllocSwapSeg: ( EXTERNAL )
 ------------
    Input:      NONE
    Output:     pointer to the allocated segment

    Allocate memory for a screen swap segment.
************************************************************************/
{
    TinyFreeBlock( SwapSeg );
}

static void AllocSwapSeg()
/***********************************************************************
 AllocSwapSeg: ( EXTERNAL )
 ------------
    Input:      NONE
    Output:     pointer to the allocated segment

    Allocate memory for a screen swap segment.
************************************************************************/
{
    tiny_ret_t      ret;

    /* allocate space for swap segment.  Since NEC uses twice space as
     * IBM PC for video representation, we have to allocate PageSize * 2
     * for NEC screen representation.
     */
    ret = TinyAllocBlock( PageSize >> 3 );
    if( ret < 0 ) StartupErr( "unable to allocate swap area" );
    SwapSeg = ret;
}


void InitScreen()
/***********************************************************************
 InitScreen: ( EXTERNAL )
 ----------
    Input:      NONE
    Output:     NONE

    Initialize program and debugger screen.
************************************************************************/
{
    char            __far *vect;

    /* check for Microsoft mouse */
    vect = *((char __far * __far *)MK_FP( 0, 4*0x33 ));
    if( vect == NULL || *vect == IRET ) _SwitchOff( SW_USE_MOUSE );

    PageSize =  ( UIData->height == 25 ) ? 4096 :
                ( UIData->height * UIData->width * 2 + 256 );

    switch( FlipMech ) {
    case FLIP_SWAP:
        AllocSwapSeg();
        SaveBIOSSettings();
        movedata( 0xA000, 0, SwapSeg, 0, PageSize );
        movedata( 0xA200, 0, SwapSeg, PageSize, PageSize );
        break;
    case FLIP_PAGE:
        SaveBIOSSettings();
        SetPage( NEC_DEBUG_SCREEN );
        break;
    case FLIP_TWO:
        break;
    case FLIP_OVERWRITE:
        SaveBIOSSettings();
        break;
    }
}


bool UsrScrnMode()
/***********************************************************************
 UsrScrnMode: ( EXTERNAL )
 -----------
    Input:      NONE
    Output:     NONE

    setup the user screen mode
************************************************************************/
{
    return( FALSE );
}


void DbgScrnMode()
/***********************************************************************
 DbgScrnMode: ( EXTERNAL )
 -----------
    Input:      NONE
    Output:     NONE

    setup the debugger screen mode
************************************************************************/
{
    if( FlipMech == FLIP_PAGE ) {
        SetPage( NEC_DEBUG_SCREEN );
        WndDirty();
    }
}


bool DebugScreen()
/***********************************************************************
 DebugScreen: ( EXTERNAL )
 -----------
    Input:      NONE
    Output:     NONE

    swap/page to debugger screen
************************************************************************/
{
    bool    usr_vis;

    _NEC_DBG_IN( "***** DebugScreen()\r\n" );
    usr_vis = TRUE;
    SaveBIOSSettings();
    switch( FlipMech ) {
    case FLIP_SWAP:
        movedata( 0xA000, 0, SwapSeg, 0, PageSize );
        movedata( 0xA200, 0, SwapSeg, PageSize, PageSize );
        WndDirty();
        usr_vis = FALSE;
        break;
    case FLIP_PAGE:
        SetPage( NEC_DEBUG_SCREEN );
        WndDirty();
        usr_vis = FALSE;
        break;
    case FLIP_OVERWRITE:
        SaveBIOSSettings();
        WndDirty();
        usr_vis = FALSE;
        break;
    }
    if( !SavTextScrnOn ) {
        BIOSTextOn();
    }
    if( SavGrScrnOn ) {
        BIOSGraphOff();
    }
    uiswap();
    return( usr_vis );
}


bool UserScreen()
/***********************************************************************
 UserScreen: ( EXTERNAL )
 ----------
    Input:      NONE
    Output:     NONE

    swap/page to user screen
************************************************************************/
{
    bool    dbg_vis;

    dbg_vis = TRUE;
    uiswap();
    switch( FlipMech ) {
    case FLIP_SWAP:
        movedata( SwapSeg, 0, 0xA000, 0, PageSize );
        movedata( SwapSeg, PageSize, 0xA200, 0, PageSize );
        dbg_vis = FALSE;
        break;
    case FLIP_PAGE:
        SetPage( NEC_USER_SCREEN );
        dbg_vis = FALSE;
        break;
    }
    if( !SavTextScrnOn ) {
        BIOSTextOff();
    }
    if( SavGrScrnOn ) {
        BIOSGraphOn();
    }
    BIOSSetMode( SaveMode );
    BIOSSetCurPos( SavCurPos );
    if( !SavCurBlinkOn ) {
        BIOSSetCurBlinkOff();
    } else {
        BIOSSetCurBlinkOn();
    }
    if( !SavCursorOn ) {
        BIOSSetCursorOff();
    } else {
        BIOSSetCursorOn();
    }
    return( dbg_vis );
}


void FiniScreen()
/***********************************************************************
 FiniScreen: ( EXTERNAL )
 ----------
    Input:      NONE
    Output:     NONE

    finish screen swapping/paging
************************************************************************/
{
    UserScreen();
    BIOSTextOn();       /* because we assume the text screen is initially on*/
    BIOSGraphOff();             /* assume the graphic screen is initially off*/
    BIOSSetCurBlinkOn();        /* assume the cursor blinks initially */
    BIOSSetCursorOn();          /* assume the cursor is initially on */
}
