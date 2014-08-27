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


#include "wdeglbl.h"
#include <stdio.h>
#include <ctype.h>
#include "fmedit.def"
#include "wdecust.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdemain.h"
#include "wdefont.h"
#include "wdegetfn.h"
#include "wdestat.h"
#include "wdelist.h"
#include "wdedebug.h"
#include "wde_rc.h"
#include "jdlg.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_NUM_CUSTOMS         2
#define WDE_CHECK_WHICH( w )    (((w < 0) || (w >= WDE_NUM_CUSTOMS)) ? FALSE : TRUE)
#define WDE_PREVIEW_PAD         10

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WdeSelectCustProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static BOOL         WdeLoadMSCustomControls( WdeCustLib * );
static WdeCustLib   *WdeAllocCustLib( void );
static BOOL         WdeFreeCustLib( WdeCustLib * );
static void         WdeFreeSelectWinCBox( HWND );
static bool         WdeSetSelectWinCBox( HWND, WdeCustLib * );
static bool         WdeSetSelectWin( HWND );
static bool         WdeSetCurrentControl( HWND, int );
static bool         WdePreviewSelected( HWND );

/****************************************************************************/
/* extern variables                                                         */
/****************************************************************************/
extern char     *WdeCustOpenTitle;
extern char     *WdeCustFilter;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HFONT    WdeSelectFont       = NULL;
static LIST     *WdeCustomLibList   = NULL;
static LIST     *WdeLibList         = NULL;

WdeCurrCustControl WdeCurrControl[WDE_NUM_CUSTOMS]  = {
    { NULL, 0 },
    { NULL, 0 }
};

bool WdeIsBorBtnIDSupported( uint_16 id )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( id );

    return( FALSE );
}

void WdeGetCurrentCustControl( int which, WdeCustLib **lib, UINT *index )
{
    if( !WDE_CHECK_WHICH( which ) ) {
        WdeWriteTrail( "WdeGetCurrentCustControl: bad which!" );
        return;
    }
    *lib = WdeCurrControl[which].lib;
    *index = WdeCurrControl[which].index;
}

bool WdeIsCurrentCustControlSet( int which )
{
    if( WDE_CHECK_WHICH( which ) ) {
        if( WdeCurrControl[which].lib != NULL ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

bool WdeCustControlsLoaded( void )
{
    return( WdeCustomLibList != NULL );
}

bool WdeSetCurrentCustControl( int which )
{
    INT_PTR   ret;
    HINSTANCE inst;
    FARPROC   proc;

    if( WdeCustomLibList == NULL ) {
        WdeSetStatusByID( -1, WDE_NOCUSTLOADED );
        return( TRUE );
    }

    if( !WDE_CHECK_WHICH( which ) ) {
        WdeWriteTrail( "WdeSetCurrentCustControl: bad which!" );
        return( FALSE );
    }

    inst = WdeGetAppInstance();
    proc = MakeProcInstance( (FARPROC)WdeSelectCustProc, inst );
    if( proc == NULL ) {
        WdeWriteTrail( "WdeSetCurrentCustomControl: MakeProcInstance failed!" );
        return( FALSE );
    }
    ret = JDialogBoxParam( inst, "WdeSelectCustom", WdeGetMainWindowHandle(),
                           (DLGPROC)proc, (LPARAM)(LPVOID)&which );
    FreeProcInstance( proc );

    /* if the window could not be created return FALSE */
    if( ret == -1 ) {
        WdeWriteTrail( "WdeSetCurrentCustomControl: Could not create selection window!" );
        return( FALSE );
    }

    return( TRUE );
}

bool WdeLoadCustomLib( bool nt_lib, bool load_only )
{
    char                *name;
    HINSTANCE           inst;
    WdeCustLib          *lib;
    BOOL                ret;
    WdeGetFileStruct    gf;

    gf.file_name = NULL;
    gf.title = WdeCustOpenTitle;
    gf.filter = WdeCustFilter;

    name = WdeGetOpenFileName( &gf );

    if( name == NULL ) {
        return( FALSE );
    }

    inst = LoadLibrary( name );

    if( inst == (HANDLE)NULL ) {
        WdeWriteTrail( "WdeLoadCustomLib: LoadLibrary call failed!" );
        WRMemFree( name );
        return( FALSE );
    }

    lib = WdeAllocCustLib();
    if( lib == NULL ) {
        WdeWriteTrail( "WdeLoadCustomLib: WdeAllocCustLib failed!" );
        WRMemFree( name );
        FreeLibrary( inst );
        return( FALSE );
    }

    lib->inst = inst;
    lib->nt_lib = nt_lib;
    lib->load_only = load_only;
    lib->file_name = name;

    if( load_only ) {
        WdeSetStatusByID( -1, WDE_LIBRARYLOADED );
        ret = TRUE;
    } else {
        if( nt_lib ) {
            ret = WdeLoadMSCustomControls( lib );
        } else {
            ret = TRUE;
        }
    }

    if( !ret ) {
        WdeFreeCustLib( lib );
        return( FALSE );
    }

    if( load_only ) {
        WdeInsertObject( &WdeLibList, (void *)lib );
    } else {
        WdeInsertObject( &WdeCustomLibList, (void *)lib );
    }

    return( TRUE );
}

BOOL WdeLoadMSCustomControls( WdeCustLib *lib )
{
    LPFNCCINFO  info_proc;
    UINT        num_classes;

    info_proc = (LPFNCCINFO)GetProcAddress( lib->inst, "CustomControlInfoA" );

    if( info_proc == NULL ) {
        info_proc = (LPFNCCINFO)GetProcAddress( lib->inst, "CustomControlInfoW" );
        if( info_proc == NULL ) {
            WdeWriteTrail( "WdeLoadMSCustomControls: Info Proc not found!" );
        } else {
            WdeSetStatusByID( -1, WDE_UNICUSTNOTSUPPORTED );
        }
        return( FALSE );
    }

    num_classes = (*info_proc)( NULL );

    if( num_classes == 0 ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: Info Proc returned NULL!" );
        return( FALSE );
    }

    lib->lpcci = (LPCCINFO)WRMemAlloc( sizeof( CCINFO ) * num_classes );
    if( lib->lpcci == NULL ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: LPCCINFO alloc failed!" );
        return( FALSE );
    }
    memset( lib->lpcci, 0, sizeof( CCINFO ) * num_classes );

    lib->num_classes = (*info_proc)( lib->lpcci );

    if( lib->num_classes != num_classes ) {
        WdeWriteTrail( "WdeLoadMSCustomControls: LPCCINFO inconsistent!" );
    }

    return( TRUE );
}

WdeCustLib *WdeAllocCustLib( void )
{
    WdeCustLib  *lib;

    lib = (WdeCustLib *)WRMemAlloc( sizeof( WdeCustLib ) );

    if( lib == NULL ) {
        WdeWriteTrail( "WdeAllocCustLib: WdeCustLib alloc failed!" );
        return( NULL );
    }

    memset( lib, 0, sizeof( WdeCustLib ) );

    WRMemValidate( lib );

    return( lib );
}

bool WdeFreeAllCustLibs( void )
{
    LIST        *llist;
    WdeCustLib  *lib;

    if( WdeCustomLibList != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            lib = (WdeCustLib *)ListElement( llist );
            WdeFreeCustLib( lib );
        }
        ListFree( WdeCustomLibList );
        WdeCustomLibList = NULL;
    }

    if( WdeLibList != NULL ) {
        for( llist = WdeLibList; llist != NULL; llist = ListNext( llist ) ) {
            lib = (WdeCustLib *)ListElement( llist );
            WdeFreeCustLib( lib );
        }
        ListFree( WdeLibList );
        WdeLibList = NULL;
    }

    return( TRUE );
}

BOOL WdeFreeCustLib( WdeCustLib *lib )
{
    if( lib != NULL ) {
        if( lib->file_name != NULL ) {
            WRMemFree( lib->file_name );
        }
        if( lib->inst != NULL ) {
            FreeLibrary( lib->inst );
        }
        if( lib->lpcci != NULL ) {
            WRMemFree( lib->lpcci );
        }
        WRMemFree( lib );
    } else {
        WdeWriteTrail( "WdeFreeCustLib: NULL lib!" );
        return( FALSE );
    }

    return( TRUE );
}

void WdeFindClassInAllCustLibs( char *class, LIST **list )
{
    LIST        *llist;
    WdeCustLib  *lib;
    UINT        i;

    if( list == NULL ) {
        return;
    }

    *list = NULL;

    if( class != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            if( (lib = (WdeCustLib *)ListElement( llist )) != NULL ) {
                for( i = 0; i < lib->num_classes; i++ ) {
                    if( !stricmp( class, lib->lpcci[i].szClass ) ) {
                        ListAddElt( list, (void *)&lib->lpcci[i] );
                    }
                }
            }
        }
    }
}

bool WdeFindLibIndexFromInfo( LPCCINFO lpcci, WdeCustLib **lib, UINT *index )
{
    LIST        *llist;
    WdeCustLib  *l;
    UINT        i;

    if( lib == NULL || index == NULL ) {
        return( FALSE );
    }

    *lib = NULL;
    *index = 0;

    if( lpcci != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            if( (l = (WdeCustLib *)ListElement( llist )) != NULL ) {
                for( i = 0; i < l->num_classes; i++ ) {
                    if( &l->lpcci[i] == lpcci ) {
                        *lib = l;
                        *index = i;
                        return( TRUE );
                    }
                }
            }
        }
    }

    return( FALSE );
}

void WdeFreeSelectWinCBox( HWND win )
{
    HWND                cbox;
    LRESULT             count;
    WdeCurrCustControl  *current;

    cbox = GetDlgItem( win, IDB_CUST_DESC );

    count = SendMessage( cbox, CB_GETCOUNT, 0, 0 );

    if( count > 0 ) {
        do {
            current = (WdeCurrCustControl *)
                SendMessage( cbox, CB_GETITEMDATA, count, 0 );
            if( current != NULL ) {
                WRMemFree( current );
                SendMessage( cbox, CB_SETITEMDATA, count, (LPARAM)NULL );
            }
            count--;
        } while( count != 0 );
    }
}

bool WdeSetSelectWinCBox( HWND cbox, WdeCustLib *lib )
{
    WdeCurrCustControl  *current;
    UINT                i;
    LRESULT             index;

    for( i = 0; i < lib->num_classes; i++ ) {
        current = (WdeCurrCustControl *)WRMemAlloc( sizeof( WdeCurrCustControl ) );
        if( current == NULL ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: alloc failed!" );
            return( FALSE );
        }
        current->lib = lib;
        current->index = i;
        index = SendMessage( cbox, CB_ADDSTRING, 0, (LPARAM)(LPSTR)lib->lpcci[i].szDesc );

        if( index == CB_ERR || index == CB_ERRSPACE ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: CB_ADDSTRING failed!" );
            WRMemFree( current );
            return( FALSE );
        }

        index = SendMessage( cbox, CB_SETITEMDATA, index, (LPARAM)(LPVOID)current );

        if( index == CB_ERR ) {
            WdeWriteTrail( "WdeSetSelectWinCBox: CB_SETITEMDATA failed!" );
            WRMemFree( current );
            return( FALSE );
        }
    }

    return( TRUE );
}

bool WdeSetSelectWin( HWND win )
{
    HWND        cbox;
    LIST        *llist;
    WdeCustLib  *lib;

    cbox = GetDlgItem( win, IDB_CUST_DESC );

    if( WdeCustomLibList != NULL ) {
        for( llist = WdeCustomLibList; llist != NULL; llist = ListNext( llist ) ) {
            lib = (WdeCustLib *)ListElement( llist );
            if( !WdeSetSelectWinCBox( cbox, lib ) ) {
                return( FALSE );
            }
        }
    }

    return( TRUE );
}

bool WdeSetCurrentControl( HWND win, int which )
{
    LRESULT             index;
    WdeCurrCustControl  *current;

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCOUNT, 0, 0 );
    if( index == 0 || index == CB_ERR ) {
        return( TRUE );
    }

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCURSEL, 0, 0 );

    current = (WdeCurrCustControl *)
        SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETITEMDATA, index, 0 );
    if( current == NULL ) {
        WdeWriteTrail( "WdeSetCurrentControl: CB_GETITEMDATA failed!" );
        return( FALSE );
    }

    WdeCurrControl[which].lib = current->lib;
    WdeCurrControl[which].index = current->index;

    return( TRUE );
}

void WdeMapCustomSize( UINT *w, UINT *h, WdeResizeRatio *r )
{
    RECT                trect;
    DialogSizeInfo      dsize;

    dsize.x = 0;
    dsize.y = 0;
    dsize.width = *w;
    dsize.height = *h;
    WdeDialogToScreen( NULL, r, &dsize, &trect );
    *w = trect.right;
    *h = trect.bottom;
}

bool WdePreviewSelected( HWND win )
{
    WdeCurrCustControl  *current;
    LRESULT             index;
    char                *class;
    char                *caption;
    HWND                pwin;
    HWND                parea;
    RECT                psize;
    RECT                trect;
    DWORD               pstyle;
    DWORD               pestyle;
    POINT               p;
    UINT                pwidth;
    UINT                pheight;
    WdeResizeRatio      resizer;

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCOUNT, 0, 0 );
    if( index == 0 || index == CB_ERR ) {
        return( TRUE );
    }

    parea = GetDlgItem( win, IDB_CUST_PREVIEW );

    pwin = GetDlgItem( win, WDE_PREVIEW_ID );
    if ( pwin != NULL ) {
        DestroyWindow( pwin );
    }

    GetClientRect( parea, &psize );

    InflateRect( &psize, -WDE_PREVIEW_PAD, -WDE_PREVIEW_PAD );

    index = SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETCURSEL, 0, 0 );

    current = (WdeCurrCustControl *)
        SendDlgItemMessage( win, IDB_CUST_DESC, CB_GETITEMDATA, index, 0 );

    if( current == NULL ) {
        WdeWriteTrail( "WdePreviewSelected: CB_GETITEMDATA failed!" );
        return( FALSE );
    }

    if( current->lib->lpcci[current->index].flOptions & CCF_NOTEXT ) {
        caption = NULL;
    } else {
        caption = current->lib->lpcci[current->index].szTextDefault;
    }

    class = current->lib->lpcci[current->index].szClass;
    pstyle = current->lib->lpcci[current->index].flStyleDefault;
    pestyle = current->lib->lpcci[current->index].flExtStyleDefault;
    pwidth = current->lib->lpcci[current->index].cxDefault;
    pheight = current->lib->lpcci[current->index].cyDefault;

    WdeGetResizerFromFont( &resizer, "System", 10 );

    WdeMapCustomSize( &pwidth, &pheight, &resizer );

    if( pwidth > psize.right - psize.left ) {
        pwidth = psize.right - psize.left;
    }

    if( pheight > psize.bottom - psize.top ) {
        pheight = psize.bottom - psize.top;
    }

    p.x = 0;
    p.y = 0;

    SendDlgItemMessage( win, IDB_CUST_CLASSNAME, WM_SETTEXT, 0, (LPARAM)(LPSTR)class );

    pstyle &= ~(WS_POPUP | WS_VISIBLE | WS_TABSTOP);
    pstyle |= WS_CHILD;

    pwin = CreateWindowEx( pestyle, class, caption,
                           pstyle, p.x, p.y, pwidth, pheight, win,
                           (HMENU)WDE_PREVIEW_ID, WdeGetAppInstance(), NULL );

    if( pwin == NULL ) {
        WdeWriteTrail( "WdePreviewSelected: CreateWindow failed!" );
        return( FALSE );
    }

    GetClientRect( pwin, &trect );

    pwidth = trect.right;
    pheight = trect.bottom;

    p.x = ((psize.right - psize.left) - pwidth) / 2 + WDE_PREVIEW_PAD;
    p.y = ((psize.bottom - psize.top) - pheight) / 2 + WDE_PREVIEW_PAD;

    MapWindowPoints( parea, win, &p, 1 );

    SetWindowPos( pwin, (HWND)NULL, p.x, p.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

    ShowWindow( pwin, SW_SHOW );

    return( TRUE );
}

WINEXPORT BOOL CALLBACK WdeSelectCustProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    static int which = 0;
    BOOL       ret;

    ret = FALSE;

    switch( message ) {
    case WM_SETFONT:
        WdeSelectFont = (HFONT)wParam;
        break;

    case WM_INITDIALOG:
        which = *(int *)lParam;
        if( !WdeSetSelectWin( hDlg ) ) {
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, FALSE );
        }
        SendDlgItemMessage( hDlg, IDB_CUST_DESC, CB_SETCURSEL, 0, 0 );
        WdePreviewSelected( hDlg );
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WinHelp( WdeGetMainWindowHandle(), "resdlg.hlp", HELP_CONTENTS, 0 );
            break;

        case IDOK:
            WdeSetCurrentControl( hDlg, which );
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, TRUE );
            ret = TRUE;
            break;

        case IDCANCEL:
            WdeFreeSelectWinCBox( hDlg );
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;

        case IDB_CUST_DESC:
            switch( GET_WM_COMMAND_CMD( wParam, lParam ) ) {
            case CBN_CLOSEUP:
                WdePreviewSelected( hDlg );
                break;
            }
        }
    }

    return( ret );
}
