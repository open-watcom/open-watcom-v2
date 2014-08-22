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


#include "vi.h"
#include "mouse.h"
#include "win.h"
#include "source.h"
#ifdef __WIN__
#include "font.h"
#include "utils.h"

extern int  MouseX, MouseY;
static int  last_start_line = 1, last_end_line = 1;
#endif

#ifndef __WIN__
/*
 * markRegion - mark a region as selected/unselected
 */
static void markRegion( bool val )
{
    fcb         *cfcb;
    line        *cline;
    linenum     ln;
    linenum     s, e;

    if( SelRgn.start.line > SelRgn.end.line ) {
        s = SelRgn.end.line;
        e = SelRgn.start.line;
    } else {
        s = SelRgn.start.line;
        e = SelRgn.end.line;
    }

    if( CGimmeLinePtr( s, &cfcb, &cline ) ) {
        return;
    }
    ln= e - s;
    for( ;; ) {
        cline->u.ld.hilite = val;
        ln--;
        if( ln < 0 ) {
            break;
        }
        if( CGimmeNextLinePtr( &cfcb, &cline ) ) {
            break;
        }
    }

} /* markRegion */
#endif

/*
 * GetFixedSelectedRegion - put the fixed selected region into
 *                          the select_rgn specified
 */
void GetFixedSelectedRegion( select_rgn *rgn )
{
    bool        swap_cols;

    swap_cols = false;
    if( SelRgn.start.line > SelRgn.end.line ) {
        rgn->start.line = SelRgn.end.line;
        rgn->end.line = SelRgn.start.line;
        swap_cols = true;
    } else {
        rgn->start.line = SelRgn.start.line;
        rgn->end.line = SelRgn.end.line;
    }
    if( SelRgn.start.line == SelRgn.end.line &&
        SelRgn.start.column > SelRgn.end.column ) {
        swap_cols = true;
    }
    if( swap_cols ) {
        rgn->start.column = SelRgn.end.column;
        rgn->end.column = SelRgn.start.column - 1;
    } else {
        rgn->start.column = SelRgn.start.column;
        rgn->end.column = SelRgn.end.column - 1;
    }
    rgn->lines = SelRgn.lines;
    rgn->selected = SelRgn.selected;
    rgn->dragging = SelRgn.dragging;
}

/*
 * InitSelectedRegion - set start of selected region
 */
void InitSelectedRegion( void )
{
    SelRgn.lines = false;
    SelRgn.start = CurrentPos;
    SelRgn.end = CurrentPos;
    SelRgn.start_col_v = VirtualColumnOnCurrentLine( CurrentPos.column );
#ifdef __WIN__
    last_start_line = CurrentPos.line;
    last_end_line = CurrentPos.line;
#endif

} /* InitSelectedRegion */

/*
 * SelRgnInit - initialize module statics
 */
void SelRgnInit( void )
{
}

/*
 * SelRgnFini - deinitialize module statics
 */
void SelRgnFini( void )
{
}

/*
 * updateRegion - update marked region
 */
static void updateRegion( void )
{
#ifdef __WIN__
    if( !SelRgn.selected ) {
        int min_line, max_line;
        min_line = last_start_line;
        if( min_line > last_start_line )
            min_line = last_start_line;
        max_line = last_start_line;
        if( max_line < last_end_line )
            max_line = last_end_line;
        DCDisplaySomeLines( min_line - LeftTopPos.line, max_line - LeftTopPos.line );
        last_start_line = last_end_line = 1;
        return;
    } else {
        int min_start, max_start;
        int min_end, max_end;
        min_start = SelRgn.start.line;
        if( min_start > last_start_line )
            min_start = last_start_line;
        max_start = SelRgn.start.line;
        if( max_start < last_start_line )
            max_start = last_start_line;
        min_end = SelRgn.end.line;
        if( min_end > last_end_line )
            min_end = last_end_line;
        max_end = SelRgn.end.line;
        if( max_end < last_end_line )
            max_end = last_end_line;
        DCDisplaySomeLines( min_start - LeftTopPos.line, max_start - LeftTopPos.line );
        DCDisplaySomeLines( min_end - LeftTopPos.line, max_end - LeftTopPos.line );
    }
    last_start_line = SelRgn.start.line;
    last_end_line = SelRgn.end.line;
#else
    markRegion( true );
    DCDisplayAllLines();
#endif

} /* updateRegion */

/*
 * UnselectRegion - remove selected region
 */
void UnselectRegion( void )
{
    if( !SelRgn.selected ) {
        return;
    }
    SelRgn.selected = false;
    EditFlags.Dragging = false;
#ifdef __WIN__
    updateRegion();
#else
    markRegion( false );
    DCDisplayAllLines();
#endif

} /* UnselectRegion */

/*
 * UpdateDrag - update selected region
 */
void UpdateDrag( window_id id, int win_x, int win_y )
{
    int         nx, ny, height;
    int         moveCursor;

    SelRgn.selected = true;
    moveCursor = 0;
    height = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
#ifdef __WIN__
    if( id == CurrentWindow && InsideWindow( id, MouseX, MouseY ) ) {
#else
    if( id == CurrentWindow && InsideWindow( id, win_x, win_y ) ) {
#endif
        ny = LeftTopPos.line + win_y - 1;
        if( ny > CurrentFile->fcbs.tail->end_line ) {
            ny = CurrentFile->fcbs.tail->end_line;
            moveCursor = 1;
        } else if( ny < 1 ) {
            ny = 1;
            moveCursor = -1;
        }
        GoToLineRelCurs( ny );
        win_x += LeftTopPos.column;
        nx = RealColumnOnCurrentLine( win_x );
        GoToColumnOnCurrentLine( nx );
    } else {
#ifndef __WIN__
        if( MouseRow >= WindowAuxInfo( CurrentWindow, WIND_INFO_Y2 ) ) {
            GoToLineRelCurs( LeftTopPos.line + height );
        } else if( MouseRow <= WindowAuxInfo( CurrentWindow, WIND_INFO_Y1 ) ) {
            GoToLineRelCurs( LeftTopPos.line - 1 );
        } else if( MouseCol <= WindowAuxInfo( CurrentWindow, WIND_INFO_X1 ) ) {
            GoToColumnOnCurrentLine( LeftTopPos.column - 1 );
        } else if( MouseCol >= WindowAuxInfo( CurrentWindow, WIND_INFO_X2 ) ) {
            GoToColumnOnCurrentLine( LeftTopPos.column + WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH ));
        }
#else
        {
            RECT            rect;

            GetClientRect( CurrentWindow, &rect );
            if( MouseY > rect.bottom ) {
                ny = LeftTopPos.line + height;
                if( ny > CurrentFile->fcbs.tail->end_line ) {
                    ny = CurrentFile->fcbs.tail->end_line;
                    moveCursor = 1;
                }
                GoToLineRelCurs( ny );
            } else if( MouseY < 0 ) {
                ny = LeftTopPos.line - 1;
                if( ny < 1 ) {
                    ny = 1;
                    moveCursor = -1;
                }
                GoToLineRelCurs( ny );
            } else if( MouseX < 0 ) {
                GoToColumnOnCurrentLine( LeftTopPos.column - 1 );
            } else if( MouseX > rect.right ) {
                if( EditFlags.Modeless ) {
                    GoToColumnOnCurrentLine( 1 + LeftTopPos.column +
                        WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS ) );
                } else {
                    GoToColumnOnCurrentLine( LeftTopPos.column +
                        WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS ) );
                }
            }
        }
#endif
    }

    if( moveCursor == -1 ) {
        GoToColumnOnCurrentLine( 1 );
    } else if( moveCursor == 1 ) {
        GoToColumnOnCurrentLine( CurrentFile->fcbs.tail->lines.tail->len + 1 );
    }

} /* UpdateDrag */

/*
 * UpdateCursorDrag - update drag after cursor movement
 */
void UpdateCursorDrag( void )
{
    if( !EditFlags.Dragging ) {
        return;
    }
    if( SelRgn.end.line == CurrentPos.line && SelRgn.end.column == CurrentPos.column ) {
        return;
    }
#ifndef __WIN__
    markRegion( false );
#endif
    SelRgn.end = CurrentPos;
    if( !EditFlags.LineBased ) {
        SelRgn.lines = false;
    } else if( SelRgn.start.line != SelRgn.end.line ) {
        SelRgn.lines = true;
    } else if( SelRgn.start.column == SelRgn.end.column ) {
        SelRgn.lines = true;
    } else {
        SelRgn.lines = false;
    }
    updateRegion();

} /* UpdateCursorDrag */

/*
 * SetSelRegionCols - set selected region on a line
 */
void SetSelRegionCols( linenum sl, int sc, int ec )
{
    vi_rc   rc;
    line    *line;
    fcb     *fcb;
    char    *data;

    SelRgn.lines = false;
    SelRgn.selected = true;
    SelRgn.start.line = SelRgn.end.line = sl;
    SelRgn.start.column = sc;
    SelRgn.end.column = ec + 1;
    SelRgn.start_col_v = 0;
    rc = CGimmeLinePtr( sl, &fcb, &line );
    if( rc == ERR_NO_ERR ) {
        data = ( line->u.ld.nolinedata ) ? WorkLine->data : line->data;
        SelRgn.start_col_v = GetVirtualCursorPosition( data, SelRgn.start.column );
    }

    updateRegion();
    DCUpdate();

} /* SetSelRegionCols */

/*
 * ReselectRegion - restore the previous selection.
 */
vi_rc ReselectRegion( void )
{
    if( SelRgn.start.line != 0 && SelRgn.end.line != 0 ) {
        SelRgn.selected = true;
        EditFlags.Dragging = true;
        updateRegion();
        return( ERR_NO_ERR );
    }
    return( ERR_NO_REGION );

} /* ReselectRegion */

/*
 * startSelectedRegion - start selection region area from keyboard
 */
vi_rc startSelectedRegion( bool line_based )
{
    if( ShiftDown() && SelRgn.selected ) {
        EditFlags.Dragging = true;
        UpdateCursorDrag();
    } else {
        if( SelRgn.selected ) {
            UnselectRegion();
            return( ERR_NO_ERR );
        }
        EditFlags.Dragging = true;
        InitSelectedRegion();
        SelRgn.lines = line_based;
        SelRgn.selected = true;
        updateRegion();
    }
    return( ERR_NO_ERR );

} /* startSelectedRegion */

/*
 * SelectLeft - update selected region, moving left
 */
vi_rc SelectLeft( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return MoveLeft( r, count );

} /* SelectLeft */

/*
 * SelectRight - update selected region, moving right
 */
vi_rc SelectRight( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return MoveRight( r, count );

} /* SelectRight */

/*
 * SelectUp - update selected region, moving up
 */
vi_rc SelectUp( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( GoToLineRelCurs( CurrentPos.line - 1 ) );

} /* SelectUp */

/*
 * SelectDown - update selected region, moving down
 */
vi_rc SelectDown( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( GoToLineRelCurs( CurrentPos.line + 1 ) );

} /* SelectDown */

/*
 * SelectHome - update selected region, moving to beginning of line
 */
vi_rc SelectHome( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( GoToColumnOnCurrentLine( 1 ) );

} /* SelectHome */

/*
 * SelectEnd - update selected region, moving to end of line
 */
vi_rc SelectEnd( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    CurrentPos.column = MAX_INPUT_LINE;
    ValidateCurrentColumn();
    return( GoToColumnOnCurrentLine( CurrentPos.column ) );

} /* SelectEnd */

/*
 * SelectPageUp - update selected region, moving one page up
 */
vi_rc SelectPageUp( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( MovePage( -1, 1, true ) );

} /* SelectPageUp */

/*
 * SelectPageDown - update selected region, moving one page down
 */
vi_rc SelectPageDown( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( MovePage( 1, 1, true ) );

} /* SelectPageDown */

vi_rc SelectBackwardsWord( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveBackwardsWord( r, count ) );
}

vi_rc SelectForwardWord( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveForwardWord( r, count ) );
}

vi_rc SelectStartOfFile( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveStartOfFile( r, count ) );
}

vi_rc SelectEndOfFile( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveEndOfFile( r, count ) );
}

vi_rc SelectTopOfPage( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveTopOfPage( r, count ) );
}

vi_rc SelectBottomOfPage( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( false );
    }
    return( MoveBottomOfPage( r, count ) );
}

/*
 * DoSelectSelection - selected region was selected, invoke aprops script
 */
vi_rc DoSelectSelection( bool doMenu )
{
    linenum     sl, el, tl;
    int         sc, ec, tc;
    vi_rc       rc = ERR_NO_ERR;

    if( SelRgn.selected && doMenu ) {
        sl = SelRgn.start.line;
        el = SelRgn.end.line;
        if( sl > el ) {
            tl = sl;
            sl = el;
            el = tl;
        }
        if( CurrentPos.line >= sl && CurrentPos.line <= el ) {
            if( sl == el && !SelRgn.lines ) {
                sc = SelRgn.start.column;
                ec = SelRgn.end.column;
                if( sc > ec ) {
                    sc--;
                    tc = sc;
                    sc = ec;
                    ec = tc;
                } else if( sc < ec ) {
                    ec--;
                }

                if( CurrentPos.column >= sc && CurrentPos.column <= (ec + 1) ) {
                    rc = InvokeColSelHook( sc, ec );
                }
            } else {
                rc = InvokeLineSelHook( SelRgn.start.line, SelRgn.end.line );
            }
            UnselectRegion();
            return( rc );
        }
        UnselectRegion();
    }
    if( ShiftDown() && LastEvent != '_' ) {
        sc = 1;
        ec = CurrentLine->len;
        SetSelRegionCols( CurrentPos.line, sc, ec );
        if( doMenu ) {
            rc = InvokeLineSelHook( CurrentPos.line, CurrentPos.line );
        }
    } else {
        rc = GimmeCurrentEntireWordDim( &sc, &ec, false );
        if( rc != ERR_NO_ERR ) {
            return( ERR_NO_ERR );
        }
        SetSelRegionCols( CurrentPos.line, sc, ec );
        if( doMenu ) {
            rc = InvokeColSelHook( sc, ec );
        }
    }
    if( doMenu ) {
        UnselectRegion();
    }
    return( rc );

} /* DoSelectSelection */

/*
 * DoSelectSelectionPopMenu - $%!@ DoSelectSelection that always pops
 */
vi_rc DoSelectSelectionPopMenu( void )
{
    return( DoSelectSelection( true ) );
}

/*
 * GetSelectedRegion - get the current selected region
 */
vi_rc GetSelectedRegion( range *r )
{
    vi_rc       rc;

    rc = ERR_NO_SELECTION;
    if( SelRgn.selected ) {
        r->line_based = SelRgn.lines;
        r->start.line = SelRgn.start.line;
        r->end.line = SelRgn.end.line;
        if( (SelRgn.start.line > SelRgn.end.line) ||
            (SelRgn.start.line == SelRgn.end.line &&
            SelRgn.end.column < SelRgn.start.column) ) {
            // don't include char at SelRgn.start.column
            r->end.column = SelRgn.end.column;
            r->start.column = SelRgn.start.column - 1;
        } else {
            // don't include char at SelRgn.end.column
            r->end.column = SelRgn.end.column - 1;
            r->start.column = SelRgn.start.column;
        }
        r->fix_range = false;
        rc = ERR_NO_ERR;
    }
    return( rc );

} /* GetSelectedRegion */

/*
 * SetSelectedRegion - set the selected region to a specific area
 */
vi_rc SetSelectedRegion( range *r )
{
    vi_rc   rc;
    line    *line;
    fcb     *fcb;

    UnselectRegion();
    SelRgn.start.line = r->start.line;
    SelRgn.end.line = r->end.line;

    SelRgn.start_col_v = 1;
    if( r->line_based ) {
        SelRgn.lines = true;
        SelRgn.start.column = 1;
        SelRgn.end.column = LineLength( r->end.line );
    } else {
        SelRgn.lines = false;
        if( r->start.column < r->end.column ) {
            SelRgn.start.column = r->start.column + 1;
            SelRgn.end.column = r->end.column + 2;
        } else {
            SelRgn.start.column = r->end.column + 1;
            SelRgn.end.column = r->start.column + 2;
        }
        rc = CGimmeLinePtr( SelRgn.start.line, &fcb, &line );
        if( rc == ERR_NO_ERR ) {
            SelRgn.start_col_v = WinVirtualCursorPosition( line->data,
                                                           SelRgn.start.column );
        }
    }
    SelRgn.selected = true;
    updateRegion();
    return( ERR_NO_ERR );

} /* SetSelectedRegion */

/*
 * SetSelectedRegionFromLine - set the SelRgn w/ start_v
 */
vi_rc SetSelectedRegionFromLine( range *r, linenum lineno )
{
    vi_rc   rc;
    fcb     *fcb;
    line    *line;
    char    *data;

    SetSelectedRegion( r );

    rc = CGimmeLinePtr( lineno, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    data = line->u.ld.nolinedata ? WorkLine->data : line->data;
    SelRgn.start_col_v = GetVirtualCursorPosition( data, SelRgn.start.column );

    return( ERR_NO_ERR );

} /* SetSelectedRegionFromLine */

/*
 * NormalizeRange - convert a range to the (external) selrgn format
 */
void NormalizeRange( range *r )
{
    i_mark      tmp;

    if( r->start.line > r->end.line ) {
        tmp = r->start;
        r->start = r->end;
        r->end = tmp;
    } else if( r->start.line == r->end.line ) {
        if( r->start.column > r->end.column ) {
            tmp = r->start;
            r->start = r->end;
            r->end = tmp;
        }
    }
    if( r->line_based ) {
        r->start.column = 1;
        if( r->fix_range && r->end.column == 1 ) {
            r->end.line -= 1;
        }
        r->end.column = LineLength( r->end.line );
    } else {
        if( r->fix_range ) {
            r->end.column -= 1;
            if( r->end.column == 0 ) {
                r->end.line -= 1;
                r->end.column = LineLength( r->end.line );
            }
        }
    }

    /* change these to be zero-based instead of one-based */
    r->start.column -= 1;
    r->end.column -= 1;

} /* NormalizeRange */

/*
 * SelectAll - select the entire contents of the file
 */
vi_rc SelectAll( void )
{
    range   r;
    vi_rc   rc;
    r.line_based = true;
    r.start.line = 1;
    CFindLastLine( &r.end.line );
    rc = SetSelectedRegion( &r );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    rc = GoToLineNoRelCurs( r.end.line );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    return( GoToColumnOK( LineLength( r.end.line ) + 1 ) );

} /* SelectAll */
