/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "wderes.h"
#include "wdeinfo.h"
#include "wdereq.h"
#include "wdeobjid.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdeldres.h"
#include "wdefinit.h"
#include "wdeedit.h"
#include "wdemain.h"
#include "wdesvres.h"
#include "wdesdlg.h"
#include "wdelist.h"
#include "wdegetfn.h"
#include "wdewait.h"
#include "wdesym.h"
#include "wde_wres.h"
#include "wdesdup.h"
#include "wdefdiag.h"
#include "wdetfile.h"
#include "wdectool.h"
#include "wdeselft.h"
#include "wdefmenu.h"
#include "wdedebug.h"
#include "wdestat.h"
#include "wdefbase.h"
#include "wdedde.h"
#include "wdefont.h"
#include "wde_rc.h"
#include "wrdll.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WdeResWndProc( HWND, UINT, WPARAM, LPARAM );

void    WdeMouseRtn( HWND, RECT * );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WdeSetCurrentRes( WdeResInfo * );
static bool         WdeCreateResourceWindow( WdeResInfo *, int, char * );
static bool         WdeAddDlgItems( WdeResInfo * );
static bool         WdeRemoveResource( WdeResInfo * );
static char         *WdeGetQueryName( WdeResInfo * );
static bool         WdeQuerySaveResOnDeleteRes( WdeResInfo *, bool );
static bool         WdeQuerySaveSymOnDeleteRes( WdeResInfo *, bool );
static int          WdeIncNumRes( void );
static int          WdeDecNumRes( void );
static WdeResInfo   *WdeResInfoFromWin( HWND );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
char    *WdeResUntitled         = NULL;
char    *WdeResOpenTitle        = NULL;
char    *WdeResSaveTitle        = NULL;
char    *WdeResOpenFilter       = NULL;
char    *WdeResSaveFilter       = NULL;
char    *WdeResSaveFilterEXE    = NULL;
char    *WdeResSaveFilterDLL    = NULL;
char    *WdeResSaveFilterALL    = NULL;
char    *WdeSymSaveFilter       = NULL;
char    *WdeLoadHeaderTitle     = NULL;
char    *WdeWriteHeaderTitle    = NULL;
char    *WdeDlgSaveIntoTitle    = NULL;
char    *WdeDlgSaveAsTitle      = NULL;
char    *WdeDlgSaveTitle        = NULL;
char    *WdeNoSym               = NULL;
char    *WdeCustOpenTitle       = NULL;
char    *WdeCustFilter          = NULL;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST         *WdeResList         = NULL;
static unsigned     WdeResCounter       = 0;
static int          WdeNumRes           = 0;
static WdeResInfo   *WdeCurrentRes      = NULL;
static bool         WdeOldStickyMode    = FALSE;

static void WdeMassageFilter( char *filter )
{
    WRMassageFilter( filter );
}

void WdeFiniResStrings( void )
{
    if( WdeResUntitled != NULL ) {
        WdeFreeRCString( WdeResUntitled );
        WdeResUntitled = NULL;
    }
    if( WdeResOpenTitle != NULL ) {
        WdeFreeRCString( WdeResOpenTitle );
        WdeResOpenTitle = NULL;
    }
    if( WdeResSaveTitle != NULL ) {
        WdeFreeRCString( WdeResSaveTitle );
        WdeResSaveTitle = NULL;
    }
    if( WdeResOpenFilter != NULL ) {
        WdeFreeRCString( WdeResOpenFilter );
        WdeResOpenFilter = NULL;
    }
    if( WdeResSaveFilter != NULL ) {
        WdeFreeRCString( WdeResSaveFilter );
        WdeResSaveFilter = NULL;
    }
    if( WdeResSaveFilterEXE != NULL ) {
        WdeFreeRCString( WdeResSaveFilterEXE );
        WdeResSaveFilterEXE = NULL;
    }
    if( WdeResSaveFilterDLL != NULL ) {
        WdeFreeRCString( WdeResSaveFilterDLL );
        WdeResSaveFilterDLL = NULL;
    }
    if( WdeResSaveFilterALL != NULL ) {
        WdeFreeRCString( WdeResSaveFilterALL );
        WdeResSaveFilterALL = NULL;
    }
    if( WdeSymSaveFilter != NULL ) {
        WdeFreeRCString( WdeSymSaveFilter );
        WdeSymSaveFilter = NULL;
    }
    if( WdeLoadHeaderTitle != NULL ) {
        WdeFreeRCString( WdeLoadHeaderTitle );
        WdeLoadHeaderTitle = NULL;
    }
    if( WdeWriteHeaderTitle != NULL ) {
        WdeFreeRCString( WdeWriteHeaderTitle );
        WdeWriteHeaderTitle = NULL;
    }
    if( WdeDlgSaveIntoTitle != NULL ) {
        WdeFreeRCString( WdeDlgSaveIntoTitle );
        WdeDlgSaveIntoTitle = NULL;
    }
    if( WdeDlgSaveAsTitle != NULL ) {
        WdeFreeRCString( WdeDlgSaveAsTitle );
        WdeDlgSaveAsTitle = NULL;
    }
    if( WdeDlgSaveTitle != NULL ) {
        WdeFreeRCString( WdeDlgSaveTitle );
        WdeDlgSaveTitle = NULL;
    }
    if( WdeNoSym != NULL ) {
        WdeFreeRCString( WdeNoSym );
        WdeNoSym = NULL;
    }
    if( WdeCustOpenTitle != NULL ) {
        WdeFreeRCString( WdeCustOpenTitle );
        WdeCustOpenTitle = NULL;
    }
    if( WdeCustFilter != NULL ) {
        WdeFreeRCString( WdeCustFilter );
        WdeCustFilter = NULL;
    }
}

bool WdeInitResStrings( void )
{
    bool        ok;

    WdeResUntitled = WdeAllocRCString( WDE_UNTITLEDPROJECT );
    ok = (WdeResUntitled != NULL);

    if( ok ) {
        WdeResOpenTitle = WdeAllocRCString( WDE_OPENPROJECT );
        ok = (WdeResOpenTitle != NULL);
    }
    if( ok ) {
        WdeResSaveTitle = WdeAllocRCString( WDE_SAVEPROJECT );
        ok = (WdeResSaveTitle != NULL);
    }
    if( ok ) {
        WdeResOpenFilter = WdeAllocRCString( WDE_OPENFILTER );
        ok = (WdeResOpenFilter != NULL);
        if( ok ) {
            WdeMassageFilter( WdeResOpenFilter );
        }
    }
    if( ok ) {
        WdeResSaveFilter = WdeAllocRCString( WDE_SAVEFILTER );
        ok = (WdeResSaveFilter != NULL);
        if( ok ) {
            WdeMassageFilter( WdeResSaveFilter );
        }
    }
    if( ok ) {
        WdeResSaveFilterEXE = WdeAllocRCString( WDE_SAVEFILTEREXE );
        ok = (WdeResSaveFilterEXE != NULL);
        if( ok ) {
            WdeMassageFilter( WdeResSaveFilterEXE );
        }
    }
    if( ok ) {
        WdeResSaveFilterDLL = WdeAllocRCString( WDE_SAVEFILTERDLL );
        ok = (WdeResSaveFilterDLL != NULL);
        if( ok ) {
            WdeMassageFilter( WdeResSaveFilterDLL );
        }
    }
    if( ok ) {
        WdeResSaveFilterALL = WdeAllocRCString( WDE_SAVEFILTERALL );
        ok = (WdeResSaveFilterALL != NULL);
        if( ok ) {
            WdeMassageFilter( WdeResSaveFilterALL );
        }
    }
    if( ok ) {
        WdeSymSaveFilter = WdeAllocRCString( WDE_SYMFILTER );
        ok = (WdeSymSaveFilter != NULL);
        if( ok ) {
            WdeMassageFilter( WdeSymSaveFilter );
        }
    }
    if( ok ) {
        WdeLoadHeaderTitle = WdeAllocRCString( WDE_LOADSYMTITLE );
        ok = (WdeLoadHeaderTitle != NULL);
    }
    if( ok ) {
        WdeWriteHeaderTitle = WdeAllocRCString( WDE_WRITESYMTITLE );
        ok = (WdeWriteHeaderTitle != NULL);
    }
    if( ok ) {
        WdeDlgSaveIntoTitle = WdeAllocRCString( WDE_SAVEDLGINTOTITLE );
        ok = (WdeDlgSaveIntoTitle != NULL);
    }
    if( ok ) {
        WdeDlgSaveAsTitle = WdeAllocRCString( WDE_SAVEDLGASTITLE );
        ok = (WdeDlgSaveAsTitle != NULL);
    }
    if( ok ) {
        WdeDlgSaveTitle = WdeAllocRCString( WDE_SAVEDLGTITLE );
        ok = (WdeDlgSaveTitle != NULL);
    }
    if( ok ) {
        WdeNoSym = WdeAllocRCString( WDE_NOSYMBOL );
        ok = (WdeNoSym != NULL);
    }
    if( ok ) {
        WdeCustOpenTitle = WdeAllocRCString( WDE_OPENCUSTTITLE );
        ok = (WdeCustOpenTitle != NULL);
    }
    if( ok ) {
        WdeCustFilter = WdeAllocRCString( WDE_OPENCUSTFILTER );
        ok = (WdeCustFilter != NULL);
        if( ok ) {
            WdeMassageFilter( WdeCustFilter );
        }
    }

    if( !ok ) {
        WdeFiniResStrings();
    }

    return( ok );
}

void WdeSetCurrentRes( WdeResInfo *res_info )
{
    WdeCurrentRes = res_info;
}

WdeResInfo *WdeGetCurrentRes( void )
{
    return( WdeCurrentRes );
}

bool WdeRegisterResClass( HINSTANCE app_inst )
{
    WNDCLASS wc;

    /* fill in the WINDOW CLASS structure for the edit window */
    wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WdeResWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = app_inst;
    wc.hIcon = LoadIcon( app_inst, "ResIcon" );
    wc.hCursor = NULL;
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "WdeResClass";

    /* register the edit window class */
    if( !RegisterClass( &wc ) ) {
        WdeDisplayErrorMsg( WDE_REGISTEREDITFAILED );
        return( FALSE );
    }

    return( TRUE );
}

int WdeIncNumRes( void )
{
    WdeNumRes++;
    return( WdeNumRes );
}

int WdeDecNumRes( void )
{
    if( WdeNumRes != 0 ) {
        WdeNumRes--;
    }
    return( WdeNumRes );
}

int WdeGetNumRes( void )
{
    return( WdeNumRes );
}

void WdeShowResourceWindows( int show )
{
    LIST        *rlist;
    WdeResInfo  *res_info;

    for( rlist = WdeResList; rlist != NULL; rlist = ListNext( rlist ) ) {
        res_info = (WdeResInfo *)ListElement( rlist );
        if( show == SW_HIDE ) {
            WdeSetEditMode( res_info, FALSE );
        }
        ShowWindow( res_info->res_win, show );
        EnableWindow( res_info->res_win, show != SW_HIDE );
        if( show != SW_HIDE ) {
            WdeSetEditMode( res_info, TRUE );
        }
    }
}

void WdeAddResDlgItemToResInfo( WdeResInfo *info, WdeResDlgItem *item )
{
    WdeInsertObject( &info->dlg_item_list, item );
}

WdeResInfo *WdeCreateNewResource( char *title )
{
    WdeResInfo  *res_info;
    bool        ok;

    ok = ((res_info = WdeAllocResInfo()) != NULL);

    if( ok ) {
        ok = ((res_info->info = WRAllocWRInfo()) != NULL);
    }

    if( ok ) {
        res_info->hash_table = WRInitHashTable();
        ok = (res_info->hash_table != NULL);
    }

    if( ok ) {
        if( title != NULL ) {
            res_info->info->save_name = WdeStrDup( title );
            ok = (res_info->info->save_name != NULL);
        }
    }

    if( ok ) {
#ifdef __NT__
        res_info->is32bit = TRUE;
#else
        res_info->is32bit = FALSE;
#endif
        ok = WdeCreateResourceWindow( res_info, 0, title );
    }

    if( ok ) {
        ListAddElt( &WdeResList, (void *)res_info );
        if( !WdeIsDDE() || title == NULL ) {
            ok = (WdeCreateNewDialog( NULL, res_info->is32bit ) != NULL);
        }
    }

    if( ok ) {
        WdeCheckBaseScrollbars( FALSE );
    }

    if( res_info ) {
        if( ok ) {
            WdeSetResModified( res_info, FALSE );
        } else {
            WdeRemoveResource( res_info );
            res_info = NULL;
        }
    }

    return( res_info );
}

bool WdeAddDlgItems( WdeResInfo *res_info )
{
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WdeResDlgItem       *dlg_item;

    if( res_info == NULL ) {
        return( FALSE );
    }

    if( res_info->dlg_entry != NULL ) {
        rnode = res_info->dlg_entry->Head;
    } else {
        rnode = NULL;
    }

    while( rnode != NULL ) {
        lnode = rnode->Head;
        while( lnode != NULL ) {
            dlg_item = WdeAllocResDlgItem();
            if( dlg_item == NULL ) {
                return( FALSE );
            }
            dlg_item->is32bit = res_info->is32bit;
            dlg_item->rnode = rnode;
            dlg_item->lnode = lnode;
            WdeAddResDlgItemToResInfo( res_info, dlg_item );
            lnode = lnode->Next;
        }
        rnode = rnode->Next;
    }

    return( TRUE );
}

bool WdeOpenResource( char *fn )
{
    char                *name;
    WdeResInfo          *res_info;
    WdeGetFileStruct    gf;
    bool                ok, got_name;

    WdeSetWaitCursor( TRUE );

    res_info = NULL;
    name = NULL;
    got_name = FALSE;

    if( fn != NULL ) {
        if( WdeFileExists( fn ) ) {
            name = WdeStrDup( fn );
            gf.fn_offset = WRFindFnOffset( name );
        } else {
            return( FALSE );
        }
    } else {
        gf.file_name = NULL;
        gf.title = WdeResOpenTitle;
        gf.filter = WdeResOpenFilter;
        ok = ((name = WdeGetOpenFileName( &gf )) != NULL);
    }

    if( ok ) {
        got_name = TRUE;
        ok = ((res_info = WdeLoadResource( name )) != NULL);
    }

    if( ok ) {
        res_info->hash_table = WRInitHashTable();
        ok = (res_info->hash_table != NULL);
    }

    if( ok ) {
        if( res_info->info->internal_type != WR_DONT_KNOW ) {
            res_info->is32bit = WRIs32Bit( res_info->info->internal_type );
        } else {
            res_info->is32bit = WRIs32Bit( res_info->info->file_type );
        }
        ok = WdeAddDlgItems( res_info );
    }

    if( ok ) {
        WdeFindAndLoadSymbols( res_info );
        ok = WdeCreateResourceWindow( res_info, gf.fn_offset, NULL );
    }

    if( ok ) {
        if( WdeResInfoHasDialogs( res_info ) ) {
            WdeSelectDialog( res_info );
        } else {
            WdeDisplayErrorMsg( WDE_PRJHASNODIALOGS );
        }
        ListAddElt( &WdeResList, (void *)res_info );
        WdeSetResModified( res_info, FALSE );
        WdeCheckBaseScrollbars( FALSE );
    }

    if( !ok ) {
        if( res_info != NULL ) {
            WdeFreeResInfo( res_info );
            res_info = NULL;
        }
        if( got_name ) {
            WdeDisplayErrorMsg( WDE_RESOURCESNOTLOADED );
        }
    }

    if( name != NULL ) {
        WRMemFree( name );
    }

    WdeSetWaitCursor( FALSE );

    return( ok );
}

WdeResInfo *WdeResInfoFromWin( HWND win )
{
    WdeResInfo  *info;
    LIST        *rlist;

    if( win != NULL ) {
        info = NULL;
        for( rlist = WdeResList; rlist != NULL; rlist = ListNext( rlist ) ) {
            info = (WdeResInfo *)ListElement( rlist );
            if( info->res_win == win ) {
                return( info );
            }
        }
    }

    return( NULL );
}

void WdeActivateResourceWindow( WdeResInfo *res_info, WPARAM wParam, LPARAM lParam )
{
    WdeResInfo  *info;
    OBJPTR      current_obj;
    bool        fActivate;
    HWND        hwndDeact;
    int         id;

    _wde_touch( wParam );

    if( res_info == NULL ) {
        return;
    }

    fActivate = GET_WM_MDIACTIVATE_FACTIVATE( res_info->res_win, wParam, lParam );

    hwndDeact = GET_WM_MDIACTIVATE_HWNDDEACTIVATE( wParam, lParam );

    res_info->active = fActivate;

    if( fActivate ) {
        WdeSetCurrentRes( res_info );
        info = WdeResInfoFromWin( hwndDeact );
        if( info != NULL ) {
            info->active = FALSE;
        }
    }

    if( res_info->editting ) {
        InitState( res_info->forms_win );
        id = WdeGetCToolID();
        WdeSetBaseObject( id );
        if( fActivate ) {
            current_obj = GetCurrObject();
            if( current_obj != NULL ) {
                Notify( current_obj, PRIMARY_OBJECT, NULL );
            }
        }
    }
}

static void WdeCheckIfActiveWindow( void )
{
    HWND        main;
    HWND        active;

    main = WdeGetMainWindowHandle();
    active = GetActiveWindow();
    if( active != main ) {
        if( IsIconic( main ) ) {
            ShowWindow( main, SW_RESTORE );
        }
#ifdef __NT__
        SetWindowPos( main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( main, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( main, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetForegroundWindow( main );
#else
        SetActiveWindow( main );
        SetWindowPos( main, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif
    }
}

char *WdeGetQueryName( WdeResInfo *res_info )
{
    char *name;

    if( res_info->info->save_name != NULL ) {
        name = res_info->info->save_name;
    } else if( res_info->info->file_name != NULL ) {
        name = res_info->info->file_name;
    } else {
        name = WdeResUntitled;
    }

    return( name );
}

bool WdeQuerySaveResOnDeleteRes( WdeResInfo *res_info, bool fatal_exit )
{
    int         ret;
    UINT        style;
    char        *file;
    char        *text;
    HWND        frame;

    if( res_info != NULL && res_info->dlg_item_list != NULL &&
        WdeIsResModified( res_info ) ) {
        WdeCheckIfActiveWindow();
        if( fatal_exit ) {
            style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
        } else {
            style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
        }
        frame = WdeGetMDIWindowHandle();
        if( IsIconic( frame ) ) {
            SendMessage( frame, WM_MDIRESTORE, (WPARAM)res_info->res_win, 0 );
        }
        SendMessage( frame, WM_MDIACTIVATE, (WPARAM)res_info->res_win, 0 );
        file = WdeGetQueryName( res_info );
        if( WdeIsDDE() ) {
            text = WdeAllocRCString( WDE_UPDATEMODIFIEDPRJ );
        } else {
            text = WdeAllocRCString( WDE_SAVEMODIFIEDPRJ );
        }
        ret = MessageBox( res_info->res_win, text, file, style );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
        if( ret == IDYES ) {
            if( WdeIsDDE() ) {
                return( WdeUpdateDDEEditSession() );
            } else {
                return( WdeSaveResource( res_info, FALSE ) );
            }
        } else if( ret == IDCANCEL ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

bool WdeQuerySaveSymOnDeleteRes( WdeResInfo *res_info, bool fatal_exit )
{
    int         ret;
    UINT        style;
    char        *file;
    char        *text;
    HWND        frame;

    if( fatal_exit ) {
        style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    } else {
        style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
    }

    if( res_info != NULL && res_info->hash_table != NULL &&
        WdeIsHashTableDirty( res_info->hash_table ) ) {
        WdeCheckIfActiveWindow();
        file = WdeGetQueryName( res_info );
        frame = WdeGetMDIWindowHandle();
        if( IsIconic( frame ) ) {
            SendMessage( frame, WM_MDIRESTORE, (WPARAM)res_info->res_win, 0 );
        }
        SendMessage( frame, WM_MDIACTIVATE, (WPARAM) res_info->res_win, 0 );
        text = WdeAllocRCString( WDE_SAVEMODIFIEDSYM );
        ret = MessageBox( res_info->res_win, text, file, style );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
        if( ret == IDYES ) {
            if( res_info->sym_name == NULL ) {
                res_info->sym_name = WdeCreateSymName( file );
            }
            if( !WdeWriteSymbols( res_info->hash_table, &res_info->sym_name, FALSE ) ) {
                return( FALSE );
            }
        } else if( ret == IDCANCEL ) {
            return( FALSE );
        } else if( ret == IDNO ) {
            WdeHashSaveRejected( res_info->hash_table );
        }
    }

    return( TRUE );
}

bool WdeDestroyResourceWindow( WdeResInfo *res_info )
{
    bool        ret;

    ret = (res_info != NULL && WdeQuerySaveResOnDeleteRes( res_info, FALSE ) &&
           WdeQuerySaveSymOnDeleteRes( res_info, FALSE ));

    WdeHashClearSaveRejected( res_info->hash_table );

    if( ret ) {
        if( !WdeRemoveResource( res_info ) ) {
            WdeWriteTrail( "WdeDestroyResourceWindow: RemoveResource failed!" );
        }
    }

    return( ret );
}

bool WdeCreateResourceWindow( WdeResInfo *res_info, int fn_offset, char *title )
{
    MDICREATESTRUCT     mdics;
    LRESULT             ret;
    HWND                win;
    bool                ok;
    bool                old;
    DWORD               style;
    RECT                r;
    HMENU               sys_menu;
    char                *win_title;
    int                 win_title_len;

    _wde_touch( fn_offset );

    WdeIncNumRes();

    style = 0;

    if( WdeGetNumRes() == 1 ) {
        WdeSetAppMenuToRes( TRUE );
        old = WdeSetStickyMode( WdeOldStickyMode );
        style = WS_MAXIMIZE;
    } else {
        if( WdeIsCurrentMDIWindowZoomed() ) {
            style = WS_MAXIMIZE;
        }
    }

    mdics.szClass = "WdeResClass";

    win_title = NULL;
    if( title == NULL ) {
        if( res_info->info->file_name ) {
            // perhaps make this an option
            //title = &res_info->info->file_name[fn_offset];
            mdics.szTitle = res_info->info->file_name;
        } else {
            WdeResCounter++;
            win_title_len = strlen( WdeResUntitled ) + 7;
            win_title = (char *)WRMemAlloc( win_title_len );
            sprintf( win_title, "%s.%d", WdeResUntitled, 0xffff & WdeResCounter );
            mdics.szTitle = win_title;
        }
    } else {
        mdics.szTitle = title;
    }

    win = WdeGetMDIWindowHandle();
    GetClientRect( win, &r );

    mdics.hOwner = WdeGetAppInstance();
    mdics.x = CW_USEDEFAULT;
    mdics.y = CW_USEDEFAULT;
    mdics.cx = CW_USEDEFAULT;
    mdics.cy = CW_USEDEFAULT;
    //mdics.cx = r.right - r.left;
    //mdics.cy = r.bottom - r.top;
    mdics.style = style;
    mdics.lParam = (LPARAM)res_info;

    ret = SendMessage( win, WM_MDICREATE, 0, (LPARAM)&mdics );

    if( win_title != NULL ) {
        WRMemFree( win_title );
    }

#ifdef __NT__
    win = (HWND)ret;
#else
    win = (HWND)LOWORD( ret );
#endif

    ok = (res_info->res_win != NULL && res_info->res_win == win);
    if( !ok ) {
        WdeWriteTrail( "WdeCreateResourceWindow: Bad window handle!" );
    }

    if( WdeIsDDE() ) {
        sys_menu = GetSystemMenu( win, FALSE );
        if( sys_menu != (HMENU)NULL ) {
            EnableMenuItem( sys_menu, SC_CLOSE, MF_GRAYED );
        }
    }

    if( ok ) {
        ok = WdeCreateEditWindows( res_info );
        if( !ok ) {
            WdeWriteTrail( "WdeCreateResourceWindow: Could not create edit windows!" );
        }
    }

    if( ok ) {
        OpenFormEdit( res_info->forms_win, WdeGetCreateTable(), MENU_NONE, SCROLL_NONE );
        WdeSetEditMode( res_info, TRUE );
        SetHorizontalInc( 1 );
        SetVerticalInc( 1 );
        InitState( res_info->forms_win );
        SetMouseRtn( res_info->forms_win, WdeMouseRtn );
        //MakeObjectCurrent( GetMainObject() );
    } else {
        if( WdeGetNumRes() == 1 ) {
            WdeSetAppMenuToRes( FALSE );
            WdeSetStickyMode( old );
        }
        WdeDecNumRes();
    }

    return( ok );
}

char *WdeSelectSaveFilter( WRFileType ftype )
{
    char        *filter;

    switch( ftype ) {
    case WR_WIN16_EXE:
    case WR_WINNT_EXE:
        filter = WdeResSaveFilterEXE;
        break;
    case WR_WIN16_DLL:
    case WR_WINNT_DLL:
        filter = WdeResSaveFilterDLL;
        break;
    default:
        filter = WdeResSaveFilter;
    }

    return( filter );
}

bool WdeSaveResource( WdeResInfo *res_info, bool get_name )
{
    WdeGetFileStruct    gf;
    char                *filter;
    char                *fn;
    int                 fn_offset;
    bool                got_name;
    bool                ok;
    OBJPTR              main_obj;

    fn_offset = 0;
    got_name = FALSE;

    ok = (res_info != NULL && res_info->info != NULL);

    if( ok ) {
        if( res_info->info->save_name != NULL ) {
            fn = res_info->info->save_name;
        } else {
            res_info->info->save_type = res_info->info->file_type;
            fn = WdeStrDup( res_info->info->file_name );
            got_name = TRUE;
        }

        if( get_name || fn == NULL || *fn == '\0' ) {
            filter = WdeSelectSaveFilter( res_info->info->file_type );
            gf.file_name = fn;
            gf.title = WdeResSaveTitle;
            gf.filter = filter;
            fn = WdeGetSaveFileName( &gf );
            got_name = TRUE;
            res_info->info->save_type = WR_DONT_KNOW;
        }

        ok = (fn != NULL && *fn != '\0');
    }

    if( ok ) {
        if( got_name && res_info->info->save_name != NULL ) {
            WRMemFree( res_info->info->save_name );
        }
        res_info->info->save_name = fn;
        if( res_info->info->save_type == WR_DONT_KNOW ) {
            res_info->info->save_type = WdeSelectFileType( fn, res_info->is32bit );
        }
        ok = (res_info->info->save_type != WR_DONT_KNOW);
    }

    if( ok ) {
        if( WdeIsHashTableDirty( res_info->hash_table ) &&
            !WdeIsHashSaveRejectedSet( res_info->hash_table ) ) {
            if( res_info->sym_name == NULL ) {
                res_info->sym_name = WdeCreateSymName( fn );
            }
        }
    }

    if( ok ) {
        if( WdeIsHashTableDirty( res_info->hash_table ) ) {
            if( (main_obj = GetMainObject()) != NULL ) {
                Forward( main_obj, RESOLVE_HELPSYMBOL, &ok, NULL ); /* JPK */
                Forward( main_obj, RESOLVE_SYMBOL, &ok, NULL );
            }
        }
        WdeCreateDLGInclude( res_info, res_info->sym_name );
        ok = WdeSaveResourceToFile( res_info );
        if( !ok ) {
            WdeDisplayErrorMsg( WDE_SAVEFAILED );
        }
    }

    if( ok ) {
        if( WdeIsHashTableDirty( res_info->hash_table ) &&
            !WdeIsHashSaveRejectedSet( res_info->hash_table ) ) {
            WdeWriteSymbols( res_info->hash_table, &res_info->sym_name, get_name );
        }
    }

    if( ok ) {
        //fn_offset = WRFindFnOffset( fn );
        SendMessage( res_info->res_win, WM_SETTEXT, 0, (LPARAM)(LPVOID)&fn[fn_offset] );
    }

    return( ok );
}

bool WdeQueryKillApp( bool fatal_exit )
{
    LIST        *rlist;
    WdeResInfo  *res_info;
    bool        kill_app;

    if( WdeResList != NULL ) {
        kill_app = TRUE;
        for( rlist = WdeResList; rlist != NULL && kill_app; rlist = ListNext( rlist ) ) {
            res_info = (WdeResInfo *)ListElement( rlist );
            kill_app = WdeQuerySaveResOnDeleteRes( res_info, fatal_exit ) &&
                       WdeQuerySaveSymOnDeleteRes( res_info, fatal_exit );
            kill_app = kill_app || fatal_exit;
            WdeHashClearSaveRejected( res_info->hash_table );
        }
        if( !fatal_exit && !kill_app ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

void WdeFreeResList( void )
{
    LIST       *rlist;
    WdeResInfo *res_info;

    if( WdeResList != NULL ) {
        for( rlist = WdeResList; rlist != NULL; rlist = ListNext( rlist ) ) {
            res_info = (WdeResInfo *)ListElement( rlist );
            WdeSetCurrentRes( res_info );
            WdeFreeResInfo( res_info );
        }
        ListFree( WdeResList );
        WdeResList = NULL;
    }
}

bool WdeRemoveResource( WdeResInfo *res_info )
{
    LIST *node;

    if( WdeResList == NULL ) {
        return( FALSE );
    }

    if( (node = ListFindElt( WdeResList, res_info )) != NULL ) {
        ListRemoveElt( &WdeResList, (void *)ListElement( node ) );
    } else {
        WdeWriteTrail( "WdeRemoveResource: res_info not found!" );
        return( FALSE );
    }

    if( WdeGetNumRes() == 1 ) {
        WdeOldStickyMode = WdeSetStickyMode( FALSE );
        WdeSetBaseObject( IDM_SELECT_MODE );
    }

    WdeFreeResInfo( res_info );

    if( WdeGetNumRes() == 1 ) {
        WdeSetAppMenuToRes( FALSE );
    }

    WdeDecNumRes();

    return( TRUE );
}

bool WdeRemoveDialogFromResInfo( WdeResInfo *res_info, WdeResDlgItem *ditem,
                                 bool destroy_object )
{
    WResResNode   *rnode;
    WResLangNode  *lnode;

    if( res_info == NULL || ditem == NULL ) {
        return( FALSE );
    }

    res_info->modified = true;

    rnode = ditem->rnode;
    lnode = ditem->lnode;

    ListRemoveElt( &res_info->dlg_item_list, ditem );
    WdeFreeResDlgItem( &ditem, destroy_object );

    if( rnode != NULL || lnode != NULL ) {
        return( WRRemoveLangNodeFromDir( res_info->info->dir, &res_info->dlg_entry,
                                         &rnode, &lnode ) );
    }

    return( TRUE );
}

WINEXPORT LRESULT CALLBACK WdeResWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    WdeResInfo  *res_info;
    int         msg_processed;
    LRESULT     ret;

    msg_processed = FALSE;
    ret = FALSE;
    res_info = NULL;

    switch( message ) {
    case WM_CREATE:
        res_info = (WdeResInfo *)((MDICREATESTRUCT *)((CREATESTRUCT *)lParam)->lpCreateParams)->lParam;
        res_info->res_win = hWnd;
        SET_WNDINFO( hWnd, (LONG_PTR)res_info );
        break;
    //case WM_COMMAND:
    //case WM_KEYUP:
    //case WM_KEYDOWN:
    case WM_SIZE:
    case WM_MDIACTIVATE:
    case WM_CLOSE:
        res_info = (WdeResInfo *)GET_WNDINFO( hWnd );
        break;
    case WM_DESTROY:
        SET_WNDINFO( hWnd, (LONG_PTR)NULL );
        break;
    }

    if( res_info != NULL ) {
        switch( message ) {
#if 0
        case WM_KEYUP:
        case WM_KEYDOWN:
            WdePassToEdit( message, wParam, lParam );
            break;

        case WM_COMMAND:
            switch( LOWORD( wParam ) ) {
            case IDM_ESCAPE:
                ret = WdePassToEdit( message, wParam, lParam );
                break;
            }
            break;
#endif

        case WM_SIZE:
            WdeResizeEditWindows( res_info );
            break;

        case WM_MDIACTIVATE:
            WdeActivateResourceWindow( res_info, wParam, lParam );
            break;

        case WM_CLOSE:
            if( !WdeDestroyResourceWindow( res_info ) ) {
                return( (LRESULT)FALSE );
            }
            break;
        }
    }

    if( !msg_processed ) {
        ret = DefMDIChildProc( hWnd, message, wParam, lParam );
    }

    return( ret );
}

bool WdeSetObjectInfo( OBJPTR dlg, WdeResInfo **ri, WdeResDlgItem **di, WResID *id )
{
    WdeResInfo      *res_info;
    WdeResDlgItem   *item;
    WResID          *name;
    bool            ok;

    ok = (dlg != NULL && id != NULL && ri != NULL && di != NULL);

    if( ok ) {
        ok = ((res_info = WdeGetCurrentRes()) != NULL);
    }

    if( ok ) {
        ok = ((item = WdeAllocResDlgItem()) != NULL);
    }

    if( ok ) {
        ok = ((name = WdeCopyWResID( id )) != NULL);
    }

    if( ok ) {
        item->object = dlg;
        item->dialog_name = name;
        item->modified = true;
        item->is32bit = res_info->is32bit;
        WdeAddResDlgItemToResInfo( res_info, item );
        *ri = res_info;
        *di = item;
    } else {
        if( item != NULL ) {
            WdeFreeResDlgItem( &item, FALSE );
        }
        if( name != NULL ) {
            WResIDFree( name );
        }
    }

    return( ok );
}

OBJPTR WdeGetCurrentDialog( void )
{
    WdeResInfo  *info;
    OBJPTR      curr_obj;
    OBJPTR      ancestor;
    OBJ_ID      id;

    info = WdeGetCurrentRes();
    if( info == NULL ) {
        return( NULL );
    }
    InitState( info->forms_win );

    curr_obj = GetCurrObject();
    if( curr_obj == NULL ) {
        return( NULL );
    }

    if( !Forward( curr_obj, IDENTIFY, &id, NULL ) ) {
        WdeWriteTrail( "WdeGetCurrentDialog: IDENTIFY failed!" );
        return( NULL );
    }

    while( id != DIALOG_OBJ && id != BASE_OBJ ) {
        GetObjectParent( curr_obj, &ancestor );
        if( ancestor == NULL ) {
            WdeWriteTrail( "WdeGetCurrentDialog: IDENTIFY failed!" );
            return( NULL );
        }
        curr_obj = ancestor;
        if( !Forward( curr_obj, IDENTIFY, &id, NULL ) ) {
            WdeWriteTrail( "WdeGetCurrentDialog: IDENTIFY failed!" );
            return( NULL );
        }
    }

    if( id != DIALOG_OBJ ) {
        return( NULL );
    }

    return( curr_obj );
}

OBJPTR WdeIsDialogInList( LIST *l )
{
    OBJ_ID  oid;
    OBJPTR  obj;

    for( ; l != NULL; l = ListNext( l ) ) {
        obj = ListElement( l );
        if( Forward( obj, IDENTIFY, &oid, NULL ) && oid == DIALOG_OBJ ) {
            return( obj );
        }
    }

    return( NULL );
}

bool WdeMouseRtnCreate( HWND win, RECT *r )
{
    RECT            *ncp;
    RECT            rect;
    LIST            *l;
    SUBOBJ_REQUEST  req;
    OBJPTR          obj;
    OBJPTR          ro;
    bool            adjust;
    POINT           pt;
    WdeResizeRatio  resizer;
    WdeResInfo      *info;
    DialogSizeInfo  d;

    rect = *r;

    /* if we are creating a dialog discard the NC area */
    if( GetBaseObjType() == DIALOG_OBJ ) {
        ncp = WdeGetDefaultDialogNCSize();
        rect.left += ncp->left;
        rect.top += ncp->top;
        rect.right -= ncp->right;
        rect.bottom -= ncp->bottom;
        if( rect.right < rect.left ) {
            rect.right = rect.left;
        }
        if( rect.bottom < rect.top ) {
            rect.bottom = rect.top;
        }
        adjust = FALSE;
        obj = GetMainObject();
    } else {
        l = NULL;
        req.p.ty = AT_POINT;
        req.p.pt.x = rect.left;
        req.p.pt.y = rect.top;
        Forward( GetMainObject(), FIND_SUBOBJECTS, &req, &l );

        obj = WdeIsDialogInList( l );

        if( l ) {
            ListFree( l );
        }

        if( obj ) {
            adjust = TRUE;
        } else {
            adjust = FALSE;
            obj = GetMainObject();
        }
    }

    if( !Forward( obj, GET_RESIZER, &resizer, &ro ) ) {
        return( FALSE );
    }

    if( adjust ) {
        if( !Forward( ro, GET_WINDOW_HANDLE, &win, NULL) ) {
            return( FALSE );
        }
        GetOffset( &pt );
        OffsetRect( &rect, -pt.x, -pt.y );
        if( (info = WdeGetCurrentRes()) != NULL ) {
            MapWindowPoints( info->edit_win, win, (POINT *)&rect, 2 );
        }
    }

    if( WdeScreenToDialog( NULL, &resizer, &rect, &d ) ) {
        WdeWriteObjectDimensions( (int_16)d.x, (int_16)d.y,
                                  (int_16)d.width, (int_16)d.height );
    }

    return( TRUE );
}

bool WdeMouseRtnResize( HWND win, RECT *r )
{
    POINT           pt;
    LIST            *l;
    OBJPTR          curr_obj;
    OBJ_ID          oid;
    OBJPTR          obj;
    OBJPTR          ro;
    RECT            rect;
    RECT            nc;
    WdeResizeRatio  resizer;
    bool            adjust;
    WdeResInfo      *info;
    DialogSizeInfo  d;
    SUBOBJ_REQUEST  req;

    rect = *r;
    adjust = FALSE;

    /* try to find the object id of resized object */
    curr_obj = GetCurrObject();
    if( curr_obj == NULL || !Forward( curr_obj, IDENTIFY, &oid, NULL ) ) {
        oid = 0;
    }

    if( oid == DIALOG_OBJ ) {
        if( !Forward( curr_obj, GET_NC_SIZE, &nc, NULL ) ) {
            return( FALSE );
        }
        rect.left += nc.left;
        rect.top += nc.top;
        rect.right -= nc.right;
        rect.bottom -= nc.bottom;
        if( rect.right < rect.left ) {
            rect.right = rect.left;
        }
        if( rect.bottom < rect.top ) {
            rect.bottom = rect.top;
        }
        if( !Forward( curr_obj, GET_RESIZER, &resizer, &ro ) ) {
            return( FALSE );
        }
    } else {
        l = NULL;
        req.p.ty = AT_POINT;
        req.p.pt.x = rect.left;
        req.p.pt.y = rect.top;
        Forward( GetMainObject(), FIND_SUBOBJECTS, &req, &l );
        obj = WdeIsDialogInList( l );
        if( l != NULL ) {
            ListFree( l );
        }
        if( obj != NULL ) {
            adjust = TRUE;
        } else {
            obj = GetMainObject();
        }
        if( !Forward( obj, GET_RESIZER, &resizer, &ro ) ) {
            return( FALSE );
        }
    }

    if( adjust ) {
        if( !Forward( ro, GET_WINDOW_HANDLE, &win, NULL ) ) {
            return( FALSE );
        }
        GetOffset( &pt );
        OffsetRect( &rect, -pt.x, -pt.y );
        if( (info = WdeGetCurrentRes()) != NULL ) {
            MapWindowPoints( info->edit_win, win, (POINT *)&rect, 2 );
        }
    }

    if( WdeScreenToDialog( NULL, &resizer, &rect, &d ) ) {
        WdeWriteObjectDimensions( (int_16)d.x, (int_16)d.y,
                                  (int_16)d.width, (int_16)d.height );
    }

    return( TRUE );
}

void WdeMouseRtn( HWND win, RECT *r )
{
    if( GetBaseObjType() == EDIT_SELECT ) {
        WdeMouseRtnResize( win, r );
    } else {
        WdeMouseRtnCreate( win, r );
    }
}
