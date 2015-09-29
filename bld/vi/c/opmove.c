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
#include "win.h"
#include <assert.h>

static vi_key   lastKeys[2];

static vi_rc checkLine( linenum *ln )
{
    vi_rc       rc;
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
    if( EditFlags.Modeless && (rc == ERR_NO_SUCH_LINE) ) {
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
                r->line_based = true;
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
            if( !IsPastLastLine( line + 1 ) ) {
                r->start.line = line + 1;
                r->line_based = true;
                *col = 1;
            } else {
                *col = len;
            }
        } else {
            if( EditFlags.OperatorWantsMove ) {
                if( *col == len + 1 ) {
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
static vi_rc verifyMoveFromPageTop( range *r, linenum ln )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    ln += LeftTopPos.line;
    if( IsPastLastLine( ln ) ) {
        CFindLastLine( &ln );
    }
    r->line_based = true;
    r->start.line = ln;
    return( ERR_NO_ERR );

} /* verifyMoveFromPageTop */

/*
 * MovePageMiddle - move to the middle of the page
 */
vi_rc MovePageMiddle( range *r, long count )
{
    linenum     ln, lne;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;
    ln = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES ) - 1;
    CFindLastLine( &lne );
    lne = lne - LeftTopPos.line + 1;
    if( ln > lne ) {
        ln = lne;
    }
    return( verifyMoveFromPageTop( r, ln / 2 ) );

} /* MovePageMiddle */

vi_rc MovePageTop( range *r, long count )
{
    return( verifyMoveFromPageTop( r, count - 1 ) );

} /* MovePageTop */

vi_rc MovePageBottom( range *r, long count )
{
    linenum     ln;
    int         lines;
    int         amt;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
    if( IsPastLastLine( LeftTopPos.line + lines ) ) {
        CFindLastLine( &ln );
        amt = ln - LeftTopPos.line - count + 1;
    } else {
        amt = lines - count;
    }

    return( verifyMoveFromPageTop( r, amt ) );

} /* MovePageBottom */

static vi_rc doVerticalMove( range *r, linenum new )
{
    vi_rc       rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = checkLine( &new );
    r->start.line = new;
    r->start.column = CurrentPos.column;
    r->line_based = true;
    return( rc );
}

vi_rc MoveUp( range *r, long count )
{
    return( doVerticalMove( r, CurrentPos.line - count ) );
}

vi_rc MoveDown( range *r, long count )
{
    return( doVerticalMove( r, CurrentPos.line + count ) );
}


static vi_rc newColumnOnCurrentLine( range *r, int new_col )
{
    vi_rc   rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    rc = ERR_NO_ERR;
    r->start.line = CurrentPos.line;
    r->line_based = false;
    if( new_col < CurrentPos.column ) {
        rc = checkLeftMove( CurrentPos.line, &new_col, r );
    } else {
        rc = checkRightMove( CurrentPos.line, &new_col, r );
    }
    r->start.column = new_col;
    return( rc );
}

vi_rc MoveLineEnd( range *r, long count )
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

#if 0
vi_rc LineEndRange( range *r, long count )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;
    r->start.line = CurrentPos.line;
    r->line_based = false;
    r->end = r->start;
    r->end.column = CurrentLine->len + 1;
    return( ERR_NO_ERR );
}
#endif

vi_rc MoveStartOfLine( range *r, long count )
{
    count = count;
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    return( newColumnOnCurrentLine( r, FindStartOfCurrentLine() ) );
}

vi_rc MoveLineBegin( range *r, long count )
{
    vi_rc   rc;

    count = count;
    rc = newColumnOnCurrentLine( r, 1 );

    // Make "Home" behave like "0"
    LeftTopPos.column = 0;
    DCDisplayAllLines();
    return( rc );
}

vi_rc MoveLeft( range *r, long count )
{
    return( newColumnOnCurrentLine( r, CurrentPos.column - (int)count ) );
}

vi_rc MoveRight( range *r, long count )
{
    return( newColumnOnCurrentLine( r, CurrentPos.column + (int)count ) );
}

vi_rc MoveToColumn( range *r, long count )
{
    return( newColumnOnCurrentLine( r, RealColumnOnCurrentLine( (int)count ) ) );
}

/*
 * MoveTab - move forward a tab
 */
vi_rc MoveTab( range *r, long count )
{
    int                 i, vc, len;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }

    r->start.line = CurrentPos.line;
    r->line_based = false;
    len = VirtualLineLen( CurrentLine->data );
    vc = VirtualColumnOnCurrentLine( CurrentPos.column );
    for( ; count > 0; --count ) {
        i = Tab( vc, EditVars.TabAmount );
        vc += i;
        if( vc > len ) {
            r->start.column = len;
            return( ERR_NO_SUCH_COLUMN );
        }
    }
    r->start.column = RealColumnOnCurrentLine( vc );
    return( ERR_NO_ERR );

} /* MoveTab */

/*
 * MoveShiftTab - move back a tab
 */
vi_rc MoveShiftTab( range *r, long count )
{
    int                 i, vc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    r->start.line = CurrentPos.line;
    r->line_based = false;
    vc = VirtualColumnOnCurrentLine( CurrentPos.column );
    for( ; count > 0; --count ) {
        i = ShiftTab( vc, EditVars.TabAmount );
        vc -= i;
        if( vc < 1 ) {
            r->start.column = 1;
            return( ERR_NO_SUCH_COLUMN );
        }
    }
    r->start.column = RealColumnOnCurrentLine( vc );
    return( ERR_NO_ERR );

} /* MoveShiftTab */

static vi_rc doMoveToStartEndOfLine( range *r, long count, bool start )
{
    linenum             new;
    vi_rc               rc;
    line                *line;
    fcb                 *fcb;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    new = CurrentPos.line + count;
    rc = checkLine( &new );
    r->start.line = new;
    if( rc == ERR_NO_ERR ) {
        rc = CGimmeLinePtr( new, &fcb, &line );
        if( rc == ERR_NO_ERR ) {
            if( start ) {
                r->start.column = FindStartOfALine( line );
            } else {
                r->start.column = line->len + 1;
            }
            /*
             * Not sure if we should be line-based or not; for now
             * I'm just cloning MKS.
             */
            r->line_based = true;
        }
    }
    return( rc );
}

vi_rc MoveStartNextLine( range *r, long count )
{
    return( doMoveToStartEndOfLine( r, count, true ) );
}

vi_rc MoveStartPrevLine( range *r, long count )
{
    return( doMoveToStartEndOfLine( r, -count, true ) );
}

/*
 * moveForwardAWord - move to next word
 */
static vi_rc moveForwardAWord( range *r, bool end, bool bigword, int count )
{
    int         i;
    vi_rc       rc;
    i_mark      curr;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    assert( count > 0 );
    rc = ERR_NO_ERR;
    curr = r->end;
    for( i = 0; i < count; i++ ) {
        if( end ) {
            rc = MarkEndOfNextWordForward( &r->start, &curr, bigword );
        } else {
            rc = MarkStartOfNextWordForward( &r->start, &curr, bigword );
        }
        if( rc != ERR_NO_ERR ) {
            break;
        }
        curr = r->start;
    }
    r->line_based = false;
    return( rc );

} /* MoveForwardWord */

vi_rc MoveForwardWord( range *r, long count )
{
    return( moveForwardAWord( r, false, false, count ) );
}

vi_rc MoveForwardBigWord( range *r, long count )
{
    return( moveForwardAWord( r, false, true, count ) );
}

vi_rc MoveForwardWordEnd( range *r, long count )
{
    return( moveForwardAWord( r, true, false, count ) );
}

vi_rc MoveForwardBigWordEnd( range *r, long count )
{
    return( moveForwardAWord( r, true, true, count ) );
}

/*
 * moveBackwardsAWord - move back a word
 */
static vi_rc moveBackwardsAWord( range *r, bool bigword, int count )
{
    int         i;
    i_mark      curr;
    vi_rc       rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    assert( count > 0 );
    curr = r->end;
    for( i = 0; i < count; i++ ) {
        rc = MarkStartOfNextWordBackward( &r->start, &curr, bigword );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        curr = r->start;
    }
    r->line_based = false;
    return( ERR_NO_ERR );

} /* moveBackwardsAWord */

vi_rc MoveBackwardsWord( range *r, long count )
{
    return( moveBackwardsAWord( r, false, count ) );
}

vi_rc MoveBackwardsBigWord( range *r, long count )
{
    return( moveBackwardsAWord( r, true, count ) );
}

/*
 * doACharFind - find a character on a line
 */
static vi_rc doACharFind( range *r, bool forward, int num, long count )
{
    int         c;
    vi_key      lc;
    vi_rc       rc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    r->line_based = false;
    lc = LastEvent;
    rc = FindCharOnCurrentLine( forward, num, &c, count );
    if( rc == ERR_NO_ERR && c >= 0 ) {
        lastKeys[0] = lc;
        lastKeys[1] = LastEvent;
        r->start.column = c;
        return( ERR_NO_ERR );
    }
    return( rc );

} /* doACharFind */

vi_rc MoveUpToChar( range *r, long count )
{
    return( doACharFind( r, true, 0, count ) );
}

vi_rc MoveUpToBeforeChar( range *r, long count )
{
    return( doACharFind( r, true, -1, count ) );
}

vi_rc MoveBackToChar( range *r, long count )
{
    return( doACharFind( r, false, 0, count ) );
}

vi_rc MoveBackToAfterChar( range *r, long count )
{
    return( doACharFind( r, false, 1, count ) );
}

/*
 * DoGo - go to a specified line
 */
vi_rc DoGo( range *r, long count )
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
    r->line_based = true;
    return( ERR_NO_ERR );

} /* DoGo */

/*
 * moveToLastCFind - go to last character found using f, F, t or T
 */
static vi_rc moveToLastCFind( range *r, bool reverse, long count )
{
    vi_rc       rc;
    vi_key      tmp[2];
    vi_key      lastc;

    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    if( lastKeys[0] == VI_KEY( NULL ) ) {
        return( ERR_NO_PREVIOUS_COMMAND );
    }
    tmp[0] = lastKeys[0];
    tmp[1] = lastKeys[1];
    KeyAdd( lastKeys[1] );
    lastc = lastKeys[0];
    if( reverse ) {
        if( islower( lastc ) ) {
            lastc = toupper( lastc );
        } else {
            lastc = tolower( lastc );
        }
    }
    rc = (EventList[lastc].rtn.move)( r, count );
    lastKeys[0] = tmp[0];
    lastKeys[1] = tmp[1];
    return( rc );

} /* moveToLastCFind */

vi_rc MoveToLastCharFind( range *r, long count )
{
    return( moveToLastCFind( r, false, count ) );
}

vi_rc MoveToLastCharFindRev( range *r, long count )
{
    return( moveToLastCFind( r, true, count ) );
}

/*
 * MoveStartOfFile - go to the first char of file
 */
vi_rc MoveStartOfFile( range *r, long count )
{
    if( CurrentLine == NULL ) {
        return( ERR_NO_FILE );
    }
    count = count;      // not needed here
    r->start.line = 1;
    r->start.column = 1;
    r->line_based = false;
    return( ERR_NO_ERR );

} /* MoveStartOfFile */

/*
 * MoveEndOfFile - go to the top of file
  */
vi_rc MoveEndOfFile( range *r, long count )
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
    r->line_based = false;
    return( ERR_NO_ERR );

} /* MoveEndOfFile */

vi_rc MoveTopOfPage( range *r, long count )
{
    count = count;
    return( doMoveToStartEndOfLine( r, LeftTopPos.line-CurrentPos.line, true ) );
}

vi_rc MoveBottomOfPage( range *r, long count )
{
    int bottom = LeftTopPos.line +
                 WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES ) - 1;
    count = count;
    return( doMoveToStartEndOfLine( r, bottom - CurrentPos.line, false ) );
}

