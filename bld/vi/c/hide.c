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

#if 0
/*
 * DoHideCmd - hide a line range
 */
int DoHideCmd( void )
{
    int         wasline = TRUE;
    vi_rc       rc;
    int         scol, ecol;
    linenum     s, e;

    UpdateCurrentStatus( CSTATUS_HIDE );
    rc = GetRange( 'g', &wasline, &scol, &ecol, &s, &e );
    if( rc != ERR_NO_ERR ) {
        if( rc == ERR_INVALID_RANGE ) {
            rc = ERR_INVALID_HIDE_CMD;
        }
        if( rc == RANGE_REQUEST_CANCELLED ) {
            rc = ERR_NO_ERR;
        }
        return( rc );
    }

    if( wasline == 0 ) {
        e = s = CurrentLineNumber;
    }

    return( HideLineRange( s, e, FALSE ) );

} /* DoHideCmd */
#endif

/*
 * HideLineRange - hide/unhide a given line range
 */
vi_rc HideLineRange( linenum s, linenum e, bool unhide )
{
    vi_rc       rc;
    bool        hideval;
    fcb         *cfcb;
    line        *cline;
    char        *st;
    linenum     c;

    hideval = TRUE;
    if( unhide ) {
        hideval = FALSE;
    }

    for( c = s; c <= e; c++ ) {
        rc = CGimmeLinePtr( c, &cfcb, &cline );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        cline->inf.ld.hidden = hideval;
    }

    DCDisplayAllLines();
    if( unhide ) {
        st = "revealed";
    } else {
        st = "hidden";
    }
    Message1( "%l lines %s", e - s + 1, st );
    EditFlags.Dotable = TRUE;
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* HideLineRange */

#if 0
/*
 * GetHiddenRange - get range of lines hidden around a given line
 */
void GetHiddenRange( linenum l, linenum *s, linenum *e )
{
    line        *cline, *oline;
    fcb         *cfcb, *ofcb;
    int         i;

    (*s) = (*e) = l;

    i = CGimmeLinePtr( l, &ofcb, &oline );
    if( i ) {
        return;
    }

    /*
     * go back
     */
    cfcb = ofcb;
    cline = oline;
    while( 1 ) {
        i = GimmePrevLinePtr( &cfcb, &cline );
        if( i ) {
            break;
        }
        if( cline->inf.ld.hidden ) {
            (*s)--;
            continue;
        } else {
            break;
        }
    }

    /*
     * go forwards
     */
    cfcb = ofcb;
    cline = oline;
    while( 1 ) {
        i = CGimmeNextLinePtr( &cfcb, &cline );
        if( i ) {
            break;
        }
        if( cline->inf.ld.hidden ) {
            (*e)++;
            continue;
        } else {
            break;
        }
    }

} /* GetHiddenRange */

/*
 * GetHiddenBreaks
 */
linenum GetHiddenLineBreaks( linenum s, linenum e )
{
    linenum     curr;
    line        *cline;
    fcb         *cfcb;
    int         i;
    linenum     cnt, s1, e1, tmp;

    if( s > e ) {
        tmp = s;
        s = e;
        e = tmp;
    }
    cnt = 0L;
    curr = s;
    while( curr <=e ) {
        i = CGimmeLinePtr( curr, &cfcb, &cline );
        if( i ) {
            break;
        }
        if( cline->inf.ld.hidden ) {
            GetHiddenRange( curr, &s1, &e1 );
            curr = e1 + 1;
            cnt++;
        } else {
            curr++;
        }
    }
    return( cnt );

} /* GetHiddenLineBreaks */

/*
 * GetHiddenLineCount - get number of hidden lines in a range
 */
linenum GetHiddenLineCount( linenum s, linenum e )
{
    line        *cline;
    fcb         *cfcb;
    int         i;
    linenum     cnt, tmp;

    if( s > e ) {
        tmp = s;
        s = e;
        e = tmp;
    }
    i = CGimmeLinePtr( s, &cfcb, &cline );
    if( i ) {
        return( 0L );
    }
    cnt = 0L;
    while( s <= e ) {
        if( cline->inf.ld.hidden ) {
            cnt++;
        }
        s++;
        i = CGimmeNextLinePtr( &cfcb, &cline );
        if( i ) {
            break;
        }
    }
    return( cnt );

} /* GetHiddenLineCount */
#endif
