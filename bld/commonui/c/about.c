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
* Description:  About dialog box.
*
****************************************************************************/


#include "precomp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bool.h"
#include "wpi.h"
#include "about.h"
#include "aboutdlg.h"
#ifndef NOUSE3D
#include "ctl3dcvr.h"
#endif
#include "ldstr.h"
#include "uistr.gh"
#include "wprocmap.h"

#if defined( __WATCOMC__ ) && defined( __WINDOWS__ ) && !defined( __WINDOWS_386__ )
    #pragma library( "toolhelp.lib" )   /* For SystemHeapInfo */
#endif


/*
 * AboutProc - callback routine for settings dialog
 */
WINEXPORT INT_PTR CALLBACK AboutProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        buff[256];
    LPABOUTINFO pai;

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        pai = (LPABOUTINFO)lparam;
        if( pai->title != NULL ) {
            SetWindowText( hwnd, pai->title );
        }
        if( pai->name != NULL ) {
            sprintf( buff, banner1p1( "%s" ), pai->name );
            SetDlgItemText( hwnd, ABOUT_NAME, buff );
        }
        if( pai->version != NULL ) {
            SetDlgItemText( hwnd, ABOUT_VERSION, pai->version );
        }
        if( pai->first_cr_year != NULL ) {
#if defined( __WATCOMC__) && !defined( __ALPHA__ )
            if( _fstrcmp( pai->first_cr_year, CURR_YEAR ) ) {
#else
            if( strcmp( pai->first_cr_year, CURR_YEAR ) ) {
#endif
                sprintf( buff, banner2a( "%s" ), pai->first_cr_year );
            } else {
                strcpy( buff, banner2a( "1984" ) );
            }
            SetDlgItemText( hwnd, ABOUT_COPYRIGHT, buff );
        }
#if defined( __WINDOWS__ ) || defined( __WINDOWS_386__ )
        {
            DWORD       flags;
            DWORD       kfree;
            WORD        pcfree;
            SYSHEAPINFO shi;
            char        work[16];
            char        info[50];

            flags = GetWinFlags();
            if( flags & WF_ENHANCED ) {
                CopyRCString( ABT_386_ENH, info, sizeof( info ) );
            } else {
                CopyRCString( ABT_STD_MODE, info, sizeof( info ) );
            }
            SetDlgItemText( hwnd, ABOUT_INFO1, info );
            kfree = GetFreeSpace( 0 ) / 1024L;
            ltoa( kfree, work, 10 );
            RCsprintf( buff, ABT_MEM_X_KB_FREE, (LPSTR)work );
            SetDlgItemText( hwnd, ABOUT_INFO2, buff );

            shi.dwSize = sizeof( shi );
            if( SystemHeapInfo( &shi ) ) {
                pcfree = shi.wGDIFreePercent;
                if( pcfree > shi.wUserFreePercent )
                    pcfree = shi.wUserFreePercent;
                RCsprintf( buff, ABT_SYS_RESOURCES_FREE, pcfree );
            } else {
                CopyRCString( ABT_SYS_RESOURCES_N_A, buff, sizeof( buff ) );
            }
            SetDlgItemText( hwnd, ABOUT_INFO3, buff );
        }
#else
        {
            SYSTEM_INFO         si;
            MEMORYSTATUS        ms;

            GetSystemInfo( &si );
            switch( si.dwProcessorType ) {
            case PROCESSOR_INTEL_386:
                CopyRCString( ABT_RUNNING_ON_386, buff, sizeof( buff ) );
                break;
            case PROCESSOR_INTEL_486:
                CopyRCString( ABT_RUNNING_ON_486, buff, sizeof( buff ) );
                break;
            case PROCESSOR_INTEL_PENTIUM:
                CopyRCString( ABT_RUNNING_ON_586, buff, sizeof( buff ) );
                break;
            case PROCESSOR_MIPS_R4000:
            case PROCESSOR_ALPHA_21064:
            default:
                buff[0] = '\0';
                break;
            }
            SetDlgItemText( hwnd, ABOUT_INFO1, buff );

            ms.dwLength = sizeof( ms );
            GlobalMemoryStatus( &ms );
            RCsprintf( buff, ABT_VIRT_MEM,
                       (ms.dwAvailPhys + ms.dwAvailPageFile) / 1024L );
            SetDlgItemText( hwnd, ABOUT_INFO2, buff );
            RCsprintf( buff, ABT_MEM_LOAD, ms.dwMemoryLoad );
            SetDlgItemText( hwnd, ABOUT_INFO3, buff );

        }
#endif
        return( TRUE );
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            break;
        }
    }
    return( FALSE );

} /* AboutProc */

/*
 * DoAbout - show the startup dialog
 */
void DoAbout( LPABOUTINFO ai )
{
    FARPROC     proc;

    proc = MakeDlgProcInstance( AboutProc, ai->inst );
    DialogBoxParam( ai->inst, "About", ai->owner, (DLGPROC)proc, (LPARAM)ai );
    FreeProcInstance( proc );

} /* DoAbout */
