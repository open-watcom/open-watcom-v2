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


//
// EPMTEST      : test interface DLL for communicating to EPM
//

#define INCL_WINDDE
#define INCL_WINWINDOWMGR
#define INCL_WININPUT
#include <os2.h>

#include "..\wedit.h"

static char Err1[] = { "fshapes.for: *ERR* SP-24 in FMAIN, structure QMSG has not been defined" };
static char Err2[] = { "fshapes.for(44): *ERR* MD-03 column 51, operator not expecting REAL operands" };
static char Err3[] = { "fshapes.for(51): *ERR* SX-16 column 15, expecting LOGICAL expression" };

static int      CurrLocation = { 0 };
static int      NumLocations = { 3 };
static Row[] = { 3, 7, 10 };
static Col[] = { 12, 3, 7 };
static Len[] = { 5, 2, 3 };
static ResourceId[] = { 28728, 26659, 30000 };
static Errors[] = { &Err1, &Err2, &Err3 };


MRESULT EXPENTRY MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
//============================================================================

    HPS         ps;
    RECTL       rcl;

    switch( msg ) {
    case WM_CREATE:
        EDITConnect();
        EDITFile( "d:\\editdll\\epm\\epmlink.c", "wfcerrs.hlp" );
        EDITLocate( Row[CurrLocation], Col[CurrLocation], Len[CurrLocation] );
        ++CurrLocation;
        break;
    case WM_PAINT:
        ps = WinBeginPaint( hwnd, NULL, NULL );
        WinQueryWindowRect( hwnd, &rcl );
        WinFillRect( ps, &rcl, CLR_WHITE );
        WinEndPaint( ps );
        return( 0 );
    case WM_BUTTON1DOWN:
        if( CurrLocation < NumLocations ) {
            if( EDITLocateError( Row[CurrLocation], Col[CurrLocation],
                                 Len[CurrLocation], ResourceId[CurrLocation],
                                 Errors[CurrLocation] ) ) {
                ++CurrLocation;
            }
        }
        break;
    case WM_BUTTON2DOWN:
        EDITConnect();
        EDITFile( "d:\\editdll\\epm\\epmlink.c", "wfcerrs.hlp" );
        EDITLocate( Row[CurrLocation], Col[CurrLocation], Len[CurrLocation] );
        CurrLocation = 1;
        break;
    case WM_CLOSE:
        EDITDisconnect();
        break;
    }
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}


int     main() {
/**************/

    ULONG       style;
    QMSG        qmsg;
    HWND        FrameHandle;
    HWND        WinHandle;
    HMQ         hMessageQueue;
    HAB         AnchorBlock;

    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );
    if( !WinRegisterClass( AnchorBlock, "WATCOM", (PFNWP)MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }
    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &style,
                                      "WATCOM", "", 0, NULL, 0,
                                      &WinHandle );
    if( FrameHandle == 0 ) return( 0 );

    while( WinGetMsg( AnchorBlock, &qmsg, NULL, 0, 0 ) ) {
        WinDispatchMsg( AnchorBlock, &qmsg );
    }

    WinDestroyWindow( FrameHandle );
    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}
