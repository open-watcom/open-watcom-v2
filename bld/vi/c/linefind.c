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
#ifdef __WIN__
    #include "winrtns.h"
#endif

static bool testIfCharNotInRange( char, char * );

/*
 * findFirstCharInListForward - find first occurance of a character in
 *                             charlist in specified line
 */
static int findFirstCharInListForward( line *l, char *clist, int scol )
{
    int         i;
    char        c, *lst;

    for( i = scol; i < l->len; i++ ) {
        c = l->data[i];
        for( lst = clist; *lst != '\0'; ++lst ) {
            if( *lst == c ) {
                return( i );
            }
        }
    }
    return( -1 );

} /* findFirstCharInListForward */

/*
 * findFirstCharInListBackwards - find first occurance of a character in
 *                             charlist in specified line
 */
static int findFirstCharInListBackwards( line *l, char *clist, int scol )
{
    int         i;
    char        c, *lst;

    if( l->len < scol ) {
        return( -1 );
    }
    for( i = scol; i >= 0; i-- ) {
        c = l->data[i];
        for( lst = clist; *lst != '\0'; ++lst ) {
            if( *lst == c ) {
                return( i );
            }
        }
    }
    return( -1 );

} /* findFirstCharInListBackwards */

/*
 * FindFirstCharNotInListForward - find first char in a line that
 *                             is not in charlist
 */
int FindFirstCharNotInListForward( line *l, char *clist, int scol )
{
    int         i;
    char        c, *lst;

    for( i = scol; i < l->len; i++ ) {
        c = l->data[i];
        for( lst = clist; *lst != 0; lst++ ) {
            if( *lst == c ) {
                break;
            }
        }
        if( *lst == 0 ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharNotInListForward */

/*
 * FindFirstCharNotInListBackwards - find first char on a line that
 *                             is not in charlist
 */
int FindFirstCharNotInListBackwards( line *l, char *clist, int scol )
{
    int         i;
    char        c, *lst;

    if( l->len < scol ) {
        return( -1 );
    }
    for( i = scol; i >= 0; i-- ) {
        c = l->data[i];
        for( lst = clist; *lst != 0; lst++ ) {
            if( *lst == c ) {
                break;
            }
        }
        if( *lst == 0 ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharNotInListBackwards */

/*
 * FindFirstCharInRangeForward - find first occurance of a character in
 *                             a range in charlist in specified line
 */
int FindFirstCharInRangeForward( line *l, char *clist, int scol )
{
    int i;

    for( i = scol; i < l->len; i++ ) {
        if( TestIfCharInRange( l->data[i], clist ) ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharInRangeForward */

/*
 * FindFirstCharNotInRangeForward - find first occurance of a character
 *                             not in a range in charlist in specified line
 */
int FindFirstCharNotInRangeForward( line *l, char *clist, int scol )
{
    int i;

    for( i = scol; i < l->len; i++ ) {
        if( testIfCharNotInRange( l->data[i], clist ) ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharNotInRangeForward */

/*
 * FindFirstCharInRangeBackwards - find first occurance of a character in
 *                             a range in charlist in specified line
 */
int FindFirstCharInRangeBackwards( line *l, char *clist, int scol )
{
    int i;

    if( l->len < scol ) {
        return( -1 );
    }
    for( i = scol; i >= 0; i-- ) {
        if( TestIfCharInRange( l->data[i], clist ) ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharInRangeBackwards */

/*
 * FindFirstCharNotInRangeBackward - find first occurance of a character
 *                             not in a range in charlist in specified line
 */
int FindFirstCharNotInRangeBackward( line *l, char *clist, int scol )
{
    int i;

    if( l->len < scol ) {
        return( -1 );
    }
    for( i = scol; i >= 0; i-- ) {
        if( testIfCharNotInRange( l->data[i], clist ) ) {
            return( i );
        }
    }
    return( -1 );

} /* FindFirstCharNotInRangeBackward */

/*
 * TestIfCharInRange - do just that
 */
bool TestIfCharInRange( char c, char *clist )
{
    char        *lst;

    
    for( lst = clist; *lst != 0; lst += 2 ) {
        if( c >= *lst && c <= *(lst + 1)  ) {
            return( TRUE );
        }
    }
    return( FALSE );

} /* TestIfCharInRange */

/*
 * testIfCharNotInRange - do just that
 */
static bool testIfCharNotInRange( char c, char *clist )
{
    char        *lst;

    for( lst = clist; *lst != 0; lst += 2 ) {
        if( c >= *lst && c <= *(lst + 1) ) {
            return( FALSE );
        }
    }
    return( TRUE );

} /* testIfCharNotInRange */

/*
 * FindCharOnCurrentLine - look for c char on a line ('f','F','t','T' cmds)
 */
vi_rc FindCharOnCurrentLine( bool fwdflag, int mod, int *col, int cnt )
{
    int         i, c, j;
    char        lst[2];

    c = CurrentPos.column - 1;
    *col = -1;
    LastEvent = GetNextEvent( FALSE );
    if( LastEvent == VI_KEY( ESC ) ) {
        return( ERR_NO_ERR );
    }
    lst[0] = (char)LastEvent;
    lst[1] = 0;
    i = 0;
    for( j = 0; j < cnt; j++ ) {
        i = -1;
        if( fwdflag ) {
            if( c < CurrentLine->len - 1 ) {
                i = findFirstCharInListForward( CurrentLine, lst, c + 1 );
            }
        } else {
            if( c > 0 ) {
                i = findFirstCharInListBackwards( CurrentLine, lst, c - 1 );
            }
        }
        if( i < 0 ) {
            Error( GetErrorMsg( ERR_CHAR_NOT_FOUND ), LastEvent );
            return( DO_NOT_CLEAR_MESSAGE_WINDOW );
        }
        c = i;
    }
    *col = 1 + i + mod;
    return( ERR_NO_ERR );

} /* FindCharOnCurrentLine */

/*
 * FancyGotoLine - goto line through dialog box
 */
vi_rc FancyGotoLine( void )
{
#ifdef __WIN__
    linenum     newline;

    if( GetLineDialog( &newline ) ) {
        SetCurrentLine( newline );
        NewCursor( CurrentWindow, EditVars.NormalCursorType );
    }
#endif
    return( ERR_NO_ERR );

} /* FancyGotoLine */
