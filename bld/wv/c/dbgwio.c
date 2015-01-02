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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "mempiece.h"
#include "dbgadget.h"
#include "mad.h"
#include "memtypes.h"
#include "dbgitem.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"


extern void             MemInitTypes( mad_type_kind mas, mem_type_walk_data *data );
extern void             MemFiniTypes( mem_type_walk_data *data );
extern bool             DlgMadTypeExpr( char *title, item_mach *value, mad_type_handle th );

extern bool             DlgGivenAddr( char *title, address *value );

#define PIECE_TYPE( x ) ( (x)-MENU_IO_FIRST_TYPE )

static mem_type_walk_data       IOData;
static gui_menu_struct *IOTypeMenu = NULL;
static gui_menu_struct DummyMenu[1];

#include "menudef.h"
static gui_menu_struct IOMenu[] = {
    #include "menuio.h"
};

enum {
    PIECE_READ,
    PIECE_WRITE,
    PIECE_ADDRESS,
    PIECE_VALUE
};

typedef struct {
    item_mach   value;
    address     addr;
    int         type;
    unsigned    value_known     : 1;
} io_location;

typedef struct {
    int         num_rows;
    io_location *list;
} io_window;
#define WndIO( wnd ) ( (io_window*)WndExtra( wnd ) )


static WNDNUMROWS IONumRows;
static int IONumRows( a_window *wnd )
{
    return( WndIO( wnd )->num_rows );
}


static void IOAddNewAddr( a_window *wnd, address *addr, int type )
{
    io_window   *io = WndIO( wnd );
    int         row;
    io_location *curr;

    row = io->num_rows;
    io->num_rows++;
    io->list = WndMustRealloc( io->list, io->num_rows*sizeof( io_location ) );
    curr = &io->list[ row ];
    curr->type = PIECE_TYPE( type );
    curr->addr = *addr;
    curr->value_known = FALSE;
}

static  WNDMENU IOMenuItem;
static void     IOMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    io_window   *io = WndIO( wnd );
    address     addr;
    bool        ok;
    item_mach   item;
    io_location *curr;
    unsigned    old;

    piece=piece;
    if( row < io->num_rows && row >= 0 ) {
        curr = &io->list[ row ];
    } else {
        curr = NULL;
    }
    switch( id ) {
    case MENU_INITIALIZE:
        if( curr == NULL ) {
            WndMenuGrayAll( wnd );
        } else {
            WndMenuEnableAll( wnd );
        }
        WndMenuEnable( wnd, MENU_IO_NEW_ADDRESS, TRUE );
        break;
    case MENU_IO_DELETE:
        io->num_rows--;
        memcpy( &io->list[row], &io->list[ row+1 ],
                ( io->num_rows - row ) * sizeof( io_location ) );
        WndNoSelect( wnd );
        WndRepaint( wnd );
        break;
    case MENU_IO_NEW_ADDRESS:
        addr = NilAddr;
        if( !DlgGivenAddr( LIT( New_Port_Addr ), &addr ) ) return;
        WndRowDirty( wnd, io->num_rows );
        IOAddNewAddr( wnd, &addr, MENU_IO_FIRST_TYPE );
        WndScrollBottom( wnd );
        break;
    case MENU_IO_MODIFY:
        if( row >= io->num_rows || row < 0 ) break;
        if( piece == PIECE_VALUE ) {
            old = NewCurrRadix( IOData.info[ curr->type ].piece_radix );
            item.ud = curr->value_known ? curr->value.ud : 0;
            ok = DlgMadTypeExpr( TxtBuff, &item, IOData.info[ curr->type ].type );
            if( ok ) {
                curr->value = item;
                curr->value_known = TRUE;
            }
            NewCurrRadix( old );
        } else {
            addr = curr->addr;
            if( !DlgGivenAddr( LIT( New_Port_Addr ), &addr ) ) return;
            curr->addr = addr;
            curr->value_known = FALSE;
        }
        WndRowDirty( wnd, row );
        break;
    case MENU_IO_READ:
        curr->value_known = TRUE;
        if( ItemGetMAD( &curr->addr, &curr->value, IT_IO, IOData.info[ curr->type ].type ) == IT_NIL ) {
            curr->value_known = FALSE;
        }
        WndPieceDirty( wnd, row, PIECE_VALUE );
        break;
    case MENU_IO_WRITE:
        if( curr->value_known ) {
            ItemPutMAD( &curr->addr, &curr->value, IT_IO, IOData.info[ curr->type ].type );
        }
        break;
    default:
        curr->type = PIECE_TYPE( id );
        WndZapped( wnd );
        break;
    }
}


static  WNDMODIFY       IOModify;
static void     IOModify( a_window *wnd, int row, int piece )
{
    if( row < 0 ) {
        IOMenuItem( wnd, MENU_IO_NEW_ADDRESS, row, piece );
        return;
    }
    if( row >= IONumRows( wnd ) ) return;
    switch( piece ) {
    case PIECE_READ:
        IOMenuItem( wnd, MENU_IO_READ, row, piece );
        break;
    case PIECE_WRITE:
        IOMenuItem( wnd, MENU_IO_WRITE, row, piece );
        break;
    default:
        WndFirstMenuItem( wnd, row, piece );
        break;
    }
}

static WNDGETLINE IOGetLine;
static  bool    IOGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    io_window   *io = WndIO( wnd );
//    bool        ret;
    io_location *curr;
    int         i;
    unsigned    old, new;
    unsigned    max;

    if( row >= io->num_rows ) return( FALSE );
    curr = &io->list[ row ];
//    ret = TRUE;
    line->text = TxtBuff;
    switch( piece ) {
    case PIECE_READ:
        SetGadgetLine( wnd, line, GADGET_READ );
        return( TRUE );
    case PIECE_WRITE:
        SetGadgetLine( wnd, line, GADGET_WRITE );
        line->indent = MaxGadgetLength;
        return( TRUE );
    case PIECE_ADDRESS:
        AddrToIOString( &curr->addr, TxtBuff, TXT_LEN );
        line->indent = 2*MaxGadgetLength;
        return( TRUE );
    case PIECE_VALUE:
        new = IOData.info[ curr->type ].piece_radix;
        old = NewCurrRadix( new );
        line->indent = 2*MaxGadgetLength + 10 * WndMaxCharX( wnd );
        if( curr->value_known ) {
            max = TXT_LEN;
            MADTypeHandleToString( new, IOData.info[ curr->type ].type, &curr->value, TxtBuff, &max );
        } else {
            for( i = 0; i < IOData.info[ curr->type ].item_width; ++i ) {
                TxtBuff[i] = '?';
            }
            TxtBuff[i] = '\0';
        }
        NewCurrRadix( old );
        return( TRUE );
    default:
        return( FALSE );
    }
}


static WNDREFRESH IORefresh;
static void     IORefresh( a_window *wnd )
{
    WndNoSelect( wnd );
    WndRepaint( wnd );
}


void SetIOMenuItems()
{
    WndEnableMainMenu( MENU_MAIN_OPEN_IO, IOData.num_types != 0 );
}

void InitIOWindow()
{
    int                 i;

    MemInitTypes( MAS_IO | MTK_INTEGER, &IOData );
    if( IOData.num_types == 0 ) {
        return;
    }
    IOTypeMenu = WndMustAlloc( IOData.num_types * sizeof( *IOTypeMenu ) );
    for( i = 0; i < IOData.num_types; ++i ) {
        IOTypeMenu[ i ].id = MENU_IO_FIRST_TYPE + i;
        IOTypeMenu[ i ].style = GUI_ENABLED | WND_MENU_ALLOCATED;
        IOTypeMenu[ i ].label = DupStr( IOData.labels[ i ] );
        IOTypeMenu[ i ].hinttext = DupStr( LIT( Empty ) );
        IOTypeMenu[ i ].num_child_menus = 0;
        IOTypeMenu[ i ].child = NULL;
    }
    for( i = 0; i < ArraySize( IOMenu ); ++i ) {
        if( IOMenu[ i ].id == MENU_IO_TYPE ) {
            IOMenu[ i ].child = IOTypeMenu;
            IOMenu[ i ].num_child_menus = IOData.num_types;
            break;
        }
    }
}

void FiniIOWindow()
{
    WndFree( IOTypeMenu );
    MemFiniTypes( &IOData );
}

static WNDCALLBACK IOEventProc;
static bool IOEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    io_window   *io = WndIO( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        if( io->num_rows != 0 ) {
            IOMenuItem( wnd, MENU_IO_READ, 0, PIECE_VALUE );
        }
        return( TRUE );
    case GUI_DESTROY :
        WndFree( io->list );
        WndFree( io );
        return( TRUE );
    }
    return( FALSE );
}

wnd_info IOInfo = {
    IOEventProc,
    IORefresh,
    IOGetLine,
    IOMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    IOModify,
    IONumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_RADIX_CHANGE,
    DefPopUp( IOMenu )
};

extern void IONewAddr( a_window *wnd, address *addr, int type )
{
    IOAddNewAddr( wnd, addr, type );
    IOMenuItem( wnd, MENU_IO_READ, WndIO( wnd )->num_rows-1, PIECE_VALUE );
    WndRepaint( wnd );
}


extern a_window *DoWndIOOpen( address *addr, mad_type_handle type )
{
    io_window   *io;
    int         i;

    if( IOData.num_types == 0 ) return( NULL );
    io = WndMustAlloc( sizeof( io_window ) );
    io->list = WndMustAlloc( sizeof( io_location ) );
    io->num_rows = 1;
    io->list->addr = *addr;
    io->list->type = PIECE_TYPE( MENU_IO_FIRST_TYPE );
    if( type != MAD_NIL_TYPE_HANDLE ) {
        for( i = 0; i < IOData.num_types; i++ ) {
            if( IOData.info[ i ].type == type ) break;
        }
        if( i != IOData.num_types ) {
            io->list->type = i;
        }
    }
    io->list->value.ud = 0;
    io->list->value_known = FALSE;
    return( DbgWndCreate( LIT( WindowIO_Ports ), &IOInfo, WND_IO, io, &IOIcon ) );
}

extern WNDOPEN WndIOOpen;
extern a_window *WndIOOpen()
{
    io_window   *io;
    a_window    *wnd;

    io = WndMustAlloc( sizeof( io_window ) );
    io->list = NULL;
    io->num_rows = 0;
    wnd = DbgWndCreate( LIT( WindowIO_Ports ), &IOInfo, WND_IO, io, &IOIcon );
    if( wnd != NULL ) WndClrSwitches( wnd, WSW_ONLY_MODIFY_TABSTOP );
    return( wnd );
}
