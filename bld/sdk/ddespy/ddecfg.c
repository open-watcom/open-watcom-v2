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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wddespy.h"
#include "watini.h"

#define         SECT_NAME       "WATCOM DDE Spy"

#define         MFILTER         "Msg_Filter"
#define         CFILTER         "CB_Filter"
#define         MONSENT         "Mon_sent"
#define         MONPOST         "Mon_post"
#define         MONCB           "Mon_cb"
#define         MONSTR          "Mon_string"
#define         MONERR          "Mon_error"
#define         MONLNK          "Mon_link"
#define         MONCONV         "Mon_conversation"
#define         TRKVIS          "Track_visible"
#define         TRK_XSIZE       "Track_xsize"
#define         TRK_YSIZE       "Track_ysize"
#define         TRK_XPOS        "Track_xpos"
#define         TRK_YPOS        "Track_ypos"
#define         MSGFLTER        "message_filter"
#define         CBFLTER         "cb_filter"
#define         MAIN_WND_EXT    "main"
#define         LOG_NAME        "log_name"
#define         LOG_STATE       "logging"
#define         CFG_SCROLL      "autoscroll"
#define         CFG_ALIAS       "aliases"
#define         CFG_SCREEN      "screen_output"
#define         CFG_SHOW_TB     "show_toolbar"
#define         CFG_SHOW_HINTS  "show_hints"

/*
 * SetGlobalDefault - set the ConfigInfo structure to its default values
 */

static void SetGlobalDefault( void ) {
    ConfigInfo.scroll = TRUE;
    ConfigInfo.alias = TRUE;
    ConfigInfo.screen_out = TRUE;
    ConfigInfo.show_tb = TRUE;
    ConfigInfo.show_hints = TRUE;
}

/*
 * MakeWndCfgName - create a name for information about a window
 */
static MakeWndCfgName( char *buf, char *name, char *ext ) {
    sprintf( buf, "%s_%s", name, ext );
}

/*
 * WriteWindowInfo - save position/size information about a window
 */
static void WriteWindowInfo( WndConfigInfo *info, char *name_ext ) {

    char        name[30];
    char        buf[10];

    MakeWndCfgName( name, TRKVIS, name_ext );
    PutProfileBool( name, info->visible );

    MakeWndCfgName( name, TRK_XPOS, name_ext );
    itoa( info->xpos, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, WATCOM_INI );

    MakeWndCfgName( name, TRK_YPOS, name_ext );
    itoa( info->ypos, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, WATCOM_INI );

    MakeWndCfgName( name, TRK_XSIZE, name_ext );
    itoa( info->xsize, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, WATCOM_INI );

    MakeWndCfgName( name, TRK_YSIZE, name_ext );
    itoa( info->ysize, buf, 10 );
    WritePrivateProfileString( SECT_NAME, name, buf, WATCOM_INI );
}

/*
 * ReadWindowInfo - read size/position information about a window
 */
static void ReadWindowInfo( WndConfigInfo *info, char *name_ext ) {

    char        name[30];

    MakeWndCfgName( name, TRKVIS, name_ext );
    info->visible = GetPrivateProfileInt( SECT_NAME, name,
                                    info->visible, WATCOM_INI );

    MakeWndCfgName( name, TRK_XPOS, name_ext );
    info->xpos = GetPrivateProfileInt( SECT_NAME, name,
                                    info->xpos, WATCOM_INI );

    MakeWndCfgName( name, TRK_YPOS, name_ext );
    info->ypos = GetPrivateProfileInt( SECT_NAME, name,
                                    info->ypos, WATCOM_INI );

    MakeWndCfgName( name, TRK_XSIZE, name_ext );
    info->xsize = GetPrivateProfileInt( SECT_NAME, name,
                                    info->xsize, WATCOM_INI );

    MakeWndCfgName( name, TRK_YSIZE, name_ext );
    info->ysize = GetPrivateProfileInt( SECT_NAME, name,
                                    info->ysize, WATCOM_INI );
}

/*
 * PutProfileBool - write a boolean value to the configuration file
 */
static PutProfileBool( char *id, BOOL val ) {

    char        buf[15];

    if( val ) {
        itoa( TRUE, buf, 10 );
    } else {
        itoa( FALSE, buf, 10 );
    }
    WritePrivateProfileString( SECT_NAME, id, buf, WATCOM_INI );
} /* PutProfileBool */

/*
 * ReadConfig - read the configuration information
 */
void ReadConfig() {

    char        msgfilter[ MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2 ];
    char        cbfilter[ CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2 ];
    char        buf[10];
    WORD        i;

    SetGlobalDefault();
    SetTrackWndDefault();
    SetMainWndDefault();

    Monitoring[ MON_SENT_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONSENT, TRUE, WATCOM_INI );
    Monitoring[ MON_POST_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONPOST, TRUE, WATCOM_INI );
    Monitoring[ MON_CB_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONCB, TRUE, WATCOM_INI );
    Monitoring[ MON_STR_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONSTR, TRUE, WATCOM_INI );
    Monitoring[ MON_ERR_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONERR, TRUE, WATCOM_INI );
    Monitoring[ MON_LNK_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONLNK, TRUE, WATCOM_INI );
    Monitoring[ MON_CONV_IND ] =
                GetPrivateProfileInt( SECT_NAME, MONCONV, TRUE, WATCOM_INI );

    /* global settings */

    ConfigInfo.scroll = GetPrivateProfileInt( SECT_NAME, CFG_SCROLL,
                                ConfigInfo.scroll, WATCOM_INI );
    ConfigInfo.alias = GetPrivateProfileInt( SECT_NAME, CFG_ALIAS,
                                ConfigInfo.alias, WATCOM_INI );
    ConfigInfo.screen_out = GetPrivateProfileInt( SECT_NAME, CFG_SCREEN,
                                ConfigInfo.screen_out, WATCOM_INI );
    ConfigInfo.show_tb = GetPrivateProfileInt( SECT_NAME, CFG_SHOW_TB,
                                ConfigInfo.show_tb, WATCOM_INI );
    ConfigInfo.show_hints = GetPrivateProfileInt( SECT_NAME, CFG_SHOW_HINTS,
                                ConfigInfo.show_hints, WATCOM_INI );

    /* window size/pos info */
    for( i = 0; i < NO_TRK_WND; i++ ) {
        itoa( i, buf, 10 );
        ReadWindowInfo( Tracking + i, buf );
    }
    ReadWindowInfo( &MainWndConfig, MAIN_WND_EXT );

    /* filter info */
    memset( msgfilter, '1', sizeof( msgfilter ) );
    msgfilter[ MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 1 ] = '\0';
    memset( cbfilter, '1', sizeof( cbfilter ) );
    cbfilter[ CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 1 ] = '\0';
    GetPrivateProfileString( SECT_NAME, MSGFLTER, msgfilter, msgfilter,
                MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2, WATCOM_INI );
    GetPrivateProfileString( SECT_NAME, CBFLTER, cbfilter, cbfilter,
                CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2, WATCOM_INI );
    SetFilter( msgfilter, cbfilter );

    /* logging info */
    LoadLogConfig( WATCOM_INI, SECT_NAME );
    InitMonoFont( SECT_NAME, WATCOM_INI, SYSTEM_FIXED_FONT, Instance );
}

/*
 * SaveConfigFile - save the configuration information
 */
void SaveConfigFile() {

    char        msgfilter[ MFILTER_LAST_MSG - MFILTER_FIRST_MSG + 2 ];
    char        cbfilter[ CFILTER_LAST_MSG - CFILTER_FIRST_MSG + 2 ];
    char        buf[10];
    WORD        i;

    /* monitoring information */
    PutProfileBool( MONSENT, Monitoring[ MON_SENT_IND ] );
    PutProfileBool( MONCB, Monitoring[ MON_CB_IND ] );
    PutProfileBool( MONSTR, Monitoring[ MON_STR_IND ] );
    PutProfileBool( MONERR, Monitoring[ MON_ERR_IND ] );
    PutProfileBool( MONLNK, Monitoring[ MON_LNK_IND ] );
    PutProfileBool( MONCONV, Monitoring[ MON_CONV_IND ] );
    PutProfileBool( MONPOST, Monitoring[ MON_POST_IND ] );

    /* Global Settings */
    PutProfileBool( CFG_SCROLL, ConfigInfo.scroll );
    PutProfileBool( CFG_ALIAS, ConfigInfo.alias );
    PutProfileBool( CFG_SCREEN, ConfigInfo.screen_out );
    PutProfileBool( CFG_SHOW_TB, ConfigInfo.show_tb );
    PutProfileBool( CFG_SHOW_HINTS, ConfigInfo.show_hints );


    /* window position/size information */
    for( i = 0; i < NO_TRK_WND; i++ ) {
        itoa( i, buf, 10 );
        WriteWindowInfo( Tracking + i, buf );
    }
    WriteWindowInfo( &MainWndConfig, MAIN_WND_EXT );

    /* filter information */
    GetFilter( msgfilter, cbfilter );
    WritePrivateProfileString( SECT_NAME, MSGFLTER, msgfilter, WATCOM_INI );
    WritePrivateProfileString( SECT_NAME, CBFLTER, cbfilter, WATCOM_INI );
    SaveLogConfig( WATCOM_INI, SECT_NAME );
    SaveMonoFont( SECT_NAME, WATCOM_INI );
}
