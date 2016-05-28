/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of the Open Watcom Uninstaller.
*
****************************************************************************/


#define WIN32_LEAN_AND_MEAN
#include "commonui.h"
#include <shellapi.h>
#include <shlobj.h>
#include "ldstr.h"
#include "rcstr.gh"
#include "progress.h"

#define CHECK_FILE_NAME "owsetenv.bat"
#define UNINSTALL_NAME  "Open Watcom"

/*
 * DeleteFolder - delete all files in a folder
 */
static void DeleteFolder( HWND hDlg, LPCSTR pszFolder, LPCSTR pszUninstallFile )
{
    char            szFilter[MAX_PATH];
    char            szFilePath[MAX_PATH];
    char            *pch;
    WIN32_FIND_DATA wfd;
    HANDLE          hFindFile;
    MSG             msg;

    strcpy( szFilter, pszFolder );
    strcat( szFilter, "\\*.*" );
    strcpy( szFilePath, pszFolder );
    strcat( szFilePath, "\\" );
    pch = szFilePath + strlen( szFilePath );
    hFindFile = FindFirstFile( szFilter, &wfd );
    do {
        /* Let the progress dialog handle any messages. */
        while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
            if( !IsDialogMessage( hDlg, &msg ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }

        if( !strcmp( wfd.cFileName, "." ) || !strcmp( wfd.cFileName, ".." ) ) {
            /* Skip over the special directories. */
        } else if( pszUninstallFile != NULL &&
                   !stricmp( wfd.cFileName, pszUninstallFile ) ) {
            /* Just skip over the uninstaller file for now. */
        } else {
            strcpy( pch, wfd.cFileName );
            if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                DeleteFolder( hDlg, szFilePath, NULL );
                RemoveDirectory( szFilePath );
            } else {
                SetDlgItemText( hDlg, IDC_STATUS, wfd.cFileName );
                DeleteFile( szFilePath );
            }
        }

    } while( FindNextFile( hFindFile, &wfd ) );
    FindClose( hFindFile );

} /* DeleteFolder */

/*
 * ProgressDlgProc - dialog procedure for the progress dialog
 */
static INT_PTR CALLBACK ProgressDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    return( FALSE );

} /* ProgressDlgProc */

/*
 * WinMain - entry point for the uninstaller
 */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
    char            szExePath[MAX_PATH];
    char            szCheckPath[MAX_PATH];
    char            szInstallPath[MAX_PATH];
    char            *pch;
    char            *pszTitle;
    const char      *pszMessage;
    char            *pszUninstallFile;
    HWND            hDlg;
    WIN32_FIND_DATA wfd;
    HANDLE          hFindFile;
    LPITEMIDLIST    pidlPrograms;
    char            szProgramsPath[MAX_PATH];

    /*
     * Load the title for message boxes.
     */
    pszTitle = AllocRCString( UNINS_TITLE );

    /*
     * Get the directory where the uninstaller is located.
     */
    GetModuleFileName( NULL, szExePath, MAX_PATH );
    strcpy( szInstallPath, szExePath );
    pch = strrchr( szInstallPath, '\\' );
    *pch = '\0';

    /*
     * Look for the check file to ensure that the uninstaller is being actually run from
     * an Open Watcom installation directory.
     */
    strcpy( szCheckPath, szInstallPath );
    strcat( szCheckPath, "\\" );
    strcat( szCheckPath, CHECK_FILE_NAME );
    hFindFile = FindFirstFile( szCheckPath, &wfd );
    if( hFindFile == INVALID_HANDLE_VALUE ) {
        pszMessage = GetRCString( UNINS_NO_OW_INST );
        MessageBox( NULL, pszMessage, pszTitle, MB_OK | MB_ICONSTOP );
        FreeRCString( pszTitle );
        return( 1 );
    }
    FindClose( hFindFile );

    /*
     * Check that the user actually wants to uninstall Open Watcom.
     */
    pszMessage = GetRCString( UNINS_CONFIRM );
    if( MessageBox( NULL, pszMessage, pszTitle, MB_YESNO | MB_ICONQUESTION ) == IDNO ) {
        FreeRCString( pszTitle );
        return( 0 );
    }

    /*
     * Display the progress dialog box during the uninstallation.
     */
    hDlg = CreateDialog( hInstance, "PROGRESS", NULL, ProgressDlgProc );

    /*
     * Get the name of the uninstaller.
     */
    pszUninstallFile = strrchr( szExePath, '\\' );
    pszUninstallFile++;

    /*
     * Delete all files except the uninstaller.
     */
    DeleteFolder( hDlg, szInstallPath, pszUninstallFile );

    /*
     * Delete the Open Watcom program folder from the start menu.
     */
    if( SUCCEEDED( SHGetSpecialFolderLocation( NULL, CSIDL_PROGRAMS, &pidlPrograms ) ) ) {
        if( SHGetPathFromIDList( pidlPrograms, szProgramsPath ) ) {
            pch = szProgramsPath + strlen( szProgramsPath );
            *pch = '\\';
            pch++;
            strcpy( pch, "Open Watcom C - C++" );
            DeleteFolder( hDlg, szProgramsPath, NULL );
            RemoveDirectory( szProgramsPath );
            strcpy( pch, "Open Watcom C - C++ Tools Help" );
            DeleteFolder( hDlg, szProgramsPath, NULL );
            RemoveDirectory( szProgramsPath );
        }
    }

    /*
     * Tell Windows to delete the uninstaller and the folder in which it lives the next
     * time the computer is restarted.
     */
    MoveFileEx( szExePath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
    MoveFileEx( szInstallPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );

    /*
     * Remove the uninstaller from the Add/Remove Programs list.
     */
    RegDeleteKey( HKEY_LOCAL_MACHINE,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"
                  UNINSTALL_NAME );

    /*
     * Take away the progress dialog.
     */
    EndDialog( hDlg, 0 );

    /*
     * Inform the user that the uninstallation has been completed.
     */
    pszMessage = GetRCString( UNINS_COMPLETED );
    MessageBox( NULL, pszMessage, pszTitle, MB_OK );

    FreeRCString( pszTitle );
    return( 0 );

} /* WinMain */
