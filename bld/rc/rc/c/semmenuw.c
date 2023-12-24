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


/*

The data structure for MENUEX's is as follows:


struct MenuExHeader {
    WORD   wVersion;          // Possibly number of different control
    WORD   wBytesToFollow;    // This number is 04 00 for Menus
    WCHAR  data[];
}

struct NormalMenuExItem {
    DWORD  dwType;
    DWORD  dwState;
    DWORD  dwId;
    WORD   fItemFlags;
    WCHAR  szItemText[];    // DWORD Aligned
}

struct PopupMenuExItem {
    DWORD  dwType;
    DWORD  dwState;
    DWORD  dwId;
    WORD   fItemFlags;
    WCHAR  szItemText[];   // DWORD Aligned
    DWORD  dwHelpId;
}

*/


#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"


static void SemFreeSubMenu( FullMenu *submenu );

MenuFlags SemWINAddFirstMenuOption( YYTOKENTYPE token )
/*****************************************************/
{
    return( SemWINAddMenuOption( 0, token ) );
}

MenuFlags SemWINAddMenuOption( MenuFlags oldflags, YYTOKENTYPE token )
/********************************************************************/
{
    switch( token ) {
    case Y_GRAYED:
        oldflags |= MENU_GRAYED;
        break;
    case Y_INACTIVE:
        oldflags |= MENU_INACTIVE;
        break;
    case Y_BITMAP:
        oldflags |= MENU_BITMAP;
        break;
    case Y_CHECKED:
        oldflags |= MENU_CHECKED;
        break;
    case Y_POPUP:
        oldflags |= MENU_POPUP;
        break;
    case Y_MENUBARBREAK:
        oldflags |= MENU_MENUBARBREAK;
        break;
    case Y_MENUBREAK:
        oldflags |= MENU_MENUBREAK;
        break;
    case Y_OWNERDRAW:
        if ( !CmdLineParms.VersionStamp20 )
            oldflags |= MENU_OWNERDRAWN;
        break;
    case Y_HELP:
        oldflags |= MENU_HELP;
        break;
    }

    return( oldflags );
}

FullMenu *SemWINNewMenu( FullMenuItem firstitem )
/***********************************************/
{
    FullMenu       *newmenu;
    FullMenuItem   *newitem;

    newmenu = RESALLOC( sizeof( FullMenu ) );
    newitem = RESALLOC( sizeof( FullMenuItem ) );

    if( newmenu == NULL || newitem == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newitem = firstitem;
    newmenu->head = NULL;
    newmenu->tail = NULL;

    ResAddLLItemAtEnd( (void **)&(newmenu->head), (void **)&(newmenu->tail), newitem );

    return( newmenu );
}

FullMenu *SemWINAddMenuItem( FullMenu *currmenu, FullMenuItem curritem )
/**********************************************************************/
{
    FullMenuItem     *newitem;


    newitem = RESALLOC( sizeof( FullMenuItem ) );

    if( newitem == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newitem = curritem;

    ResAddLLItemAtEnd( (void **)&(currmenu->head), (void **)&(currmenu->tail), newitem );

    return( currmenu );
}

static void SemCheckMenuItemPopup( FullMenuItem *item, YYTOKENTYPE tokentype )
/****************************************************************************/
{
    if( tokentype == Y_MENU ) {
        if( item->item.popup.item.type == MT_MENUEX ) {
            RcError( ERR_MENUEX_POPUP_OPTIONS );
        }
    } else if( tokentype == Y_MENU_EX ) {
        item->item.popup.item.menuData.ItemFlags = MENUEX_POPUP;
        if( item->item.popup.item.type == MT_MENU ) {
            RcError( ERR_MENU_POPUP_OPTIONS );
        }
    }
}

static void SemCheckMenuItemNormal( FullMenuItem *item, YYTOKENTYPE tokentype )
/*****************************************************************************/
{
    if( tokentype == Y_MENU ) {
        if( item->item.normal.type == MT_MENUEX ) {
            RcError( ERR_MENUEX_NORMAL_OPTIONS );
        } else if( item->item.normal.type == MT_MENUEX_NO_ID ) {
            RcError( ERR_MISSING_MENUITEM_ID );
        }

    } else if( tokentype == Y_MENU_EX ) {
        if( item->item.normal.type == MT_MENU ) {
            RcError( ERR_MENU_NORMAL_OPTIONS );
        }
    }
}

static bool SemWriteMenuItem( FullMenuItem *item, int islastitem,
                          int *err_code, YYTOKENTYPE tokentype )
/**************************************************************/
{
    bool    error;

    error = false;
    if( item->IsPopup ) {
        SemCheckMenuItemPopup( item, tokentype );
        if( islastitem ) {
            item->item.popup.item.menuData.ItemFlags |= MENU_ENDMENU;
        }
        if( tokentype == Y_MENU ) {
            if ( CmdLineParms.VersionStamp20 ) {
                error = ResWriteMenuItemPopupOldWin( &(item->item.popup.item.menuData),
                            item->UseUnicode, CurrResFile.fp );
	    }
	    else {
                error = ResWriteMenuItemPopup( &(item->item.popup.item.menuData),
                            item->UseUnicode, CurrResFile.fp );
	    }
        } else if( tokentype == Y_MENU_EX ) {
            error = ResWriteMenuExItemPopup( &(item->item.popup.item.menuData),
                      &(item->item.popup.item.menuExData), item->UseUnicode,
                      CurrResFile.fp );
        }
    } else {
        SemCheckMenuItemNormal( item, tokentype );
        if( islastitem ) {
            item->item.normal.menuData.ItemFlags |= MENU_ENDMENU;
        }
        if( tokentype == Y_MENU ) {
            if ( CmdLineParms.VersionStamp20 ) {
                error = ResWriteMenuItemNormalOldWin( &(item->item.normal.menuData),
                            item->UseUnicode, CurrResFile.fp );
	    }
	    else {
                error = ResWriteMenuItemNormal( &(item->item.normal.menuData),
                            item->UseUnicode, CurrResFile.fp );
	    }
        } else if( tokentype == Y_MENU_EX ) {
            error = ResWriteMenuExItemNormal( &(item->item.normal.menuData),
                         &(item->item.normal.menuExData), item->UseUnicode,
                         CurrResFile.fp );
        }
    }
    *err_code = LastWresErr();
    return( error );
}

static bool SemWriteSubMenu( FullMenu *submenu, int *err_code, YYTOKENTYPE tokentype )
/************************************************************************************/
{
    bool            error;
    int             islastitem;
    FullMenuItem    *curritem;

    error = false;

    if( ErrorHasOccured ) {
        return( false );
    }

    for( curritem = submenu->head; curritem != NULL && !error; curritem = curritem->next ) {
        islastitem = (curritem == submenu->tail);
        if( !ErrorHasOccured ) {
            error = SemWriteMenuItem( curritem, islastitem, err_code, tokentype );
            if( !error && curritem->IsPopup ) {
                error = SemWriteSubMenu( curritem->item.popup.submenu, err_code, tokentype );
            }
        }
    }

    if( error ) {
        ErrorHasOccured = true;
    }
    return( error );
}

static void SemFreeMenuItem( FullMenuItem *curritem )
/****************************************************/
{
    if( curritem->IsPopup ) {
        SemFreeSubMenu( curritem->item.popup.submenu );
        if( curritem->item.popup.item.menuData.ItemText != NULL ) {
            RESFREE( curritem->item.popup.item.menuData.ItemText );
        }
    } else {
        if( curritem->item.normal.menuData.ItemText != NULL ) {
            RESFREE( curritem->item.normal.menuData.ItemText );
        }
    }
}

static void SemFreeSubMenu( FullMenu *submenu )
/**********************************************/
{
    FullMenuItem   *curritem;
    FullMenuItem   *nextitem;

    for( curritem = submenu->head; curritem != NULL; curritem = nextitem ) {
        nextitem = curritem->next;
        SemFreeMenuItem( curritem );
        RESFREE( curritem );
    }
    RESFREE( submenu );
}

void SemWINWriteMenu( WResID *name, ResMemFlags flags, FullMenu *menu,
                   YYTOKENTYPE tokentype )
/********************************************************************/
{
    MenuHeader      head;
    ResLocation     loc;
    bool            error;
    int             err_code;
    uint_8          headerdata[RES_HEADER_SIZE];

    error = false;
    if( !ErrorHasOccured ) {
        if( tokentype == Y_MENU ) {
            head.Version = 0;    /* currently these fields are both 0 */
            head.Size = 0;
            loc.start = SemStartResource();
            if ( !CmdLineParms.VersionStamp20 ) /* Windows 2.x menus do not have a header */
                error = ResWriteMenuHeader( &head, CurrResFile.fp );
        } else if( tokentype == Y_MENU_EX ) {
            head.Version = RES_HEADER_VERSION;
            head.Size = RES_HEADER_SIZE;
            memset( headerdata, 0, head.Size );
            ResWritePadDWord( CurrResFile.fp );
            loc.start = SemStartResource();
            error = ResWriteMenuExHeader( &head, CurrResFile.fp, headerdata );
        } else {
            loc.start = 0;      // Is this valid?
        }
        if( error ) {
            err_code = LastWresErr();
        } else {
            error = SemWriteSubMenu( menu, &err_code, tokentype );
        }
        if( !error && CmdLineParms.MSResFormat && CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            error = ResWritePadDWord( CurrResFile.fp );
        }
        if( error) {
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_MENU ) ), flags, loc );
        }
    } else {
        RESFREE( name );
    }
    SemFreeSubMenu( menu );
}
