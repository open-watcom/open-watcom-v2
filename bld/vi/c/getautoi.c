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
#include <ctype.h>
#include "vi.h"

/*
 * getBracketLoc - find a matching '(' for a ')'
 */
static int getBracketLoc( linenum *mline, int *mcol )
{
    int         rc;
    char        tmp[3];
    int         len;
    linenum     lne;
    bool        oldmagic = EditFlags.Magic;
    bool        oldnrss = EditFlags.NoReplaceSearchString;

    EditFlags.NoReplaceSearchString = TRUE;
    EditFlags.Magic = TRUE;
    tmp[0] = '\\';
    tmp[1] = ')';
    tmp[2] = 0;
    lne = CurrentLineNumber;
    rc = GetFind( tmp, mline, mcol, &len, FINDFL_BACKWARDS|FINDFL_NOERROR);
    EditFlags.NoReplaceSearchString = oldnrss;
    if( *mline != CurrentLineNumber ) {
        EditFlags.Magic = oldmagic;
        return( ERR_FIND_NOT_FOUND );
    }
    if( rc ) {
        EditFlags.Magic = oldmagic;
        return( rc );
    }

    /*
     * find the matching '('
     */
    CurrentLineNumber = *mline;
    CurrentColumn = *mcol;
    CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
    rc = FindMatch( mline, mcol );
    EditFlags.Magic = oldmagic;
    return( rc );

} /* getBracketLoc */

/*
 * GetAutoIndentAmount - do just that
 */
int GetAutoIndentAmount( char *buff, int extra, bool above_line )
{
    int         i,j=0,k;
    bool        tabme;
    int         rc,col;
    linenum     sline;
    line        *cline;
    fcb         *cfcb;
    char        ch;
    int         indent_amount;

    while( EditFlags.AutoIndent ) {
        i = FindStartOfCurrentLine();
        cline = CurrentLine;
        /*
         * find extra indentation due to cmode
         */
        if( EditFlags.CMode ) {
            if( EditFlags.CaseShift ) {
            }
            for( k=cline->len-1;k>=0;k-- ) {
                ch = cline->data[k];
                if( ch == '{' || ch == '}' ) {
                    if( ch == '{' && above_line ) {
                        break;
                    }
                    if( ch == '}' && !above_line ) {
                        break;
                    }
                    extra += ShiftWidth;
                    SaveCurrentFilePos();
                    CurrentColumn = k+1;
                    /* add a { to keep matches even! */
                    if( ch == '}' ) {
                        rc = FindMatch( &sline, &col );
                        if( !rc ) {
                            CurrentLineNumber = sline;
                            CurrentColumn = col-1;
                            CGimmeLinePtr( CurrentLineNumber, &CurrentFcb, &CurrentLine );
                        }
                    }
                    rc = getBracketLoc( &sline, &col );
                    RestoreCurrentFilePos();
                    if( !rc ) {
                        CGimmeLinePtr( sline, &cfcb, &cline );
                        i = FindStartOfALine( cline );
                    }
                    break;
                }
            }
        }

        /*
         * create the final indent amount
         */
        if( i==1 ) {
            if( cline->len != 0 && !isspace( cline->data[0] )) {
                indent_amount = 1;
            } else {
                indent_amount = GetVirtualCursorPosition( cline->data, cline->len + 1 );
            }
        } else {
            indent_amount = GetVirtualCursorPosition( cline->data, i );
        }
        tabme = EditFlags.RealTabs;
        indent_amount += extra;
        j = InsertTabSpace( indent_amount-1, buff, &tabme );
        break;
    }
    buff[j]=0;
    return( j );

} /* GetAutoIndentAmount */
