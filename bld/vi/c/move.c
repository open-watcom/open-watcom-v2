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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "vi.h"
#include "source.h"
#include "win.h"
#ifdef __WIN__
    #include "winvi.h"
#endif

/*
 * goToLine - go to a specified line number
 */
static int goToLine( linenum lineno, int relcurs )
{
    int         i,text_lines,tl;
    linenum     diff,cwl,nwl;
//    linenum   s,e,hiddcnt;
    bool        dispall,pageshift;
    fcb         *cfcb;
    line        *cline;
    int         pad;

    if( lineno < 1 ) {
        return( ERR_NO_SUCH_LINE );
    }

    /*
     * get pointer to requested line
     */
    i = CGimmeLinePtr( lineno, &cfcb, &cline );
    if( i ) {
        return( i );
    }
#if 0
    if( cline->inf.ld.hidden ) {
        GetHiddenRange( lineno, &s, &e );
        if( lineno > CurrentLineNumber ) {
            lineno = e+1;
        } else {
            lineno = s-1;
        }
        i = CGimmeLinePtr( lineno, &cfcb, &cline );
        if( i ) {
            return( i );
        }
    }
#endif

    /*
     * compute new location
     */
    CurrentFcb = cfcb;
    CurrentLine = cline;
    diff = lineno - CurrentLineNumber;
    if( diff == 0 && !EditFlags.GlobalInProgress) {
        return( ERR_NO_ERR );
    }
    cwl = CurrentLineNumber - TopOfPage+1;
    nwl = cwl + diff;

    /*
     * if we go off the window, relocate
     */
    pageshift = FALSE;
    dispall = FALSE;

    text_lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
    if( nwl < 1 || nwl > text_lines ) {
        tl = text_lines/2;
        if( !relcurs ) {
            TopOfPage = lineno - tl;
        } else {
            TopOfPage = lineno + 1 - cwl;
            pad = ( EditFlags.JumpyScroll == TRUE ) ? 1 : 0;
            if( diff > 0 ) {
                TopOfPage += pad;
                diff += pad;
            } else {
                TopOfPage -= pad;
                diff -= pad;
            }
            if( diff > -tl && diff < tl && !dispall ) {
                pageshift = TRUE;
            }
        }
        if( TopOfPage < 1 ) {
            assert( diff < 0 );
            diff += ( 1 - TopOfPage );
            TopOfPage = 1;
        }
        if( TopOfPage > lineno ) {
            assert( diff > 0 );
            diff = TopOfPage - lineno;
            TopOfPage = lineno;
        }
        dispall = TRUE;
    }
#if 0
    hiddcnt = GetHiddenLineCount( TopOfPage, lineno );
    if( hiddcnt > 0 ) {
        pageshift = FALSE;
        dispall = TRUE;
    }
#endif

    if( CheckCurrentColumn() || EditFlags.Dragging ) {
        // pageshift wont help if we also have to column shift
        // and not really useful if dragging

        dispall = TRUE;
        pageshift = FALSE;
    }


    /* call SetCurrentLineNumber AFTER TopOfPage set & CurrentColumn checked
    */
    SetCurrentLineNumber( lineno );

    if( pageshift ) {
        dispall = FALSE;
        ShiftWindowUpDown( CurrentWindow, diff );
        if( EditFlags.LineNumbers ) {
            ShiftWindowUpDown( CurrNumWindow, diff );
        }
        if( diff > 0 ) {
            DCDisplaySomeLines( text_lines - diff, text_lines - 1 );
        } else {
            DCDisplaySomeLines( 0, -diff - 1 );
        }
    }
    UpdateStatusWindow();
    SetWindowCursor();
    if( dispall ) {
        DCInvalidateAllLines(); // lines definitely invalid
        DCDisplayAllLines();
    }
    return( ERR_NO_ERR );

} /* goToLine */

int GoToLineRelCurs( linenum lineno ) { return( goToLine( lineno, TRUE ) ); }
int GoToLineNoRelCurs( linenum lineno ) { return( goToLine( lineno, FALSE ) ); }

void SetCurrentLineNumber( linenum l )
{
    long        last;
    long        height;

    CurrentLineNumber = l;
    UpdateCursorDrag();
    VarAddRandC();

    if( CurrentFile != NULL ) {
        height = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
        last = CurrentFile->fcb_tail->end_line - height + 1;
        if ( TopOfPage > last ){
             last = TopOfPage;
        }
    } else {
        last = 1;
    }
    PositionVerticalScrollThumb( CurrentWindow, TopOfPage, last );
}

/*
 * GoToColumnOK - go to a specified column that does not need a max check
 */
int GoToColumnOK( int colno )
{
    return( GoToColumn( colno, colno ) );

} /* GoToColumnOK */

/*
 * GoToColumnOnCurrentLine - go to a specified column on the current line
 */
int GoToColumnOnCurrentLine( int colno )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }

    if( EditFlags.Modeless ) {
        GoToColumn( colno, CurrentLine->len + 1 );
    } else {
        GoToColumn( colno, CurrentLine->len );
    }

    return( ERR_NO_ERR );

} /* GoToColumnOnCurrentLine */

/*
 * GoToColumn - go to a specified column
 */
int GoToColumn( int colno, int maxcol  )
{
    int vc;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    if( maxcol == 0 ) {
        maxcol=1;
    }
    if( colno == 0 ) {
        colno=1;
    }
    if( colno < 1 || colno > maxcol ) {
        return( ERR_NO_SUCH_COLUMN );
    }

    /*
     * compute new location, and re-display text if needed
     */
    ColumnDesired = VirtualCursorPosition2( colno );
    CurrentColumn = colno;
    if( !CheckLeftColumn() ) {
        DCDisplayAllLines();
        PositionHorizontalScrollThumb( CurrentWindow, LeftColumn );
    }

    SetWindowCursor();
    vc = VirtualCursorPosition();
    UpdateStatusWindow();
    VarAddGlobalLong( "C", (long) vc );
    UpdateCursorDrag();
    return( ERR_NO_ERR );

} /* GoToColumn */

/*
 * SetCurrentLine - reset current line after changes in current file structure
 */
int SetCurrentLine( linenum lineno  )
{
    int         i,text_lines;
    fcb         *cfcb;
    line        *cline;

    if( lineno <= 0 ) {
        lineno = 1;
    }
    i = CGimmeLinePtr( lineno, &cfcb, &cline );
    if( i ) {
        return( i );
    }

    CurrentLine = cline;
    CurrentFcb = cfcb;

    text_lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
    if( lineno < TopOfPage || lineno > ( TopOfPage + text_lines - 1 ) ) {
        TopOfPage = lineno - text_lines/2;
    }
    if( TopOfPage < 1 ) {
        TopOfPage = 1;
    }

    CheckCurrentColumn();
    SetCurrentLineNumber( lineno );
    UpdateStatusWindow();
    SetWindowCursor();
    DCDisplayAllLines();
    return( ERR_NO_ERR );
} /* SetCurrentLine */

/*
 * CheckLeftColumn - check if CurrentColumn and LeftColumn give a position
 *                   in the window; if not, LeftColumn is changed appropriatly
 */
int CheckLeftColumn( void )
{
    int diff,wc,rc=TRUE,pad;

    wc = VirtualCursorPosition() - LeftColumn;

    rc = ColumnInWindow( wc, &diff );
    if( !rc ) {
        // |diff| is already at least 1
        pad = ( EditFlags.JumpyScroll == TRUE ) ? SCROLL_HLINE - 1 : 0;
        if( diff < 0 ) {
            LeftColumn += ( diff - pad );
        } else {
            LeftColumn += ( diff + pad );
        }
        if( LeftColumn < 0 ) {
            LeftColumn = 0;
        }
    }
    return( rc );

} /* CheckLeftColumn */

/*
 * ValidateCurrentColumn - give CurrentColumn an acceptable value
 */
void ValidateCurrentColumn( void )
{
    line        *cline;

    if( CurrentLine == NULL ) {
        return;
    }
    if( CurrentLine->inf.ld.nolinedata ) {
        cline = WorkLine;
    } else {
        cline = CurrentLine;
    }

    if( CurrentColumn >= cline->len ) {
        if( (EditFlags.InsertModeActive || EditFlags.Modeless) &&
                        CurrentColumn > cline->len ) {
            CurrentColumn = cline->len+1;
        } else {
            CurrentColumn = cline->len;
        }
    }
    if( CurrentColumn <= 0  ) {
        CurrentColumn = 1;
    }

} /* ValidateCurrentColumn */

/*
 * CheckCurrentColumn - check state of current column, return TRUE if need to
 *                      redisplay page
 */
int CheckCurrentColumn( void )
{
    int clen,vcp, dispall = FALSE;

    clen = RealLineLen( CurrentLine->data );
    if( clen == 0 ) {
        clen=1;
    }
    ValidateCurrentColumn();

    vcp = VirtualCursorPosition();

    if( vcp != ColumnDesired ) {
        if( clen >= ColumnDesired ) {
            CurrentColumn = RealCursorPosition( ColumnDesired );
        } else {
            if( EditFlags.InsertModeActive || EditFlags.Modeless ) {
                CurrentColumn = CurrentLine->len+1;
            } else {
                CurrentColumn = CurrentLine->len;
            }
        }
        ValidateCurrentColumn();
        dispall = !CheckLeftColumn();
        /* changed CurrentColumn - update horiz scrollbar
        */
        PositionHorizontalScrollThumb( CurrentWindow, LeftColumn );
    }
    VarAddGlobalLong( "C", (long) CurrentColumn );
    return( dispall );

} /* CheckCurrentColumn */

/*
 * ShiftTab - figure out value of reverse tab
 */
int ShiftTab( int col, int ta )
{
    int i,j;

    col--;
    j = col/ta;
    i = col-j*ta;
    if( i == 0 && col != 0 ) {
        i = ta;
    }
    return( i );

} /* ShiftTab */

/*
 * SetCurrentColumn - set CurrentColumn, positioning LeftColumn nicely
 */
int SetCurrentColumn( int newcol )
{
    long        oldpos;
    int         text_cols;

    oldpos = CurrentColumn - LeftColumn;
    if( newcol <= 0 ) {
        newcol = 1;
    }

    text_cols = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_COLS );
    if( oldpos < 0 || oldpos >= text_cols ) {
        LeftColumn = newcol - SCROLL_HLINE - 1;
    } else {
        LeftColumn = newcol - oldpos - 1;
    }
    if( LeftColumn < 0 ) {
        LeftColumn = 0;
    }

    CurrentColumn = newcol;

    CheckCurrentColumn();
    UpdateCursorDrag();
    VarAddRandC();

    PositionHorizontalScrollThumb( CurrentWindow, LeftColumn );
    UpdateStatusWindow();
    SetWindowCursor();
    DCDisplayAllLines();
    return( ERR_NO_ERR );
} /* SetCurrentColumn */

/*
 * LocateCmd - parse a locate command (format: locate r,c[,len])
 */
int LocateCmd( char *data )
{
    char        tmp[ MAX_STR ];
    linenum     r;
    int         c;
    int         len;

#ifdef __WIN__
    if( BAD_ID( CurrentWindow ) ){
        return( ERR_INVALID_LOCATE );
    }
#endif

    if( NextWord1( data, tmp ) <= 0 ) {
        return( ERR_INVALID_LOCATE );
    }
    r = atol( tmp );
    if( NextWord1( data, tmp ) <= 0 ) {
        return( ERR_INVALID_LOCATE );
    }
    c = atoi( tmp );

    // real selection length
    while( isspace( *data ) ) {
        data++;
    }
    len = 0;
    if( *data != 0 ) {
        if( NextWord1( data, tmp ) <= 0 ) {
            return( ERR_INVALID_LOCATE );
        }
        len = atoi( tmp );
    }

    GoToLineNoRelCurs( r );

    c = RealCursorPosition( c );
    GoToColumnOnCurrentLine( c + len );

#ifdef __WIN__
    // likely only called by dde, which doesn't use event loop,
    // so must ensure cache ok and set cursor here

    DCInvalidateAllLines();
    DCDisplayAllLines();
    DCUpdate();
    SetWindowCursor();
    SetWindowCursorForReal();
#endif

    if( len > 0 ) {
        SetSelRegionCols( CurrentLineNumber, c, c + len - 1 );
    }
    return( ERR_NO_ERR );
}
