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
#include <ctype.h>
#include "wzoom.h"
#include "watini.h"
#include "inipath.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK ConfigDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#define SECT_NAME       "WATCOM Zoom Utility"

#define STICKY_ID       "sticky"
#define ON_TOP          "topmost"
#define AUTOREFRESH     "autorefresh"
#define INTERVAL        "refresh_interval"

#define BUFSIZE         5

typedef struct {
    char        interval[ BUFSIZE ];
} ConfigDlgInfo;

static char iniPath[_MAX_PATH];

/*
 * ParseNumeric
 */
static BOOL ParseNumeric( char *buf, BOOL signed_val, DWORD *val ) {

    char        *end;
    char        *last;

    end = buf;
    while( *end ) end++;
    if( end != buf ) {
        end --;
        while( isspace( *end ) ) end--;
        end++;
    }
    if( signed_val ) {
        *val = strtol( buf, &last, 0 );
    } else {
        *val = strtoul( buf, &last, 0 );
    }
    if( end != last || end == buf ) {
        return( FALSE );
    }
    return( TRUE );
}

/*
 * ConfigDlgProc
 */
BOOL CALLBACK ConfigDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD                cmd;
    HWND                ctl;
    char                buf[BUFSIZE];
    DWORD               val;
    ConfigDlgInfo       *info;

    lparam = lparam;
    info = (ConfigDlgInfo *)GET_DLGDATA( hwnd );
    switch( msg ) {
    case WM_INITDIALOG:
        info = MemAlloc( sizeof( ConfigDlgInfo ) );
        SET_DLGDATA( hwnd, info );
#ifdef __NT__
        ctl = GetDlgItem( hwnd, CFG_STICKY );
        ShowWindow( ctl, SW_HIDE );
#else
        if( ConfigInfo.stickymagnifier ) {
            CheckDlgButton( hwnd, CFG_STICKY, BST_CHECKED );
        }
#endif
        if( ConfigInfo.topmost ) {
            CheckDlgButton( hwnd, CFG_TOP, BST_CHECKED );
        }
        sprintf( info->interval, "%u", ConfigInfo.refresh_interval );
        if( ConfigInfo.autorefresh ) {
            CheckDlgButton( hwnd, CFG_AUTOREFRESH, BST_CHECKED );
            SetDlgItemText( hwnd, CFG_REFRESH_INTERVAL, info->interval );
        } else {
            ctl = GetDlgItem( hwnd, CFG_REFRESH_INTERVAL );
            EnableWindow( ctl, FALSE );
            ctl = GetDlgItem( hwnd, CFG_INTERVAL_TXT );
            EnableWindow( ctl, FALSE );
            SetDlgItemText( hwnd, CFG_REFRESH_INTERVAL, "" );
        }
        SendDlgItemMessage( hwnd, CFG_REFRESH_INTERVAL,
                            EM_LIMITTEXT, BUFSIZE - 1, 0 );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case CFG_AUTOREFRESH:
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == BN_CLICKED ) {
                if( IsDlgButtonChecked( hwnd, CFG_AUTOREFRESH ) ) {
                    ctl = GetDlgItem( hwnd, CFG_REFRESH_INTERVAL );
                    EnableWindow( ctl, TRUE );
                    ctl = GetDlgItem( hwnd, CFG_INTERVAL_TXT );
                    EnableWindow( ctl, TRUE );
                    SetDlgItemText( hwnd, CFG_REFRESH_INTERVAL,
                                    info->interval );
                } else {
                    ctl = GetDlgItem( hwnd, CFG_REFRESH_INTERVAL );
                    EnableWindow( ctl, FALSE );
                    ctl = GetDlgItem( hwnd, CFG_INTERVAL_TXT );
                    EnableWindow( ctl, FALSE );
                    GetDlgItemText( hwnd, CFG_REFRESH_INTERVAL,
                                    info->interval, BUFSIZE );
                    SetDlgItemText( hwnd, CFG_REFRESH_INTERVAL, "" );
                }
            }
            break;
        case IDOK:
            if( IsDlgButtonChecked( hwnd, CFG_AUTOREFRESH ) ) {
                GetDlgItemText( hwnd, CFG_REFRESH_INTERVAL, buf, BUFSIZE );
                if( !ParseNumeric( buf, FALSE, &val ) ) {
                    RCMessageBox( hwnd, STR_INTERV_NOT_NUMERIC,
                            AppName, MB_OK | MB_ICONEXCLAMATION ) ;
                    break;
                }
                if( val == 0 || val > 0xFFFFFFFF / 2 ) {
                    RCMessageBox( hwnd, STR_INTERV_IS_ZERO,
                            AppName, MB_OK | MB_ICONEXCLAMATION ) ;
                    break;
                }
                ConfigInfo.refresh_interval = val;
                DoAutoRefresh( MainWin, TRUE );
            } else {
                DoAutoRefresh( MainWin, FALSE );
            }
            ConfigInfo.stickymagnifier = IsDlgButtonChecked( hwnd,
                                                CFG_STICKY );
            ConfigInfo.topmost = IsDlgButtonChecked( hwnd, CFG_TOP );
            if( ConfigInfo.topmost ) {
                SetWindowPos( MainWin, HWND_TOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            } else {
                SetWindowPos( MainWin, HWND_NOTOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE );
            }
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDCANCEL:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;
    case WM_CLOSE:
        MemFree( info );
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

/*
 * DoConfig
 */
void DoConfig( HWND hwnd ) {

    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)ConfigDlgProc, Instance );
    DialogBox( Instance, "ZOOM_CONFIGURE", hwnd, (DLGPROC)fp );
    FreeProcInstance( fp );
}

/*
 * LoadConfig
 */
void LoadConfig( void ) {

    GetConfigFilePath( iniPath, sizeof(iniPath) );
    strcat( iniPath, "\\" WATCOM_INI );
#ifndef __NT__
    ConfigInfo.stickymagnifier = GetPrivateProfileInt( SECT_NAME, STICKY_ID,
                                             FALSE, iniPath );
#else
    ConfigInfo.stickymagnifier = FALSE;
#endif
    ConfigInfo.topmost = GetPrivateProfileInt( SECT_NAME, ON_TOP,
                                             TRUE, iniPath );
    ConfigInfo.refresh_interval = GetPrivateProfileInt( SECT_NAME, INTERVAL,
                                                         10, iniPath );
    ConfigInfo.autorefresh = GetPrivateProfileInt( SECT_NAME, AUTOREFRESH,
                                                         FALSE, iniPath );
}

/*
 * SaveConfig
 */
void SaveConfig( void ) {

    char        buf[15];

#ifndef __NT__
    itoa( ConfigInfo.stickymagnifier, buf, 10 );
    WritePrivateProfileString( SECT_NAME, STICKY_ID, buf, iniPath );
#endif
    itoa( ConfigInfo.topmost, buf, 10 );
    WritePrivateProfileString( SECT_NAME, ON_TOP, buf, iniPath );
    itoa( ConfigInfo.refresh_interval, buf, 10 );
    WritePrivateProfileString( SECT_NAME, INTERVAL, buf, iniPath );
    itoa( ConfigInfo.autorefresh, buf, 10 );
    WritePrivateProfileString( SECT_NAME, AUTOREFRESH, buf, iniPath );
}

