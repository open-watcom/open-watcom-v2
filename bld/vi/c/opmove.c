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


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "vi.h"
#include "keys.h"
#include "win.h"

static lastChar[2];

static int checkLine( linenum *ln )
{
    int         rc;
    linenum     last;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = ERR_NO_ERR;
    if( (*ln) < 1 ) {
        *ln = 1;
        rc = ERR_NO_SUCH_LINE;
    } else {
        CFindLastLine( &last );
        if( (*ln) > last ) {
            *ln = last;
            rc = ERR_NO_SUCH_LINE;
        }
    }
    if( ( EditFlags.Modeless == TRUE ) && ( rc == ERR_NO_SUCH_LINE ) ) {
        rc = ERR_NO_ERR;
    }
    return( rc );
}

static int checkLeftMove( linenum line, int *col, range *r )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    if( (*col) < 1 ) {
        if( EditFlags.Modeless ) {
            if( line > 1 ) {
                r->start.line = line - 1;
                r->line_based = TRUE;
                *col = LineLength( line - 1 ) + 1;

            } else {
                *col = 1;
            }
        } else {
            *col = 1;
            return( ERR_NO_SUCH_COLUMN );
        }
    }
    return( ERR_NO_ERR );
}

static int checkRightMove( linenum line, int *col, range *r )
{
    int         len;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    len = LineLength( line );
    if( EditFlags.Modeless ) {
        len++;
    }
    if( (*col) > len ) {
        if( EditFlags.Modeless ) {
            if ( !IsPastLastLine( line + 1 ) ) {
                r->start.line = line + 1;
                r->line_based = TRUE;
                *col = 1;
            } else {
                *col = len;
            }
        } else {
            if( EditFlags.OperatorWantsMove ) {
                if( *col == len+1 ) {
                    return( ERR_NO_ERR );
                }
            }
            *col = len;
            return( ERR_NO_SUCH_COLUMN );
        }
    }
    return( ERR_NO_ERR );
}

/*
 * verifyMoveFromPageTop - move a certain amount past the top of page,
 *                         verifying that the end of file is not overrun
 */
static int verifyMoveFromPageTop( range *r, linenum ln )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    ln += TopOfPage;
    if( IsPastLastLine( ln ) ) {
        CFindLastLine( &ln );
    }
    r->line_based = TRUE;
    r->start.line = ln;
    return( ERR_NO_ERR );

} /* verifyMoveFromPageTop */

/*
 * MovePageMiddle - move to the middle of the page
 */
int MovePageMiddle( range *r, long count )
{
    linenum     ln, lne;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;
    ln = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES ) - 1;
    CFindLastLine( &lne );
    lne = lne - TopOfPage + 1;
    if( ln > lne ) {
        ln = lne;
    }
    return( verifyMoveFromPageTop( r, ln / 2 ) );

} /* MovePageMiddle */

int MovePageTop( range *r, long count )
{
    return( verifyMoveFromPageTop( r, count - 1 ) );

} /* MovePageTop */

int MovePageBottom( range *r, long count )
{
    linenum     ln;
    int         lines;
    int         amt;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
    if( IsPastLastLine( TopOfPage + lines ) ) {
        CFindLastLine( &ln );
        amt = ln - TopOfPage - count + 1;
    } else {
        amt = lines - count;
    }

    return( verifyMoveFromPageTop( r, amt ) );

} /* MovePageBottom */

static int doVerticalMove( range *r, linenum new )
{
    int         rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = checkLine( &new );
    r->start.line = new;
    r->start.column = CurrentColumn;
    r->line_based = TRUE;
    return( rc );
}

int MoveUp( range *r, long count )
{
    return( doVerticalMove( r, CurrentLineNumber - count ) );
}

int MoveDown( range *r, long count )
{
    return( doVerticalMove( r, CurrentLineNumber + count ) );
}


static int newColumnOnCurrentLine( range *r, int new_col )
{
    int   rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = ERR_NO_ERR;
    r->start.line = CurrentLineNumber;
    r->line_based = FALSE;
    if( new_col < CurrentColumn ) {
        rc = checkLeftMove( CurrentLineNumber, &new_col, r );
    } else {
        rc = checkRightMove( CurrentLineNumber, &new_col, r );
    }
    r->start.column = new_col;
    return( rc );
}

int MoveLineEnd( range *r, long count )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;
    if( EditFlags.Modeless ) {
        return( newColumnOnCurrentLine( r, CurrentLine->len + 1 ) );
    } else {
        return( newColumnOnCurrentLine( r, CurrentLine->len ) );
    }
}

int MoveStartOfLine( range *r, long count )
{
    count = count;
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    return( newColumnOnCurrentLine( r, FindStartOfCurrentLine() ) );
}

int MoveLineBegin( range *r, long count )
{
    int rc;
    count = count;
    rc = newColumnOnCurrentLine( r, 1 );

    // Make "Home" behave like "0"
    LeftColumn = 0;
    DCDisplayAllLines();
    return( rc );
}

int MoveLeft( range *r, long count )
{
    return( newColumnOnCurrentLine( r, CurrentColumn - (int)count ) );
}

int MoveRight( range *r, long count )
{
    return( newColumnOnCurrentLine( r, CurrentColumn + (int)count ) );
}

int MoveToColumn( range *r, long count )
{
    return( newColumnOnCurrentLine( r, RealCursorPosition( (int)count ) ) );
}

/*
 * MoveTab - move forward a tab
 */
int MoveTab( range *r, long count )
{
    int                 i, vc, len;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }

    r->start.line = CurrentLineNumber;
    r->line_based = FALSE;
    len = RealLineLen( CurrentLine->data );
    vc = VirtualCursorPosition();
    while( count ) {
        i = Tab( vc, TabAmount );
        vc += i;
        if( vc > len ) {
            r->start.column = len;
            return( ERR_NO_SUCH_COLUMN );
        }
        count -= 1;
    }
    r->start.column = RealCursorPosition( vc );
    return( ERR_NO_ERR );
} /* MoveTab */

/*
 * MoveShiftTab - move back a tab
 */
int MoveShiftTab( range *r, long count )
{
    int                 i, vc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    r->start.line = CurrentLineNumber;
    r->line_based = FALSE;
    vc = VirtualCursorPosition();
    while( count ) {
        i = ShiftTab( vc, TabAmount );
        vc -= i;
        if( vc < 1 ) {
            r->start.column = 1;
            return( ERR_NO_SUCH_COLUMN );
        }
        count -= 1;
    }
    r->start.column = RealCursorPosition( vc );
    return( ERR_NO_ERR );
} /* MoveShiftTab */

static int doMoveToStartEndOfLine( range *r, long count, bool start )
{
    linenum             new;
    int                 rc;
    line                *line;
    fcb                 *fcb;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    new = CurrentLineNumber + count;
    rc = checkLine( &new );
    r->start.line = new;
    if( rc == ERR_NO_ERR ) {
        rc = CGimmeLinePtr( new, &fcb, &line );
        if( rc == ERR_NO_ERR ) {
            if( start == TRUE ) {
                r->start.column = FindStartOfALine( line );
            } else {
                r->start.column = line->len + 1;
            }
            /*
             * Not sure if we should be line-based or not; for now
             * I'm just cloning MKS.
             */
            r->line_based = TRUE;
        }
    }
    return( rc );
}

int MoveStartNextLine( range *r, long count )
{
    return( doMoveToStartEndOfLine( r, count, TRUE ) );
}

int MoveStartPrevLine( range *r, long count )
{
    return( doMoveToStartEndOfLine( r, -count, TRUE ) );
}

/*
 * moveForwardAWord - move to next word
 */
static int moveForwardAWord( range *r, bool end, bool bigword, int count )
{
    int         i, rc;
    i_mark      curr;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    assert( count > 0 );
    curr = r->end;
    for( i = 0; i < count; i++ ) {
        if( end ) {
            rc = MarkEndOfNextWordForward( &r->start, &curr, bigword );
        } else {
            rc = MarkStartOfNextWordForward( &r->start, &curr, bigword );
        }
        if( rc != ERR_NO_ERR ) break;
        curr = r->start;
    }
    r->line_based = FALSE;
    return( rc );

} /* MoveForwardWord */

int MoveForwardWord( range *r, long count )
{
    return( moveForwardAWord( r, FALSE, FALSE, count ) );
}

int MoveForwardBigWord( range *r, long count )
{
    return( moveForwardAWord( r, FALSE, TRUE, count ) );
}

int MoveForwardWordEnd( range *r, long count )
{
    return( moveForwardAWord( r, TRUE, FALSE, count ) );
}

int MoveForwardBigWordEnd( range *r, long count )
{
    return( moveForwardAWord( r, TRUE, TRUE, count ) );
}

/*
 * moveBackwardsAWord - move back a word
 */
static int moveBackwardsAWord( range *r, bool bigword, int count )
{
    int         i, rc;
    i_mark      curr;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    assert( count > 0 );
    curr = r->end;
    for( i = 0; i < count; i++ ) {
        rc = MarkStartOfNextWordBackward( &r->start, &curr, bigword );
        if( rc != ERR_NO_ERR ) break;
        curr = r->start;
    }
    r->line_based = FALSE;
    return( ERR_NO_ERR );

} /* moveBackwardsAWord */

int MoveBackwardsWord( range *r, long count )
{
    return( moveBackwardsAWord( r, FALSE, count ) );
}

int MoveBackwardsBigWord( range *r, long count )
{
    return( moveBackwardsAWord( r, TRUE, count ) );
}

/*
 * doACharFind - find a character on a line
 */
static int doACharFind( range *r, int forward, int num, long count )
{
    int         i,c;
    char        lc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    r->line_based = FALSE;
    lc = LastEvent;
    i = FindCharOnCurrentLine( forward, num, &c, count );
    if( !i && c >= 0 ) {
        lastChar[0] = lc;
        lastChar[1] = LastEvent;
        r->start.column = c;
        return( ERR_NO_ERR );
    }
    return( i );

} /* doACharFind */

int MoveUpToChar( range *r, long count )
{
    return( doACharFind( r, TRUE, 0, count ) );
}

int MoveUpToBeforeChar( range *r, long count )
{
    return( doACharFind( r, TRUE, -1, count ) );
}

int MoveBackToChar( range *r, long count )
{
    return( doACharFind( r, FALSE, 0, count ) );
}

int MoveBackToAfterChar( range *r, long count )
{
    return( doACharFind( r, FALSE, 1, count ) );
}

/*
 * DoGo - go to a specified line
 */
int DoGo( range *r, long count )
{
    linenum     lne;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    CFindLastLine( &lne );
    if( NoRepeatInfo ) {
        r->start.line = lne;
    } else {
        if( count > lne ) {
            if( EditFlags.Modeless ) {
                count = lne;
            } else {
                return( ERR_NO_SUCH_LINE );
            }
        }
        r->start.line = count;
    }
    // change column - is this correct? MKS Vi does it...
    r->start.column = 1;
    r->line_based = TRUE;
    return( ERR_NO_ERR );

} /* DoGo */

/*
 * moveToLastCFind - go to last character found using f, F, t or T
 */
static int moveToLastCFind( range *r, bool reverse, long count )
{
    int         rc, tmp, lastc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    if( lastChar[0] == 0 ) {
        return( ERR_NO_PREVIOUS_COMMAND );
    }
    tmp = *(int *)lastChar;
    KeyAdd( lastChar[1] );
    lastc = lastChar[0];
    if( reverse ) {
        if( islower( lastc ) ) {
            lastc = toupper( lastc );
        } else {
            lastc = tolower( lastc );
        }
    }
    rc = (EventList[lastc].rtn.move)( r, count );
    *(int *)lastChar = tmp;
    return( rc );

} /* moveToLastCFind */

int MoveToLastCharFind( range *r, long count )
{
    return( moveToLastCFind( r, FALSE, count ) );
}

int MoveToLastCharFindRev( range *r, long count )
{
    return( moveToLastCFind( r, TRUE, count ) );
}

/*
 * MoveStartOfFile - go to the first char of file
 */
int MoveStartOfFile( range *r, long count )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;      // not needed here
    r->start.line = 1;
    r->start.column = 1;
    r->line_based = FALSE;
    return( ERR_NO_ERR );
} /* MoveStartOfFile */

/*
 * MoveEndOfFile - go to the top of file
  */
int MoveEndOfFile( range *r, long count )
{
    linenum     ln;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;      // not needed here
    CFindLastLine( &ln );
    r->start.line = ln;
    if( EditFlags.Modeless ) {
        r->start.column = LineLength( ln ) + 1;
    } else {
        r->start.column = LineLength( ln );
    }
    r->line_based = FALSE;
    return( ERR_NO_ERR );
} /* MoveEndOfFile */

int MoveTopOfPage( range *r, long count )
{
    count = count;
    return( doMoveToStartEndOfLine( r, TopOfPage-CurrentLineNumber, TRUE ) );
}

int MoveBottomOfPage( range *r, long count )
{
    int bottom = TopOfPage +
                 WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES ) - 1;
    count = count;
    return( doMoveToStartEndOfLine( r, bottom - CurrentLineNumber, FALSE ) );
}

