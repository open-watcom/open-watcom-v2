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
#include <string.h>
#include "vi.h"
#include "rxsupp.h"

/*
 * DoMatching - do matching braces command
 */
int DoMatching( range *r, long count )
{
    linenum     line;
    int         column, rc;

    count = count;
    rc = FindMatch( &line, &column );
    r->line_based = FALSE;
    r->start.line = line;
    r->start.column = column;
    return( rc );

} /* DoMatching */

/*
 * FindMatch - do matching braces command
 */
int FindMatch( linenum *xln, int *xcol )
{
    char        *match[2];
    int         matchcnt,which,m1,m2,i;
    linenum     ln;
    int         cl;
    char        matchd[MAX_STR],tmp[MAX_STR];
    char        *linedata;

    /*
     * build match command
     */
    matchd[0] = '!';
    matchd[1] = 0;
    for( i=0;i<MatchCount;i++ ) {
        MySprintf(tmp,"(%s)",MatchData[i] );
        strcat( matchd,tmp );
        if( i != MatchCount-1 ) {
            strcat( matchd,"|" );
        }
    }

    /*
     * find start of match on this line
     */
    ln = CurrentLineNumber;
    cl = CurrentColumn-1;

    if( FindRegularExpression( matchd, &ln, cl, &linedata, ln, FALSE ) ) {
        return( ERR_NOTHING_TO_MATCH );
    }

    /*
     * find out which matched
     */
    for( i=1;i<NSUBEXP;i++ ) {
        if( CurrentRegularExpression->startp[i] != NULL ) {
            which = i-1;
            break;
        }
    }

    cl = GetCurrRegExpColumn( linedata );

    /*
     * get appropriate array entry
     */
    m1 = 2*(which>>1);
    m2 = which % 2;
    match[0] = MatchData[ m1 ];
    match[1] = MatchData[ m1+1 ];
    matchcnt = 1;
    MySprintf( matchd,"!(%s)|(%s)",match[0],match[1] );
    i = CurrentRegComp( matchd );
    if( i ) {
        return( i );
    }

    /*
     * matched the first of a pair, so look for the closing element
     */
    while( TRUE ) {
        if( m2 ) {
            cl--;
            if( FindRegularExpressionBackwards( NULL, &ln, cl, &linedata, -1L, FALSE) ) {
                break;
            }
        } else {
            cl++;
            if( FindRegularExpression( NULL, &ln, cl, &linedata, MAX_LONG, FALSE) ) {
                break;
            }
        }
        cl = GetCurrRegExpColumn( linedata );
        if( CurrentRegularExpression->startp[m2 +1] != NULL ) {
            matchcnt++;
        } else {
            matchcnt--;
            if( matchcnt == 0 ) {
                *xln = ln;
                *xcol = 1+ cl;
                return( ERR_NO_ERR );
            }
        }
    }

    Error( GetErrorMsg( ERR_MATCH_NOT_FOUND ), match[ (m2==0) ] );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* FindMatch */

/*
 * AddMatchString - add another match string
 */
int AddMatchString( char *data )
{
    char        st[MAX_STR],st2[MAX_STR];

    if( MatchCount >= MAX_SEARCH_STRINGS*2-2 ) {
        return( ERR_TOO_MANY_MATCH_STRINGS );
    }
    RemoveLeadingSpaces( data );
    if( NextWordSlash( data,st ) <= 0 ) {
        return( ERR_INVALID_MATCH );
    }
    if( NextWordSlash( data,st2 ) <= 0 ) {
        return( ERR_INVALID_MATCH );
    }
    AddString( &(MatchData[MatchCount]), st );
    AddString( &(MatchData[MatchCount+1]), st2 );
    MatchCount += 2;
    Message1( "match pair \"%s\"-\"%s\" added", st, st2 );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* AddMatchString */

void MatchFini( void ){
    int i;

    for( i=0; i<MatchCount; i++ ){
        MemFree( MatchData[i] );
    }
}
