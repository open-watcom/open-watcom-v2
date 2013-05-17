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

static char     *wordList;
static bool     noWrap;

/*
 * InitWordSearch - set up for word search
 */
void InitWordSearch( char *regword )
{
    wordList = regword;

} /* InitWordSearch */

/*
 * ptrFromMark: Return a character pointer to the position in some
 * line's data which is indicated by the given mark.
 */
static char *ptrFromMark( i_mark *curr )
{
    line        *line;
    fcb         *fcb;
    vi_rc       rc;
    char        *ptr;

    ptr = NULL;
    rc = CGimmeLinePtr( curr->line, &fcb, &line );
    if( rc == ERR_NO_ERR ) {
        if( curr->column > 0 && curr->column <= line->len ) {
            ptr = &(line->data[curr->column - 1]);
        } else if( !EditFlags.WordWrap ) {
            if( curr->column == 0 ) {
                ptr = &(line->data[curr->column]);
            } else {
                ptr = &(line->data[line->len - 1]);
            }
            noWrap = TRUE;
        }
    }
    return( ptr );

} /* ptrFromMark */

typedef enum {
    BLOCK_WORD,
    BLOCK_WHITESPACE,
    BLOCK_DELIM,
    BLOCK_ENDOFLINE,
    BLOCK_THEENDDAMMIT
} btype;

static btype charType( char c, bool big )
{
    if( noWrap ) {
        noWrap = FALSE;
        return( BLOCK_THEENDDAMMIT );
    } else if( TestIfCharInRange( c, wordList ) ) {
        return( BLOCK_WORD );
    } else if( c == 0 ) {
        return( BLOCK_ENDOFLINE );
    } else if( isspace( c ) ) {
        return( BLOCK_WHITESPACE );
    } else if( big ) {
        return( BLOCK_WORD );
    } else {
        return( BLOCK_DELIM );
    }

} /* charType */

/*
 * incrementMark: move the position of the mark forward and return a
 * pointer to the next logical position in the file, or NULL if we
 * hit a line break (no guarantees about mark position then).
 */
static char *incrementMark( i_mark *mark )
{
    mark->column += 1;
    return( ptrFromMark( mark ) );
}

static char *decrementMark( i_mark *mark )
{
    mark->column -= 1;
    return( ptrFromMark( mark ) );
}

static char *nextLine( i_mark *mark )
{
    do {
        mark->line += 1;
        mark->column = 1;
    } while( LineLength( mark->line ) == 0 && !IsPastLastLine( mark->line ) );
    return( ptrFromMark( mark ) );

} /* nextLine */

static char *prevLine( i_mark *mark )
{
    do {
        mark->line -= 1;
        mark->column = LineLength( mark->line );
    } while( mark->column == 0 && mark->line > 0 );
    return( ptrFromMark( mark ) );

} /* prevLine */

/*
 * eatSpace: increment mark over all whitespace until we hit a
 * character which is not whitespace. This ignores line breaks.
 */
static char *eatSpace( i_mark *mark, bool reverse )
{
    char        *s;

    s = ptrFromMark( mark );
    if( s == NULL ) {
        return( NULL );
    }
    while( charType( *s, TRUE ) == BLOCK_WHITESPACE ) {
        if( reverse ) {
            s = decrementMark( mark );
        } else {
            s = incrementMark( mark );
        }
        if( s == NULL ) {
            if( EditFlags.OperatorWantsMove ) {
                return( NULL );
            }
            if( reverse ) {
                s = prevLine( mark );
            } else {
                s = nextLine( mark );
            }
            if( s == NULL ) {
                break;
            }
        }
    }
    return( s );

} /* eatSpace */

/*
 * MarkStartOfNextWordForward: given a mark curr which denotes a logical
 * current position, we return the location of the next logical word in
 * the result mark, or ERR_NOT_THAT_MANY_WORDS if there are no more logical
 * words in the file. Note that this will span lines.
 */
vi_rc MarkStartOfNextWordForward( i_mark *result, i_mark *curr, bool big )
{
    char        *s;
    btype       block_type;

    noWrap = FALSE;
    *result = *curr;

    s = ptrFromMark( result );
    while( s == NULL ) {
        s = nextLine( result );
        if( s == NULL ) {
            return( ERR_NOT_THAT_MANY_WORDS );
        }
        if( EditFlags.Modeless ) {
            return( ERR_NO_ERR );
        }
    }

    block_type = charType( *s, big );
    while( charType( *s, big ) == block_type ) {
        s = incrementMark( result );
        if( s == NULL ) {
            if( EditFlags.OperatorWantsMove || EditFlags.Modeless ) {
                return( ERR_NO_ERR );
            }
            s = nextLine( result );
            if( s == NULL ) {
                return( ERR_NOT_THAT_MANY_WORDS );
            }
            break;
        }
    }
    eatSpace( result, FALSE );
    return( ERR_NO_ERR );

} /* MarkStartOfNextWordForward */

/*
 * MarkEndOfNextWordForward - find a pointer to the end of the next
 *                            word (in the forwards direction)
 */
vi_rc MarkEndOfNextWordForward( i_mark *result, i_mark *curr, bool big )
{
    char        *s;
    btype       block_type;
    btype       block_type2;

    noWrap = FALSE;
    *result = *curr;
    if( EditFlags.IsChangeWord ) {
        EditFlags.IsChangeWord = FALSE;
        s = ptrFromMark( result );
        if( s == NULL ) {
            return( ERR_NO_ERR );
        }
        block_type = charType( *s, big );
        block_type2 = charType( *(s + 1), big );
        if( block_type == BLOCK_ENDOFLINE ||
            block_type != block_type2 ) {
            return( ERR_NO_ERR );
        }
    }
    s = incrementMark( result );
    if( s == NULL ) {
        s = nextLine( result );
        if( s == NULL ) {
            if( EditFlags.OperatorWantsMove ) {
                *result = *curr;
                return( ERR_NO_ERR );
            }
            return( ERR_NOT_THAT_MANY_WORDS );
        }
    }
    s = eatSpace( result, FALSE );
    if( s == NULL ) {
        return( ERR_NOT_THAT_MANY_WORDS );
    }
    block_type = charType( *s, big );
    while( charType( *(s + 1), big ) == block_type ) {
        s = incrementMark( result );
        if( s == NULL ) {
            break;
        }
    }
    return( ERR_NO_ERR );

} /* MarkEndOfNextWordForward */

/*
 * MarkEndOfNextWordForward - find a pointer to the start of the next
 *                            word (in the backwards direction)
 */
vi_rc MarkStartOfNextWordBackward( i_mark *result, i_mark *curr, bool big )
{
    char        *s;
    btype       block_type;

    noWrap = FALSE;
    *result = *curr;
    s = decrementMark( result );
    if( s == NULL ) {
        s = prevLine( result );
        if( s == NULL ) {
            return( ERR_NOT_THAT_MANY_WORDS );
        }
        if( EditFlags.Modeless ) {
            incrementMark( result );
            return ERR_NO_ERR;
        }
    }
    s = eatSpace( result, TRUE );
    if( s == NULL ) {
        return( ERR_NOT_THAT_MANY_WORDS );
    }

    /*
     * because we are looking at the previous character in the following loop,
     * we have to be careful in case we are sitting at the start of a line.
     */
    if( result->column > 1 ) {
        block_type = charType( *s, big );
        while( charType( *(s - 1), big ) == block_type ) {
            s = decrementMark( result );
            if( s == NULL || result->column == 1 ) {
                break;
            }
        }
    }
    return( ERR_NO_ERR );

} /* MarkStartOfNextWordBackward */

/*
 * GimmeCurrentWord - fetch word at cursor position
 */
vi_rc GimmeCurrentWord( char *buffer, int buffer_size, bool big )
{
    i_mark      curr, end, start;
    int         i, j;
    line        *line;
    fcb         *fcb;
    vi_rc       rc;

    curr = CurrentPos;
    start = curr;
    rc = MarkEndOfNextWordForward( &end, &curr, big );
    if( rc == ERR_NO_ERR ) {
        rc = CGimmeLinePtr( end.line, &fcb, &line );
        if( rc == ERR_NO_ERR ) {
            i = start.column - 1;
            j = 0;
            buffer_size -= 1;
            while( i < end.column && j < buffer_size ) {
                buffer[j++] = line->data[i++];
            }
            buffer[j] = 0;
        }
    }
    return( rc );

} /* GimmeCurrentWord */

/*
 * GimmeCurrentEntireWordDim - fetch forward & backward to get the entire word
 */
vi_rc GimmeCurrentEntireWordDim( int *sc, int *ec, bool big )
{
    i_mark      curr, start, end;
    char        *s;
    int         last_col;
    btype       block_type;
    vi_rc       rc;

    noWrap = FALSE;
    rc = ERR_NO_WORD_TO_FIND;
    curr = CurrentPos;
    s = ptrFromMark( &curr );
    if( s == NULL ) {
        return( rc );
    }
    block_type = charType( *s, big );
    if( block_type == BLOCK_WORD || block_type == BLOCK_DELIM ) {
        end = curr;
        last_col = LineLength( end.line );
        while( charType( *s, big ) == block_type ) {
            if( end.column > last_col ) {
                break;
            }
            s = incrementMark( &end );
            if( s == NULL ) {
                break;
            }
        }
        start = curr;
        s = ptrFromMark( &start );
        while( charType( *s, big ) == block_type ) {
            if( start.column < 1 ) {
                break;
            }
            s = decrementMark( &start );
            if( s == NULL ) {
                break;
            }
        }
        *sc = start.column + 1;
        *ec = end.column - 1;
        rc = ERR_NO_ERR;
    }
    return( rc );

} /* GimmeCurrentEntireWordDim */
