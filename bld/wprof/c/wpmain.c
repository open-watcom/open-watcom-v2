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
* Description:  Execution profiler mainline.
*
****************************************************************************/


#include <string.h>
#include "wio.h"
#include "common.h"
#if defined( __WINDOWS__ ) || defined( __NT__ )
#include <windows.h>
#endif
#include "aui.h"
#include "wpaui.h"
#if defined( __OS2_PM__ )
#include "rcdef.h"
#endif

extern void WPInitHelp(void);
extern void WPInit(void);
extern void AboutOpen(void);
extern void DlgOpenSample(void);
extern bool WPSampFound(void);
extern void OpenSample(void);
extern void WPFindDoPopUp(a_window *wnd,int id);
extern void WPDoPopUp(a_window *wnd,gui_menu_struct *gui_menu);
extern void ReportSampleInfo(void);



extern gui_colour_set   WndColours[];
extern int              WndNumColours;
extern a_window *       WndMain;
extern char             SamplePath[];

#if defined( __OS2_PM__ )
STATIC gui_resource     MainIcon = { WPROF_ICON, NULL };
#endif

extern void GUImain( void )
/*************************/
{
    WPInit();
    WndCreateStatusWindow( &WndColours[ WPA_STATUS_LINE ] );
    WndStatusText( "" );
#if defined( __OS2_PM__ )
    WndSetIcon( WndMain, &MainIcon );
#endif
    WPInitHelp();
    AboutOpen();
    WndShowAll();
    WndShowWndMain();
    if( !WPSampFound() ) {
        DlgOpenSample();
    } else {
        OpenSample();
    }
}


extern bool WndProcMacro( a_window * wnd, unsigned key )
/******************************************************/
{
#if 0
    int     menu;

    menu=menu;
#endif
    switch( key ) {
    case GUI_KEY_F3:
        WPFindDoPopUp( wnd, MENU_SAMP_ZOOM_IN );
        return( true );
    case GUI_KEY_F4:
    case GUI_KEY_BACKSPACE:
        WPFindDoPopUp( wnd, MENU_SAMP_BACK_OUT );
        return( true );
    case GUI_KEY_PERIOD:
        WPDoPopUp( wnd, NULL );
        return( true );
#ifndef NDEBUG
    case GUI_KEY_CTRL_R:
        ReportSampleInfo();
        return( true );
#endif
    default:
        return( false );
    }
    /* for codes not handled by the pop-up */
#if 0
    WndMainMenuProc( wnd, menu );
    return( true );
#endif
}
