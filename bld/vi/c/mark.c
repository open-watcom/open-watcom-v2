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
#include <assert.h>

static int  unMark( mark * );
static vi_rc invalidMark( mark *, int );
static vi_rc goToMark( range * );
static vi_rc getAMark( int, linenum *, int * );
static vi_rc tryToFindMark( mark *, int );

/*
 * Mark numbers etc: everywhere you see a "no" used to indicate a
 * mark, this is the index, **BASE 1**, into the MarkList array. This
 * way a '0' or NO_MARK indicates a lack of marks which makes a
 * handy default since line structures are zeroed when allocated.
 * To hide the ugliness, we use these macros below to calculate
 * the no for a given key press and get a pointer for a given number.
 */
#define NO_MARK         0
#define MARK_PTR( x )   (&MarkList[(x) - 1])
#define KEY_TO_NO( c )  (((c) == '`' || (c) == '\'') ? MAX_MARKS + 1 : (c) - 'a' + 1)

static mark *currContext;

/*
 * SetMark - set a mark at current position
 */
vi_rc SetMark( void )
{
    vi_key      key;

    /*
     * get mark to set
     */
    key = GetNextEvent( FALSE );
    if( key == VI_KEY( ESC ) ) {
        return( MARK_REQUEST_CANCELLED );
    }
    if( key == '.' ) {
        if( EditFlags.MemorizeMode ) {
            return( DoDotMode() );
        } else {
            Message1( "%sstarted", MEMORIZE_MODE );
            DotDigits = 0;
            EditFlags.MemorizeMode = TRUE;
            return( ERR_NO_ERR );
        }
    }
    if( key == '=' ) {
        if( EditFlags.AltMemorizeMode ) {
            return( DoAltDotMode() );
        } else {
            Message1( "Alternate %sstarted", MEMORIZE_MODE );
            AltDotDigits = 0;
            EditFlags.AltMemorizeMode = TRUE;
            return( ERR_NO_ERR );
        }
    }
    return( SetGenericMark( CurrentPos.line, CurrentPos.column, key ) );

} /* SetMark */

/*
 * MarkOnLine - this function returns TRUE if the given mark is one the
 *              given line. It simply follows the trail through the mark
 *              list until it finds a NO_MARK or the mark it is searching for.
 */
bool MarkOnLine( line *line, int no )
{
    mark        *m;

    if( line->inf.ld.mark == no ) {
        return( TRUE );
    }
    m = MARK_PTR( line->inf.ld.mark );
    while( m->next != NO_MARK ) {
        if( m->next == no ) {
            return( TRUE );
        }
        m = MARK_PTR( m->next );
    }
    return( FALSE );

} /* MarkOnLine */

/*
 * RemoveMarkFromLine - removes the given mark from whatever line we
 *                      happen to find it on. After this the mark is no
 *                      longer in use.
 */
vi_rc RemoveMarkFromLine( int no )
{
    mark        *mark, *curr;
    fcb         *fcb;
    line        *line;
    vi_rc       rc;

    assert( no > 0 && no <= MAX_MARKS + 1 );
    mark = MARK_PTR( no );
    rc = CGimmeLinePtr( mark->p.line, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        /* hmmmm.... we are in trouble here I believe */
        /* should try and find it somewhere else */
        rc = tryToFindMark( mark, no - 1 );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        /* this gimme should be guaranteed to work since we did it in
           tryToFindMark */
        CGimmeLinePtr( mark->p.line, &fcb, &line );
    }
    if( line->inf.ld.mark != NO_MARK ) {
        if( line->inf.ld.mark == no ) {
            line->inf.ld.mark = mark->next;
        } else {
            curr = MARK_PTR( line->inf.ld.mark );
            while( curr->next != no ) {
                if( curr->next == NO_MARK ) {
                    /* we have run through the linked list and not found it */
                    /* so we must have a lost mark here */
                    return( ERR_MARK_NOT_SET );
                }
                curr = MARK_PTR( curr->next );
            }
            /* remove it from the linked list */
            curr->next = mark->next;
        }
        mark->inuse = FALSE;
        return( ERR_NO_ERR );
    }
    return( ERR_MARK_NOT_SET );

} /* RemoveMarkFromLine */

/*
 * SetGenericMark - set a mark at a generic line
 */
vi_rc SetGenericMark( linenum num, int col, char mlet )
{
    int         no;
    mark        *cmark;
    line        *mline;
    fcb         *mfcb;
    vi_rc       rc;

    rc = CGimmeLinePtr( num, &mfcb, &mline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * unmark the current line
     */
    if( mlet == '!' ) {
        no = mline->inf.ld.mark;
        while( no != NO_MARK ) {
            no = unMark( MARK_PTR( no ) );
        }
        Message1( "Marks cleared on line" );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }

    /*
     * get mark to  use
     */
    if( mlet < 'a' || mlet > 'z' ) {
        return( ERR_INVALID_MARK_RANGE );
    }

    no = KEY_TO_NO( mlet );;
    cmark = MARK_PTR( no );

    /*
     * purge the old mark
     */
    if( cmark->inuse ) {
        /* we don't check this return value because even if it
           fails we want to nuke the mark */
        RemoveMarkFromLine( no );
    }

    /*
     * do the set
     */
    cmark->next = mline->inf.ld.mark;
    mline->inf.ld.mark = no;
    cmark->p.line = num;
    cmark->p.column = col;
    cmark->inuse = TRUE;

    Message1( "Mark '%c' set", mlet );

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* SetMark */

vi_rc GoMark( range *r, long count )
{
    count = count;
    r->line_based = FALSE;
    return( goToMark( r ) );
}

vi_rc GoMarkLine( range *r, long count )
{
    count = count;
    r->line_based = TRUE;
    return( goToMark( r ) );
}

vi_rc GetMarkLine( linenum *ln )
{
    return( getAMark( TRUE, ln, NULL ) );
}

vi_rc GetMark( linenum *ln, int *cl )
{
    return( getAMark( FALSE, ln, cl ) );
}

/*
 * getAMark - get a specified mark
 */
static vi_rc getAMark( int lineonly, linenum *ln, int *cl )
{
    int         no;
    mark        *m;
    vi_rc       rc;
    vi_key      key;

    /*
     * get mark to go to
     */
    key = GetNextEvent( FALSE );
    if( key == VI_KEY( ESC ) ) {
        return( MARK_REQUEST_CANCELLED );
    }

    no = KEY_TO_NO( key );
    m = MARK_PTR( no );
    rc = VerifyMark( no, lineonly );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    *ln = m->p.line;
    if( !lineonly ) {
        *cl = m->p.column;
    }

    return( ERR_NO_ERR );

} /* getAMark */

/*
 * goToMark - go to a specified mark
 */
static vi_rc goToMark( range *r )
{
    int         no;
    mark        *m;
    vi_rc       rc;
    vi_key      key;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    /*
     * get mark to go to
     */
    key = GetNextEvent( FALSE );
    if( key == VI_KEY( ESC ) ) {
        return( MARK_REQUEST_CANCELLED );
    }

    /*
     * NOTE: the line_based flag was set by whoever called this routine.
     * Both the column and line are filled in and it is up to the operator
     * to ignore the column if r->line_based is TRUE.
     */
    no = KEY_TO_NO( key );
    m = MARK_PTR( no );
    rc = VerifyMark( no, r->line_based );
    r->start = m->p;
    return( rc );

} /* goToMark */

/*
 * VerifyMark - check that a mark is okay
 */
vi_rc VerifyMark( int no, int lineonly )
{
    fcb         *cfcb;
    line        *cline;
    mark        *cmark;
    int         len;
    vi_rc       rc;

    if( no <= 0 || no > MAX_MARKS + 1 ) {
        return( ERR_INVALID_MARK_RANGE );
    }
    if( no == MAX_MARKS + 1 ) {
        return( ERR_NO_ERR );
    }
    cmark = MARK_PTR( no );
    if( !cmark->inuse ) {
        Error( GetErrorMsg( ERR_MARK_NOT_SET ), no + 'a' - 1 );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    rc = CGimmeLinePtr( cmark->p.line, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        if( rc == ERR_NO_SUCH_LINE ) {
            if( tryToFindMark( cmark, no ) ) {
                return( invalidMark( cmark, no ) );
            }
            rc = CGimmeLinePtr( cmark->p.line, &cfcb, &cline );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
        } else {
            return( rc );
        }
    }
    if( !MarkOnLine( cline, no ) ) {
        if( tryToFindMark( cmark, no ) ) {
            return( invalidMark( cmark, no ) );
        }
        rc = CGimmeLinePtr( cmark->p.line, &cfcb, &cline );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    if( !lineonly ) {
        len = cline->len;
        if( len == 0 ) {
            len = 1;
        }
        if( cmark->p.column > len ) {
            return( invalidMark( cmark, no) );
        }
    }
    return( ERR_NO_ERR );

} /* VerifyMark */

/*
 * unMark - clear a mark
 */
static int unMark( mark *cmark )
{
    int next;

    cmark->inuse = FALSE;
    next = cmark->next;
    cmark->next = 0;
    return( next );

} /* unMark */

/*
 * invalidMark - set a mark as no longer vaid
 */
static vi_rc invalidMark( mark *cmark, int no )
{
    int         i;
    mark        *m;

    m = &MarkList[0];
    /* remove the mark from any linked lists it is in */
    for( i = 0; i < MAX_MARKS; i++, m++ ) {
        if( m->next == no ) {
            m->next = cmark->next;
            break;
        }
    }
    unMark( cmark );
    Error( GetErrorMsg( ERR_MARK_NOW_INVALID ), no + 'a' - 1 );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* invalidMark */

/*
 * AllocateMarkList - do ust that
 */
void AllocateMarkList( void )
{
    MarkList = MemAlloc( MARK_SIZE * (MAX_MARKS + 1) );
    SetMarkContext();
    currContext->inuse = TRUE;
    currContext->p.line = 1;
    currContext->p.column = 1;

} /* AllocateMarkList */

/*
 * SetMarkContext - set mark currContext ptr
 */
void SetMarkContext( void )
{
    if( MarkList != NULL ) {
        currContext = &MarkList[MAX_MARKS];
    } else {
        currContext = NULL;
    }

} /* SetMarkContext */

/*
 * FreeMarkList - do just that
 */
void FreeMarkList( void )
{
    MemFree2( &MarkList );
    /* set currContext to NULL so we get a GPFault if we deref it */
    currContext = NULL;

} /* FreeMarkList */

/*
 * tryToFindMark - try to find a moved mark
 */
static vi_rc tryToFindMark( mark *cmark, int no )
{
    fcb         *cfcb;
    line        *cline;
    linenum     lineno = 1;
    vi_rc       rc;

    rc = CGimmeLinePtr( 1, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    while( TRUE ) {
        if( MarkOnLine( cline, no ) ) {
            cmark->p.line = lineno;
            return( ERR_NO_ERR );
        }
        lineno++;
        rc = CGimmeNextLinePtr( &cfcb, &cline );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

} /* tryToFindMark */

/*
 * MemorizeCurrentContext - keep track of last context mark
 */
void MemorizeCurrentContext( void )
{
    if( currContext != NULL ) {
        currContext->p = CurrentPos;
    }

} /* MemorizeCurrentContext */
