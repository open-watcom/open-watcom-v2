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


#include "commonui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wddespy.h"
#include "inipath.h"
#include "watini.h"

#define SECT_NAME       "Open Watcom DDE Spy"

#define MFILTER         "Msg_Filter"
#define CFILTER         "CB_Filter"
#define MONSENT         "Mon_sent"
#define MONPOST         "Mon_post"
#define MONCB           "Mon_cb"
#define MONSTR          "Mon_string"
#define MONERR          "Mon_error"
#define MONLNK          "Mon_link"
#define MONCONV         "Mon_conversation"
#define TRKVIS          "Track_visible"
#define TRK_XSIZE       "Track_xsize"
#define TRK_YSIZE       "Track_ysize"
#define TRK_XPOS        "Track_xpos"
#define TRK_YPOS        "Track_ypos"
#define MSGFLTER        "message_filter"
#define CBFLTER         "cb_filter"
#define MAIN_WND_EXT    "main"
#define LOG_NAME        "log_name"
#define LOG_STATE       "logging"
#define CFG_SCROLL      "autoscroll"
#define CFG_ALIAS       "aliases"
#define CFG_SCREEN      "screen_output"
#define CFG_SHOW_TB     "show_toolbar"
#define CFG_SHOW_HINTS  "show_hints"
#define CFG_ON_TOP      "on_top"

static char iniPath[_MAX_PATH];

/*
 * setGlobalDefault - set the ConfigInfo structure to its default values
 */
static void setGlobalDefault( void )
{
    ConfigInfo.scroll = TRUE;
    ConfigInfo.alias = TRUE;
    ConfigInfo.screen_out = TRUE;
    ConfigInfo.show_tb = TRUE;
    ConfigInfo.show_hints = TRUE;
    ConfigInfo.on_top = FALSE;

} /* setGlobalDefault */

/*
 * makeWndCfgName - create a name for information about a window
 */
static void makeWndCfgName( char *buf, char *name, char *ext )
{
    sprintf( buf, "%s_%s", name, ext );

} /* makeWndCfgName */

/*
 * putProfileBool - write a boolean value to the configuration file
 */
static void putProfileBool( char *id, bool val )
{
    char        buf[15];

    if( val ) {
        itoa( true, buf, 10 );
    } else {
        itoa( false, buf, 10 );
    }
    WritePrivateProfileString( SECT_NAME, id, buf, iniPath );

} /* putProfileBool */

/*
 * writeWindowInfo - save position/size information about a window
 */
static void writeWindowInfo( WndConfigInfo *info, char *name_ext )
{
    char        name[30];
    char        buf[10];

    makeWndCfgName( name, TRKVIS, name_ext );
    putProfileBool( name, info->visible );

    makeWndCfgName( name, TRK_XPOS, name_ext );
    itoa( info->xpos, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, iniPath );

    makeWndCfgName( name, TRK_YPOS, name_ext );
    itoa( info->ypos, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, iniPath );

    makeWndCfgName( name, TRK_XSIZE, name_ext );
    itoa( info->xsize, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, iniPath );

    makeWndCfgName( name, TRK_YSIZE, name_ext );
    itoa( info->ysize, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, iniPath );

} /* writeWindowInfo */

/*
 * readWindowInfo - read size/position information about a window
 */
static void readWindowInfo( WndConfigInfo *info, char *name_ext )
{
    char        name[30];

    makeWndCfgName( name, TRKVIS, name_ext );
    info->visible = GetPrivateProfileInt( SECT_NAME, name, info->visible, iniPath );

    makeWndCfgName( name, TRK_XPOS, name_ext );
    info->xpos = GetPrivateProfileInt( SECT_NAME, name, info->xpos, iniPath );

    makeWndCfgName( name, TRK_YPOS, name_ext );
    info->ypos = GetPrivateProfileInt( SECT_NAME, name, info->ypos, iniPath );

    makeWndCfgName( name, TRK_XSIZE, name_ext );
    info->xsize = GetPrivateProfileInt( SECT_NAME, name, info->xsize, iniPath );

    makeWndCfgName( name, TRK_YSIZE, name_ext );
    info->ysize = GetPrivateProfileInt( SECT_NAME, name, info->ysize, iniPath );

} /* readWindowInfo */

/*
 * ReadConfig - read the configuration information
 */
void ReadConfig( void )
{
    char        msgfilter[MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2];
    char        cbfilter[CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2];
    char        buf[10];
    WORD        i;

    GetConfigFilePath( iniPath, sizeof( iniPath ) );
    strcat( iniPath, "\\" WATCOM_INI );

    setGlobalDefault();
    SetTrackWndDefault();
    SetMainWndDefault();

    Monitoring[MON_SENT_IND] = GetPrivateProfileInt( SECT_NAME, MONSENT, true, iniPath );
    Monitoring[MON_POST_IND] = GetPrivateProfileInt( SECT_NAME, MONPOST, true, iniPath );
    Monitoring[MON_CB_IND] = GetPrivateProfileInt( SECT_NAME, MONCB, true, iniPath );
    Monitoring[MON_STR_IND] = GetPrivateProfileInt( SECT_NAME, MONSTR, true, iniPath );
    Monitoring[MON_ERR_IND] = GetPrivateProfileInt( SECT_NAME, MONERR, true, iniPath );
    Monitoring[MON_LNK_IND] = GetPrivateProfileInt( SECT_NAME, MONLNK, true, iniPath );
    Monitoring[MON_CONV_IND] = GetPrivateProfileInt( SECT_NAME, MONCONV, true, iniPath );

    /* global settings */

    ConfigInfo.scroll = GetPrivateProfileInt( SECT_NAME, CFG_SCROLL,
                                              ConfigInfo.scroll, iniPath );
    ConfigInfo.alias = GetPrivateProfileInt( SECT_NAME, CFG_ALIAS,
                                             ConfigInfo.alias, iniPath );
    ConfigInfo.screen_out = GetPrivateProfileInt( SECT_NAME, CFG_SCREEN,
                                                  ConfigInfo.screen_out, iniPath );
    ConfigInfo.show_tb = GetPrivateProfileInt( SECT_NAME, CFG_SHOW_TB,
                                               ConfigInfo.show_tb, iniPath );
    ConfigInfo.show_hints = GetPrivateProfileInt( SECT_NAME, CFG_SHOW_HINTS,
                                                  ConfigInfo.show_hints, iniPath );
    ConfigInfo.on_top = GetPrivateProfileInt( SECT_NAME, CFG_ON_TOP,
                                              ConfigInfo.on_top, iniPath );

    /* window size/pos info */
    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        itoa( i, buf, 10 );
        readWindowInfo( Tracking + i, buf );
    }
    readWindowInfo( &MainWndConfig, MAIN_WND_EXT );

    /* filter info */
    memset( msgfilter, '1', sizeof( msgfilter ) );
    msgfilter[MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 1] = '\0';
    memset( cbfilter, '1', sizeof( cbfilter ) );
    cbfilter[CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 1] = '\0';
    GetPrivateProfileString( SECT_NAME, MSGFLTER, msgfilter, msgfilter,
                             MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2, iniPath );
    GetPrivateProfileString( SECT_NAME, CBFLTER, cbfilter, cbfilter,
                             CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2, iniPath );
    SetFilter( msgfilter, cbfilter );

    /* logging info */
    LoadLogConfig( iniPath, SECT_NAME );
    InitMonoFont( SECT_NAME, iniPath, SYSTEM_FIXED_FONT, Instance );

} /* ReadConfig */

/*
 * SaveConfigFile - save the configuration information
 */
void SaveConfigFile( void )
{
    char        msgfilter[MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2];
    char        cbfilter[CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2];
    char        buf[10];
    WORD        i;

    /* monitoring information */
    putProfileBool( MONSENT, Monitoring[MON_SENT_IND] );
    putProfileBool( MONCB, Monitoring[MON_CB_IND] );
    putProfileBool( MONSTR, Monitoring[MON_STR_IND] );
    putProfileBool( MONERR, Monitoring[MON_ERR_IND] );
    putProfileBool( MONLNK, Monitoring[MON_LNK_IND] );
    putProfileBool( MONCONV, Monitoring[MON_CONV_IND] );
    putProfileBool( MONPOST, Monitoring[MON_POST_IND] );

    /* Global Settings */
    putProfileBool( CFG_SCROLL, ConfigInfo.scroll );
    putProfileBool( CFG_ALIAS, ConfigInfo.alias );
    putProfileBool( CFG_SCREEN, ConfigInfo.screen_out );
    putProfileBool( CFG_SHOW_TB, ConfigInfo.show_tb );
    putProfileBool( CFG_SHOW_HINTS, ConfigInfo.show_hints );
    putProfileBool( CFG_ON_TOP, ConfigInfo.on_top );

    /* window position/size information */
    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        itoa( i, buf, 10 );
        writeWindowInfo( Tracking + i, buf );
    }
    writeWindowInfo( &MainWndConfig, MAIN_WND_EXT );

    /* filter information */
    GetFilter( msgfilter, cbfilter );
    WritePrivateProfileString( SECT_NAME, MSGFLTER, msgfilter, iniPath );
    WritePrivateProfileString( SECT_NAME, CBFLTER, cbfilter, iniPath );
    SaveLogConfig( iniPath, SECT_NAME );
    SaveMonoFont( SECT_NAME, iniPath );

} /* SaveConfigFile */
