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

static unsigned  unMark( mark * );
static vi_rc invalidMark( mark *, unsigned );
static vi_rc goToMark( range * );
static vi_rc getAMark( bool, linenum *, int * );
static vi_rc tryToFindMark( mark *, unsigned );

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
#define KEY_TO_NO( c )  (((c) == VI_KEY( GRAVE ) || (c) == VI_KEY( QUOTE )) ? MAX_MARKS + 1 : (c) - VI_KEY( a ) + 1)

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
    key = GetNextEvent( false );
    if( key == VI_KEY( ESC ) ) {
        return( MARK_REQUEST_CANCELLED );
    }
    if( key == VI_KEY( DOT ) ) {
        if( EditFlags.MemorizeMode ) {
            return( DoDotMode() );
        } else {
            Message1( "%sstarted", MEMORIZE_MODE );
            DotDigits = 0;
            EditFlags.MemorizeMode = true;
            return( ERR_NO_ERR );
        }
    }
    if( key == VI_KEY( EQUALS ) ) {
        if( EditFlags.AltMemorizeMode ) {
            return( DoAltDotMode() );
        } else {
            Message1( "Alternate %sstarted", MEMORIZE_MODE );
            AltDotDigits = 0;
            EditFlags.AltMemorizeMode = true;
            return( ERR_NO_ERR );
        }
    }
    return( SetGenericMark( CurrentPos.line, CurrentPos.column, key ) );

} /* SetMark */

/*
 * MarkOnLine - this function returns true if the given mark is one the
 *              given line. It simply follows the trail through the mark
 *              list until it finds a NO_MARK or the mark it is searching for.
 */
static bool MarkOnLine( line *line, unsigned no )
{
    mark        *m;

    if( line->u.ld.mark == no ) {
        return( true );
    }
    for( m = MARK_PTR( line->u.ld.mark ); m->next != NO_MARK; m = MARK_PTR( m->next ) ) {
        if( m->next == no ) {
            return( true );
        }
    }
    return( false );

} /* MarkOnLine */

/*
 * RemoveMarkFromLine - removes the given mark from whatever line we
 *                      happen to find it on. After this the mark is no
 *                      longer in use.
 */
static vi_rc RemoveMarkFromLine( unsigned no )
{
    mark        *mark, *curr;
    fcb         *fcb;
    line        *line;
    vi_rc       rc;

    assert( no != NO_MARK && no <= MAX_MARKS + 1 );
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
    if( line->u.ld.mark != NO_MARK ) {
        if( line->u.ld.mark == no ) {
            line->u.ld.mark = mark->next;
        } else {
            for( curr = MARK_PTR( line->u.ld.mark ); curr->next != no; curr = MARK_PTR( curr->next ) ) {
                if( curr->next == NO_MARK ) {
                    /* we have run through the linked list and not found it */
                    /* so we must have a lost mark here */
                    return( ERR_MARK_NOT_SET );
                }
            }
            /* remove it from the linked list */
            curr->next = mark->next;
        }
        mark->inuse = false;
        return( ERR_NO_ERR );
    }
    return( ERR_MARK_NOT_SET );

} /* RemoveMarkFromLine */

/*
 * SetGenericMark - set a mark at a generic line
 */
vi_rc SetGenericMark( linenum num, int col, vi_key key )
{
    unsigned    no;
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
    if( key == VI_KEY( EXCLAMATION ) ) {
        no = mline->u.ld.mark;
        while( no != NO_MARK ) {
            no = unMark( MARK_PTR( no ) );
        }
        Message1( "Marks cleared on line" );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }

    /*
     * get mark to  use
     */
    if( key < VI_KEY( a ) || key > VI_KEY( z ) ) {
        return( ERR_INVALID_MARK_RANGE );
    }

    no = KEY_TO_NO( key );;
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
    cmark->next = mline->u.ld.mark;
    mline->u.ld.mark = no;
    cmark->p.line = num;
    cmark->p.column = col;
    cmark->inuse = true;

    Message1( "Mark '%c' set", (char)key );

    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* SetMark */

vi_rc GoMark( range *r, long count )
{
    count = count;
    r->line_based = false;
    return( goToMark( r ) );
}

vi_rc GoMarkLine( range *r, long count )
{
    count = count;
    r->line_based = true;
    return( goToMark( r ) );
}

vi_rc GetMarkLine( linenum *ln )
{
    return( getAMark( true, ln, NULL ) );
}

vi_rc GetMark( linenum *ln, int *cl )
{
    return( getAMark( false, ln, cl ) );
}

/*
 * getAMark - get a specified mark
 */
static vi_rc getAMark( bool lineonly, linenum *ln, int *cl )
{
    unsigned    no;
    mark        *m;
    vi_rc       rc;
    vi_key      key;

    /*
     * get mark to go to
     */
    key = GetNextEvent( false );
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
    unsigned    no;
    mark        *m;
    vi_rc       rc;
    vi_key      key;

    if( CurrentFile == NULL ) {
        return( ERR_NO_FILE );
    }

    /*
     * get mark to go to
     */
    key = GetNextEvent( false );
    if( key == VI_KEY( ESC ) ) {
        return( MARK_REQUEST_CANCELLED );
    }

    /*
     * NOTE: the line_based flag was set by whoever called this routine.
     * Both the column and line are filled in and it is up to the operator
     * to ignore the column if r->line_based is true.
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
vi_rc VerifyMark( unsigned no, bool lineonly )
{
    fcb         *cfcb;
    line        *cline;
    mark        *cmark;
    int         len;
    vi_rc       rc;

    if( no == NO_MARK || no > MAX_MARKS + 1 ) {
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
static unsigned unMark( mark *cmark )
{
    unsigned next;

    cmark->inuse = false;
    next = cmark->next;
    cmark->next = NO_MARK;
    return( next );

} /* unMark */

/*
 * invalidMark - set a mark as no longer vaid
 */
static vi_rc invalidMark( mark *cmark, unsigned no )
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
    currContext->inuse = true;
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
    MemFreePtr( (void **)&MarkList );
    /* set currContext to NULL so we get a GPFault if we deref it */
    currContext = NULL;

} /* FreeMarkList */

/*
 * tryToFindMark - try to find a moved mark
 */
static vi_rc tryToFindMark( mark *cmark, unsigned no )
{
    fcb         *cfcb;
    line        *cline;
    linenum     lineno = 1;
    vi_rc       rc;

    rc = CGimmeLinePtr( 1, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    for( ;; ) {
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
