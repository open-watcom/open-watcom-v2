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
#include "spy.h"
#include <dde.h>
#include "wwinhelp.h"

static int      currBit;
static char     *savedBits;

/*
 * SpyMsgDialog - process message range dialogs
 */
BOOL CALLBACK SpyMsgDialog( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    int         i;
    static int  which,hi,lo;
    char        fl;
    HWND        nwnd,pwnd;
    WORD        cmdid;
    char        *fmtstr;
    char        title[100];

    switch( msg ) {
    case WM_INITDIALOG:
        which = lparam;
        nwnd = GetDlgItem( hwnd, DLGMSG_NEXT );
        pwnd = GetDlgItem( hwnd, DLGMSG_PREV );
        fmtstr = GetRCString( STR_MSG_RANGE_STR );
        if( which == 1 ) {
            sprintf( title, fmtstr, MSG_RANGE_0_FIRST, MSG_RANGE_0_LAST );
            EnableWindow( pwnd, FALSE );
            lo = DLGMSG1;
            hi = DLGMSG2;
        } else if( which == 2 ) {
            sprintf( title, fmtstr, MSG_RANGE_1_FIRST, MSG_RANGE_1_LAST );
            lo = DLGMSG2;
            hi = DLGMSG3;
        } else if( which == 3 ) {
            sprintf( title, fmtstr, MSG_RANGE_2_FIRST, MSG_RANGE_2_LAST );
            lo = DLGMSG3;
            hi = DLGMSG4;
        } else {
            sprintf( title, fmtstr, MSG_RANGE_3_FIRST, MSG_RANGE_3_LAST );
            EnableWindow( nwnd, FALSE );
            lo = DLGMSG4;
            hi = DLGMSG4END;
        }
        SetWindowText( hwnd, title );
        for( i=lo; i<hi;i++ ) {
            SetDlgMonoFont( hwnd, i );
            CheckDlgButton( hwnd, i, savedBits[i-DLGMSG1] );
        }
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        Ctl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( cmdid >= DLGMSG1 && cmdid < DLGMSG4END ) {
            if( savedBits[cmdid-DLGMSG1] ) {
                savedBits[cmdid-DLGMSG1] = 0;
            } else {
                savedBits[cmdid-DLGMSG1] = 1;
            }
            CheckDlgButton( hwnd, cmdid, savedBits[cmdid-DLGMSG1] );
            break;
        }
        switch( cmdid ) {
        case DLGMSG_CLEARALL:
        case DLGMSG_SETALL:
            fl = (cmdid == DLGMSG_SETALL);
            for( i=lo; i<hi;i++ ) {
                savedBits[i-DLGMSG1] = fl;
                CheckDlgButton( hwnd, i, fl );
            }
            break;
        case DLGMSG_NEXT:
            EndDialog( hwnd, which+1 );
            break;
        case DLGMSG_PREV:
            EndDialog( hwnd, which-1 );
            break;
        case IDCANCEL:
            EndDialog( hwnd, -1 );
            break;
        case IDOK:
            EndDialog( hwnd, 0 );
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* SysMsgDialog */

/*
 * DoSpyMsgDialog
 */
void DoSpyMsgDialog( HWND hwnd, int which )
{
    FARPROC     fp;
    char        name[80];
    char        *savewatch;

    savewatch = CloneBitState( savedBits );
    while( 1 ) {
        sprintf( name,"SPYMSGS%d", which );
        fp = MakeProcInstance( (FARPROC) SpyMsgDialog, Instance );
        which = JDialogBoxParam( ResInstance, name, hwnd, (LPVOID) fp, which );
        FreeProcInstance( fp );
        if( which <= 0 ) {
            break;
        }
    }
    if( which == -1 ) {
        CopyBitState( savedBits, savewatch );
    }
    FreeBitState( savewatch );

} /* DoSpyMsgDialog */

/*
 * MessageDialog - process messages required dialog
 */
BOOL CALLBACK MessageDialog( HWND hwnd, int msg, UINT wparam, DWORD lparam )
{
    int         i;
    char        fl;
    WORD        cmdid;
    char        *rcstr;
    char        buf[100];

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        for( i=SPYMSG_CLIPBOARD; i<= SPYMSG_WINDOW; i++ ) {
            fl = Filters.array[i-SPYMSG_CLIPBOARD].flag[ currBit ];
            CheckDlgButton( hwnd, i, fl );
        }
        if( currBit == M_WATCH ) {
            rcstr = GetRCString( STR_MSGS_TO_WATCH );
        } else {
            rcstr = GetRCString( STR_MSGS_TO_STOP );
        }
        SetWindowText( hwnd, rcstr );
        rcstr = GetRCString( STR_MSG_RANGE_STR );
        sprintf( buf, rcstr, MSG_RANGE_0_FIRST, MSG_RANGE_0_LAST );
        SetDlgItemText( hwnd, SPYMSG_RANGE_0, buf );

        sprintf( buf, rcstr, MSG_RANGE_1_FIRST, MSG_RANGE_1_LAST );
        SetDlgItemText( hwnd, SPYMSG_RANGE_1, buf );

        sprintf( buf, rcstr, MSG_RANGE_2_FIRST, MSG_RANGE_2_LAST );
        SetDlgItemText( hwnd, SPYMSG_RANGE_2, buf );

        sprintf( buf, rcstr, MSG_RANGE_3_FIRST, MSG_RANGE_3_LAST );
        SetDlgItemText( hwnd, SPYMSG_RANGE_3, buf );
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        Ctl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( cmdid >= SPYMSG_CLIPBOARD && cmdid <= SPYMSG_WINDOW ) {
            i = cmdid - SPYMSG_CLIPBOARD;
            fl = Filters.array[i].flag[ currBit ];
            if( fl ) {
                fl = FALSE;
            } else {
                fl = TRUE;
            }
            CheckDlgButton( hwnd, cmdid, fl );
            Filters.array[i].flag[ currBit ] = fl;
            SetFilterSaveBitsMsgs( Filters.array[i].type, fl, savedBits );
            break;
        }
        switch( cmdid ) {
        case SPYMSG_ALLCLEAR:
        case SPYMSG_ALLSET:
            fl = (cmdid == SPYMSG_ALLSET);
            for( i=DLGMSG1;i<DLGMSG4END;i++ ) {
                savedBits[i-DLGMSG1] = fl;
            }
            for( i=SPYMSG_CLIPBOARD;i<=SPYMSG_WINDOW;i++ ) {
                Filters.array[i-SPYMSG_CLIPBOARD].flag[ currBit ] = fl;
                CheckDlgButton( hwnd, i, fl );
            }
            break;
        case IDOK:
            EndDialog( hwnd, 0 );
            break;
        case IDCANCEL:
            EndDialog( hwnd, 1 );
            break;
        case SPYMSG_DLG1:
        case SPYMSG_DLG2:
        case SPYMSG_DLG3:
        case SPYMSG_DLG4:
            DoSpyMsgDialog( hwnd, cmdid - SPYMSG_DLG1 + 1 );
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* MessageDialog */

/*
 * DoMessageDialog
 */
void DoMessageDialog( HWND hwnd, WORD cmdid )
{
    FARPROC     fp;
    int         rc;
    char        filts[SPYMSG_WINDOW-SPYMSG_CLIPBOARD+2];
    int         i;

    if( cmdid == SPY_MESSAGES_WATCH ) {
        currBit = M_WATCH;
    } else {
        currBit = M_STOPON;
    }
    for( i=SPYMSG_CLIPBOARD; i<= SPYMSG_WINDOW; i++ ) {
        filts[i-SPYMSG_CLIPBOARD] = Filters.array[i-SPYMSG_CLIPBOARD].flag[ currBit ];
    }
    savedBits = SaveBitState( currBit );
    fp = MakeProcInstance( (FARPROC) MessageDialog, Instance );
    rc = JDialogBox( ResInstance, "SPYMSGDIALOG", hwnd, (LPVOID) fp );
    if( rc ) {
        for( i=SPYMSG_CLIPBOARD; i<= SPYMSG_WINDOW; i++ ) {
            Filters.array[i-SPYMSG_CLIPBOARD].flag[ currBit ] = filts[i-SPYMSG_CLIPBOARD];
        }
    } else {
        RestoreBitState( savedBits, currBit );
    }
    FreeProcInstance( fp );

} /* DoMessageDialog */

message         *msgPtr;
char            oldBits[2];
DWORD           oldCount;
HWND            currHwnd;
BOOL            doHilite;

#ifdef _M_IX86
extern LPSTR GetPointer( DWORD );
#ifdef __NT__
#pragma aux GetPointer = parm[eax] value[eax];
#else
#pragma aux GetPointer = parm[dx ax] value[dx ax];
#endif
#else
#define GetPointer( dword ) ( (LPSTR)dword )
#endif

/*
 * WindowIsDestroyed - the window that recieved this message has been
 *                      destroyed.  Set up the dialog appropriately
 */

static void WindowIsDestroyed( HWND hwnd ) {

    char        hwndstring[30];
    char        buf[100];
    char        *fmtstr;

    EnableWindow( GetDlgItem( hwnd, MSGSEL_SHOWINFO ), FALSE );
    EnableWindow( GetDlgItem( hwnd, MSGSEL_HILIGHT ), FALSE );
    GetDlgItemText( hwnd, MSGSEL_WINDOWID, hwndstring, 30 );
    fmtstr = GetRCString( STR_DESTROYED );
    sprintf( buf, fmtstr, hwndstring );
    SetDlgItemText( hwnd, MSGSEL_WINDOWID, buf );
}

static void setMessageName( HWND hwnd, char *str ) {
    char        buf[128];

    RCsprintf( buf, STR_MESSAGE, str );
    SetDlgItemText( hwnd, MSGSEL_NAME, buf );
}

/*
 * MessageSelectDialog - a single message item selected
 */
BOOL CALLBACK MessageSelectDialog( HWND hwnd, int msg, UINT wparam, DWORD lparam )
{
    int         i;
    WORD        id;
    WORD        cmdid;
    LPSTR       ptr;
    char        str[256];
    char        tmp[20];
    char        *endptr;
    char        *warnmsg;
    char        *strptr;

    switch( msg ) {
    case WM_INITDIALOG:
        doHilite = FALSE;
        i = 0;
        ptr = GetPointer( lparam );
        strcpy( str, ptr );
        while( str[i] != ' ' ) {
            i++;
        }
        str[i] = 0;
        strptr = MemAlloc( strlen( str ) + 1 );
        strcpy( strptr, str );
        SetWindowLong( hwnd, DWL_USER, (DWORD)strptr );
        setMessageName( hwnd, str );
        str[SPYOUT_MSG+SPYOUT_MSG_LEN] = 0;
        id = strtol( &str[SPYOUT_MSG], &endptr, 16 );
        if( endptr != str + SPYOUT_MSG + SPYOUT_MSG_LEN ) {
            EndDialog( hwnd, 0 );
            break;
        }
        currHwnd = (HWND) strtol( &str[SPYOUT_HWND], &endptr, 16 );
        if( endptr != str + SPYOUT_MSG - 1 ) {
            EndDialog( hwnd, 0 );
            break;
        }
        msgPtr = GetMessageDataFromID( id );
        if( msgPtr == NULL ) {
            EndDialog( hwnd, 0 );
            break;
        }
        oldBits[M_WATCH] = msgPtr->bits[M_WATCH];
        oldBits[M_STOPON] = msgPtr->bits[M_STOPON];
        oldCount = msgPtr->count;
        if( oldBits[M_WATCH] ) {
            CheckDlgButton( hwnd, MSGSEL_WATCH, 1 );
        }
        if( oldBits[M_STOPON] ) {
            CheckDlgButton( hwnd, MSGSEL_STOPON, 1 );
        }
        ltoa( msgPtr->count, tmp, 10 );
        SetDlgItemText( hwnd, MSGSEL_COUNT, tmp );
        sprintf( tmp, "%0*lX", SPYOUT_HWND_LEN, (DWORD)currHwnd );
        SetDlgItemText( hwnd, MSGSEL_WINDOWID, tmp );
        /* make sure windows hasn't reallocated the handle to us */
        if( !IsWindow( currHwnd ) || currHwnd == hwnd ) {
            WindowIsDestroyed( hwnd );
        }
        break;
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        switch( cmdid ) {
        case MSGSEL_HELP:
            strptr = (char *) GetWindowLong( hwnd, DWL_USER );
#ifdef __NT__
            WWinHelp( hwnd, "win32sdk.hlp", HELP_KEY, (LPARAM) strptr );
#else
            WWinHelp( hwnd, "win31wh.hlp", HELP_KEY, (LPARAM) strptr );
#endif
            break;
        case MSGSEL_SHOWINFO:
            if( IsWindow( currHwnd ) ) {
                ShowFramedInfo( hwnd, currHwnd );
            } else {
                warnmsg = GetRCString( STR_WIN_DOESNT_EXIST );
                MessageBox( hwnd, warnmsg, SpyName, MB_OK );
                WindowIsDestroyed( hwnd );
            }
            break;
        case MSGSEL_HILIGHT:
            if( IsWindow( currHwnd ) ) {
                FrameAWindow( currHwnd, FALSE );
                doHilite = !doHilite;
            } else {
                warnmsg = GetRCString( STR_WIN_DOESNT_EXIST );
                MessageBox( hwnd, warnmsg, SpyName, MB_OK );
                WindowIsDestroyed( hwnd );
            }
            break;
        case MSGSEL_CLEARCOUNT:
            msgPtr->count = 0;
            tmp[0] = '0';
            tmp[1] = 0;
            SetDlgItemText( hwnd, MSGSEL_COUNT, tmp );
            break;
        case MSGSEL_WATCH:
        case MSGSEL_STOPON:
            if( cmdid == MSGSEL_WATCH ) {
                i = M_WATCH;
            } else {
                i = M_STOPON;
            }
            if( msgPtr->bits[i] ) {
                msgPtr->bits[i] = 0;
            } else {
                msgPtr->bits[i] = 1;
            }
            CheckDlgButton( hwnd, wparam, msgPtr->bits[i] );
            break;
        case IDOK:
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDCANCEL:
            msgPtr->bits[M_WATCH] = oldBits[M_WATCH];
            msgPtr->bits[M_STOPON] = oldBits[M_STOPON];
            msgPtr->count = oldCount;
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;
    case WM_CLOSE:
        if( doHilite ) {
            FrameAWindow( currHwnd, FALSE );
        }
        EndDialog( hwnd, 0 );
        break;
    case WM_DESTROY:
        strptr = (char *) GetWindowLong( hwnd, DWL_USER );
        MemFree( strptr );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* MessageSelectDialog */

/*
 * DoMessageSelDialog
 */
void DoMessageSelDialog( HWND hwnd )
{
    FARPROC     fp;
    char        str[80];
    LRESULT     sel;

    sel = SendMessage( SpyListBox, LB_GETCURSEL, 0, 0L );
    if( sel == (WORD)LB_ERR ) {
        return;
    }
    SendMessage( SpyListBox, LB_GETTEXT, sel, (DWORD) (LPSTR) str );
    fp = MakeProcInstance( (FARPROC) MessageSelectDialog, Instance );
    JDialogBoxParam( ResInstance, "MSGSELECT", hwnd, (LPVOID) fp, (DWORD) (LPSTR) str );
    FreeProcInstance( fp );

} /* DoMessageSelDialog */
