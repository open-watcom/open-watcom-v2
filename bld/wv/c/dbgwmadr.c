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
* Description:  Register window layout and sizing.
*
****************************************************************************/


#include <string.h>
#include "dbgdefn.h"
#include "dbgwind.h"
#include "dbgreg.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgitem.h"
#include "madcli.h"
#include "mad.h"

extern char             *DupStr( char *str );
extern address          AddrRegIP( machine_state *regs );
extern unsigned         GetInsSize( address addr );
extern unsigned         NewCurrRadix( unsigned );
extern void             WndInspect( char *item );
extern char             *GetCmdName( int index );
extern char             *Format( char *buff, char *fmt, ... );
extern void             RecordEvent( char *p );
extern void             GetMADTypeDefault( mad_type_kind, mad_type_handle );
extern void             PushAddr( address val );
extern bool             DlgMadTypeExpr( char *title, item_mach *value, mad_type_handle );
extern gui_menu_struct *WndAppendToggles( mad_toggle_strings const *toggles, unsigned *pnum_toggles,
                                   gui_menu_struct *old, unsigned num_old, int id );
extern void             WndDeleteToggles( gui_menu_struct *popup, unsigned num_old, unsigned num_toggles );
extern unsigned         GetMADMaxFormatWidth( mad_type_handle th );
extern void             WndInspectExprSP( char *item );
extern void             RegFindData( mad_type_kind kind, mad_reg_set_data **pdata );
extern void             RegValue( item_mach *value, const mad_reg_info *reginfo, machine_state *mach );
extern void             RegNewValue( const mad_reg_info *, item_mach const *, mad_type_handle  );

extern machine_state    *DbgRegs;
extern machine_state    *PrevRegs;
extern char             *TxtBuff;
extern unsigned char    CurrRadix;

#include "menudef.h"
static gui_menu_struct RegMenu[] = {
    #include "menureg.h"
};

typedef struct {
        char                    standout;
        mad_reg_info  const     *info;
        gui_ord                 max_extent;
        gui_ord                 max_descript;
        char                    max_value;
} a_reg_info;

typedef struct {
    gui_ord     descript;
    gui_ord     value;
} reg_indent;

typedef struct {
        mad_reg_set_data        *data;
        char                    up;
        char                    rows;
        char                    count;
        a_reg_info              *info;
        reg_indent              *indents;
        gui_menu_struct         *popup;
        unsigned                num_toggles;
        mad_type_kind           kind;
} reg_window;
#define WndReg( wnd ) ( (reg_window*)WndExtra( wnd ) )


typedef struct {
    char                *descript;
    unsigned            max_descript;
    const mad_reg_info  *reginfo;
    mad_type_handle     disp_type;
    unsigned            max_value;
} reg_display_piece;

static bool GetDisplayPiece( reg_display_piece *disp, reg_window *reg, machine_state *mach, int i )
{
    return( MADRegSetDisplayGetPiece( reg->data, &mach->mr, i, &disp->descript,
                                  &disp->max_descript, &disp->reginfo,
                                  &disp->disp_type, &disp->max_value ) == MS_OK );
}

static bool RegResize( a_window *wnd )
{
    reg_window          *reg = WndReg( wnd );
    gui_ord             space;
    int                 old_up;
    int                 i,j;
    reg_display_piece   disp;
    gui_ord             max_extent;
    gui_ord             max_descript;
    a_reg_info          *info;
    gui_ord             indent;
    gui_ord             value,descript;
    char                *p;
    unsigned            len;

    old_up = reg->up;
    reg->up = 1;

    RegFindData( reg->kind, &reg->data );
    reg->count = 0;
    for( ;; ) {
        if( !GetDisplayPiece( &disp, reg, DbgRegs, reg->count ) ) break;
        reg->count++;
    }

    WndFree( reg->info );
    reg->info = WndMustAlloc( reg->count * sizeof( *reg->info ) );
    space = WndAvgCharX( wnd );

    max_extent = 0;
    max_descript = 0;
    for( i = 0; i < reg->count; ++i ) {
        GetDisplayPiece( &disp, reg, DbgRegs, i );
        if( disp.max_value == 0 && disp.reginfo != NULL ) {
            disp.max_value = GetMADMaxFormatWidth( disp.disp_type );
        }
        info = &reg->info[i];
        info->max_value = disp.max_value;
        info->info = disp.reginfo;
        if( disp.max_descript > strlen( disp.descript ) ) {
            info->max_descript = space + disp.max_descript * WndAvgCharX( wnd );
        } else {
            info->max_descript = space + WndExtentX( wnd, disp.descript );
        }
        info->max_extent = space + disp.max_value * WndAvgCharX( wnd );
        info->standout = FALSE;
        if( info->max_extent > max_extent ) {
            max_extent = info->max_extent;
        }
        if( info->max_descript > max_descript ) {
            max_descript = info->max_descript;
        }
    }

    reg->up = MADRegSetDisplayGrouping( reg->data );
    if( reg->up == 0 ) {
        reg->up = WndWidth( wnd ) / ( max_extent + max_descript );
        if( reg->up < 1 ) reg->up = 1;
        if( reg->up > reg->count ) reg->up = reg->count;
    }
    reg->rows = ( reg->count + reg->up - 1 ) / reg->up;

    // calculate the indents

    WndFree( reg->indents );
    reg->indents = WndMustAlloc( reg->count * sizeof( *reg->indents ) );

    // For each column
    for( i = 0; i < reg->up; ++i ) {
        reg->indents[i].descript = 0;
        reg->indents[i].value = 0;
        // Calc max widths for column
        for( j = i; j < reg->count; j += reg->up ) {
            if( reg->info[j].max_extent > reg->indents[i].value ) {
                reg->indents[i].value = reg->info[j].max_extent;
            }
            if( reg->info[j].max_descript > reg->indents[i].descript ) {
                reg->indents[i].descript = reg->info[j].max_descript;
            }
        }
    }
    // Calc indents for each column
    indent = 0;
    value = 0;
    descript = 0;
    // For each column
    for( i = 0; i < reg->up; ++i ) {
        value = reg->indents[i].value;
        descript = reg->indents[i].descript;
        reg->indents[i].descript = indent;
        reg->indents[i].value = indent + descript;
        indent += value + descript;
#if ( defined( __GUI__ ) && defined( __OS2__ ) )
        // OS/2 PM GUI needs more space between columns
        indent += space;
#endif
    }
    // Copy indents to all registers by column
    for( i = reg->up; i < reg->count; ++i ) {
        reg->indents[i] = reg->indents[i % reg->up];
    }

    if( reg->up != old_up ) {
        WndScrollAbs( wnd, 0 );
        WndNoCurrent( wnd );
    }

    p = TxtBuff + MADCliString( MADRegSetName( reg->data ), TXT_LEN, TxtBuff );
    *p++ = ' ';
    *p++ = '(';
    len = MADRegSetLevel( reg->data, TXT_LEN - ( p - TxtBuff ), p );
    if( len == 0 ) {
        p -= 2;
    } else {
        p += len;
        *p++ = ')';
    }
    *p++ = '\0';
    WndSetTitle( wnd, TxtBuff );

    return( TRUE );
}


static WNDNUMROWS RegNumRows;
static int RegNumRows( a_window *wnd )
{
    return( WndReg( wnd )->rows );
}


static int GetRegIdx( reg_window *reg, int row, int piece )
{
    int         i;

    if( row == WND_NO_ROW ) return( -1 );
    i = row * reg->up + piece;
    if( i >= reg->count ) i = -1;
    return( i );
}


static DLGPICKTEXT RegValueName;
static char *RegValueName( void *_possible, int i )
{
    mad_modify_list    *possible = _possible;
    unsigned    max;

    if( possible[i].name == MSTR_NIL ) {
        max = TXT_LEN;
        MADTypeHandleToString( MADTypePreferredRadix( possible[i].type ),
                possible[i].type, possible[i].data, &max, TxtBuff );
    } else {
        MADCliString( possible[i].name, TXT_LEN, TxtBuff );
    }
    return( TxtBuff );
}

static  WNDMODIFY RegModify;
static  void    RegModify( a_window *wnd, int row, int piece )
{
    int                 i;
    item_mach           value;
    reg_window          *reg = WndReg( wnd );
    bool                ok;
    unsigned            old;
    reg_display_piece   disp;
    mad_type_info       tinfo;
    mad_modify_list const    *possible;
    unsigned            num_possible;

    if( row < 0 ) return;
    piece >>= 1;
    i = GetRegIdx( reg, row, piece );
    if( i == -1 ) return;
    if( !GetDisplayPiece( &disp, reg, DbgRegs, i ) ) return;
    if( disp.reginfo == NULL ) return;
    if( MADRegSetDisplayModify( reg->data, disp.reginfo, &possible, &num_possible ) != MS_OK ) return;
    old = NewCurrRadix( MADTypePreferredRadix( disp.disp_type ) );
    MADRegFullName( disp.reginfo, ".", TXT_LEN, TxtBuff );
    RegValue( &value, disp.reginfo, DbgRegs );
    if( num_possible == 1 ) {
        ok = DlgMadTypeExpr( TxtBuff, &value, disp.disp_type );
        if( ok ) {
            RegNewValue( disp.reginfo, &value, possible->type );
        }
    } else {
        for( i = 0; i < num_possible; ++i ) {
            MADTypeInfo( possible[i].type, &tinfo );
            if( memcmp( &value, possible[i].data, tinfo.b.bits / BITS_PER_BYTE ) == 0 ) break;
        }
        if( num_possible == 2 ) {
            if( i == 0 ) {
                i = 1;
            } else {
                i = 0;
            }
        } else {  //MJC const cast
            i = DlgPickWithRtn( TxtBuff, (mad_modify_list*) possible, i, RegValueName, num_possible );
        }
        if( i != -1 ) {
            RegNewValue( disp.reginfo, possible[i].data, possible[i].type );
        }
    }
    NewCurrRadix( old );
}

static  WNDMENU RegMenuItem;
static void     RegMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    reg_window          *reg = WndReg( wnd );
    int                 i;
    unsigned            bit;
    mad_modify_list const *possible;
    unsigned            num_possible;
    address             addr;
    bool                valid_reg;

    i = GetRegIdx( reg, row, piece >>1 );
    switch( id ) {
    case MENU_INITIALIZE:
        valid_reg = i != -1 && reg->info[i].info != NULL &&
                    MADRegSetDisplayModify( reg->data, reg->info[i].info,
                                            &possible, &num_possible ) == MS_OK;
        WndMenuEnable( wnd, MENU_REGISTER_MODIFY, valid_reg );
        WndMenuEnable( wnd, MENU_REGISTER_INSPECT, valid_reg );
        bit = MADRegSetDisplayToggle( reg->data, 0, 0 );
        for( i = 0; i < reg->num_toggles; ++i ) {
            WndMenuCheck( wnd, MENU_REGISTER_TOGGLES + i, ( bit & 1 ) != 0 );
            bit >>= 1;
        }
        break;
    case MENU_REGISTER_INSPECT:
        if( MADRegInspectAddr( reg->info[ i ].info, &DbgRegs->mr, &addr ) == MS_OK ) {
            PushAddr( addr );
            WndInspectExprSP( "" );
        }
        break;
    case MENU_REGISTER_MODIFY:
        RegModify( wnd, row, piece );
        break;
    default:
        bit = 1 << ( id - MENU_REGISTER_TOGGLES );
        MADRegSetDisplayToggle( reg->data, bit, bit );
        RegResize( wnd );
        WndZapped( wnd );
    }
}


static WNDGETLINE RegGetLine;
static  bool    RegGetLine( a_window *wnd, int row, int piece,
                            wnd_line_piece *line )
{
    int                 column;
    int                 i;
    reg_window          *reg = WndReg( wnd );
    unsigned            max = TXT_LEN;
    unsigned            old,new;
    item_mach           value;
    reg_display_piece   disp;

    column = piece >> 1;
    if( column >= reg->up ) return( FALSE );
    i = GetRegIdx( reg, row, column );
    if( i >= reg->count ) return( FALSE );
    if( i == -1 ) return( FALSE );
    if( !GetDisplayPiece( &disp, reg, DbgRegs, i ) ) return( FALSE );
    line->text = TxtBuff;
    if( piece & 1 ) {
        line->indent = reg->indents[column].value;
        if( reg->info[i].info == NULL ) {
            strcpy( TxtBuff, "   " );
        } else {
            new = MADTypePreferredRadix( disp.disp_type );
            old = NewCurrRadix( new );
            RegValue( &value, reg->info[i].info, DbgRegs );
            max = reg->info[i].max_value + 1;
            MADTypeHandleToString( new, disp.disp_type, &value, &max, TxtBuff );
            NewCurrRadix( old );
            reg->info[i].standout = FALSE;
            if( MADRegModified( reg->data, reg->info[i].info, &PrevRegs->mr, &DbgRegs->mr ) == MS_MODIFIED_SIGNIFICANTLY ) {
                reg->info[i].standout = TRUE;
                line->attr = WND_STANDOUT;
            }
        }
    } else {
        line->indent = reg->indents[column].descript;
        strcpy( TxtBuff, disp.descript );
        if( TxtBuff[0] != '\0' ) {
            strcat( TxtBuff, ":" );
        }
        line->tabstop = FALSE;
    }
    return( TRUE );
}


static WNDREFRESH RegRefresh;
static void     RegRefresh( a_window *wnd )
{
    int                 row,rows;
    int                 reg_num;
    int                 i;
    reg_window          *reg = WndReg( wnd );
    reg_display_piece   disp;

    // if register type changes, from 16 to 32-bit,
    // this will force the mad to call back NOW, setting UP_REG_RESIZE
    GetDisplayPiece( &disp, reg, DbgRegs, 0 );
    if( WndFlags & UP_REG_RESIZE ) {
        RegResize( wnd );
        WndZapped( wnd );
        return;
    }
    if( WndFlags & UP_MAD_CHANGE ) {
        WndZapped( wnd );
        return;
    }
    rows = RegNumRows( wnd );
    for( row = 0; row < rows; ++row ) {
        for( reg_num = 0; reg_num < reg->up; ++reg_num ) {
            i = GetRegIdx( reg, row, reg_num );
            if( i == -1 ) break;
            if( reg->info[i].standout || ( reg->info[i].info != NULL &&
                MADRegModified( reg->data, reg->info[i].info, &PrevRegs->mr, &DbgRegs->mr ) != MS_OK ) ) {
                WndPieceDirty( wnd, row, reg_num*2+1 );
            }
        }
    }
}

static WNDCALLBACK RegEventProc;
static bool RegEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    reg_window          *reg = WndReg( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_RESIZE:
        if( RegResize( wnd ) ) {
            WndZapped( wnd );
        }
        return( TRUE );
    case GUI_INIT_WINDOW:
        reg->info = NULL;
        reg->indents = NULL;
        RegResize( wnd );
        reg->popup = WndAppendToggles( MADRegSetDisplayToggleList( reg->data ), &reg->num_toggles, RegMenu, ArraySize( RegMenu ), MENU_REGISTER_TOGGLES );
        WndSetPopUpMenu( wnd, reg->popup, ArraySize( RegMenu ) + reg->num_toggles );
        return( TRUE );
    case GUI_DESTROY :
        WndDeleteToggles( reg->popup, ArraySize( RegMenu ), reg->num_toggles );
        WndFree( reg->info );
        WndFree( reg->indents );
        WndFree( reg );
        return( TRUE );
    }
    return( FALSE );
}

wnd_info MadRegInfo = {
    RegEventProc,
    RegRefresh,
    RegGetLine,
    RegMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    RegNumRows,
    NoNextRow,
    NoNotify,
    UP_MAD_CHANGE+UP_REG_CHANGE+UP_REG_RESIZE,
    DefPopUp( RegMenu )
};

void MadRegChangeOptions( a_window *wnd )
{
    RegResize( wnd );
    WndZapped( wnd );
}

extern a_window *WndMadRegOpen( mad_type_kind kind, wnd_class class, gui_resource *icon )
{
    reg_window  *reg;
    a_window    *wnd;

    reg = WndMustAlloc( sizeof( reg_window ) );
    reg->kind = kind;
    wnd = DbgWndCreate( LIT( Empty ), &MadRegInfo, class, reg, icon );
    if( wnd == NULL ) return( NULL );
    return( wnd );
}
