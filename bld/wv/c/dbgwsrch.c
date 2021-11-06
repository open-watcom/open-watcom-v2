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


#include <stddef.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "wndregx.h"
#include "sortlist.h"
#include "srcmgt.h"
#include "strutil.h"
#include "dbgsrc.h"
#include "wndsys.h"
#include "modlist.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dbgwsrch.h"
#include "menudef.h"


#define WndSrch( wnd ) ( (srch_window *)WndExtra( wnd ) )

enum {
    PIECE_OPENER,
    PIECE_MODULE,
    PIECE_SOURCE,
};

typedef struct {
    char        *source_line;
    mod_handle  mod;
    cue_fileid  file_id;
    boolbit     open : 1;
} found_item;

typedef struct srch_window      srch_window;

typedef void (SRCH_WALKER)( srch_window * );

typedef struct a_cue {
    struct a_cue        *next;
    cue_handle          *cueh;
    char                name[1];
} a_cue;

struct srch_window {
    void        *rx;
    char        *expr;
    int         max_mod_name;
    wnd_row     num_rows;
    char        magic_str[MAX_MAGIC_STR + 1];
    found_item  *found;
    SRCH_WALKER *walk;
    void        *cookie;
    a_cue       *file_list;
    boolbit     ignore_case : 1;
    boolbit     use_rx      : 1;
};

static gui_menu_struct SrchMenu[] = {
    #include "menusrch.h"
};

static wnd_row SrchNumRows( a_window wnd )
{
    return( WndSrch( wnd )->num_rows );
}


static walk_result AddSrcFile( cue_handle *cueh, void *d )
{
    a_cue       *file;
    srch_window *srch = d;
    int         len;

    len = DIPCueFile( cueh, NULL, 0 ) + 1;
    file = WndMustAlloc( sizeof( a_cue ) + cue_SIZE + len );
    file->cueh = (cue_handle*)((char*)file + sizeof( a_cue ) + len  );
    DIPCueFile( cueh, file->name, len );
    HDLAssign( cue, file->cueh, cueh );
    file->next = srch->file_list;
    srch->file_list = file;
    return( WR_CONTINUE );
}

static walk_result SearchSrcFile( srch_window *srch, cue_handle *cueh )
{
    void        *viewhndl;
    const char  *pos,*endpos;
    found_item  *found;
    unsigned    i;
    int         len;

    viewhndl = OpenSrcFile( cueh );
    if( viewhndl == NULL )
        return( WR_CONTINUE );
    DIPCueFile( cueh, TxtBuff, TXT_LEN );
    WndStatusText( TxtBuff );
    for( i = 1; (len = FReadLine( viewhndl, i, 0, TxtBuff, TXT_LEN )) != FREADLINE_ERROR; ++i ) {
        TxtBuff[len] = NULLCHAR;
        pos = TxtBuff;
        endpos = NULL;
        if( WndRXFind( srch->rx, &pos, &endpos ) ) {
            found = WndRealloc( srch->found, ( srch->num_rows + 1 ) * sizeof( found_item ) );
            if( found == NULL )
                break;
            srch->found = found;
            found[srch->num_rows].mod = DIPCueMod( cueh );
            found[srch->num_rows].file_id = DIPCueFileId( cueh );
            found[srch->num_rows].open = false;
            found[srch->num_rows].source_line = DupStr( TxtBuff );
            srch->num_rows++;
            len = DIPModName( DIPCueMod( cueh ), NULL, 0 );
            if( srch->max_mod_name < len )
                srch->max_mod_name = len;
            break;
        }
    }
    if( len == FREADLINE_ERROR )
        TxtBuff[0] = NULLCHAR;
    FDoneSource( viewhndl );
    return( WR_CONTINUE );
}

static walk_result BuildFileList( mod_handle mh, void *d )
{
    if( DIPModHasInfo( mh, HK_CUE ) == DS_OK ) {
        DIPWalkFileList( mh, AddSrcFile, d );
    }
    return( WR_CONTINUE );
}


static int CueCompare( void *pa, void *pb )
{
    return( strcmp( (*(a_cue **)pa)->name, (*(a_cue **)pb)->name ) );
}

static void GlobalModWalker( srch_window *srch )
{
    a_cue       *file,*next;

    DIPWalkModList( NO_MOD, BuildFileList, srch );
    srch->file_list = SortLinkedList( srch->file_list,
                offsetof( a_cue, next ), CueCompare, WndAlloc, WndFree );
    for( file = srch->file_list; file != NULL; file = file->next ) {
        if( file->next != NULL && strcmp( file->name, file->next->name ) == 0 )
            continue;
        SearchSrcFile( srch, file->cueh );
    }
    for( file = srch->file_list; file != NULL; file = next ) {
        next = file->next;
        WndFree( file );
    }
    srch->file_list = NULL;
}


static void NoModWalker( srch_window *srch )
{
    /* unused parameters */ (void)srch;
}


static int FoundCompare( const void *a, const void *b )
{
    return( ModCompare( &((found_item const *)a)->mod, &((found_item const *)b)->mod ) );
}

static  void    SrchFreeFound( srch_window *srch )
{
    int         i;

    for( i = 0; i < srch->num_rows; ++i ) {
        WndFree( srch->found[i].source_line );
    }
    WndFree( srch->found );
    srch->found = NULL;
    srch->num_rows = 0;
}

static void     SrchInit( a_window wnd )
{
    srch_window *srch = WndSrch( wnd );

    SrchFreeFound( srch );
    srch->max_mod_name = 0;
    srch->ignore_case = WndGetSrchIgnoreCase();
    srch->use_rx = WndGetSrchRX();
    strcpy( srch->magic_str, WndGetSrchMagicChars() );
    srch->walk( srch );
    if( srch->num_rows != 0 ) {
        qsort( srch->found, srch->num_rows, sizeof( found_item ), FoundCompare );
    }
    WndZapped( wnd );
}


static void SrchMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    srch_window *srch = WndSrch( wnd );
    a_window    new;

    /* unused parameters */ (void)piece;

    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuEnable( wnd, MENU_SEARCH_SOURCE, ( row != WND_NO_ROW && row <= srch->num_rows ) );
        break;
    case MENU_SEARCH_SOURCE:
        new = WndModInspect( srch->found[row].mod );
        if( new == NULL )
            break;
        WndSetSrchIgnoreCase( srch->ignore_case );
        WndSetMagicStr( srch->use_rx ? srch->magic_str : LIT_ENG( Empty ) );
        WndSetSrchItem( new, srch->expr );
        WndSearch( new, true, 1 );
        break;
    }
}


static  bool    SrchGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    srch_window *srch = WndSrch( wnd );
    found_item  *found;

    if( row >= srch->num_rows )
        return( false );
    if( srch->found == NULL )
        return( false );
    found = &srch->found[row];
    switch( piece ) {
    case PIECE_OPENER:
        found->open = OpenGadget( wnd, line, found->mod, true );
        return( true );
    case PIECE_MODULE:
        DIPModName( found->mod, TxtBuff, TXT_LEN );
        line->text = TxtBuff;
        line->indent = MaxGadgetLength;
        line->extent = WND_MAX_EXTEND;
        return( true );
    case PIECE_SOURCE:
        line->indent = MaxGadgetLength;
        line->indent += ( srch->max_mod_name + 2 ) * WndAvgCharX( wnd );
        line->tabstop = false;
        line->use_prev_attr = true;
        if( found->source_line == NULL )
            return( false );
        line->text = found->source_line;
        return( true );
    default:
        return( false );
    }
}



static void     SrchRefresh( a_window wnd )
{
    srch_window *srch = WndSrch( wnd );
    found_item  *found;
    int         i;

    if( UpdateFlags & UP_SYMBOLS_LOST ) {
        if( UpdateFlags & UP_SYMBOLS_LOST ) {
            srch->walk = NoModWalker;
        }
        SrchInit( wnd );
    } else {
        for( i = 0; i < srch->num_rows; ++i ) {
            found = &srch->found[i];
            found->open = CheckOpenGadget( wnd, i, found->open, found->mod, true, PIECE_OPENER );
        }
    }
}


static bool SrchWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
{
    srch_window *srch = WndSrch( wnd );

    /* unused parameters */ (void)parm;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        SrchInit( wnd );
        WndSetKeyPiece( wnd, PIECE_MODULE );
        return( true );
    case GUI_DESTROY :
        WndFreeRX( srch->expr );
        WndFreeRX( srch->rx );
        SrchFreeFound( srch );
        WndFree( srch );
        return( true );
    }
    return( false );
}

static bool ChkUpdate( void )
{
    return( UpdateFlags & (UP_SYMBOLS_LOST | UP_OPEN_CHANGE) );
}

wnd_info SrchInfo = {
    SrchWndEventProc,
    SrchRefresh,
    SrchGetLine,
    SrchMenuItem,
    NoVScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    SrchNumRows,
    NoNextRow,
    NoNotify,
    ChkUpdate,
    PopUp( SrchMenu ),
};

static a_window DoWndSrchOpen( const char *expr, SRCH_WALKER *walk, void *cookie )
{
    srch_window *srch;
    void        *rx;

    srch = WndMustAlloc( sizeof( srch_window ) );
    srch->file_list = NULL;
    srch->expr = DupStr( expr );
    srch->cookie = cookie;
    srch->found = NULL;
    srch->num_rows = 0;
    srch->walk = walk;
    rx = WndCompileRX( expr );
    if( srch->expr == NULL || rx == NULL ) {
        WndFree( srch->expr );
        WndFree( rx );
        WndFree( srch );
        return( NULL );
    }
    srch->rx = rx;
    return( DbgWndCreate( LIT_DUI( WindowSearch ), &SrchInfo, WND_ALL, srch, &SrchIcon ) );
}

a_window WndSrchOpen( const char *expr )
{
    return( DoWndSrchOpen( expr, GlobalModWalker, NULL ) );
}
