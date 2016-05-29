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


#include "imgedit.h"
#include <stdarg.h>
#include "ieclrpal.h"
#include "ieprofil.h"

static char     className[] = "Watcom Imgedit";

WPI_MRESULT CALLBACK ClientProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 mp1, WPI_PARAM2 mp2 );

/*
 * imgEditInit - initialization
 */
static BOOL imgEditInit( HAB hab )
{
    ULONG       flags;
    BOOL        maximized;
    HWND        frame;
    char        clientclass[] = { "IMGEDClientClass" };
    HWND        hwnd;
    WPI_RECT    rect;
    HMENU       hmenu;

    // Change this to use the querycaps routine
    ColourPlanes = 1;
    BitsPerPixel = 4;

    Instance.hab = hab;
    Instance.mod_handle = NULL;
    /*
     * set up window class
     */
    if (!WinRegisterClass( hab,
                           className,
                           (PFNWP)ImgEdFrameProc,
                           CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           clientclass,
                           (PFNWP)ClientProc,
                           CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           PaletteClass,
                           (PFNWP)ColourPalWinProc,
                           CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           CURRENT_CLASS,
                           (PFNWP)CurrentWndProc,
                           CS_SIZEREDRAW | CS_SYNCPAINT,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           AVAIL_CLASS,
                           (PFNWP)ColoursWndProc,
                           CS_SIZEREDRAW | CS_SYNCPAINT,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           SCREEN_CLASS,
                           (PFNWP)ScreenWndProc,
                           CS_SIZEREDRAW,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           DrawAreaClassB,
                           (PFNWP)DrawAreaWinProc,
                           CS_SIZEREDRAW | CS_SYNCPAINT | CS_CLIPSIBLINGS,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           DrawAreaClassI,
                           (PFNWP)DrawAreaWinProc,
                           CS_SIZEREDRAW | CS_SYNCPAINT | CS_CLIPSIBLINGS,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           DrawAreaClassC,
                           (PFNWP)DrawAreaWinProc,
                           CS_SIZEREDRAW | CS_SYNCPAINT | CS_CLIPSIBLINGS,
                           0 )) {
        return( FALSE );
    }
    if (!WinRegisterClass( hab,
                           ViewWinClass,
                           (PFNWP)ViewWindowProc,
                           CS_MOVENOTIFY | CS_SIZEREDRAW | CS_CLIPCHILDREN,
                           0 )) {
        return( FALSE );
    }

    LoadImgedConfig();
    maximized = ImgedConfigInfo.ismaximized;
    InitPalette();
    /*
     * now make the main window
     */
    flags = FCF_TITLEBAR        |
            FCF_SIZEBORDER      |
            FCF_MINMAX          |
            FCF_SYSMENU         |
            FCF_TASKLIST        |
            FCF_ICON            |
            FCF_MENU;

    frame = WinCreateStdWindow(
        HWND_DESKTOP,
        0L,
        &flags,
        className,
        ImgEdName,
        0L,
        (HMODULE)0,
        IMGED_MAIN,
        &HMainWindow);

    WinSetWindowPos(frame, HWND_TOP, ImgedConfigInfo.x_pos,
                ImgedConfigInfo.y_pos, ImgedConfigInfo.width,
                ImgedConfigInfo.height, SWP_MOVE | SWP_SHOW | SWP_SIZE);

    _wpi_getclientrect( HMainWindow, &rect );
    flags = 0L;
    /*
     * N.B. - NOTE that ClientWindow is actually a FRAME WINDOW!!!!!  The
     *        name is a bit misleading and it is unlike HMainWindow which
     *        is a client window.
     */
    ClientWindow = WinCreateStdWindow( HMainWindow, 0L, &flags, clientclass,
                                        "", 0L, (HMODULE)0, 0, &hwnd);

    WinSetWindowPos(ClientWindow, HWND_TOP, 0, STATUS_WIDTH,
                _wpi_getwidthrect( rect ),
                _wpi_getheightrect(rect) -STATUS_WIDTH -FUNCTIONBAR_WIDTH -1,
                SWP_MOVE | SWP_SHOW | SWP_SIZE);

    CreateColourPal();
    InitFunctionBar( HMainWindow );
    InitTools( _wpi_getframe(HMainWindow) );

    hmenu = _wpi_getmenu( frame );
    if (ImgedConfigInfo.grid_on) {
        CheckGridItem(hmenu);
    }
    SetHintText("Open Watcom Image Editor.");
    WinSetFocus( HWND_DESKTOP, _wpi_getframe(HMainWindow) );
    return( TRUE );
} /* imgEditInit */

/*
 * parseCmdLine - parses the command line to see if there is an image to
 *                open right away.
 */
static void parseCmdLine( int count, char **cmdline )
{
    char        fname[ _MAX_PATH ];
    int         i;

    for( i=1; i < count; ++i ) {
        strcpy( fname, cmdline[i] );
        OpenFileOnStart( fname );
    }
} /* parseCmdLine */

/*
 * imgeditFini - finishing details of image editor
 */
static void imgeditFini( HMQ hmq )
{
    SaveImgedConfig();
    WinDestroyMsgQueue( hmq );
    WinTerminate( Instance.hab );
} /* imgeditFini */

/*
 * main - main entry point for PM
 */
int main( int argc, char *argv[] )
{
    HMQ         hmq;
    QMSG        qmsg;
    HAB         hab;

    hab = WinInitialize( 0 );
    if(!hab) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }

    hmq = WinCreateMsgQueue( hab, 0 );
    if(!hmq) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        WinTerminate(hab);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }

    if( !imgEditInit( hab ) ) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        WinTerminate(hab);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }

    IEEnableMenuInput( TRUE );

    if( argc > 1 ) {
        parseCmdLine( argc, argv );
    }

    while( WinGetMsg( hab, &qmsg, 0, 0, 0 ) ) {
        WinDispatchMsg( hab, &qmsg );
    }

    imgeditFini( hmq );

    return 0;
} /* main */

