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


#include <stdlib.h>
#include <stdio.h>
#include <process.h>

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "sampinfo.h"
#include "msg.h"
#include "pathlist.h"
#ifdef TRMEM
#include "trmemcvr.h"
#endif

//#include "utils.def"
//#include "aboutmsg.def"
//#include "getsamps.def"
//#include "clrsamps.def"
//#include "wpstart.def"
//#include "msg.def"
//#include "wpwind.def"
//#include "wpsamp.def"
//#include "dlgsamp.def"
//#include "dlgoptn.def"
//#include "wpcnvt.def"
//#include "wphelp.def"
extern void WPProcHelp(gui_help_actions action);
extern void WPFini(void);
extern void AboutClose(void);
extern void DlgAbout(void);
extern void DlgOpenSample(void);
extern void ClearSample(sio_data *curr_sio);
extern void DlgGetOptions(a_window *wnd);
extern void WPConvert(a_window *wnd,int convert_select);
extern bint GetSampleInfo(void);
extern void WPSampleOpen(void);
extern void AboutSetOff(void);
extern void ErrorMsg(char *msg,... );

extern char         SamplePath[_MAX_PATH];
extern sio_data *   CurrSIOData;


#define OFN_SAMPLE_FILE (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST)


static gui_menu_struct fileMenu[] = {
    { "&Open...", MENU_OPEN_SAMPLE, GUI_ENABLED, "Open a sample file" },
    { "&Close", MENU_CLOSE_SAMPLE, GUI_ENABLED,
                            "Close the current sample information" },
    { "", 0, GUI_SEPARATOR },
    { "Op&tions...", MENU_OPTIONS, GUI_ENABLED, "Set Profiler Options" },
#if _OS != _OS_WIN && _OS != _OS_NT && !defined(_OS2_PM)
    { "S&ystem", MENU_SYSTEM, GUI_ENABLED, "Start an operating system shell" },
#endif
#ifdef TRMEM
    { "&Memory Usage(Debug)", MENU_MEMPRT, GUI_ENABLED, "Print out current memory usage" },
#endif
    { "", 0, GUI_SEPARATOR },
    { "E&xit", MENU_EXIT, GUI_ENABLED, "Exit the profiler" },
};


static gui_menu_struct convertMenu[] = {
    { "Current &Module...", MENU_CONVERT_MODULE, GUI_ENABLED,
                            "Convert the current module" },
    { "Current &Image...", MENU_CONVERT_IMAGE, GUI_ENABLED,
                            "Convert the current image" },
    { "&All Images...", MENU_CONVERT_ALL, GUI_ENABLED,
                            "Convert all of the images" },
};


static gui_menu_struct helpMenu[] = {
    { "&Contents", MENU_HELP_CONTENTS, GUI_ENABLED,
                            "Show help contents" },
#if _OS == _OS_WIN || _OS == _OS_NT || defined(_OS2_PM)
    { "&On Help", MENU_HELP_ONHELP, GUI_ENABLED,
                            "Show Help about Help" },
    { "&Search", MENU_HELP_SEARCH, GUI_ENABLED,
                            "Search help for a topic" },
#endif
    { "", 0, GUI_SEPARATOR },
    { "&About", MENU_ABOUT, GUI_ENABLED, "Display program information" },
};


gui_menu_struct WndMainMenu[] = {
    { "&File",  MENU_FILE, GUI_ENABLED,
      "", WndMenuFields( fileMenu ) },
    { "&Convert",  MENU_CONVERT, GUI_ENABLED,
      "Convert the data to an output format", WndMenuFields( convertMenu ) },
    { "&Windows", MENU_WINDOWS, GUI_ENABLED+GUI_MDIWINDOW,
      "Select an active window" },
    { "&Actions",  MENU_ACTIONS, GUI_ENABLED+WND_MENU_POPUP,
      "Select an action for the active window", NULL, 0 },
    { "&Help",  MENU_HELP, GUI_ENABLED,
      "Display profiler help", WndMenuFields( helpMenu ) },
};

int WndNumMenus = { WndMenuSize( WndMainMenu ) };



extern bool WndMainMenuProc( a_window * wnd, unsigned id )
/********************************************************/
{
    a_window *      active;
    sio_data *      curr_sio;
#if _OS != _OS_WIN && _OS != _OS_NT
    char *          sys_spec;
#endif

    wnd=wnd;
    active = WndFindActive();
    switch( id ) {
    case MENU_OPEN_SAMPLE:
        DlgOpenSample();
        break;
    case MENU_CLOSE_SAMPLE:
        curr_sio = WndExtra( active );
        if( curr_sio != NULL ) {
            ClearSample( curr_sio );
            WndClose( active );
        }
        break;
#if _OS != _OS_WIN && _OS != _OS_NT
    case MENU_SYSTEM:
        GUISpawnStart();
        sys_spec = getenv( "COMSPEC" );
        if( sys_spec == NULL
         || spawnl( P_WAIT, sys_spec, sys_spec, NULL ) == -1 ) {
            ErrorMsg( LIT( Bad_System_Load ) );
        }
        GUISpawnEnd();
        break;
#endif
#ifdef TRMEM
    case MENU_MEMPRT:
        GUISpawnStart();
        TRMemPrtUsage();
        getchar();
        GUISpawnEnd();
        break;
#endif
    case MENU_OPTIONS:
        DlgGetOptions( active );
        break;
    case MENU_HELP_CONTENTS:
        WPProcHelp( GUI_HELP_CONTENTS );
        break;
#if _OS == _OS_WIN || _OS == _OS_NT || defined(_OS2_PM)
    case MENU_HELP_ONHELP:
        WPProcHelp( GUI_HELP_ON_HELP );
        break;
    case MENU_HELP_SEARCH:
        WPProcHelp( GUI_HELP_SEARCH );
        break;
#endif
    case MENU_ABOUT:
        DlgAbout();
        break;
    case MENU_EXIT:
        WPFini();
        break;
    case MENU_CONVERT_MODULE:
    case MENU_CONVERT_IMAGE:
    case MENU_CONVERT_ALL:
        WPConvert( active, id );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}



extern void OpenSample()
/**********************/
{
    void *      cursor_type;

#if _OS == _OS_WIN || _OS == _OS_NT || defined(_OS2_PM)
    AboutClose();
#else
    AboutSetOff();
#endif
    cursor_type = WndHourGlass( NULL );
    if( GetSampleInfo() ) {
        WPSampleOpen();
    }
    WndHourGlass( cursor_type );
}
