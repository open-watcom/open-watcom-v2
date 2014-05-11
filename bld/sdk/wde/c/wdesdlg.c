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
#include "wdeactn.h"
#include "wdeobjid.h"
#include "wderes.h"
#include "wdemain.h"
#include "wdefdlg.h"
#include "wdeldres.h"
#include "wdelist.h"
#include "wdefdiag.h"
#include "wdedebug.h"
#include "wde_wres.h"
#include "wdefutil.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdewait.h"
#include "wdegoto.h"
#include "wdectl3d.h"
#include "wde_rc.h"
#include "wde_wres.h"
#include "wdei2mem.h"
#include "wdesdlg.h"
#include "jdlg.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    WdeResInfo *res_info;
    LIST       *selection;
    Bool        remove;
} WdeDialogSelectInfo;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WdeSelectDialogProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static LIST  *WdeSelectDialogs( WdeResInfo *, Bool );
static Bool   WdeSetSelectInfo( HWND, WdeDialogSelectInfo * );
static Bool   WdeGetSelectInfo( HWND, WdeDialogSelectInfo * );
static Bool   WdeInitSelectListBox( WdeResInfo *, HWND );
static char  *WdeResolveDialogName( WdeResInfo *, WResID * );
static Bool   WdeAddControlToDialog( WdeResInfo *, OBJPTR, WdeDialogBoxControl *, POINT *, HWND );
static Bool   WdeAddControlsToDialog( WdeResInfo *, OBJPTR, WdeDialogBoxInfo * );
static OBJ_ID WdeGetOBJIDFromControl( WdeDialogBoxControl * );
static OBJ_ID WdeGetOBJIDFromClassNum( uint_8, uint_32 );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WdeResInfoHasDialogs( WdeResInfo *res_info )
{
    return( res_info->dlg_item_list != NULL );
}

Bool WdeSelectDialog( WdeResInfo *res_info )
{
    LIST            *selection;
    WdeResDlgItem   *ditem;
    LIST            *slist;
    Bool            ok;
    Bool            last;

    if( ListCount( res_info->dlg_item_list ) == 1 ) {
        ok = WdeOpenSelectedDialog( res_info, ListElement( res_info->dlg_item_list ), TRUE );
        return( ok );
    }

    selection = WdeSelectDialogs( res_info, FALSE );
    ok = TRUE;
    WdeSetWaitCursor( TRUE );

    for( slist = selection; slist != NULL; slist = ListConsume( slist ) ) {
        ditem = (WdeResDlgItem *)ListElement( slist );
        last = (ListNext(slist) == NULL);
        if( !WdeOpenSelectedDialog( res_info, ditem, last ) ) {
            WdeWriteTrail( "WdeSelectDialog: open failed!" );
            ok = FALSE;
        }
    }

    WdeSetWaitCursor( FALSE );

    return( ok );
}

Bool WdeRemoveDialog( WdeResInfo *res_info )
{
    LIST            *selection;
    WdeResDlgItem   *ditem;
    LIST            *slist;
    Bool            ok;

    selection = WdeSelectDialogs( res_info, TRUE );
    ok = TRUE;

    for( slist = selection; slist != NULL; slist = ListConsume( slist ) ) {
        ditem = (WdeResDlgItem *)ListElement ( slist );
        ok = WdeRemoveDialogFromResInfo( res_info, ditem, TRUE );
    }

    Notify( GetMainObject(), PRIMARY_OBJECT, NULL );

    return( ok );
}

LIST *WdeSelectDialogs( WdeResInfo *res_info, Bool remove )
{
    int                 ret;
    HINSTANCE           inst;
    FARPROC             proc;
    WdeDialogSelectInfo si;

    if( res_info == NULL ) {
        return( FALSE );
    }

    inst = WdeGetAppInstance();

    proc = MakeProcInstance( (FARPROC)WdeSelectDialogProc, inst );

    if( proc == NULL ) {
        return( FALSE );
    }

    si.res_info = res_info;
    si.selection = NULL;
    si.remove = remove;

    ret = JDialogBoxParam( inst, "WdeSelectDialog", res_info->res_win,
                           (DLGPROC)proc, (LPARAM)&si );

    FreeProcInstance( proc );

    /* if the window could not be created return FALSE */
    if( ret == -1 ) {
        WdeWriteTrail( "WdeSelectDialogs: dialog not created!" );
        return( NULL );
    }

    UpdateWindow( WdeGetMainWindowHandle() );

    InitState( res_info->forms_win );

    return( si.selection );
}

Bool WdeSetSelectInfo( HWND hDlg, WdeDialogSelectInfo *si )
{
    HWND        win;
    char        *text;

    if( si == NULL || si->res_info == NULL || !WdeResInfoHasDialogs( si->res_info ) ) {
        return( FALSE );
    }

    if( si->remove ) {
        text = WdeAllocRCString( WDE_REMOVEDIALOGS );
        SendMessage( hDlg, WM_SETTEXT, 0, (LPARAM)text );
        if( text != NULL ) {
            WdeFreeRCString( text );
        }
    }

    win = GetDlgItem( hDlg, IDB_SELECT_LISTBOX );

    return( WdeInitSelectListBox( si->res_info, win ) );
}

Bool WdeGetSelectInfo( HWND hDlg, WdeDialogSelectInfo *si )
{
    LRESULT         count;
    int             *sel;
    Bool            ok;
    LRESULT         ret;
    HWND            win;
    WdeResDlgItem   *ditem;

    if( si == NULL || si->res_info == NULL ) {
        return( FALSE );
    }

    win = GetDlgItem( hDlg, IDB_SELECT_LISTBOX );
    count = SendMessage( win, LB_GETSELCOUNT, 0, 0 );
    if( count == 0 ) {
        return( TRUE );
    }

    sel = (int *)WRMemAlloc( count * sizeof( int ) );
    if( sel == NULL ) {
        WdeWriteTrail( "WdeGetSelectInfo: alloc failed!" );
        return( FALSE );
    }
    memset( sel, 0, count * sizeof( int ) );

    ret = SendMessage( win, LB_GETSELITEMS, count, (LPARAM)sel );

    if( !ret || ret == LB_ERR ) {
        WdeWriteTrail( "WdeGetSelectInfo: LB_GETSELITEMS failed!" );
        return( FALSE );
    }

    if( ret != count ) {
        WdeWriteTrail( "WdeGetSelectInfo: Inconsistency detected!" );
    }

    ok = TRUE;
    for( ; count > 0; count-- ) {
        ret = SendMessage( win, LB_GETITEMDATA, sel[count - 1], 0 );
        if( ret != LB_ERR ) {
            ditem = WdeFindDialogInResInfo( si->res_info, (int)ret );
            if( ditem == NULL ) {
                ok = FALSE;
                break;
            }
            ListAddElt( &si->selection, ditem );
        }
    }

    if( sel ) {
        WRMemFree( sel );
    }

    return( ok );
}

Bool WdeInitSelectListBox( WdeResInfo *res_info, HWND win )
{
    char            *name;
    LIST            *dlist;
    WdeResDlgItem   *ditem;
    WResID          *id;
    LRESULT         index;
    int             count;

    if( win == NULL ) {
        return( FALSE );
    }

    dlist = res_info->dlg_item_list;
    SendMessage( win, WM_SETREDRAW, FALSE, 0 );
    count = 0;
    while( dlist != NULL ) {
        ditem = (WdeResDlgItem *)ListElement( dlist );
        id = NULL;

        if( ditem->object != NULL || ditem->dialog_info != NULL ) {
            if( ditem->object != NULL ) {
                Forward( ditem->object, GET_OBJECT_INFO, NULL, &id );
            } else if( ditem->dialog_name != NULL ) {
                id = ditem->dialog_name;
            }
        } else if( ditem->rnode != NULL ) {
            id = &ditem->rnode->Info.ResName;
        }

        if( id == NULL ) {
            return( FALSE );
        }

        name = WdeResolveDialogName( res_info, id );

        if( name == NULL ) {
            return( FALSE );
        }

        /* add the name to the list box */
        index = SendMessage( win, LB_ADDSTRING, 0, (LPARAM)(LPSTR)name );
        SendMessage( win, LB_SETITEMDATA, index, (LPARAM)count );

        WRMemFree( name );

        count++;

        dlist = ListNext( dlist );
    }

    SendMessage( win, WM_SETREDRAW, TRUE, 0 );
    InvalidateRect( win, NULL, TRUE );

    return( TRUE );
}

char *WdeResolveDialogName( WdeResInfo *res_info, WResID *id )
{
    char *name;

    name = NULL;

    if( res_info->hash_table != NULL && !id->IsName ) {
        name = WdeResolveValue( res_info->hash_table, (WdeHashValue)id->ID.Num );
    }

    if( name == NULL ) {
        name = WResIDToStr( id );
    }

    return( name );
}

Bool WdeOpenSelectedDialog( WdeResInfo *res_info, WdeResDlgItem *ditem, Bool make_current )
{
    Bool        from_id;

    if( res_info == NULL || ditem == NULL ) {
        return( FALSE );
    }

    if( ditem->object != NULL ) {
        if( make_current ) {
            MakeObjectCurrent( ditem->object );
            WdeHandleGotoCurrentObject();
        }
        return( TRUE );
    }

    if( (ditem->rnode == NULL || ditem->lnode == NULL) &&
        (ditem->dialog_info == NULL || ditem->dialog_name == NULL) ) {
        return( FALSE );
    }

    if( ditem->dialog_info == NULL ) {
        ditem->dialog_info = WdeLoadDialogFromRes( res_info, ditem->lnode, ditem->is32bit );
        ditem->modified = FALSE;
    }

    if( ditem->dialog_name == NULL ) {
        ditem->dialog_name = WdeCopyWResID( &ditem->rnode->Info.ResName );
        if( ditem->dialog_name == NULL ) {
            return( FALSE );
        }
    }

    if( !WdeOpenDialogFromResInfo( res_info, ditem ) ) {
        WdeDisplayErrorMsg( WDE_DIALOGNOTOPENED );
        return( FALSE );
    }

    if( ditem->object != NULL ) {
        from_id = TRUE;
        Forward( ditem->object, RESOLVE_HELPSYMBOL, NULL, &from_id ); /* JPK */
        Forward( ditem->object, RESOLVE_SYMBOL, NULL, &from_id );
    }

    if( make_current ) {
        MakeObjectCurrent( ditem->object );
        WdeHandleGotoCurrentObject();
    }

    return( TRUE );
}

Bool WdeOpenDialogFromResInfo ( WdeResInfo *res_info, WdeResDlgItem *ditem )
{
    Bool    old;
    Bool    show;

    old = ditem->modified;

    if( res_info != NULL && ditem != NULL && ditem->dialog_info != NULL ) {
        ditem->object = WdeCreateDialogFromRes( res_info, ditem );
        if( ditem->object == NULL ) {
            return( FALSE );
        }
        if( !WdeAddControlsToDialog( res_info, ditem->object, ditem->dialog_info ) ) {
            Destroy( ditem->object, FALSE );
            ditem->object = NULL;
            return( FALSE );
        }
        show = TRUE;
        Forward( ditem->object, SHOW_WIN, &show, NULL );
        ditem->modified = old;
        return( TRUE );
    }

    return( FALSE );
}

Bool WdeAddControlsToDialog( WdeResInfo *res_info, OBJPTR dialog, WdeDialogBoxInfo *info )
{
    WdeDialogBoxControl *control;
    LIST                *clist;
    HWND                dialog_window;
    POINT               origin;

    if( !Forward( dialog, GET_WINDOW_HANDLE, &dialog_window, NULL ) ) {
        WdeWriteTrail( "WdeAddControlsToDialog: GET_WINDOW_HANDLE failed!" );
        return( FALSE );
    }

    GetOffset( &origin );

    for( clist = info->control_list; clist != NULL; clist = ListNext( clist ) ) {
        control = (WdeDialogBoxControl *)ListElement( clist );
        if( !WdeAddControlToDialog( res_info, dialog, control, &origin, dialog_window ) ) {
            WdeWriteTrail( "WdeAddControlsToDialog: add control failed!" );
        }
    }

    return( TRUE );
}

Bool WdeAddControlToDialog( WdeResInfo *res_info, OBJPTR dialog,
                            WdeDialogBoxControl *control, POINT *origin, HWND dialog_win )
{
    OBJPTR  new;
    OBJ_ID  object_id;
    RECT    control_rect;
    Bool    clear_int;

    control_rect.left = GETCTL_SIZEX( control );
    control_rect.top = GETCTL_SIZEY( control );
    control_rect.right = control_rect.left + GETCTL_SIZEW( control );
    control_rect.bottom = control_rect.top + GETCTL_SIZEH( control );

    MapDialogRect( dialog_win, &control_rect );

    WdeMapWindowRect( dialog_win, res_info->edit_win, &control_rect );

    control_rect.left += origin->x;
    control_rect.top += origin->y;
    control_rect.right += origin->x;
    control_rect.bottom += origin->y;

    object_id = WdeGetOBJIDFromControl( control );

    if( object_id == 0 ) {
        new = Create( CUSTCNTL1_OBJ, dialog, &control_rect, (OBJPTR)control );
        if( new == NULL ) {
            WdeWriteTrail( "WdeAddControlToDialog: changing to text!" );
            object_id = TEXT_OBJ;
            SETCTL_STYLE( control, SS_LEFT | WS_BORDER | WS_VISIBLE |
                                   WS_TABSTOP | WS_GROUP );
            if( GETCTL_CLASSID( control ) != NULL ) {
                WRMemFree( GETCTL_CLASSID( control ) );
            }
            SETCTL_CLASSID( control, ResNumToControlClass( CLASS_STATIC ) );
            new = Create(object_id, dialog, &control_rect, (OBJPTR)control );
        }
    } else {
        new = Create( object_id, dialog, &control_rect, (OBJPTR)control );
    }

    if( new == NULL ) {
        WdeWriteTrail( "WdeAddControlToDialog: Create failed!" );
        return( FALSE );
    }

    if( !Register( new ) ) {
        WdeWriteTrail( "WdeAddControlToDialog: Register failed!" );
        Destroy( new, FALSE );
        return( FALSE );
    }

    if( Forward( new, IS_OBJECT_CLEAR, &clear_int, NULL ) && clear_int ) {
        Forward( new, ON_TOP, NULL, NULL );
    }

    return( TRUE );
}

OBJ_ID WdeGetOBJIDFromClassNum( uint_8 class, uint_32 style )
{
    OBJ_ID id;

    id = 0;

    switch( class ) {
    case CLASS_BUTTON:
        style &= 0x0000000f;
        switch( style ) {
        case BS_PUSHBUTTON:
        case BS_DEFPUSHBUTTON:
        case BS_USERBUTTON:
        case BS_OWNERDRAW:
            id = PBUTTON_OBJ;
            break;

        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
        case BS_3STATE:
        case BS_AUTO3STATE:
            id = CBUTTON_OBJ;
            break;

        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
            id = RBUTTON_OBJ;
            break;

        case BS_GROUPBOX:
            id = GBUTTON_OBJ;
            break;

        default:
            WdeWriteTrail( "WdeGetOBJIDFromClassNum: bad button!" );
            break;
        }
        break;

    case CLASS_EDIT:
        id = EDIT_OBJ;
        break;

    case CLASS_STATIC:
        style &= 0x0000000f;
        switch( style ) {
        case SS_LEFT:
        case SS_LEFTNOWORDWRAP:
        case SS_CENTER:
        case SS_RIGHT:
        case SS_SIMPLE:
            id = TEXT_OBJ;
            break;

        case SS_BLACKRECT:
        case SS_GRAYRECT:
        case SS_WHITERECT:
        case SS_BLACKFRAME:
        case SS_GRAYFRAME:
        case SS_WHITEFRAME:
            id = FRAME_OBJ;
            break;

        case SS_ICON:
#if __NT__XX
        case SS_BITMAP:
        case SS_ENHMETAFILE:
#endif
            id = ICON_OBJ;
            break;

        default:
            WdeWriteTrail( "WdeGetOBJIDFromClassNum: bad static!" );
            break;
        }
        break;

    case CLASS_LISTBOX:
        id = LISTBOX_OBJ;
        break;

    case CLASS_SCROLLBAR:
        style &= 0x00000009;
        if( style == SBS_HORZ ) {
            id = HSCROLL_OBJ;
        } else if( style == SBS_VERT ) {
            id = VSCROLL_OBJ;
        } else if( style == SBS_SIZEBOX ) {
            id = SIZEBOX_OBJ;
        } else {
            WdeWriteTrail( "WdeGetOBJIDFromClassNum: bad scrollbar!" );
        }
        break;

    case CLASS_COMBOBOX:
        id = COMBOBOX_OBJ;
        break;

    default:
        WdeWriteTrail( "WdeGetOBJIDFromClassNum: Could not figure out control class!" );
        break;
    }

    return( id );
}

OBJ_ID WdeGetOBJIDFromControl( WdeDialogBoxControl *control )
{
    ControlClass    *class;
    uint_8          class_id;
    OBJ_ID          id;

    class = GETCTL_CLASSID( control );

    id = 0;

    if( class->Class & 0x80 ) {
        id = WdeGetOBJIDFromClassNum( class->Class, GETCTL_STYLE( control ) );
    } else {
        id = WdeGetCommonControlClassFromClassName( class->ClassName );
        if( id == 0 ) {
            class_id = WdeGetClassFromClassName( class->ClassName );
            /* assume some sort of custom class if class_id is 0 */
            if( class_id != 0 ) {
                id = WdeGetOBJIDFromClassNum( class_id, GETCTL_STYLE( control ) );
            }
        }
    }

    return( id );
}

WINEXPORT BOOL CALLBACK WdeSelectDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WdeDialogSelectInfo *si;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WdeCtl3dColorChange();
        break;

    case WM_INITDIALOG:
        si = (WdeDialogSelectInfo *)lParam;
        SET_DLGDATA( hDlg, (LONG_PTR)si );
        if( !WdeSetSelectInfo( hDlg, si ) ) {
            EndDialog( hDlg, FALSE );
        }
        ret = TRUE;
        break;

    case WM_COMMAND:
        si = (WdeDialogSelectInfo *)GET_DLGDATA( hDlg );
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WdeHelpRoutine();
            break;
        case IDB_SELECT_LISTBOX:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != LBN_DBLCLK ) {
                break;
            }
        case IDOK:
            if( !WdeGetSelectInfo( hDlg, si ) ) {
                EndDialog( hDlg, FALSE );
            }
            EndDialog( hDlg, TRUE );
            ret = TRUE;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;
        }
        break;
    }

    return( ret );
}
