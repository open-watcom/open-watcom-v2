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


#include <windows.h>
#include <win1632.h>
#include <string.h>
#include <stdlib.h>

#include "wdeglbl.h"
#include "wdemem.h"
#include "wdemsgs.h"
#include "wderesin.h"
#include "wdeopts.h"
#include "wdestat.h"
#include "wdedefin.h"
#include "wdeactn.h"
#include "wdemain.h"
#include "wderes.h"
#include "wdesdup.h"
#include "wdecust.h"
#include "wdelist.h"
#include "wdedebug.h"
#include "wde_wres.h"
#include "wdefutil.h"
#include "wdedefsz.h"
#include "wdefcust.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC      dispatcher;
    WNDPROC      win_proc;
    int          win_extra;
    char        *win_class;
    OBJPTR       object_handle;
    OBJ_ID       object_id;
    OBJPTR       control;
    WdeCustLib  *cust_lib;
    UINT         cust_index;
} WdeCustomObject;

typedef struct {
    char    *class;
    char    *new_name;
    int      win_extra;
    WNDPROC  win_proc;
} WdeCustClassNode;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern LRESULT WINEXPORT WdeCustomSuperClassProc (HWND, UINT, WPARAM, LPARAM);
extern BOOL WINEXPORT WdeCustomDispatcher     ( ACTION, WdeCustomObject *,
                                                void *, void * );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WdeChooseCustControlFromList ( LIST *, WdeDialogBoxControl *,
                                           WdeCustLib **, UINT * );
static BOOL WdeChooseCustControlType ( LPCCINFO, WdeDialogBoxControl *,
                                       WdeCustLib **, UINT *, uint_32 * );
static OBJPTR WdeMakeCustom          ( OBJPTR, RECT *, OBJPTR, int );
static OBJPTR WdeCustomCreater       ( OBJPTR, RECT *, OBJPTR, OBJ_ID,
                                       WdeDialogBoxControl *,
                                       WdeCustLib *, UINT );
static BOOL WdeCustomDestroy         ( WdeCustomObject *, BOOL *, void *);
static BOOL WdeCustomValidateAction  ( WdeCustomObject *, ACTION *, void *);
static BOOL WdeCustomCopyObject      ( WdeCustomObject *, WdeCustomObject **,
                                       WdeCustomObject * );
static BOOL WdeCustomIdentify        ( WdeCustomObject *, OBJ_ID *, void *);
static BOOL WdeCustomGetWndProc      ( WdeCustomObject *, WNDPROC *, void * );
static BOOL WdeCustomGetWindowClass  ( WdeCustomObject *, char **, void *);
static BOOL WdeCustomDefine          ( WdeCustomObject *, POINT *, void *);
static Bool WdeAddNewClassToList     ( char *, char *, int, WNDPROC );
static LIST *WdeFindClassInList      ( char *);
static Bool WdeCustomRegisterClass   ( char *, HINSTANCE, char **,
                                       int *, WNDPROC *);
static void WdeFreeClassList         ( void );
static void WdeFreeClassNode         ( WdeCustClassNode *);

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeCustomDispatch;
static WdeDialogBoxControl      *WdeDefaultCustom = NULL;
static LIST                     *WdeCustClassList = NULL;
static char                     WdeClassName[MAX_NAME];

static DISPATCH_ITEM WdeCustomActions[] = {
    { DESTROY           ,  WdeCustomDestroy             }
,   { COPY              ,  WdeCustomCopyObject          }
,   { VALIDATE_ACTION   ,  WdeCustomValidateAction      }
,   { IDENTIFY          ,  WdeCustomIdentify            }
,   { GET_WINDOW_CLASS  ,  WdeCustomGetWindowClass      }
,   { DEFINE            ,  WdeCustomDefine              }
,   { GET_WND_PROC      ,  WdeCustomGetWndProc          }
};

#define MAX_ACTIONS      (sizeof(WdeCustomActions)/sizeof (DISPATCH_ITEM))

void WdeChooseCustControlFromList ( LIST *info_list,
                                    WdeDialogBoxControl *control,
                                    WdeCustLib **lib, UINT *index )
{
    LIST       *ilist;
    LPCCINFO    lpcci;
    uint_32     min_hd;
    BOOL        found;
    char        temp[5];

    min_hd = 32;
    *lib   = NULL;
    *index = 0;

    found = FALSE;

    if ( info_list ) {
        for ( ilist = info_list; ilist; ilist = ListNext(ilist) ) {
            lpcci = (LPCCINFO) ListElement(ilist);
            found = WdeChooseCustControlType ( lpcci, control, lib, index,
                                               &min_hd );
            if ( found ) {
                break;
            }
        }
    }

    if ( !found && min_hd ) {
        ultoa ( min_hd, temp, 10 );
        WdeWriteTrail("WdeChooseCustControlFromList: "
                      "Selected custom with hamming distance:");
        WdeWriteTrail(temp);
    }

    return;
}

BOOL WdeChooseCustControlType ( LPCCINFO lpcci, WdeDialogBoxControl *control,
                                WdeCustLib **lib, UINT *index,
                                uint_32 *min_hd )
{
    BOOL    found;
    uint_32 s1;
    uint_32 s2;
    uint_32 new_min;

    found = FALSE;

    s1  = lpcci->flStyleDefault;
    s1 &= 0x0000ffff;

    s2  = GETCTL_STYLE( control ) & 0x0000ffff;

    if ( s1 == s2 ) {
        found = WdeFindLibIndexFromInfo ( lpcci, lib, index );
    } else {
        new_min = WdeHammingDistance ( s1, s2 );
        if ( new_min < *min_hd ) {
            WdeFindLibIndexFromInfo ( lpcci, lib, index );
            *min_hd = new_min;
        }
    }

    return ( found );
}

OBJPTR WINEXPORT WdeCustomCreate1 ( OBJPTR parent, RECT *obj_rect,
                                    OBJPTR handle)
{
    return ( WdeMakeCustom ( parent, obj_rect, handle, 0 ) );
}

OBJPTR WINEXPORT WdeCustomCreate2 ( OBJPTR parent, RECT *obj_rect,
                                    OBJPTR handle)
{
    return ( WdeMakeCustom ( parent, obj_rect, handle, 1 ) );
}

Bool WdeCheckForSmallRect( OBJPTR parent, WdeCustLib *cust_lib,
                           UINT cust_index, RECT *obj_rect )
{
    UINT                width;
    UINT                height;
    WdeResizeRatio      r;

    if( !parent || !cust_lib || !obj_rect ) {
        return ( FALSE );
    }

    /* check if the objects size is greater than min allowed */
    if( ( ( obj_rect->right  - obj_rect->left ) >= MIN_X ) ||
        ( ( obj_rect->bottom - obj_rect->top  ) >= MIN_Y ) ) {
        return ( TRUE );
    }

    width = cust_lib->lpcci[cust_index].cxDefault;
    height = cust_lib->lpcci[cust_index].cyDefault;

    if( !Forward( parent, GET_RESIZER, &r, NULL ) ) {
        return( FALSE );
    }

    WdeMapCustomSize( &width, &height, &r );

    obj_rect->right  = obj_rect->left + width;
    obj_rect->bottom = obj_rect->top  + height;

    return( TRUE );
}

OBJPTR WdeMakeCustom ( OBJPTR parent, RECT *obj_rect,
                       OBJPTR handle, int which )
{
    DialogStyle         style;
    OBJPTR              ret;
    WdeCustLib          *cust_lib;
    UINT                cust_index;
    char                *class_name;
    WdeDialogBoxControl *control;
    LIST                *info_list;

    info_list  = NULL;
    cust_lib   = NULL;
    cust_index = 0;

    if ( handle == NULL ) {
        WdeGetCurrentCustControl ( which, &cust_lib, &cust_index );

        if ( !cust_lib ) {
            if ( !WdeSetCurrentCustControl (which) ) {
                WdeWriteTrail("WdeMakeCustom: "
                              "WdeSetCurrentCustControl failed!");
                return ( NULL );
            }
            WdeGetCurrentCustControl ( which, &cust_lib, &cust_index );
        }

        if ( cust_lib == NULL ) {
            WdeWriteTrail("WdeMakeCustom: No custom controls!");
            return ( NULL );
        }

        style = cust_lib->lpcci[cust_index].flStyleDefault;
        style &= ~WS_POPUP;
        style |= ( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP );
        SETCTL_STYLE( WdeDefaultCustom, style );
        SETCTL_TEXT( WdeDefaultCustom,
                     ResStrToNameOrOrd(
                         cust_lib->lpcci[cust_index].szTextDefault ) );
        SETCTL_CLASSID( WdeDefaultCustom,
                     WdeStrToControlClass(
                         cust_lib->lpcci[cust_index].szClass ) );

        SETCTL_ID( WdeDefaultCustom, WdeGetNextControlID() );

        control = WdeDefaultCustom;
    } else {
        control = (WdeDialogBoxControl *) handle;
        class_name = WdeControlClassToStr ( GETCTL_CLASSID( control ) );
        if( class_name ) {
            WdeFindClassInAllCustLibs ( class_name, &info_list );
            WdeMemFree ( class_name );
        }
        if( !info_list ) {
            WdeSetStatusByID( -1, WDE_NOCUSTOMFORCLASS );
            return( NULL );
        }
        WdeChooseCustControlFromList ( info_list, control,
                                       &cust_lib, &cust_index );
        if( !cust_lib ) {
            WdeWriteTrail("WdeMakeCustom: "
                          "No custom control fits this class & style!");
            WdeSetStatusByID( -1, WDE_CANTFINDCUSTOM );
            return( NULL );
        }
    }

    WdeCheckForSmallRect( parent, cust_lib, cust_index, obj_rect );

    ret = WdeCustomCreater( parent, obj_rect, NULL,
                            ( CUSTCNTL1_OBJ + which ),
                            control, cust_lib, cust_index );

    if( handle == NULL ) {
        WdeMemFree( GETCTL_TEXT( WdeDefaultCustom ) );
        WdeMemFree( GETCTL_CLASSID( WdeDefaultCustom ) );
    }

    SETCTL_STYLE( WdeDefaultCustom, 0 );
    SETCTL_TEXT( WdeDefaultCustom, NULL );
    SETCTL_CLASSID( WdeDefaultCustom, NULL );

    return( ret );
}

void WdeFreeClassList ( void )
{
    LIST             *clist;
    WdeCustClassNode *node;

    for ( clist = WdeCustClassList; clist; clist = ListNext( clist ) ) {
        node = (WdeCustClassNode *) ListElement ( clist );
        WdeFreeClassNode ( node );
    }

    return;
}

void WdeFreeClassNode ( WdeCustClassNode *node )
{
    if ( node != NULL ) {
        if ( node->class != NULL ) {
            WdeMemFree ( node->class );
        }
        if ( node->new_name != NULL ) {
            WdeMemFree ( node->new_name );
        }
        WdeMemFree ( node );
    }
}

Bool WdeAddNewClassToList ( char *class, char *new_name,
                            int win_extra, WNDPROC win_proc )
{
    WdeCustClassNode *node;
    char             *str;

    node = (WdeCustClassNode *) WdeMemAlloc ( sizeof (WdeCustClassNode) );
    if ( node == NULL ) {
        WdeWriteTrail("WdeAddNewClassToList: node alloc failed!");
        return ( FALSE );
    }

    str = WdeStrDup ( class );
    if ( str == NULL ) {
        WdeWriteTrail("WdeAddNewClassToList: class strdup failed!");
        WdeMemFree ( node );
        return ( FALSE );
    }
    node->class     = str;

    str = WdeStrDup ( new_name );
    if ( str == NULL ) {
        WdeWriteTrail("WdeAddNewClassToList: new_name alloc failed!");
        WdeMemFree ( node->class );
        WdeMemFree ( node );
        return ( FALSE );
    }
    node->new_name  = str;

    node->win_extra = win_extra;
    node->win_proc  = win_proc;

    WdeInsertObject ( &WdeCustClassList, (void *)node );

    return ( TRUE );
}

LIST *WdeFindClassInList ( char *class )
{
    LIST             *clist;
    WdeCustClassNode *node;

    for ( clist = WdeCustClassList; clist; clist = ListNext( clist ) ) {
        node = (WdeCustClassNode *) ListElement ( clist );
        if ( !strcmpi ( node->class, class ) ) {
            break;
        }
    }

    return ( clist );
}

Bool WdeCustomRegisterClass( char *class, HINSTANCE inst, char **new_name,
                             int *win_extra, WNDPROC *win_proc )
{
    WdeCustClassNode *node;
    WNDCLASS          wc;
    LIST             *clist;

    if( ( clist = WdeFindClassInList ( class ) ) != NULL ) {
        node = (WdeCustClassNode *) ListElement( clist );
        *win_extra = node->win_extra;
        *win_proc  = node->win_proc;
        *new_name = WdeStrDup( node->new_name );
        if( *new_name == NULL ) {
            WdeWriteTrail("WdeCustomRegisterClass: new_name alloc failed!");
            return( FALSE );
        }
        return( TRUE );
    }

    if( !GetClassInfo( inst, class, &wc ) ) {
        WdeWriteTrail("WdeCustomRegisterClass: GetClassInfo failed!");
        return( FALSE );
    }

    *new_name = (char *) WdeMemAlloc( strlen( class ) + 5 );
    if( *new_name == NULL ) {
        WdeWriteTrail("WdeCustomRegisterClass: new_name alloc failed!");
        return( FALSE );
    }
    strcpy( *new_name, "WDE_" );
    strcat( *new_name, class );

    *win_extra = wc.cbWndExtra;
    *win_proc  = wc.lpfnWndProc;

    if( wc.style & CS_GLOBALCLASS ) {
        wc.style ^= CS_GLOBALCLASS;
    }
    if( wc.style & CS_PARENTDC ) {
        wc.style ^= CS_PARENTDC;
    }
    wc.style |= ( CS_HREDRAW | CS_VREDRAW );

    wc.hInstance      = WdeApplicationInstance;
    wc.lpszClassName  = *new_name;
    wc.cbWndExtra    += sizeof( WNDPROC );
    //wc.lpfnWndProc    = WdeCustomSuperClassProc;

    if( !RegisterClass( &wc ) ) {
        WdeWriteTrail("WdeCustomRegisterClass: RegisterClass failed!");
        //subclass controls instead of super classing them makes this
        // much less fatal
        //WdeMemFree ( *new_name );
        //return ( FALSE );
    }

    if( !WdeAddNewClassToList(class, *new_name, *win_extra, *win_proc) ) {
        WdeWriteTrail("WdeCustomRegisterClass: AddNewClass failed!");
        WdeMemFree( *new_name );
        return( FALSE );
    }

    return( TRUE );
}

OBJPTR WdeCustomCreater ( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                          OBJ_ID id, WdeDialogBoxControl *info,
                          WdeCustLib *cust_lib, UINT cust_index )
{
    WdeCustomObject *new;
    char            *class;

    WdeDebugCreate("Custom", parent, obj_rect, handle);

    WdeMemValidate ( parent );

    if ( parent == NULL ) {
        WdeWriteTrail("WdeCustomCreate: Custom has no parent!");
        return ( NULL );
    }

    new = (WdeCustomObject *) WdeMemAlloc ( sizeof(WdeCustomObject) );
    if ( new == NULL ) {
        WdeWriteTrail("WdeCustomCreate: Object malloc failed");
        return ( NULL );
    }

    class = cust_lib->lpcci[cust_index].szClass;

    if ( !WdeCustomRegisterClass ( class, cust_lib->inst,
                                   &new->win_class,
                                   &new->win_extra, &new->win_proc ) ) {
        WdeWriteTrail("WdeCustomCreate: WdeCustomRegisterClass failed!");
        WdeMemFree ( new );
        return ( NULL );
    }

    new->dispatcher  = WdeCustomDispatch;
    new->object_id   = id;
    new->cust_lib    = cust_lib;
    new->cust_index  = cust_index;

    if ( handle ==  NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle);

    if (new->control == NULL) {
        WdeWriteTrail("WdeCustomCreate: CONTROL_OBJ not created!");
        WdeMemFree ( new );
        return ( NULL );
    }

    if (!Forward ( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL) ) {
        WdeWriteTrail("WdeCustomCreate: SET_OBJECT_INFO failed!");
        Destroy ( new->control, FALSE );
        WdeMemFree ( new );
        return ( NULL );
    }

    if (!Forward ( (OBJPTR)new->object_handle, CREATE_WINDOW,
                   NULL, NULL) ) {
        WdeWriteTrail("WdeCustomCreate: CREATE_WINDOW failed!");
        Destroy ( new->control, FALSE );
        WdeMemFree ( new );
        return ( NULL );
    }

    return ( new );
}

BOOL WINEXPORT WdeCustomDispatcher ( ACTION act, WdeCustomObject *obj,
                                     void *p1, void *p2)
{
    int     i;

    WdeDebugDispatch("Custom", act, obj, p1, p2);

    WdeMemChkRange ( obj, sizeof(WdeCustomObject) );

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCustomActions[i].id == act ) {
            return( (WdeCustomActions[i].rtn)( obj, p1, p2 ) );
        }
    }

    return (Forward ((OBJPTR)obj->control, act, p1, p2));
}

Bool WdeCustomInit ( Bool first )
{
    _wde_touch(first);
    WdeApplicationInstance = WdeGetAppInstance();

    WdeDefaultCustom = WdeAllocDialogBoxControl ();
    if ( !WdeDefaultCustom ) {
        WdeWriteTrail ("WdeCustomInit: Alloc of control failed!");
        return( FALSE );
    }

    /* set up the default control structure */
    SETCTL_STYLE( WdeDefaultCustom, 0 );
    SETCTL_ID( WdeDefaultCustom, 0 );
    SETCTL_EXTRABYTES( WdeDefaultCustom, 0 );
    SETCTL_SIZEX( WdeDefaultCustom, 0 );
    SETCTL_SIZEY( WdeDefaultCustom, 0 );
    SETCTL_SIZEW( WdeDefaultCustom, 0 );
    SETCTL_SIZEH( WdeDefaultCustom, 0 );
    SETCTL_TEXT( WdeDefaultCustom, NULL );
    SETCTL_CLASSID( WdeDefaultCustom, NULL );

    WdeCustomDispatch = MakeProcInstance((FARPROC)WdeCustomDispatcher,
                                           WdeGetAppInstance());

    return( TRUE );
}

void WdeCustomFini ( void )
{
    WdeFreeClassList        ();
    WdeFreeDialogBoxControl ( &WdeDefaultCustom );
    FreeProcInstance        ( WdeCustomDispatch );
}

BOOL WdeCustomDestroy ( WdeCustomObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    if ( !Forward ( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail("WdeCustomDestroy: Control DESTROY failed");
        return ( FALSE );
    }

    if ( obj->win_class != NULL ) {
        WdeMemFree( obj->win_class );
    }

    WdeMemFree( obj );

    return ( TRUE );
}

BOOL WdeCustomValidateAction ( WdeCustomObject *obj, ACTION *act, void *p2 )
{
    int     i;

    for ( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCustomActions[i].id == *act ) {
            return ( TRUE );
        }
    }

    return ( ValidateAction( (OBJPTR) obj->control, *act, p2 ) );
}

BOOL WdeCustomCopyObject ( WdeCustomObject *obj, WdeCustomObject **new,
                           WdeCustomObject *handle )
{
    if (new == NULL) {
        WdeWriteTrail("WdeCustomCopyObject: Invalid new object!");
        return ( FALSE );
    }

    *new = (WdeCustomObject *) WdeMemAlloc ( sizeof(WdeCustomObject) );

    if ( *new == NULL ) {
        WdeWriteTrail("WdeCustomCopyObject: Object malloc failed");
        return ( FALSE );
    }

    (*new)->dispatcher  = obj->dispatcher;
    (*new)->win_proc    = obj->win_proc;
    (*new)->win_extra   = obj->win_extra;
    (*new)->object_id   = obj->object_id;
    (*new)->cust_lib    = obj->cust_lib;
    (*new)->cust_index  = obj->cust_index;

    (*new)->win_class  = WdeStrDup ( obj->win_class );
    if ( (*new)->win_class == NULL ) {
        WdeWriteTrail("WdeCustomCopyObject: Class alloc failed!");
        WdeMemFree ( (*new) );
        return ( FALSE );
    }

    if ( handle ==  NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if (!CopyObject(obj->control, &((*new)->control), (*new)->object_handle)) {
        WdeWriteTrail("WdeCustomCopyObject: Control not created!");
        WdeMemFree ( (*new) );
        return ( FALSE );
    }

    return ( TRUE );
}

BOOL WdeCustomIdentify ( WdeCustomObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(p2);

    *id = obj->object_id;

    return ( TRUE );
}

BOOL WdeCustomGetWndProc( WdeCustomObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *proc = WdeCustomSuperClassProc;

    return ( TRUE );
}

BOOL WdeCustomGetWindowClass ( WdeCustomObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch(obj);
    _wde_touch(p2);

    *class = obj->win_class;

    return ( TRUE );
}

BOOL WdeCustomDefine ( WdeCustomObject *obj, POINT *pnt, void *p2 )
{
    BOOL                redraw;
    HWND                dialog_owner;
    LPCCINFO            lpcci;
    CCSTYLE             ccs;
    WdeDefineObjectInfo o_info;
    WdeDialogBoxControl *info;
    char                *text;
    int                 tlen;
    LPFNCCSTYLE         proc;

    /* touch unused vars to get rid of warning */
    _wde_touch(pnt);
    _wde_touch(p2);

    if ( !Forward ( obj, GET_WINDOW_HANDLE, &o_info.win, NULL ) ) {
        WdeWriteTrail("WdeControlDefine: GET_WINDOW_HANDLE failed!");
        return ( FALSE );
    }

    if ( !Forward ( obj->object_handle, GET_OBJECT_INFO,
                    &(o_info.info.c.info), &(o_info.symbol) ) ) {
        WdeWriteTrail("WdeCustomDefine: GET_OBJECT_INFO failed!");
        return ( FALSE );
    }

    if ( !WdeGetOption ( WdeOptUseDefDlg ) ) {
        o_info.obj       = obj->object_handle;
        o_info.obj_id    = obj->object_id;
        o_info.mask      = 0xffff;
        o_info.hook_func = WdeWinStylesHook;
        o_info.set_func  = NULL;
        o_info.get_func  = NULL;
        o_info.res_info  = WdeGetCurrentRes();
        return ( WdeGenericDefine ( &o_info ) );
    }

    info = o_info.info.c.info;

    dialog_owner = WdeGetMainWindowHandle();

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_DEFININGCUSTOM, -1 );

    lpcci = &(obj->cust_lib->lpcci[obj->cust_index]);

    ccs.flStyle    = GETCTL_STYLE( info );
    ccs.flExtStyle = NULL;
    ccs.lgid       = NULL;
    ccs.wReserved1 = NULL;

    if( text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( info ), 10 ) ) {
        tlen = strlen ( text );
        if ( tlen < CCHCCTEXT ) {
            strcpy ( ccs.szText, text );
        } else {
            memcpy ( ccs.szText, text, CCHCCTEXT );
            ccs.szText[CCHCCTEXT-1] = '\0';
        }
        WdeMemFree ( text );
    } else {
         ccs.szText[0] = '\0';
    }

    proc = lpcci->lpfnStyle;

    redraw = (BOOL) (*proc) ( dialog_owner, &ccs );

    if ( redraw ) {

        SETCTL_STYLE( info, ( DialogStyle ) ccs.flStyle );

        if ( !Forward ( obj->object_handle, DESTROY_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail("WdeCustumDefine: DESTROY_WINDOW failed!");
            return ( FALSE );
        }

        if (!Forward (obj->object_handle, CREATE_WINDOW, NULL, NULL)) {
            WdeWriteTrail("WdeCustumDefine: CREATE_WINDOW failed!");
            return ( FALSE );
        }

        Notify ( obj->object_handle, CURRENT_OBJECT, NULL );
    }

    WdeSetStatusReadyText();

    return ( TRUE );
}

LRESULT WINEXPORT WdeCustomSuperClassProc( HWND hWnd, UINT message,
                                           WPARAM wParam,
                                           volatile LPARAM lParam )
{
    WNDPROC           wnd_proc;
    int               extra;
    LIST             *clist;
    WdeCustClassNode *node;

    if ( WdeProcessMouse ( hWnd, message, wParam, lParam ) ) {
        return ( FALSE );
    }

    extra = (int) GET_CBWNDEXTRA( hWnd );
    extra -= sizeof(WNDPROC);
    wnd_proc = (WNDPROC) GetWindowLong ( hWnd, extra );
    if ( wnd_proc == NULL ) {
        if ( !GetClassName ( hWnd, WdeClassName, MAX_NAME ) ) {
            WdeWriteTrail("WdeCustomSuperClassProc: GetClassName failed!");
            return ( FALSE );
        }
        clist = WdeFindClassInList ( &(WdeClassName[4]) );
        if ( clist == NULL ) {
            WdeWriteTrail("WdeCustomSuperClassProc: FindClassInList failed!");
            return ( FALSE );
        }
        node = (WdeCustClassNode *) ListElement ( clist );
        wnd_proc = node->win_proc;
        if ( wnd_proc == NULL ) {
            WdeWriteTrail("WdeCustomSuperClassProc: NULL wnd_proc!");
            return ( FALSE );
        }
        SetWindowLong ( hWnd, extra, (LONG) wnd_proc );
    }

    return ( CallWindowProc ( wnd_proc, hWnd, message, wParam, lParam ) );

}

