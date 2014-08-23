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
* Description:  Spy configuration functions.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include "watini.h"
#include "inipath.h"

static char spyApp[] = "WATCOMSpy";
static char iniPath[_MAX_PATH] = WATCOM_INI;

/*
 * LoadSpyConfig - get configuration from a profile file
 */
void LoadSpyConfig( char *fname )
{
    char        *str, *vals;
    int         i, j, k;
    int         x, y;
    int         check;

    str = alloca( TotalMessageArraySize + 1 );
    vals = alloca( TotalMessageArraySize + 1 );
    for( i = 0; i < FILTER_ENTRIES; i++ ) {
        Filters[i].flag[M_WATCH] = TRUE;
        Filters[i].flag[M_STOPON] = FALSE;
    }
    if( fname == NULL ) {
        GetConfigFilePath( iniPath, sizeof( iniPath ) );
        strcat( iniPath, "\\" WATCOM_INI );
        fname = iniPath;
        LoadLogConfig( fname, spyApp );
        InitMonoFont( spyApp, fname, SYSTEM_FIXED_FONT, Instance );

        x = GetSystemMetrics( SM_CXSCREEN );
        y = GetSystemMetrics( SM_CYSCREEN );

        /* set defaults */
        SpyMainWndInfo.xpos = x / 8 - x / 16;
        SpyMainWndInfo.ypos = y / 8;
        SpyMainWndInfo.xsize = 3 * (x / 4) + x / 8;
        SpyMainWndInfo.ysize = 3 * (y / 4);
        SpyMainWndInfo.on_top = false;
        SpyMainWndInfo.show_hints = true;
        SpyMainWndInfo.show_toolbar = true;

        /* load configured values */
        SpyMainWndInfo.xpos = GetPrivateProfileInt( spyApp, "wnd_xpos",
                                                    SpyMainWndInfo.xpos, fname );
        SpyMainWndInfo.ypos = GetPrivateProfileInt( spyApp, "wnd_ypos",
                                                    SpyMainWndInfo.ypos, fname );
        SpyMainWndInfo.xsize = GetPrivateProfileInt( spyApp, "wnd_xsize",
                                                     SpyMainWndInfo.xsize, fname );
        SpyMainWndInfo.ysize = GetPrivateProfileInt( spyApp, "wnd_ysize",
                                                     SpyMainWndInfo.ysize, fname );
        SpyMainWndInfo.on_top = GetPrivateProfileInt( spyApp, "wnd_topmost", SpyMainWndInfo.on_top, fname ) != 0;
        SpyMainWndInfo.show_hints = GetPrivateProfileInt( spyApp, "show_hint", SpyMainWndInfo.show_hints, fname ) != 0;
        SpyMainWndInfo.show_toolbar = GetPrivateProfileInt( spyApp, "show_toolbar", SpyMainWndInfo.show_toolbar, fname ) != 0;
    }

    /*
     * what specific messages to watch
     */
    memset( vals, '1', TotalMessageArraySize );
    vals[TotalMessageArraySize] = 0;
    GetPrivateProfileString( spyApp, "watch", vals, str,
                             TotalMessageArraySize + 1, fname );

    for( j = 0, i = 0; j < ClassMessagesSize; j++ ) {
        for( k = 0; k < ClassMessages[j].message_array_size; k++ ) {
            ClassMessages[j].message_array[k].bits[M_WATCH] = ( str[i++]  != '0');
        }
    }

    /*
     * what specific messages to stop on
     */
    memset( vals, '0', TotalMessageArraySize );
    GetPrivateProfileString( spyApp, "stopon", vals, str,
                             TotalMessageArraySize + 1, fname );
    for( j = 0, i = 0; j < ClassMessagesSize; j++ ) {
        for( k = 0; k < ClassMessages[j].message_array_size; k++ ) {
            ClassMessages[j].message_array[k].bits[M_STOPON] = ( str[i++]  == '1');
        }
    }

    /*
     * what message classes to watch
     */
    memset( vals, '1', FILTER_ENTRIES );
    vals[FILTER_ENTRIES] = 0;
    GetPrivateProfileString( spyApp, "watchclasses", vals, str,
                             FILTER_ENTRIES + 1, fname );
    for( i = 0; i < FILTER_ENTRIES; i++ ) {
        Filters[i].flag[M_WATCH] = ( str[i]  != '0');
    }

    /*
     * what message classes to stopon
     */
    memset( vals, '0', FILTER_ENTRIES );
    GetPrivateProfileString( spyApp, "stoponclasses", vals, str,
                             FILTER_ENTRIES + 1, fname );
    for( i = 0; i < FILTER_ENTRIES; i++ ) {
        Filters[i].flag[M_STOPON] = ( str[i] == '1' );
    }

    /*
     * get misc info
     */
    vals[0] = '1';
    vals[1] = 0;
    GetPrivateProfileString( spyApp, "autosavecfg", vals, str, 2, fname );
    AutoSaveConfig = ( str[0]  != '0');
    if( AutoSaveConfig ) {
        check = MF_CHECKED;
    } else {
        check = MF_UNCHECKED;
    }
    CheckMenuItem( SpyMenu, SPY_MESSAGES_ASCFG, check );

} /* LoadSpyConfig */

/*
 * SaveSpyConfig - write configuration to a profile file
 */
void SaveSpyConfig( char *fname )
{
    char        *str;
    char        c;
    int         i, j, k;
    char        buf[10];

    str = alloca( TotalMessageArraySize + 1 );
    if( fname == NULL ) {
        fname = iniPath;
        SaveLogConfig( fname, spyApp );
        SaveMonoFont( spyApp, fname );

#define GET_BOOL(x)  (((x)!=0) ? "" : "0")
        itoa( SpyMainWndInfo.xpos, buf, 10 );
        WritePrivateProfileString( spyApp, "wnd_xpos", buf, fname );
        itoa( SpyMainWndInfo.ypos, buf, 10 );
        WritePrivateProfileString( spyApp, "wnd_ypos", buf, fname );
        itoa( SpyMainWndInfo.xsize, buf, 10 );
        WritePrivateProfileString( spyApp, "wnd_xsize", buf, fname );
        itoa( SpyMainWndInfo.ysize, buf, 10 );
        WritePrivateProfileString( spyApp, "wnd_ysize", buf, fname );
        WritePrivateProfileString( spyApp, "wnd_topmost", GET_BOOL( SpyMainWndInfo.on_top ), fname );
        WritePrivateProfileString( spyApp, "show_hint", GET_BOOL( SpyMainWndInfo.show_hints ), fname );
        WritePrivateProfileString( spyApp, "show_toolbar", GET_BOOL( SpyMainWndInfo.show_toolbar ), fname );
#undef GET_BOOL
    }

    if( fname != iniPath || AutoSaveConfig ) {
        /*
         * what specific messages to watch
         */
        for( j = 0, i = 0; j < ClassMessagesSize; j++ ) {
            for( k = 0; k < ClassMessages[j].message_array_size; k++ ) {
                if( ClassMessages[j].message_array[k].bits[M_WATCH] ) {
                    c = '1';
                } else {
                    c = '0';
                }
                str[i++] = c;
            }
        }
        str[TotalMessageArraySize] = 0;
        WritePrivateProfileString( spyApp, "watch", str, fname );

        /*
         * what specific messages to stop on
         */
        for( j = 0, i = 0; j < ClassMessagesSize; j++ ) {
            for( k = 0; k < ClassMessages[j].message_array_size; k++ ) {
                if( ClassMessages[j].message_array[k].bits[M_STOPON] ) {
                    c = '1';
                } else {
                    c = '0';
                }
                str[i++] = c;
            }
        }
        str[TotalMessageArraySize] = 0;
        WritePrivateProfileString( spyApp, "stopon", str, fname );

        /*
         * what message classes to watch
         */
        for( i = 0; i < FILTER_ENTRIES; i++ ) {
            if( Filters[i].flag[M_WATCH] ) {
                c = '1';
            } else {
                c = '0';
            }
            str[i] = c;
        }
        str[FILTER_ENTRIES] = 0;
        WritePrivateProfileString( spyApp, "watchclasses", str, fname );

        /*
         * what message classes to stopon
         */
        for( i = 0; i < FILTER_ENTRIES; i++ ) {
            if( Filters[i].flag[M_STOPON] ) {
                c = '1';
            } else {
                c = '0';
            }
            str[i] = c;
        }
        str[FILTER_ENTRIES] = 0;
        WritePrivateProfileString( spyApp, "stoponclasses", str, fname );
    }

    /*
     * save misc info
     */
    if( AutoSaveConfig ) {
        c = '1';
    } else {
        c = '0';
    }
    str[0] = c;
    str[1] = 0;
    WritePrivateProfileString( spyApp, "autosavecfg", str, fname );

} /* SaveSpyConfig */

/*
 * DoSaveSpyConfig - allow user to save spy config
 */
void DoSaveSpyConfig( void )
{
    char        fname[_MAX_PATH];

    if( !GetFileName( "*.ini", FILE_SAVE, fname ) ) {
        return;
    }
    fclose( fopen( fname, "w" ) );
    SaveSpyConfig( fname );

} /* DoSaveSpyConfig */

/*
 * DoLoadSpyConfig - allow user to save spy config
 */
void DoLoadSpyConfig( void )
{
    char        fname[_MAX_PATH];

    if( !GetFileName( "*.ini", FILE_OPEN, fname ) ) {
        return;
    }
    LoadSpyConfig( fname );

} /* DoLoadSpyConfig */

