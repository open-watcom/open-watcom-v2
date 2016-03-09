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
 * getBracketLoc - find a matching '(' for a ')'
 */
static vi_rc getBracketLoc( i_mark *pos )
{
    vi_rc       rc;
    char        tmp[3];
    int         len;
//    linenum     lne;

    tmp[0] = '\\';
    tmp[1] = ')';
    tmp[2] = '\0';
//    lne = CurrentPos.line;
    RegExpAttrSave( -1, NULL );
    rc = GetFind( tmp, pos, &len, FINDFL_BACKWARDS | FINDFL_NOERROR | FINDFL_NOCHANGE );
    RegExpAttrRestore();
    if( pos->line != CurrentPos.line ) {
        return( ERR_FIND_NOT_FOUND );
    }
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * find the matching '('
     */
    CurrentPos = *pos;
    CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
    rc = FindMatch( pos );
    return( rc );

} /* getBracketLoc */

/*
 * GetAutoIndentAmount - do just that
 */
int GetAutoIndentAmount( char *buff, int extra, bool above_line )
{
    int         i, j = 0, k;
    bool        tabme;
    vi_rc       rc;
    line        *cline;
    fcb         *cfcb;
    char        ch;
    int         indent_amount;
    i_mark      pos;

    while( EditFlags.AutoIndent ) {
        i = FindStartOfCurrentLine();
        cline = CurrentLine;
        /*
         * find extra indentation due to cmode
         */
        if( EditFlags.CMode ) {
            if( EditFlags.CaseShift ) {
            }
            for( k = cline->len - 1; k >= 0; k-- ) {
                ch = cline->data[k];
                if( ch == '{' || ch == '}' ) {
                    if( ch == '{' && above_line ) {
                        break;
                    }
                    if( ch == '}' && !above_line ) {
                        break;
                    }
                    extra += EditVars.ShiftWidth;
                    SaveCurrentFilePos();
                    CurrentPos.column = k + 1;
                    /* add a { to keep matches even! */
                    if( ch == '}' ) {
                        rc = FindMatch( &pos );
                        if( rc == ERR_NO_ERR ) {
                            CurrentPos = pos;
                            CurrentPos.column -= 1;
                            CGimmeLinePtr( CurrentPos.line, &CurrentFcb, &CurrentLine );
                        }
                    }
                    rc = getBracketLoc( &pos );
                    RestoreCurrentFilePos();
                    if( rc == ERR_NO_ERR ) {
                        CGimmeLinePtr( pos.line, &cfcb, &cline );
                        i = FindStartOfALine( cline );
                    }
                    break;
                }
            }
        }

        /*
         * create the final indent amount
         */
        if( i == 1 ) {
            if( cline->len != 0 && !isspace( cline->data[0] ) ) {
                indent_amount = 1;
            } else {
                indent_amount = GetVirtualCursorPosition( cline->data, cline->len + 1 );
            }
        } else {
            indent_amount = GetVirtualCursorPosition( cline->data, i );
        }
        tabme = EditFlags.RealTabs;
        indent_amount += extra;
        j = InsertTabSpace( indent_amount - 1, buff, &tabme );
        break;
    }
    buff[j] = '\0';
    return( j );

} /* GetAutoIndentAmount */
