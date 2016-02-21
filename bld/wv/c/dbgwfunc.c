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
* Description:  The 'Functions' window.
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "madinter.h"
#include "dbgadget.h"
#include "namelist.h"
#include "dbgbrk.h"
#include "wndsys.h"
#include "dbgtrace.h"
#include "dbgmisc.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgdot.h"
#include "dbgwfil.h"
#include "dbgwfunc.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dlgbreak.h"

#include "menudef.h"
static gui_menu_struct FuncMenu[] = {
    #include "menufunc.h"
};

typedef struct {
    name_list           ___n;           // don't reference directly!
    mod_handle          mod;
    gui_ord             max_name;
    bool                is_global     : 1;
    bool                toggled_break : 1;
    bool                d2_only       : 1;
} func_window;

#define WndFunc( wnd ) ( (func_window*)WndExtra( wnd ) )
#define NameList( f ) ( &(f)->___n )

enum {
    PIECE_BREAK,
    PIECE_NAME,
    PIECE_DEMANGLED,
};


static  WNDMODIFY       FuncModify;
static  void    FuncModify( a_window *wnd, int row, int piece )
{
    address     addr;
    func_window *func = WndFunc( wnd );

    if( piece == PIECE_BREAK ) {
        if( row < 0 )
            return;
        if( row >= NameListNumRows( NameList( func ) ) )
            return;
        addr = NameListAddr( NameList( func ), row );
        func->toggled_break = ( ( UpdateFlags & UP_BREAK_CHANGE ) == 0 );
        ToggleBreak( addr );
        WndRowDirty( wnd, row );
    } else {
        WndFirstMenuItem( wnd, row, piece );
    }
}

static void FuncNoMod( a_window *wnd )
{
    func_window *func = WndFunc( wnd );

    WndScrollAbs( wnd, 0 );
    NameListFree( NameList( func ) );
    WndZapped( wnd );
}

static void FuncGetSourceName( a_window *wnd, int row )
{
    func_window *func = WndFunc( wnd );

    NameListName( NameList( func ), row, TxtBuff, SN_QUALIFIED );
}

static int FuncNumRows( a_window *wnd )
{
    return( NameListNumRows( NameList( WndFunc( wnd ) ) ) );
}

static void CalcIndent( a_window *wnd )
{
    gui_ord     len,max;
    int         row,rows;

    rows = FuncNumRows( wnd );
    max = 0;
    for( row = 0; row < rows; ++row ) {
        FuncGetSourceName( wnd, row );
        len = WndExtentX( wnd, TxtBuff );
        if( len > max ) {
            max = len;
        }
    }
    WndFunc( wnd )->max_name = max;
}

static void FuncSetMod( a_window *wnd, mod_handle mod )
{
    func_window *func = WndFunc( wnd );

    func->mod = mod;
    NameListAddModules( NameList( func ), mod, func->d2_only, true );
    CalcIndent( wnd );
}

static void FuncNewOptions( a_window *wnd )
{
    FuncNoMod( wnd );
    FuncSetMod( wnd, WndFunc( wnd )->mod );
    WndZapped( wnd );
}

static void     FuncMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    address     addr;
    func_window *func = WndFunc( wnd );

    piece=piece;
    addr = NilAddr;
    if( row != WND_NO_ROW )
        NameListAddr( NameList( func ), row );
    switch( id ) {
    case MENU_INITIALIZE:
        if( IS_NIL_ADDR( addr ) ) {
            WndMenuGrayAll( wnd );
        } else {
            WndMenuEnableAll( wnd );
            WndMenuEnable( wnd, MENU_FUNCTIONS_SOURCE, HasLineInfo( addr ) );
        }
        WndMenuEnable( wnd, MENU_FUNCTIONS_D2_ONLY, true );
        WndMenuCheck( wnd, MENU_FUNCTIONS_D2_ONLY, func->d2_only );
        break;
    case MENU_FUNCTIONS_SOURCE:
        WndSrcInspect( addr );
        break;
    case MENU_FUNCTIONS_ASSEMBLY:
        WndAsmInspect( addr );
        break;
    case MENU_FUNCTIONS_BREAK:
        DlgBreak( addr );
        break;
    case MENU_FUNCTIONS_D2_ONLY:
        func->d2_only = !func->d2_only;
        FuncNewOptions( wnd );
        break;
    }
}


static WNDGETLINE FuncGetLine;
static  bool    FuncGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    address     addr;
    func_window *func = WndFunc( wnd );

    if( row >= NameListNumRows( NameList( func ) ) )
        return( false );
    addr = NameListAddr( NameList( func ), row );
    switch( piece ) {
    case PIECE_BREAK:
        FileBreakGadget( wnd, line, false, FindBreak( addr ) );
        return( true );
    case PIECE_NAME:
        line->indent = MaxGadgetLength;
        line->text = TxtBuff;
        line->extent = WND_MAX_EXTEND;
        FuncGetSourceName( wnd, row );
        return( true );
    case PIECE_DEMANGLED:
        if( NameListName( NameList( func ), row, TxtBuff, SN_DEMANGLED ) == 0 ) {
            return( false );
        }
        line->text = TxtBuff;
        line->use_prev_attr = true;
        line->indent = MaxGadgetLength + func->max_name + 2 * WndAvgCharX( wnd );
        return( true );
    default:
        return( false );
    }
}

extern  void    FuncNewMod( a_window *wnd, mod_handle mod )
{
    if( WndFunc( wnd )->mod == mod )
        return;
    FuncNoMod( wnd );
    FuncSetMod( wnd, mod );
    WndZapped( wnd );
}


static WNDREFRESH FuncRefresh;
static void FuncRefresh( a_window *wnd )
{
    func_window *func = WndFunc( wnd );
    mod_handle  mod;

    if( UpdateFlags & UP_SYM_CHANGE ) {
        if( func->is_global ) {
            FuncNewMod( wnd, func->mod );
        } else if( DeAliasAddrMod( GetCodeDot(), &mod ) == SR_NONE ) {
            FuncNoMod( wnd );
        } else {
            FuncNewMod( wnd, mod );
        }
    } else if( ( UpdateFlags & UP_CODE_ADDR_CHANGE ) && !func->is_global ) {
        if( DeAliasAddrMod( GetCodeDot(), &mod ) == SR_NONE ) {
            FuncNoMod( wnd );
        } else {
            FuncNewMod( wnd, mod );
        }
    } else if( UpdateFlags & UP_BREAK_CHANGE ) {
        if( func->toggled_break ) {
            func->toggled_break = false;
        } else {
            WndRepaint( wnd );
        }
    }
}

static void FuncSetOptions( a_window *wnd )
{
    func_window *func = WndFunc( wnd );

    func->d2_only = func->is_global && _IsOn( SW_FUNC_D2_ONLY );
    FuncNewOptions( wnd );
}

static WNDCALLBACK FuncEventProc;
static bool FuncEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    func_window *func = WndFunc( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        NameListInit( NameList( func ), WF_CODE );
        func->toggled_break = false;
        FuncSetOptions( wnd );
        WndSetKey( wnd, PIECE_NAME );
        return( true );
    case GUI_RESIZE :
        CalcIndent( wnd );
        WndZapped( wnd );
        break;
    case GUI_DESTROY :
        NameListFree( NameList( func ) );
        WndFree( func );
        return( true );
    }
    return( false );
}

void FuncChangeOptions( void )
{
    WndForAllClass( WND_GBLFUNCTIONS, FuncSetOptions );
}

wnd_info FuncInfo = {
    FuncEventProc,
    FuncRefresh,
    FuncGetLine,
    FuncMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    FuncModify,
    FuncNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_SYM_CHANGE+UP_BREAK_CHANGE+UP_CODE_ADDR_CHANGE,
    DefPopUp( FuncMenu )
};

extern a_window *DoWndFuncOpen( bool is_global, mod_handle mod )
{
    func_window     *func;
    wnd_class_wv    wndclass;
    const char      *title;

    func = WndMustAlloc( sizeof( func_window ) );
    func->mod = mod;
    if( is_global ) {
        wndclass = WND_GBLFUNCTIONS;
        func->is_global = true;
    } else {
        wndclass = WND_FUNCTIONS;
        func->is_global = false;
    }
    if( is_global ) {
        title = LIT_DUI( WindowGlobal_Functions );
    } else {
        title = LIT_DUI( WindowFunctions );
    }
    return( DbgWndCreate( title, &FuncInfo, wndclass, func, &FuncIcon ) );
}

extern a_window *WndFuncOpen( void )
{
    return( DoWndFuncOpen( false, NO_MOD ) );
}

extern a_window *WndGblFuncOpen( void )
{
    return( DoWndFuncOpen( true, NO_MOD ) );
}
