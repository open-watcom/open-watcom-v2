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


#include "guiwind.h"
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#ifndef __OS2_PM__
    #include "wwinhelp.h"
#endif

extern  WPI_INST        GUIMainHInst;

#ifdef __OS2_PM__

static gui_help_instance InitHelp( HWND hwnd, WPI_INST inst, char *title, char *help_lib )
{
    HWND        hwndHelpInstance;
    HELPINIT    help;

    help.cb = sizeof( HELPINIT );
    help.pszTutorialName = NULL;
    help.phtHelpTable = NULL;
    help.hmodHelpTableModule = 0;
    help.hmodAccelActionBarModule = 0;
    help.idAccelTable = 0;
    help.idActionBar = 0;
    help.pszHelpWindowTitle = title;
#ifdef __FLAT__
    help.fShowPanelId = CMIC_HIDE_PANEL_ID;
#else
    help.usShowPanelId = CMIC_HIDE_PANEL_ID;
#endif
    help.pszHelpLibraryName = help_lib;
    hwndHelpInstance = WinCreateHelpInstance( inst.hab, &help );
    if( hwndHelpInstance != (HWND)NULL ) {
        if( !WinAssociateHelpInstance( hwndHelpInstance, hwnd ) ) {
            WinDestroyHelpInstance( hwndHelpInstance );
            hwndHelpInstance = NULLHANDLE;
        }
    }

    return( (gui_help_instance)hwndHelpInstance );
}

static void FiniHelp( gui_help_instance inst, HWND hwnd, char *file )
{
    hwnd=hwnd;
    file=file;
    if( (HWND)inst != (HWND)NULL ) {
        WinAssociateHelpInstance( (HWND)inst, NULLHANDLE );
        WinDestroyHelpInstance( (HWND)inst );
    }
}

bool DisplayContents( gui_help_instance inst, HWND hwnd, char *file )
{
    hwnd=hwnd;
    file=file;
    return( !WinSendMsg( (HWND)inst, HM_HELP_CONTENTS, NULL, NULL ) );
}

bool DisplayHelpOnHelp( gui_help_instance inst, HWND hwnd, char *file )
{
    hwnd=hwnd;
    file=file;
    return( !WinSendMsg( (HWND)inst, HM_DISPLAY_HELP, NULL, NULL ) );
}

bool DisplayHelpSearch( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    hwnd=hwnd;
    file=file;
    topic=topic;
    return( !WinSendMsg( (HWND)inst, HM_HELP_INDEX, NULL, NULL ) );
}

bool DisplayHelpKey( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    hwnd=hwnd;
    file=file;
    return( !WinSendMsg( (HWND)inst, HM_DISPLAY_HELP,
                         MPFROMLONG( (LONG)&topic ),
                         MPFROMSHORT( HM_PANELNAME ) ) );
}

bool DisplayHelpContext( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    hwnd=hwnd;
    file=file;
    return( !WinSendMsg( (HWND)inst, HM_DISPLAY_HELP,
                         MPFROM2SHORT( (SHORT)topic, 0 ),
                         MPFROMSHORT( HM_RESOURCEID ) ) );
}

#else

static gui_help_instance InitHelp( HWND hwnd, WPI_INST inst, char *title, char *help_file )
{
    hwnd = hwnd;
    inst = inst;
    title = title;
    help_file = help_file;
    return( (gui_help_instance)help_file );
}

static void FiniHelp( gui_help_instance inst, HWND hwnd, char *file )
{
    inst=inst;
    WWinHelp( hwnd, file, (UINT)HELP_QUIT, (DWORD)0 );
}

bool DisplayContents( gui_help_instance inst, HWND hwnd, char *file )
{
    inst=inst;
    return( WWinHelp( hwnd, file, (UINT)HELP_CONTENTS, 0 ) );
}

bool DisplayContentsHH( gui_help_instance inst, HWND hwnd, char *file )
{
    inst = inst;
    return( WHtmlHelp( hwnd, file, (UINT)HELP_CONTENTS, 0 ) );
}

bool DisplayHelpOnHelp( gui_help_instance inst, HWND hwnd, char *file )
{
    inst=inst;
    return( WWinHelp( hwnd, file, (UINT)HELP_HELPONHELP, 0 ) );
}

bool DisplayHelpSearch( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    inst=inst;
    if( topic == NULL ) {
        topic = "";
    }
    return( WWinHelp( hwnd, file, (UINT)HELP_PARTIALKEY, (DWORD)(LPCSTR)topic ) );
}

bool DisplayHelpSearchHH( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    inst = inst;
    if( topic == NULL ) {
        topic = "";
    }
    return( WHtmlHelp( hwnd, file, (UINT)HELP_PARTIALKEY, (DWORD)(LPCSTR)topic ) );
}

bool DisplayHelpContext( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    inst=inst;
    return( WWinHelp( hwnd, file, (UINT)HELP_CONTEXT, (DWORD)topic ) );
}

bool DisplayHelpKey( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    inst=inst;
    return( WWinHelp( hwnd, file, (UINT)HELP_KEY, (DWORD)topic ) );
}

bool DisplayHelpKeyHH( gui_help_instance inst, HWND hwnd, char *file, char *topic )
{
    inst = inst;
    return( WHtmlHelp( hwnd, file, (UINT)HELP_KEY, (DWORD)topic ) );
}

#endif


gui_help_instance GUIHelpInit( gui_window *wnd, char *file, char *title )
{
    return( InitHelp( wnd->hwnd, GUIMainHInst, title, file ) );
}

void GUIHelpFini( gui_help_instance inst, gui_window *wnd, char *file )
{
    FiniHelp( inst, wnd->hwnd, file );
}

bool GUIShowHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act,
                  char *file, char *topic )
{
    bool        ret;

    ret = FALSE;

    switch( act ) {
    case GUI_HELP_CONTENTS:
        ret = DisplayContents( inst, wnd->hwnd, file );
        break;
    case GUI_HELP_ON_HELP:
        ret = DisplayHelpOnHelp( inst, wnd->hwnd, file );
        break;
    case GUI_HELP_SEARCH:
        ret = DisplayHelpSearch( inst, wnd->hwnd, file, topic );
        break;
    case GUI_HELP_CONTEXT:
        ret = DisplayHelpContext( inst, wnd->hwnd, file, topic );
        break;
    case GUI_HELP_KEY:
        ret = DisplayHelpKey( inst, wnd->hwnd, file, topic );
        break;
    }

    return( ret );
}

bool GUIShowHtmlHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act,
                      char *file, char *topic )
{
    bool        ret;

    ret = FALSE;

#ifndef __OS2_PM__
    switch( act ) {
    case GUI_HELP_CONTENTS:
        ret = DisplayContentsHH( inst, wnd->hwnd, file );
        break;
    case GUI_HELP_SEARCH:
        ret = DisplayHelpSearchHH( inst, wnd->hwnd, file, topic );
        break;
    case GUI_HELP_KEY:
        ret = DisplayHelpKeyHH( inst, wnd->hwnd, file, topic );
        break;
    }
#endif

    return( ret );
}

bool GUIDisplayHelp( gui_window *wnd, char *file, char *topic )
{
#ifdef __OS2_PM__
    wnd = wnd;
    file = file;
    topic = topic;
    return( FALSE );
#else
    if( topic == NULL ) {
        return( WWinHelp( wnd->hwnd, file, (UINT)HELP_INDEX, (DWORD)NULL ) );
    } else {
        return( WWinHelp( wnd->hwnd, file, (UINT)HELP_KEY, (DWORD)topic ) );
    }
#endif
}

bool GUIDisplayHelpWin4( gui_window *wnd, char *file, char *topic )
{
#ifdef __OS2_PM__
    wnd = wnd;
    file = file;
    topic = topic;
    return( FALSE );
#else
    if( topic == NULL ) {
        #if defined( __NT__ )
            DWORD   version;

            version = GetVersion();
            version = 100 * LOBYTE(LOWORD(version)) + HIBYTE(LOWORD(version));
            if( version >= 351 ) {
                // NT 3.51 or higher
                return( WWinHelp( wnd->hwnd, file, (UINT)HELP_FINDER, (DWORD)NULL ) );
            }
        #endif
        return( WWinHelp( wnd->hwnd, file, (UINT)HELP_INDEX, (DWORD)NULL ) );
    } else {
        return( WWinHelp( wnd->hwnd, file, (UINT)HELP_KEY, (DWORD)topic ) );
    }
#endif
}

bool GUIDisplayHelpId( gui_window *wnd, char *file, int id )
{
#ifdef __OS2_PM__
    wnd = wnd;
    file = file;
    id = id;
    return( FALSE );
#else
    return( WWinHelp( wnd->hwnd, file, (UINT)HELP_CONTEXT, (DWORD)id ) );
#endif
}
