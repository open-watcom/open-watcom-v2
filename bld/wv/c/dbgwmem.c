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


#include <ctype.h>
#include <float.h>
#include <limits.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgitem.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "ldsupp.h"
#include "memtypes.h"
#include "strutil.h"
#include "dbgscan.h"
#include "madinter.h"
#include "dbgutil.h"
#include "dbgmemor.h"
#include "dbgbrk.h"
#include "dbgdot.h"
#include "wndsys.h"
#include "remcore.h"
#include "addarith.h"
#include "dlgscan.h"
#include "dlgexpr.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dbgmad.h"
#include "dbgwmem.h"


extern bool             DlgDataAddrFormat( char *, void *, void (*fmt)(void*,char*));
extern void             MemFiniTypes( mem_type_walk_data *data );
extern void             MemInitTypes( mad_type_kind mas, mem_type_walk_data *data );


typedef gui_ord (MEMHEADER)(a_window *,int);

#define TITLE_SIZE      1


static mem_type_walk_data       MemData;
static gui_menu_struct *MemTypeMenu = NULL;
static gui_menu_struct DummyMenu[1];

#include "menudef.h"
static gui_menu_struct MemMenu[] = {
    #include "menumem.h"
};

#define PIECE_TYPE( x ) ( (x)-MENU_MEMORY_FIRST_TYPE )

static unsigned         MemByteType;

static  MEMHEADER       MemHeader;
static  MEMHEADER       BinHeader;

static MEMHEADER *HeadTab[] =
{
    MemHeader,
    BinHeader
};

typedef struct mem_backout {
    struct mem_backout *next;
    address             home;
    address             addr;
    unsigned            curr_offset;
    unsigned            total_size;
    char                *follow;
    bool                has_current : 1;
} mem_backout;

typedef struct mem_window {
    union {
        struct {
            address     addr;  // this is the current top of window address
            address     home;  // this is the first address looked at
            void        *contents;
            int         size;
            char        *follow;
            mem_backout *backout;
        } m;
        struct {
            file_handle     fh;
            unsigned long   offset;
            unsigned long   size;
        } f;
    } u;
    unsigned    curr_offset;
    unsigned    items_per_line;
    unsigned    item_size;
    unsigned    item_width;
    unsigned    total_size;
    int         piece_type;
    int         last_type_popup;
    gui_ord     address_end;
    long        bp_offset;
    long        sp_offset;
    wnd_row     cursor_row;
    int         cursor_piece;
    int         shadow_piece;
    mad_type_handle init_type;      //MAD: what if active MAD changes?
    bool        file    : 1;
    bool        stack   : 1;
} mem_window;
#define WndMem( wnd ) ( (mem_window *)WndExtra( wnd ) )

static void MemValidAddr( address addr )
{
    char        ch;

    if( _IsOff( SW_RUNNING_PROFILE ) ) {
        if( ProgPeekWrap( addr, &ch, 1 ) != 1 ) {
            Error( ERR_NONE, LIT_ENG( ERR_NO_READ_MEM ), addr );
        }
    }
}

static char *MemGetTitle( mem_window *mem )
{
    char        *p;

    p = StrCopy( LIT_DUI( WindowMemory ), TxtBuff );
    p = StrCopy( " (", p );
    p = AddrToString( &mem->u.m.home, MAF_FULL, p, TXT_LEN - ( p - TxtBuff ) );
    p = StrCopy( ")", p );
    return( TxtBuff );
}

static unsigned MemCurrOffset( a_window *wnd )
{
    wnd_row     curr_row;
    int         curr_piece;
    mem_window  *mem = WndMem( wnd );

    WndGetCurrent( wnd, &curr_row, &curr_piece );
    if( curr_row < 0 )
        return( 0 );
    --curr_piece;
    if( curr_piece >= mem->items_per_line )
        curr_piece -= mem->items_per_line;
    return( ( curr_row * mem->items_per_line + curr_piece ) * mem->item_size );
}

static gui_ord MemHeader( a_window *wnd, int piece )
{
    address     addr;
    mem_window  *mem = WndMem( wnd );
    gui_ord     len;
    char        buff[TXT_LEN];

    if( mem->stack && piece > 0 )
        return( -1 );
    if( mem->file ) {
        if( piece > 0 )
            return( -1 );
        CnvULong( MemCurrOffset( wnd ), TxtBuff, TXT_LEN );
        return( 0 );
    }
    if( piece > 1 )
        return( -1 );
    addr = AddrAddWrap( mem->u.m.addr, MemCurrOffset( wnd ) );
    AddrToString( &addr, MAF_FULL, buff, sizeof( buff ) );
    switch( piece ) {
    case 0:
        StrCopy( buff, TxtBuff );
        return( 0 );
    case 1:
        StrAddr( &addr, TxtBuff, TXT_LEN );
        if( strcmp( buff, TxtBuff ) == 0 )
            break;
        len = WndExtentX( wnd, buff ) + 2*WndMidCharX( wnd );
        return( len );
    }
    return( -1 );
}


static gui_ord BinHeader( a_window *wnd, int piece )
{
    mem_window  *mem = WndMem( wnd );

    if( piece != 0 )
        return( -1 );
    CnvULongDec( mem->u.f.offset + MemCurrOffset( wnd ), TxtBuff, TXT_LEN );
    return( 0 );
}

static void MemGetContents( a_window *wnd, bool make_dirty )
{
    int         size;
    mem_window  *mem = WndMem( wnd );
    void        *old;
    int         old_size;

    old = mem->u.m.contents;
    old_size = mem->u.m.size;
    size = mem->items_per_line * mem->item_size * WndRows( wnd );
    mem->u.m.contents = WndAlloc( size );
    if( mem->u.m.contents == NULL ) {
        mem->u.m.size = 0;
    } else {
        mem->u.m.size = ProgPeekWrap( mem->u.m.addr, mem->u.m.contents, size );
    }
    AddrToString( &mem->u.m.addr, MAF_OFFSET, TxtBuff, TXT_LEN );
    mem->address_end = ( strlen( TxtBuff ) + 1 ) * WndMidCharX( wnd );
    if( mem->stack ) {
        mem->sp_offset = AddrDiff( Context.stack, mem->u.m.addr );
        mem->bp_offset = AddrDiff( Context.frame, mem->u.m.addr );
    }
    if( make_dirty ) {
        WndNoSelect( wnd );
        if( mem->u.m.size != old_size ||
            old == NULL ||
            mem->u.m.contents == NULL ) {
            WndRepaint( wnd );
        } else {
            for( size = 0; size < old_size; ++size ) {
                if( ((char*)old)[size] != ((char*)mem->u.m.contents)[size] ) {
                    WndRowDirty( wnd, size / ( mem->items_per_line * mem->item_size ) );
                }
            }
        }
    }
    WndFree( old );
}

static void MemSetStartAddr( a_window *wnd, address addr, bool new_home )
{
    mem_window  *mem = WndMem( wnd );

    if( new_home )
        mem->u.m.home = addr;
    mem->u.m.addr = addr;
    MemGetContents( wnd, false );
}

static  void MemRefresh( a_window *wnd )
{
    mem_window  *mem = WndMem( wnd );

    if( !mem->file ) {
        MemGetContents( wnd, true );
    } else {
        CnvULong( ULONG_MAX, TxtBuff, TXT_LEN );
        mem->address_end = ( strlen( TxtBuff ) + 1 ) * WndMidCharX( wnd );
    }
    if( !WndHasCurrent( wnd ) ) {
        WndFirstCurrent( wnd );
    }
}

static void MemGetNewAddr( a_window *wnd )
{
    address         addr;
    unsigned long   offset;
    mem_window      *mem;
    mad_radix       old_radix;

    mem = WndMem( wnd );
    if( mem->file ) {
        bool    rc;

        offset = mem->u.f.offset;
        old_radix = NewCurrRadix( 10 );
        rc = DlgLongExpr( LIT_DUI( New_Offset ), (long *)&offset );
        NewCurrRadix( old_radix );
        if( !rc )
            return;
        if( offset > mem->u.f.size )
            return;
        mem->u.f.offset = offset;
    } else {
        addr = mem->u.m.home;
        if( !DlgDataAddr( LIT_DUI( New_Addr ), &addr ) )
            return;
        MemValidAddr( addr );
        MemSetStartAddr( wnd, addr, true );
        WndNoCurrent( wnd );
        MemRefresh( wnd );
        SetDataDot( addr );
    }
    WndSetTitle( wnd, MemGetTitle( mem ) );
    WndZapped( wnd );
}


extern void MemSetLength( a_window *wnd, unsigned size )
{
    WndMem( wnd )->total_size = size;
}


extern void MemSetFollow( a_window *wnd, char *follow )
{
    mem_window  *mem = WndMem( wnd );

    WndFree( mem->u.m.follow );
    mem->u.m.follow = follow;
}


static  void    MemSetType( a_window *wnd, unsigned idx )
{
    mem_window  *mem = WndMem( wnd );

    mem->piece_type = idx;
    if( mem->stack ) {
        mem->items_per_line = 1;
    } else {
        mem->items_per_line = MemData.info[mem->piece_type].items_per_line;
    }
    mem->item_size = MemData.info[mem->piece_type].item_size;
    mem->item_width = MemData.info[mem->piece_type].item_width;
}


static  bool    CanModify( a_window *wnd, int row, int piece )
{
    piece = piece;
    if( row < 0 )
        return( false );
    if( WndMem( wnd )->file )
        return( false );
    return( true );
}

static void SetNewDataDot( a_window *wnd )
{
    mem_window  *mem = WndMem( wnd );

    if( !mem->file ) {
        SetDataDot( AddrAddWrap( mem->u.m.addr, MemCurrOffset( wnd ) ) );
    }
}

static  void    MemUpdateCursor( a_window *wnd )
{
    mem_window  *mem = WndMem( wnd );
    wnd_row     cursor_row;
    int         cursor_piece;
    int         shadow_piece;

    WndGetCurrent( wnd, &cursor_row, &cursor_piece );
    if( cursor_piece > mem->items_per_line ) {
        shadow_piece = cursor_piece - mem->items_per_line;
    } else {
        shadow_piece = cursor_piece + mem->items_per_line;
    }

    if( cursor_row != WND_NO_ROW ) {
        WndRowDirty( wnd, -TITLE_SIZE );
    }
    if( cursor_row == mem->cursor_row && cursor_piece == mem->cursor_piece )
        return;
    if( mem->piece_type == MemByteType ) {
        if( mem->cursor_row != WND_NO_ROW ) {
            WndPieceDirty( wnd, mem->cursor_row, mem->shadow_piece );
        }
        if( cursor_row != WND_NO_ROW ) {
            WndPieceDirty( wnd, cursor_row, shadow_piece );
        }
    }
    SetNewDataDot( wnd );
    mem->cursor_row = cursor_row;
    mem->cursor_piece = cursor_piece;
    mem->shadow_piece = shadow_piece;
}


static  void    MemModify( a_window *wnd, int row, int piece )
{
    address     addr;
    union {
        item_mach i;
        char      str[TXT_LEN];
    } item;
    bool        is_char;
    mad_radix   old_radix;
    unsigned    item_size;
    mem_window  *mem = WndMem( wnd );

    --piece;
    if( !CanModify( wnd, row, piece ) )
        return;
    if( piece >= mem->items_per_line ) {
        piece -= mem->items_per_line;
        is_char = true;
    } else {
        is_char = false;
    }
    addr = AddrAddWrap( mem->u.m.addr,
                           ( row * mem->items_per_line + piece ) * mem->item_size );
    StrAddr( &addr, TxtBuff, TXT_LEN );
    ProgPeekWrap( addr, (void *)&item, mem->item_size );
    old_radix = NewCurrRadix( MemData.info[mem->piece_type].piece_radix );
    item_size = mem->item_size;
    if( is_char ) {
        if( DlgString( TxtBuff, item.str ) ) {
            ChangeMemUndoable( addr, &item, strlen( item.str ) );
        }
    } else {
        if( DlgMadTypeExpr( TxtBuff, &item.i, MemData.info[mem->piece_type].type ) ) {
            ChangeMemUndoable( addr, &item, item_size );
        }
    }
    NewCurrRadix( old_radix );
}


static bool MemScrollBytes( a_window *wnd, int tomove, bool new_home )
{
    address     addr;
    char        ch;
    mem_window  *mem = WndMem( wnd );

    addr = AddrAddWrap( mem->u.m.addr, tomove );
    if( ProgPeekWrap( addr, &ch, 1 ) != 1 )
        return( false );
    MemSetStartAddr( wnd, addr, new_home );
    SetNewDataDot( wnd );
//    SetDataDot( addr );
    WndFixedThumb( wnd );
    return( true );
}

static void MemFreeBackout( mem_window *mem )
{
    mem_backout *junk;

    junk = mem->u.m.backout;
    mem->u.m.backout = junk->next;
    WndFree( junk->follow );
    WndFree( junk );
}

static void MemSetCurrent( a_window *wnd, unsigned offset )
{
    wnd_row     row;
    int         line_size;
    int         piece;
    mem_window  *mem = WndMem( wnd );

    line_size = mem->items_per_line * mem->item_size;
    row = offset / line_size;
    piece = ( offset - row * line_size ) / mem->item_size;
    WndNewCurrent( wnd, row, piece+1 );
}

static void MemBackout( a_window *wnd )
{
    mem_backout *backout;
    mem_window  *mem = WndMem( wnd );

    backout = mem->u.m.backout;
    if( backout == NULL )
        return;
    MemSetStartAddr( wnd, backout->home, true );
    MemSetStartAddr( wnd, backout->addr, false );
    if( backout->has_current ) {
        MemSetCurrent( wnd, backout->curr_offset );
    } else {
        WndNoCurrent( wnd );
    }
    mem->total_size = backout->total_size;
    if( backout->follow ) {
        WndFree( mem->u.m.follow );
        mem->u.m.follow = backout->follow;
        backout->follow = NULL;
    }
    MemFreeBackout( mem );
    WndSetTitle( wnd, MemGetTitle( mem ) );
    SetNewDataDot( wnd );
//    SetDataDot( mem->u.m.home );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}

static void MemNewBackout( a_window *wnd )
{
    mem_backout *backout;
    mem_window  *mem = WndMem( wnd );

    backout = WndAlloc( sizeof( *backout ) );
    if( backout == NULL )
        return;
    backout->next = mem->u.m.backout;
    backout->home = mem->u.m.home;
    backout->addr = mem->u.m.addr;
    backout->curr_offset = MemCurrOffset( wnd );
    backout->total_size = mem->total_size;
    backout->has_current = WndHasCurrent( wnd );
    if( mem->u.m.follow ) {
        backout->follow = DupStr( mem->u.m.follow );
    } else {
        backout->follow = NULL;
    }
    mem->u.m.backout = backout;
    for( backout = backout->next; backout != NULL; backout = backout->next ) {
        if( AddrComp( mem->u.m.backout->home, backout->home ) == 0 ) {
            Warn( LIT_DUI( WARN_Cycle_Detected ) );
            return;
        }
    }
}

static void MemFollow( a_window *wnd )
{
    address     new_addr;
    mem_window  *mem = WndMem( wnd );
    char        ch;

    if( mem->u.m.follow != NULL ) {
        new_addr = mem->u.m.home;
        SetDataDot( new_addr );
        if( !DlgScanGivenAddr( mem->u.m.follow, &new_addr ) ) {
            Warn( TxtBuff ); /* stashed here */
            MemBackout( wnd );
        } else {
            mem->total_size = 0;
            if( ProgPeekWrap( new_addr, &ch, 1 ) != 1 ) {
                MemBackout( wnd );
                Error( ERR_NONE, LIT_ENG( ERR_NO_READ_MEM ), new_addr );
            }
            MemSetStartAddr( wnd, new_addr, true );
        }
    } else {
        new_addr = AddrAddWrap( mem->u.m.addr, mem->total_size );
        mem->u.m.home = mem->u.m.addr;
        if( !MemScrollBytes( wnd, mem->total_size, true ) ) {
            MemBackout( wnd );
        } else {
            WndNoCurrent( wnd );
        }
    }
    WndSetTitle( wnd, MemGetTitle( mem ) );
    SetNewDataDot( wnd );
//  SetDataDot( mem->u.m.home );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}


static void DoFollow( a_window *wnd, char *fmt )
{
    mem_window  *mem = WndMem( wnd );

    MemNewBackout( wnd );
    WndFree( mem->u.m.follow );
    Format( TxtBuff, fmt, AddrDiff( mem->u.m.addr, mem->u.m.home ) + MemCurrOffset( wnd ) );
    mem->u.m.follow = DupStr( TxtBuff );
    MemFollow( wnd );
}

static void SetBreakWrite( a_window *wnd )
{
    mem_window  *mem = WndMem( wnd );
    char        buff[TXT_LEN];
    address     addr;

    addr = AddrAddWrap( mem->u.m.addr, MemCurrOffset( wnd ) );
    StrAddr( &addr, buff, TXT_LEN );
    if( !BreakWrite( addr, MemData.info[mem->piece_type].type, buff ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_NOT_WATCH_SIZE ) );
    }
}

static bool GetBuff( mem_window *mem, unsigned long offset, char *buff, size_t size )
{
    unsigned long   new;
    size_t          len;

    if( mem->file ) {
        offset += mem->u.f.offset;
        new = SeekStream( mem->u.f.fh, offset, DIO_SEEK_ORG );
        if( new != offset )
            return( false );
        len = ReadStream( mem->u.f.fh, buff, size );
        return( len == size );
    } else {
        if( mem->u.m.contents == NULL )
            return( false );
        if( offset + size > mem->u.m.size )
            return( false );
        memcpy( buff, (char*)mem->u.m.contents + offset, size );
        return( true );
    }
}

static  bool    MemGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    char            buff[16];
    unsigned long   offset;
    mem_window      *mem = WndMem( wnd );
    address         addr;
    char            *p;
    mad_radix       old_radix, new_radix;
    size_t          max;
    char            ch;

    line->text = TxtBuff;
    if( row < 0 ) {
        row += TITLE_SIZE;
        switch( row ) {
        case 0:
            line->tabstop = false;
            line->indent = HeadTab[mem->file]( wnd, piece );
            line->attr = WND_STANDOUT;
            if( line->indent == (gui_ord)-1 )
                return( false );
            return( true );
#if 0
        case 1:
            if( piece != 0 )
                return( false );
            SetUnderLine( wnd, line );
            return( true );
#endif
        default:
            return( false );
        }
    }
    if( row >= WndRows( wnd ) )
        return( false );
    TxtBuff[0] = NULLCHAR;
    offset = row * mem->items_per_line;
    if( piece == 0 ) {
        line->tabstop = false;
        offset *= mem->item_size;
        if( mem->file ) {
            p = CnvULong( mem->u.f.offset + offset, TxtBuff, TXT_LEN );
            StrCopy( ":", p );
            return( true );
        }
        if( mem->stack ) {
            if( offset == mem->sp_offset ) {
                MADRegSpecialName( MSR_SP, &DbgRegs->mr, MAF_OFFSET, TxtBuff, TXT_LEN );
                line->text = TxtBuff;
                line->attr = WND_STANDOUT;
                return( true );
            } else if( offset == mem->bp_offset ) {
                MADRegSpecialName( MSR_FP, &DbgRegs->mr, MAF_OFFSET, TxtBuff, TXT_LEN );
                line->text = TxtBuff;
                line->attr = WND_STANDOUT;
                return( true );
            }
        }
        addr = AddrAddWrap( mem->u.m.addr, offset );
        p = AddrToString( &addr, MAF_OFFSET, TxtBuff, TXT_LEN );
        StrCopy( ":", p );
        return( true );
    }
    if( row == mem->cursor_row && piece == mem->shadow_piece ) {
        line->attr = WND_STANDOUT;
    }
    --piece;
    if( piece >= mem->items_per_line ) {
        if( mem->piece_type != MemByteType )
            return( false );
        if( piece >= 2 * mem->items_per_line )
            return( false );
        if( mem->item_width <= 2 ) {
            line->indent = ( mem->item_width * WndAvgCharX(wnd) );
        } else {
            line->indent = ( mem->item_width * WndMidCharX(wnd) );
        }
        line->indent += WndMidCharX( wnd );
        line->indent *= mem->items_per_line;
        line->indent += ( piece - mem->items_per_line + 1 ) * WndMaxCharX(wnd);
        offset += piece - mem->items_per_line;
    } else {
        line->indent = ( piece * mem->item_width );
        if( mem->item_width <= 2 ) {
            line->indent *= WndAvgCharX( wnd );
        } else {
            line->indent *= WndMidCharX( wnd );
        }
        line->indent += piece * WndMidCharX( wnd );
        offset += piece;
    }
    line->indent += mem->address_end + WndAvgCharX( wnd );
    offset *= mem->item_size;
    if( mem->total_size != 0 && offset >= mem->total_size )
        return( false );
    if( !GetBuff( mem, offset, buff, mem->item_size ) )
        return( false );
    if( piece >= mem->items_per_line ) {
        ch = buff[0];
        if( !isprint( ch ) )
            ch = '.';
        TxtBuff[0] = ch;
        TxtBuff[1] = NULLCHAR;
    } else {
        new_radix = MemData.info[mem->piece_type].piece_radix;
        old_radix = NewCurrRadix( new_radix );
        max = TXT_LEN;
        MADTypeHandleToString( new_radix, MemData.info[mem->piece_type].type, &buff, TxtBuff, &max );
        NewCurrRadix( old_radix );
    }
    return( true );
}

static void MemResize( a_window *wnd )
{
    mem_window          *mem = WndMem( wnd );
    int                 mult;
    wnd_line_piece      line;
    unsigned            curr_offset;

    curr_offset = MemCurrOffset( wnd );
    if( mem->stack ) {
        MemSetStartAddr( wnd, Context.stack, true );
        WndZapped( wnd );
        return;
    }
    if( mem->piece_type == MemByteType ) {
        mult = 2;
    } else {
        mult = 1;
    }
    for( mem->items_per_line = MemData.info[mem->piece_type].items_per_line;
         mem->items_per_line > 1; --mem->items_per_line ) {
        MemGetLine( wnd, 0, mem->items_per_line*mult, &line );
        line.indent += WndExtentX( wnd, line.text );
        if( line.indent <= WndWidth( wnd ) ) {
            break;
        }
    }
    MemSetCurrent( wnd, curr_offset );
    MemRefresh( wnd );
}

static void     MemMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    mem_window  *mem = WndMem( wnd );

    --piece;
    if( piece >= mem->items_per_line )
        piece -= mem->items_per_line;
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuEnable( wnd, MENU_MEMORY_MODIFY, CanModify( wnd, row, piece ) );
        WndMenuEnable( wnd, MENU_MEMORY_LEFT, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_RIGHT, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_ASSEMBLY, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_ADDRESS, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_BREAK_WRITE, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_FOLLOW_NEAR, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_FOLLOW_FAR, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_FOLLOW_CURSOR, !mem->file && WndHasCurrent( wnd ) || mem->total_size != 0 );
        WndMenuEnable( wnd, MENU_MEMORY_FOLLOW_SEGMENT, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_HOME, !mem->file );
        WndMenuEnable( wnd, MENU_MEMORY_REPEAT, !mem->file &&
                            ( mem->u.m.follow || mem->total_size != 0 ) );
        WndMenuEnable( wnd, MENU_MEMORY_PREV, !mem->file && mem->u.m.backout );
        break;
    case MENU_MEMORY_ASSEMBLY:
        WndAsmInspect( AddrAddWrap( mem->u.m.addr, MemCurrOffset( wnd ) ) );
        break;
    case MENU_MEMORY_FOLLOW_FAR:
        DoFollow( wnd, "%%(.%o)" );
        break;
    case MENU_MEMORY_FOLLOW_NEAR:
        DoFollow( wnd, "*(.%o)" );
        break;
    case MENU_MEMORY_FOLLOW_SEGMENT:
        DoFollow( wnd, "(*(short*)(.%o)):0" );
        break;
    case MENU_MEMORY_FOLLOW_CURSOR:
        MemNewBackout( wnd );
        WndFree( mem->u.m.follow );
        mem->u.m.follow = NULL;
        if( WndHasCurrent( wnd ) ) {
            mem->total_size = MemCurrOffset( wnd );
        }
        MemFollow( wnd );
        break;
    case MENU_MEMORY_HOME:
        while( mem->u.m.backout != NULL ) {
            MemBackout( wnd );
        }
        MemSetStartAddr( wnd, mem->u.m.home, true );
        MemSetCurrent( wnd, 0 );
        WndRepaint( wnd );
        break;
    case MENU_MEMORY_REPEAT:
        MemNewBackout( wnd );
        MemFollow( wnd );
        break;
    case MENU_MEMORY_PREV:
        MemBackout( wnd );
        break;
    case MENU_MEMORY_BREAK_WRITE:
        SetBreakWrite( wnd );
        break;
    case MENU_MEMORY_MODIFY:
        MemModify( wnd, row, piece + 1 );
        break;
    case MENU_MEMORY_ADDRESS:
        MemGetNewAddr( wnd );
        break;
    case MENU_MEMORY_LEFT:
        MemScrollBytes( wnd, -mem->item_size, false );
        WndNoSelect( wnd );
        WndRepaint( wnd );
        break;
    case MENU_MEMORY_RIGHT:
        MemScrollBytes( wnd, mem->item_size, false );
        WndNoSelect( wnd );
        WndRepaint( wnd );
        break;
    default:
        MemSetType( wnd, PIECE_TYPE( id ) );
        MemResize( wnd );
        WndZapped( wnd );
        break;
    }
}

static  void StkRefresh( a_window *wnd )
{
    MemSetStartAddr( wnd, Context.stack, true );
    WndZapped( wnd );
}


static  int     MemScroll( a_window *wnd, int lines )
{
    int             tomove;
    unsigned long   offset;
    unsigned long   new;
    mem_window      *mem = WndMem( wnd );

    if( lines >= WND_MAX_ROW )
        return( 0 ); // CTRL-END request
    if( mem->cursor_row != WND_NO_ROW && mem->piece_type == MemByteType ) {
        WndPieceDirty( wnd, mem->cursor_row, mem->shadow_piece );
    }
    tomove = mem->items_per_line * mem->item_size * lines;
    if( mem->total_size != 0 )
        return( 0 );
    if( mem->file ) {
        offset = mem->u.f.offset + tomove;
        if( tomove >= 0 && offset < mem->u.f.offset || tomove < 0 && offset > mem->u.f.offset ) {
            offset = 0;
        }
        new = SeekStream( mem->u.f.fh, offset, DIO_SEEK_ORG );
        if( new != offset )
            return( 0 );
        if( ReadStream( mem->u.f.fh, TxtBuff, 1 ) != 1 )
            return( 0 );
        mem->u.f.offset = offset;
        WndSetVScrollRange( wnd, WndRows( wnd ) * 2 );
        WndSetThumbPercent( wnd, ( offset * 100 ) / mem->u.f.size );
    } else {
        if( !MemScrollBytes( wnd, tomove, false ) ) {
            lines = 0;
        }
    }
    WndRowDirty( wnd, -TITLE_SIZE );
    return( lines );
}

void InitMemWindow( void )
{
    int                 i;

    MemInitTypes( MAS_MEMORY | MTK_ALL, &MemData );
    MemTypeMenu = WndMustAlloc( MemData.num_types * sizeof( *MemTypeMenu ) );
    for( i = 0; i < MemData.num_types; ++i ) {
        MemTypeMenu[i].id = MENU_MEMORY_FIRST_TYPE + i;
        MemTypeMenu[i].style = GUI_ENABLED | WND_MENU_ALLOCATED;
        MemTypeMenu[i].label = DupStr( MemData.labels[i] );
        MemTypeMenu[i].hinttext = DupStr( LIT_ENG( Empty ) );
        MemTypeMenu[i].num_child_menus = 0;
        MemTypeMenu[i].child = NULL;
    }
    for( i = 0; i < ArraySize( MemMenu ); ++i ) {
        if( MemMenu[i].id == MENU_MEMORY_TYPE ) {
            MemMenu[i].child = MemTypeMenu;
            MemMenu[i].num_child_menus = MemData.num_types;
            break;
        }
    }
    MemByteType = 0;
    for( i = 0; i < MemData.num_types; ++i ) {
        if( MemData.info[i].item_size == 1 &&
            MemData.info[i].piece_radix == 16 ) {
            MemByteType = i;
            break;
        }
    }
}

void FiniMemWindow( void )
{
    WndFree( MemTypeMenu );
    MemFiniTypes( &MemData );
}

static bool MemEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    mem_window      *mem = WndMem( wnd );
    int             i;

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        if( mem->init_type != MAD_NIL_TYPE_HANDLE ) {
            mem->piece_type = MemByteType;
            for( i = 0; i < MemData.num_types; i++ ) {
                if( MemData.info[i].type == mem->init_type ) break;
            }
            if( i != MemData.num_types ) {
                mem->piece_type = i;
            }
        }
        MemSetType( wnd, mem->piece_type ); /* stashed here by Open routine */
        if( mem->file ) {
            mem->u.f.offset = 0;
            if( mem->u.f.fh != NIL_HANDLE ) {
                mem->u.f.size = SeekStream( mem->u.f.fh, 0L, DIO_SEEK_END );
                if( mem->u.f.size == ERR_SEEK )
                    mem->u.f.size = 0;
                SeekStream( mem->u.f.fh, 0L, DIO_SEEK_ORG );
            }
        } else {
            mem->u.m.follow = NULL;
            mem->u.m.backout = NULL;
        }
        mem->total_size = 0;
        mem->cursor_row = WND_NO_ROW;
        mem->last_type_popup = 0;
        MemRefresh( wnd );
        /* fall through */
    case GUI_RESIZE:
        MemResize( wnd );
        WndFixedThumb( wnd );
        return( true );
    case GUI_DESTROY :
        if( mem->file && mem->u.f.fh != NIL_HANDLE ) {
            FileClose( mem->u.f.fh );
        }
        if( !mem->file ) {
            WndFree( mem->u.m.contents );
            WndFree( mem->u.m.follow );
            while( mem->u.m.backout != NULL ) {
                MemFreeBackout( mem );
            }
        }
        WndFree( mem );
        return( true );
    case GUI_NO_EVENT : // sent whenever WndDirtyCurr is called
        MemUpdateCursor( wnd );
        return( true );
    }
    return( false );
}

wnd_info BinInfo = {
    MemEventProc,
    MemRefresh,
    MemGetLine,
    MemMenuItem,
    MemScroll,
    NoBegPaint,
    NoEndPaint,
    MemModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_RADIX_CHANGE,
    DefPopUp( MemMenu )
};

wnd_info MemInfo = {
    MemEventProc,
    MemRefresh,
    MemGetLine,
    MemMenuItem,
    MemScroll,
    NoBegPaint,
    NoEndPaint,
    MemModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_RADIX_CHANGE+UP_MEM_CHANGE+UP_SYM_CHANGE+UP_NEW_PROGRAM,
    DefPopUp( MemMenu )
};

wnd_info StkInfo = {
    MemEventProc,
    StkRefresh,
    MemGetLine,
    MemMenuItem,
    MemScroll,
    NoBegPaint,
    NoEndPaint,
    MemModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_RADIX_CHANGE+UP_MEM_CHANGE+UP_STACKPOS_CHANGE+UP_REG_CHANGE+UP_SYM_CHANGE+UP_NEW_PROGRAM,
    DefPopUp( MemMenu )
};

extern  a_window        *DoWndMemOpen( address addr, mad_type_handle type )
{
    mem_window  *mem;
    a_window    *wnd;

    MemValidAddr( addr );
    mem = WndMustAlloc( sizeof( mem_window ) );
    mem->u.m.addr = mem->u.m.home = addr;
    mem->u.m.contents = NULL;
    SetDataDot( addr );
    mem->file = false;
    mem->stack = false;
    mem->init_type = type;
    mem->piece_type = MemByteType;
    wnd = DbgTitleWndCreate( MemGetTitle( mem ), &MemInfo, WND_MEMORY, mem, &MemIcon, TITLE_SIZE, false );
    return( wnd );
}

extern  a_window        *WndMemOpen( void )
{
    return( DoWndMemOpen( NilAddr, MAD_NIL_TYPE_HANDLE ) );
}


a_window        *WndStkOpen( void )
{
    a_window    *wnd;
    mem_window  *mem;

    mem = WndMustAlloc( sizeof( mem_window ) );
    mem->u.m.addr = mem->u.m.home = Context.stack;
    mem->u.m.contents = NULL;
    mem->init_type = GetMADTypeHandleDefaultAt( Context.stack, MTK_INTEGER );
    mem->file = false;
    mem->stack = true;
    wnd = DbgTitleWndCreate( LIT_DUI( WindowStack ), &StkInfo, WND_STACK, mem, &StkIcon, TITLE_SIZE, false );
    return( wnd );
}


a_window        *DoWndBinOpen( const char *title, file_handle fh )
{
    mem_window  *mem;
    a_window    *wnd;

    mem = WndMustAlloc( sizeof( mem_window ) );
    mem->file = true;
    mem->stack = false;
    mem->init_type = MAD_NIL_TYPE_HANDLE;
    mem->piece_type = MemByteType;
    mem->u.f.fh = fh;
    wnd = DbgTitleWndCreate( title, &BinInfo, WND_BINARY, mem, &MemIcon, TITLE_SIZE, false );
    return( wnd );
}
