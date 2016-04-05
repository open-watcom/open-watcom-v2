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
* Description:  Spy message dialogs implementation.
*
****************************************************************************/


#include "bool.h"
#include "spy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dde.h>
#include "wwinhelp.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK SpyMsgDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT BOOL CALLBACK MessageDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT BOOL CALLBACK MessageSelectDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static int      currBit;
static bool     *savedBits;

#define SDM_SETPAGE (WM_USER + 1)

/*
 * SpyMsgDialog - process message range dialogs
 */
BOOL CALLBACK SpyMsgDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i, j, k, max;
    static int  which, firstmsg, pages;
    bool        fl;
    HWND        nwnd, pwnd;
    ctl_id      cmdid;
    char        *fmtstr;
    char        title[100];

    switch( msg ) {
    case WM_INITDIALOG:
        SendMessage( hwnd, SDM_SETPAGE, 0, lparam );
        break;
    case SDM_SETPAGE:
        which = lparam;
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
        EnableWindow( pwnd, which > 1 );
        EnableWindow( nwnd, which < pages );
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
        for( j = 0, k = firstmsg; j < ClassMessagesSize; j++ ) {
            if( k < ClassMessages[j].message_array_size ) {
                break;
            }
            k -= ClassMessages[j].message_array_size;
        }
        for( i = 0; i < max; i++ ) {
            if( k + i >= ClassMessages[j].message_array_size ) {
                k -= ClassMessages[j].message_array_size;
                ++j;
            }
            SetDlgItemText( hwnd, DLGMSG1 + i, ClassMessages[j].message_array[k + i].str );
            CheckDlgButton( hwnd, DLGMSG1 + i, ( savedBits[firstmsg + i] ) ? BST_CHECKED : BST_UNCHECKED );
        }
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( cmdid >= DLGMSG1 && cmdid < DLGMSG1 + NUM_DLGMSGS ) {
            fl = (savedBits[cmdid - DLGMSG1 + firstmsg]) ? FALSE : TRUE;
            savedBits[cmdid - DLGMSG1 + firstmsg] = fl;
            CheckDlgButton( hwnd, cmdid, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
            break;
        }
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
static void DoSpyMsgDialog( HWND hwnd, int which )
{
    FARPROC     fp;
    bool        *savewatch;
    INT_PTR     rc;

    savewatch = CloneBitState( savedBits );
    fp = MakeProcInstance( (FARPROC)SpyMsgDialog, Instance );
    rc = JDialogBoxParam( ResInstance, "SPYMSGS", hwnd, (DLGPROC)fp, which );
    FreeProcInstance( fp );
    if( rc == -1 ) {
        CopyBitState( savedBits, savewatch );
    }
    FreeBitState( savewatch );

} /* DoSpyMsgDialog */

/*
 * MessageDialog - process messages required dialog
 */
BOOL CALLBACK MessageDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i;
    bool        fl;
    ctl_id      cmdid;
    char        *rcstr;
    int         id;

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        for( id = SPYMSG_CLIPBOARD; id <= SPYMSG_CONTROLS; id++ ) {
            i = id - SPYMSG_CLIPBOARD;
            fl = Filters[i].flag[currBit];
            CheckDlgButton( hwnd, id, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
        }
        if( currBit == M_WATCH ) {
            rcstr = GetRCString( STR_MSGS_TO_WATCH );
        } else {
            rcstr = GetRCString( STR_MSGS_TO_STOP );
        }
        SetWindowText( hwnd, rcstr );
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( cmdid >= SPYMSG_CLIPBOARD && cmdid <= SPYMSG_CONTROLS ) {
            i = cmdid - SPYMSG_CLIPBOARD;
            fl = (Filters[i].flag[currBit]) ? FALSE : TRUE;
            CheckDlgButton( hwnd, cmdid, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
            Filters[i].flag[currBit] = fl;
            SetFilterSaveBitsMsgs( i, fl, savedBits );
            break;
        }
        switch( cmdid ) {
        case SPYMSG_ALLCLEAR:
        case SPYMSG_ALLSET:
            fl = (cmdid == SPYMSG_ALLSET);
            for( id = SPYMSG_CLIPBOARD; id <= SPYMSG_CONTROLS; id++ ) {
                i = id - SPYMSG_CLIPBOARD;
                Filters[i].flag[currBit] = fl;
                CheckDlgButton( hwnd, id, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
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
void DoMessageDialog( HWND hwnd, ctl_id cmdid )
{
    FARPROC     fp;
    INT_PTR     rc;
    bool        filts[SPYMSG_CONTROLS - SPYMSG_CLIPBOARD + 1];
    int         i;
    int         id;

    if( cmdid == SPY_MESSAGES_WATCH ) {
        currBit = M_WATCH;
    } else {
        currBit = M_STOPON;
    }
    for( id = SPYMSG_CLIPBOARD; id <= SPYMSG_CONTROLS; id++ ) {
        i = id - SPYMSG_CLIPBOARD;
        filts[i] = Filters[i].flag[currBit];
    }
    savedBits = SaveBitState( currBit );
    fp = MakeProcInstance( (FARPROC)MessageDialog, Instance );
    rc = JDialogBox( ResInstance, "SPYMSGDIALOG", hwnd, (DLGPROC)fp );
    if( rc ) {
        for( id = SPYMSG_CLIPBOARD; id <= SPYMSG_CONTROLS; id++ ) {
            i = id - SPYMSG_CLIPBOARD;
            Filters[i].flag[currBit] = filts[i];
        }
    } else {
        RestoreBitState( savedBits, currBit );
    }
    FreeProcInstance( fp );

} /* DoMessageDialog */

message         *msgPtr;
bool            oldBits[2];
DWORD           oldCount;
HWND            currHwnd;
BOOL            doHilite;

#ifdef _M_IX86
extern LPSTR GetPointer( LPARAM );
#ifdef __NT__
#pragma aux GetPointer = parm[eax] value[eax];
#else
#pragma aux GetPointer = parm[dx ax] value[dx ax];
#endif
#else
#define GetPointer( lparam ) ((LPSTR)lparam)
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
BOOL CALLBACK MessageSelectDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i;
    WORD        id;
    ctl_id      cmdid;
    LPSTR       ptr;
    char        str[256];
    char        tmp[20];
    char        class_name[80];
    char        *endptr;
    char        *warnmsg;
    char        *strptr;
    bool        fl;

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
        SET_DLGDATA( hwnd, strptr );
        setMessageName( hwnd, str );
        str[SPYOUT_MSG + SPYOUT_MSG_LEN] = 0;
        id = strtol( &str[SPYOUT_MSG], &endptr, 16 );
        if( endptr != str + SPYOUT_MSG + SPYOUT_MSG_LEN ) {
            EndDialog( hwnd, 0 );
            break;
        }
        currHwnd = (HWND)(pointer_int)strtol( &str[SPYOUT_HWND], &endptr, 16 );
        if( endptr != str + SPYOUT_MSG - 1 ) {
            EndDialog( hwnd, 0 );
            break;
        }
        GetClassName( currHwnd, class_name, 80 );
        msgPtr = GetMessageDataFromID( id, class_name );
        if( msgPtr == NULL ) {
            EndDialog( hwnd, 0 );
            break;
        }
        oldBits[M_WATCH] = msgPtr->bits[M_WATCH];
        oldBits[M_STOPON] = msgPtr->bits[M_STOPON];
        oldCount = msgPtr->count;
        if( oldBits[M_WATCH] ) {
            CheckDlgButton( hwnd, MSGSEL_WATCH, BST_CHECKED );
        }
        if( oldBits[M_STOPON] ) {
            CheckDlgButton( hwnd, MSGSEL_STOPON, BST_CHECKED );
        }
        ltoa( msgPtr->count, tmp, 10 );
        SetDlgItemText( hwnd, MSGSEL_COUNT, tmp );
        sprintf( tmp, "%0*lX", SPYOUT_HWND_LEN, (DWORD)(pointer_int)currHwnd );
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
            strptr = (char *)GET_DLGDATA( hwnd );
#ifdef __NT__
            WWinHelp( hwnd, "win32sdk.hlp", HELP_KEY, (LPARAM)strptr );
#else
            WWinHelp( hwnd, "win31wh.hlp", HELP_KEY, (LPARAM)strptr );
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
            fl = ( msgPtr->bits[i] ) ? FALSE : TRUE;
            msgPtr->bits[i] = fl;
            CheckDlgButton( hwnd, wparam, ( fl ) ? BST_CHECKED : BST_UNCHECKED );
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
            FrameAWindow( currHwnd );
        }
        EndDialog( hwnd, 0 );
        break;
    case WM_DESTROY:
        strptr = (char *)GET_DLGDATA( hwnd );
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

    if( !GetSpyBoxSelection( str ) ) {
        return;
    }
    fp = MakeProcInstance( (FARPROC)MessageSelectDialog, Instance );
    JDialogBoxParam( ResInstance, "MSGSELECT", hwnd, (DLGPROC)fp, (LPARAM)(LPSTR)str );
    FreeProcInstance( fp );

} /* DoMessageSelDialog */

