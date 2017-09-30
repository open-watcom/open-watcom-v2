/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Spy message dialogs implementation.
*
****************************************************************************/


#include "spy.h"
#include <dde.h>
#include "wclbproc.h"
#include "wwinhelp.h"


#define SDM_SETPAGE (WM_USER + 1)

/* Local Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK SpyMsgDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT INT_PTR CALLBACK MessageDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT INT_PTR CALLBACK MessageSelectDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

message         *msgPtr;
bool            old_watch;
bool            old_stopon;
DWORD           old_count;
HWND            currHwnd;

static bool     doHilite = false;
static bool     is_watch;
static bool     *savedBits;

/*
 * SpyMsgDialogDlgProc - process message range dialogs
 */
INT_PTR CALLBACK SpyMsgDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        i;
    WORD        j;
    WORD        k;
    WORD        max;
    static WORD which;
    static WORD firstmsg;
    static WORD pages;
    bool        fl;
    HWND        nwnd, pwnd;
    ctl_id      cmdid;
    const char  *fmtstr;
    char        title[100];
    bool        ret;

    ret = false;

    switch( msg ) {
    case WM_INITDIALOG:
        SendMessage( hwnd, SDM_SETPAGE, 0, lparam );
        ret = true;
        break;
    case SDM_SETPAGE:
        which = (WORD)lparam;
        firstmsg = (which - 1) * NUM_DLGMSGS;
        pages = TotalMessageArraySize / NUM_DLGMSGS;
        if( TotalMessageArraySize % NUM_DLGMSGS > 0 ) {
            pages++;
        }
        fmtstr = GetRCString( STR_IMC_TITLE );
        sprintf( title, fmtstr, which, pages );
        SetWindowText( hwnd, title );
        nwnd = GetDlgItem( hwnd, DLGMSG_NEXT );
        pwnd = GetDlgItem( hwnd, DLGMSG_PREV );
        EnableWindow( pwnd, ( which > 1 ) ? TRUE : FALSE );
        EnableWindow( nwnd, ( which < pages ) ? TRUE : FALSE );
        if( which == pages ) {
            max = TotalMessageArraySize % NUM_DLGMSGS;
        } else {
            max = NUM_DLGMSGS;
        }
        if( which == pages ) {
            for( i = max; i < NUM_DLGMSGS; i++ ) {
                ShowWindow( GetDlgItem( hwnd, DLGMSG1 + i ), SW_HIDE );
            }
        } else {
            for( i = 0; i < NUM_DLGMSGS; i++ ) {
                ShowWindow( GetDlgItem( hwnd, DLGMSG1 + i ), SW_SHOW );
            }
        }
        k = firstmsg;
        for( j = 0; j < ClassMessagesSize; j++ ) {
            if( k < ClassMessages[j].message_array_size ) {
                break;
            }
            k -= ClassMessages[j].message_array_size;
        }
        for( i = 0; i < max; i++ ) {
            if( k >= ClassMessages[j].message_array_size ) {
                k -= ClassMessages[j].message_array_size;
                ++j;
            }
            SetDlgItemText( hwnd, DLGMSG1 + i, ClassMessages[j].message_array[k].str );
            CheckDlgButton( hwnd, DLGMSG1 + i, ( savedBits[firstmsg + i] ) ? BST_CHECKED : BST_UNCHECKED );
            k++;
        }
        ret = true;
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        ret = true;
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( cmdid >= DLGMSG1 && cmdid < DLGMSG1 + NUM_DLGMSGS ) {
            fl = (savedBits[cmdid - DLGMSG1 + firstmsg]) ? FALSE : TRUE;
            savedBits[cmdid - DLGMSG1 + firstmsg] = fl;
            CheckDlgButton( hwnd, cmdid, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
        } else {
            switch( cmdid ) {
            case DLGMSG_CLEARALL:
            case DLGMSG_SETALL:
                fl = (cmdid == DLGMSG_SETALL);
                if( which == pages ) {
                    max = TotalMessageArraySize % NUM_DLGMSGS;
                } else {
                    max = NUM_DLGMSGS;
                }
                for( i = 0; i < max; i++ ) {
                    savedBits[firstmsg + i] = fl;
                    CheckDlgButton( hwnd, DLGMSG1 + i, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
                }
                break;
            case DLGMSG_NEXT:
                SendMessage( hwnd, SDM_SETPAGE, 0, which + 1 );
                break;
            case DLGMSG_PREV:
                SendMessage( hwnd, SDM_SETPAGE, 0, which - 1 );
                break;
            case IDCANCEL:
                EndDialog( hwnd, -1 );
                break;
            case IDOK:
                EndDialog( hwnd, 0 );
                break;
            }
        }
        ret = true;
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        ret = true;
        break;
    }
    return( ret );

} /* SpyMsgDialogDlgProc */

/*
 * DoSpyMsgDialog
 */
static void DoSpyMsgDialog( HWND hwnd, int which )
{
    DLGPROC     dlgproc;
    bool        *savewatch;
    INT_PTR     rc;

    savewatch = CloneBitState( savedBits );
    dlgproc = MakeProcInstance_DLG( SpyMsgDialogDlgProc, Instance );
    rc = JDialogBoxParam( ResInstance, "SPYMSGS", hwnd, dlgproc, which );
    FreeProcInstance_DLG( dlgproc );
    if( rc == -1 ) {
        CopyBitState( savedBits, savewatch );
    }
    FreeBitState( savewatch );

} /* DoSpyMsgDialog */

/*
 * MessageDialogDlgProc - process messages required dialog
 */
INT_PTR CALLBACK MessageDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i;
    bool        fl;
    ctl_id      cmdid;
    const char  *rcstr;
    bool        ret;

    lparam = lparam;

    ret = false;

    switch( msg ) {
    case WM_INITDIALOG:
        for( i = 0; i < FILTER_ENTRIES; i++ ) {
            if( is_watch ) {
                fl = Filters[i].watch;
            } else {
                fl = Filters[i].stopon;
            }
            SetDlgItemText( hwnd, Filters[i].id, (LPCSTR)Filters[i].str );
            CheckDlgButton( hwnd, Filters[i].id, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
        }
        if( is_watch ) {
            rcstr = GetRCString( STR_MSGS_TO_WATCH );
        } else {
            rcstr = GetRCString( STR_MSGS_TO_STOP );
        }
        SetWindowText( hwnd, rcstr );
        ret = true;
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        ret = true;
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        switch( cmdid ) {
        case SPYMSG_ALLCLEAR:
        case SPYMSG_ALLSET:
            fl = (cmdid == SPYMSG_ALLSET);
            for( i = 0; i < FILTER_ENTRIES; i++ ) {
                if( is_watch ) {
                    Filters[i].watch = fl;
                } else {
                    Filters[i].stopon = fl;
                }
                CheckDlgButton( hwnd, Filters[i].id, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
                SetFilterSaveBitsMsgs( i, fl, savedBits );
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
        default:
            for( i = 0; i < FILTER_ENTRIES; i++ ) {
                if( cmdid == Filters[i].id ) {
                    if( is_watch ) {
                        Filters[i].watch = fl = !Filters[i].watch;
                    } else {
                        Filters[i].stopon = fl = !Filters[i].stopon;
                    }
                    CheckDlgButton( hwnd, cmdid, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
                    SetFilterSaveBitsMsgs( i, fl, savedBits );
                    break;
                }
            }
            break;
        }
        ret = true;
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        ret = true;
        break;
    }
    return( ret );

} /* MessageDialogDlgProc */

/*
 * DoMessageDialog
 */
void DoMessageDialog( HWND hwnd, ctl_id cmdid )
{
    DLGPROC     dlgproc;
    INT_PTR     rc;
    bool        filts[FILTER_ENTRIES];
    int         i;

    is_watch = ( cmdid == SPY_MESSAGES_WATCH );
    for( i = 0; i < FILTER_ENTRIES; i++ ) {
        if( is_watch ) {
            filts[i] = Filters[i].watch;
        } else {
            filts[i] = Filters[i].stopon;
        }
    }
    savedBits = SaveBitState( is_watch );
    dlgproc = MakeProcInstance_DLG( MessageDialogDlgProc, Instance );
    rc = JDialogBox( ResInstance, "SPYMSGDIALOG", hwnd, dlgproc );
    if( rc ) {
        for( i = 0; i < FILTER_ENTRIES; i++ ) {
            if( is_watch ) {
                Filters[i].watch = filts[i];
            } else {
                Filters[i].stopon = filts[i];
            }
        }
    } else {
        RestoreBitState( savedBits, is_watch );
    }
    FreeProcInstance_DLG( dlgproc );

} /* DoMessageDialog */

/*
 * WindowIsDestroyed - the window that recieved this message has been
 *                      destroyed.  Set up the dialog appropriately
 */

static void WindowIsDestroyed( HWND hwnd )
{
    char        hwndstring[30];
    char        buf[100];
    const char  *fmtstr;

    EnableWindow( GetDlgItem( hwnd, MSGSEL_SHOWINFO ), FALSE );
    EnableWindow( GetDlgItem( hwnd, MSGSEL_HILIGHT ), FALSE );
    GetDlgItemText( hwnd, MSGSEL_WINDOWID, hwndstring, 30 );
    fmtstr = GetRCString( STR_DESTROYED );
    sprintf( buf, fmtstr, hwndstring );
    SetDlgItemText( hwnd, MSGSEL_WINDOWID, buf );
}

static void setMessageName( HWND hwnd, char *str )
{
    char        buf[128];

    RCsprintf( buf, STR_MESSAGE, str );
    SetDlgItemText( hwnd, MSGSEL_NAME, buf );
}

/*
 * MessageSelectDialogDlgProc - a single message item selected
 */
INT_PTR CALLBACK MessageSelectDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i;
    UINT        id;
    ctl_id      cmdid;
    LPSTR       ptr;
    char        str[256];
    char        tmp[20];
    char        class_name[80];
    char        *endptr;
    const char  *warnmsg;
    char        *strptr;
    bool        fl;
    bool        ret;

    ret = false;

    switch( msg ) {
    case WM_INITDIALOG:
        doHilite = false;
        ptr = (LPSTR)(ULONG_PTR)lparam;
        strcpy( str, ptr );
        i = 0;
        while( str[i] != ' ' ) {
            i++;
        }
        str[i] = '\0';
        strptr = MemAlloc( strlen( str ) + 1 );
        strcpy( strptr, str );
        SET_DLGDATA( hwnd, strptr );
        setMessageName( hwnd, str );
        str[SPYOUT_MSG + SPYOUT_MSG_LEN] = '\0';
        id = strtoul( str + SPYOUT_MSG, &endptr, 16 );
        if( endptr != str + SPYOUT_MSG + SPYOUT_MSG_LEN ) {
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        }
#ifdef _WIN64
        str[SPYOUT_HWND + SPYOUT_HWND_LEN] = '\0';
        currHwnd = (HWND)(ULONG_PTR)strtoul( str + SPYOUT_HWND + SPYOUT_HWND_LEN/2, &endptr, 16 );
        if( endptr != str + SPYOUT_HWND + SPYOUT_HWND_LEN ) {
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        }
        str[SPYOUT_HWND + SPYOUT_HWND_LEN/2] = '\0';
        currHwnd += ((ULONG_PTR)strtoul( str + SPYOUT_HWND, &endptr, 16 )) << 32;
        if( endptr != str + SPYOUT_HWND + SPYOUT_HWND_LEN/2 ) {
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        }
#else
        currHwnd = (HWND)(UINT_PTR)strtoul( str + SPYOUT_HWND, &endptr, 16 );
        if( endptr != str + SPYOUT_HWND + SPYOUT_HWND_LEN ) {
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        }
#endif
        i = GetClassName( currHwnd, class_name, sizeof( class_name ) );
        class_name[i] = '\0';
        msgPtr = GetMessageDataFromID( id, class_name );
        if( msgPtr == NULL ) {
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        }
        old_watch = msgPtr->watch;
        old_stopon = msgPtr->stopon;
        old_count = msgPtr->count;
        if( old_watch ) {
            CheckDlgButton( hwnd, MSGSEL_WATCH, BST_CHECKED );
        }
        if( old_stopon ) {
            CheckDlgButton( hwnd, MSGSEL_STOPON, BST_CHECKED );
        }
        ltoa( msgPtr->count, tmp, 10 );
        SetDlgItemText( hwnd, MSGSEL_COUNT, tmp );
        GetHexStr( tmp, (UINT_PTR)currHwnd, SPYOUT_HWND_LEN );
        tmp[SPYOUT_HWND_LEN] = '\0';
        SetDlgItemText( hwnd, MSGSEL_WINDOWID, tmp );
        /* make sure windows hasn't reallocated the handle to us */
        if( !IsWindow( currHwnd ) || currHwnd == hwnd ) {
            WindowIsDestroyed( hwnd );
        }
        ret = true;
        break;
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        switch( cmdid ) {
        case MSGSEL_HELP:
#ifdef __WINDOWS__
            WWinHelp( hwnd, "win31wh.hlp", HELP_KEY, (LPARAM)GET_DLGDATA( hwnd ) );
#else
            WWinHelp( hwnd, "win32sdk.hlp", HELP_KEY, (LPARAM)GET_DLGDATA( hwnd ) );
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
                FrameAWindow( currHwnd );
                doHilite = !doHilite;
            } else {
                warnmsg = GetRCString( STR_WIN_DOESNT_EXIST );
                MessageBox( hwnd, warnmsg, SpyName, MB_OK );
                WindowIsDestroyed( hwnd );
            }
            break;
        case MSGSEL_CLEARCOUNT:
            msgPtr->count = 0;
            SetDlgItemText( hwnd, MSGSEL_COUNT, "0" );
            break;
        case MSGSEL_WATCH:
            msgPtr->watch = fl = !msgPtr->watch;
            CheckDlgButton( hwnd, (int)wparam, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
            break;
        case MSGSEL_STOPON:
            msgPtr->stopon = fl = !msgPtr->stopon;
            CheckDlgButton( hwnd, (int)wparam, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
            break;
        case IDOK:
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDCANCEL:
            msgPtr->watch = old_watch;
            msgPtr->stopon = old_stopon;
            msgPtr->count = old_count;
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        ret = true;
        break;
    case WM_CLOSE:
        if( doHilite ) {
            FrameAWindow( currHwnd );
        }
        EndDialog( hwnd, 0 );
        ret = true;
        break;
    case WM_DESTROY:
        strptr = (char *)GET_DLGDATA( hwnd );
        MemFree( strptr );
        ret = true;
        break;
    }
    return( ret );

} /* MessageSelectDialogDlgProc */

/*
 * DoMessageSelDialog
 */
void DoMessageSelDialog( HWND hwnd )
{
    DLGPROC     dlgproc;
    char        str[SPYOUT_LENGTH + 1];

    if( !GetSpyBoxSelection( str ) ) {
        return;
    }
    dlgproc = MakeProcInstance_DLG( MessageSelectDialogDlgProc, Instance );
    JDialogBoxParam( ResInstance, "MSGSELECT", hwnd, dlgproc, (LPARAM)(LPSTR)str );
    FreeProcInstance_DLG( dlgproc );

} /* DoMessageSelDialog */
