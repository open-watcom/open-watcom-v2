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
#include "win.h"
#include "rxsupp.h"

static char wrapMsg[] = "Wrapped past %s of file";
static bool wrapMsgPrinted;

/*
 * FindRegularExpression - do a forward search for a regular expression
 */
int FindRegularExpression( char *pat, linenum *clineno, int ccol,
                           char **linedata, linenum termline, int sw )
{
    int         i,scol,rc=FALSE;
    linenum     ilineno;
    bool        wrapped=FALSE;
    char        *data;
    line        *cline;
    fcb         *cfcb;

    /*
     * initialize for search
     */
    if( wrapMsgPrinted ) {
        wrapMsgPrinted = FALSE;
        ClearWindow( MessageWindow );
    }
    if( sw ) {
        ilineno = *clineno;
    }
    i = CGimmeLinePtr( *clineno, &cfcb, &cline );
    if( i ) {
        return( i );
    }
    scol = ccol;
    if( pat != NULL ) {
        i = CurrentRegComp( pat );
        if( i ) {
            return( i );
        }
    }

    /*
     * loop until string found
     */
    while( TRUE ) {

        data = &cline->data[scol];
        rc = RegExec( CurrentRegularExpression, data, (scol==0) );
        if( RegExpError != ERR_NO_ERR ) {
            return( RegExpError );
        }

        if( rc ) {
            *linedata = cline->data;
            return( ERR_NO_ERR );
        }

        /*
         * get next line
         */
        i = CGimmeNextLinePtr( &cfcb, &cline );
        if( i ) {
            if( i == ERR_NO_MORE_LINES ) {
                if( !sw ) {
                    return( ERR_FIND_END_OF_FILE );
                } else {
                    Message1( wrapMsg, "bottom" );
                    MyBeep();
                    wrapMsgPrinted = TRUE;
                }
                if( wrapped ) {
                    return( ERR_FIND_NOT_FOUND );
                }
                *clineno = 1;
                i = CGimmeLinePtr( *clineno, &cfcb, &cline );
                if( i ) {
                    return( i );
                }
                *clineno = 0;
                wrapped = TRUE;
            } else {
                return( i );
            }
        }
        scol = 0;
        (*clineno) += 1;
        if( *clineno > termline ) {
            return( ERR_FIND_PAST_TERM_LINE );
        }
        if( wrapped ) {
            if( *clineno > ilineno ) {
                return( ERR_FIND_NOT_FOUND );
            }
        }

    }

} /* FindRegularExpression */

/*
 * FindRegularExpressionBackwards - do a reverse search for a regular expression
 */
int FindRegularExpressionBackwards( char *pat, linenum *clineno, int ccol,
                           char **linedata, linenum termline, int sw )
{
    int         i,scol,rc,col,len;
    char        *data;
    bool        wrapped=FALSE,found;
    linenum     ilineno;
    line        *cline;
    fcb         *cfcb;
    regexp      rcpy;

    /*
     * initialize for search
     */
    i = CGimmeLinePtr( *clineno, &cfcb, &cline );
    if( i ) {
        return( i );
    }
    if( sw ) {
        ilineno = *clineno;
    }
    scol = ccol;
    if( pat != NULL ) {
        i = CurrentRegComp( pat );
        if( i ) {
            return( i );
        }
    }

    /*
     * loop until string found
     */
    while( TRUE ) {

        data = cline->data;
        found = FALSE;
        /*
         * run through all possible matches on the line, accepting
         * only the last one
         */
        while( TRUE ) {
            rc = RegExec( CurrentRegularExpression, data, (data==cline->data) );
            if( RegExpError != ERR_NO_ERR ) {
                return( RegExpError );
            }
            if( rc ) {
                col = GetCurrRegExpColumn( cline->data );
                len = GetCurrRegExpLength();
                if( col + len - 1 > scol ) {
                    break;
                }
                found = TRUE;
                memcpy( &rcpy, CurrentRegularExpression, sizeof( regexp ) );
                data = &(cline->data[col+1]);
                if( *data == 0 ) {
                    break;
                }
            } else {
                break;
            }
        }

        if( found ) {
            *linedata = cline->data;
            memcpy( CurrentRegularExpression, &rcpy, sizeof( regexp ) );
            return( ERR_NO_ERR );
        }

        /*
         * get next line
         */
        i = GimmePrevLinePtr( &cfcb, &cline );
        if( i ) {
            if( i == ERR_NO_MORE_LINES ) {
                if( !sw ) {
                    return( ERR_FIND_TOP_OF_FILE );
                } else {
                    Message1( wrapMsg, "top" );
                    MyBeep();
                    wrapMsgPrinted = TRUE;
                }
                if( wrapped ) {
                    return( ERR_FIND_NOT_FOUND );
                }
                i = CFindLastLine( clineno );
                if( i ) {
                    return( i );
                }
                i = CGimmeLinePtr( *clineno, &cfcb, &cline );
                if( i ) {
                    return( i );
                }
                (*clineno) += 1;
                wrapped = TRUE;
            } else {
                return( i );
            }
        }
        scol = cline->len-1;
        (*clineno) -= 1;
        if( *clineno < termline ) {
            return( ERR_FIND_PAST_TERM_LINE );
        }
        if( wrapped ) {
            if( *clineno < ilineno ) {
                return( ERR_FIND_NOT_FOUND );
            }
        }

    }

} /* FindRegularExpressionBackwards */
