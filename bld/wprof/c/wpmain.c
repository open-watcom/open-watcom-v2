/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "common.h"
#if defined( __WINDOWS__ ) || defined( __NT__ )
#include <windows.h>
#endif
#include "aui.h"
#include "wpaui.h"
#if defined( __OS2_PM__ )
#include "rcdef.rh"
#endif
#include "dlgsamp.h"
#include "wpdriver.h"
#include "wphelp.h"
#include "wpsamp.h"
#include "aboutmsg.h"
#include "rptsamps.h"
#include "wpstart.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpdata.h"


#if defined( __OS2_PM__ )
STATIC gui_resource     MainIcon = { WPROF_ICON, NULL };
#endif

void GUImain( void )
/******************/
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


bool WndProcMacro( a_window wnd, gui_key key )
/********************************************/
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
