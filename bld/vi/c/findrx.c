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
#include "rxsupp.h"

static char wrapMsg[] = "Wrapped past %s of file";
static bool wrapMsgPrinted = FALSE;

/*
 * FindRegularExpression - do a forward search for a regular expression
 */
vi_rc FindRegularExpression( char *pat, i_mark *pos1, char **linedata,
                             linenum termline, find_type flags )
{
    vi_rc       rc;
    int         found;
    linenum     ilineno = 0;
    bool        wrapped = FALSE;
    char        *data;
    line        *cline;
    fcb         *cfcb;
    int         scol;
    linenum     sline;

    /*
     * initialize for search
     */
    if( wrapMsgPrinted ) {
        wrapMsgPrinted = FALSE;
        ClearWindow( MessageWindow );
    }
    sline = pos1->line;
    if( flags & FINDFL_WRAP ) {
        ilineno = sline;
    }
    rc = CGimmeLinePtr( sline, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    scol = pos1->column;
    if( pat != NULL ) {
        rc = CurrentRegComp( pat );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

    /*
     * loop until string found
     */
    data = &cline->data[scol];
    while( (found = RegExec( CurrentRegularExpression, data, (data == cline->data) )) == FALSE ) {
        if( RegExpError != ERR_NO_ERR ) {
            return( RegExpError );
        }
        /*
         * get next line
         */
        rc = CGimmeNextLinePtr( &cfcb, &cline );
        if( rc == ERR_NO_ERR ) {
            ++sline;
        } else if( rc == ERR_NO_MORE_LINES ) {
            if( !(flags & FINDFL_WRAP) ) {
                return( ERR_FIND_END_OF_FILE );
            } else {
                Message1( wrapMsg, "bottom" );
                MyBeep();
                wrapMsgPrinted = TRUE;
            }
            if( wrapped ) {
                return( ERR_FIND_NOT_FOUND );
            }
            sline = 1;
            rc = CGimmeLinePtr( sline, &cfcb, &cline );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            wrapped = TRUE;
        } else {
            return( rc );
        }
        if( sline > termline ) {
            return( ERR_FIND_PAST_TERM_LINE );
        }
        if( wrapped ) {
            if( sline > ilineno ) {
                return( ERR_FIND_NOT_FOUND );
            }
        }
        scol = 0;
        data = cline->data;
    }
    *linedata = cline->data;
    pos1->column = GetCurrRegExpColumn( cline->data );
    pos1->line = sline;
    return( ERR_NO_ERR );

} /* FindRegularExpression */

/*
 * FindRegularExpressionBackwards - do a reverse search for a regular expression
 */
vi_rc FindRegularExpressionBackwards( char *pat, i_mark *pos1, char **linedata,
                                      linenum termline, find_type flags )
{
    vi_rc       rc;
    char        *data;
    bool        wrapped = FALSE;
    bool        found;
    linenum     ilineno = 0;
    line        *cline;
    fcb         *cfcb;
    regexp      rcpy;
    int         scol;
    linenum     sline;

    /*
     * initialize for search
     */
    if( wrapMsgPrinted ) {
        wrapMsgPrinted = FALSE;
        ClearWindow( MessageWindow );
    }
    sline = pos1->line;
    rc = CGimmeLinePtr( sline, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( flags & FINDFL_WRAP ) {
        ilineno = sline;
    }
    scol = pos1->column;
    if( pat != NULL ) {
        rc = CurrentRegComp( pat );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    memset( &rcpy, 0, sizeof( rcpy ) );

    /*
     * loop until string found
     */
    for( ;; ) {
        data = cline->data;
        found = FALSE;
        /*
         * run through all possible matches on the line, accepting
         * only the last one
         */
        if( scol >= 0 ) {
            while( *data != '\0' && RegExec( CurrentRegularExpression, data, (data == cline->data) ) == TRUE ) {
                int     col, len;

                if( RegExpError != ERR_NO_ERR ) {
                    return( RegExpError );
                }
                col = GetCurrRegExpColumn( cline->data );
                len = GetCurrRegExpLength();
                if( col + len > scol ) {
                    break;
                }
                found = TRUE;
                memcpy( &rcpy, CurrentRegularExpression, sizeof( regexp ) );
                data = &(cline->data[col + 1]);
            }
            if( found ) {
                break;
            }
        }

        /*
         * get next line
         */
        rc = GimmePrevLinePtr( &cfcb, &cline );
        if( rc == ERR_NO_ERR ) {
            --sline;
        } else if( rc == ERR_NO_MORE_LINES ) {
            if( !(flags & FINDFL_WRAP) ) {
                return( ERR_FIND_TOP_OF_FILE );
            } else {
                Message1( wrapMsg, "top" );
                MyBeep();
                wrapMsgPrinted = TRUE;
            }
            if( wrapped ) {
                return( ERR_FIND_NOT_FOUND );
            }
            rc = CFindLastLine( &sline );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            rc = CGimmeLinePtr( sline, &cfcb, &cline );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            wrapped = TRUE;
        } else {
            return( rc );
        }
        if( sline < termline ) {
            return( ERR_FIND_PAST_TERM_LINE );
        }
        if( wrapped ) {
            if( sline < ilineno ) {
                return( ERR_FIND_NOT_FOUND );
            }
        }
        scol = cline->len;
    }
    *linedata = cline->data;
    memcpy( CurrentRegularExpression, &rcpy, sizeof( regexp ) );
    pos1->column = GetCurrRegExpColumn( cline->data );
    pos1->line = sline;
    return( ERR_NO_ERR );

} /* FindRegularExpressionBackwards */
