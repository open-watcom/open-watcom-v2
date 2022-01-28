/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include "wrestat.h"
#include "wremain.h"
#include "wre.rh"
#include "wrelist.h"
#include "wrehints.h"
#include "wremsg.h"
#include "ldstr.h"
#include "wrdll.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_NESTED_POPUPS 2

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    int         id;
    msg_id      hint;
} WREHintItem;

typedef struct {
    int         loc[MAX_NESTED_POPUPS];
    HMENU       hpopup;
    msg_id      hint;
} WREPopupHintItem;

typedef struct {
    int                 num;
    HMENU               hmenu;
    WREPopupHintItem    *hint_items;
} WREPopupListItem;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WREHintItem      *WREGetHintItem( int id );
static void             WREHandlePopupHint( HMENU, HMENU );
static msg_id           WREGetPopupHint( WREPopupListItem *, HMENU );
static WREPopupListItem *WREFindPopupListItem( HMENU hmenu );
static bool             WRECreateWREPopupListItem( int, HMENU, WREPopupHintItem * );
static bool             WREInitHintItems( int, HMENU, WREPopupHintItem * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST  *WREPopupList = NULL;

static WREHintItem WREHints[] =
{
    { IDM_NEW                   , WRE_HINT_NEW                  }
,   { IDM_OPEN                  , WRE_HINT_OPEN                 }
,   { IDM_SAVE                  , WRE_HINT_SAVE                 }
,   { IDM_SAVEAS                , WRE_HINT_SAVEAS               }
,   { IDM_EXIT                  , WRE_HINT_EXIT                 }
,   { IDM_OPTIONS               , WRE_HINT_OPTIONS              }
,   { IDM_NEW_CURSOR            , WRE_HINT_NEW_CURSOR           }
,   { IDM_NEW_BITMAP            , WRE_HINT_NEW_BITMAP           }
,   { IDM_NEW_ICON              , WRE_HINT_NEW_ICON             }
,   { IDM_NEW_MENU              , WRE_HINT_NEW_MENU             }
,   { IDM_NEW_DIALOG            , WRE_HINT_NEW_DIALOG           }
,   { IDM_NEW_STRING            , WRE_HINT_NEW_STRING           }
,   { IDM_NEW_FONT              , WRE_HINT_NEW_FONT             }
,   { IDM_NEW_ACCELERATOR       , WRE_HINT_NEW_ACCELERATOR      }
,   { IDM_NEW_RCDATA            , WRE_HINT_NEW_RCDATA           }
,   { IDM_RES_SAVEAS            , WRE_HINT_RES_SAVEAS           }
,   { IDM_RES_SAVE_INTO         , WRE_HINT_RES_SAVE_INTO        }
,   { IDM_RES_EDIT              , WRE_HINT_RES_EDIT             }
,   { IDM_RES_RENAME            , WRE_HINT_RES_RENAME           }
,   { IDM_RES_MEM_FLAGS         , WRE_HINT_RES_MEM_FLAGS        }
,   { IDM_SHOW_RIBBON           , WRE_HINT_SHOW_RIBBON          }
,   { IDM_MDI_CASCADE           , WRE_HINT_MDI_CASCADE          }
,   { IDM_MDI_TILEH             , WRE_HINT_MDI_TILEH            }
,   { IDM_MDI_TILEV             , WRE_HINT_MDI_TILEV            }
,   { IDM_MDI_ARRANGE           , WRE_HINT_MDI_ARRANGE          }
,   { IDM_ABOUT                 , WRE_HINT_ABOUT                }
,   { IDM_HELP                  , WRE_HINT_HELP                 }
,   { IDM_HELP_SEARCH           , WRE_HINT_HELP_SEARCH          }
,   { IDM_HELP_ON_HELP          , WRE_HINT_HELP_ON_HELP         }
,   { IDM_DELETE                , WRE_HINT_DELETE               }
,   { IDM_CUT                   , WRE_HINT_CUT                  }
,   { IDM_PASTE                 , WRE_HINT_PASTE                }
,   { IDM_COPY                  , WRE_HINT_COPY                 }
,   { IDM_LOAD_SYMBOLS          , WRE_HINT_LOAD_SYMBOLS         }
,   { IDM_EDIT_SYMBOLS          , WRE_HINT_VIEW_SYMBOLS         }
,   { IDM_SAVE_SYMBOLS          , WRE_HINT_WRITE_SYMBOLS        }
,   { -1                        , 0                             }
};

static WREPopupHintItem WREPopupHints[] =
{
    { { 0, -1 },  NULL, WRE_HINT_FILEMENU       }
,   { { 1, -1 },  NULL, WRE_HINT_EDITMENU       }
,   { { 1,  5 },  NULL, WRE_HINT_EDITSYMMENU    }
,   { { 2, -1 },  NULL, WRE_HINT_RESMENU        }
,   { { 2,  0 },  NULL, WRE_HINT_NEWMENU        }
,   { { 3, -1 },  NULL, WRE_HINT_WINDOWMENU     }
,   { { 4, -1 },  NULL, WRE_HINT_HELPMENU       }
};

void WREHandleMenuSelect ( WPARAM wParam, LPARAM lParam )
{
    HMENU hmenu;
    HMENU hpopup;
    WORD  flags;

    if( MENU_CLOSED( wParam, lParam ) ) {
        WRESetStatusText ( NULL, "", TRUE );
    } else {
        hmenu  = WREGetMenuHandle();
        flags = GET_WM_MENUSELECT_FLAGS( wParam, lParam );
        if ( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
            WRESetStatusText ( NULL, "", TRUE );
        } else if ( flags & MF_POPUP ) {
#ifdef __NT__
            hpopup = GetSubMenu( (HMENU)lParam, GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
#else
            hpopup = (HMENU)GET_WM_MENUSELECT_ITEM( wParam, lParam );
#endif
            WREHandlePopupHint ( hmenu, hpopup );
        } else {
            WREDisplayHint ( GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
        }
    }
}

void WREDisplayHint( ctl_id id )
{
    char        *buf;
    char        *mditext;
    WREHintItem *hint;

    if( id < WRE_MDI_FIRST ) {
        hint = WREGetHintItem ( id );
        if( hint != NULL ) {
            WRESetStatusByID( 0, hint->hint );
        }
    } else {
        mditext = AllocRCString( WRE_HINT_MDIMSG );
        if( mditext ) {
            buf = WRMemAlloc( strlen(mditext) + 20 + 1 );
            if( buf ) {
                sprintf( buf, mditext, WRE_MDI_FIRST + 1 - id );
                WRESetStatusText( NULL, buf, TRUE );
                WRMemFree( buf );
            }
            FreeRCString( mditext );
        }
    }

    return;
}

WREHintItem *WREGetHintItem ( int id )
{
    int i;

    for ( i = 0; WREHints[i].id != -1; i++ ) {
        if ( WREHints[i].id == id ) {
            return ( &(WREHints[i]) );
        }
    }

    return ( NULL );
}

WREPopupListItem *WREFindPopupListItem ( HMENU hmenu )
{
    LIST             *plist;
    WREPopupListItem *p;

    for ( plist = WREPopupList; plist; plist = ListNext ( plist ) ) {
        p = (WREPopupListItem *) ListElement ( plist );
        if ( p->hmenu == hmenu ) {
            return ( p );
        }
    }

    return ( NULL );
}

msg_id WREGetPopupHint( WREPopupListItem *p, HMENU hpopup )
{
    int i;

    for( i = 0; i < p->num; i++ ) {
        if( p->hint_items[i].hpopup == hpopup ) {
            return( p->hint_items[i].hint );
        }
    }

    return( 0 );
}

void WREHandlePopupHint( HMENU hmenu, HMENU hpopup )
{
    WREPopupListItem    *p;
    msg_id              hint;

    hint = 0;

    p = WREFindPopupListItem( hmenu );

    if( p ) {
        hint = WREGetPopupHint( p, hpopup );
    }

    if( hint > 0 ) {
        WRESetStatusByID( 0, hint );
    } else {
        WRESetStatusText( NULL, "", TRUE );
    }
}

bool WREInitHints( void )
{
    bool ret;

    ret = WRECreateWREPopupListItem( 7, WREGetMenuHandle(), WREPopupHints );

    return( ret );
}

void WREFiniHints ( void )
{
    LIST             *plist;
    WREPopupListItem *p;

    for ( plist = WREPopupList; plist; plist = ListConsume ( plist ) ) {
        p = (WREPopupListItem *)ListElement( plist );
        WRMemFree ( p );
    }
}

bool WRECreateWREPopupListItem ( int num, HMENU hmenu, WREPopupHintItem *hint_items )
{
    WREPopupListItem *p;

    p = (WREPopupListItem *)WRMemAlloc( sizeof(WREPopupListItem) );

    if ( p ) {
        p->num        = num;
        p->hmenu       = hmenu;
        p->hint_items = hint_items;
        if ( WREInitHintItems ( num, hmenu, hint_items ) ) {
            ListAddElt ( &WREPopupList, p );
        } else {
            WRMemFree( p );
            return ( FALSE );
        }
    } else {
        return ( FALSE );
    }

    return ( TRUE );
}

bool WREInitHintItems ( int num, HMENU hmenu, WREPopupHintItem *hint_items )
{
    int   i;
    int   j;
    HMENU hpopup;

    for ( i = 0; i < num; i++ ) {
        hpopup = hmenu;
        for ( j = 0; (j < MAX_NESTED_POPUPS) && (hint_items[i].loc[j] != -1); j++ ) {
            hpopup = GetSubMenu( hpopup, hint_items[i].loc[j] );
        }
        hint_items[i].hpopup = hpopup;
    }

    return ( TRUE );
}
