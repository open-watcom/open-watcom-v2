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


#include <malloc.h>
#include "imgedit.h"
#include "ieprofil.h"
#include "ietoolbr.h"
#include "ieclrpal.h"
#include "watini.h"

extern int TBWidth;
extern int TBHeight;

static char imgedSection[] = "WATCOM Image Editor";
static char dfltInitFile[] = WATCOM_INI;

/*
 * setDefaultValues - sets the default values of ImgedConfigInfo structure.
 */
static void setDefaultValues( settings_info *info )
{
    int         x,y;

    /* save settings */
    info->settings = SET_SAVE_ALL;

    x = _wpi_getsystemmetrics( SM_CXSCREEN );
    y = _wpi_getsystemmetrics( SM_CYSCREEN );

    /* set defaults */
    ImgedConfigInfo.x_pos = 0;
    ImgedConfigInfo.width = x;
#ifdef __OS2_PM__
    ImgedConfigInfo.y_pos = y / 5;
    ImgedConfigInfo.height = 4 * ( y / 5 );
#else
    ImgedConfigInfo.y_pos = 0;
    ImgedConfigInfo.height = 4 * ( y / 5 );
#endif
    ImgedConfigInfo.ismaximized = FALSE;

    ImgedConfigInfo.tool_xpos = ImgedConfigInfo.x_pos +
                                        ImgedConfigInfo.width - TBWidth - 2;
    ImgedConfigInfo.tool_ypos = ImgedConfigInfo.y_pos +
                            _wpi_getsystemmetrics(SM_CYCAPTION) +
                            _wpi_getsystemmetrics(SM_CYMENU ) +
                            FUNCTIONBAR_WIDTH + 2;

    ImgedConfigInfo.pal_xpos = ImgedConfigInfo.x_pos +
                                        ImgedConfigInfo.width - CP_WIDTH - 2;
#ifdef __OS2_PM__
    ImgedConfigInfo.pal_ypos = ImgedConfigInfo.y_pos + STATUS_WIDTH;
#else
    ImgedConfigInfo.pal_ypos = ImgedConfigInfo.y_pos +
                                        ImgedConfigInfo.height -
                                        STATUS_WIDTH - CP_HEIGHT - 2;
#endif

    ImgedConfigInfo.view_xpos = VIEWIN_XPOS;
    ImgedConfigInfo.view_ypos = ImgedConfigInfo.y_pos +
                            _wpi_getsystemmetrics(SM_CYCAPTION) +
                            _wpi_getsystemmetrics(SM_CYMENU ) +
                            FUNCTIONBAR_WIDTH + 2;

    /* other settings */
    ImgedConfigInfo.show_state = SET_SHOW_TOOL | SET_SHOW_CLR | SET_SHOW_VIEW;
    info->paste = SET_PASTE_STR;
    info->rotate =  10 + SET_ROT_SIMPLE;        // 10 means keep area
    info->viewwnd = SET_VIEW_1;
    info->wrapshift = TRUE;
    ImgedConfigInfo.shift = 1;
    ImgedConfigInfo.square_grid = TRUE;
    ImgedConfigInfo.brush_size = 2;
    ImgedConfigInfo.grid_on = TRUE;

    strcpy( info->opendir, "" );
    strcpy( info->savedir, "" );
    sprintf( info->colour, "%ld", WHITE );
} /* setDefaultValues */

/*
 * loadPositionValues - loads the position values from the ini file.
 */
static void loadPositionValues( HINI hini )
{
    int         sysbox_width;
    int         x,y;

    x = _wpi_getsystemmetrics( SM_CXSCREEN );
    y = _wpi_getsystemmetrics( SM_CYSCREEN );
    sysbox_width = _wpi_getsystemmetrics( SM_CYCAPTION );

    /* position of main window */
    ImgedConfigInfo.x_pos = _wpi_getprivateprofileint( hini, imgedSection,
                        "wnd_xpos", ImgedConfigInfo.x_pos, dfltInitFile );

    ImgedConfigInfo.y_pos = _wpi_getprivateprofileint( hini, imgedSection,
                        "wnd_ypos", ImgedConfigInfo.y_pos, dfltInitFile );
    ImgedConfigInfo.width = _wpi_getprivateprofileint( hini, imgedSection,
                        "wnd_width", ImgedConfigInfo.width, dfltInitFile );
    ImgedConfigInfo.height = _wpi_getprivateprofileint( hini, imgedSection,
                        "wnd_height", ImgedConfigInfo.height, dfltInitFile );
    ImgedConfigInfo.ismaximized = _wpi_getprivateprofileint( hini,
        imgedSection, "ismaximized", ImgedConfigInfo.ismaximized, dfltInitFile );

    /* location of the tool window */
    ImgedConfigInfo.tool_xpos = _wpi_getprivateprofileint( hini, imgedSection,
                    "tool_xpos", ImgedConfigInfo.tool_xpos, dfltInitFile );
    ImgedConfigInfo.tool_ypos = _wpi_getprivateprofileint( hini, imgedSection,
                    "tool_ypos", ImgedConfigInfo.tool_ypos, dfltInitFile );

    /* location of the colour palette */
    ImgedConfigInfo.pal_xpos = _wpi_getprivateprofileint( hini, imgedSection,
                        "pal_xpos", ImgedConfigInfo.pal_xpos, dfltInitFile );
    ImgedConfigInfo.pal_ypos = _wpi_getprivateprofileint( hini, imgedSection,
                        "pal_ypos", ImgedConfigInfo.pal_ypos, dfltInitFile );

    /* location of the view window */
    ImgedConfigInfo.view_xpos = _wpi_getprivateprofileint( hini, imgedSection,
                "view_xpos", ImgedConfigInfo.view_xpos, dfltInitFile );
    ImgedConfigInfo.view_ypos = _wpi_getprivateprofileint( hini, imgedSection,
                "view_ypos", ImgedConfigInfo.view_ypos, dfltInitFile );
    /*
     * now we want to make sure nothing is off the screen!
     */
    if (ImgedConfigInfo.x_pos + ImgedConfigInfo.width > x) {
        ImgedConfigInfo.width = x - ImgedConfigInfo.x_pos - 2;
    }
    if (ImgedConfigInfo.y_pos + ImgedConfigInfo.height > y) {
        ImgedConfigInfo.height = y - ImgedConfigInfo.y_pos - 2;
    }
    if (ImgedConfigInfo.tool_xpos > x - sysbox_width - 2) {
        ImgedConfigInfo.tool_xpos = x - TBWidth - 1;
    }
    if (ImgedConfigInfo.tool_ypos > y - sysbox_width - 2) {
        ImgedConfigInfo.tool_ypos = y - TBHeight - 1;
    }
    if (ImgedConfigInfo.pal_xpos > x - sysbox_width - 2) {
        ImgedConfigInfo.pal_xpos = x - CP_WIDTH - 1;
    }
    if (ImgedConfigInfo.pal_ypos > y - sysbox_width - 2) {
        ImgedConfigInfo.pal_ypos = y - CP_HEIGHT - 1;
    }
    if (ImgedConfigInfo.view_xpos > x - sysbox_width - 2) {
        ImgedConfigInfo.view_xpos = x - sysbox_width - 5;
    }
    if (ImgedConfigInfo.pal_ypos > y - sysbox_width - 2) {
        ImgedConfigInfo.pal_ypos = y - sysbox_width - 5;
    }
    hini = hini;
} /* loadPositionValues */

/*
 * loadSettings - loads the settings for the image editor (not including
 *                position.
 */
static void loadSettings( settings_info *info, HINI hini )
{
    ImgedConfigInfo.show_state = _wpi_getprivateprofileint( hini,
        imgedSection, "show_state", ImgedConfigInfo.show_state, dfltInitFile);

    info->paste = _wpi_getprivateprofileint( hini, imgedSection, "paste",
                                                info->paste, dfltInitFile );
    info->rotate = _wpi_getprivateprofileint( hini, imgedSection, "rotate",
                                info->rotate, dfltInitFile );
    info->viewwnd = _wpi_getprivateprofileint( hini, imgedSection,
                                "view_window", info->viewwnd, dfltInitFile );
    info->wrapshift = _wpi_getprivateprofileint( hini, imgedSection,
                                "wrapshift", info->wrapshift, dfltInitFile );
    _wpi_getprivateprofilestring( hini, imgedSection, "open_dir",
                    info->opendir, info->opendir, _MAX_PATH, dfltInitFile );
    _wpi_getprivateprofilestring( hini, imgedSection, "save_dir",
                    info->savedir, info->savedir, _MAX_PATH, dfltInitFile );
    _wpi_getprivateprofilestring( hini, imgedSection, "screen_colour",
                    info->colour, info->colour, 10, dfltInitFile );
    ImgedConfigInfo.shift = _wpi_getprivateprofileint( hini, imgedSection,
                        "shift_amt", ImgedConfigInfo.shift, dfltInitFile );
    ImgedConfigInfo.square_grid = _wpi_getprivateprofileint( hini,
        imgedSection,"square_grid", ImgedConfigInfo.square_grid, dfltInitFile );
    ImgedConfigInfo.brush_size = _wpi_getprivateprofileint( hini,
        imgedSection, "brush_size", ImgedConfigInfo.brush_size, dfltInitFile);
    ImgedConfigInfo.grid_on = _wpi_getprivateprofileint( hini, imgedSection,
                        "grid_on", ImgedConfigInfo.grid_on, dfltInitFile );
    hini = hini;
} /* loadSettings */

/*
 * LoadImgedConfig - get configuration from a profile file
 */
void LoadImgedConfig( void )
{
    settings_info       info;
    HINI                handle_inifile;

    setDefaultValues( &info );

    /*
     * Image editor will ALWAYS maintain this setting ... which determines
     * whether or not all other settings will be saved.  Note that the
     * PM version opens the ini file and the win version stores the info
     * in some kind of cache
     */
    handle_inifile = _wpi_openinifile( Instance, dfltInitFile );
    info.settings = _wpi_getprivateprofileint( handle_inifile, imgedSection,
                            "profile", info.settings, dfltInitFile );

    if (info.settings & SET_SAVE_POS) {
        loadPositionValues( handle_inifile );
    }

    if (info.settings & SET_SAVE_SET) {
        loadSettings( &info, handle_inifile );
    }
    _wpi_closeinifile( handle_inifile );
    SetSettingsDlg( &info );
    SetInitialOpenDir( info.opendir );
    SetInitialSaveDir( info.savedir );
    SetInitScreenColour( atol(info.colour) );

} /* LoadImgedConfig */

/*
 * savePositionValues - saves the window position values.
 */
static void savePositionValues( HINI hini )
{
    char        buf[10];

    itoa( ImgedConfigInfo.ismaximized, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "ismaximized", buf, dfltInitFile );
    if ( !ImgedConfigInfo.ismaximized ) {
        itoa( ImgedConfigInfo.x_pos, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_xpos", buf, dfltInitFile );
        itoa( ImgedConfigInfo.y_pos, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_ypos", buf, dfltInitFile );
        itoa( ImgedConfigInfo.width, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_width", buf, dfltInitFile );
        itoa( ImgedConfigInfo.height, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_height", buf, dfltInitFile );
    }

    /* tool window position */
    itoa( ImgedConfigInfo.tool_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "tool_xpos", buf, dfltInitFile );
    itoa( ImgedConfigInfo.tool_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "tool_ypos", buf, dfltInitFile );

    /* colour palette window position */
    itoa( ImgedConfigInfo.pal_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "pal_xpos", buf, dfltInitFile );
    itoa( ImgedConfigInfo.pal_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "pal_ypos", buf, dfltInitFile );

    /* view window position */
    itoa( ImgedConfigInfo.view_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_xpos", buf, dfltInitFile );
    itoa( ImgedConfigInfo.view_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_ypos", buf, dfltInitFile );
    hini = hini;
} /* savePositionValues */

/*
 * saveSettings - saves the settings of the current session (not including
 *                position).
 */
static void saveSettings( settings_info *info, HINI hini )
{
    char        buf[10];

    itoa( info->paste, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "paste", buf, dfltInitFile );
    itoa( info->rotate, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "rotate", buf, dfltInitFile );
    itoa( info->viewwnd, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_window", buf, dfltInitFile );
    itoa( ImgedConfigInfo.square_grid, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "square_grid", buf, dfltInitFile );
    itoa( ImgedConfigInfo.shift, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "shift_amt", buf, dfltInitFile );
    itoa( ImgedConfigInfo.brush_size, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "brush_size", buf, dfltInitFile );
    itoa( info->wrapshift, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "wrapshift", buf, dfltInitFile );
    itoa( ImgedConfigInfo.grid_on, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "grid_on", buf, dfltInitFile );
    _wpi_writeprivateprofilestring( hini, imgedSection, "screen_colour",
                                                info->colour, dfltInitFile );
    _wpi_writeprivateprofilestring( hini, imgedSection, "open_dir",
                                                info->opendir, dfltInitFile );
    _wpi_writeprivateprofilestring( hini, imgedSection, "save_dir",
                                                info->savedir, dfltInitFile );

    /*
     * save the show state of the tool window, colour palette, view window.
     */
    itoa( ImgedConfigInfo.show_state, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "show_state", buf, dfltInitFile );
    hini = hini;
} /* saveSettings */

/*
 * SaveImgedConfig - write configuration to a profile file
 */
void SaveImgedConfig( void )
{
    settings_info       info;
    char                buf[10];
    HINI                handle_inifile;

    GetSettings( &info );
    sprintf(info.colour, "%ld", GetBkColour());
    strcpy( info.opendir, GetInitOpenDir() );
    strcpy( info.savedir, GetInitSaveDir() );

    handle_inifile = _wpi_openinifile( Instance, dfltInitFile );
    itoa( info.settings, buf, 10 );
    _wpi_writeprivateprofilestring( handle_inifile, imgedSection, "profile",
                                                        buf, dfltInitFile);
    if (info.settings & SET_SAVE_POS) {
        savePositionValues( handle_inifile );
    }

    if (info.settings & SET_SAVE_SET) {
        saveSettings( &info, handle_inifile );
    }
    _wpi_closeinifile( handle_inifile );
} /* SaveImgedConfig */

