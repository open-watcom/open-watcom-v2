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
#include <setjmp.h>
#include "expr.h"
#include "rxsupp.h"


static int      bPos;
static char     *bPtr;
static int      recurseDepth;

/*
 * addChar - add a character to new expanded line
 */
static bool addChar( char ch )
{
    bPtr[bPos] = ch;
    bPos++;
    if( bPos >= MAX_SRC_LINE - 1 ) {
        bPtr[bPos] = 0;
        return( false );
    }
    return( true );

} /* addChar */

/*
 * Expand - expand local variables
 */
char *Expand( char *odata, const char *data, vlist *vl )
{
    char        result[MAX_SRC_LINE];
    char        varname[MAX_SRC_LINE];
    char        *ptr;
    int         paren_level;
    char        ch;
    bool        done;
    bool        has_var;
    bool        need_closebracket;
    vars        *v;
    char        *obptr;
    int         obpos;

    if( recurseDepth >= 3 ) {
        if( odata != data ) {
            strcpy( odata, data );
        }
    } else {
        obpos = bPos;
        obptr = bPtr;
        recurseDepth++;
        bPos = 0;
        bPtr = result;

        for( ;; ) {
            ch = *data++;
            if( ch == '%' ) {
                if( *data == '%' ) {
                    if( !addChar( '%' ) ) {
                        break;
                    }
                    data++;
                    continue;
                }
                has_var = false;
                if( *data != '(' ) {
                    varname[0] = *data++;
                    varname[1] = 0;
                } else {
                    data++;
                    paren_level = 1;
                    ptr = varname;
                    while( (ch = *data++) != '\0' ) {
                        if( ch == '%' ) {
                            has_var = true;
                        } else if( ch == '(' ) {
                            paren_level++;
                        } else if( ch == ')' ) {
                            paren_level--;
                            if( paren_level == 0 ) {
                                break;
                            }
                        }
                        *ptr++ = ch;
                    }
                    *ptr = 0;
                }
                if( has_var ) {
                    Expand( varname, varname, vl );
                }

                v = VarFind( varname, vl );
                if( v != NULL ) {
                    ptr = v->value;
                    if( EditFlags.CompileScript && !EditFlags.CompileAssignmentsDammit ) {
                        if( varname[0] >= 'A' && varname[0] <= 'Z' ) {
                            ptr = varname;
                        }
                    }
                } else {
                    ptr = varname;
                }
                done = false;
                need_closebracket = false;
                if( ptr == varname ) {
                    if( !addChar( '%' ) ) {
                        done = true;
                    } else if( varname[1] != 0 ) {
                        if( !addChar( '(' ) ) {
                            done = true;
                        } else {
                            need_closebracket = true;
                        }
                    }
                }

                while( !done && (ch = *ptr++) != '\0' ) {
                    if( !addChar( ch ) ) {
                        done = true;
                        break;
                    }
                }
                if( done ) {
                    break;
                }
                if( need_closebracket ) {
                    if( !addChar( ')' ) ) {
                        done = true;
                        break;
                    }
                }
            } else {
                if( !addChar( ch ) ) {
                    break;
                }
                if( ch == 0 ) {
                    break;
                }
            }
        }
        memcpy( odata, result, bPos );
        recurseDepth--;
        bPos = obpos;
        bPtr = obptr;
    }
    return( odata );

} /* Expand */
