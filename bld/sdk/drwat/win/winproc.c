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
#include <time.h>
#include <string.h>
#include "drwatcom.h"
#include "savelbox.h"
#include "mark.h"
#include "aboutdlg.h"
#include "wwinhelp.h"

#define DR_HELP_FILE    "drwatcom.hlp"
#define HELP_HELP_FILE  "winhelp.hlp"

/*
 * MarkPrint - call back function for processing marks
 */
void MarkPrint( char *str ) {
    LBStrPrintf( ListBox, str );
}

/*
 * HandleDialogs - handle dialog stuff selected from menus
 */
static BOOL HandleDialogs( WORD wparam )
{
    switch( wparam ) {
    case MENU_LOG_OPTIONS:
        DoLogDialog( MainWindow );
        return( TRUE );
        break;
    case MENU_DUMP_A_TASK:
        DumpATask( MainWindow );
        return( TRUE );
        break;
    }
    return( FALSE );

} /* HandleDialogs */

/*
 * SaveExtra - save extra to file
 */
void SaveExtra( FILE *f )
{
    time_t      tod;

    fprintf( f,"%s\n", AppName );
    tod = time( NULL );
    fprintf( f,"%s", ctime( &tod ) );
    fprintf( f,"------------------------------------------------------------------------\n" );

} /* SaveExtra */

/*
 * WindowProc - main window message handler
 */
LONG __export FAR PASCAL WindowProc( HWND hwnd, int msg, WORD wparam, LONG lparam )
{
    HDC         hdc;
    TEXTMETRIC  tm;
    WORD        height,width;
    WORD        x,y;
    about_info  ai;

    switch ( msg ) {
    case WM_CREATE:
        /*
         * get properties of the window
         */
        hdc = GetDC( hwnd );
        GetTextMetrics( hdc, &tm );
        CharSizeX = tm.tmAveCharWidth;
        CharSizeY = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc);
        ListBox = CreateListBox( hwnd );
        LBPrintf( ListBox, STR_STARTED, AppName );
        break;

    case WM_INITMENU:
        CheckMenuItem( (HWND) wparam, MENU_ALERT_WARNINGS,
                        AlertOnWarnings ? MF_CHECKED : MF_UNCHECKED );
        EnableMenuItem( (HWND) wparam, MENU_RESET_FAULT_BIT,
                        FaultHandlerEntered ? MF_ENABLED : MF_GRAYED );
        break;

    case WM_SIZE:
        x = CharSizeX;
        y = CharSizeY;
        width = LOWORD( lparam ) - 2* x;
        height = HIWORD( lparam ) - 2*y;
        if( width < x) {
            width = x;
        }
        if( height < y ) {
            height = y;
        }
        MoveListBox( ListBox, x, y, width, height );
        ClearAlert();
        break;

    case WM_USER:
        HandleNotify( wparam, lparam );
        break;

    case WM_COMMAND:
        switch( wparam ) {
        case MENU_MARK:
            ProcessMark( hwnd, Instance, MarkPrint );
            break;
        case MENU_SHOW_DIP_STATUS:
            ShowDIPStatus( hwnd );
            break;
        case MENU_ALERT_WARNINGS:
            AlertOnWarnings = !AlertOnWarnings;
            break;
        case MENU_SET_FONT:
            if( ChooseMonoFont( hwnd ) ) {
                SetListBoxFont( ListBox );
            }
            break;
        case MENU_LOG_CURRENT_STATE:
            MakeLog( FALSE );
            break;
        case MENU_ERASE_LOG_FILE:
            EraseLog();
            break;
        case MENU_SAVE_AS:
            SaveListBox( SLB_SAVE_AS, SaveExtra, "", AppName, hwnd,
                         GetListBoxHwnd( ListBox ) );
            break;
        case MENU_SAVE:
            SaveListBox( SLB_SAVE_TMP, SaveExtra, ".\\drwat.txt", AppName,
                         hwnd, GetListBoxHwnd( ListBox ) );
            break;
        case MENU_CLEAR:
            ClearListBox( ListBox );
            break;
        case MENU_ABOUT:
            ai.owner = hwnd;
            ai.inst = Instance;
            ai.name = AllocRCString( STR_ABOUT_NAME );
            ai.version = banner1p2( _DRWATCOM_VERSION_ );
            ai.first_cr_year = AllocRCString( STR_ABOUT_COPYRIGHT_YEAR );
            ai.title = AllocRCString( STR_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.first_cr_year );
            FreeRCString( ai.title );
            break;
        case MENU_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case MENU_RESET_FAULT_BIT:
            FaultHandlerEntered = FALSE;
            break;
        case MENU_HELP_CONTENTS:
            WWinHelp( hwnd, DR_HELP_FILE, HELP_CONTENTS, 0 );
            break;
        case MENU_HELP_SRCH:
            WWinHelp( hwnd, DR_HELP_FILE, HELP_PARTIALKEY, (DWORD)"" );
            break;
        case MENU_HELP_ON_HELP:
            WWinHelp( hwnd, HELP_HELP_FILE, HELP_HELPONHELP, 0 );
            break;
        default:
            HandleDialogs( wparam );
            break;
        }
        break;
    case WM_SYSCOMMAND:
        if( (wparam & 0xfff0) == SC_CLOSE ) {
            ShowWindow( MainWindow, SW_SHOWMINIMIZED );
            return( 0L );
        }
        if( wparam == MENU_LOG_CURRENT_STATE ) {
            MakeLog( FALSE );
        } else if( wparam == MENU_CLEAR_ALERT ) {
            ClearAlert();
        } else if( !HandleDialogs( wparam ) ) {
            return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        }
        break;

    case WM_CLOSE:
        if( FaultHandlerEntered ) {
            RCMessageBox( NULL, STR_CANT_CLOSE_DURING_FAULT,
                          AppName, MB_TASKMODAL | MB_OK );
            return( 0L );
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );

    case WM_QUERYENDSESSION:
        PutProfileInfo();
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );

    case WM_DESTROY:
        FiniListBox( ListBox );
        PutProfileInfo();
        DestroyMonoFonts();
        WWinHelp( hwnd, DR_HELP_FILE, HELP_QUIT, 0 );
        PostQuitMessage(0);
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */
