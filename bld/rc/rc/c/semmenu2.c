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
* Description:  OS/2 Menu related semantic actions.
*
****************************************************************************/


/*

The data structure for OS/2 MENU resources is as follows:

struct MenuHeader {
    ULONG  cbSize;          // Size of menu/submenu including this field
    USHORT idCodePage;      // Menu codepage
    USHORT idClass;         // Window class ID (always WC_MENU?)
    USHORT cItems;          // Number of menu items that follow
}

struct MenuItem {
    USHORT flStyle;         // Menu style flags
    USHORT fAttributes;     // Menu attribute flags
    USHORT cmd;             // Menu item ID
    CHAR   szItemName[];    // Null-terminated menu text (none for separator)
}

*/


#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "semantc2.h"
#include "layer0.h"
#include "rcrtns.h"
#include "reserr.h"
#include "clibext.h"
#include "rccore.h"


static bool ResOS2WriteMenuHeader( MenuHeaderOS2 *currhead, WResFileID handle )
/*********************************************************************/
{
    if( RCWRITE( handle, currhead, sizeof( MenuHeaderOS2 ) ) != sizeof( MenuHeaderOS2 ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

static bool ResOS2WriteMenuItemNormal( const MenuItemOS2 *curritem, WResFileID handle )
/*****************************************************************************/
{
    bool        error;
    uint_16     tmp16;

    tmp16 = curritem->ItemStyle;
    error = ResWriteUint16( &tmp16, handle );
    if( !error ) {
        tmp16 = curritem->ItemAttrs;
        error = ResWriteUint16( &tmp16, handle );
    }
    if( !error ) {
        tmp16 = curritem->ItemCmd;
        error = ResWriteUint16( &tmp16, handle );
    }
    if( !error ) {
        if( !(curritem->ItemStyle & OS2_MIS_SEPARATOR) && curritem->ItemText != NULL )
            error = ResWriteString( curritem->ItemText, false, handle );
    }

    return( error );
}

static void SemOS2FreeSubMenu( FullMenuOS2 *submenu );

MenuFlags SemOS2AddFirstMenuOption( YYTOKENTYPE token )
/*****************************************************/
{
    return( SemOS2AddMenuOption( 0, token ) );
}

MenuFlags SemOS2AddMenuOption( MenuFlags oldflags, YYTOKENTYPE token )
/********************************************************************/
{
    switch( token ) {
    case Y_BITMAP:
        oldflags |= MENU_BITMAP;
        break;
    }

    return( oldflags );
}

FullMenuOS2 *SemOS2NewMenu( FullMenuItemOS2 firstitem )
/*****************************************************/
{
    FullMenuOS2       *newmenu;
    FullMenuItemOS2   *newitem;

    newmenu = RCALLOC( sizeof( FullMenuOS2 ) );
    newitem = RCALLOC( sizeof( FullMenuItemOS2 ) );

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

FullMenuOS2 *SemOS2AddMenuItem( FullMenuOS2 *currmenu, FullMenuItemOS2 curritem )
/*******************************************************************************/
{
    FullMenuItemOS2     *newitem;

    newitem = RCALLOC( sizeof( FullMenuItemOS2 ) );

    if( newitem == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newitem = curritem;

    ResAddLLItemAtEnd( (void **)&(currmenu->head), (void **)&(currmenu->tail), newitem );

    return( currmenu );
}

static bool SemOS2WriteMenuItem( FullMenuItemOS2 *item, int *err_code )
/*********************************************************************/
{
    bool    error;

    error = ResOS2WriteMenuItemNormal( &(item->item), CurrResFile.handle );
    *err_code = LastWresErr();
    return( error );
}

static int SemOS2CalcSubMenuSize( FullMenuOS2 *submenu, int *count )
/******************************************************************/
{
    FullMenuItemOS2 *curritem;
    int             size = 0, dummycount;

    size += sizeof( MenuHeaderOS2 );

    if( submenu == NULL ) {
        return( size );
    }

    for( curritem = submenu->head; curritem != NULL; curritem = curritem->next ) {
        *count += 1;
        size += 3 * sizeof( uint_16 );
        if( !(curritem->item.ItemStyle & OS2_MIS_SEPARATOR) &&
            curritem->item.ItemText != NULL )
            size += strlen( curritem->item.ItemText ) + 1;
        if( curritem->item.ItemStyle & OS2_MIS_SUBMENU ) {
            size += SemOS2CalcSubMenuSize( curritem->submenu, &dummycount );
        }
    }
    return( size );
}


static bool SemOS2WriteSubMenu( FullMenuOS2 *submenu, int *err_code,
                               uint_32 codepage )
/******************************************************************/
{
    int             count = 0;
    bool            error;
    FullMenuItemOS2 *curritem;
    MenuHeaderOS2   head;

    if( ErrorHasOccured ) {
        return( false );
    }

    head.Size     = SemOS2CalcSubMenuSize( submenu, &count );
    head.Codepage = codepage;
    head.Class    = 4;
    head.NumItems = count;

    error = ResOS2WriteMenuHeader( &head, CurrResFile.handle );
    if( error ) {
        ErrorHasOccured = true;
        return( error );
    }

    if( submenu == NULL ) {
        return( error );
    }

    error = false;
    for( curritem = submenu->head; curritem != NULL && !error; curritem = curritem->next ) {
        if( !ErrorHasOccured ) {
            error = SemOS2WriteMenuItem( curritem, err_code );
            if( !error && (curritem->item.ItemStyle & OS2_MIS_SUBMENU) ) {
                error = SemOS2WriteSubMenu( curritem->submenu, err_code, codepage );
            }
        }
    }

    if( error ) {
        ErrorHasOccured = true;
    }
    return( error );
}

static void SemOS2FreeMenuItem( FullMenuItemOS2 *curritem )
/*********************************************************/
{
    if( curritem->submenu != NULL ) {
        SemOS2FreeSubMenu( curritem->submenu );
        if( curritem->item.ItemText != NULL ) {
            RCFREE( curritem->item.ItemText );
        }
    } else {
        if( curritem->item.ItemText != NULL ) {
            RCFREE( curritem->item.ItemText );
        }
    }
}

static void SemOS2FreeSubMenu( FullMenuOS2 *submenu )
/***************************************************/
{
    FullMenuItemOS2   *curritem;
    FullMenuItemOS2   *olditem;

    if( submenu != NULL ) {
        curritem = submenu->head;
        while( curritem != NULL ) {
            SemOS2FreeMenuItem( curritem );
            olditem = curritem;
            curritem = curritem->next;
            RCFREE( olditem );
        }

        RCFREE( submenu );
    }
}

void SemOS2WriteMenu( WResID *name, ResMemFlags flags, FullMenuOS2 *menu,
                   YYTOKENTYPE tokentype, uint_32 codepage )
/***********************************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    tokentype = tokentype;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        error = SemOS2WriteSubMenu( menu, &err_code, codepage );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( OS2_RT_MENU ), flags, loc );
    } else {
        RCFREE( name );
    }

    SemOS2FreeSubMenu( menu );
    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
    ErrorHasOccured = true;
    SemOS2FreeSubMenu( menu );
    return;
}
