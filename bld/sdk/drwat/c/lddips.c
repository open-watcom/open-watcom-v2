/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "drwatcom.h"
#include "wclbproc.h"
#include "dip.h"
#include "dipload.rh"
#include "diplasth.h"
#include "ldstr.h"
#include "rcstr.grh"
#include "jdlg.h"


/* Local Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK ShowDipStatDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

typedef struct {
    char        *loadmsg;
    bool        loaded;
#ifdef __WINDOWS__
    HINSTANCE   hinst;
#endif
} LoadInfo;

static LoadInfo         *theLoadInfo;
static int              dipCnt;

static void initDipMsgs( void )
{
    char        *ptr;

    dipCnt = 0;
    for( ptr = DIPDefaults; *ptr != '\0'; ptr += strlen( ptr ) + 1 ) {
        dipCnt++;
    }
    theLoadInfo = MemAlloc( dipCnt * sizeof( LoadInfo ) );
}

INT_PTR CALLBACK ShowDipStatDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        cmd;
    int         i;
    bool        ret;

    lparam = lparam;

    ret = false;

    switch( msg ) {
    case WM_INITDIALOG:
        for( i = 0; i < dipCnt; i++ ) {
            SetDlgMonoFont( hwnd, DIPLD_LIST );
            SetDlgMonoFont( hwnd, DIPLD_HEADER );
            SendDlgItemMessage( hwnd, DIPLD_LIST, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)theLoadInfo[i].loadmsg );
        }
        ret = true;
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDOK:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
        }
        ret = true;
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        ret = true;
        break;
    }
    return( ret );
}

void ShowDIPStatus( HWND hwnd )
{
    DLGPROC     dlgproc;

    dlgproc = MakeProcInstance_DLG( ShowDipStatDlgProc, Instance );
    JDialogBox( Instance, "DIP_STATUS_DLG", hwnd, dlgproc );
    FreeProcInstance_DLG( dlgproc );
}

bool LoadTheDips( void )
{
    int         rc;
    unsigned    i;
    char        *ptr;
    char        buf[256];
    char        status[80];
    bool        diploaded;

    diploaded = false;
    initDipMsgs();
    i = 0;
    for( ptr = DIPDefaults; *ptr != '\0'; ptr += strlen( ptr ) + 1 ) {
        rc = DIPLoad( ptr );
        if( rc & DS_ERR ) {
            rc &= ~DS_ERR;
            switch( rc ) {
            case DS_FOPEN_FAILED:
                CopyRCString( STR_DIP_NOT_FOUND, status, sizeof( status ) );
                break;
            case DS_INVALID_DIP_VERSION:
                CopyRCString( STR_WRONG_DIP_VERSION, status, sizeof( status ) );
                break;
            case DS_INVALID_DIP:
                CopyRCString( STR_BAD_DIP, status, sizeof( status) );
                break;
            case DS_TOO_MANY_DIPS:
                CopyRCString( STR_TOO_MANY_DIPS, status, sizeof( status) );
                break;
            default:
                CopyRCString( STR_DIP_ERR_OTHER, status, sizeof( status) );
                break;
            }
        } else {
            CopyRCString( STR_DIP_LOADED, status, sizeof( status) );
            diploaded = true;
        }
        sprintf( buf, "%-18s %s", ptr, status );
        theLoadInfo[i].loadmsg = MemAlloc( strlen( buf ) + 1 );
        theLoadInfo[i].loaded = ( (rc & DS_ERR) == 0 );
#ifdef __WINDOWS__
        if( theLoadInfo[i].loaded ) {
            theLoadInfo[i].hinst = DIPLastHandle;
        }
#endif
        strcpy( theLoadInfo[i].loadmsg, buf );
        i++;
    }
    if( !diploaded ) {
        RCMessageBox( NULL, STR_NO_DIPS_LOADED, AppName, MB_OK | MB_ICONEXCLAMATION );
        ShowDIPStatus( NULL );
        DIPFini();
    }
    return( diploaded );
}

void FiniDipMsgs( void )
{
    int     i;

    for( i = 0; i < dipCnt; i++ ) {
        MemFree( theLoadInfo[i].loadmsg );
    }
}

#ifdef __WINDOWS__
bool IsDip( HINSTANCE hinst )
{
    int     i;

    for( i = 0; i < dipCnt; i++ ) {
        if( theLoadInfo[i].loaded && theLoadInfo[i].hinst == hinst ) {
            return( true );
        }
    }
    return( false );
}
#endif
