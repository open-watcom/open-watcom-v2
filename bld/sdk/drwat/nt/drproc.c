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


#include "bool.h"
#include "drwatcom.h"
#include "menu.h"
#include "aboutdlg.h"
#include "savelbox.h"
#include "mark.h"
#include "end.h"
#include "wwinhelp.h"
#include "jdlg.h"
#include <stdio.h>
#include <time.h>

#define DR_HELP_FILE    "drnt.hlp"
#define DR_CHM_FILE     "drnt.chm"
#define HELP_HELP_FILE  "winhelp.hlp"

/*
 * MarkPrint - call back function for processing marks
 */
void MarkPrint( char *str ) {
    LBStrPrintf( MainLBox, str );
}

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
 * QueryEnddlgProc
 */

BOOL CALLBACK QueryEndDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        cmd;
    char        buf[100];
    HWND        lb;
    ProcNode    *procinfo;
    ProcStats   procstat;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        lb = GetDlgItem( hwnd, END_LIST );
        procinfo = GetNextOwnedProc( NULL );
        while( procinfo != NULL ) {
            if( GetProcessInfo( procinfo->procid, &procstat ) ) {
                RCsprintf( buf, STR_PROCESS_X_NAME, procinfo->procid,
                         procstat.name );
            } else {
                RCsprintf( buf, STR_PROCESS_X, procinfo->procid );
            }
            SendMessage( lb, LB_ADDSTRING, 0, (LPARAM)buf );
            procinfo = GetNextOwnedProc( procinfo );
        }
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case END_YES:
        case END_NO:
            EndDialog( hwnd, cmd );
            break;
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

/*
 * QueryEnd - warn the user about the consequences of exitting
 */
BOOL QueryEnd( HWND owner ) {

    INT_PTR     rc;

    if( GetNextOwnedProc( NULL ) == NULL ) {
        return( TRUE );
    }
    rc = JDialogBox( Instance, "END_DLG", owner, QueryEndDlgProc );
    return( rc == END_YES );
}

static void setupSystemMenu( HWND hwnd ) {
    HMENU       smh;
    HMENU       mh;
    char        menuname[256];

    smh = GetSystemMenu( hwnd, FALSE );
    mh = GetMenu( hwnd );
    AppendMenu( smh, MF_SEPARATOR, 0,NULL );
    GetMenuString( mh, MENU_LOG_CURRENT_STATE, menuname, sizeof( menuname ),
                   MF_BYCOMMAND );
    AppendMenu( smh, MF_ENABLED, MENU_LOG_CURRENT_STATE, menuname );
    GetMenuString( mh, MENU_LOG_OPTIONS, menuname, sizeof( menuname ),
                   MF_BYCOMMAND );
    AppendMenu( smh, MF_ENABLED, MENU_LOG_OPTIONS, menuname );
    GetMenuString( mh, MENU_TASK_CTL, menuname, sizeof( menuname ),
                   MF_BYCOMMAND );
    AppendMenu( smh, MF_ENABLED, MENU_TASK_CTL, menuname );
}

/*
 * WindowProc - main window message handler
 */
LONG CALLBACK MainWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD                        cmd;
    about_info                  ai;
    HMENU                       mh;
    CommunicationBuffer         *dbginfo;

    switch ( msg ) {
    case WM_CREATE:
        setupSystemMenu( hwnd );
        MainLBox = CreateListBox( hwnd );
        mh = GetMenu( hwnd );
        if( ConfigData.auto_attatch ) {
            CheckMenuItem( mh, MENU_AUTO_ATTATCH, MF_BYCOMMAND | MF_CHECKED );
        }
        if( ConfigData.continue_exception ) {
            CheckMenuItem( mh, MENU_EXCEPTION_CONTINUE,
                                MF_BYCOMMAND | MF_CHECKED );
        }
        LBPrintf( MainLBox, STR_DRNT_STARTED, AppName );
        break;
    case WM_SIZE:
        MoveListBox( MainLBox, 0, 0, LOWORD( lparam ), HIWORD( lparam ) );
        ClearAlert();
        break;
    case WM_SYSCOMMAND:
        switch( wparam ) {
        case MENU_LOG_CURRENT_STATE:
        case MENU_LOG_OPTIONS:
        case MENU_TASK_CTL:
            SendMessage( hwnd, WM_COMMAND, wparam, 0 );
            break;
        default:
            return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        }
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case LISTBOX_1:
            break;
        case MENU_EXCEPTION_CONTINUE:
            ConfigData.continue_exception = !ConfigData.continue_exception;
            mh = GetMenu( hwnd );
            if( ConfigData.continue_exception ) {
                CheckMenuItem( mh, MENU_EXCEPTION_CONTINUE,
                                   MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( mh, MENU_EXCEPTION_CONTINUE,
                                   MF_BYCOMMAND | MF_UNCHECKED );
            }
            break;
        case MENU_LOG_VIEW:
            ViewLog();
            break;
        case MENU_NEW_TASK:
            CallProcCtl( MENU_NEW_TASK, NULL, NULL );
            break;
        case MENU_AUTO_ATTATCH:
            ConfigData.auto_attatch = ! ConfigData.auto_attatch;
            mh = GetMenu( hwnd );
            if( ConfigData.auto_attatch ) {
                CheckMenuItem( mh, MENU_AUTO_ATTATCH,
                               MF_BYCOMMAND | MF_CHECKED );
            } else {
                CheckMenuItem( mh, MENU_AUTO_ATTATCH,
                               MF_BYCOMMAND | MF_UNCHECKED );
            }
            break;
        case MENU_SHOW_DIP_STATUS:
            ShowDIPStatus( hwnd );
            break;
        case MENU_CLEAR:
            ClearListBox( MainLBox );
            break;
        case MENU_SAVE_AS:
            SaveListBox( SLB_SAVE_AS, SaveExtra, "", AppName, hwnd,
                         GetListBoxHwnd( MainLBox ) );
            break;
        case MENU_SAVE:
            SaveListBox( SLB_SAVE_TMP, SaveExtra, ".\\drwat.txt", AppName,
                         hwnd, GetListBoxHwnd( MainLBox ) );
            break;
        case MENU_FONT:
            if( ChooseMonoFont( hwnd ) ) {
                SetListBoxFont( MainLBox );
            }
            break;
        case MENU_MARK:
            ProcessMark( hwnd, Instance, MarkPrint );
            break;
        case MENU_ABOUT:
            ai.owner = hwnd;
            ai.inst = Instance;
            ai.name = AllocRCString( STR_ABOUT_NAME );
            ai.version = AllocRCString( STR_ABOUT_VERSION );
            ai.first_cr_year = "1993";
            ai.title = AllocRCString( STR_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            break;
        case MENU_HELP_CONTENTS:
            if( !WHtmlHelp( hwnd, DR_CHM_FILE, HELP_CONTENTS, 0 ) ) {
                WWinHelp( hwnd, DR_HELP_FILE, HELP_CONTENTS, 0 );
            }
            break;
        case MENU_HELP_SRCH:
            if( !WHtmlHelp( hwnd, DR_CHM_FILE, HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
                WWinHelp( hwnd, DR_HELP_FILE, HELP_PARTIALKEY, (HELP_DATA)"" );
            }
            break;
        case MENU_HELP_ON_HELP:
            WWinHelp( hwnd, HELP_HELP_FILE, HELP_HELPONHELP, 0 );
            break;
        case MENU_TASK_CTL:
            DisplayProcList();
            break;
        case MENU_LOG_CURRENT_STATE:
            MakeLog( NULL );
            break;
        case MENU_ERASE_LOG_FILE:
            EraseLog();
            break;
        case MENU_LOG_OPTIONS:
            SetLogOptions( hwnd );
            break;
        case MENU_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        default:
            MessageBox( hwnd, "This function is not yet available",
                        AppName, MB_OK );
            break;
        }
        break;
    case DR_DEBUG_EVENT:
        dbginfo = (CommunicationBuffer *)lparam;
        dbginfo->action = DebugEventHandler( &dbginfo->dbginfo );
        break;
    case WM_COPYDATA:
        {
            COPYDATASTRUCT      *copydata;

            copydata = (COPYDATASTRUCT *)lparam;
            ProcessCommandLine( copydata->lpData );
        }
        break;
    case WM_CLOSE:
        if( QueryEnd( hwnd ) ) {
            SendMessage( hwnd, WM_DESTROY, 0, 0L );
        }
        break;
    case WM_DESTROY:
        FiniListBox( MainLBox );
        PutProfileInfo();
        DestroyMonoFonts();
        WWinHelp( hwnd, DR_HELP_FILE, HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */
