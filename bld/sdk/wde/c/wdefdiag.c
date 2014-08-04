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
#include <mbstring.h>
#include "wderes.h"
#include "wdeopts.h"
#include "wdeactn.h"
#include "wde_wres.h"
#include "wdemain.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdesdlg.h"
#include "wdefdlg.h"
#include "wdetest.h"
#include "wdeobjid.h"
#include "wdeinfo.h"
#include "wdefont.h"
#include "wdestyle.h"
#include "wderesiz.h"
#include "wdefmenu.h"
#include "wdegeted.h"
#include "wdefutil.h"
#include "wdectool.h"
#include "wdesdup.h"
#include "wdelist.h"
#include "wdeoinfo.h"
#include "wdefordr.h"
#include "wdesym.h"
#include "wdestat.h"
#include "wdedebug.h"
#include "wdedefin.h"
#include "wresall.h"
#include "wde_rc.h"
#include "wderesin.h"
#include "wdesvobj.h"
#include "wdectl3d.h"
#include "wdecctl.h"
#include "wdefbase.h"
#include "wdefsbar.h"
#include "wdefdiag.h"
#include "windlg.h"
#include "windlg32.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#ifdef __NT__
    #define WDEDLGTEMPLATE (LPCDLGTEMPLATE)
#else
    #define WDEDLGTEMPLATE
#endif

#define WDE_NEW_DIALOG_X        20
#define WDE_NEW_DIALOG_Y        (WDE_NEW_DIALOG_X + 20)
#define WDE_NEW_DIALOG_HEIGHT   125
#define WDE_NEW_DIALOG_WIDTH    (WDE_NEW_DIALOG_HEIGHT + 20)

#define WDE_TT_FONT_MIN         8
#define WDE_TT_FONT_MAX         36

#define DIALOG_MIN_SIZE_BORDER  WDE_BORDER_SIZE

#define DIALOG_MIN_WIDTH        75
#define DIALOG_MIN_HEIGHT       DIALOG_MIN_WIDTH

#define DEFAULT_FONT            "Helv"
#define DEFAULT_POINT_SIZE      8
#define DEFAULT_JFONT           "‚l‚r –¾’©"
#define DEFAULT_JPOINT_SIZE     10
#define DEFAULT_MEMFLAGS        (MEMFLAG_DISCARDABLE | MEMFLAG_PURE | MEMFLAG_MOVEABLE)

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    FARPROC             dispatcher;
    HWND                window_handle;
    HWND                parent_handle;
    OBJ_ID              object_id;
    OBJPTR              object_handle;
    OBJPTR              parent;
    OBJPTR              o_item;
    uint_16             num_children; // was uint_8 ==> not big enough for NT
    LIST                *children;
    LIST                *ochildren;
    HFONT               font;
    RECT                nc_size;
    uint_16             mem_flags;
    WdeDialogBoxHeader  *dialog_info;
    WdeResInfo          *res_info;
    WdeResDlgItem       *dlg_item;
    WdeResizeRatio      resizer;
    WResID              *name;
//  WResHelpID          *helpname;
    char                *file_name;
    char                *symbol;
    char                *helpsymbol;
    WdeOrderMode        mode;
} WdeDialogObject;

typedef struct {
   uint_32      style;
   uint_8       items;
   uint_16      x;
   uint_16      y;
   uint_16      cx;
   uint_16      cy;
/* char         dtMenuName[];       Variable-length string */
/* char         dtClassName[];      Variable-length string */
/* char         dtCaptionText[];    Variable-length string */
} DialogTemplateFixed;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL       CALLBACK WdeDialogDispatcher( ACTION, WdeDialogObject *, void *, void * );
WINEXPORT BOOL       CALLBACK WdeDialogProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT BOOL       CALLBACK WdeDialogDefineProc( HWND, WORD, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool     WdeRemoveObject( WdeResInfo *, OBJPTR );
static bool     WdeCalcDialogNCSize( WdeDialogObject *, RECT * );
static bool     WdeKludgeDialogSize( WdeDialogObject *, bool, bool );
static OBJPTR   WdeDialogCreater( OBJPTR, RECT *, OBJPTR );
static WResID   *WdeCreateDialogTitle( void );
static BOOL     WdeDialogTest( WdeDialogObject *, void *, void * );
static BOOL     WdeDialogSaveObject( WdeDialogObject *, WORD *, void * );
static BOOL     WdeDialogMove( WdeDialogObject *, POINT *, bool * );
static bool     WdeOffsetDialogUnits( WdeDialogObject *, RECT *, RECT * );
static bool     WdeUpdateDialogUnits( WdeDialogObject *, RECT *, RECT * );
static BOOL     WdeDialogGetFont( WdeDialogObject *, HFONT *, void * );
static BOOL     WdeDialogResolveSymbol( WdeDialogObject *, bool *, bool * );
static BOOL     WdeDialogResolveHelpSymbol( WdeDialogObject *, bool *, bool * );
static BOOL     WdeDialogGetResizer( WdeDialogObject *, WdeResizeRatio *, OBJPTR * );
static BOOL     WdeDialogGetNCSize( WdeDialogObject *, RECT *, void * );
static BOOL     WdeDialogIdentify( WdeDialogObject *, OBJ_ID *, void * );
static BOOL     WdeDialogDefine( WdeDialogObject *, POINT *, void * );
static BOOL     WdeDialogResize( WdeDialogObject *, RECT *, bool * );
static BOOL     WdeDialogIsMarkValid( WdeDialogObject *, BOOL *, void * );
static BOOL     WdeDialogDestroy( WdeDialogObject *, bool *, bool * );
static BOOL     WdeDialogRestore( WdeDialogObject *, void *, void * );
static BOOL     WdeDialogCreateWindow( WdeDialogObject *, bool *, void * );
static BOOL     WdeDialogDestroyWindow( WdeDialogObject *, bool *, bool * );
static BOOL     WdeDialogShowWindow( WdeDialogObject *, bool *, void * );
static BOOL     WdeDialogGetResizeInfo( WdeDialogObject *, RESIZE_ID *, void * );
static BOOL     WdeDialogValidateAction( WdeDialogObject *, ACTION *, void * );
static BOOL     WdeDialogSetFont( WdeDialogObject *, HFONT *, WdeResizeRatio * );
static BOOL     WdeDialogDraw( WdeDialogObject *, RECT *, HDC * );
static BOOL     WdeDialogAddSubObject( WdeDialogObject *, OBJPTR, void * );
static BOOL     WdeDialogGetSubObjectList( WdeDialogObject *, LIST **, void * );
static BOOL     WdeDialogFindSubObjects( WdeDialogObject *, SUBOBJ_REQUEST *, LIST ** );
static BOOL     WdeDialogFindObjectsPt( WdeDialogObject *, POINT *, LIST ** );
static BOOL     WdeDialogGetNextChild( WdeDialogObject *, OBJPTR *, bool * );
static BOOL     WdeDialogRemoveSubObject( WdeDialogObject *, OBJPTR, void * );
static BOOL     WdeDialogGetWindowHandle( WdeDialogObject *, HWND *, void * );
static BOOL     WdeDialogNotify( WdeDialogObject *, NOTE_ID *, void * );
static BOOL     WdeDialogFirstChild( WdeDialogObject *, void *, void *  );
static BOOL     WdeDialogPutChildFirst( WdeDialogObject *, OBJPTR, void * );
static BOOL     WdeDialogOnTop( WdeDialogObject *, void *, void * );
static BOOL     WdeDialogPasteObject( WdeDialogObject *, OBJPTR, POINT * );
static BOOL     WdeDialogCutObject( WdeDialogObject *, WdeDialogObject **, void * );
static BOOL     WdeDialogCopyObject( WdeDialogObject *, WdeDialogObject **, WdeDialogObject * );
static BOOL     WdeDialogSetOrderMode( WdeDialogObject *, WdeOrderMode *, void * );
static BOOL     WdeDialogGetOrderMode( WdeDialogObject *, WdeOrderMode *, void * );
static BOOL     WdeDialogSetObjectInfo( WdeDialogObject *, WdeDialogBoxHeader *, void * );
static BOOL     WdeDialogGetObjectInfo( WdeDialogObject *obj, WdeDialogBoxHeader **, WResID ** );
static BOOL     WdeDialogGetObjectHelpInfo( WdeDialogObject *obj, WdeDialogBoxHeader **, char ** );
static BOOL     WdeDialogGetScrollRect( WdeDialogObject *, RECT *, void * );
static BOOL     WdeDialogGetResizeInc( WdeDialogObject *, POINT *, void * );

static bool WdeBuildDialogTemplate( WdeDialogBoxHeader *, HGLOBAL * );
static void WdeDialogSetDefineDialogInfo( WdeDefineObjectInfo *, HWND );
static void WdeDialogGetDefineDialogInfo( WdeDefineObjectInfo *, HWND );
static void WdeDialogSetDialogStyles( HWND, DialogStyle );
static void WdeDialogGetDialogStyles( HWND, DialogStyle * );
static void WdeDialogSetDialogEXStyles( HWND, uint_32 );
static void WdeDialogGetDialogEXStyles( HWND, uint_32 * );
static void WdeDialogSetDialogMemFlags( HWND, uint_16 );
static void WdeDialogGetDialogMemFlags( HWND, uint_16 * );
static void WdeDialogSetDialogFontInfo( HWND, WdeDialogObject * );
static void WdeDialogSetDialogPntInfo( HWND, int );
static void WdeFreeDialogObject( WdeDialogObject * );
static void WdeWriteDialogToInfo( WdeDialogObject * );
static BOOL WdeDialogModifyInfo( WdeDialogObject *, WdeInfoStruct *, void * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static FARPROC                  WdeDialogDispatch;
static FARPROC                  WdeDialogDefineProcInst;
static DLGPROC                  WdeDialogProcInst;
static DLGPROC                  WdeTestProcInst;
static HINSTANCE                WdeAppInst;
static WdeDialogBoxHeader       *WdeDefaultDialog;

static HFONT             WdeLastFont;

static DISPATCH_ITEM WdeDialogActions[] = {
    { MOVE,                 (DISPATCH_RTN *)WdeDialogMove               },
    { DESTROY,              (DISPATCH_RTN *)WdeDialogDestroy            },
    { RESIZE,               (DISPATCH_RTN *)WdeDialogResize             },
    { DRAW,                 (DISPATCH_RTN *)WdeDialogDraw               },
    { VALIDATE_ACTION,      (DISPATCH_RTN *)WdeDialogValidateAction     },
    { GET_WINDOW_HANDLE,    (DISPATCH_RTN *)WdeDialogGetWindowHandle    },
    { CREATE_WINDOW,        (DISPATCH_RTN *)WdeDialogCreateWindow       },
    { DESTROY_WINDOW,       (DISPATCH_RTN *)WdeDialogDestroyWindow      },
    { SHOW_WIN,             (DISPATCH_RTN *)WdeDialogShowWindow         },
    { RESIZE_INFO,          (DISPATCH_RTN *)WdeDialogGetResizeInfo      },
    { NOTIFY,               (DISPATCH_RTN *)WdeDialogNotify             },
    { PASTE,                (DISPATCH_RTN *)WdeDialogPasteObject        },
    { COPY,                 (DISPATCH_RTN *)WdeDialogCopyObject         },
    { CUT,                  (DISPATCH_RTN *)WdeDialogCutObject          },
    { GET_OBJECT_INFO,      (DISPATCH_RTN *)WdeDialogGetObjectInfo      },
    { SET_OBJECT_INFO,      (DISPATCH_RTN *)WdeDialogSetObjectInfo      },
    { GET_OBJECT_HELPINFO,  (DISPATCH_RTN *)WdeDialogGetObjectHelpInfo  },
    { ADD_SUBOBJECT,        (DISPATCH_RTN *)WdeDialogAddSubObject       },
    { FIND_SUBOBJECTS,      (DISPATCH_RTN *)WdeDialogFindSubObjects     },
    { FIND_OBJECTS_PT,      (DISPATCH_RTN *)WdeDialogFindObjectsPt      },
    { REMOVE_SUBOBJECT,     (DISPATCH_RTN *)WdeDialogRemoveSubObject    },
    { GET_SUBOBJ_LIST,      (DISPATCH_RTN *)WdeDialogGetSubObjectList   },
    { IDENTIFY,             (DISPATCH_RTN *)WdeDialogIdentify           },
    { DEFINE,               (DISPATCH_RTN *)WdeDialogDefine             },
    { SET_FONT,             (DISPATCH_RTN *)WdeDialogSetFont            },
    { GET_FONT,             (DISPATCH_RTN *)WdeDialogGetFont            },
    { GET_RESIZER,          (DISPATCH_RTN *)WdeDialogGetResizer         },
    { GET_NC_SIZE,          (DISPATCH_RTN *)WdeDialogGetNCSize          },
    { SAVE_OBJECT,          (DISPATCH_RTN *)WdeDialogSaveObject         },
    { ON_TOP,               (DISPATCH_RTN *)WdeDialogOnTop              },
    { TEST,                 (DISPATCH_RTN *)WdeDialogTest               },
    { BECOME_FIRST_CHILD,   (DISPATCH_RTN *)WdeDialogFirstChild         },
    { PUT_ME_FIRST,         (DISPATCH_RTN *)WdeDialogPutChildFirst      },
    { GET_RESIZE_INC,       (DISPATCH_RTN *)WdeDialogGetResizeInc       },
    { GET_SCROLL_RECT,      (DISPATCH_RTN *)WdeDialogGetScrollRect      },
    { IS_MARK_VALID,        (DISPATCH_RTN *)WdeDialogIsMarkValid        },
    { RESTORE_OBJECT,       (DISPATCH_RTN *)WdeDialogRestore            },
    { RESOLVE_SYMBOL,       (DISPATCH_RTN *)WdeDialogResolveSymbol      },
    { RESOLVE_HELPSYMBOL,   (DISPATCH_RTN *)WdeDialogResolveHelpSymbol  },
    { MODIFY_INFO,          (DISPATCH_RTN *)WdeDialogModifyInfo         },
    { GET_NEXT_CHILD,       (DISPATCH_RTN *)WdeDialogGetNextChild       },
    { SET_ORDER_MODE,       (DISPATCH_RTN *)WdeDialogSetOrderMode       },
    { GET_ORDER_MODE,       (DISPATCH_RTN *)WdeDialogGetOrderMode       }
};

#define MAX_ACTIONS      (sizeof( WdeDialogActions ) / sizeof( DISPATCH_ITEM ))

bool WdeRemoveObject( WdeResInfo *res_info, OBJPTR object )
{
    WdeResDlgItem   *ditem;
    signed int      pos;

    if( res_info == NULL ) {
        return( FALSE );
    }

    pos = WdeFindObjectPosInDialogList( res_info, object );
    if( pos == -1 ) {
        return( FALSE );
    }

    ditem = WdeFindDialogInResInfo( res_info, pos );
    if( ditem == NULL ) {
        WdeWriteTrail( "WdeRemoveObject: item not found!" );
        return( FALSE );
    }

    if( !WdeRemoveDialogFromResInfo( res_info, ditem, FALSE ) ) {
        WdeWriteTrail( "WdeRemoveObject: remove failed!" );
        return( FALSE );
    }

    return( TRUE );
}

void WdeDialogModified( void *_obj )
{
    WdeDialogObject *obj = _obj;

    if( obj != NULL && obj->dlg_item != NULL ) {
        obj->dlg_item->modified = TRUE;
    }
}

bool WdePreserveDialogWithDBI( void *_obj )
{
    WdeDialogObject *obj = _obj;
    void            *vp;

    vp = WdeDBIFromObject( obj );

    if( vp == NULL ) {
        return( FALSE );
    }

    if( obj->dlg_item->dialog_info != NULL ) {
        WdeFreeDialogBoxInfo( obj->dlg_item->dialog_info );
    }

    obj->dlg_item->dialog_info = vp;
    obj->dlg_item->object = NULL;

    return( TRUE );
}

WdeDialogBoxInfo *WdeDBIFromObject( void *_obj )
{
    WdeDialogObject     *obj = _obj;
    WdeDialogBoxInfo    *info;
    LIST                *clist;
    LIST                *end;
    WdeDialogBoxControl *control;
    WdeOrderedEntry     *oentry;
    char                *symbol;
    char                *helpsymbol;

    bool                is32bitEx;
    WdeDialogBoxHeader  *dh;

    info = (WdeDialogBoxInfo *)WRMemAlloc( sizeof( WdeDialogBoxInfo ) );

    if( info == NULL ) {
        WdeWriteTrail( "WdeDBIFromObject: WdeResInfo alloc failed!" );
        return( NULL );
    }

    SETHDR_NUMITEMS( obj->dialog_info, obj->num_children );

    info->dialog_header = WdeCopyDialogBoxHeader( obj->dialog_info );

    if( info->dialog_header == NULL ) {
        WdeWriteTrail( "WdeDBIFromObject: CopyDBH failed!" );
        WRMemFree( info );
        return( NULL );
    }

    is32bitEx = FALSE;

    info->MemoryFlags = obj->mem_flags;
    info->dialog_header->symbol = WdeStrDup( obj->symbol );
    info->dialog_header->helpsymbol = WdeStrDup( obj->helpsymbol );
    info->control_list = NULL;

    end = NULL;

    WdeCleanOrderedList( &obj->ochildren );

    for( clist = obj->ochildren; clist; clist = ListNext( clist ) ) {
        oentry = (WdeOrderedEntry  *)ListElement( clist );
        if( !Forward( oentry->obj, GET_OBJECT_INFO, &control, &symbol ) ) {
            WdeWriteTrail( "WdeDBIFromObject: GET_OBJECT_INFO failed!" );
            WdeFreeDialogBoxInfo( info );
            return( NULL );
        }
        /* JPK - do it again for the help symbol */
        if( !Forward( oentry->obj, GET_OBJECT_HELPINFO, &control, &helpsymbol ) ) {
            WdeWriteTrail( "WdeDBIFromObject: GET_OBJECT_HELPINFO failed!" );
            WdeFreeDialogBoxInfo( info );
            return( NULL );
        }

        control = WdeCopyDialogBoxControl( control );

        control->symbol = WdeStrDup( symbol );
        control->helpsymbol = WdeStrDup( helpsymbol );

        if( obj->dialog_info->is32bit ) {
            if( control->HelpId != 0 || control->ExtendedStyle != 0 ) {
                is32bitEx = TRUE;
            }
        }

        if( end != NULL ) {
            ListInsertElt( end, control );
            end = ListNext( end );
        } else {
            ListAddElt( &info->control_list, control );
            end = info->control_list;
        }
    }

    dh = info->dialog_header;
    if( obj->dialog_info->is32bit ) {
        if( dh->HelpId != 0 || dh->ExtendedStyle != 0 ||
            dh->FontWeightDefined || dh->FontItalicDefined ) {
            is32bitEx = TRUE;
        }
    } else {
        dh->is32bit = FALSE;
    }

    dh->is32bitEx = is32bitEx;

    return( info );
}

static uint_32 NumDialogTitles = 0;

WResID *WdeCreateDialogTitle( void )
{
    char        *text;
    char        *title;
    WResID      *name;

    NumDialogTitles++;

    name = NULL;
    text = WdeAllocRCString( WDE_DEFDIALOGNAME );
    if( text != NULL ) {
        title = (char *)WRMemAlloc( strlen( text ) + 10 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, NumDialogTitles );
            name = WResIDFromStr( title );
            WRMemFree( title );
        }
        WdeFreeRCString( text );
    }

    return( name );
}

RECT *WdeGetDefaultDialogNCSize( void )
{
    static RECT nc_size;
    static bool nc_size_set = FALSE;

    if ( !nc_size_set ) {
        nc_size.left = GetSystemMetrics( SM_CXDLGFRAME ) +
                       GetSystemMetrics( SM_CXBORDER );
        nc_size.right = nc_size.left;

        nc_size.bottom = GetSystemMetrics( SM_CYDLGFRAME );
        nc_size.top = nc_size.bottom;
        nc_size.bottom += GetSystemMetrics( SM_CYBORDER );
        nc_size.top += GetSystemMetrics( SM_CYCAPTION );

        nc_size_set = TRUE;
    }

    return( &nc_size );
}

bool WdeKludgeDialogSize( WdeDialogObject *obj, bool adjust_for_nc,
                          bool snap_to_grid )
{
    RECT        old_rect;
    RECT        new_rect;
    bool        user_action;
    POINT       pt;

    Location( (OBJPTR)obj, &old_rect );

    if( snap_to_grid ) {
        pt.x = GETHDR_SIZEX( obj->dialog_info );
        pt.y = GETHDR_SIZEY( obj->dialog_info );
        WdeSnapPointToGrid( &pt );
        SETHDR_SIZEX( obj->dialog_info, pt.x );
        SETHDR_SIZEY( obj->dialog_info, pt.y );
    }

    if( !WdeDialogToScreen( obj, &obj->resizer,
                            GETHDR_PSIZE( obj->dialog_info ), &new_rect ) ) {
        WdeWriteTrail( "WdeKludgeDialogSize: WdeDialogToScreen failed!" );
        return( FALSE );
    }

    if( adjust_for_nc ) {
        /* adjust for the size of the non-client area */
        new_rect.left -= obj->nc_size.left;
        new_rect.top -= obj->nc_size.top;
        new_rect.right += obj->nc_size.right;
        new_rect.bottom += obj->nc_size.bottom;
    }

    user_action = FALSE;

    /* YIKES!!! what a kludge */
    if( !EqualRect( &new_rect, &old_rect ) ) {
        if( !WdeDialogResize( obj, &new_rect, &user_action ) ) {
            WdeWriteTrail( "WdeKludgeDialogSize: Resize failed!" );
            return( FALSE );
        }
    }

    return( TRUE );
}

OBJPTR WdeCreateNewDialog( WResID *name, bool is32bit )
{
    OBJPTR          base_obj;
    WdeDialogObject *new;
    RECT            new_rect;
    bool            ok;


    ok = ((base_obj = GetMainObject()) != NULL);

    if( ok ) {
        new_rect.left = WDE_NEW_DIALOG_X;
        new_rect.top = WDE_NEW_DIALOG_Y;
        new_rect.right = WDE_NEW_DIALOG_WIDTH;
        new_rect.bottom = WDE_NEW_DIALOG_HEIGHT;
        ok = ((new = WdeDialogCreater( base_obj, &new_rect, NULL )) != NULL);
        if( !ok ) {
            WdeWriteTrail( "WdeCreateNewDialog: Create failed!" );
        }
    }

    if( ok ) {
        if( name != NULL ) {
            new->name = WdeCopyWResID( name );
        } else {
            new->name = WdeCreateDialogTitle();
        }
//      new->helpname = NULL;
        new->helpsymbol = NULL;
        ok = (new->name != NULL);
        if( !ok ) {
            WdeWriteTrail( "WdeDialogCreate: could not create dialog name!" );
        }
    }

    if( ok ) {
        ok = WdeSetObjectInfo( new, &new->res_info, &new->dlg_item, new->name );
    }

    if( ok ) {
        new->dlg_item->is32bit = is32bit;
        SETHDR_SIZEX( WdeDefaultDialog, WDE_NEW_DIALOG_X );
        SETHDR_SIZEY( WdeDefaultDialog, WDE_NEW_DIALOG_Y );
        SETHDR_SIZEW( WdeDefaultDialog, WDE_NEW_DIALOG_WIDTH );
        SETHDR_SIZEH( WdeDefaultDialog, WDE_NEW_DIALOG_HEIGHT );

        new->dialog_info = WdeCopyDialogBoxHeader( WdeDefaultDialog );
        ok = (new->dialog_info != NULL);
        if( !ok ) {
            WdeWriteTrail( "WdeDialogCreate: CopyDBH failed!" );
        }
    }

    if( ok ) {
        ok = WdeDialogCreateWindow( new, NULL, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeDialogCreate: CREATE_WINDOW failed!" );
        }
    }

    if( ok ) {
        ok = Register( (OBJPTR)new );
        if( !ok ) {
            WdeWriteTrail( "WdeCreateNewDialog: Register failed!" );
        }
    }

    if( ok ) {
        MakeObjectCurrent( new );
    } else {
        if( new ) {
            Destroy( (OBJPTR)new, FALSE );
            new = NULL;
        }
    }

    return( new );
}

OBJPTR WdeCreateDialogFromRes( WdeResInfo *res_info, WdeResDlgItem *ditem )
{
    OBJPTR          base_obj;
    WdeDialogObject *new;
    RECT            new_rect;
    bool            ok;
    bool            show;

    ok = (res_info != NULL && ditem != NULL);

    if( ok ) {
        ok = ((base_obj = GetMainObject()) != NULL);
    }

    if( ok ) {
        new_rect.left = WDE_NEW_DIALOG_X;
        new_rect.top = WDE_NEW_DIALOG_Y;
        new_rect.right = WDE_NEW_DIALOG_WIDTH;
        new_rect.bottom = WDE_NEW_DIALOG_HEIGHT;
        ok = ((new = WdeDialogCreater( base_obj, &new_rect, NULL )) != NULL);
    }

    if( ok ) {
        new->name = WdeCopyWResID( ditem->dialog_name );
//      new->helpname = WdeCopyWResHelpID( ditem->helpname );
        ok = (new->name != NULL);
    }

    if( ok ) {
        new->res_info = res_info;
        new->dlg_item = ditem;
        new->mem_flags = ditem->dialog_info->MemoryFlags;
        new->dialog_info = WdeCopyDialogBoxHeader( ditem->dialog_info->dialog_header );
        ok = (new->dialog_info != NULL);
    }

    if( ok ) {
        show = FALSE;
        ok = WdeDialogCreateWindow( new, &show, NULL );
    }

    if( ok ) {
        ok = Register( (OBJPTR)new );
    }

    if( !ok ) {
        Destroy( (OBJPTR)new, FALSE );
        new = NULL;
    }

    return( new );
}

WINEXPORT OBJPTR CALLBACK WdeDialogCreate( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    WdeDialogObject *new;
    RECT            *def_nc_size;

    def_nc_size = WdeGetDefaultDialogNCSize();

    obj_rect->left += def_nc_size->left;
    obj_rect->top += def_nc_size->top;
    obj_rect->right -= def_nc_size->right;
    obj_rect->bottom -= def_nc_size->bottom;

    if( obj_rect->right < obj_rect->left ) {
        obj_rect->right = obj_rect->left;
    }

    if( obj_rect->bottom < obj_rect->top ) {
        obj_rect->bottom = obj_rect->top;
    }

    new = WdeDialogCreater( parent, obj_rect, handle );

    if( new == NULL ) {
        return( NULL );
    }

    new->name = WdeCreateDialogTitle();
    if( new->name == NULL ) {
        WdeWriteTrail( "WdeDialogCreate: WdeCreateDialogTitle failed!" );
        Destroy( (OBJPTR)new, FALSE );
        return( NULL );
    }

    if( !WdeSetObjectInfo( new, &new->res_info, &new->dlg_item, new->name ) ) {
        WdeWriteTrail( "WdeDialogCreate: WdeSetObjectInfo failed!" );
        Destroy( (OBJPTR)new, FALSE );
        return( NULL );
    }

    WdeScreenToDialog( new->parent, NULL, obj_rect, GETHDR_PSIZE( WdeDefaultDialog ) );

    new->dialog_info = WdeCopyDialogBoxHeader( WdeDefaultDialog );
    if( new->dialog_info == NULL ) {
        WdeWriteTrail( "WdeDialogCreate: CopyDBH failed!" );
        Destroy( (OBJPTR)new, FALSE );
        return( NULL );
    }

    if( !WdeDialogCreateWindow( new, NULL, NULL ) ) {
        WdeWriteTrail( "WdeDialogCreate: CREATE_WINDOW failed!" );
        Destroy( (OBJPTR)new, FALSE );
        return( NULL );
    }

    return( new );
}

OBJPTR WdeDialogCreater( OBJPTR parent, RECT *obj_rect, OBJPTR handle )
{
    WdeDialogObject *new;
    OBJPTR          ancestor;
    OBJ_ID          ancestor_id;
    RECT            parent_rect;
    bool            resize_dialog_height;
    bool            resize_dialog_width;

    WdeDebugCreate( "Dialog", parent, obj_rect, handle );

    if( parent == NULL ) {
        WdeWriteTrail( "WdeDialogCreater: Dialog has no parent!" );
        return( NULL );
    }

    new = (WdeDialogObject *)WRMemAlloc( sizeof( WdeDialogObject ) );
    if( new == NULL ) {
        WdeWriteTrail( "WdeDialogCreater: Dialog object malloc failed" );
        return( new );
    }
    memset( new, 0, sizeof( WdeDialogObject ) );

    /* make sure we are setting the correct parent */
    ancestor = parent;
    Forward( (OBJPTR)ancestor, IDENTIFY, &ancestor_id, NULL );
    while( ancestor_id != BASE_OBJ ) {
        GetObjectParent( ancestor, &ancestor );
        Forward( (OBJPTR)ancestor, IDENTIFY, &ancestor_id, NULL );
    }

    new->parent = ancestor;
    new->object_id  = DIALOG_OBJ;
    new->mode = WdeSelect;
    new->dispatcher = WdeDialogDispatch;
    new->mem_flags = DEFAULT_MEMFLAGS;

    resize_dialog_height = FALSE;
    resize_dialog_width = FALSE;

    if( obj_rect->right - obj_rect->left < DIALOG_MIN_WIDTH  ) {
        resize_dialog_width = TRUE;
    }

    if( obj_rect->bottom - obj_rect->top < DIALOG_MIN_HEIGHT ) {
        resize_dialog_height = TRUE;
    }

    if( resize_dialog_width || resize_dialog_height ) {
        Location( new->parent, &parent_rect );
    }

    if( resize_dialog_width ) {
        if( obj_rect->left + DIALOG_MIN_WIDTH >= parent_rect.right ) {
            obj_rect->left = 0;
            if( obj_rect->right > DIALOG_MIN_WIDTH ) {
                obj_rect->left = obj_rect->right - DIALOG_MIN_WIDTH;
            }
        } else {
            obj_rect->right = obj_rect->left + DIALOG_MIN_WIDTH;
        }
    }

    if( resize_dialog_height ) {
        if( obj_rect->top + DIALOG_MIN_HEIGHT >= parent_rect.bottom ) {
            obj_rect->top = 0;
            if( obj_rect->bottom > DIALOG_MIN_HEIGHT ) {
                obj_rect->top = obj_rect->bottom - DIALOG_MIN_HEIGHT;
            }
        } else {
            obj_rect->bottom = obj_rect->top + DIALOG_MIN_WIDTH;
        }
    }

    if( !Forward( new->parent, GET_WINDOW_HANDLE, &new->parent_handle, NULL ) ) {
        WdeWriteTrail( "WdeDialogCreater: Couldn't get parent window handle!" );
        WdeFreeDialogBoxHeader( &new->dialog_info );
        WRMemFree( new );
        return( NULL );
    }

    if( handle == NULL ) {
        new->object_handle = new;
    } else {
        new->object_handle = handle;
    }

    new->o_item = Create( OBJ_ITEM, new->parent, obj_rect, new->object_handle );

    if( new->o_item == NULL ) {
        WdeWriteTrail( "WdeDialogCreater: OITEM not created!" );
        WdeFreeDialogBoxHeader( &new->dialog_info );
        WRMemFree( new );
        return( NULL );
    }

    if( !Forward( new->parent, GET_FONT, &new->font, NULL ) ) {
        WdeWriteTrail( "WdeDialogCreater: Couldn't get parent font!" );
        WdeFreeDialogBoxHeader( &new->dialog_info );
        WRMemFree( new );
        return( NULL );
    }

    if( !WdeGetStickyMode() ) {
        WdeSetBaseObject( IDM_SELECT_MODE );
    }

    return( new );
}

WINEXPORT BOOL CALLBACK WdeDialogDispatcher( ACTION act, WdeDialogObject *obj, void *p1, void *p2 )
{
    int     i;

    WdeDebugDispatch( "Dialog", act, obj, p1, p2 );

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeDialogActions[i].id == act ) {
            if( WdeDialogActions[i].rtn ) {
                return( WdeDialogActions[i].rtn( obj, p1, p2 ) );
            } else {
                return( Forward( obj->parent, act, p1, p2 ) );
            }
        }
    }

    return( Forward( obj->o_item, act, p1, p2 ) );
}

bool WdeDialogInit( bool first )
{
    char        *text;
    bool        use_default;

    _wde_touch( first );

    WdeAppInst = WdeGetAppInstance();

    WdeDefaultDialog = WdeAllocDialogBoxHeader();
    if( WdeDefaultDialog == NULL ) {
        WdeWriteTrail( "WdeDialogInit: Alloc of dialog failed!" );
        return( FALSE );
    }
#ifdef __NT__
    WdeDefaultDialog->is32bit = TRUE;
#endif

    /* set up the default control structure */
    SETHDR_STYLE( WdeDefaultDialog, WS_CAPTION | WS_SYSMENU |
                                    WS_VISIBLE | DS_MODALFRAME | DS_SETFONT );

    // the following assumes the same memory allocation scheme is
    // being used for regular stuff and RC strings
    text = WdeAllocRCString( WDE_DEFAULTDIALOGCAPTION );
    SETHDR_CAPTION( WdeDefaultDialog, text );

    use_default = TRUE;

#ifdef WDE_USE_RC_FONTS
    char        *cp;
    int         point_size;
    text = WdeAllocRCString( WDE_DEFAULTDIALOGFONT );
    if( text != NULL ) {
        cp = _mbschr( text, '.' );
        if( cp != NULL ) {
            *cp = '\0';
            SETHDR_FONTNAME( WdeDefaultDialog, WdeStrDup( text ) );
            cp++;
            point_size = atoi( cp );
            SETHDR_POINTSIZE( WdeDefaultDialog, point_size );
            use_default = FALSE;
        }
        WdeFreeRCString( text );
    }
#endif

    if( use_default ) {
        if( GetSystemMetrics( SM_DBCSENABLED ) ) {
            SETHDR_FONTNAME( WdeDefaultDialog, WdeStrDup( DEFAULT_JFONT ) );
            SETHDR_POINTSIZE( WdeDefaultDialog, DEFAULT_JPOINT_SIZE );
        } else {
            SETHDR_FONTNAME( WdeDefaultDialog, WdeStrDup( DEFAULT_FONT ) );
            SETHDR_POINTSIZE( WdeDefaultDialog, DEFAULT_POINT_SIZE );
        }
    }

    WdeDialogDispatch = MakeProcInstance( (FARPROC)WdeDialogDispatcher, WdeAppInst );
    WdeDialogDefineProcInst = MakeProcInstance( (FARPROC)WdeDialogDefineProc, WdeAppInst );
    WdeTestProcInst = (DLGPROC)MakeProcInstance( (FARPROC)WdeTestDlgProc, WdeAppInst );
    WdeDialogProcInst = (DLGPROC)MakeProcInstance ( (FARPROC)WdeDialogProc, WdeAppInst );

    return( TRUE );
}

void WdeDialogFini( void )
{
    WdeFreeDialogBoxHeader( &WdeDefaultDialog );
    FreeProcInstance( WdeDialogDefineProcInst);
    FreeProcInstance( (FARPROC)WdeTestProcInst );
    FreeProcInstance( (FARPROC)WdeDialogProcInst );
    FreeProcInstance( WdeDialogDispatch );
}

BOOL WdeDialogResolveSymbol( WdeDialogObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    OBJPTR              child;
    LIST                *olist;
    void                *vp;
    bool                found;

    if( obj->res_info == NULL ) {
        return( FALSE );
    }

    for( olist = obj->children; olist; olist = ListNext( olist ) ) {
        child =  ListElement( olist );
        Forward( child, RESOLVE_SYMBOL, b, from_id );
    }

    if( !obj->name->IsName ) {
        if( from_id != NULL && *from_id ) {
            vp = WdeResolveValue( obj->res_info->hash_table,
                                  (WdeHashValue)obj->name->ID.Num );
            if( vp != NULL ) {
                if( obj->symbol != NULL ) {
                    WRMemFree( obj->symbol );
                }
                obj->symbol = vp;
                WdeDialogModified( obj );
            }
        } else {
            if( obj->symbol != NULL ) {
                val = WdeLookupName( obj->res_info->hash_table, obj->symbol, &found );
                if( found ) {
                    obj->name->ID.Num = (uint_16)val;
                    WdeDialogModified( obj );
                } else {
                    WRMemFree( obj->symbol );
                    obj->symbol = NULL;
                }
            }
        }
        if( b != NULL && *b && obj->object_handle == GetCurrObject() ) {
            WdeWriteDialogToInfo( obj );
        }
    }

    return( TRUE );
}

BOOL WdeDialogResolveHelpSymbol( WdeDialogObject *obj, bool *b, bool *from_id )
{
    WdeHashValue        val;
    OBJPTR              child;
    LIST                *olist;
    void                *vp;
    bool                found;

    if( obj->res_info == NULL ) {
        return( FALSE );
    }

    for( olist = obj->children; olist != NULL; olist = ListNext( olist ) ) {
        child =  ListElement( olist );
        Forward( child, RESOLVE_HELPSYMBOL, b, from_id );
    }

    if( from_id != NULL && *from_id ) {
        vp = WdeResolveValue( obj->res_info->hash_table,
                              (WdeHashValue)GETHDR_HELPID( obj->dialog_info ) );
        if( vp != NULL ) {
            if( obj->helpsymbol != NULL ) {
                WRMemFree( obj->helpsymbol );
            }
            obj->helpsymbol = vp;
            obj->dialog_info->helpsymbol = WdeStrDup( obj->helpsymbol );
            WdeDialogModified( obj );
        }
    } else {
        if( obj->helpsymbol ) {
            val = WdeLookupName( obj->res_info->hash_table,
                                 obj->helpsymbol, &found );
            if( found ) {
                SETHDR_HELPID( obj->dialog_info, val );
                WdeDialogModified( obj );
            } else {
                WRMemFree( obj->helpsymbol );
                obj->helpsymbol = NULL;
            }
        }
    }
    if( b != NULL && *b && obj->object_handle == GetCurrObject() ) {
        WdeWriteDialogToInfo( obj );
    }

    return( TRUE );
}

BOOL WdeDialogModifyInfo( WdeDialogObject *obj, WdeInfoStruct *in, void *p2 )
{
    WResID              *old_name;
    char                *old_symbol;
    WdeHashEntry        *entry;
    bool                dup;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( in->d.caption ) {
        if( GETHDR_CAPTION( obj->dialog_info ) ) {
            WRMemFree( GETHDR_CAPTION( obj->dialog_info ) );
        }
        SETHDR_CAPTION( obj->dialog_info, in->d.caption );
        if( GETHDR_CAPTION( obj->dialog_info ) ) {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPSTR)GETHDR_CAPTION( obj->dialog_info ) );
        } else {
            SendMessage( obj->window_handle, WM_SETTEXT, 0, (LPARAM)(LPCSTR)"" );
        }
    }

    old_name = obj->name;
    old_symbol = obj->symbol;

    if( in->symbol != NULL ) {
        dup = FALSE;
        obj->symbol = in->symbol;
        entry = WdeDefAddHashEntry( obj->res_info->hash_table, obj->symbol, &dup );
        if( entry != NULL ) {
            obj->name = WResIDFromNum( entry->value );
            in->d.name = obj->name;
        } else {
            obj->name = old_name;
            obj->symbol = old_symbol;
            old_name = NULL;
            old_symbol = NULL;
        }
    } else if( in->d.name ) {
        obj->name = in->d.name;
    }

    if( old_symbol != NULL ) {
        WRMemFree( old_symbol );
    }

    if( old_name ) {
        WResIDFree( old_name );
    }

    WdeDialogModified( obj );

    return( TRUE );
}

BOOL WdeDialogTest( WdeDialogObject *obj, void *p1, void *p2 )
{
    GLOBALHANDLE    dialog_template;
    uint_8          *locked_global_mem;
    char            *MenuName;
    char            *ClassName;
    DWORD           style;
    LIST            *clist;
    WdeOrderedEntry *oentry;
    HWND            hwin;
    uint_16         flag;

    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    SETHDR_NUMITEMS( obj->dialog_info, obj->num_children );

    MenuName = WdeResNameOrOrdinalToStr( GETHDR_MENUNAME( obj->dialog_info ), 10 );
    if( MenuName != NULL && *MenuName == '\0' ) {
        WRMemFree( MenuName );
        MenuName = NULL;
    }

    ClassName = WdeResNameOrOrdinalToStr( GETHDR_CLASSNAME( obj->dialog_info ), 10 );
    if( ClassName != NULL && *ClassName == '\0' ) {
        WRMemFree( ClassName );
        ClassName = NULL;
    }

    style = GETHDR_STYLE( obj->dialog_info );

    /* we don't like child windows just yet -- problems with redrawing
     * edit environment
     */
    if( style & WS_CHILD ) {
        style ^= WS_CHILD;
        style |= WS_POPUP;
    }

    if( style & DS_SYSMODAL ) {
        style ^= DS_SYSMODAL;
    }

    if( obj->dialog_info->is32bitEx ) {
        dialog_template = DialogEXTemplate( (DWORD)style, GETHDR_EXSTYLE( obj->dialog_info ),
            GETHDR_HELPID( obj->dialog_info ), GETHDR_SIZEX( obj->dialog_info ),
            GETHDR_SIZEY( obj->dialog_info ), GETHDR_SIZEW( obj->dialog_info ),
            GETHDR_SIZEH( obj->dialog_info ), MenuName, ClassName,
            GETHDR_CAPTION( obj->dialog_info ), GETHDR_POINTSIZE( obj->dialog_info ),
            GETHDR_FONTNAME( obj->dialog_info ), GETHDR_FONTWEIGHT( obj->dialog_info ),
            GETHDR_FONTITALIC( obj->dialog_info ) );
    } else {
        dialog_template = DialogTemplate( style, GETHDR_SIZEX( obj->dialog_info ),
            GETHDR_SIZEY( obj->dialog_info ), GETHDR_SIZEW( obj->dialog_info ),
            GETHDR_SIZEH( obj->dialog_info ), MenuName, ClassName,
            GETHDR_CAPTION( obj->dialog_info ), GETHDR_POINTSIZE( obj->dialog_info ),
            GETHDR_FONTNAME( obj->dialog_info ) );
    }

    if( MenuName != NULL ) {
        WRMemFree( MenuName );
    }

    if( ClassName != NULL ) {
        WRMemFree( ClassName );
    }

    if( dialog_template == NULL ) {
        WdeWriteTrail( "WdeDialogTest: DialogTemplate failed!" );
        return( FALSE );
    }

    WdeCleanOrderedList( &obj->ochildren );

    if( obj->dialog_info->is32bitEx ) {
        flag = TESTEX;
    } else {
        flag = TEST;
    }
    /* add all the children */
    for( clist = obj->ochildren; clist != NULL; clist = ListNext( clist ) ) {
        oentry = ListElement( clist );
        if( !Forward( oentry->obj, flag, &dialog_template, NULL ) ) {
            WdeWriteTrail( "WdeDialogTest: control TEST failed!" );
            GlobalFree( dialog_template );
            return( FALSE );
        }
    }

    DoneAddingControls( dialog_template );

    locked_global_mem = (uint_8 *)GlobalLock( dialog_template );

    if( locked_global_mem == NULL ) {
        WdeWriteTrail( "WdeDialogTest: Could not Lock template!" );
        GlobalFree( dialog_template );
        return( FALSE );
    }

    hwin = CreateDialogIndirectParam( WdeAppInst, WDEDLGTEMPLATE locked_global_mem,
                                      obj->res_info->forms_win, WdeTestProcInst,
                                      (LPARAM)obj );

    GlobalUnlock( dialog_template );

    GlobalFree( dialog_template );

    if( hwin == NULL ) {
        WdeWriteTrail( "WdeDialogTest: Could not create window!" );
        ShowWindow( obj->window_handle, SW_SHOW );
        return( FALSE );
    }

    if( obj->num_children != 0 && !WdeSetTestControlDefaults( hwin ) ) {
        WdeWriteTrail( "WdeDialogTest: WdeSetTestControlDefaults failed!" );
    }

    ShowWindow( hwin, SW_SHOW );

    return( TRUE );
}

BOOL WdeDialogRestore( WdeDialogObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( WdeIsDialogRestorable( obj ) ) {
        ShowWindow( obj->window_handle, SW_HIDE );
        if( !WdeOpenDialogFromResInfo( obj->res_info, obj->dlg_item ) ) {
            ShowWindow( obj->window_handle, SW_SHOW );
            obj->dlg_item->object = obj;
            return( FALSE );
        }
        MakeObjectCurrent( obj->dlg_item->object );
        Destroy( (OBJPTR)obj, FALSE );
    } else {
        return( FALSE );
    }

    return( TRUE );
}

bool WdeIsDialogRestorable( void *_obj )
{
    WdeDialogObject *obj = _obj;

    return( obj != NULL && obj->res_info != NULL && obj->dlg_item != NULL &&
            obj->dlg_item->dialog_info != NULL );
}

BOOL WdeDialogSaveObject( WdeDialogObject *obj, WORD *id, void *p2 )
{
    WdeDialogBoxInfo    *dbi;
    uint_8              ret;
    WResLangType        lang;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    ret = FALSE;

    if( WdeIsHashTableDirty( obj->res_info->hash_table ) ) {
        Forward( obj->object_handle, RESOLVE_SYMBOL, NULL, NULL );
        Forward( obj->object_handle, RESOLVE_HELPSYMBOL, NULL, NULL );
    }

    dbi = WdeDBIFromObject( obj );
    if( dbi == NULL ) {
        return( FALSE );
    }

    if( obj->dlg_item->dialog_info != NULL ) {
        WdeFreeDialogBoxInfo( obj->dlg_item->dialog_info );
    }
    obj->dlg_item->dialog_info = dbi;

    if( obj->dlg_item->lnode != NULL ) {
        lang = obj->dlg_item->lnode->Info.lang;
    } else {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
    }

    switch( *id ) {
    case IDM_DIALOG_SAVE:
        ret = WdeSaveObject( obj->res_info, dbi, &obj->file_name,
                             obj->name, &lang, FALSE, FALSE );
        break;

    case IDM_DIALOG_SAVEAS:
        ret = WdeSaveObject( obj->res_info, dbi, &obj->file_name,
                             obj->name, &lang, FALSE, TRUE );
        break;

    case IDM_DIALOG_SAVEINTO:
        ret = WdeSaveObject( obj->res_info, dbi, &obj->file_name,
                             obj->name, &lang, TRUE, TRUE );
        break;
    }

    return( ret );
}

BOOL WdeDialogGetResizeInc( WdeDialogObject *obj, POINT *p, void *p2 )
{
    DialogSizeInfo      d;
    RECT                r;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    p->x = 1;
    p->y = 1;

    d.x = 0;
    d.y = 0;
    d.width = WdeGetOption( WdeOptReqGridX );
    d.height = WdeGetOption( WdeOptReqGridY );

    if( obj->window_handle != (HWND)NULL ) {
        SetRect( &r, 0, 0, d.width, d.height );
        MapDialogRect( obj->window_handle, &r );
        p->x = r.right;
        p->y = r.bottom;
    } else {
        if( WdeDialogToScreen( obj, &obj->resizer, &d, &r ) ) {
            p->x = r.right;
            p->y = r.bottom;
        }
    }

    return( TRUE );
}

BOOL WdeDialogGetScrollRect( WdeDialogObject *obj, RECT *r, void *p2 )
{
    return( Forward( obj->parent, GET_SCROLL_RECT, r, p2 ) );
}

BOOL WdeDialogGetResizer( WdeDialogObject *obj, WdeResizeRatio *resizer, OBJPTR *o )
{
    *resizer = obj->resizer;
    if( o != NULL ) {
        *o = obj;
    }

    return( TRUE );
}

BOOL WdeDialogGetFont( WdeDialogObject *obj, HFONT *font, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *font = obj->font;

    return( TRUE );
}

BOOL WdeDialogIdentify( WdeDialogObject *obj, OBJ_ID *id, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *id = obj->object_id;

    return( TRUE );
}

BOOL WdeDialogDefine( WdeDialogObject *obj, POINT *pnt, void *p2 )
{
    WdeDefineObjectInfo  o_info;
    bool                 redraw;
    bool                 quick;
    bool                 destroy_children;

    /* touch unused vars to get rid of warning */
    _wde_touch( pnt );
    _wde_touch( p2 );

    if( obj->mode != WdeSelect ) {
        return( TRUE );
    }

    o_info.obj = obj;
    o_info.obj_id = obj->object_id;
    o_info.win = obj->window_handle;
    o_info.symbol = obj->symbol;
    o_info.helpsymbol = obj->helpsymbol;
    o_info.hook_func = WdeWinStylesHook;
    o_info.set_func = NULL;
    o_info.get_func = NULL;
    o_info.res_info = obj->res_info;
    o_info.info.d.name = obj->name;
    o_info.info.d.header = obj->dialog_info;
    o_info.mask = WS_POPUP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE |
                  WS_DISABLED | WS_BORDER | WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL |
                  WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX  | WS_MAXIMIZEBOX;

    if( !WdeGetOption( WdeOptUseDefDlg ) ) {
        redraw = WdeGenericDefine( &o_info );
    } else {
        WdeSetStatusText( NULL, "", FALSE );
        WdeSetStatusByID( WDE_DEFININGDIALOG, -1 );

        redraw = JDialogBoxParam( WdeAppInst, "WdeDefineDIALOG", obj->window_handle,
                                  (DLGPROC)WdeDialogDefineProcInst, (LPARAM)&o_info );

        if( redraw ) {
            quick = TRUE;
            destroy_children = TRUE;
            HideSelectBoxes();

            if( !WdeDialogDestroyWindow( obj, &quick, &destroy_children ) ) {
                WdeWriteTrail( "WdeDialogDefine: WdeDialogDestroyWindow failed!" );
                return( FALSE );
            }

            if( !WdeDialogCreateWindow( obj, NULL, NULL ) ) {
                WdeWriteTrail( "WdeDialogDefine: CREATE_WINDOW failed!" );
                return( FALSE );
            }

            ShowSelectBoxes();

            UpdateScroll();
        }
    }

    if( redraw ) {
        obj->symbol = o_info.symbol;
        obj->helpsymbol = o_info.helpsymbol;
        obj->name = o_info.info.d.name;
        WdeWriteDialogToInfo( obj );
        WdeDialogModified( obj );
    }

    WdeSetStatusReadyText();

    return( TRUE );
}

BOOL WdeDialogIsMarkValid( WdeDialogObject *obj, BOOL *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->mode == WdeSelect ) {
        *flag = (obj->window_handle != NULL && IsWindowVisible( obj->window_handle ));
    } else {
        *flag = FALSE;
    }

    return( TRUE );
}

BOOL WdeDialogDestroy( WdeDialogObject *obj, bool *flag, bool *hide )
{
    LIST        *clist;
    OBJPTR      sub_obj;
    OBJPTR      parent;
    RECT        rect;
    bool        quick, b;
    bool        destroy_children;

    /* touch unused vars to get rid of warning */
    _wde_touch( flag );

    parent = obj->parent;

    if( obj->mode != WdeSelect ) {
        WdeDialogSetOrderMode( obj, &obj->mode, NULL );
    }

    if( hide != NULL && *hide ) {
        WdePreserveDialogWithDBI( obj );
        if( obj->res_info != NULL ) {
            sub_obj = WdeGetNextObject( FALSE, obj, parent );
            if( sub_obj == NULL ) {
                sub_obj = parent;
            }
            obj->res_info->next_current = sub_obj;
        }
    } else {
        WdeRemoveObject( obj->res_info, obj );
    }

    Location( obj->object_handle, &rect );

    if( parent != NULL ) {
        if( !RemoveObject( parent, obj->object_handle ) ) {
            WdeWriteTrail( "WdeDialogDestroy: RemoveObject failed!" );
        }
    }

    quick = TRUE;
    destroy_children = TRUE;

    if( !WdeDialogDestroyWindow( obj, &quick, &destroy_children ) ) {
        WdeWriteTrail( "WdeDialogDestroy: WdeDialogDestroyWindow failed!" );
    }

    b = FALSE;

    // we do this BEFORE we destroy the children as a child if this
    // dialog may be the current object
    ObjectDestroyed( obj->object_handle );
    if( hide != NULL && *hide ) {
        if( parent != NULL ) {
            MakeObjectCurrent( parent );
        }
    }

    /* destroy all children */
    clist = WdeListCopy( obj->children );
    for( ; clist != NULL; clist = ListConsume( clist ) ) {
        sub_obj = ListElement( clist );
        if( !Forward( sub_obj, DESTROY, &b, NULL ) ) {
            WdeWriteTrail( "WdeDialogDestroy: Child DESTROY failed" );
        }
    }

    if( !Forward( obj->o_item, DESTROY, &b, NULL ) ) {
        WdeWriteTrail( "WdeDialogDestroy: Remove OITEM failed!" );
        return( FALSE );
    }

    WdeFreeOrderedList( obj->ochildren );

    if( obj->res_info != NULL ) {
        if( obj->res_info->next_current == obj->object_handle ) {
            obj->res_info->next_current = NULL;
        }
    }

    WdeFreeDialogObject( obj );

    MarkInvalid( &rect );

    WdeCheckBaseScrollbars( FALSE );

    return( TRUE );
}

void WdeFreeDialogObject( WdeDialogObject *obj  )
{
    if( obj->dialog_info != NULL ) {
        WdeFreeDialogBoxHeader( &obj->dialog_info );
    }

    if( obj->name ) {
        WResIDFree( obj->name );
    }

    if( obj->symbol != NULL ) {
        WRMemFree( obj->symbol );
    }

    if( obj->file_name != NULL ) {
        WRMemFree( obj->file_name );
    }

//  if( obj->helpname ) {
//      WResHelpIDFree( obj->helpname );
//  }

    if( obj->helpsymbol != NULL ) {
        WRMemFree( obj->helpsymbol );
    }

    WRMemFree( obj );
}

BOOL WdeDialogCreateWindow( WdeDialogObject *obj, bool *show, void *p2 )
{
    HGLOBAL             hglobal_mem;
    uint_8              *locked_global_mem;
    RECT                rect;
    LIST                *olist;
    OBJPTR              child;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    //SETHDR_STYLE( obj->dialog_info, GETHDR_STYLE( obj->dialog_info ) & ~WS_VISIBLE );

    if( !WdeBuildDialogTemplate( obj->dialog_info, &hglobal_mem ) ) {
        return( FALSE );
    }

    locked_global_mem = (uint_8 *)GlobalLock( hglobal_mem );

    if( locked_global_mem == NULL ) {
        WdeWriteTrail( "WdeDialogCreateWindow: Could not lock template!" );
        GlobalFree( hglobal_mem );
        return( FALSE );
    }

    if( !Forward( obj->parent, GET_FONT, &WdeLastFont, NULL ) ) {
        WdeWriteTrail( "WdeDialogCreateWindow: Couldn't get parent font!" );
        GlobalFree( hglobal_mem );
        return( FALSE );
    }

    obj->window_handle = CreateDialogIndirect( WdeAppInst, WDEDLGTEMPLATE locked_global_mem,
                                               obj->parent_handle, WdeDialogProcInst );

    GlobalUnlock( hglobal_mem );

    GlobalFree( hglobal_mem );

    if( obj->window_handle == NULL ) {
        WdeWriteTrail( "WdeDialogCreateWindow: Could not create window!" );
        return( FALSE );
    }

    // get the expected location of the dialog
    Location( (OBJPTR)obj, &rect );
    SetWindowPos( obj->window_handle, (HWND)NULL,
                  rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

    if( !WdeCalcDialogNCSize( obj, &obj->nc_size ) ) {
        WdeWriteTrail( "WdeDialogCreateWindow: WdeCalcDialogNCSize failed!" );
    }

    SetRect( &rect, 4, 8, 0, 0 );
    MapDialogRect( obj->window_handle, &rect );
    obj->resizer.xmap = rect.left;
    obj->resizer.ymap = rect.top;

    if( !WdeDialogSetFont( obj, &WdeLastFont, &obj->resizer ) ) {
        WdeWriteTrail( "WdeDialogCreateWindow: SET_FONT failed!" );
        return( FALSE );
    }

    /* let's recreate the children in reverse order */
    if( obj->children != NULL ) {
        WdeListLastElt( obj->children, &olist );
    } else {
        olist = NULL;
    }

    for( ; olist; olist = ListPrev( olist ) ) {
        OBJ_ID  id;
        child = ListElement( olist );
        if( !Forward( child, CREATE_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeDialogCreateWindow: Failed to create child window!" );
            return( FALSE );
        }
        if( Forward( child, IDENTIFY, &id, NULL ) && id == SBAR_OBJ ) {
            WdeSBNoodleSize( child, TRUE );
        }
    }

    WdeDialogOnTop( obj, NULL, NULL );

    SendMessage( obj->window_handle, WM_NCACTIVATE, TRUE, 0 );

    if( show == NULL || (show != NULL && *show) ) {
        ShowWindow( obj->window_handle, SW_SHOW );
    }

    return( TRUE );
}

BOOL WdeDialogOnTop( WdeDialogObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    WdeBringWindowToTop( obj->window_handle );
#ifdef __NT__
    SendMessage( obj->window_handle, WM_NCACTIVATE, (WPARAM)TRUE, 0 );
#endif

    return( TRUE );
}

BOOL WdeDialogAddSubObject( WdeDialogObject *dialog, OBJPTR obj, void *p2 )
{
    OBJ_ID      id;
    RECT        dialog_rect;
    RECT        obj_rect;
    POINT       pnt;
    int         fudge;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    /* make sure the object is not another dialog object */
    Forward( (OBJPTR)obj, IDENTIFY, &id, NULL );
    if( id == DIALOG_OBJ ) {
        return( Forward( dialog->parent, ADD_SUBOBJECT, obj, p2 ) );
    }

    /* make sure the objects top left corner is south east of the
     * dialogs top left corner
     */
    Location( obj, &obj_rect );
    Location( (OBJPTR)dialog, &dialog_rect );

    dialog_rect.left += dialog->nc_size.left;
    dialog_rect.right -= dialog->nc_size.right;
    dialog_rect.top += dialog->nc_size.top;
    dialog_rect.bottom -= dialog->nc_size.bottom;

    pnt.x = obj_rect.left;
    pnt.y = obj_rect.top;

    fudge = 0;
    if( id == SBAR_OBJ ) {
        fudge = 10;
    }

    if( pnt.x < dialog_rect.left - fudge || pnt.y < dialog_rect.top ) {
        WdeWriteTrail( "WdeDialogAddSubObject: Cntrl not contained by dialog top left!" );
        return( FALSE );
    }

    if( dialog->num_children < WDE_DIALOG_MAX_CONTROLS ) {
        /* object will be inserted at the end of the list */
        WdeInsertObject( &dialog->children, obj );
        WdeAddOrderedEntry( &dialog->ochildren, obj );
        dialog->num_children++;

        if( !Notify( obj, NEW_PARENT, dialog->object_handle ) ) {
            WdeWriteTrail( "WdeDialogAddSubObject: NEW_PARENT notify failed!" );
            return( FALSE );
        }
    } else {
        WdeWriteTrail( "WdeDialogAddSubObject: Dialog full!" );
        WdeDisplayErrorMsg( WDE_DIALOGFULL );
        return( FALSE );
    }

    return( TRUE );
}

bool WdeCalcDialogNCSize( WdeDialogObject *obj, RECT *size )
{
    RECT          win_rect;
    RECT          client_rect;

    if( obj != NULL && obj->window_handle != NULL ) {
        GetWindowRect( obj->window_handle, &win_rect );
        GetClientRect( obj->window_handle, &client_rect );
        WdeMapWindowRect( obj->window_handle, (HWND)NULL, &client_rect );

        size->left = client_rect.left - win_rect.left;
        size->top = client_rect.top - win_rect.top;
        size->right = win_rect.right - client_rect.right;
        size->bottom = win_rect.bottom - client_rect.bottom;
    } else {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeDialogGetNCSize( WdeDialogObject *obj, RECT *size, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *size = obj->nc_size;

    return( TRUE );
}

BOOL WdeDialogFindSubObjects( WdeDialogObject *obj, SUBOBJ_REQUEST *req, LIST **obj_list )
{
    return( WdeFindSubObjects( req, obj_list, obj->children ) );
}

BOOL WdeDialogFindObjectsPt( WdeDialogObject *obj, POINT *pt, LIST **obj_list )
{
    LIST   *subobjs;

    if( WdeFindObjectsAtPt( pt, &subobjs, obj->children ) ) {
        ListAddElt( obj_list, ListElement( subobjs ) );
        ListFree( subobjs );
    } else {
        ListAddElt( obj_list, obj );
    }

    return( TRUE );
}

BOOL WdeDialogGetNextChild( WdeDialogObject *obj, OBJPTR *o, bool *up )
{
    return( WdeGetNextChild( &obj->ochildren, o, *up ) );
}

BOOL WdeDialogRemoveSubObject( WdeDialogObject *dialog, OBJPTR obj, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( dialog->num_children != 0 && ListFindElt( dialog->children, obj ) ) {
        ListRemoveElt( &dialog->children, obj );
        WdeRemoveOrderedEntry( dialog->ochildren, obj );
        dialog->num_children--;
        if( dialog->num_children == 0 ) {
            dialog->children = NULL;
        }
    } else {
        return( FALSE );
    }

    WdeDialogModified( dialog );

    return( TRUE );
}

BOOL WdeDialogGetSubObjectList( WdeDialogObject *obj, LIST **l, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *l = obj->children;

    return( TRUE );
}

BOOL WdeDialogDraw( WdeDialogObject *obj, RECT *area, HDC *dc )
{
    RECT        rect;
    POINT       origin;
    RECT        trect;
    OBJPTR      child;
    RECT        child_rect;

    Location( obj->object_handle, &rect );

    GetOffset( &origin );

    if( obj->children != NULL ) {
        child = ListElement( obj->children );
        Location( child, &child_rect ) ;
        if( IntersectRect( &trect, area, &child_rect ) ) {
            if( !Forward( child, ON_TOP, NULL, NULL ) ) {
                WdeWriteTrail( "WdeDialogDraw: child ON_TOP failed!" );
                return( FALSE );
            }
        }
    }

    if( dc != NULL && IntersectRect( &trect, area, &rect ) ) {
        OffsetRect( &trect, -origin.x, -origin.y );
        RedrawWindow( obj->res_info->edit_win, &trect, (HRGN)NULL,
                      RDW_INTERNALPAINT | //RDW_INVALIDATE |
                      RDW_UPDATENOW );
        //SendMessage( obj->window_handle, WM_NCACTIVATE, (WPARAM)TRUE, NULL );
    }

    OffsetRect( &rect, -origin.x, -origin.y );

    SetWindowPos( obj->window_handle, (HWND)NULL, rect.left, rect.top, 0, 0,
                  SWP_NOZORDER | SWP_NOSIZE | (dc != NULL ? 0 : SWP_NOREDRAW) );

    SendMessage( obj->window_handle, WM_NCACTIVATE, (WPARAM)TRUE, 0 );

    return( TRUE );
}

BOOL WdeDialogSetFont( WdeDialogObject *obj, HFONT *font, WdeResizeRatio *resizer )
{
    LIST        *olist;
    OBJPTR      child;

    obj->font = *font;

    if( !WdeKludgeDialogSize( obj, TRUE, FALSE ) ) {
        WdeWriteTrail( "WdeDialogSetFont: Couldn't kludge size!" );
        return( FALSE );
    }

    /* we must set the font of all children in reverse order */
    if( obj->children != NULL ) {
        WdeListLastElt( obj->children, &olist );
    } else {
        olist = NULL;
    }

    for( ; olist != NULL; olist = ListPrev( olist ) ) {
        child = ListElement( olist );
        if( !Forward( child, SET_FONT, font, resizer ) ) {
            WdeWriteTrail( "WdeDialogSetFont: Couldn't set child font!" );
            return( FALSE );
        }
    }

    return( TRUE );
}

BOOL WdeDialogDestroyWindow( WdeDialogObject *obj, bool *quick, bool *destroy_children )
{
    LIST        *olist;
    OBJPTR      child;
    RECT        rect;

    Location( obj->object_handle, &rect );

    if( destroy_children != NULL && *destroy_children ) {
        for( olist = obj->children; olist != NULL; olist = ListNext( olist ) ) {
            child = ListElement( olist );
            if( !Forward( child, DESTROY_WINDOW, quick, NULL ) ) {
                WdeWriteTrail( "WdeDialogDestroyWindow: DESTROY_WINDOW failed!" );
            }
        }
    }

    if( obj->window_handle != NULL ) {
        DestroyWindow( obj->window_handle );
    }

    obj->window_handle = NULL;

    MarkInvalid( &rect );

    return( TRUE );
}

BOOL WdeDialogShowWindow( WdeDialogObject *obj, bool *flag, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( flag == NULL ) {
        return( FALSE );
    }

    WdeShowObjectWindow( obj->window_handle, *flag );

    return( TRUE );
}

BOOL WdeDialogGetResizeInfo( WdeDialogObject *obj, RESIZE_ID *info, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( obj );
    _wde_touch( p2 );

    if( obj->mode == WdeSelect ) {
        *info = R_ALL;
    } else {
        *info = R_NONE;
    }

    return( TRUE );
}

static BOOL WdeDialogValidateMove( WdeDialogObject *obj, POINT *pnt, ACTION act )
{
    RECT obj_rect;
    RECT nc_size;

    if( act == MOVE && obj->mode != WdeSelect ) {
        return( FALSE );
    }

    Location( (OBJPTR)obj, &obj_rect );

    nc_size.left = obj->nc_size.left;
    if( nc_size.left < DIALOG_MIN_SIZE_BORDER )
        nc_size.left = DIALOG_MIN_SIZE_BORDER;
    nc_size.top = obj->nc_size.top;
    if( nc_size.top < DIALOG_MIN_SIZE_BORDER )
        nc_size.top = DIALOG_MIN_SIZE_BORDER;
    nc_size.right = obj->nc_size.right;
    if( nc_size.right < DIALOG_MIN_SIZE_BORDER )
        nc_size.right = DIALOG_MIN_SIZE_BORDER;
    nc_size.bottom = obj->nc_size.bottom;
    if( nc_size.bottom < DIALOG_MIN_SIZE_BORDER )
        nc_size.bottom = DIALOG_MIN_SIZE_BORDER;

    obj_rect.left += nc_size.left;
    obj_rect.right -= nc_size.right;
    obj_rect.top += nc_size.top;
    obj_rect.bottom -= nc_size.bottom;

    if( PtInRect( &obj_rect, *pnt ) ) {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeDialogValidateAction( WdeDialogObject *obj, ACTION *act, void *p2 )
{
    int     i;

    if( *act == MOVE || *act == PICK ) {
        return( WdeDialogValidateMove( obj, (POINT *)p2, *act ) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( WdeDialogActions[i].id == *act ) {
            return( TRUE );
        }
    }

    return( ValidateAction( (OBJPTR)obj->o_item, *act, p2 ) );
}

BOOL WdeDialogGetWindowHandle( WdeDialogObject *obj, HWND *hwin, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *hwin = obj->window_handle;

    return( TRUE );
}

BOOL WdeDialogFirstChild( WdeDialogObject *obj, void *p1, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p1 );
    _wde_touch( p2 );

    if( obj->parent == NULL ) {
        return( TRUE );
    }

    if( !Forward( obj->parent, PUT_ME_FIRST, obj->object_handle, NULL ) ) {
        WdeWriteTrail( "WdeDialogFirstChild: PUT_ME_FIRST failed!" );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeDialogPutChildFirst( WdeDialogObject *obj, OBJPTR child, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->num_children != 0) {
        if( !WdePutObjFirst( child, &obj->children ) ) {
            WdeWriteTrail( "WdeDialogPutChildFirst: WdePutObjFirst failed!" );
            return( FALSE );
        }
    } else {
        WdeWriteTrail( "WdeDialogPutChildFirst: No children!" );
        return( FALSE );
    }

    return( TRUE );
}

void WdeWriteDialogToInfo( WdeDialogObject *obj )
{
    WdeInfoStruct is;

    is.obj_id = DIALOG_OBJ;
    is.res_info = obj->res_info;
    is.obj = obj->object_handle;
    is.size = GETHDR_SIZE( obj->dialog_info );
    is.d.caption = GETHDR_CAPTION( obj->dialog_info );
    is.d.name = obj->name;
    is.symbol = WdeStrDup( obj->symbol );

    WdeWriteInfo( &is );

    if( is.symbol != NULL ) {
        WRMemFree( is.symbol );
    }
}

BOOL WdeDialogNotify( WdeDialogObject *obj, NOTE_ID *id, void *p2 )
{
    HWND        handle;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    switch( *id ) {
    case MOVE_START:
        HideSelectBoxes();
        break;
    case MOVE_END:
        ShowSelectBoxes();
        break;
    case PRIMARY_OBJECT:
        /* make sure the current object is first in the parent list */
        /* I know, a child object has implementation knowlegde of its
         * parent. C'est la vie.
         */
        if( !WdeDialogFirstChild( obj, NULL, NULL ) ) {
            WdeWriteTrail( "WdeDialogNotify: FIRST_CHILD falied!" );
            return( FALSE );
        }

        WdeDialogOnTop( obj, NULL, NULL );

        WdeSetDialogObjectMenu( WdeIsDialogRestorable( obj ),
                                obj->res_info && obj->res_info->hash_table, obj->mode );

        WdeWriteDialogToInfo( obj );

        return( TRUE );

    case NEW_PARENT:
        if( p2 == NULL ) {
            obj->parent = NULL;
            obj->parent_handle = NULL;
        } else {
            if( obj->parent != (OBJPTR)p2 ) {
                obj->parent = (OBJPTR)p2;
                if( !Forward( (OBJPTR)obj->parent, GET_WINDOW_HANDLE, &handle, NULL ) ) {
                    return ( FALSE );
                }
                obj->parent_handle = handle;
            }
        }
        return( Notify( (OBJPTR)obj->o_item, *id, obj->parent ) );
    }

    return( FALSE );
}

BOOL WdeDialogResize( WdeDialogObject *obj, RECT *new_pos, bool *flag )
{
    LIST    *olist;
    OBJPTR  child;
    POINT   offset;
    RECT    old_location;
    RECT    new_location;
    POINT   origin;

    if( *flag && obj->mode != WdeSelect ) {
        return( FALSE );
    }

    if( new_pos->right - new_pos->left < obj->nc_size.left + obj->nc_size.right ) {
        new_pos->right = new_pos->left + obj->nc_size.left + obj->nc_size.right;
    }

    if( new_pos->bottom - new_pos->top < obj->nc_size.top + obj->nc_size.bottom ) {
        new_pos->bottom = new_pos->top + obj->nc_size.top + obj->nc_size.bottom;
    }

    Location( (OBJPTR)obj, &old_location );

    if( !Resize( obj->o_item, new_pos, *flag ) ) {
        WdeWriteTrail( "WdeDialogResize: O_ITEM RESIZE failed!" );
        return( FALSE );
    }

    if( *flag ) {
        WdeDialogModified( obj );
        WdeUpdateDialogUnits( obj, new_pos, &obj->nc_size );
        if( !WdeKludgeDialogSize( obj, TRUE, FALSE ) ) {
            WdeWriteTrail( "WdeDialogResize: Couldn't kludge size!" );
            return( FALSE );
        }
    }

    Location( (OBJPTR)obj, &new_location );

    GetOffset( &origin );

    if( !MoveWindow( obj->window_handle, new_location.left - origin.x,
                     new_location.top - origin.y, new_location.right - new_location.left,
                     new_location.bottom - new_location.top, TRUE ) ) {
        WdeWriteTrail( "WdeDialogResize: MoveWindow failed!" );
        if( !Resize( obj->o_item, &old_location, FALSE ) ) {
            WdeWriteTrail( "WdeDialogResize: O_ITEM RESIZE undo failed!" );
        }
        return( FALSE );
    }

    /* tell all children that they have a new location but DONT actually
     * move them!!!
     */

    offset.x = new_location.left - old_location.left;
    offset.y = new_location.top - old_location.top;

    if( offset.x != 0 || offset.y != 0 ) {
        /* let's relocate the children in reverse order */
        if( obj->children != NULL ) {
            WdeListLastElt( obj->children, &olist );
        } else {
            olist = NULL;
        }

        for( ; olist != NULL; olist = ListPrev( olist ) ) {
            child = ListElement( olist );
            Notify( child, SET_LOCATION, NULL ) ;
        }
    }

    // look for a status bar and noodle its location
    for( olist = obj->children; olist != NULL; olist = ListNext( olist ) ) {
        OBJ_ID  id;
        child = ListElement( olist );
        if( Forward( child, IDENTIFY, &id, NULL ) && id == SBAR_OBJ ) {
            WdeSBNoodleSize( child, TRUE );
        }
    }

    if( *flag ) {
        WdeCheckBaseScrollbars( FALSE );
    }

    return( TRUE );
}

bool WdeUpdateDialogUnits( WdeDialogObject *obj, RECT *new, RECT *nc_size )
{
    RECT           size;
    DialogSizeInfo dsize;

    if( new == NULL ) {
        return( FALSE );
    }

    size = *new;

    size.left += nc_size->left;
    size.top += nc_size->top;
    size.right -= nc_size->right;
    size.bottom -= nc_size->bottom;

    /* save the old dialog units */
    dsize = GETHDR_SIZE( obj->dialog_info );

    if( !WdeScreenToDialog( obj, &obj->resizer, &size,
                            GETHDR_PSIZE( obj->dialog_info ) ) ) {
        /* restore the old dialog units */
        SETHDR_SIZE( obj->dialog_info, dsize );
        return( FALSE );
    }

    return( TRUE );
}

BOOL WdeDialogMove( WdeDialogObject *obj, POINT *off, bool *forms_called )
{
    LIST    *olist;
    OBJPTR  child;
    RECT    new_location;
    RECT    old_location;
    POINT   origin;
    POINT   actual_offset;
    POINT   offset;
    bool    ok;
    OBJPTR  clone;

    if( *forms_called && (clone = WdeCloneObject( obj->object_handle, off )) != NULL ) {
        offset.x = 0;
        offset.y = 0;
    } else {
        offset.x = off->x;
        offset.y = off->y;
    }

    Location( (OBJPTR)obj, &old_location );

    /* update the location of the object */
    ok = Move( obj->o_item, &offset, *forms_called );

    if( ok && *forms_called ) {
        WdeOffsetDialogUnits( obj, NULL, &obj->nc_size );
        if( !WdeKludgeDialogSize( obj, TRUE, TRUE ) ) {
            WdeWriteTrail( "WdeDialogMove: Couldn't kludge size!" );
            ok = FALSE;
        }
    }

    if( ok ) {
        Location( (OBJPTR)obj, &new_location );
        GetOffset( &origin );
        OffsetRect( &new_location, -origin.x, -origin.y );
        SetWindowPos( obj->window_handle, (HWND)NULL,
                      new_location.left, new_location.top, 0, 0,
                      SWP_NOZORDER | SWP_NOSIZE );
#if 0
        if( !MoveWindow( obj->window_handle,
                         new_location.left, new_location.top,
                         new_location.right - new_location.left,
                         new_location.bottom - new_location.top,
                         TRUE ) ) {
            WdeWriteTrail( "WdeDialogMove: MoveWindow failed!" );
            ok = FALSE;
        }
#endif
    }

    /* tell all children that they have a new location but DONT actually
     * move them!!!
     */

    if( ok ) {
        actual_offset.x = new_location.left - old_location.left;
        actual_offset.y = new_location.top - old_location.top;

        if( actual_offset.x != 0 || actual_offset.y != 0 ) {
            /* let's relocate the children in reverse order */
            if( obj->children != NULL ) {
                WdeListLastElt( obj->children, &olist );
            } else {
                olist = NULL;
            }

            for( ; olist != NULL; olist = ListPrev( olist ) ) {
                child = ListElement( olist );
                Notify( child, SET_LOCATION, NULL ) ;
            }
        }
    }

    if( !ok ) {
        if( *forms_called ) {
            RemoveObject( obj->parent, obj->object_handle );
        }
        if( !Resize( obj->o_item, &old_location, FALSE ) ) {
            WdeWriteTrail( "WdeDialogMove: O_ITEM RESIZE undo failed!" );
        }
    } else if( ok && *forms_called )  {
        WdeDialogModified( obj );
        WdeCheckBaseScrollbars( FALSE );
    }

    return( ok );
}

bool WdeOffsetDialogUnits( WdeDialogObject *obj, RECT *new, RECT *nc_size )
{
    RECT           new_pos;
    RECT           nc;
    DialogSizeInfo dsize;

    if( !nc_size ) {
        nc = obj->nc_size;
    } else {
        nc = *nc_size;
    }

    if( new == NULL ) {
        Location( (OBJPTR)obj, &new_pos );
    } else {
        new_pos = *new;
    }

    new_pos.left += nc.left;
    new_pos.top += nc.top;

    if( WdeScreenToDialog( obj, &obj->resizer, &new_pos, &dsize ) ) {
        SETHDR_SIZEX( obj->dialog_info, dsize.x );
        SETHDR_SIZEY( obj->dialog_info, dsize.y );
        return( TRUE );
    }

    return( FALSE );
}

BOOL WdeDialogGetObjectInfo( WdeDialogObject *obj,
                             WdeDialogBoxHeader **info, WResID **name )
{
    if( info != NULL ) {
        *info = obj->dialog_info;
    }

    if( name != NULL ) {
        *name = obj->name;
    }

    return( TRUE );
}

BOOL WdeDialogSetObjectInfo( WdeDialogObject *obj, WdeDialogBoxHeader *info, void *p2 )
{
    void *vp;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( info == NULL ) {
        return( FALSE );
    }

    vp = WdeCopyDialogBoxHeader( info );

    if( vp == NULL ) {
        WdeWriteTrail( "WdeDialogSetObjectInfo: copy failed!" );
        return( FALSE );
    }

    if( obj->dialog_info != NULL ) {
        WdeFreeDialogBoxHeader( &obj->dialog_info );
    }

    obj->dialog_info = vp;

    return( TRUE );
}

BOOL WdeDialogGetObjectHelpInfo( WdeDialogObject *obj,
                                 WdeDialogBoxHeader **info, char **helpsymbol )
{
    _wde_touch( info );

    if( helpsymbol != NULL ) {
        *helpsymbol = obj->helpsymbol;
    }

    return( TRUE );
}

BOOL WdeDialogPasteObject( WdeDialogObject *obj, OBJPTR parent, POINT *pnt )
{
    LIST            *c;
    LIST            *oc;
    bool            ok;
    WdeOrderedEntry *oentry;

    c = obj->children;
    oc = obj->ochildren;
    obj->children = NULL;
    obj->ochildren = NULL;

    ok = WdeSetObjectInfo( obj, &obj->res_info, &obj->dlg_item, obj->name );
    if( !ok ) {
        WdeWriteTrail( "WdeDialogPasteObject: WdeSetObjectInfo failed!" );
    }

    if( ok ) {
        ok = PasteObject( obj->o_item, parent, *pnt );
        if( !ok ) {
            WdeWriteTrail( "WdeDialogPasteObject: Paste of OITEM failed!" );
        }
    }

    if( ok ) {
        WdeOffsetDialogUnits( obj, NULL, &obj->nc_size );
        ok = WdeDialogCreateWindow( obj, NULL, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeDialogPasteObject: create window falied!" );
        }
    }

    if( ok ) {
        obj->children = c;
        obj->ochildren = oc;
        for( ; oc != NULL; oc = ListNext( oc ) ) {
            oentry = (WdeOrderedEntry *)ListElement( oc );
            ok = Forward( oentry->obj, PASTE, obj->object_handle, NULL );
            if( !ok ) {
                WdeWriteTrail( "WdeDialogPasteObject: A PASTE failed!" );
            }
        }
    }

    if( ok ) {
        ok = WdeDialogFirstChild( obj, NULL, NULL );
        if( !ok ) {
            WdeWriteTrail( "WdeDialogPasteObject: FIRST_CHILD falied!" );
        }
    }

    if( ok ) {
        if( !obj->name->IsName && obj->symbol != NULL ) {
            WdeAddSymbolToObjectHashTable( obj->res_info, obj->symbol, obj->name->ID.Num );
        }
    } else {
        obj->children = c;
        obj->ochildren = oc;
    }

    return( ok );
}

BOOL WdeDialogCopyObject( WdeDialogObject *obj, WdeDialogObject **new,
                          WdeDialogObject *handle )
{
    LIST            *olist;
    OBJPTR          new_child;
    WdeOrderedEntry *oentry;

    if( new == NULL ) {
        WdeWriteTrail( "WdeDialogCopyObject: Invalid new object!" );
        return( FALSE );
    }

    *new = (WdeDialogObject *)WRMemAlloc( sizeof( WdeDialogObject ) );

    if( *new == NULL ) {
        WdeWriteTrail( "WdeDialogCopyObject: Object malloc failed" );
        return( FALSE );
    }
    memset( *new, 0, sizeof( WdeDialogObject ) );

    (*new)->dispatcher = obj->dispatcher;
    (*new)->object_id = obj->object_id;
    (*new)->font = obj->font;
    (*new)->mem_flags = obj->mem_flags;
    (*new)->resizer = obj->resizer;
    (*new)->nc_size = obj->nc_size;
    (*new)->dialog_info = WdeCopyDialogBoxHeader( obj->dialog_info );
    (*new)->symbol = WdeStrDup( obj->symbol );
    (*new)->name = WdeCopyWResID( obj->name );
    (*new)->helpsymbol = WdeStrDup( obj->helpsymbol );
//  (*new)->helpname = WdeCopyWResHelpID( obj->helpname );
    (*new)->mode = WdeSelect;

    if( (*new)->dialog_info == NULL || (*new)->name == NULL ) {
        WdeWriteTrail( "WdeDialogCopyObject: NULL field detected!" );
        WdeFreeDialogObject( *new );
        return( FALSE );
    }

    if( handle == NULL ) {
        (*new)->object_handle = *new;
    } else {
        (*new)->object_handle = handle;
    }

    if( !CopyObject( obj->o_item, &(*new)->o_item, (*new)->object_handle ) ) {
        WdeWriteTrail( "WdeDialogCopyObject: OITEM not created!" );
        WdeFreeDialogObject( *new );
        return( FALSE );
    }

    WdeCleanOrderedList( &obj->ochildren );

    /* copy all children */
    for( olist = obj->ochildren; olist != NULL; olist = ListNext( olist ) ) {
        oentry = (WdeOrderedEntry *)ListElement( olist );
        if( CopyObject( oentry->obj, &new_child, NULL ) ) {
            if( !WdeDialogAddSubObject( *new, new_child, NULL ) ) {
                WdeWriteTrail( "WdeDialogCopyObject: AddSubObject failed!" );
            }
        } else {
            WdeWriteTrail( "WdeDialogCopyObject: CopyObject failed!" );
        }
    }

    return( TRUE );
}

BOOL WdeDialogCutObject( WdeDialogObject *obj, WdeDialogObject **new, void *p2 )
{
    NOTE_ID   note_id;
    bool      quick;
    bool      destroy_children;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( new == NULL ) {
        return( FALSE );
    }

    *new = NULL;

    WdeRemoveObject( obj->res_info, obj );

    obj->res_info = NULL;
    obj->dlg_item = NULL;

    if( !RemoveObject( obj->parent, obj->object_handle ) ) {
        WdeWriteTrail( "WdeDialogCutObject: Remove from parent failed!" );
        return( FALSE );
    }

    note_id = NEW_PARENT;
    if( !WdeDialogNotify( obj, &note_id, NULL ) ) {
        WdeWriteTrail( "WdeDialogCutObject: New parent notify failed!" );
        if( !AddObject( obj->parent, obj->object_handle ) ) {
            WdeWriteTrail( "WdeDialogCutObject: Add to parent failed!" );
        }
        return( FALSE );
    }

    quick = TRUE;
    destroy_children = TRUE;

    if( !WdeDialogDestroyWindow( obj, &quick, &destroy_children ) ) {
        WdeWriteTrail( "WdeDialogDestroy: WdeDialogDestroyWindow failed!" );
    }

    *new = obj->object_handle;

    WdeCheckBaseScrollbars( FALSE );

    return( TRUE );
}

BOOL WdeDialogGetOrderMode( WdeDialogObject *obj, WdeOrderMode *mode, void *p2 )
{
    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    *mode = obj->mode;

    return( TRUE );
}

BOOL WdeDialogSetOrderMode( WdeDialogObject *obj, WdeOrderMode *mode, void *p2 )
{
    WdeSetOrderLists    *sol;
    WdeOrderedEntry     *oentry;
    LIST                *olist;
    int                 pos;
    RECT                rect;
    POINT               origin;

    /* touch unused vars to get rid of warning */
    _wde_touch( p2 );

    if( obj->ochildren == NULL ) {
        return( FALSE );
    }

    if( obj->mode == WdeSelect ) {
        sol = (WdeSetOrderLists *)WRMemAlloc( sizeof( WdeSetOrderLists ) );
        if( sol == NULL ) {
            return( FALSE );
        }
        WdeCleanOrderedList( &obj->ochildren );
        pos = 0;
        for( olist = obj->ochildren; olist != NULL; olist = ListNext( olist ) ) {
            if( (oentry = (WdeOrderedEntry *)ListElement( olist )) != NULL ) {
                oentry->pos = ++pos;
            }
        }
        sol->oldlist = WdeCopyOrderedList( obj->ochildren );
        if( sol->oldlist == NULL ) {
            return( FALSE );
        }
        sol->newlist = NULL;
        WdeSetBaseObject( IDM_SELECT_MODE );
        HideSelectBoxes();
        MakeObjectCurrent( obj );
    }

    for( olist = obj->ochildren; olist != NULL; olist = ListNext( olist ) ) {
        if( (oentry = (WdeOrderedEntry *)ListElement( olist )) != NULL ) {
            if( !Forward( oentry->obj, SET_ORDER_MODE, mode, &sol ) ) {
                WdeWriteTrail( "WdeDialogSetOrderMode: failed to set child mode!" );
            }
        }
    }

    if( obj->mode == *mode ) {
        WdeCleanOrderedList( &sol->oldlist );
        WdeCleanOrderedList( &sol->newlist );
        WdeListConcat( &sol->newlist, sol->oldlist, 0 );
        ListFree( sol->oldlist );
        WdeFreeOrderedList( obj->ochildren );
        obj->ochildren = sol->newlist;
        obj->mode = WdeSelect;
        WRMemFree( sol );
        GetOffset( &origin );
        Location( (OBJPTR)obj, &rect );
        OffsetRect( &rect, -origin.x, -origin.y );
        ShowSelectBoxes();
        RedrawWindow( obj->res_info->edit_win, &rect, (HRGN)NULL,
                      RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME );
    } else {
        obj->mode = *mode;
        WdeReorderTags( sol, TRUE );
    }

    WdeSetDialogObjectMenu( WdeIsDialogRestorable( obj ),
                            obj->res_info->hash_table != NULL, obj->mode );

    return( TRUE );
}

bool WdeBuildDialogTemplate ( WdeDialogBoxHeader *dialog_header, HGLOBAL *hglobal_mem )
{
    char        *MenuName;
    char        *ClassName;
    char        *fontname;
    short       pointsize;
    bool        ok;
    uint_32     style;

    MenuName = NULL;
    ClassName = NULL;

    ok = (dialog_header != NULL && hglobal_mem != NULL);

    if( ok ) {
        style = GETHDR_STYLE( dialog_header );
        style &= ~(WS_VISIBLE | WS_POPUP | DS_ABSALIGN | DS_SYSMODAL);
        style |= WS_CLIPSIBLINGS | WS_CHILD;
        //MenuName = WdeResNameOrOrdinalToStr( GETHDR_MENUNAME( dialog_header ), 10 );
        //if( MenuName != NULL && *MenuName == '\0' ) {
        //    WRMemFree( MenuName );
        //    MenuName = NULL;
        //}
        ClassName = WdeResNameOrOrdinalToStr( GETHDR_CLASSNAME( dialog_header ), 10 );
        if( ClassName != NULL && *ClassName == '\0' ) {
            WRMemFree( ClassName );
            ClassName = NULL;
        }
        ok = (ClassName == NULL || (ClassName != NULL && WdeIsClassDefined( ClassName )));
        if( !ok ) {
            //WdeDisplayErrorMsg( WDE_UNDEFINEDCLASS );
            WdeSetStatusByID( -1, WDE_UNDEFINEDCLASS );
            WRMemFree( ClassName );
            ClassName = NULL;
            ok = TRUE;
        }
    }

    fontname = GETHDR_FONTNAME( dialog_header );
    pointsize = GETHDR_POINTSIZE( dialog_header );

    // hokey kanji stuff
#ifndef __NT__
    if( (style & DS_SETFONT ) == 0 && GetSystemMetrics( SM_DBCSENABLED ) ) {
        style |= DS_SETFONT;
        fontname = "System";
        pointsize = 14;
    }
#endif

    /* Allocate global block of memory for Dialog template */
    if( ok ) {
        *hglobal_mem = DialogTemplate( style, GETHDR_SIZEX( dialog_header ),
            GETHDR_SIZEY( dialog_header ), GETHDR_SIZEW( dialog_header ),
            GETHDR_SIZEH( dialog_header ), MenuName, ClassName,
            GETHDR_CAPTION( dialog_header ), pointsize, fontname );

        ok = (*hglobal_mem != NULL);
        if( !ok ) {
            WdeWriteTrail( "Could not Alloc dialog template!" );
        }
    }

    if( ok ) {
        DoneAddingControls( *hglobal_mem );
    }

    if( MenuName != NULL ) {
        WRMemFree( MenuName );
    }

    if( ClassName != NULL ) {
        WRMemFree( ClassName );
    }

    return( ok );
}

WINEXPORT BOOL CALLBACK WdeDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LPARAM  new_lparam;
    int     msg_processed;

    msg_processed = FALSE;

    if( WdeProcessMouse( hWnd, message, wParam, lParam ) ) {
        return( TRUE );
    }

    switch( message ) {
#if 0
    case WM_SETTEXT:
    case WM_NCPAINT:
    case WM_NCACTIVATE:
        SetWindowLong( hWnd, DWL_MSGRESULT,
                       WdeCtl3dDlgFramePaint( hWnd, message, wParam, lParam ) );
        return( TRUE );
#endif
    case WM_DLGBORDER:
        if( lParam ) {
            *((int *)lParam) = CTL3D_BORDER;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOBORDER;
            }
        }
        msg_processed = TRUE;
        break;
    case WM_DLGSUBCLASS:
        if( lParam ) {
            *((int *)lParam) = CTL3D_SUBCLASS;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOSUBCLASS;
            }
        }
        msg_processed = TRUE;
        break;
    case WM_INITDIALOG:
        if( WdeGetOption( WdeOptUse3DEffects ) ) {
            WdeCtl3dSubclassDlg( hWnd, CTL3D_ALL );
        }
        break;

    /* force mouse activate to activate the dialog on mouse clicks
     * in all parts of the dialog window
     */
    case WM_MOUSEACTIVATE:
        if( LOWORD( lParam ) != (WORD)HTCLIENT && LOWORD( lParam ) != (WORD)HTERROR &&
            LOWORD( lParam ) != (WORD)HTTRANSPARENT &&
            LOWORD( lParam ) != (WORD)HTNOWHERE ) {
            new_lparam = MAKELONG( HTCLIENT, HIWORD( lParam ) );
            SendMessage( hWnd, message, wParam, new_lparam );
        }
        msg_processed = TRUE;
        break;

    case WM_SETFONT:
        WdeLastFont = (HFONT)wParam;
        break;
    }

    return( msg_processed );
}

WINEXPORT BOOL CALLBACK WdeDialogDefineProc( HWND hDlg, WORD message, WPARAM wParam, LPARAM lParam )
{
    static WdeDefineObjectInfo *o_info;
    static uint_8              init_done;
    bool                       ret;
    int                        font_index;

    ret = FALSE;

    if( init_done && o_info != NULL ) {
        ret = WdeProcessSymbolCombo( hDlg, message, wParam, lParam,
                                     o_info->res_info->hash_table,
                                     o_info->info.d.id,
                                     o_info->info.d.use_id );

        if( !ret ) {
            ret = WdeProcessHelpSymbolCombo( hDlg, message, wParam, lParam,
                                             o_info->res_info->hash_table,
                                             o_info->info.d.header->HelpId,
                                             TRUE );
        }

        if( !ret && o_info->hook_func != NULL ) {
            ret = (*o_info->hook_func)( hDlg, message, wParam, lParam, o_info->mask );
        }

        if( ret ) {
            return( ret );
        }
    }

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WdeCtl3dColorChange();
        break;

    case WM_INITDIALOG:
        init_done = FALSE;
        o_info = (WdeDefineObjectInfo *)lParam;
        WdeDialogSetDefineDialogInfo( o_info, hDlg );
        init_done = TRUE;
        ret = TRUE;
        break;

    case WM_COMMAND:
        if( !init_done ) {
            break;
        }
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WdeHelpRoutine();
            break;

        case IDOK:
            init_done = FALSE;
            EndDialog( hDlg, TRUE );
            WdeDialogGetDefineDialogInfo( o_info, hDlg );
            ret = TRUE;
            break;

        case IDCANCEL:
            init_done = FALSE;
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;

        case IDB_FONTNAME:
            switch( GET_WM_COMMAND_CMD( wParam, lParam ) ) {
            case CBN_CLOSEUP:
                /* get the currently selected item */
                font_index = SendDlgItemMessage( hDlg, IDB_FONTNAME, CB_GETCURSEL, 0, 0 );

                if( font_index == CB_ERR ) {
                    WdeWriteTrail( "Could not get font index" );
                    break;
                }

                WdeDialogSetDialogPntInfo( hDlg, font_index );

                SendDlgItemMessage( hDlg, IDB_POINTSIZE, CB_SETCURSEL, 0, 0 );

                break;
            }
            break;
        }
    }

    return( ret );
}

void WdeDialogSetDefineDialogInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    WdeDialogObject *obj;
    DialogStyle     mask;

    obj = o_info->obj;
    mask = o_info->mask;

    /* set the window styles */
    if( GETHDR_STYLE( obj->dialog_info ) & 0xffff0000 ) {
        WdeSetWinStyles( hDlg, GETHDR_STYLE( obj->dialog_info ), mask );
        if( (GETHDR_STYLE( obj->dialog_info ) & WS_CAPTION) == WS_CAPTION ) {
            CheckDlgButton( hDlg, IDB_WS_CAPTION, 1 );
        }
    } else {
        CheckDlgButton( hDlg, IDB_WS_OVERLAPPED, 1 );
    }

    /* set the dialog styles */
    WdeDialogSetDialogStyles( hDlg, GETHDR_STYLE( obj->dialog_info ) );

#if __NT__XX
    /* set the dialog extended styles */
    WdeDialogSetDialogEXStyles( hDlg, GETHDR_EXSTYLE( obj->dialog_info ) );
#endif

    /* set the dialog memory and load flags */
    WdeDialogSetDialogMemFlags( hDlg, obj->mem_flags );

    /* set the dialog font and point size combo boxes */
    WdeDialogSetDialogFontInfo( hDlg, obj );

    /* set the caption */
    if( GETHDR_CAPTION( obj->dialog_info ) ) {
        WdeSetEditWithStr( GETHDR_CAPTION( obj->dialog_info ), hDlg, IDB_TEXT );
    }

    /* Help ID stuff is handled in here too - JPK */
    WdeSetDefineObjectSymbolInfo( o_info, hDlg );

    /* set the menu name */
    WdeSetEditWithResNameOr( GETHDR_MENUNAME( obj->dialog_info ), hDlg, IDB_MENU );

    /* set the menu name */
    WdeSetEditWithResNameOr( GETHDR_CLASSNAME( obj->dialog_info ), hDlg, IDB_CLASS );
}

void WdeDialogGetDefineDialogInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    void                *vp;
    bool                mod;
    WdeDialogObject     *obj;
    DialogStyle         mask;
    DialogStyle         style;
    ResNameOrOrdinal    *rname;
#if __NT__XX
    uint_32             ExStyle;
#endif

    obj = o_info->obj;
    mask = o_info->mask;

    style = GETHDR_STYLE( obj->dialog_info );

    /* get the window styles */
    WdeGetWinStyles( hDlg, &style, mask );

    /* get the dialog styles */
    WdeDialogGetDialogStyles( hDlg, &style );

#if __NT__XX
    /* set the dialog extended styles */
    WdeDialogGetDialogEXStyles( hDlg, &ExStyle );
    SETHDR_EXSTYLE( obj->dialog_info, ExStyle );
#endif

    /* get the dialog memory and load flags */
    WdeDialogGetDialogMemFlags( hDlg, &obj->mem_flags );

    /* get the dialog caption */
    vp = WdeGetStrFromEdit( hDlg, IDB_TEXT, &mod );
    if( vp != NULL ) {
        if( mod ) {
            if( GETHDR_CAPTION( obj->dialog_info ) ) {
                WRMemFree( GETHDR_CAPTION( obj->dialog_info ) );
            }
            if( WdeIsStrSpace( vp ) ) {
                SETHDR_CAPTION( obj->dialog_info, NULL );
                WRMemFree( vp );
            } else {
                SETHDR_CAPTION( obj->dialog_info, vp );
            }
        } else {
            WRMemFree( vp );
        }
    }

    WdeGetDefineObjectSymbolInfo( o_info, hDlg );
    WdeGetDefineObjectHelpSymbolInfo( o_info, hDlg );

    if( GETHDR_FONTNAME( obj->dialog_info ) ) {
        WRMemFree( GETHDR_FONTNAME( obj->dialog_info ) );
        SETHDR_FONTNAME( obj->dialog_info, NULL );
    }

    vp = WdeGetStrFromCombo( hDlg, IDB_FONTNAME );
    if( vp != NULL ) {
        if( WdeIsStrSpace( vp ) ) {
            WRMemFree( vp );
        } else {
            SETHDR_FONTNAME( obj->dialog_info, vp );
        }
    }

    if( GETHDR_FONTNAME( obj->dialog_info ) ) {
        SETHDR_POINTSIZE( obj->dialog_info,
                          (uint_16)WdeGetUINT32FromCombo( hDlg, IDB_POINTSIZE ) );
        style |= DS_SETFONT;
    } else {
        SETHDR_POINTSIZE( obj->dialog_info, 0 );
        style &= ~DS_SETFONT;
    }

    /* get the menu name */
    rname = WdeGetResNameOrFromEdit( hDlg, IDB_MENU, &mod );
    if( mod && rname != NULL ) {
        if( GETHDR_MENUNAME( obj->dialog_info ) ) {
            WRMemFree( GETHDR_MENUNAME( obj->dialog_info ) );
        }
        SETHDR_MENUNAME( obj->dialog_info, rname );
        if( rname->ord.fFlag != 0xff && WdeIsStrSpace( rname->name ) ) {
            WRMemFree( rname );
            SETHDR_MENUNAME( obj->dialog_info, NULL );
        }
    }

    /* get the class name */
    rname = WdeGetResNameOrFromEdit( hDlg, IDB_CLASS, &mod );
    if( mod && rname != NULL ) {
        if( GETHDR_CLASSNAME( obj->dialog_info ) ) {
            WRMemFree( GETHDR_CLASSNAME( obj->dialog_info ) );
        }
        SETHDR_CLASSNAME( obj->dialog_info, rname );
        if( rname->ord.fFlag != 0xff && WdeIsStrSpace( rname->name ) ) {
            WRMemFree( rname );
            SETHDR_CLASSNAME( obj->dialog_info, NULL );
        }
    }

    SETHDR_STYLE( obj->dialog_info, style );
}

void WdeDialogGetDialogStyles( HWND hDlg, DialogStyle *style )
{
    /* zero the styles we are interested in */
    *style &= ~(DS_ABSALIGN | DS_SYSMODAL | DS_LOCALEDIT | DS_MODALFRAME | DS_NOIDLEMSG);

    /* set the styles if they are checked */
    if( IsDlgButtonChecked( hDlg, IDB_DS_ABSALIGN ) ) {
        *style |= DS_ABSALIGN;
    }

    if( IsDlgButtonChecked( hDlg, IDB_DS_SYSMODAL ) ) {
        *style |= DS_SYSMODAL;
    }

    if( IsDlgButtonChecked( hDlg, IDB_DS_LOCALEDIT ) ) {
        *style |= DS_LOCALEDIT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_DS_MODALFRAME ) ) {
        *style |= DS_MODALFRAME;
    }

    if( IsDlgButtonChecked( hDlg, IDB_DS_NOIDLEMSG ) ) {
        *style |= DS_NOIDLEMSG;
    }


#if __NT__XX
    *style &= ~(DS_3DLOOK | DS_CENTER | DS_CENTERMOUSE | DS_CONTEXTHELP | DS_CONTROL |
                DS_FIXEDSYS | DS_NOFAILCREATE | DS_SETFOREGROUND);

    if( IsDlgButtonChecked( hDlg, IDB_DS_3DLOOK ) ) {
        *style |= DS_3DLOOK;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_CENTER ) ) {
        *style |= DS_CENTER;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_CENTERMOUSE ) ) {
        *style |= DS_CENTERMOUSE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_CONTEXTHELP ) ) {
        *style |= DS_CONTEXTHELP;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_CONTROL ) ) {
        *style |= DS_CONTROL;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_FIXEDSYS ) ) {
        *style |= DS_FIXEDSYS;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_NOFAILCREATE ) ) {
        *style |= DS_NOFAILCREATE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_DS_SETFOREGROUND ) ) {
        *style |= DS_SETFOREGROUND;
    }
#endif
}

void WdeDialogSetDialogStyles( HWND hDlg, DialogStyle style )
{
    if( style & DS_ABSALIGN ) {
        CheckDlgButton( hDlg, IDB_DS_ABSALIGN, 1 );
    }

    if( style & DS_SYSMODAL ) {
        CheckDlgButton( hDlg, IDB_DS_SYSMODAL, 1 );
    }

    if( style & DS_LOCALEDIT ) {
        CheckDlgButton( hDlg, IDB_DS_LOCALEDIT, 1 );
    }

    if( style & DS_MODALFRAME ) {
        CheckDlgButton( hDlg, IDB_DS_MODALFRAME, 1 );
    }

    if( style & DS_NOIDLEMSG ) {
        CheckDlgButton( hDlg, IDB_DS_NOIDLEMSG, 1 );
    }

#if __NT__XX
    EnableWindow( GetDlgItem( hDlg, IDB_DS_3DLOOK ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CENTER ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CENTERMOUSE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CONTEXTHELP ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CONTROL ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_FIXEDSYS ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_NOFAILCREATE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_SETFOREGROUND ), TRUE );

    if( style & DS_3DLOOK ) {
        CheckDlgButton( hDlg, IDB_DS_3DLOOK, 1 );
    }
    if( style & DS_CENTER ) {
        CheckDlgButton( hDlg, IDB_DS_CENTER, 1 );
    }
    if( style & DS_CENTERMOUSE ) {
        CheckDlgButton( hDlg, IDB_DS_CENTERMOUSE, 1 );
    }
    if( style & DS_CONTEXTHELP ) {
        CheckDlgButton( hDlg, IDB_DS_CONTEXTHELP, 1 );
    }
    if( style & DS_CONTROL ) {
        CheckDlgButton( hDlg, IDB_DS_CONTROL, 1 );
    }
    if( style & DS_FIXEDSYS ) {
        CheckDlgButton( hDlg, IDB_DS_FIXEDSYS, 1 );
    }
    if( style & DS_NOFAILCREATE ) {
        CheckDlgButton( hDlg, IDB_DS_NOFAILCREATE, 1 );
    }
    if( style & DS_SETFOREGROUND ) {
        CheckDlgButton( hDlg, IDB_DS_SETFOREGROUND, 1 );
    }
#else
    EnableWindow( GetDlgItem( hDlg, IDB_DS_3DLOOK ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CENTER ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CENTERMOUSE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CONTEXTHELP ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_CONTROL ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_FIXEDSYS ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_NOFAILCREATE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_DS_SETFOREGROUND ), FALSE );
#endif
}

#if __NT__XX
void WdeDialogSetDialogEXStyles( HWND hDlg, uint_32 ExStyle )
{
    if( ExStyle & WS_EX_CLIENTEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_CLIENTEDGE, 1 );
    }
    if( ExStyle & WS_EX_WINDOWEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_WINDOWEDGE, 1 );
    }
    if( ExStyle & WS_EX_STATICEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_STATICEDGE, 1 );
    }
    if( (ExStyle & WS_EX_OVERLAPPEDWINDOW) == WS_EX_OVERLAPPEDWINDOW ) {
        CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, 1 );
    }
    if( (ExStyle & WS_EX_PALETTEWINDOW) == WS_EX_PALETTEWINDOW ) {
        CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, 1 );
    }
    if( ExStyle & WS_EX_TOOLWINDOW ) {
        CheckDlgButton( hDlg, IDB_WS_EX_TOOLWINDOW, 1 );
    }
    if( ExStyle & WS_EX_MDICHILD ) {
        CheckDlgButton( hDlg, IDB_WS_EX_MDICHILD, 1 );
    }
    if( ExStyle & WS_EX_TOPMOST ) {
        CheckDlgButton( hDlg, IDB_WS_EX_TOPMOST, 1 );
    }
    if( ExStyle & WS_EX_ACCEPTFILES ) {
        CheckDlgButton( hDlg, IDB_WS_EX_ACCEPTFILES, 1 );
    }
    if( ExStyle & WS_EX_CONTEXTHELP ) {
        CheckDlgButton( hDlg, IDB_WS_EX_CONTEXTHELP, 1 );
    }
    if( ExStyle & WS_EX_TRANSPARENT ) {
        CheckDlgButton( hDlg, IDB_WS_EX_TRANSPARENT, 1 );
    }
    if( ExStyle & WS_EX_NOPARENTNOTIFY ) {
        CheckDlgButton( hDlg, IDB_WS_EX_NOPARENTNOTIFY, 1 );
    }

    if( ExStyle & WS_EX_RIGHT ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RIGHT, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LEFT, 1 );
    }

    if( ExStyle & WS_EX_LEFTSCROLLBAR ) {
        CheckDlgButton( hDlg, IDB_WS_EX_LEFTSCROLLBAR, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_RIGHTSCROLLBAR, 1 );
    }

    if( ExStyle & WS_EX_RTLREADING ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RTLREADING, 1 );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LTRREADING, 1 );
    }
}

void WdeDialogGetDialogEXStyles( HWND hDlg, uint_32 *ExStyle )
{
    /* zero the styles we are interested in */
    *ExStyle = 0;

    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CLIENTEDGE ) ) {
        *ExStyle |= WS_EX_CLIENTEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_WINDOWEDGE ) ) {
        *ExStyle |= WS_EX_WINDOWEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_STATICEDGE ) ) {
        *ExStyle |= WS_EX_STATICEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW ) ) {
        *ExStyle |= WS_EX_OVERLAPPEDWINDOW;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_PALETTEWINDOW ) ) {
        *ExStyle |= WS_EX_PALETTEWINDOW;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_TOOLWINDOW ) ) {
        *ExStyle |= WS_EX_TOOLWINDOW;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_MDICHILD ) ) {
        *ExStyle |= WS_EX_MDICHILD;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_TOPMOST ) ) {
        *ExStyle |= WS_EX_TOPMOST;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_ACCEPTFILES ) ) {
        *ExStyle |= WS_EX_ACCEPTFILES;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CONTEXTHELP ) ) {
        *ExStyle |= WS_EX_CONTEXTHELP;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_TRANSPARENT ) ) {
        *ExStyle |= WS_EX_TRANSPARENT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_NOPARENTNOTIFY ) ) {
        *ExStyle |= WS_EX_NOPARENTNOTIFY;
    }

    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RIGHT ) ) {
        *ExStyle |= WS_EX_RIGHT;
    }

    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_LEFTSCROLLBAR ) ) {
        *ExStyle |= WS_EX_LEFTSCROLLBAR;
    }

    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RTLREADING ) ) {
        *ExStyle |= WS_EX_RTLREADING;
    }
}

#endif

void WdeDialogGetDialogMemFlags( HWND hDlg, uint_16 *flags )
{
    *flags = 0;

    if( IsDlgButtonChecked( hDlg, IDB_MEM_MOVEABLE ) ) {
        *flags |= MEMFLAG_MOVEABLE;
    }

    if( IsDlgButtonChecked( hDlg, IDB_MEM_DISCARDABLE ) ) {
        *flags |= MEMFLAG_DISCARDABLE;
    }

    if( IsDlgButtonChecked( hDlg, IDB_MEM_PURE ) ) {
        *flags |= MEMFLAG_PURE;
    }

    if( IsDlgButtonChecked( hDlg, IDB_LOAD_PRELOAD ) ) {
        *flags |= MEMFLAG_PRELOAD;
    }
}

void WdeDialogSetDialogMemFlags( HWND hDlg, uint_16 flags )
{
    if( flags & MEMFLAG_MOVEABLE ) {
        CheckDlgButton( hDlg, IDB_MEM_MOVEABLE, 1 );
    }

    if( flags & MEMFLAG_DISCARDABLE ) {
        CheckDlgButton( hDlg, IDB_MEM_DISCARDABLE, 1 );
    }

    if( flags & MEMFLAG_PURE ) {
        CheckDlgButton( hDlg, IDB_MEM_PURE, 1 );
    }

    if( flags & MEMFLAG_PRELOAD ) {
        CheckDlgButton( hDlg, IDB_LOAD_PRELOAD, 1 );
    }
}

void WdeDialogSetDialogPntInfo( HWND hDlg, int index )
{
    LIST            *olist;
    WdeFontNames    *font_name;
    WdeFontData     *font_data;
    char            temp[15];
    int             i;

    if( index != -1 ) {
        olist = WdeGetFontList();
    } else {
        olist = NULL;
    }

    for( i = 0; i < index && olist != NULL; i++ ) {
        olist = ListNext( olist );
    }

    SendDlgItemMessage( hDlg, IDB_POINTSIZE, CB_RESETCONTENT, 0, 0 );

    if( olist == NULL ) {
        return;
    }

    font_name = ListElement( olist );

    if( font_name->fonttype & TRUETYPE_FONTTYPE ) {
        for( i = WDE_TT_FONT_MIN; i <= WDE_TT_FONT_MAX; i += 2 ) {
            ultoa( i, temp, 10 );
            SendDlgItemMessage( hDlg, IDB_POINTSIZE, CB_ADDSTRING, 0,
                                (LPARAM)(LPSTR)temp );
        }
    } else {
        for( olist = font_name->family_list; olist != NULL; olist = ListNext( olist ) ) {
            font_data = (WdeFontData *)ListElement( olist );
            ultoa( font_data->pointsize, temp, 10 );
            SendDlgItemMessage( hDlg, IDB_POINTSIZE, CB_ADDSTRING, 0,
                                (LPARAM)(LPSTR)temp );
        }
    }
}

void WdeDialogSetDialogFontInfo( HWND hDlg, WdeDialogObject *obj )
{
    LIST            *olist;
    LIST            *font_list;
    WdeFontNames    *font_name;
    bool            found;
    int             index;

    font_list = WdeGetFontList();

    found = FALSE;
    index = -1;
    for( olist = font_list; olist != NULL; olist = ListNext( olist ) ) {
        font_name = (WdeFontNames *)ListElement( olist );
        SendDlgItemMessage( hDlg, IDB_FONTNAME, CB_ADDSTRING, 0,
                            (LPARAM)(LPSTR)font_name->name );
        /* if the dialog has font info then find the index of
         * the font in the list
         */
        if( !found && GETHDR_FONTNAME( obj->dialog_info ) &&
            (GETHDR_STYLE( obj->dialog_info ) & DS_SETFONT) ) {
            if( !strcmp( font_name->name, GETHDR_FONTNAME( obj->dialog_info ) ) ) {
                found = TRUE;
            }
            index++;
        }
    }

    if( index != -1 ) {
        WdeDialogSetDialogPntInfo( hDlg, index );
    }

    /* set the font name */
    if( GETHDR_STYLE( obj->dialog_info ) & DS_SETFONT ) {
        /* set the name of the font */
        if( GETHDR_FONTNAME( obj->dialog_info ) ) {
            WdeSetEditWithStr( GETHDR_FONTNAME( obj->dialog_info ), hDlg, IDB_FONTNAME );

            /* set the point size of the font */
            WdeSetEditWithUINT32( (uint_32)GETHDR_POINTSIZE( obj->dialog_info ),
                                  10, hDlg, IDB_POINTSIZE );
        }
    }
}
