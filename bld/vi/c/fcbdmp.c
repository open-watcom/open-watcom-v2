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
* Description:  Dump File Control Blocks.
*
****************************************************************************/


#include "vi.h"
#ifdef _M_I86
    #include <i86.h>
#endif
#include "win.h"

#ifdef DBG
#include <malloc.h>
static type_style errStyle = { 7, 0, 0 };
#endif

#ifdef DBG
void HeapMsg( int msg )
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
#ifdef DBG
    int                 i;
    struct _heapinfo    hinfo;

    i = _heapchk();
    Message1( "_heapchk has returned" );
    HeapMsg( i );
    if( GetKeyboard() == 'q' ) {
        return( ERR_NO_ERR );
    }
    hinfo._pentry = NULL;
    while( 1 ) {
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
#ifdef DBG
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
    WPrintfLine( fw, 4, "Bytes_pending: %d", (int) CurrentFile->bytes_pending );
    WPrintfLine( fw, 5, "Modified: %d", (int) CurrentFile->modified );
    lc = 7;
    for( cfcb = CurrentFile->fcbs.head; cfcb != NULL; cfcb = cfcb->next ) {
        fcbcnt++;
        WPrintfLine( fw, lc++, "%d) %W - (%l,%l) bytes:%d offset:%l lstswp:%l xaddr:%W", fcbcnt,
            cfcb, cfcb->start_line, cfcb->end_line, cfcb->byte_cnt, cfcb->offset,
            cfcb->last_swap, cfcb->xmemaddr );
        WPrintfLine( fw, lc++, "    swp:%d in:%d dsp:%d ded:%d nswp:%d xmem:%d xms:%d.   next=%W,prev=%W",
            (int) cfcb->swapped, (int) cfcb->in_memory, (int) cfcb->on_display,
            (int) cfcb->dead, (int) cfcb->non_swappable, (int) cfcb->in_extended_memory,
            (int) cfcb->in_xms_memory, cfcb->next,cfcb->prev );

        if( lc > 22 || cfcb->next == NULL ) {
            if( GetKeyboard() == 'q' ) {
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
#ifdef DBG
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
    cfcb = FcbThreadHead;

    while( cfcb != NULL ) {

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
            if( GetKeyboard() == 'q' ) {
                break;
            }
            ClearWindow( fw );
            lc = 1;
        }
        cfcb = cfcb->thread_next;

    }

    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );
}

vi_rc SanityCheck( void )
{
#ifdef DBG
    int         lc, tfcbcnt = 0, fcbcnt, sum;
    window_id   fw;
    fcb         *cfcb;
    info        *inf;
    linenum     cl, lcnt;
    vi_rc       rc;

    EditFlags.WatchForBreak = TRUE;
    if( (rc = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle )) ) {
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
                if( GetKeyboard() == 'q' ) {
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
    EditFlags.WatchForBreak = EditFlags.BreakPressed = FALSE;

    CloseAWindow( fw );
#endif
    return( ERR_NO_ERR );
}

vi_rc LineInfo( void )
{
#ifdef DBG
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
#ifdef DBG
    int         ln = 1, i, col, fcbcnt, depth = 0;
    window_id   fw;
    linenum     lne, lcnt;
    undo        *cundo;
    fcb         *cfcb;

    if( UndoStack->current < 0 ) {
        return( ERR_NO_ERR );
    }
    if( (i = NewWindow( &fw, 0, 0, 79, 24, 1, LIGHT_GREEN, BLACK, &errStyle )) ) {
        return( ERR_NO_ERR );
    }

    cundo = UndoStack->stack[UndoStack->current];

    while( TRUE ) {
        switch( cundo->type ) {
        case START_UNDO_GROUP:
            depth--;
            if( cundo->data.sdata.depth == 1 ) {
                lne = cundo->data.sdata.line;
                col = cundo->data.sdata.col;
                WPrintfLine( fw, ln++, "START_UNDO_GROUP(%d): lne=%l, col= %d", depth,
                    lne, col );
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

#ifdef DBG
void CheckFcb( fcb *cfcb, int *bcnt, linenum *lnecnt )
{
    line        *cline;

    *bcnt = 0;
    *lnecnt = 0;
    cline = cfcb->line_head;
    while( cline != NULL ) {
        *bcnt += cline->len + 1;
        *lnecnt += 1;
        cline = cline->next;
    }

} /* CheckFcb */

#endif

#ifdef __DOS__
#include "xmem.h"
#ifndef NOXTD
extern xtd_struct XMemCtrl;
#endif
#ifndef NOEMS
extern ems_struct EMSCtrl;
#endif
#ifndef NOXMS
extern xms_struct XMSCtrl;
#endif
#endif
static char freeBytes[] =  "%s:  %l bytes free (%d%%)";
static char twoStr[] = "%Y%s";
extern int maxStatic;
//extern long __undocnt;

/*
 * DumpMemory - dump memory avaliable
 */
vi_rc DumpMemory( void )
{
    int         ln = 1;
    window_id   wn;
    window_info *wi;
    char        tmp[128], tmp2[128];
#if !defined( __WIN__ ) && !defined( __386__ ) && !defined( __OS2__ ) && \
    !defined( __UNIX__ ) && !defined( __ALPHA__ )
    long        mem1;
#endif
    long        mem2;
    vi_rc       rc;

    wi = &filecw_info;
    rc = NewWindow2( &wn, wi );
#if defined(__OS2__ )
    WPrintfLine( wn, ln++, "Mem:  (unlimited) (maxStatic=%d)", maxStatic );
#else
    WPrintfLine( wn, ln++, "Mem:  %l bytes memory (%l for editing) (maxStatic=%d)",
        MaxMemFree, MaxMemFreeAfterInit, maxStatic );
#endif

    mem2 = (MaxSwapBlocks - SwapBlocksInUse) * (long) MAX_IO_BUFFER;
    MySprintf( tmp, freeBytes, "Dsk", mem2,
        (int) ((100L * mem2) / ((long)MaxSwapBlocks * (long)MAX_IO_BUFFER)) );
#ifdef __386__
    MySprintf( tmp2, "386 Flat memory addressing" );
#else
#ifndef NOXTD
    if( XMemCtrl.inuse ) {
        mem1 = XMemCtrl.amount_left - XMemCtrl.allocated * (long) MAX_IO_BUFFER;
        MySprintf( tmp2, freeBytes, "XTD", mem1,
            (int) ((100L * mem1) / XMemCtrl.amount_left) );
    } else {
#endif
        MySprintf( tmp2, "XTD: N/A" );
#ifndef NOXTD
    }
#endif

#endif
    WPrintfLine( wn, ln++, twoStr, tmp, tmp2 );

#ifndef NOEMS
    if( EMSCtrl.inuse ) {
        mem1 = (long)(TotalEMSBlocks - EMSBlocksInUse) * (long)MAX_IO_BUFFER;
        MySprintf( tmp, freeBytes, "EMS", mem1,
                (int) ((100L * mem1) / ((long)TotalEMSBlocks * (long)MAX_IO_BUFFER)) );
    } else {
#endif
        MySprintf( tmp, "EMS:  N/A" );
#ifndef NOEMS
    }
#endif
#ifndef NOXMS
    if( XMSCtrl.inuse ) {
        mem1 = (long)(TotalXMSBlocks - XMSBlocksInUse) * (long)MAX_IO_BUFFER;
        MySprintf( tmp2, freeBytes, "XMS", mem1,
            (int) ((100L * mem1) / ((long)TotalXMSBlocks * (long)MAX_IO_BUFFER)) );
    } else {
#endif
        MySprintf( tmp2, "XMS: N/A" );
#ifndef NOXMS
    }
#endif
    WPrintfLine( wn, ln++, twoStr, tmp, tmp2 );
//    WPrintfLine( wn, ln++, "Reserved %l bytes of DOS memory", MinMemoryLeft );

    WPrintfLine( wn, ln++, "File CB's: %d", FcbBlocksInUse );
//    WPrintfLine( wn, ln++, "File CB's: %d, Undo blocks=%l(%l bytes)", FcbBlocksInUse,
//        __undocnt, __undocnt * sizeof( undo ) );

    WPrintfLine( wn, ln + 1, MSG_PRESSANYKEY );

    GetNextEvent( FALSE );
    CloseAWindow( wn );
    return( ERR_NO_ERR );

} /* DumpMemory */
