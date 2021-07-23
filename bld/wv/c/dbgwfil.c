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
* Description:  Processing of pop-up menu for source file view.
*
****************************************************************************/


#include <limits.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgitem.h"
#include "dbgstk.h"
#include "srcmgt.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "dbgexpr.h"
#include "dbgbrk.h"
#include "dbgparse.h"
#include "wndsys.h"
#include "dbgmisc.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "addarith.h"
#include "dbginsty.h"
#include "dbgupdt.h"
#include "dlgexpr.h"
#include "dbgdot.h"
#include "dbgwass.h"
#include "dbgwfil.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "menudef.h"


#define MAX_LINE_LEN    255 // must not wrap a gui_ord

#define WndFile( wnd ) ( (file_window *)WndExtra( wnd ) )

#define NOT_ACTIVE ((unsigned)-1)

enum {
    PIECE_BREAK,
    PIECE_SOURCE
};

typedef struct {
    void                *viewhndl;
    unsigned long       size;
    unsigned            active;
    mod_handle          mod;
    cue_fileid          file_id;
    wnd_row             rows;
    unsigned long       rows_offset;
    unsigned long       range;
    address             dotaddr;
    char                *name;
    a_window            asw;
    int                 eof;
    boolbit             track           : 1;
    boolbit             erase           : 1;
    boolbit             toggled_break   : 1;
} file_window;

extern bool             FirstLinInfo( mod_handle, address *, unsigned * );
extern unsigned         ExprSize( stack_entry * );

static gui_menu_struct FileShowMenu[] = {
    #include "mfilshow.h"
};

static gui_menu_struct FileMenu[] = {
    #include "menufile.h"
};

void    SrcJoinAsm( a_window wnd, a_window asw )
{
    WndFile( wnd )->asw = asw;
}

void    SrcNewAsmNotify( a_window asw, mod_handle mod, bool track )
{
    file_window *file;
    a_window    wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( WndClass( wnd ) != WND_SOURCE )
            continue;
        file = WndFile( wnd );
        if( track != file->track )
            continue;
        if( mod != file->mod )
            continue;
        if( file->asw != NULL )
            continue;
        AsmJoinSrc( asw, wnd );
        SrcJoinAsm( wnd, asw );
        break;
    }
}

void    SrcFreeAsm( a_window wnd )
{
    if( wnd == NULL )
        return;
    WndFile( wnd )->asw = NULL;
}

#ifdef DEADCODE
bool    SrcIsTracking( a_window wnd )
{
    return( WndFile( wnd )->track );
}
#endif

static address GetRowAddr( file_window *file, wnd_row row, bool exact )
{
    DIPHDL( cue, cueh );

    if( file->mod == NO_MOD || row < 0 )
        return( NilAddr );
    switch( DIPLineCue( file->mod, file->file_id, row + 1, 0, cueh ) ) {
    case SR_NONE:
        return( NilAddr );
    case SR_CLOSEST:
        if( exact )
            return( NilAddr );
        break;
    }
    return( DIPCueAddr( cueh ) );
}


static void Centre( a_window wnd, unsigned line )
{
    WndZapped( wnd );
    WndVScroll( wnd, line - ( WndRows( wnd ) / 2 ) - WndTop( wnd ) );
}


static void GotoLine( a_window wnd )
{
    long        line;
    mad_radix   old_radix;
    wnd_row     curr_row;
    wnd_piece   curr_piece;

    old_radix = NewCurrRadix( 10 );
    WndGetCurrent( wnd, &curr_row, &curr_piece );
    if( curr_row < 0 || curr_row == WND_NO_ROW ) {
        line = WndTop( wnd );
    } else {
        line = curr_row;
    }
    ++line;
    if( DlgLongExpr( LIT_DUI( New_Line ), &line ) ) {
        --line;
        WndDirtyCurr( wnd );
        Centre( wnd, line );
        WndNewCurrent( wnd, line, PIECE_SOURCE );
    }
    NewCurrRadix( old_radix );
}


static void     FileMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    address     addr;
    mod_handle  mod;
    bool        has_addr;
    bool        has_popitem;
    file_window *file = WndFile( wnd );

    /* unused parameters */ (void)piece;

    addr = GetRowAddr( file, row, id != MENU_FILE_ASSEMBLY );
    has_addr = !IS_NIL_ADDR( addr );
    switch( id ) {
    case MENU_INITIALIZE:
        has_popitem = ( *WndPopItem( wnd ) != NULLCHAR );
        if( has_popitem && !ScanSelectedExpr( WndPopItem( wnd ) ) ) {
            has_popitem = false;
        }
        WndMenuEnable( wnd, MENU_FILE_SHOW, true );
        WndMenuEnable( wnd, MENU_FILE_SHOW_ADDRESS, true );
        WndMenuEnable( wnd, MENU_FILE_SHOW_MODULE, true );
        WndMenuEnable( wnd, MENU_FILE_FUNCTIONS, file->mod != NO_MOD );
        WndMenuEnable( wnd, MENU_FILE_HOME, true );
        addr = GetRowAddr( file, row, false );
        WndMenuEnable( wnd, MENU_FILE_ASSEMBLY, !IS_NIL_ADDR( addr ) );
        WndMenuEnable( wnd, MENU_FILE_WATCH, has_popitem );
        WndMenuEnable( wnd, MENU_FILE_INSPECT, has_popitem );
        WndMenuEnable( wnd, MENU_FILE_STEP_INTO, file->mod != NO_MOD && has_popitem );
        WndMenuEnable( wnd, MENU_FILE_BREAK, has_popitem );
        WndMenuEnable( wnd, MENU_FILE_RUN, has_addr );
        WndMenuEnable( wnd, MENU_FILE_SKIP_TO_CURSOR, has_addr );
        break;
    case MENU_FILE_RUN:
        GoToAddr( addr );
        break;
    case MENU_FILE_SKIP_TO_CURSOR:
        SkipToAddr( addr );
        break;
    case MENU_FILE_BREAK:
        BreakOnSelected( WndPopItem( wnd ) );
        break;
    case MENU_FILE_HOME:
        GoHome();
        break;
    case MENU_FILE_SHOW_MODULE:
        mod = file->mod;
        if( DlgModName( LIT_DUI( New_Module ), &mod ) ) {
            WndModInspect( mod );
        }
        break;
    case MENU_FILE_SHOW_ADDRESS:
        if( DlgCodeAddr( LIT_DUI( New_Addr ), &addr ) ) {
            WndSrcInspect( addr );
        }
        break;
    case MENU_FILE_STEP_INTO:
        StepIntoFunction( WndPopItem( wnd ) );
        break;
    case MENU_FILE_INSPECT:
        WndInspect( WndPopItem( wnd ) );
        break;
    case MENU_FILE_WATCH:
        WndVarInspect( WndPopItem( wnd ) );
        break;
    case MENU_FILE_SEARCH:
        WndSaveToHistory( SrchHistory, WndPopItem( wnd ) );
        DbgWndSearch( wnd, false, DlgSearch( wnd, SrchHistory ) );
        break;
    case MENU_FILE_ASSEMBLY:
        AsmWndFind( file->asw, addr, file->track );
        break;
    case MENU_FILE_LINE:
        GotoLine( wnd );
        break;
    case MENU_FILE_FUNCTIONS:
        WndFuncInspect( file->mod );
        break;
    }
}


static void FilePosInit( a_window wnd )
{
    file_window *file = WndFile( wnd );

    if( file->viewhndl == NULL ) {
        file->size = 0;
    } else {
        file->size = FSize( file->viewhndl );
    }
    WndSetThumbPercent( wnd, 0 );
}


static void FilePos( a_window wnd, int pos )
{
    unsigned long       range;
    file_window *file = WndFile( wnd );
    size_t              len;

    if( pos < 0 )
        pos = 0;
    if( file->viewhndl == NULL ) {
        if( pos + WndRows( wnd ) > file->eof )
            return;
        WndSetTop( wnd, pos );
        return;
    }
    len = FReadLine( file->viewhndl, pos + WndRows( wnd ), 0, TxtBuff, TXT_LEN );
    if( len == FREADLINE_ERROR ) {
        len = 0;
        pos = FCurrLine( file->viewhndl ) - WndRows( wnd ) - 1;
        if( pos < 0 ) {
            pos = 0;
        }
    }
    TxtBuff[len] = NULLCHAR;
    WndSetTop( wnd, pos );
    if( pos >= file->rows ) {
        file->rows = pos + 1;
        file->rows_offset = FLastOffset( file->viewhndl );
    }
    if( file->rows == 0 )
        file->rows = 1;
    if( file->rows_offset == 0 ) {
        range = file->size;
    } else {
        range = file->size * file->rows / file->rows_offset;
    }
    WndSetVScrollRange( wnd, range );
    WndSetThumbPos( wnd, pos );
}


static int FileScroll( a_window wnd, int lines )
{
    int         old_top;

    old_top = WndTop( wnd );
    FilePos( wnd, old_top + lines );
    return( WndTop( wnd ) - old_top );
}


static  void    FileModify( a_window wnd, wnd_row row, wnd_piece piece )
{
    file_window *file = WndFile( wnd );
    address     addr;

    if( piece == PIECE_BREAK ) {
        addr = GetRowAddr( file, row, true );
        if( IS_NIL_ADDR( addr ) )
            return;
        file->toggled_break = ( ( UpdateFlags & UP_BREAK_CHANGE ) == 0 );
        ToggleBreak( addr );
        WndRowDirty( wnd, row );
    } else {
        WndFirstMenuItem( wnd, row, piece );
    }
}

static void FileSetDotAddr( a_window wnd, address addr )
{
    file_window *file = WndFile( wnd );

    if( AddrComp( file->dotaddr, addr ) == 0 )
        return;
    file->dotaddr = addr;
    if( IS_NIL_ADDR( addr ) )
        return;
    if( wnd == WndFindActive() ) {
        AsmMoveDot( file->asw, addr );
        SetCodeDot( addr );
    }
}

static void FileNotify( a_window wnd, wnd_row row, wnd_piece piece )
{
    file_window *file = WndFile( wnd );
    address     addr;

    /* unused parameters */ (void)piece;

    addr = GetRowAddr( file, row, false );
    if( IS_NIL_ADDR( addr ) )
        return;
    FileSetDotAddr( wnd, addr );
}


bool FileOpenGadget( a_window wnd, wnd_line_piece *line, mod_handle mod )
{
    a_window    curr;

    for( curr = WndNext( NULL ); curr != NULL; curr = WndNext( curr ) ) {
        if( WndClass( curr ) != WND_SOURCE )
            continue;
        if( mod == WndFile( curr )->mod ) {
            if( line != NULL )
                SetGadgetLine( wnd, line, GADGET_OPEN_SOURCE );
            return( true );
        }
    }
    if( line != NULL )
        SetGadgetLine( wnd, line, GADGET_CLOSED_SOURCE );
    return( false );
}


void FileBreakGadget( a_window wnd, wnd_line_piece *line, bool curr, brkp *bp )
{
    if( curr ) {
        if( bp == NULL ) {
            SetGadgetLine( wnd, line, GADGET_CURRLINE );
        } else if( bp->status.b.active ) {
            SetGadgetLine( wnd, line, GADGET_CURRBREAK );
        } else {
            SetGadgetLine( wnd, line, GADGET_CURRDIMBREAK );
        }
    } else {
        if( bp == NULL ) {
            SetGadgetLine( wnd, line, GADGET_NOBREAK );
        } else if( bp->status.b.active ) {
            SetGadgetLine( wnd, line, GADGET_BREAK );
        } else {
            SetGadgetLine( wnd, line, GADGET_DIMBREAK );
        }
    }
}


static  bool    FileGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    size_t      len;
    file_window *file = WndFile( wnd );
    address     addr;
    brkp        *bp;
    bool        curr;
    DIPHDL( cue, cueh );

    line->text = LIT_ENG( Empty );
    if( file->viewhndl == NULL && DIPModHasInfo( file->mod, HK_CUE ) != DS_OK ) {
        return( false );
    }
    curr = ( row == file->active && ContextMod == file->mod );
    switch( piece ) {
    case PIECE_BREAK:
        line->tabstop = false;
        if( row >= file->eof )
            return( false );
        if( file->mod == NO_MOD )
            return( true );
        addr = NilAddr;
        if( !WndDoingSearch ) { // too expensive
            addr = GetRowAddr( file, row, true );
        }
        if( !IS_NIL_ADDR( addr ) ) {
            bp = FindBreakByLine( file->mod, file->file_id, row + 1 );
            FileBreakGadget( wnd, line, curr, bp );
        }
        return( true );
    case PIECE_SOURCE:
        line->text = TxtBuff;
        line->extent = WND_MAX_EXTEND;
        if( curr )
            line->attr = WND_STANDOUT;
        if( file->mod != NO_MOD ) {
            line->indent = MaxGadgetLength;
        }
        if( file->viewhndl == NULL ) {
            Format( TxtBuff, LIT_DUI( No_Source_Line ), row + 1 );
            if( DIPLineCue( file->mod, file->file_id, 0, 0, cueh ) != SR_NONE ) {
                if( (DIPCueAdjust( cueh, -1, cueh ) & DS_ERR) ) {
                    file->eof = DIPCueLine( cueh );
                }
            }
            return( true );
        }
        len = FReadLine( file->viewhndl, row + 1, 0, TxtBuff, MAX_LINE_LEN + 1 );
        if( len == FREADLINE_ERROR ) {
            TxtBuff[0] = NULLCHAR;
            file->eof = row;
            return( false );
        }
        if( len > MAX_LINE_LEN ) {
            /* TxtBuff has length TXT_LEN which is significantly bigger
             * then MAX_LINE_LEN + 4, there is enough space to add overflow mark
             */
            StrCopy( " ...", TxtBuff + MAX_LINE_LEN );
        } else {
            TxtBuff[len] = NULLCHAR;
        }
        if( row >= file->rows ) {
            file->rows = row + 1;
            file->rows_offset = FLastOffset( file->viewhndl );
        }
        return( true );
    default:
        return( false );
    }
}


static void SeekToTheEnd( file_window *file )
{
    size_t  len;

    if( file->viewhndl != NULL && !FileIsRemote( file->viewhndl ) ) {
        len = FReadLine( file->viewhndl, INT_MAX, 0, TxtBuff, TXT_LEN );
        if( len == FREADLINE_ERROR )
            len = 0;
        TxtBuff[len] = NULLCHAR;
        file->rows = FCurrLine( file->viewhndl ) - 1;
        file->rows_offset = FLastOffset( file->viewhndl );
    } else {
        file->rows = 0;
        file->rows_offset = 0;
    }
}


static unsigned ActiveLine( void )
{
    DIPHDL( cue, cueh );

    if( DeAliasAddrCue( ContextMod, Context.execution, cueh ) == SR_NONE )
        return( 0 );
    return( DIPCueLine( cueh ) - 1 );
}


#ifdef DEADCODE
void    FileReset( a_window wnd )
{
    WndFile( wnd )->active = NOT_ACTIVE;
}
#endif


static void FileSetTitle( a_window wnd, mod_handle mod )
{
    char        *p;
    const char  *image_name;
    file_window *file = WndFile( wnd );

    if( file->track ) {
        p = StrCopy( ": ", StrCopy( LIT_DUI( WindowSource ), TxtBuff ) );
    } else {
        p = TxtBuff;
    }
    if( file->viewhndl != NULL ) {
        p = StrCopy( SkipPathInfo( FGetName( file->viewhndl ), 0 ), p );
    } else if( file->mod != NO_MOD ) {
        p += DIPModName( file->mod, p, TXT_LEN );
    }
    image_name = ModImageName( mod );
    if( image_name[0] != NULLCHAR ) {
        p = StrCopy( "(", StrCopy( " ", p ) );
        p = StrCopy( ")", StrCopy( SkipPathInfo( image_name, OP_REMOTE ), p ) );
    }
    WndSetTitle( wnd, TxtBuff );
}

static void FileTrack( a_window wnd, cue_handle *cueh )
{
    unsigned    active, old_active;
    unsigned    end_line;
    int         slack;
    file_window *file = WndFile( wnd );
    mod_handle  mod;
    cue_fileid  id;
    wnd_row     curr_row;
    wnd_piece   curr_piece;

    if( cueh == NULL ) {
        mod = NO_MOD;
        id = 0;
    } else {
        mod = DIPCueMod( cueh );
        id = DIPCueFileId( cueh );
    }
    if( file->viewhndl == NULL
      || file->mod != mod
      || file->file_id != id ) {
        if( file->viewhndl != NULL ) {
            FDoneSource( file->viewhndl );
        }
        file->mod = mod;
        file->file_id = id;
        FileSetDotAddr( wnd, GetCodeDot() );
        if( file->mod == NO_MOD ) {
            file->viewhndl = NULL;
        } else {
            file->viewhndl = OpenSrcFile( cueh );
        }
        FileSetTitle( wnd, mod );
        SeekToTheEnd( file );
        file->eof = INT_MAX;
        WndZapped( wnd );
        FilePosInit( wnd );
        file->active = NOT_ACTIVE;
        FilePos( wnd, 0 );
        DbgUpdate( UP_OPEN_CHANGE );
    }
    active = ActiveLine();
    if( active != file->active ) {
        FileSetDotAddr( wnd, GetCodeDot() );
        WndGetCurrent( wnd, &curr_row, &curr_piece );
        WndNoCurrent( wnd );
        if( curr_row != WND_NO_ROW ) {
            WndRowDirty( wnd, curr_row );
        }
    }
    old_active = file->active;
    file->active = NOT_ACTIVE;
    slack = WndRows( wnd ) / 4;
    if( slack > 2 )
        slack = 2;
    end_line = WndTop( wnd ) + WndRows( wnd ) - 1;
    if( old_active == NOT_ACTIVE || active > end_line ) {
        WndZapped( wnd );
        WndVScroll( wnd, active - slack - WndTop( wnd ) );
    } else if( active > end_line - slack ) {
        WndRowDirtyImmed( wnd, old_active );
        WndVScroll( wnd, WndRows( wnd ) - 2 * slack );
    } else if( active < WndTop( wnd ) ) {
        WndRowDirtyImmed( wnd, old_active );
        WndVScroll( wnd, active - WndTop( wnd ) - slack );
    } else {
        WndRowDirty( wnd, old_active );
    }
    WndNewCurrent( wnd, active, PIECE_SOURCE );
    WndRowDirty( wnd, active );
    file->active = active;
}

bool    SrcMoveDot( a_window wnd, address addr )
{
    unsigned    line;
    mod_handle  mod;
    file_window *file;
    DIPHDL( cue, cueh );

    if( wnd == NULL )
        return( false );
    file = WndFile( wnd );
    if( file->mod == NO_MOD && !file->track )
        return( false );
    if( IS_NIL_ADDR( addr ) ) {
        WndVScrollAbs( wnd, 0 );
        return( false );
    }
    DeAliasAddrMod( addr, &mod );
    if( DeAliasAddrCue( mod, addr, cueh ) == SR_NONE ) {
        if( DIPLineCue( mod, 0, 0, 0, cueh ) == SR_NONE ) {
            return( false );
        }
    }
    line = DIPCueLine( cueh );
    if( mod != file->mod || DIPCueFileId( cueh ) != file->file_id ) {
        if( !file->track )
            return( false );
        FileTrack( wnd, cueh );
    }
    --line;
    WndVScrollAbs( wnd, line ); //
    WndMoveCurrent( wnd, line, PIECE_SOURCE );
    FileSetDotAddr( wnd, addr );
    FileSetTitle( wnd, DIPCueMod( cueh ) );
    return( true );
}

a_window SrcWndFind( a_window wnd, address addr, bool track )
{
    a_window    new;
    mod_handle  mod;
    DIPHDL( cue, cueh );

    if( wnd == NULL ) {
        if( DeAliasAddrMod( addr, &mod ) == SR_NONE ) {
            new = NULL;
        } else {
            if( DeAliasAddrCue( mod, addr, cueh ) == SR_NONE ) {
                cueh = NULL;
            }
            new = DoWndSrcOpen( cueh, track );
        }
    } else {
        WndRestoreToFront( wnd );
        new = wnd;
    }
    SrcMoveDot( new, addr );
    return( new );
}

bool SrcHasFileOpen( a_window wnd )
{
    return( WndFile( wnd )->viewhndl != NULL );
}

static void     FileActive( a_window wnd, mod_handle mod )
{
    file_window *file = WndFile( wnd );
    unsigned    line;

    if( file->mod != mod ) {
        if( file->active != NOT_ACTIVE ) {
            file->active = NOT_ACTIVE;
            WndZapped( wnd );
        }
    } else {
        line = ActiveLine();
        if( line != file->active ) {
            WndRowDirty( wnd, file->active );
            file->active = line;
            WndRowDirty( wnd, file->active );
        }
    }
    if( file->mod != NO_MOD && ( UpdateFlags & UP_BREAK_CHANGE ) ) {
        WndNoSelect( wnd );
        WndSetRepaint( wnd );
    }
    if( file->active != NOT_ACTIVE ) {
        WndNewCurrent( wnd, file->active, PIECE_SOURCE );
    }
}


static  void    FileNewIP( a_window wnd )
{
    DIPHDL( cue, cueh );

    if( WndFile( wnd )->track ) {
        if( DeAliasAddrCue( ContextMod, Context.execution, cueh ) != SR_NONE ) {
            FileTrack( wnd, cueh );
        } else {
            FileTrack( wnd, NULL );
        }
    } else {
        FileActive( wnd, ContextMod );
    }
}

static void ClearSrcFile( file_window *file )
{
    if( file->viewhndl != NULL ) {
        FDoneSource( file->viewhndl );
        file->viewhndl = NULL;
    }
}

static void FileRefresh( a_window wnd )
{
    file_window *file = WndFile( wnd );
    address     dotaddr;
    DIPHDL( cue, cueh );

    if( UpdateFlags & UP_SYM_CHANGE ) {
        if( file->mod != NO_MOD )
            ClearSrcFile( file );
        if( DeAliasAddrMod( file->dotaddr, &file->mod ) == SR_NONE ) {
            file->mod = NO_MOD;
        }
        WndZapped( wnd );
    }
    if( UpdateFlags & (UP_CSIP_CHANGE | UP_STACKPOS_CHANGE) ) {
        FileNewIP( wnd );
    }
    if( (UpdateFlags & (UP_NEW_SRC | UP_SYM_CHANGE)) && (file->mod != NO_MOD) ) {
        ClearSrcFile( file );
        if( DIPLineCue( file->mod, file->file_id, 0, 0, cueh ) != SR_NONE ) {
            dotaddr = file->dotaddr;
            FileTrack( wnd, cueh );
            SrcMoveDot( wnd, dotaddr );
        }
    }
    if( (UpdateFlags & UP_BREAK_CHANGE) && (file->mod != NO_MOD) ) {
        if( file->toggled_break ) {
            file->toggled_break = false;
        } else {
            WndSetRepaint( wnd );
        }
    }
}


static bool FileWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
{
    file_window *file = WndFile( wnd );

    /* unused parameters */ (void)parm;

    switch( gui_ev ) {
    case GUI_NOW_ACTIVE:
        ActiveWindowLevel = LEVEL_SOURCE;
        if( IS_NIL_ADDR( file->dotaddr ) )
            return( true );
        SetCodeDot( file->dotaddr );
        AsmMoveDot( file->asw, file->dotaddr );
        return( true );
    case GUI_INIT_WINDOW:
        file->active = NOT_ACTIVE;
        file->rows = 0;
        file->rows_offset = 0;
        file->asw = NULL;
        SeekToTheEnd( file );
        FileNewIP( wnd );
        DbgUpdate( UP_OPEN_CHANGE );
        return( true );
    case GUI_DESTROY :
        if( file->viewhndl != NULL ) {
            FDoneSource( file->viewhndl );
        }
        if( file->erase ) {
            if( file->name ) {
                FileRemove( file->name, 0 );
            }
        }
        WndFree( file->name );
        AsmFreeSrc( file->asw );
        WndFree( file );
        DbgUpdate( UP_OPEN_CHANGE );
        return( true );
    }
    return( false );
}

static bool ChkUpdate( void )
{
    return( UpdateFlags & (UP_NEW_SRC | UP_SYM_CHANGE | UP_CSIP_CHANGE | UP_STACKPOS_CHANGE | UP_BREAK_CHANGE) );
}

wnd_info FileInfo = {
    FileWndEventProc,
    FileRefresh,
    FileGetLine,
    FileMenuItem,
    FileScroll,
    NoBegPaint,
    NoEndPaint,
    FileModify,
    NoNumRows,
    NoNextRow,
    FileNotify,
    ChkUpdate,
    PopUp( FileMenu )
};

a_window    DoWndFileOpen( const char *name, void *viewhndl,
                                        cue_handle *cueh, bool track,
                                        bool erase, wnd_class_wv wndclass )
{
    file_window *file;
    a_window    wnd;
    unsigned    line;

    file = WndMustAlloc( sizeof( file_window ) );
    file->viewhndl = viewhndl;
    if( cueh == NULL ) {
        file->mod = NO_MOD;
        file->file_id = 0;
        line = 0;
    } else {
        file->mod = DIPCueMod( cueh );
        file->file_id = DIPCueFileId( cueh );
        line = DIPCueLine( cueh );
    }
    file->track = false;
    file->erase = erase;
    file->toggled_break = false;
    file->eof = INT_MAX;
    file->name = DupStr( name );
    file->dotaddr = NilAddr;
    wnd = DbgWndCreate( LIT_ENG( Empty ), &FileInfo, wndclass, file, &SrcIcon );
    if( wnd == NULL )
        return( wnd );
    if( cueh != NULL ) {
        FileSetDotAddr( wnd, DIPCueAddr( cueh ) );
        FileSetTitle( wnd, file->mod );
    } else {
        FileSetDotAddr( wnd, NilAddr );
        WndSetTitle( wnd, file->name );
    }
    file->track = track;
    FileSetTitle( wnd, file->mod );
    WndSetSwitches( wnd, WSW_LBUTTON_SELECTS | WSW_RBUTTON_SELECTS | WSW_CHAR_CURSOR | WSW_SUBWORD_SELECT );
    WndClrSwitches( wnd, WSW_HIGHLIGHT_CURRENT );
    if( line != 0 ) {
        WndZapped( wnd );
    }
    FilePosInit( wnd );
    if( line != 0 ) {
        Centre( wnd, line - 1 );
        WndMoveCurrent( wnd, line - 1, PIECE_SOURCE );
    }
    return( wnd );
}

static  a_window    SrcFileOpen( cue_handle *cueh, bool track, bool erase, mod_handle mod )
{
    a_window    wnd;
    file_window *file;
    void        *viewhndl;

    if( cueh == NULL ) {
        viewhndl = NULL;
    } else {
        viewhndl = OpenSrcFile( cueh );
    }
    wnd = DoWndFileOpen( LIT_DUI( WindowSource ), viewhndl, cueh,
                           track, erase, track ? WND_SOURCE : WND_FILE );
    if( wnd == NULL )
        return( wnd );
    file = WndFile( wnd );
    file->mod = mod;
    if( cueh == NULL ) {
        AsmNewSrcNotify( wnd, mod, file->track );
    } else {
        AsmNewSrcNotify( wnd, file->mod, file->track );
    }
    return( wnd );
}


a_window DoWndSrcOpen( cue_handle *cueh, bool track )
{
    return( SrcFileOpen( cueh, track, false, cueh == NULL ? NO_MOD : DIPCueMod( cueh ) ) );
}


a_window WndSrcOpen( void )
{
    mod_handle  mod;
    address     addr;
    DIPHDL( cue, cueh );

    addr = GetCodeDot();
    if( IS_NIL_ADDR( addr ) ) {
        addr = Context.execution;
    }
    if( DeAliasAddrMod( addr, &mod ) == SR_NONE || DeAliasAddrCue( mod, addr, cueh ) == SR_NONE ) {
        cueh = NULL;
    }
    return( SrcFileOpen( cueh, true, false, mod ) );
}


void ProcView( void )
{
    const char          *start;
    size_t              len;
    bool                binary;

    binary = false;
    start = ScanPos();
    if( CurrToken == T_DIV ) {
        Scan();
        if( ScanCmd( "Binary\0" ) == 0 ) {
            binary = true;
        } else {
            ReScan( start );
        }
    }
    ScanItem( true, &start, &len );
    memcpy( TxtBuff, start, len );
    TxtBuff[len] = NULLCHAR;
    ReqEOC();
    if( len != 0 ) {
        WndFileInspect( TxtBuff, binary );
    } else {
        WndNewSrcInspect( GetCodeDot() );
    }
}
