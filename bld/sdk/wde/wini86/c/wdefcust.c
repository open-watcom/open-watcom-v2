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


#include "precomp.h"
#include <string.h>
#include <stdlib.h>

#include "wdeglbl.h"
#include "wdemem.h"
#include "rcstr.gh"
#include "wderes.h"
#include "wdeopts.h"
#include "wdestat.h"
#include "wdedefin.h"
#include "wdeactn.h"
#include "wdemain.h"
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
    FARPROC         dispatcher;
    WNDPROC         win_proc;
    int             win_extra;
    char            *win_class;
    OBJPTR          object_handle;
    OBJ_ID          object_id;
    OBJPTR          control;
    WdeCustControl  *cust_info;
    UINT            cust_type;
} WdeCustomObject;

typedef struct {
    char    *class;
    char    *new_name;
    int     win_extra;
    WNDPROC win_proc;
} WdeCustClassNode;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern LRESULT WINEXPORT    WdeCustomSuperClassProc( HWND, UINT, WPARAM, LPARAM );
extern BOOL WINEXPORT       WdeCustomDispatcher( ACTION, WdeCustomObject *, void *, void * );
extern WORD WINEXPORT       WdeIDToStr( WORD, LPSTR, WORD );
extern DWORD WINEXPORT      WdeStrToID( LPSTR );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void     WdeChooseCustControlFromList( LIST *, WdeDialogBoxControl *, WdeCustControl **, UINT * );
static BOOL     WdeChooseCustControlType( WdeCustControl *, WdeDialogBoxControl *, WdeCustControl **, UINT *, uint_32 * );
static OBJPTR   WdeMakeCustom( OBJPTR, RECT *, OBJPTR, int );
static OBJPTR   WdeCustomCreater( OBJPTR, RECT *, OBJPTR, OBJ_ID, WdeDialogBoxControl *, WdeCustControl *, UINT );
static BOOL     WdeCustomDestroy( WdeCustomObject *, BOOL *, void * );
static BOOL     WdeCustomValidateAction( WdeCustomObject *, ACTION *, void * );
static BOOL     WdeCustomCopyObject( WdeCustomObject *, WdeCustomObject **, WdeCustomObject * );
static BOOL     WdeCustomIdentify( WdeCustomObject *, OBJ_ID *, void * );
static BOOL     WdeCustomGetWndProc( WdeCustomObject *, WNDPROC *, void * );
static BOOL     WdeCustomGetWindowClass( WdeCustomObject *, char **, void * );
static BOOL     WdeCustomDefine( WdeCustomObject *, POINT *, void * );
static Bool     WdeAddNewClassToList( char *, char *, int, WNDPROC );
static LIST     *WdeFindClassInList( char * );
static Bool     WdeCustomRegisterClass( char *, HINSTANCE, char **, int *, WNDPROC * );
static void     WdeFreeClassList( void );
static void     WdeFreeClassNode( WdeCustClassNode * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE                WdeApplicationInstance;
static FARPROC                  WdeCustomDispatch;
static FARPROC                  WdeStr2ID;
static FARPROC                  WdeID2Str;
static WdeDialogBoxControl      *WdeDefaultCustom = NULL;
static LIST                     *WdeCustClassList = NULL;
static char                     WdeClassName[MAX_NAME];

static DISPATCH_ITEM WdeCustomActions[] = {
    { DESTROY,          (BOOL (*)( OBJPTR, void *, void * ))WdeCustomDestroy            },
    { COPY,             (BOOL (*)( OBJPTR, void *, void * ))WdeCustomCopyObject         },
    { VALIDATE_ACTION,  (BOOL (*)( OBJPTR, void *, void * ))WdeCustomValidateAction     },
    { IDENTIFY,         (BOOL (*)( OBJPTR, void *, void * ))WdeCustomIdentify           },
    { GET_WINDOW_CLASS, (BOOL (*)( OBJPTR, void *, void * ))WdeCustomGetWindowClass     },
    { DEFINE,           (BOOL (*)( OBJPTR, void *, void * ))WdeCustomDefine             },
    { GET_WND_PROC,     (BOOL (*)( OBJPTR, void *, void * ))WdeCustomGetWndProc         }
};

#define MAX_ACTIONS     (sizeof( WdeCustomActions ) / sizeof( DISPATCH_ITEM ))

void WdeChooseCustControlFromList( LIST *info_list, WdeDialogBoxControl *control,
                                   WdeCustControl **rinfo, UINT *rtype )
{
    LIST            *ilist;
    WdeCustControl  *info;
    uint_32         min_hd;
    BOOL            found;
    char            temp[5];

    min_hd = 32;
    *rinfo = NULL;

    found = FALSE;

    if( info_list != NULL ) {
        for( ilist = info_list; ilist != NULL; ilist = ListNext( ilist ) ) {
            info = (WdeCustControl *)ListElement( ilist );
            found = WdeChooseCustControlType( info, control, rinfo, rtype, &min_hd );
            if( found ) {
                break;
            }
        }
    }

    if( !found ) {
        ultoa( min_hd, temp, 10 );
        WdeWriteTrail( "WdeChooseCustControlFromList: "
                       "Selected custom with hamming distance:" );
        WdeWriteTrail( temp );
    }
}

BOOL WdeChooseCustControlType( WdeCustControl *info, WdeDialogBoxControl *control,
                               WdeCustControl **rinfo, UINT *rtype, uint_32 *min_hd )
{
    uint_16 type;
    BOOL    found;
    uint_32 s1;
    uint_32 s2;
    uint_32 new_min;

    /* if this class just has one type ASSUME (YIKES!!) that this is the
     * one we are looking for.
     */
    if( info->control_info.ms.wCtlTypes <= 1 ) {
        *rinfo = info;
        *rtype = 0;
        return( TRUE );
    }

    found = FALSE;

    for( type = 0; type < info->control_info.ms.wCtlTypes; type++ ) {
        if( info->ms_lib ) {
            s1 = info->control_info.ms.Type[type].dwStyle;
        } else {
            s1 = info->control_info.bor.Type[type].dwStyle;
        }
        s1 &= 0x0000ffff;
        s2 = GETCTL_STYLE( control ) & 0x0000ffff;
        if( s1 == s2 ) {
            found = TRUE;
            break;
        }
        new_min = WdeHammingDistance( s1, s2 );
        if( new_min < *min_hd ) {
            *rinfo = info;
            *rtype = type;
            *min_hd = new_min;
        }
    }

    if( found ) {
        *rinfo = info;
        *rtype = type;
    }

    return( found );
}

OBJPTR WINEXPORT WdeCustomCreate1( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    return( WdeMakeCustom( parent, obj_rect, handle, 0 ) );
}

OBJPTR WINEXPORT WdeCustomCreate2( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    return( WdeMakeCustom( parent, obj_rect, handle, 1 ) );
}

Bool WdeCheckForSmallRect( OBJPTR parent, WdeCustControl *cust_info,
                           UINT cust_type, RECT *obj_rect )
{
    uint_32             width;
    uint_32             height;
    WdeResizeRatio      r;

    if( parent == NULL || cust_info == NULL || obj_rect == NULL ) {
        return( FALSE );
    }

    /* check if the objects size is greater than min allowed */
    if( obj_rect->right - obj_rect->left >= MIN_X ||
        obj_rect->bottom - obj_rect->top >= MIN_Y ) {
        return( TRUE );
    }

    if( cust_info->ms_lib ) {
        width = cust_info->control_info.ms.Type[cust_type].wWidth;
        height = cust_info->control_info.ms.Type[cust_type].wHeight;
    } else {
        width = cust_info->control_info.bor.Type[cust_type].wWidth;
        height = cust_info->control_info.bor.Type[cust_type].wHeight;
    }

    if( !Forward( parent, GET_RESIZER, &r, NULL ) ) {
        return( FALSE );
    }

    WdeMapCustomSize( &width, &height, &r );

    obj_rect->right = obj_rect->left + width;
    obj_rect->bottom = obj_rect->top  + height;

    return( TRUE );
}

OBJPTR WdeMakeCustom( OBJPTR parent, RECT *obj_rect, OBJPTR handle, int which )
{
    DialogStyle         style;
    OBJPTR              ret;
    WdeCustControl      *cust_info;
    UINT                cust_type;
    char                *class_name;
    WdeDialogBoxControl *control;
    LIST                *info_list;

    info_list = NULL;
    cust_info = NULL;
    cust_type = 0;

    if( handle == NULL ) {
        WdeGetCurrentCustControl( which, &cust_info, &cust_type );

        if( cust_info == NULL ) {
            if( !WdeSetCurrentCustControl( which ) ) {
                WdeWriteTrail( "WdeMakeCustom: WdeSetCurrentCustControl failed!" );
                return( NULL );
            }
            WdeGetCurrentCustControl( which, &cust_info, &cust_type );
        }

        if( cust_info == NULL ) {
            WdeWriteTrail( "WdeMakeCustom: No custom controls!" );
            return( NULL );
        }

        if( cust_info->ms_lib ) {
            style = cust_info->control_info.ms.Type[cust_type].dwStyle;
            SETCTL_TEXT( WdeDefaultCustom,
                         ResStrToNameOrOrd( cust_info->control_info.ms.szTitle ) );
            SETCTL_CLASSID( WdeDefaultCustom,
                            WdeStrToControlClass( cust_info->control_info.ms.szClass ) );
        } else {
            style = cust_info->control_info.bor.Type[cust_type].dwStyle;
            SETCTL_TEXT( WdeDefaultCustom,
                         ResStrToNameOrOrd( cust_info->control_info.bor.szTitle ) );
            SETCTL_CLASSID( WdeDefaultCustom,
                            WdeStrToControlClass( cust_info->control_info.bor.szClass ) );
        }

        SETCTL_ID( WdeDefaultCustom, WdeGetNextControlID() );

        style &= ~WS_POPUP;
        style |= (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP);

        SETCTL_STYLE( WdeDefaultCustom, style );

        control = WdeDefaultCustom;

    } else {
        control = (WdeDialogBoxControl *)handle;
        class_name = WdeControlClassToStr( GETCTL_CLASSID( control ) );
        if( class_name != NULL ) {
            WdeFindClassInAllCustLibs( class_name, &info_list );
            WdeMemFree( class_name );
        }
        if( info_list == NULL ) {
            WdeWriteTrail( "WdeMakeCustom: There are no custom controls of this class!" );
            WdeSetStatusByID( -1, WDE_NOCUSTOMFORCLASS );
            return( NULL );
        }
        WdeChooseCustControlFromList( info_list, control, &cust_info, &cust_type );
        if( cust_info == NULL ) {
            WdeWriteTrail( "WdeMakeCustom: No custom control fits this class & style!" );
            WdeSetStatusByID( -1, WDE_CANTFINDCUSTOM );
            return( NULL );
        }
    }

    WdeCheckForSmallRect( parent, cust_info, cust_type, obj_rect );

    ret = WdeCustomCreater( parent, obj_rect, NULL, CUSTCNTL1_OBJ + which,
                            control, cust_info, cust_type );

    if( handle == NULL ) {
        WdeMemFree( GETCTL_TEXT( WdeDefaultCustom ) );
        WdeMemFree( GETCTL_CLASSID( WdeDefaultCustom ) );
    }

    SETCTL_STYLE( WdeDefaultCustom, 0 );
    SETCTL_TEXT( WdeDefaultCustom, NULL );
    SETCTL_CLASSID( WdeDefaultCustom, NULL );

    return( ret );
}

void WdeFreeClassList( void )
{
    LIST             *clist;
    WdeCustClassNode *node;

    for( clist = WdeCustClassList; clist != NULL; clist = ListNext( clist ) ) {
        node = (WdeCustClassNode *)ListElement( clist );
        WdeFreeClassNode( node );
    }
}

void WdeFreeClassNode( WdeCustClassNode *node )
{
    if( node != NULL ) {
        if( node->class != NULL ) {
            WdeMemFree( node->class );
        }
        if( node->new_name != NULL ) {
            WdeMemFree( node->new_name );
        }
        WdeMemFree( node );
    }
}

Bool WdeAddNewClassToList( char *class, char *new_name,
                           int win_extra, WNDPROC win_proc )
{
    WdeCustClassNode *node;
    char             *str;

    node = (WdeCustClassNode *)WdeMemAlloc( sizeof( WdeCustClassNode ) );
    if( node == NULL ) {
        WdeWriteTrail( "WdeAddNewClassToList: node alloc failed!" );
        return( FALSE );
    }

    str = WdeStrDup( class );
    if( str == NULL ) {
        WdeWriteTrail( "WdeAddNewClassToList: class strdup failed!" );
        WdeMemFree( node );
        return( FALSE );
    }
    node->class = str;

    str = WdeStrDup( new_name );
    if( str == NULL ) {
        WdeWriteTrail( "WdeAddNewClassToList: new_name alloc failed!" );
        WdeMemFree( node->class );
        WdeMemFree( node );
        return( FALSE );
    }
    node->new_name = str;

    node->win_extra = win_extra;
    node->win_proc = win_proc;

    WdeInsertObject( &WdeCustClassList, (void *)node );

    return( TRUE );
}

LIST *WdeFindClassInList( char *class )
{
    LIST             *clist;
    WdeCustClassNode *node;

    for( clist = WdeCustClassList; clist != NULL; clist = ListNext( clist ) ) {
        node = (WdeCustClassNode *)ListElement( clist );
        if( !strcmpi( node->class, class ) ) {
            break;
        }
    }

    return( clist );
}

Bool WdeCustomRegisterClass( char *class, HINSTANCE inst, char **new_name,
                             int *win_extra, WNDPROC *win_proc )
{
    WdeCustClassNode    *node;
    WNDCLASS            wc;
    LIST                *clist;

    if( (clist = WdeFindClassInList( class )) != NULL ) {
        node = (WdeCustClassNode *)ListElement( clist );
        *win_extra = node->win_extra;
        *win_proc = node->win_proc;
        *new_name = WdeStrDup( node->new_name );
        if( *new_name == NULL ) {
            WdeWriteTrail( "WdeCustomRegisterClass: new_name alloc failed!" );
            return( FALSE );
        }
        return( TRUE );
    }

    if( !GetClassInfo( inst, class, &wc ) ) {
        WdeWriteTrail( "WdeCustomRegisterClass: GetClassInfo failed!" );
        return( FALSE );
    }

    *new_name = (char *)WdeMemAlloc( strlen( class ) + 5 );
    if( *new_name == NULL ) {
        WdeWriteTrail( "WdeCustomRegisterClass: new_name alloc failed!" );
        return( FALSE );
    }
    strcpy( *new_name, "WDE_" );
    strcat( *new_name, class );

    *win_extra = wc.cbWndExtra;
    *win_proc = wc.lpfnWndProc;

    if( wc.style & CS_GLOBALCLASS ) {
        wc.style ^= CS_GLOBALCLASS;
    }
    if( wc.style & CS_PARENTDC ) {
        wc.style ^= CS_PARENTDC;
    }
    wc.style |= (CS_HREDRAW | CS_VREDRAW);

    wc.hInstance = WdeApplicationInstance;
    wc.lpszClassName = *new_name;
    wc.cbWndExtra += sizeof( WNDPROC );
    //wc.lpfnWndProc = WdeCustomSuperClassProc;

    if( !RegisterClass( &wc ) ) {
        WdeWriteTrail( "WdeCustomRegisterClass: RegisterClass failed!" );
        // subclass controls instead of superclassing them makes this
        // much less fatal
        //WdeMemFree( *new_name );
        //return( FALSE );
    }

    if( !WdeAddNewClassToList( class, *new_name, *win_extra, *win_proc ) ) {
        WdeWriteTrail( "WdeCustomRegisterClass: AddNewClass failed!" );
        WdeMemFree( *new_name );
        return( FALSE );
    }

    return( TRUE );
}

OBJPTR WdeCustomCreater( OBJPTR parent, RECT *obj_rect, OBJPTR handle,
                         OBJ_ID id, WdeDialogBoxControl *info,
                         WdeCustControl *cust_info, UINT cust_type )
{
    WdeCustomObject *new;
    char            *class;

    WdeDebugCreate( "Custom", parent, obj_rect, handle );

    WdeMemValidate( parent );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeCustomCreate: Custom has no parent!" );
        return( NULL );
    }

    new = (WdeCustomObject *)WdeMemAlloc( sizeof( WdeCustomObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeCustomCreate: Object malloc failed" );
        return( NULL );
    }

    if( cust_info->ms_lib ) {
        class = cust_info->control_info.ms.szClass;
    } else {
        class = cust_info->control_info.bor.szClass;
    }

    if( !WdeCustomRegisterClass( class, cust_info->lib->inst, &new->win_class,
                                 &new->win_extra, &new->win_proc ) ) {
        WdeWriteTrail( "WdeCustomCreate: WdeCustomRegisterClass failed!" );
        WdeMemFree( new );
        return( NULL );
    }

    new->dispatcher = WdeCustomDispatch;
    new->object_id = id;
    new->cust_info = cust_info;
    new->cust_type = cust_type;

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->control = Create( CONTROL_OBJ, parent, obj_rect, new->object_handle );

    if( new->control == NULL ) {
        WdeWriteTrail( "WdeCustomCreate: CONTROL_OBJ not created!" );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, SET_OBJECT_INFO, info, NULL ) ) {
        WdeWriteTrail( "WdeCustomCreate: SET_OBJECT_INFO failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    if( !Forward( (OBJPTR)new->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
        WdeWriteTrail( "WdeCustomCreate: CREATE_WINDOW failed!" );
        Destroy( new->control, FALSE );
        WdeMemFree( new );
        return( NULL );
    }

    return( new );
}

BOOL WINEXPORT WdeCustomDispatcher( ACTION act, WdeCustomObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Custom", act, obj, p1, p2 );

    WdeMemChkRange( obj, sizeof( WdeCustomObject ) );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCustomActions[i].id == act ) {
            return( WdeCustomActions[i].rtn( obj, p1, p2 ) );
        }
    }

    return( Forward( (OBJPTR)obj->control, act, p1, p2 ) );
}

Bool WdeCustomInit( Bool first )
{
    _wde_touch( first );
    WdeApplicationInstance = WdeGetAppInstance();

    WdeDefaultCustom = WdeAllocDialogBoxControl();
    if( WdeDefaultCustom == NULL ) {
        WdeWriteTrail( "WdeCustomInit: Alloc of control failed!" );
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

    WdeCustomDispatch = MakeProcInstance( (FARPROC)WdeCustomDispatcher,
                                          WdeGetAppInstance() );

    WdeStr2ID = MakeProcInstance ( (FARPROC)WdeStrToID, WdeApplicationInstance );
    WdeID2Str = MakeProcInstance ( (FARPROC)WdeIDToStr, WdeApplicationInstance );

    return( TRUE );
}

void WdeCustomFini( void )
{
    WdeFreeClassList();
    WdeFreeDialogBoxControl( &WdeDefaultCustom );
    FreeProcInstance( WdeCustomDispatch );
    FreeProcInstance( WdeStr2ID );
    FreeProcInstance( WdeID2Str );
}

BOOL WdeCustomDestroy( WdeCustomObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( !Forward( obj->control, DESTROY, flag, NULL ) ) {
        WdeWriteTrail( "WdeCustomDestroy: Control DESTROY failed" );
        return( FALSE );
    }

    if( obj->win_class != NULL ) {
        WdeMemFree( obj->win_class );
    }

    WdeMemFree( obj );

    return( TRUE );
}

BOOL WdeCustomValidateAction( WdeCustomObject *obj, ACTION *act, void *p2 )
{
    int     i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeCustomActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->control, *act, p2 ) );
}

BOOL WdeCustomCopyObject( WdeCustomObject *obj, WdeCustomObject **new,
                          WdeCustomObject *handle )
{
    if( new == NULL ) {
        WdeWriteTrail( "WdeCustomCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeCustomObject *)WdeMemAlloc( sizeof( WdeCustomObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeCustomCopyObject: Object malloc failed" );
        return( FALSE );
    }

    (*new)->dispatcher = obj->dispatcher;
    (*new)->win_proc = obj->win_proc;
    (*new)->win_extra = obj->win_extra;
    (*new)->object_id = obj->object_id;
    (*new)->cust_info = obj->cust_info;
    (*new)->cust_type = obj->cust_type;

    (*new)->win_class = WdeStrDup( obj->win_class );
    if( (*new)->win_class == NULL ) {
        WdeWriteTrail( "WdeCustomCopyObject: Class alloc failed!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    if( handle == NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->control, &(*new)->control, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeCustomCopyObject: Control not created!" );
        WdeMemFree( *new );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeCustomIdentify( WdeCustomObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeCustomGetWndProc( WdeCustomObject *obj, WNDPROC *proc, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *proc = WdeCustomSuperClassProc;

    return( TRUE );
}

BOOL WdeCustomGetWindowClass( WdeCustomObject *obj, char **class, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    *class = obj->win_class;

    return( TRUE );
}

BOOL WdeCustomDefine( WdeCustomObject *obj, POINT *pnt, void *p2 )
{
    BOOL                        redraw;
    HWND                        dialog_owner;
    HGLOBAL                     ctl_style;
    uint_32                     ctl_size;
    WDECTLSTYLE                 *ctl_data;
    WdeCustStyleProc            proc;
    WdeDialogBoxControl         *info;
    char                        *text;
    int                         tlen;
    WdeDefineObjectInfo         o_info;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    if( !Forward( (OBJPTR)obj, GET_WINDOW_HANDLE, &o_info.win, NULL ) ) {
        WdeWriteTrail( "WdeControlDefine: GET_WINDOW_HANDLE failed!" );
        return( FALSE );
    }

    if( !Forward( obj->object_handle, GET_OBJECT_INFO,
                  &o_info.info.c.info, &o_info.symbol ) ) {
        WdeWriteTrail( "WdeCustomDefine: GET_OBJECT_INFO failed!" );
        return( FALSE );
    }

    if( !WdeGetOption( WdeOptUseDefDlg ) ) {
        o_info.obj = obj->object_handle;
        o_info.obj_id = obj->object_id;
        o_info.mask = 0xffff;
        o_info.hook_func = WdeWinStylesHook;
        o_info.set_func = NULL;
        o_info.get_func = NULL;
        o_info.res_info = WdeGetCurrentRes();
        return( WdeGenericDefine( &o_info ) );
    }

    info = o_info.info.c.info;

    dialog_owner = WdeGetMainWindowHandle();

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_DEFININGCUSTOM, -1 );

    if( obj->cust_info->ms_lib ) {
        ctl_size = sizeof( CTLSTYLE );
    } else {
        ctl_size = sizeof( WDECTLSTYLE );
    }

    ctl_style = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, ctl_size );
    if( ctl_style == NULL ) {
        WdeWriteTrail( "WdeCustomDefine: Could not GlobalAlloc ctl_style!" );
        return( FALSE );
    }

    ctl_data = (WDECTLSTYLE *)GlobalLock( ctl_style );
    if( ctl_data == NULL ) {
        WdeWriteTrail( "WdeCustomDefine: Could not GlobalLock ctl_data!" );
        GlobalFree( ctl_style );
        return( FALSE );
    }

    ctl_data->wX = GETCTL_SIZEX( info );
    ctl_data->wY = GETCTL_SIZEY( info );
    ctl_data->wCx = GETCTL_SIZEW( info );
    ctl_data->wCy = GETCTL_SIZEH( info );
    ctl_data->wId = GETCTL_ID( info );
    ctl_data->dwStyle = GETCTL_STYLE( info );

    if( (text = WdeResNameOrOrdinalToStr( GETCTL_TEXT( info ), 10 )) != NULL ) {
        tlen = strlen( text );
        if( tlen < CTLCLASS ) {
            strcpy( ctl_data->szTitle, text );
        } else {
            memcpy( ctl_data->szTitle, text, CTLCLASS );
            ctl_data->szTitle[CTLCLASS - 1] = '\0';
        }
        WdeMemFree( text );
    } else {
         ctl_data->szTitle[0] = '\0';
    }

    strcpy( ctl_data->szClass, obj->cust_info->control_info.ms.szClass );

    if( !obj->cust_info->ms_lib ) {
        ctl_data->CtlDataSize = 0;
        memset( ctl_data->CtlData, 0, CTLDATALENGTH );
    }

    GlobalUnlock( ctl_style );

    proc = obj->cust_info->style_proc;

    redraw = (BOOL)(*proc)( dialog_owner, ctl_style, (LPFNSTRTOID)WdeStr2ID,
                            (LPFNIDTOSTR)WdeID2Str );

    if( redraw ) {

        ctl_data = (WDECTLSTYLE *)GlobalLock( ctl_style );
        if( ctl_data == NULL ) {
            WdeWriteTrail( "WdeCustomDefine: Could not GlobalLock ctl_data!" );
            GlobalFree( ctl_style );
            return( FALSE );
        }

        SETCTL_SIZEX( info, ctl_data->wX );
        SETCTL_SIZEY( info, ctl_data->wY );
        SETCTL_SIZEW( info, ctl_data->wCx );
        SETCTL_SIZEH( info, ctl_data->wCy );
        SETCTL_ID( info, ctl_data->wId );
        SETCTL_STYLE( info, ctl_data->dwStyle );

        WdeMemFree( GETCTL_TEXT( info ) );
        WdeMemFree( GETCTL_CLASSID( info ) );
        SETCTL_TEXT( info, ResStrToNameOrOrd( ctl_data->szTitle ) );
        SETCTL_CLASSID( info, WdeStrToControlClass( ctl_data->szClass ) );

        GlobalUnlock( ctl_style );

        if( !Forward( obj->object_handle, DESTROY_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeCustomDefine: DESTROY_WINDOW failed!" );
            return( FALSE );
        }

        if( !Forward( obj->object_handle, CREATE_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeCustomDefine: CREATE_WINDOW failed!" );
            return( FALSE );
        }

        Notify( obj->object_handle, CURRENT_OBJECT, NULL );
    }

    GlobalFree( ctl_style );

    WdeSetStatusReadyText();

    return( TRUE );
}

WORD WINEXPORT WdeIDToStr( WORD id, LPSTR str, WORD len )
{
    char s[11];
    WORD slen;

    utoa( id, s, 10 );
    slen = strlen( s );
    if( slen > len - 1 ) {
        return( 0 );
    }
    strcpy( str, s );

    return( slen );
}

DWORD WINEXPORT WdeStrToID( LPSTR str )
{
    uint_32 num;
    DWORD   ret;
    char    *ep;

    num = (uint_32)strtoul( str, &ep, 0 );
    if( *ep != '\0' ) {
        ret = MAKELONG( (UINT)0, (UINT)0 );
    } else {
        ret = MAKELONG( (UINT)1, (UINT)num );
    }

    return( ret );
}

LRESULT WINEXPORT WdeCustomSuperClassProc( HWND hWnd, UINT message, WPARAM wParam,
                                           volatile LPARAM lParam )
{
    WNDPROC             wnd_proc;
    uint_16             extra;
    LIST                *clist;
    WdeCustClassNode    *node;

    if( WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( FALSE );
    }

    extra = (uint_16)GetClassWord( hWnd, GCW_CBWNDEXTRA );
    extra -= sizeof( WNDPROC );
    wnd_proc = (WNDPROC)GetWindowLong( hWnd, extra );
    if( wnd_proc == NULL ) {
        if( !GetClassName( hWnd, WdeClassName, MAX_NAME ) ) {
            WdeWriteTrail( "WdeCustomSuperClassProc: GetClassName failed!" );
            return( FALSE );
        }
        clist = WdeFindClassInList( &WdeClassName[4] );
        if( clist == NULL ) {
            WdeWriteTrail( "WdeCustomSuperClassProc: FindClassInList failed!" );
            return( FALSE );
        }
        node = (WdeCustClassNode *)ListElement( clist );
        wnd_proc = node->win_proc;
        if( wnd_proc == NULL ) {
            WdeWriteTrail( "WdeCustomSuperClassProc: NULL wnd_proc!" );
            return( FALSE );
        }
        SetWindowLong( hWnd, extra, (LONG)wnd_proc );
    }

    return( CallWindowProc( wnd_proc, hWnd, message, wParam, lParam ) );
}
