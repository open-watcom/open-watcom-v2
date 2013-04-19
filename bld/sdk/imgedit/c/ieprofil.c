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
#include <malloc.h>
#include "ieprofil.h"
#include "ietoolbr.h"
#include "ieclrpal.h"
#include "watini.h"
#include "inipath.h"

extern int TBWidth;
extern int TBHeight;

static char imgedSection[] = "WATCOM Image Editor";
static char iniPath[_MAX_PATH] = WATCOM_INI;

/*
 * setDefaultValues - set the default values of ImgedConfigInfo structure
 */
static void setDefaultValues( settings_info *info )
{
    int         x, y;

    /* save settings */
    info->settings = SET_SAVE_ALL;

    x = _wpi_getsystemmetrics( SM_CXSCREEN );
    y = _wpi_getsystemmetrics( SM_CYSCREEN );

    /* set defaults */
    ImgedConfigInfo.x_pos = 0;
    ImgedConfigInfo.width = x;
#ifdef __OS2_PM__
    ImgedConfigInfo.y_pos = y / 5;
    ImgedConfigInfo.height = 4 * (y / 5);
#else
    ImgedConfigInfo.y_pos = 0;
    ImgedConfigInfo.height = 4 * (y / 5);
#endif
    ImgedConfigInfo.ismaximized = FALSE;

    ImgedConfigInfo.tool_xpos = ImgedConfigInfo.x_pos + ImgedConfigInfo.width - TBWidth - 2;
    ImgedConfigInfo.tool_ypos = ImgedConfigInfo.y_pos +
        _wpi_getsystemmetrics( SM_CYCAPTION ) + _wpi_getsystemmetrics( SM_CYMENU ) +
        FUNCTIONBAR_WIDTH + 2;

    ImgedConfigInfo.pal_xpos = ImgedConfigInfo.x_pos + ImgedConfigInfo.width - CP_WIDTH - 2;
#ifdef __OS2_PM__
    ImgedConfigInfo.pal_ypos = ImgedConfigInfo.y_pos + STATUS_WIDTH;
#else
    ImgedConfigInfo.pal_ypos = ImgedConfigInfo.y_pos + ImgedConfigInfo.height -
        StatusWidth - CP_HEIGHT - 2;
#endif

    ImgedConfigInfo.view_xpos = VIEWIN_XPOS;
    ImgedConfigInfo.view_ypos = ImgedConfigInfo.y_pos +
        _wpi_getsystemmetrics( SM_CYCAPTION ) + _wpi_getsystemmetrics( SM_CYMENU ) +
        FUNCTIONBAR_WIDTH + 2;

    /* other settings */
    ImgedConfigInfo.show_state = SET_SHOW_TOOL | SET_SHOW_CLR | SET_SHOW_VIEW;
    info->paste = SET_PASTE_STR;
    info->rotate = 10 + SET_ROT_SIMPLE;        // 10 means keep area
    info->viewwnd = SET_VIEW_1;
    info->wrapshift = TRUE;
    ImgedConfigInfo.shift = 1;
    ImgedConfigInfo.square_grid = TRUE;
    ImgedConfigInfo.brush_size = 2;
    ImgedConfigInfo.grid_on = TRUE;

    strcpy( info->opendir, "" );
    strcpy( info->savedir, "" );
    sprintf( info->color, "%ld", WHITE );

} /* setDefaultValues */

/*
 * loadPositionValues - load the position values from the configuration file
 */
static void loadPositionValues( HINI hini )
{
    int         sysbox_width;
    int         x, y;

    x = _wpi_getsystemmetrics( SM_CXSCREEN );
    y = _wpi_getsystemmetrics( SM_CYSCREEN );
    sysbox_width = _wpi_getsystemmetrics( SM_CYCAPTION );

    /* position of main window */
    ImgedConfigInfo.x_pos = _wpi_getprivateprofileint( hini, imgedSection,
        "wnd_xpos", ImgedConfigInfo.x_pos, iniPath );
    ImgedConfigInfo.y_pos = _wpi_getprivateprofileint( hini, imgedSection,
        "wnd_ypos", ImgedConfigInfo.y_pos, iniPath );
    ImgedConfigInfo.width = _wpi_getprivateprofileint( hini, imgedSection,
        "wnd_width", ImgedConfigInfo.width, iniPath );
    ImgedConfigInfo.height = _wpi_getprivateprofileint( hini, imgedSection,
        "wnd_height", ImgedConfigInfo.height, iniPath );
    ImgedConfigInfo.ismaximized = _wpi_getprivateprofileint( hini, imgedSection,
        "ismaximized", ImgedConfigInfo.ismaximized, iniPath );

    /* location of the tool window */
    ImgedConfigInfo.tool_xpos = _wpi_getprivateprofileint( hini, imgedSection,
        "tool_xpos", ImgedConfigInfo.tool_xpos, iniPath );
    ImgedConfigInfo.tool_ypos = _wpi_getprivateprofileint( hini, imgedSection,
        "tool_ypos", ImgedConfigInfo.tool_ypos, iniPath );

    /* location of the color palette */
    ImgedConfigInfo.pal_xpos = _wpi_getprivateprofileint( hini, imgedSection,
        "pal_xpos", ImgedConfigInfo.pal_xpos, iniPath );
    ImgedConfigInfo.pal_ypos = _wpi_getprivateprofileint( hini, imgedSection,
        "pal_ypos", ImgedConfigInfo.pal_ypos, iniPath );

    /* location of the view window */
    ImgedConfigInfo.view_xpos = _wpi_getprivateprofileint( hini, imgedSection,
        "view_xpos", ImgedConfigInfo.view_xpos, iniPath );
    ImgedConfigInfo.view_ypos = _wpi_getprivateprofileint( hini, imgedSection,
        "view_ypos", ImgedConfigInfo.view_ypos, iniPath );
    /*
     * Now we want to make sure nothing is off the screen!
     */
    if( ImgedConfigInfo.x_pos + ImgedConfigInfo.width > x ) {
        ImgedConfigInfo.width = x - ImgedConfigInfo.x_pos - 2;
    }
    if( ImgedConfigInfo.y_pos + ImgedConfigInfo.height > y ) {
        ImgedConfigInfo.height = y - ImgedConfigInfo.y_pos - 2;
    }
    if( ImgedConfigInfo.tool_xpos > x - sysbox_width - 2 ) {
        ImgedConfigInfo.tool_xpos = x - TBWidth - 1;
    }
    if( ImgedConfigInfo.tool_ypos > y - sysbox_width - 2 ) {
        ImgedConfigInfo.tool_ypos = y - TBHeight - 1;
    }
    if( ImgedConfigInfo.pal_xpos > x - sysbox_width - 2 ) {
        ImgedConfigInfo.pal_xpos = x - CP_WIDTH - 1;
    }
    if( ImgedConfigInfo.pal_ypos > y - sysbox_width - 2 ) {
        ImgedConfigInfo.pal_ypos = y - CP_HEIGHT - 1;
    }
    if( ImgedConfigInfo.view_xpos > x - sysbox_width - 2 ) {
        ImgedConfigInfo.view_xpos = x - sysbox_width - 5;
    }
    if( ImgedConfigInfo.pal_ypos > y - sysbox_width - 2 ) {
        ImgedConfigInfo.pal_ypos = y - sysbox_width - 5;
    }

} /* loadPositionValues */

/*
 * loadSettings - loads the settings for the image editor (not including position)
 */
static void loadSettings( settings_info *info, HINI hini )
{
    ImgedConfigInfo.show_state = _wpi_getprivateprofileint( hini,
        imgedSection, "show_state", ImgedConfigInfo.show_state, iniPath );

    info->paste = _wpi_getprivateprofileint( hini, imgedSection,
        "paste", info->paste, iniPath );
    info->rotate = _wpi_getprivateprofileint( hini, imgedSection,
        "rotate", info->rotate, iniPath );
    info->viewwnd = _wpi_getprivateprofileint( hini, imgedSection,
        "view_window", info->viewwnd, iniPath );
    info->wrapshift = _wpi_getprivateprofileint( hini, imgedSection,
        "wrapshift", info->wrapshift, iniPath );
    _wpi_getprivateprofilestring( hini, imgedSection, "open_dir",
        info->opendir, info->opendir, _MAX_PATH, iniPath );
    _wpi_getprivateprofilestring( hini, imgedSection, "save_dir",
        info->savedir, info->savedir, _MAX_PATH, iniPath );
    _wpi_getprivateprofilestring( hini, imgedSection, "screen_color",
        info->color, info->color, 10, iniPath );
    ImgedConfigInfo.shift = _wpi_getprivateprofileint( hini, imgedSection,
        "shift_amt", ImgedConfigInfo.shift, iniPath );
    ImgedConfigInfo.square_grid = _wpi_getprivateprofileint( hini, imgedSection,
        "square_grid", ImgedConfigInfo.square_grid, iniPath );
    ImgedConfigInfo.brush_size = _wpi_getprivateprofileint( hini,
        imgedSection, "brush_size", ImgedConfigInfo.brush_size, iniPath);
    ImgedConfigInfo.grid_on = _wpi_getprivateprofileint( hini, imgedSection,
        "grid_on", ImgedConfigInfo.grid_on, iniPath );

} /* loadSettings */

/*
 * LoadImgedConfig - get configuration from a profile file
 */
void LoadImgedConfig( void )
{
    settings_info       info;
    HINI                handle_inifile;

#ifdef __NT__
    GetConfigFilePath( iniPath, sizeof( iniPath ) );
    strcat( iniPath, "\\" WATCOM_INI );
#endif    
    setDefaultValues( &info );

    /*
     * Image editor will ALWAYS maintain this setting ... which determines
     * whether or not all other settings will be saved.  Note that the
     * PM version opens the configuration file and the Windows version stores
     * the information in some kind of cache.
     */
    handle_inifile = _wpi_openinifile( Instance, iniPath );
    info.settings = _wpi_getprivateprofileint( handle_inifile, imgedSection,
                                               "profile", info.settings, iniPath );

    if( info.settings & SET_SAVE_POS ) {
        loadPositionValues( handle_inifile );
    }

    if( info.settings & SET_SAVE_SET ) {
        loadSettings( &info, handle_inifile );
    }
    _wpi_closeinifile( handle_inifile );
    SetSettingsDlg( &info );
    SetInitialOpenDir( info.opendir );
    SetInitialSaveDir( info.savedir );
    SetInitScreenColor( atol( info.color ) );

} /* LoadImgedConfig */

/*
 * savePositionValues - save the window position values
 */
static void savePositionValues( HINI hini )
{
    char        buf[10];

    itoa( ImgedConfigInfo.ismaximized, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "ismaximized", buf, iniPath );
    if( !ImgedConfigInfo.ismaximized ) {
        itoa( ImgedConfigInfo.x_pos, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_xpos", buf, iniPath );
        itoa( ImgedConfigInfo.y_pos, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_ypos", buf, iniPath );
        itoa( ImgedConfigInfo.width, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_width", buf, iniPath );
        itoa( ImgedConfigInfo.height, buf, 10 );
        _wpi_writeprivateprofilestring( hini, imgedSection, "wnd_height", buf, iniPath );
    }

    /* tool window position */
    itoa( ImgedConfigInfo.tool_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "tool_xpos", buf, iniPath );
    itoa( ImgedConfigInfo.tool_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "tool_ypos", buf, iniPath );

    /* color palette window position */
    itoa( ImgedConfigInfo.pal_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "pal_xpos", buf, iniPath );
    itoa( ImgedConfigInfo.pal_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "pal_ypos", buf, iniPath );

    /* view window position */
    itoa( ImgedConfigInfo.view_xpos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_xpos", buf, iniPath );
    itoa( ImgedConfigInfo.view_ypos, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_ypos", buf, iniPath );

} /* savePositionValues */

/*
 * saveSettings - save the settings of the current session (not including position)
 */
static void saveSettings( settings_info *info, HINI hini )
{
    char        buf[10];

    itoa( info->paste, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "paste", buf, iniPath );
    itoa( info->rotate, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "rotate", buf, iniPath );
    itoa( info->viewwnd, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "view_window", buf, iniPath );
    itoa( ImgedConfigInfo.square_grid, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "square_grid", buf, iniPath );
    itoa( ImgedConfigInfo.shift, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "shift_amt", buf, iniPath );
    itoa( ImgedConfigInfo.brush_size, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "brush_size", buf, iniPath );
    itoa( info->wrapshift, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "wrapshift", buf, iniPath );
    itoa( ImgedConfigInfo.grid_on, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "grid_on", buf, iniPath );
    _wpi_writeprivateprofilestring( hini, imgedSection, "screen_color", info->color, iniPath );
    _wpi_writeprivateprofilestring( hini, imgedSection, "open_dir", info->opendir, iniPath );
    _wpi_writeprivateprofilestring( hini, imgedSection, "save_dir", info->savedir, iniPath );

    /*
     * save the show state of the tool window, color palette, view window.
     */
    itoa( ImgedConfigInfo.show_state, buf, 10 );
    _wpi_writeprivateprofilestring( hini, imgedSection, "show_state", buf, iniPath );

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
    sprintf( info.color, "%ld", GetViewBkColor() );
    strcpy( info.opendir, GetInitOpenDir() );
    strcpy( info.savedir, GetInitSaveDir() );

    handle_inifile = _wpi_openinifile( Instance, iniPath );
    itoa( info.settings, buf, 10 );
    _wpi_writeprivateprofilestring( handle_inifile, imgedSection, "profile", buf, iniPath );
    if( info.settings & SET_SAVE_POS ) {
        savePositionValues( handle_inifile );
    }

    if( info.settings & SET_SAVE_SET ) {
        saveSettings( &info, handle_inifile );
    }
    _wpi_closeinifile( handle_inifile );

} /* SaveImgedConfig */
