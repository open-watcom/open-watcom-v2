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
#include <string.h>
#include <stdio.h>
#include "vi.h"
#include "mouse.h"
#include "win.h"
#include "source.h"
#include "keys.h"
#ifdef __WIN__
#include "winvi.h"
#include "font.h"
#include "utils.h"


extern int MouseX, MouseY;
static int last_start_line = 1, last_end_line = 1;
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
    linenum     s,e;

    if( SelRgn.start_line > SelRgn.end_line ) {
        s = SelRgn.end_line;
        e = SelRgn.start_line;
    } else {
        s = SelRgn.start_line;
        e = SelRgn.end_line;
    }

    if( CGimmeLinePtr( s, &cfcb, &cline ) ) {
        return;
    }
    ln= e - s;
    while( 1 ) {
        cline->inf.ld.hilite = val;
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
 * the select_rgn specified
 */
void GetFixedSelectedRegion( select_rgn *rgn )
{
    bool        swap_cols;

    swap_cols = FALSE;
    if( SelRgn.start_line > SelRgn.end_line ) {
        rgn->start_line = SelRgn.end_line;
        rgn->end_line = SelRgn.start_line;
        swap_cols = TRUE;
    } else {
        rgn->start_line = SelRgn.start_line;
        rgn->end_line = SelRgn.end_line;
    }
    if( SelRgn.start_line == SelRgn.end_line &&
        SelRgn.start_col > SelRgn.end_col ) {
        swap_cols = TRUE;
    }
    if( swap_cols ) {
        rgn->start_col = SelRgn.end_col;
        rgn->end_col = SelRgn.start_col - 1;
    } else {
        rgn->start_col = SelRgn.start_col;
        rgn->end_col = SelRgn.end_col - 1;
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
    SelRgn.lines = FALSE;
    SelRgn.start_line = CurrentLineNumber;
    SelRgn.end_line = CurrentLineNumber;
    SelRgn.start_col_v = VirtualCursorPosition();
#ifdef __WIN__
    last_start_line = CurrentLineNumber;
    last_end_line = CurrentLineNumber;
#endif
    SelRgn.start_col = CurrentColumn;
    SelRgn.end_col = CurrentColumn;

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
    if( SelRgn.selected == FALSE ) {
        DCDisplaySomeLines( min( last_start_line, last_end_line ) - TopOfPage,
                            max( last_start_line, last_end_line ) - TopOfPage );
        last_start_line = last_end_line = 1;
        return;
    } else {
        DCDisplaySomeLines( min( SelRgn.start_line, last_start_line ) - TopOfPage,
                            max( SelRgn.start_line, last_start_line ) - TopOfPage );
        DCDisplaySomeLines( min( SelRgn.end_line, last_end_line ) - TopOfPage,
                            max( SelRgn.end_line, last_end_line ) - TopOfPage );
    }
    last_start_line = SelRgn.start_line;
    last_end_line = SelRgn.end_line;
#else
    markRegion( TRUE );
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
    SelRgn.selected = FALSE;
    EditFlags.Dragging = FALSE;
#ifdef __WIN__
    updateRegion();
#else
    markRegion( FALSE );
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

    SelRgn.selected = TRUE;
    moveCursor = 0;
    height = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
#ifdef __WIN__
    if( id == CurrentWindow && InsideWindow( id, MouseX, MouseY ) ) {
#else
    if( id == CurrentWindow && InsideWindow( id, win_x, win_y ) ) {
#endif
        ny = TopOfPage + win_y - 1;
        if( ny > CurrentFile->fcb_tail->end_line ) {
            ny = CurrentFile->fcb_tail->end_line;
            moveCursor = 1;
        } else if( ny < 1 ) {
            ny = 1;
            moveCursor = -1;
        }
        GoToLineRelCurs( ny );
        win_x += LeftColumn;
        nx = RealCursorPosition( win_x );
        GoToColumnOnCurrentLine( nx );
    } else {
    #ifndef __WIN__
        if( MouseRow >= WindowAuxInfo( CurrentWindow, WIND_INFO_Y2 ) ) {
            GoToLineRelCurs( TopOfPage + height );
        } else if( MouseRow <= WindowAuxInfo( CurrentWindow, WIND_INFO_Y1 ) ) {
            GoToLineRelCurs( TopOfPage - 1 );
        } else if( MouseCol <= WindowAuxInfo( CurrentWindow, WIND_INFO_X1 ) ) {
            GoToColumnOnCurrentLine( LeftColumn - 1 );
        } else if( MouseCol >= WindowAuxInfo( CurrentWindow, WIND_INFO_X2 ) ) {
            GoToColumnOnCurrentLine( LeftColumn +WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH ));
        }
    #else
    {
        RECT            rect;

        GetClientRect( CurrentWindow, &rect );
        if( MouseY > rect.bottom ) {
            ny = TopOfPage + height;
            if( ny > CurrentFile->fcb_tail->end_line ) {
                ny = CurrentFile->fcb_tail->end_line;
                moveCursor = 1;
            }
            GoToLineRelCurs( ny );
        } else if( MouseY < 0 ) {
            ny = TopOfPage - 1;
            if( ny < 1 ) {
                ny = 1;
                moveCursor = -1;
            }
            GoToLineRelCurs( ny );
        } else if( MouseX < 0 ) {
            GoToColumnOnCurrentLine( LeftColumn - 1 );
        } else if( MouseX > rect.right ) {
            if( EditFlags.Modeless ) {
                GoToColumnOnCurrentLine( 1 + LeftColumn +
                        WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS ) );
            } else {
                GoToColumnOnCurrentLine( LeftColumn +
                        WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS ) );
            }
        }
    }
    #endif
    }

    if( moveCursor == -1 ) {
        GoToColumnOnCurrentLine( 1 );
    } else if( moveCursor == 1 ) {
        GoToColumnOnCurrentLine( CurrentFile->fcb_tail->line_tail->len + 1 );
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
    if( SelRgn.end_line == CurrentLineNumber &&
        SelRgn.end_col == CurrentColumn ) {
        return;
    }
#ifndef __WIN__
    markRegion( FALSE );
#endif
    SelRgn.end_line = CurrentLineNumber;
    SelRgn.end_col = CurrentColumn;
    if( EditFlags.LineBased == FALSE ) {
        SelRgn.lines = FALSE;
    } else if( SelRgn.start_line != SelRgn.end_line ) {
        SelRgn.lines = TRUE;
    } else if( SelRgn.start_col == SelRgn.end_col ) {
        SelRgn.lines = TRUE;
    } else {
        SelRgn.lines = FALSE;
    }
    updateRegion();

} /* UpdateCursorDrag */

/*
 * SetSelRegionCols - set selected region on a line
 */
void SetSelRegionCols( linenum sl, int sc, int ec )
{
    int     rc;
    line    *line;
    fcb     *fcb;
    char    *data;

    SelRgn.lines = FALSE;
    SelRgn.selected = TRUE;
    SelRgn.start_line = SelRgn.end_line = sl;
    SelRgn.start_col = sc;
    SelRgn.end_col = ec + 1;
    SelRgn.start_col_v = 0;
    rc = CGimmeLinePtr( sl, &fcb, &line );
    if( rc == ERR_NO_ERR ) {
        data = ( line->inf.ld.nolinedata ) ? WorkLine->data : line->data;
        SelRgn.start_col_v =
                GetVirtualCursorPosition( data, SelRgn.start_col );
    }

    updateRegion();
    DCUpdate();

} /* SetSelRegionCols */

/*
 * ReselectRegion - restore the previous selection.
 */
int ReselectRegion( void )
{
    if( SelRgn.start_line != 0 && SelRgn.end_line != 0 ) {
        SelRgn.selected = TRUE;
        EditFlags.Dragging = TRUE;
        updateRegion();
        return( ERR_NO_ERR );
    }
    return( ERR_NO_REGION );

} /* ReselectRegion */

/*
 * startSelectedRegion - start selection region area from keyboard
 */
int startSelectedRegion( bool line_based )
{
    if( ShiftDown() && SelRgn.selected ) {
        EditFlags.Dragging = TRUE;
        UpdateCursorDrag();
    } else {
        if( SelRgn.selected ) {
            UnselectRegion();
            return( ERR_NO_ERR );
        }
        EditFlags.Dragging = TRUE;
        InitSelectedRegion();
        SelRgn.lines = line_based;
        SelRgn.selected = TRUE;
        updateRegion();
    }
    return( ERR_NO_ERR );

} /* startSelectedRegion */

/*
 * SelectLeft - update selected region, moving left
 */
int SelectLeft( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return MoveLeft( r, count );
} /* SelectLeft */

/*
 * SelectRight - update selected region, moving right
 */
int SelectRight( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return MoveRight( r, count );
} /* SelectRight */

/*
 * SelectUp - update selected region, moving up
 */
int SelectUp( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( GoToLineRelCurs( CurrentLineNumber - 1 ) );

} /* SelectUp */

/*
 * SelectDown - update selected region, moving down
 */
int SelectDown( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( GoToLineRelCurs( CurrentLineNumber + 1 ) );

} /* SelectDown */

/*
 * SelectHome - update selected region, moving to beginning of line
 */
int SelectHome( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( GoToColumnOnCurrentLine( 1 ) );

} /* SelectHome */

/*
 * SelectEnd - update selected region, moving to end of line
 */
int SelectEnd( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    CurrentColumn = MAX_INPUT_LINE;
    ValidateCurrentColumn();
    return( GoToColumnOnCurrentLine( CurrentColumn ) );

} /* SelectEnd */

/*
 * SelectPageUp - update selected region, moving one page up
 */
int SelectPageUp( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( MovePage( -1, 1, TRUE ) );
} /* SelectPageUp */

/*
 * SelectPageDown - update selected region, moving one page down
 */
int SelectPageDown( void )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( EditFlags.LineBased );
        if( EditFlags.LineBased ) {
            return ERR_NO_ERR;
        }
    }
    return( MovePage( 1, 1, TRUE ) );
} /* SelectPageDown */

int SelectBackwardsWord( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveBackwardsWord( r, count ) );
}

int SelectForwardWord( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveForwardWord( r, count ) );
}

int SelectStartOfFile( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveStartOfFile( r, count ) );
}

int SelectEndOfFile( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveEndOfFile( r, count ) );
}

int SelectTopOfPage( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveTopOfPage( r, count ) );
}

int SelectBottomOfPage( range *r, long count )
{
    if( !EditFlags.Dragging ) {
        startSelectedRegion( FALSE );
    }
    return( MoveBottomOfPage( r, count ) );
}

/*
 * DoSelectSelection - selected region was selected, invoke aprops script
 */
int DoSelectSelection( bool doMenu )
{
    linenum     sl,el,tl;
    int         sc,ec,tc;
    int         rc = ERR_NO_ERR;

    if( SelRgn.selected && doMenu ) {
        sl = SelRgn.start_line;
        el = SelRgn.end_line;
        if( sl > el ) {
            tl = sl;
            sl = el;
            el = tl;
        }
        if( CurrentLineNumber >= sl && CurrentLineNumber <= el ) {
            if( sl == el && !SelRgn.lines ) {
                sc = SelRgn.start_col;
                ec = SelRgn.end_col;
                if( sc > ec ) {
                    sc--;
                    tc = sc;
                    sc = ec;
                    ec = tc;
                }
                else
                if( sc < ec ) {
                    ec--;
                }

                if( CurrentColumn >= sc && CurrentColumn <= ( ec + 1 ) ) {
                    rc = InvokeColSelHook( sc, ec );
                }
            } else {
                rc = InvokeLineSelHook( SelRgn.start_line,
                                        SelRgn.end_line );
            }
            UnselectRegion();
            return( rc );
        }
        UnselectRegion();
    }
    if( ShiftDown() && LastEvent != '_' ) {
        sc = 1;
        ec = CurrentLine->len;
        SetSelRegionCols( CurrentLineNumber, sc, ec );
        if( doMenu ) {
            rc = InvokeLineSelHook( CurrentLineNumber, CurrentLineNumber );
        }
    } else {
        rc = GimmeCurrentEntireWordDim( &sc, &ec, FALSE );
        if( rc ) {
            return( ERR_NO_ERR );
        }
        SetSelRegionCols( CurrentLineNumber, sc, ec );
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
int DoSelectSelectionPopMenu( void )
{
    return( DoSelectSelection( TRUE ) );
}

/*
 * GetSelectedRegion - get the current selected region
 */
int GetSelectedRegion( range *r )
{
    int         rc;

    rc = ERR_NO_SELECTION;
    if( SelRgn.selected ) {
        r->line_based = SelRgn.lines;
        r->start.line = SelRgn.start_line;
        r->end.line = SelRgn.end_line;
        if( ( SelRgn.start_line > SelRgn.end_line ) ||
                ( SelRgn.start_line == SelRgn.end_line &&
                  SelRgn.end_col < SelRgn.start_col ) ) {
            // don't include char at SelRgn.start_col
            r->end.column = SelRgn.end_col;
            r->start.column = SelRgn.start_col - 1;
        } else {
            // don't include char at SelRgn.end_col
            r->end.column = SelRgn.end_col - 1;
            r->start.column = SelRgn.start_col;
        }
        r->fix_range = FALSE;
        rc = ERR_NO_ERR;
    }
    return( rc );

} /* GetSelectedRegion */

/*
 * SetSelectedRegion - set the selected region to a specific area
 */
int SetSelectedRegion( range *r )
{
    int     rc;
    line    *line;
    fcb     *fcb;

    UnselectRegion();
    SelRgn.start_line = r->start.line;
    SelRgn.end_line = r->end.line;

    SelRgn.start_col_v = 1;
    if( r->line_based ) {
        SelRgn.lines = TRUE;
        SelRgn.start_col = 1;
        SelRgn.end_col = LineLength( r->end.line );
    } else {
        SelRgn.lines = FALSE;
        if( r->start.column < r->end.column ) {
            SelRgn.start_col = r->start.column + 1;
            SelRgn.end_col = r->end.column + 2;
        } else {
            SelRgn.start_col = r->end.column + 1;
            SelRgn.end_col = r->start.column + 2;
        }
        rc = CGimmeLinePtr( SelRgn.start_line, &fcb, &line );
        if( rc == ERR_NO_ERR ) {
            SelRgn.start_col_v =
                    WinVirtualCursorPosition( line->data, SelRgn.start_col );
        }
    }
    SelRgn.selected = TRUE;
    updateRegion();
    return( ERR_NO_ERR );

} /* SetSelectedRegion */

/*
 * SetSelectedRegionFromLine - set the SelRgn w/ start_v
 */
int SetSelectedRegionFromLine( range *r, linenum lineno )
{
    int     rc;
    fcb     *fcb;
    line    *line;
    char    *data;

    SetSelectedRegion( r );

    rc = CGimmeLinePtr( lineno, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    data = ( line->inf.ld.nolinedata ) ? WorkLine->data : line->data;
    SelRgn.start_col_v = GetVirtualCursorPosition( data, SelRgn.start_col );

    return( ERR_NO_ERR );
}

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
}

