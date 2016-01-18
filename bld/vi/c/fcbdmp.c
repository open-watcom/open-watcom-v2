/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Dump File Control Blocks and memory usage.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include <malloc.h>

#if !defined( NDEBUG ) && !defined( __WIN__ )

void CheckFcb( fcb *cfcb, int *bcnt, linenum *lnecnt );

static type_style errStyle = DEF_TEXT_STYLE;

#endif

#if defined( __WATCOMC__ ) && !defined( NDEBUG )
static void HeapMsg( int msg )
{
    switch( msg ) {
    case _HEAPOK:
        Message2("_HEAPOK");
        break;
    case _HEAPEMPTY:
        Message2("_HEAPEMPTY");
        break;
    case _HEAPBADBEGIN:
        Message2("_HEAPBADBEGIN");
        break;
    case _HEAPBADNODE:
        Message2("_HEAPBADNODE");
        break;
    case _HEAPBADPTR:
        Message2("_HEAPBADPTR");
        break;
    case _HEAPEND:
        Message2("_HEAPEND");
        break;
    }
} /* HeapMsg */
#endif

vi_rc HeapCheck( void )
{
#if defined( __WATCOMC__ ) && !defined( NDEBUG )
    int                 i;
    struct _heapinfo    hinfo;

    i = _heapchk();
    Message1( "_heapchk has returned" );
    HeapMsg( i );
    if( GetKeyboard() == VI_KEY( q ) ) {
        return( ERR_NO_ERR );
    }
    hinfo._pentry = NULL;
    for( ;; ) {
#if defined(__NT__) || defined(__OS2V2__)
        i = _nheapwalk( &hinfo );
#else
        i = _heapwalk( &hinfo );
#endif
        if( i != _HEAPOK ) {
            Message1( "_heapwalk:  %s block at %W of size %d",
                      (hinfo._useflag == _USEDENTRY ? "USED" : "FREE"),
                      hinfo._pentry, hinfo._size );
            HeapMsg( i );
            break;
        }
    }
#endif
    return( ERR_NO_ERR );

} /* HeapCheck */

vi_rc FcbDump( void )
{
#if !defined( NDEBUG ) && !defined( __WIN__ )
    int         lc, fcbcnt = 0;
    window_id   fw;
    fcb         *cfcb;
    vi_rc       rc;

    rc = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    WPrintfLine( fw, 1, "File name: %s", CurrentFile->name );
    WPrintfLine( fw, 2, "File home: %s", CurrentFile->home );
    WPrintfLine( fw, 3, "File handle: %d,  current position: %l", CurrentFile->handle, CurrentFile->curr_pos );
    WPrintfLine( fw, 4, "Bytes_pending: %d", (int)CurrentFile->bytes_pending );
    WPrintfLine( fw, 5, "Modified: %d", (int)CurrentFile->modified );
    lc = 7;
    for( cfcb = CurrentFile->fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
        fcbcnt++;
        WPrintfLine( fw, lc++, "%d) %W - (%l,%l) bytes:%d offset:%l lstswp:%l xaddr:%W", fcbcnt,
            cfcb, cfcb->start_line, cfcb->end_line, cfcb->byte_cnt, cfcb->offset,
            cfcb->last_swap, cfcb->xmemaddr );
        WPrintfLine( fw, lc++, "    swp:%d in:%d dsp:%d ded:%d nswp:%d xmem:%d xms:%d.",
            (int)cfcb->swapped, (int)cfcb->in_memory, (int)cfcb->on_display,
            (int)cfcb->dead, (int)cfcb->non_swappable, (int)cfcb->in_extended_memory,
            (int)cfcb->in_xms_memory );
        WPrintfLine( fw, lc++, "    next=%W,prev=%W", cfcb->next, cfcb->prev );

        if( lc > 22 || cfcb->next == NULL ) {
            if( GetKeyboard() == VI_KEY( q ) ) {
                break;
            }
            ClearWindow( fw );
            lc = 1;
        }
    }

    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );

}

vi_rc FcbThreadDump( void )
{
#if !defined( NDEBUG ) && !defined( __WIN__ )
    int         lc, fcbcnt = 0;
    window_id   fw;
    char        msg[80];
    fcb         *cfcb;
    file        *cfile;
    vi_rc       rc;

    rc = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    lc = 1;
    for( cfcb = FcbThreadHead; cfcb != NULL; cfcb = cfcb->thread_next ) {
        fcbcnt++;
        cfile = cfcb->f;
        if( cfcb->dead ) {
            strcpy( msg, "** dead fcb **" );
        } else {
                if( cfile != NULL ) {
                    strcpy( msg, cfile->name );
                } else {
                    strcpy( msg, "** no file **" );
                }
        }
        WPrintfLine( fw, lc++, "%d) %d bytes, belongs to %s, lock=%d%d%d, mem=%d%d%d%d%d",
            fcbcnt, cfcb->byte_cnt, msg, abs( cfcb->globalmatch ),
            abs( cfcb->on_display ), abs( cfcb->non_swappable ), abs( cfcb->in_memory ),
            abs( cfcb->in_extended_memory ),abs( cfcb->in_ems_memory ),
            abs( cfcb->in_xms_memory ), abs( cfcb->swapped ) );
        if( lc > 22 || cfcb->thread_next == NULL ) {
            if( GetKeyboard() == VI_KEY( q ) ) {
                break;
            }
            ClearWindow( fw );
            lc = 1;
        }
    }

    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );
}

vi_rc SanityCheck( void )
{
#if !defined( NDEBUG ) && !defined( __WIN__ )
    int         lc, tfcbcnt = 0, fcbcnt, sum;
    window_id   fw;
    fcb         *cfcb;
    info        *inf;
    linenum     cl, lcnt;
    vi_rc       rc;

    EditFlags.WatchForBreak = true;
    rc = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    lc = 1;
    for( inf = InfoHead; inf != NULL; inf = inf->next ) {

        WPrintfLine( fw, lc++, "File name: %s", inf->CurrentFile->name );
        fcbcnt = 0;
        cl = 1;
        for( cfcb = inf->CurrentFile->fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
            fcbcnt++;
            tfcbcnt++;
            WPrintfLine( fw, lc, "At fcb %d", fcbcnt );
            FetchFcb( cfcb );
            CheckFcb( cfcb, &sum, &lcnt );
            if( sum != cfcb->byte_cnt ) {
                WPrintfLine( fw, lc++,
                    "Fcb %d has bad bytecnt: had %d, should have had %d", fcbcnt, sum,
                    cfcb->byte_cnt );
            } else {
                if( FcbSize( cfcb ) > MAX_IO_BUFFER )
                    WPrintfLine( fw, lc++, "Fcb %d too big : has %d (max is %d)", fcbcnt,
                    FcbSize( cfcb ), MAX_IO_BUFFER );
            }
            if( lcnt != (cfcb->end_line-cfcb->start_line + 1) ) {
                WPrintfLine( fw, lc++,
                    "Fcb %d has invalid lines: count is %l, should be %l", fcbcnt, lcnt,
                    cfcb->end_line-cfcb->start_line + 1 );
            }
            if( cfcb->start_line != cl ) {
                WPrintfLine( fw, lc++,
                    "Fcb %d has invalid lines: start is %l, should be %l", fcbcnt,
                    cfcb->start_line, cl );
            }

            if( cfcb->next == NULL ) {
                WPrintfLine( fw, lc++, "Check of %s done, %d fcbs processed",
                    inf->CurrentFile->name, fcbcnt );
            }
            if( lc > 18 || (inf->next == NULL && cfcb->next == NULL) ) {
                if( inf->next == NULL ) {
                    WPrintfLine( fw, lc, "Sanity check done, %d fcbs processed",
                        tfcbcnt );
                    WPrintfLine( fw, lc + 1, MSG_PRESSANYKEY );
                }
                if( GetKeyboard() == VI_KEY( q ) ) {
                    break;
                }
                ClearWindow( fw );
                lc = 1;
            }
            cl = cfcb->end_line + 1;
            if( EditFlags.BreakPressed ) {
                break;
            }
        }
        if( EditFlags.BreakPressed ) {
            break;
        }
    }
    EditFlags.WatchForBreak = false;
    EditFlags.BreakPressed = false;

    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );
}

vi_rc LineInfo( void )
{
#if !defined( NDEBUG ) && !defined( __WIN__ )
    fcb         *cfcb;
    int         fcbcnt = 1;
    int         bcnt;
    linenum     lcnt;

    for( cfcb = CurrentFile->fcbs.head; cfcb != CurrentFcb; cfcb = cfcb->next ) {
        fcbcnt++;
    }
    CheckFcb( CurrentFcb, &bcnt, &lcnt );
    Message1( "Length=%d, Allocated=%d.  Line->prev=%W, Line->next=%W",
        CurrentLine->len, CurrentLine->len + 1, CurrentLine->prev, CurrentLine->next );
    Message2( "Fcb %d: ptr=%W,bcnt=%d,lcnt=%l  Fcb->prev=%W, Fcb->next=%W", fcbcnt,
        CurrentFcb, bcnt, lcnt, CurrentFcb->prev, CurrentFcb->next );
#endif
    return( ERR_NO_ERR );
}

/*
 * WalkUndo
 */
vi_rc WalkUndo( void )
{
#if !defined( NDEBUG ) && !defined( __WIN__ )
    int         ln, col, fcbcnt, depth;
    window_id   fw;
    linenum     lne, lcnt;
    undo        *cundo;
    fcb         *cfcb;
    vi_rc       rc;

    if( UndoStack->current < 0 ) {
        return( ERR_NO_ERR );
    }
    rc = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle );
    if( rc != ERR_NO_ERR ) {
        return( ERR_NO_ERR );
    }

    cundo = UndoStack->stack[UndoStack->current];

    ln = 1;
    depth = 0;
    for( ;; ) {
        switch( cundo->type ) {
        case START_UNDO_GROUP:
            depth--;
            if( cundo->data.sdata.depth == 1 ) {
                lne = cundo->data.sdata.p.line;
                col = cundo->data.sdata.p.column;
                WPrintfLine( fw, ln++, "START_UNDO_GROUP(%d): lne=%l, col= %d", depth, lne, col );
            } else {
                WPrintfLine( fw, ln++, "START_UNDO_GROUP(%d)", depth );
            }
            break;

        case END_UNDO_GROUP:
            depth++;
            WPrintfLine( fw, ln++, "END_UNDO_GROUP(%d)", depth );
            break;

        case UNDO_INSERT_LINES:
            WPrintfLine( fw, ln++, "UNDO_INSERT_LINES %l,%l",
                cundo->data.del_range.start, cundo->data.del_range.end );
            break;

        case UNDO_DELETE_FCBS:
            lcnt = 0;
            fcbcnt = 0;
            for( cfcb = cundo->data.fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
                lcnt += cfcb->end_line - cfcb->start_line + 1;
                fcbcnt++;
            }
            WPrintfLine( fw, ln++, "UNDO_DELETE_FCBS: start=%l lines=%l fcbs=%d",
                cundo->data.fcbs.head->start_line, lcnt, fcbcnt );
            break;
        }
        cundo = cundo->next;
        if( cundo == NULL || ln == 20 ) {
            GetKeyboard();
            ln = 1;
            if( cundo != NULL ) {
                ClearWindow( fw );
            }
        }
        if( cundo == NULL ) {
            break;
        }
    }
    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );

} /* WalkUndo */

#if !defined( NDEBUG ) && !defined( __WIN__ )
void CheckFcb( fcb *cfcb, int *bcnt, linenum *lnecnt )
{
    line        *cline;

    *bcnt = 0;
    *lnecnt = 0;
    for( cline = cfcb->lines.head; cline != NULL; cline = cline->next ) {
        *bcnt += cline->len + 1;
        *lnecnt += 1;
    }

} /* CheckFcb */
#endif
