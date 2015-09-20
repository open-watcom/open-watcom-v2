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
#include "rxsupp.h"

/*
 * DoMatching - do matching braces command
 */
vi_rc DoMatching( range *r, long count )
{
    vi_rc       rc;
    i_mark      pos;

    count = count;
    rc = FindMatch( &pos );
    r->line_based = false;
    r->start = pos;
    return( rc );

} /* DoMatching */

/*
 * FindMatch - do matching braces command
 */
vi_rc FindMatch( i_mark *pos1 )
{
    char        *match[2];
    int         matchcnt, which, m1, m2, i;
    char        matchd[MAX_STR], tmp[MAX_STR];
    char        *linedata;
    i_mark      pos2;
    vi_rc       rc;

    /*
     * build match command
     */
    matchd[0] = '\0';
    for( i = 0; i < MatchCount; i++ ) {
        if( i > 0 ) {
            strcat( matchd, "|" );
        }
        MySprintf( tmp, "(%s)", MatchData[i] );
        strcat( matchd, tmp );
    }

    /*
     * find start of match on this line
     */
    which = 0;
    pos2 = CurrentPos;
    pos2.column -= 1;
    RegExpAttrSave( -1, NULL );
    rc = FindRegularExpression( matchd, &pos2, &linedata, pos2.line, 0 );
    if( rc == ERR_NO_ERR ) {
        /*
         * find out which matched
         */
        for( i = 1; i < NSUBEXP; i++ ) {
            if( CurrentRegularExpression->startp[i] != NULL ) {
                which = i - 1;
                break;
            }
        }
    
        /*
         * get appropriate array entry
         */
        m1 = 2 * (which >> 1);
        m2 = which % 2;
        match[0] = MatchData[m1];
        match[1] = MatchData[m1 + 1];
        matchcnt = 1;
        MySprintf( matchd, "(%s)|(%s)", match[0], match[1] );
        rc = CurrentRegComp( matchd );
        if( rc == ERR_NO_ERR ) {
            /*
             * matched the first of a pair, so look for the closing element
             */
            for( ;; ) {
                if( m2 ) {
                    pos2.column--;
                    rc = FindRegularExpressionBackwards( NULL, &pos2, &linedata, -1L, 0 );
                } else {
                    pos2.column++;
                    rc = FindRegularExpression( NULL, &pos2, &linedata, MAX_LONG, 0 );
                }
                if( rc != ERR_NO_ERR ) {
                    Error( GetErrorMsg( ERR_MATCH_NOT_FOUND ), match[(m2 == 0)] );
                    rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
                    break;
                }
                if( CurrentRegularExpression->startp[m2 + 1] != NULL ) {
                    matchcnt++;
                } else {
                    matchcnt--;
                    if( matchcnt == 0 ) {
                        *pos1 = pos2;
                        pos1->column += 1;
                        break;
                    }
                }
            }
        }
    } else {
        rc = ERR_NOTHING_TO_MATCH;
    }
    RegExpAttrRestore();
    return( rc );

} /* FindMatch */

/*
 * AddMatchString - add another match string
 */
vi_rc AddMatchString( char *data )
{
    char        st[MAX_STR], st2[MAX_STR];

    if( MatchCount >= MAX_SEARCH_STRINGS * 2 - 2 ) {
        return( ERR_TOO_MANY_MATCH_STRINGS );
    }
    RemoveLeadingSpaces( data );
    if( NextWordSlash( data, st ) <= 0 ) {
        return( ERR_INVALID_MATCH );
    }
    if( NextWordSlash( data, st2 ) <= 0 ) {
        return( ERR_INVALID_MATCH );
    }
    MatchData[MatchCount] = DupString( st );
    MatchData[MatchCount + 1] = DupString( st2 );
    MatchCount += 2;
    Message1( "match pair \"%s\"-\"%s\" added", st, st2 );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* AddMatchString */

void MatchInit( void )
{
    MatchData[0] = MemStrDup( "{" );
    MatchData[1] = MemStrDup( "}" );
    MatchData[2] = MemStrDup( "\\(" );
    MatchData[3] = MemStrDup( "\\)" );
}

void MatchFini( void )
{
    int i;

    for( i = 0; i < MatchCount; i++ ){
        MemFree( MatchData[i] );
    }
}
