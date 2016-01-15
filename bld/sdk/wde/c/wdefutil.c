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
#include <mbstring.h>
#include <ctype.h>
#include "wrdll.h"
#include "wdelist.h"
#include "wderes.h"
#include "wdemain.h"
#include "wdestyle.h"
#include "wdegeted.h"
#include "wdereq.h"
#include "wdeopts.h"
#include "wdeactn.h"
#include "wdedebug.h"
#include "wdeedit.h"
#include "wdesdup.h"
#include "wdestat.h"
#include "wdedefin.h"
#include "wdefordr.h"
#include "wdecurr.h"
#include "wdectl3d.h"
#include "rcstr.gh"
#include "wde_wres.h"
#include "wde_rc.h"
#include "wdecctl.h"
#include "wdefutil.h"
#include "wrstrdup.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_START_CONTROL_ID 100

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WdeControlDefineProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    Wde_Objects obj_id;
    char        *proc_name;
} WdeOBJIDToDefineProcItems;

typedef struct {
    UINT ncm;
    UINT m;
} WdeMouseMapStruct;

typedef struct {
    OBJPTR  base;
    uint_16 id;
} WdeNextIDStruct;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WdeNextIDStruct *WdeFindNextIDStruct( OBJPTR );
static char *WdeGetDefineProcFromOBJID( Wde_Objects );
static void  WdeSetDefineControlInfo( WdeDefineObjectInfo *, HWND );
static void  WdeGetDefineControlInfo( WdeDefineObjectInfo *, HWND );
static void  WdeDefineObjectLookupComboEntry( HWND, WORD, WdeHashTable * );
static void  WdeDefineObjectLookupHelpComboEntry( HWND, WORD, WdeHashTable * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST *WdeNextIDList = NULL;

static WdeMouseMapStruct WdeMouseMapper[] = {
    { WM_NCMOUSEMOVE,     WM_MOUSEMOVE     },
    { WM_NCLBUTTONDOWN,   WM_LBUTTONDOWN   },
    { WM_NCLBUTTONUP,     WM_LBUTTONUP     },
    { WM_NCRBUTTONDOWN,   WM_RBUTTONDOWN   },
    { WM_NCRBUTTONUP,     WM_RBUTTONUP     },
    { WM_NCLBUTTONDBLCLK, WM_LBUTTONDBLCLK },
    { WM_NCRBUTTONDBLCLK, WM_RBUTTONDBLCLK },
    { WM_NCMBUTTONDOWN,   WM_MBUTTONDOWN   },
    { WM_NCMBUTTONUP,     WM_MBUTTONUP     },
    { WM_NCMBUTTONDBLCLK, WM_MBUTTONDBLCLK },
    { 0,                  0                }
};

static WdeOBJIDToDefineProcItems WdeOBJIDToDefineProc[] = {
    { DIALOG_OBJ,    "WdeDefineDIALOG"      },
    { PBUTTON_OBJ,   "WdeDefinePUSH"        },
    { CBUTTON_OBJ,   "WdeDefineCHECK"       },
    { RBUTTON_OBJ,   "WdeDefineRADIO"       },
    { GBUTTON_OBJ,   "WdeDefineGROUPBOX"    },
    { FRAME_OBJ,     "WdeDefineFRAME"       },
    { TEXT_OBJ,      "WdeDefineTEXT"        },
    { ICON_OBJ,      "WdeDefineICON"        },
    { EDIT_OBJ,      "WdeDefineEDIT"        },
    { LISTBOX_OBJ,   "WdeDefineLISTBOX"     },
    { COMBOBOX_OBJ,  "WdeDefineCOMBOBOX"    },
    { HSCROLL_OBJ,   "WdeDefineHSCROLLBAR"  },
    { VSCROLL_OBJ,   "WdeDefineVSCROLLBAR"  },
    { SIZEBOX_OBJ,   "WdeDefineSIZEBOX"     },
    { SBAR_OBJ,      "WdeDefineSTATUSBAR"   },
    { LVIEW_OBJ,     "WdeDefineLISTVIEW"    },
    { TVIEW_OBJ,     "WdeDefineTREEVIEW"    },
    { TABCNTL_OBJ,   "WdeDefineTABCONTROL"  },
    { ANIMATE_OBJ,   "WdeDefineANIMATE"     },
    { UPDOWN_OBJ,    "WdeDefineUPDOWN"      },
    { TRACKBAR_OBJ,  "WdeDefineTRACKBAR"    },
    { PROGRESS_OBJ,  "WdeDefinePROGRESSBAR" },
    { HOTKEY_OBJ,    "WdeDefineHOTKEY"      },
    { HEADER_OBJ,    "WdeDefineHEADER"      },
    { BASE_OBJ,      NULL                   },
    { CONTROL_OBJ,   NULL                   },
    { LAST__OBJ,     NULL                   }
};


static void WdeSnapPoint( POINT *pt, unsigned vinc, unsigned hinc )
{
    pt->x = ((pt->x + (hinc >> 1)) / hinc) * hinc;
    pt->y = ((pt->y + (vinc >> 1)) / vinc) * vinc;
}

void WdeSnapPointToGrid( POINT *pt )
{
    unsigned hinc;
    unsigned vinc;

    hinc = WdeGetOption( WdeOptReqGridX );
    vinc = WdeGetOption( WdeOptReqGridY );

    WdeSnapPoint( pt, vinc, hinc );
}

OBJPTR WdeGetNextObject( bool up, OBJPTR obj, OBJPTR p )
{
    OBJPTR old_current;

    if( obj == NULL ) {
        obj = WdeGetCurrObject();
    }

    if( obj != NULL && obj != GetMainObject() ) {
        old_current = obj;
        if( p == NULL ) {
            GetObjectParent( obj, &p );
        }
        if( p != NULL ) {
            if( Forward( p, GET_NEXT_CHILD, &obj, &up ) && obj != old_current ) {
                return( obj );
            }
        }
    }

    return( NULL );
}

OBJPTR WdeCloneObject( OBJPTR obj, POINT *offset )
{
    SUBOBJ_REQUEST      req;
    OBJPTR              new;
    OBJPTR              parent;
    RECT                rect;
    WORD                state;
    bool                ok;

    new = NULL;

    ok = (offset->x || offset->y);

    if( ok ) {
        state = (WORD)GetKeyState( VK_CONTROL );
#ifdef __NT__
        ok = ((state & 0x8000) != 0x00);
#else
        ok = ((state & 0x80) != 0x00);
#endif
    }

    if( ok ) {
        ok = (CopyObject( obj, &new, NULL ) && new != NULL);
        if( !ok ) {
            WdeWriteTrail( "WdeCloneObject: CopyObject failed!" );
        }
    }

    if( ok ) {
        Location( obj, &rect );
        OffsetRect( &rect, offset->x, offset->y );
        req.p.ty = BY_POINT;
        req.p.pt.x = rect.left;
        req.p.pt.y = rect.top;
        parent = FindObject( &req );
        ok = (parent != NULL && PasteObject ( new, parent, req.p.pt ));
        if( !ok ) {
            WdeWriteTrail( "WdeCloneObject: PasteObject failed!" );
        }
    }

    if( !ok ) {
        if( new != NULL ) {
            Destroy( new, FALSE );
            new = NULL;
        }
    }

    return( new );
}

void WdeFreeControlIDs( void )
{
    LIST *olist;

    if( WdeNextIDList ) {
        WdeListLastElt( WdeNextIDList, &olist );
        for( ; olist != NULL; olist = ListPrev( olist ) ) {
            WRMemFree( ListElement( olist ) );
        }
        ListFree( WdeNextIDList );
    }
}

WdeNextIDStruct *WdeFindNextIDStruct( OBJPTR base )
{
    LIST            *olist;
    WdeNextIDStruct *ids;

    for( olist = WdeNextIDList; olist != NULL; olist = ListPrev( olist ) ) {
        ids = ListElement( olist );
        if( ids->base == base ) {
            return( ids );
        }
    }

    return( NULL );
}

uint_16 WdeGetNextControlID( void )
{
    WdeNextIDStruct *ids;
    OBJPTR          base;
    uint_16         id;

    base = GetMainObject();

    if( base != NULL ) {
        if( (ids = WdeFindNextIDStruct( base )) != NULL ) {
            if( ids->id == 0xffff ) {
                ids->id = WDE_START_CONTROL_ID;
            } else {
                ids->id++;
            }
        } else {
            ids = (WdeNextIDStruct *)WRMemAlloc( sizeof( WdeNextIDStruct ) );
            if( ids != NULL ) {
                ids->base = base;
                ids->id = WDE_START_CONTROL_ID;
                ListAddElt( &WdeNextIDList, ids );
            } else {
                WdeWriteTrail( "WdeGetNextControlID: ids alloc failed!" );
                return( WDE_START_CONTROL_ID );
            }
        }
        id = ids->id;
    } else {
        WdeWriteTrail( "WdeGetNextControlID: GetMainObject failed!" );
        id = WDE_START_CONTROL_ID;
    }

    return( id );
}

bool WdeIsStrSpace( char *_s )
{
    unsigned char   *s = (unsigned char *)_s;

    if( s != NULL ) {
        for( ; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) != 1 || (_mbclen( s ) == 1 && !isspace( *s )) ) {
                return( FALSE );
            }
        }
    }
    return( TRUE );
}

uint_32 WdeHammingDistance( uint_32 x1, uint_32 x2 )
{
    uint_32 d;
    uint_32 s;
    uint_8  i;

    for( i = 0, s = 1, d = 0; i < 32; i++, s <<= 1 ) {
        if( (x1 & s) != (x2 & s) ) {
            d++;
        }
    }
    return( d );
}

bool WdeIsClassDefined( char *class )
{
    WNDCLASS  wc;
    BOOL      ret;
    HINSTANCE app_inst;

    if( class != NULL && *class != '\0' ) {
        app_inst = WdeGetAppInstance();
        ret = GetClassInfo( app_inst, class, &wc );
    } else {
        ret = FALSE;
    }

    return( ret );
}

void WdeShowObjectWindow( HWND win, bool flag )
{
    uint_32 s;

    if( win != NULL ) {
        s = (uint_32)GetWindowLong( win, GWL_STYLE );
        if( flag ) {
            if( !(s & WS_VISIBLE) ) {
                s |= WS_VISIBLE;
                SetWindowLong( win, GWL_STYLE, s );
            }
        } else {
            if( s & WS_VISIBLE ) {
                s ^= WS_VISIBLE;
                SetWindowLong( win, GWL_STYLE, s );
            }
        }
    }
}

void WdeBringWindowToTop( HWND win )
{
#if 1
    SetWindowPos( win, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
#else
    BringWindowToTop( win );
#endif
}

bool WdePutObjFirst( OBJPTR obj, LIST **list )
{
    if( list == NULL || *list == NULL ) {
        WdeWriteTrail( "WdePutObjFirst: Unexpected NULL list" );
        return( FALSE );
    }

    if( obj == ListElement( *list ) ) {
        return( TRUE );
    }

    if( ListFindElt( *list, obj ) ) {
        ListRemoveElt( list, obj );
        ListAddElt( list, obj );
    } else {
        WdeWriteTrail( "WdePutObjFirst: Object not in list" );
        return( FALSE );
    }

    return( TRUE );
}

bool WdeFindObjectsAtPt( POINT *pt, LIST **obj_list, LIST *olist )
{
    SUBOBJ_REQUEST  req;
    OBJPTR          child;
    OBJPTR          sel;
    OBJPTR          first_clear;
    OBJPTR          first_nomove;
    bool            b;
    bool            is_clear;
    bool            can_move;
    LIST            *ol;

    *obj_list = NULL;

    req.p.ty = AT_POINT;
    req.p.pt = *pt;
    if( !WdeFindSubObjects( &req, &ol, olist ) ) {
        return( FALSE );
    }

    sel = NULL;
    first_clear = NULL;
    first_nomove = NULL;

    do {
        child = ListElement( ol );
        is_clear = Forward( child, IS_OBJECT_CLEAR, &b, NULL ) && b;
        can_move = ValidateAction( child, PICK, &req.p.pt );
        if( can_move ) {
            sel = child;
            break;
        } else {
            if( is_clear ) {
                if( !first_clear ) {
                    first_clear = child;
                }
            } else {
                if( !first_nomove ) {
                    first_nomove = child;
                }
            }
        }
        ol = ListNext( ol );
    } while( ol != NULL );

    if( sel != NULL ) {
        ListAddElt( obj_list, sel );
    } else if( first_nomove ) {
        ListAddElt( obj_list, first_nomove );
    } else if( first_clear ) {
        ListAddElt( obj_list, first_clear );
    }

    ListFree( ol );

    return( *obj_list != NULL );
}

bool WdeFindSubObjects( SUBOBJ_REQUEST *req, LIST **obj_list, LIST *olist)
{
    OBJPTR   child;
    RECT     child_rect;

    *obj_list = NULL;

    switch( req->p.ty ) {
    case ALL:
        *obj_list = WdeListCopy( olist );
        break;

    case BY_POINT:
        return( WdeFindObjectsAtPt( &req->p.pt, obj_list, olist ) );

    case AT_POINT:
        for( ; olist != NULL; olist = ListNext( olist ) ) {
            child = ListElement( olist );
            Location( child, &child_rect );
            if( PtInRect( &child_rect, req->p.pt ) ) {
                WdeInsertObject( obj_list, child );
            }
        }
        break;
    }

    return( *obj_list != NULL );
}

char *WdeRectToStr( RECT *r )
{
    char  temp[41];

    sprintf( temp, "%d, %d, %d, %d", r->left, r->top, r->right, r->bottom );

    return( WdeStrDup( temp ) );
}

void WdeStrToRect( char *str, RECT *r )
{
    memset( r, 0, sizeof( RECT ) );
    sscanf( str, "%d, %d, %d, %d", &r->left, &r->top, &r->right, &r->bottom );
}

void WdeDisableChildWindows( HWND hWnd )
{
    HWND win;

    win = GetWindow( hWnd, GW_CHILD );
    while( win != NULL ) {
        EnableWindow( win, FALSE );
        win = GetWindow( win, GW_HWNDNEXT );
    }
}

char *WdeGetDefineProcFromOBJID( Wde_Objects obj_id )
{
    int i;

    for( i = 0; WdeOBJIDToDefineProc[i].obj_id != LAST__OBJ; i++ ) {
        if( WdeOBJIDToDefineProc[i].obj_id == obj_id ) {
            return( WdeOBJIDToDefineProc[i].proc_name );
        }
    }

    return( NULL );
}

void WdeSetDefineControlInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    char        *cp;
    char        *cp2;

    /* set the control text */
    cp = WdeResNameOrOrdinalToStr( GETCTL_TEXT( o_info->info.c.info ), 10 );
    if( cp != NULL ) {
        cp2 = WRConvertStringFrom( cp, "\t\n", "tn" );
        if( cp2 != NULL ) {
            WdeSetEditWithStr( cp2, hDlg, IDB_TEXT );
            WRMemFree( cp2 );
        }
        WRMemFree( cp );
    }

    WdeSetWinStyles( hDlg, GETCTL_STYLE( o_info->info.c.info ), o_info->mask );

    /* JPK - Help Id stuff is done in here */
    WdeSetDefineObjectSymbolInfo( o_info, hDlg );

    if( o_info->set_func != NULL ) {
        (*o_info->set_func)( o_info, hDlg );
    }
}

void WdeGetDefineControlInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    DialogStyle style;
    char        *cp;
    char        *cp2;
    void        *vp;
    bool        mod;

    /* set the control text */
    cp = WdeGetStrFromEdit( hDlg, IDB_TEXT, &mod );
    if( mod ) {
        cp2 = WRConvertStringTo( cp, "\t\n", "tn" );
        if( cp2 != NULL ) {
            vp = WdeStrToResNameOrOrdinal( cp2 );
            if( GETCTL_TEXT( o_info->info.c.info ) ) {
                WRMemFree( GETCTL_TEXT( o_info->info.c.info ) );
            }
            SETCTL_TEXT( o_info->info.c.info, (ResNameOrOrdinal *)vp );
            WRMemFree( cp2 );
        }
        WRMemFree( cp );
    }

    WdeGetDefineObjectSymbolInfo( o_info, hDlg );
    WdeGetDefineObjectHelpSymbolInfo( o_info, hDlg );

    style = GETCTL_STYLE( o_info->info.c.info );
    WdeGetWinStyles( hDlg, &style, o_info->mask );
    SETCTL_STYLE( o_info->info.c.info, style );

    if( o_info->get_func != NULL ) {
        (*o_info->get_func)( o_info, hDlg );
    }
}


void WdeSetDefineObjectSymbolInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    int_32 helpid;

    if( o_info == NULL ) {
        return;
    }

    if( o_info->res_info->hash_table != NULL ) {
        WdeAddSymbolsToComboBox( o_info->res_info->hash_table, hDlg, IDB_SYMBOL );
        // JPK - add for help id
        WdeAddSymbolsToComboBox( o_info->res_info->hash_table, hDlg, IDB_HELPSYMBOL );
    }

    if( o_info->obj_id == DIALOG_OBJ ) {        /* dialog object */

        if( o_info->info.d.name != NULL && o_info->info.d.name->IsName ) {
            char *str1, *str2;
            int len;
            str1 = WResIDToStr( o_info->info.d.name );
            if( str1 != NULL ) {
                len = strlen( str1 ) + 3;
                str2 = WRMemAlloc( len );
                if( str2 != NULL ) {
                    str2[0] = '"';
                    strcpy( &str2[1], str1 );
                    str2[len - 2] = '"';
                    str2[len - 1] = '\0';
                    WdeSetEditWithStr( str2, hDlg, IDB_SYMBOL );
                    WRMemFree( str2 );
                }
                WRMemFree( str1 );
            }
            o_info->info.d.id = 0;
            o_info->info.d.use_id = FALSE;
        } else {
            if( o_info->symbol != NULL ) {
                WdeSetComboWithStr( o_info->symbol, hDlg, IDB_SYMBOL );
            } else {
                WdeSetEditWithSINT16( (int_16)o_info->info.d.name->ID.Num,
                                      10, hDlg, IDB_SYMBOL );
            }
            o_info->info.d.id = (uint_16)o_info->info.d.name->ID.Num;
            o_info->info.d.use_id = TRUE;
            WdeSetEditWithSINT16( (int_16) o_info->info.d.name->ID.Num,
                                  10, hDlg, IDB_ID );
        }

        /* JPK - added for help id */
        helpid = GETHDR_HELPID( o_info->info.d.header );
        if( o_info->info.d.header->helpsymbol != NULL ) {
            WdeSetComboWithStr( o_info->info.d.header->helpsymbol, hDlg, IDB_HELPSYMBOL );
        } else {
            if( helpid > 0 ) {
                WdeSetEditWithSINT32( helpid, 10, hDlg, IDB_HELPSYMBOL );
            }
        }
        if( helpid > 0 ) {
            WdeSetEditWithSINT32( helpid, 10, hDlg, IDB_HELPID );
        }

    } else {    /* this is a control object */

        if( o_info->symbol != NULL ) {
            WdeSetComboWithStr( o_info->symbol, hDlg, IDB_SYMBOL );
        } else {
            WdeSetEditWithSINT16( (int_16)GETCTL_ID( o_info->info.c.info ),
                                  10, hDlg, IDB_SYMBOL );
        }

        WdeSetEditWithSINT16( (int_16)GETCTL_ID( o_info->info.c.info ),
                              10, hDlg, IDB_ID );

        /* JPK - added for help id */
        helpid = GETCTL_HELPID( o_info->info.c.info );
        if( o_info->info.c.info->helpsymbol ) {
            WdeSetComboWithStr( o_info->info.c.info->helpsymbol, hDlg, IDB_HELPSYMBOL );
        } else {
            if( helpid > 0 ) {
                WdeSetEditWithSINT32( helpid, 10, hDlg, IDB_HELPSYMBOL );
            }
        }
        if( helpid > 0 ) {
            WdeSetEditWithSINT32( helpid, 10, hDlg, IDB_HELPID );
        }
    }
}

void WdeGetDefineObjectSymbolInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    char                *str;
    char                *cp;
    bool                dup;
    bool                quoted_str;
    bool                str_is_ordinal;
    uint_16             ord;
    WdeHashEntry        *entry;

    if( o_info == NULL ) {
        return;
    }

    if( o_info->symbol != NULL ) {
        WRMemFree( o_info->symbol );
    }
    o_info->symbol = NULL;

    str = WdeGetStrFromCombo( hDlg, IDB_SYMBOL );
    if( str == NULL ) {
        return;
    }

    WRStripSymbol( str );

    quoted_str = FALSE;
    if( str[0] == '"' ) {
        unsigned char   *s;

        str[0] = ' ';
        cp = NULL;
        for( s = (unsigned char *)str; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) == 1 && *s == '"' ) {
                cp = (char *)s;
            }
        }
        if( cp != NULL ) {
            *cp = '\0';
        }
        WRStripSymbol( str );
        quoted_str = TRUE;
    }

    if( str[0] == '\0' ) {
        WRMemFree( str );
        return;
    }

    ord = (uint_16)strtoul( str, &cp, 0 );
    str_is_ordinal = (*cp == '\0');

    if( o_info->obj_id == DIALOG_OBJ ) {

        if( o_info->info.d.name != NULL ) {
            WRMemFree( o_info->info.d.name );
        }
        o_info->info.d.name = NULL;

        if( quoted_str ) {
            o_info->info.d.name = WResIDFromStr( str );
        } else if( str_is_ordinal ) {
            o_info->info.d.name = WResIDFromNum( ord );
        } else {
            dup = false;
            o_info->symbol = WdeStrDup( str );
            strupr( o_info->symbol );
            entry = WdeDefAddHashEntry( o_info->res_info->hash_table, str, &dup );
            o_info->info.d.name = WResIDFromNum( entry->value );
        }

    } else {

        if( str_is_ordinal ) {
            SETCTL_ID( o_info->info.c.info, ord );
        } else {
            dup = false;
            o_info->symbol = WdeStrDup( str );
            strupr( o_info->symbol );
            entry = WdeDefAddHashEntry( o_info->res_info->hash_table, str, &dup );
            if( entry != NULL ) {
                SETCTL_ID( o_info->info.c.info, entry->value );
            }
        }

    }

    WRMemFree( str );
}

/* JPK - added function for help id */
void WdeGetDefineObjectHelpSymbolInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    char                *str;
    char                *cp;
    bool                dup;
    bool                quoted_str;
    bool                str_is_ordinal;
    uint_32             ord;
    WdeHashEntry        *entry;

    if( o_info == NULL ) {
        return;
    }

    if( o_info->helpsymbol != NULL ) {
        WRMemFree( o_info->helpsymbol );
    }
    o_info->helpsymbol = NULL;

    str = WdeGetStrFromCombo( hDlg, IDB_HELPSYMBOL );
    if( str == NULL ) {
        return;
    }

    WRStripSymbol( str );

    quoted_str = FALSE;
    if( str[0] == '"' ) {
        unsigned char   *s;

        str[0] = ' ';
        cp = NULL;
        for( s = (unsigned char *)str; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) == 1 && *s == '"' ) {
                cp = (char *)s;
            }
        }
        if( cp != NULL ) {
            *cp = '\0';
        }
        WRStripSymbol( str );
        quoted_str = TRUE;
    }

    if( str[0] == '\0' ) {
        WRMemFree( str );
        return;
    }

    ord = (uint_32)strtoul( str, &cp, 0 );
    str_is_ordinal = (*cp == '\0');

    if( o_info->obj_id == DIALOG_OBJ ) {

        if( str_is_ordinal ) {
            SETHDR_HELPID( o_info->info.d.header, ord );
        } else {
            dup = true;
            o_info->helpsymbol = WdeStrDup( str );
            o_info->info.d.header->helpsymbol = WdeStrDup( str );
            strupr( o_info->helpsymbol );
            ord = GETHDR_HELPID( o_info->info.d.header );
            if( ord > 0 ) {
                entry = WdeAddHashEntry( o_info->res_info->hash_table, str, ord, &dup );
            } else {
                entry = WdeDefAddHashEntry( o_info->res_info->hash_table, str, &dup );
            }
            if( entry != NULL ) {
                SETHDR_HELPID( o_info->info.d.header, entry->value );
            }
        }

    } else {

        if( str_is_ordinal ) {
            SETCTL_HELPID( o_info->info.c.info, ord );
        } else {
            dup = false;
            o_info->helpsymbol = WdeStrDup( str );
            o_info->info.c.info->helpsymbol = WdeStrDup( str );
            strupr( o_info->helpsymbol );
            ord = GETCTL_HELPID( o_info->info.c.info );
            if( ord > 0 ) {
                entry = WdeAddHashEntry( o_info->res_info->hash_table, str, ord, &dup );
            } else {
                entry = WdeDefAddHashEntry( o_info->res_info->hash_table, str, &dup );
            }
            if( entry != NULL ) {
                SETCTL_HELPID( o_info->info.c.info, entry->value );
            }
        }
    }

    WRMemFree( str );
}

void WdeAddSymbolToObjectHashTable( WdeResInfo *res_info, char *symbol, WdeHashValue val )
{
    bool          force;

    if( res_info != NULL && symbol != NULL ) {
        if( res_info->hash_table == NULL ) {
            res_info->hash_table = WdeInitHashTable();
        }
        if( res_info->hash_table != NULL ) {
            force = TRUE;
            WdeAddHashEntry( res_info->hash_table, symbol, val, &force );
        }
    }
}

UINT WdeTranslateNCMouseMsg( UINT ncm )
{
    int i;

    for( i = 0; WdeMouseMapper[i].ncm; i++ ) {
        if( WdeMouseMapper[i].ncm == ncm ) {
            return( WdeMouseMapper[i].m );
        }
    }
    return( 0 );
}

bool WdeProcessMouse( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UINT    new_message;
    WPARAM  new_wparam;
    POINT   mouse_location;
    bool    ret;

    ret = FALSE;

    switch( message ) {
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
        MAKE_POINT( mouse_location, lParam );
        ClientToScreen( hWnd, &mouse_location );
        ScreenToClient( WdeGetEditWindowHandle( NULL ), &mouse_location );
        WdePassToEdit( message, wParam, MAKELONG( mouse_location.x, mouse_location.y ) );
        ret = TRUE;
        break;

    case WM_NCLBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCMOUSEMOVE:
        new_wparam = WdeGetKeys();
        new_message = WdeTranslateNCMouseMsg( message );
        MAKE_POINT( mouse_location, lParam );
        ScreenToClient( WdeGetEditWindowHandle( NULL ), &mouse_location );
        WdePassToEdit( new_message, new_wparam,
                       MAKELONG( mouse_location.x, mouse_location.y ) );
        ret = TRUE;
        break;
    }

    return( ret );
}

BOOL WdeControlDefine( WdeDefineObjectInfo *o_info )
{
    char                *symbol;
    char                *helpsymbol;
    char                *dlg_template;
    INT_PTR             redraw;
    FARPROC             def_proc;
    HINSTANCE           app_inst;
    WdeOrderMode        mode;

    if( Forward( o_info->obj, GET_ORDER_MODE, &mode, NULL ) && mode != WdeSelect ) {
        return( TRUE );
    }

    if( !Forward( o_info->obj, GET_OBJECT_INFO, &o_info->info.c.info, &symbol ) ) {
        WdeWriteTrail( "WdeControlDefine: GET_OBJECT_INFO failed!" );
        return( FALSE );
    }

    /* JPK - added for help id */
    if( !Forward( o_info->obj, GET_OBJECT_HELPINFO, &o_info->info.c.info, &helpsymbol ) ) {
        WdeWriteTrail( "WdeControlDefine: GET_OBJECT_HELPINFO failed!" );
        return( FALSE );
    }

    o_info->symbol = WdeStrDup( symbol );
    o_info->helpsymbol = WdeStrDup( helpsymbol );
    o_info->res_info = WdeGetCurrentRes();

    if( o_info->win == NULL ) {
        if( !Forward( o_info->obj, GET_WINDOW_HANDLE, &o_info->win, NULL ) ) {
            WdeWriteTrail( "WdeControlDefine: GET_WINDOW_HANDLE failed!" );
            return( FALSE );
        }
    }

    if( !WdeGetOption( WdeOptUseDefDlg ) ) {
        return( WdeGenericDefine( o_info ) );
    }

    WdeSetStatusText( NULL, "", FALSE );
    WdeSetStatusByID( WDE_DEFININGCONTROL, -1 );

    app_inst = WdeGetAppInstance();

    dlg_template = WdeGetDefineProcFromOBJID( o_info->obj_id );
    if( dlg_template == NULL ) {
        WdeWriteTrail( "WdeControlDefine: Invalid OBJECT_ID!" );
        return( FALSE );
    }

    redraw = -1;

    def_proc = MakeProcInstance( (FARPROC)WdeControlDefineProc, app_inst );

    if( def_proc != NULL ) {
        redraw = JDialogBoxParam( app_inst, dlg_template, o_info->win,
                                  (DLGPROC)def_proc, (LPARAM)o_info );
        FreeProcInstance( def_proc );
    }

    if( redraw == -1 ) {
        WdeWriteTrail( "WdeControlDefine: Dialog not created!" );
        return( FALSE );
    } else if( redraw ) {
        if( !Forward( o_info->obj, SET_OBJECT_INFO, NULL, o_info->symbol ) ) {
            WdeWriteTrail( "WdeControlDefine: SET_OBJECT_INFO failed!" );
            return( FALSE );
        }
        /* JPK - added this for help id */
        if( !Forward( o_info->obj, SET_OBJECT_HELPINFO, NULL, o_info->helpsymbol ) ) {
            WdeWriteTrail( "WdeControlDefine: SET_OBJECT_HELPINFO failed!" );
            return( FALSE );
        }
        if( o_info->symbol != NULL ) {
            WdeAddSymbolToObjectHashTable( o_info->res_info, o_info->symbol,
                                           GETCTL_ID( o_info->info.c.info ) );
        }
        if( o_info->helpsymbol != NULL ) {
            WdeAddSymbolToObjectHashTable( o_info->res_info, o_info->helpsymbol,
                                           GETCTL_HELPID( o_info->info.c.info ) );
        }
        if( !Forward( o_info->obj, DESTROY_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeControlDefine: DESTROY_WINDOW failed!" );
            return( FALSE );
        }
        if( !Forward( o_info->obj, CREATE_WINDOW, NULL, NULL ) ) {
            WdeWriteTrail( "WdeControlDefine: CREATE_WINDOW failed!" );
            return( FALSE );
        }
        Notify( o_info->obj, PRIMARY_OBJECT, NULL );
    }

    if( o_info->symbol != NULL ) {
        WRMemFree( o_info->symbol );
        o_info->symbol = NULL;
    }

    if( o_info->helpsymbol != NULL ) {
        WRMemFree( o_info->helpsymbol );
        o_info->helpsymbol = NULL;
    }

    WdeSetStatusReadyText();

    return( TRUE );
}

WINEXPORT BOOL CALLBACK WdeControlDefineProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    static WdeDefineObjectInfo  *o_info = NULL;
    static BOOL                 init_done = FALSE;
    BOOL                        ret;

    ret = FALSE;

    if( init_done && o_info != NULL ) {
        if( o_info->hook_func != NULL ) {
            ret = (*o_info->hook_func)( hDlg, message, wParam, lParam, 0 );
        }

        if( !ret ) {
            ret = WdeProcessSymbolCombo( hDlg, message, wParam, lParam,
                                         o_info->res_info->hash_table,
                                         GETCTL_ID( o_info->info.c.info ), TRUE );
        }

        /* JPK - added for help id support */
        if( !ret ) {
            ret = WdeProcessHelpSymbolCombo( hDlg, message, wParam, lParam,
                                             o_info->res_info->hash_table,
                                             GETCTL_HELPID( o_info->info.c.info ),
                                             TRUE );
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
        init_done = TRUE;
        o_info = (WdeDefineObjectInfo *)lParam;
        if( o_info == NULL ) {
            EndDialog( hDlg, FALSE );
            init_done = FALSE;
            ret = TRUE;
        } else {
            WdeSetDefineControlInfo( o_info, hDlg );
        }
        ret = TRUE;
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDB_HELP:
            WdeHelpRoutine();
            break;

        case IDOK:
            WdeGetDefineControlInfo( o_info, hDlg );
            EndDialog( hDlg, TRUE );
            ret = TRUE;
            init_done = FALSE;
            o_info = NULL;
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            init_done = FALSE;
            o_info = NULL;
            break;
        }
    }

    return( ret );
}


BOOL WdeWinStylesHook( HWND hDlg, UINT message,
                       WPARAM wParam, LPARAM lParam, DialogStyle mask )
{
    DialogStyle  tstyle;
    BOOL         ret;
    bool         overlapped;

    _wde_touch( lParam );

    ret = FALSE;

    if( message == WM_COMMAND && GET_WM_COMMAND_CMD( wParam, lParam ) == BN_CLICKED ) {
        switch( LOWORD( wParam ) ) {
        case IDB_WS_CHILD:
            if( IsDlgButtonChecked( hDlg, IDB_WS_CHILD ) ) {
                CheckDlgButton( hDlg, IDB_WS_POPUP, BST_UNCHECKED );
            }
            ret = TRUE;
            break;

        case IDB_WS_POPUP:
            if( IsDlgButtonChecked( hDlg, IDB_WS_POPUP ) ) {
                CheckDlgButton( hDlg, IDB_WS_CHILD, BST_UNCHECKED );
            }
            ret = TRUE;
            break;

        case IDB_WS_CAPTION:
            if( IsDlgButtonChecked( hDlg, IDB_WS_CAPTION ) ) {
                CheckDlgButton( hDlg, IDB_WS_BORDER, BST_CHECKED );
                CheckDlgButton( hDlg, IDB_WS_DLGFRAME, BST_CHECKED );
            } else {
                CheckDlgButton( hDlg, IDB_WS_BORDER, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_WS_DLGFRAME, BST_UNCHECKED );
            }
            ret = TRUE;
            break;

        case IDB_WS_DLGFRAME:
        case IDB_WS_BORDER:
            if( IsDlgButtonChecked( hDlg, IDB_WS_BORDER ) ) {
                if( IsDlgButtonChecked( hDlg, IDB_WS_DLGFRAME ) ) {
                    CheckDlgButton( hDlg, IDB_WS_CAPTION, BST_CHECKED );
                } else {
                    CheckDlgButton( hDlg, IDB_WS_CAPTION, BST_UNCHECKED );
                }
            } else {
                CheckDlgButton( hDlg, IDB_WS_CAPTION, BST_UNCHECKED );
            }
            ret = TRUE;
            break;

#if __NT__XX
        case IDB_WS_EX_CLIENTEDGE:
            if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CLIENTEDGE ) &&
                IsDlgButtonChecked( hDlg, IDB_WS_EX_WINDOWEDGE ) ) {
                CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, BST_CHECKED );
            } else {
                CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, BST_UNCHECKED );
            }
            break;

        case IDB_WS_EX_WINDOWEDGE:
        check_WINDOWEDGE:
            if( IsDlgButtonChecked( hDlg, IDB_WS_EX_WINDOWEDGE ) ) {
                if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CLIENTEDGE ) ) {
                    CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, BST_CHECKED );
                } else {
                    CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, BST_UNCHECKED );
                }

                if( IsDlgButtonChecked( hDlg, IDB_WS_EX_TOOLWINDOW ) &&
                    IsDlgButtonChecked( hDlg, IDB_WS_EX_TOPMOST ) ) {
                    CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, BST_CHECKED );
                } else {
                    CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, BST_UNCHECKED );
                }
            } else {
                CheckDlgButton( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, BST_UNCHECKED );
            }
            break;

        case IDB_WS_EX_TOOLWINDOW:
        case IDB_WS_EX_TOPMOST:
            if( IsDlgButtonChecked( hDlg, IDB_WS_EX_TOOLWINDOW ) &&
                IsDlgButtonChecked( hDlg, IDB_WS_EX_WINDOWEDGE ) &&
                IsDlgButtonChecked( hDlg, IDB_WS_EX_TOPMOST ) ) {
                CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, BST_CHECKED );
            } else {
                CheckDlgButton( hDlg, IDB_WS_EX_PALETTEWINDOW, BST_UNCHECKED );
            }
            break;

        case IDB_WS_EX_OVERLAPPEDWINDOW:
            if( IsDlgButtonChecked( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW ) ) {
                CheckDlgButton( hDlg, IDB_WS_EX_CLIENTEDGE, BST_CHECKED );
                CheckDlgButton( hDlg, IDB_WS_EX_WINDOWEDGE, BST_CHECKED );
                // force it to recheck WINDOWEDGE
                goto check_WINDOWEDGE;
            } else {
                if( !IsDlgButtonChecked( hDlg, IDB_WS_EX_PALETTEWINDOW ) ) {
                    CheckDlgButton( hDlg, IDB_WS_EX_WINDOWEDGE, BST_UNCHECKED );
                }
                CheckDlgButton( hDlg, IDB_WS_EX_CLIENTEDGE, BST_UNCHECKED );
            }
            break;

        case IDB_WS_EX_PALETTEWINDOW:
            if( IsDlgButtonChecked( hDlg, IDB_WS_EX_PALETTEWINDOW ) ) {
                CheckDlgButton( hDlg, IDB_WS_EX_WINDOWEDGE, BST_CHECKED );
                CheckDlgButton( hDlg, IDB_WS_EX_TOOLWINDOW, BST_CHECKED );
                CheckDlgButton( hDlg, IDB_WS_EX_TOPMOST, BST_CHECKED );
                // force it to recheck WINDOWEDGE
                goto check_WINDOWEDGE;
            } else {
                if( !IsDlgButtonChecked( hDlg, IDB_WS_EX_OVERLAPPEDWINDOW ) ) {
                    CheckDlgButton( hDlg, IDB_WS_EX_WINDOWEDGE, BST_UNCHECKED );
                }
                CheckDlgButton( hDlg, IDB_WS_EX_TOOLWINDOW, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDB_WS_EX_TOPMOST, BST_UNCHECKED );
            }
            break;
#endif
        }

        WdeGetWinStyles( hDlg, &tstyle, mask );
        overlapped = IsDlgButtonChecked( hDlg, IDB_WS_OVERLAPPED );
        if( !(tstyle & 0xffff0000) ) {
            if( !overlapped ) {
                CheckDlgButton( hDlg, IDB_WS_OVERLAPPED, BST_CHECKED );
            }
        } else {
            if( overlapped ) {
                CheckDlgButton( hDlg, IDB_WS_OVERLAPPED, BST_UNCHECKED );
            }
        }
    }

    return( ret );
}

bool WdeProcessSymbolCombo( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam,
                            WdeHashTable *table, uint_16 id,  bool use_id )
{
    WORD    hw;
    bool    processed;

    _wde_touch( lParam );
    _wde_touch( id );
    _wde_touch( use_id );

    processed = FALSE;

    if ( message == WM_COMMAND && LOWORD( wParam ) == IDB_SYMBOL ) {
        hw = GET_WM_COMMAND_CMD( wParam, lParam );
        if( hw == CBN_EDITCHANGE || hw == CBN_SELCHANGE ) {
            WdeDefineObjectLookupComboEntry( hDlg, hw, table );
            processed = TRUE;
        }
    }

    return( processed );
}

void WdeDefineObjectLookupComboEntry( HWND hDlg, WORD hw, WdeHashTable *table )
{
    char                *str;
    char                *cp;
    WdeHashValue        value;
    bool                found;
    int                 index;
    int                 count;

    if( table == NULL ) {
        return;
    }

    count = (int)SendDlgItemMessage( hDlg, IDB_SYMBOL, CB_GETCOUNT, 0, 0L );
    if( count == 0 || count == CB_ERR ) {
        return;
    }

    str = NULL;
    if( hw == CBN_EDITCHANGE ) {
        str = WdeGetStrFromCombo( hDlg, IDB_SYMBOL );
    } else {
        index = (int)SendDlgItemMessage( hDlg, IDB_SYMBOL, CB_GETCURSEL, 0, 0L );
        if( index != CB_ERR ) {
            str = WdeGetStrFromComboLBox( hDlg, IDB_SYMBOL, index );
        }
    }

    if( str == NULL ) {
        return;
    }

    WRStripSymbol( str );

    // if the string numeric or empty then return
    strtoul( str, &cp, 0 );
    if( *cp == '\0' ) {
        WRMemFree( str );
        return;
    }

    if( str[0] != '"' ) {
        value = WdeLookupName( table, str, &found );
        if( found ) {
            WdeSetEditWithSINT32( (int_32)value, 10, hDlg, IDB_ID );
        }
    }

    WRMemFree( str );
}

/* JPK - added for help id support
 *     - these next two routines are essentially the same as those for
 *       the symbol combo box
*/
bool WdeProcessHelpSymbolCombo( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam,
                                WdeHashTable *table, uint_32 id, bool use_id )
{
    WORD    hw;
    bool    processed;

    _wde_touch( lParam );
    _wde_touch( id );
    _wde_touch( use_id );

    processed = FALSE;

    if( message == WM_COMMAND && LOWORD( wParam ) == IDB_HELPSYMBOL ) {
        hw = GET_WM_COMMAND_CMD( wParam, lParam );
        if( hw == CBN_EDITCHANGE || hw == CBN_SELCHANGE ) {
            WdeDefineObjectLookupHelpComboEntry( hDlg, hw, table );
            processed = TRUE;
        }
    }

    return( processed );
}

void WdeDefineObjectLookupHelpComboEntry( HWND hDlg, WORD hw, WdeHashTable *table )
{
    char                *str;
    char                *cp;
    WdeHashValue        value;
    bool                found;
    int                 index;
    int                 count;

    if( table == NULL ) {
        return;
    }

    count = (int)SendDlgItemMessage( hDlg, IDB_HELPSYMBOL, CB_GETCOUNT, 0, 0L );
    if( count == 0 || count == CB_ERR ) {
        return;
    }

    str = NULL;
    if( hw == CBN_EDITCHANGE ) {
        str = WdeGetStrFromCombo( hDlg, IDB_HELPSYMBOL );
    } else {
        index = (int)SendDlgItemMessage( hDlg, IDB_HELPSYMBOL, CB_GETCURSEL, 0, 0L );
        if( index != CB_ERR ) {
            str = WdeGetStrFromComboLBox( hDlg, IDB_HELPSYMBOL, index );
        }
    }

    if( str == NULL ) {
        return;
    }

    WRStripSymbol( str );

    // if the string numeric or empty then return
    strtoul( str, &cp, 0 );
    if( *cp == '\0' ) {
        WRMemFree( str );
        return;
    }

    if( str[0] != '"' ) {
        value = WdeLookupName( table, str, &found );
        if( found ) {
            WdeSetEditWithSINT32( (int_32)value, 10, hDlg, IDB_HELPID );
        }
    }

    WRMemFree( str );
}

void WdeMapWindowRect( HWND src, HWND dest, RECT *rect )
{
    MapWindowPoints( src, dest, (POINT *)rect, 2 );
}

void WdeEXSetDefineInfo( WdeDefineObjectInfo *o_info, HWND hDlg )
{
    DialogStyle mask;

    _wde_touch( o_info );

    mask = 0;

#if __NT__XX
    // do the extended style stuff
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_CLIENTEDGE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), TRUE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), TRUE );

    mask = GETCTL_EXSTYLE( o_info->info.c.info );
    if( mask & WS_EX_CLIENTEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_CLIENTEDGE, BST_CHECKED );
    }
    if( mask & WS_EX_STATICEDGE ) {
        CheckDlgButton( hDlg, IDB_WS_EX_STATICEDGE, BST_CHECKED );
    }
    if( mask & WS_EX_NOPARENTNOTIFY ) {
        CheckDlgButton( hDlg, IDB_WS_EX_NOPARENTNOTIFY, BST_CHECKED );
    }
    if( mask & WS_EX_RIGHT ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RIGHT, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LEFT, BST_CHECKED );
    }
    if( mask & WS_EX_RTLREADING ) {
        CheckDlgButton( hDlg, IDB_WS_EX_RTLREADING, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDB_WS_EX_LTRREADING, BST_CHECKED );
    }
#else
    // disable the extended styles
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_CLIENTEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_STATICEDGE ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_NOPARENTNOTIFY ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LEFT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RIGHT ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_LTRREADING ), FALSE );
    EnableWindow( GetDlgItem( hDlg, IDB_WS_EX_RTLREADING ), FALSE );
#endif
}

void WdeEXGetDefineInfo ( WdeDefineObjectInfo *o_info, HWND hDlg )
{
#if __NT__XX
    DialogStyle mask;

    // set the extended mask
    mask = 0;
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_STATICEDGE ) ) {
        mask |= WS_EX_STATICEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_CLIENTEDGE ) ) {
        mask |= WS_EX_CLIENTEDGE;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_NOPARENTNOTIFY ) ) {
        mask |= WS_EX_NOPARENTNOTIFY;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RIGHT ) ) {
        mask |= WS_EX_RIGHT;
    }
    if( IsDlgButtonChecked( hDlg, IDB_WS_EX_RTLREADING ) ) {
        mask |= WS_EX_RTLREADING;
    }
    SETCTL_EXSTYLE( o_info->info.c.info, mask );
#else
    _wde_touch( o_info );
    _wde_touch( hDlg );
#endif
}
