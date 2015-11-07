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
    if( bPos >= MAX_SRC_LINE - 1 )
        return( false );
    bPtr[bPos++] = ch;
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

        for( ; (ch = *data++) != '\0'; ) {
            if( ch == '%' ) {
                ch = *data++;
                if( ch == '\0' )
                    break;
                if( ch == '%' ) {
                    if( !addChar( '%' ) )
                        break;
                    continue;
                }
                has_var = false;
                if( ch != '(' ) {
                    varname[0] = ch;
                    varname[1] = '\0';
                } else {
                    paren_level = 1;
                    ptr = varname;
                    while( (ch = *data) != '\0' ) {
                        data++;
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
                    *ptr = '\0';
                    if( has_var ) {
                        Expand( varname, varname, vl );
                    }
                }

                v = VarFind( varname, vl );
                done = false;
                if( v != NULL && ( !EditFlags.CompileScript || EditFlags.CompileAssignmentsDammit || varname[0] < 'A' || varname[0] > 'Z') ) {
                    // output variable value
                    ptr = v->value;
                    while( (ch = *ptr++) != '\0' ) {
                        if( !addChar( ch ) ) {
                            done = true;
                            break;
                        }
                    }
                    if( done ) {
                        break;
                    }
                } else {
                    // output variable name
                    if( !addChar( '%' ) )
                        break;
                    ptr = varname;
                    if( ptr[1] == '\0' ) {
                        // single character variable name
                        if( !addChar( *ptr ) ) {
                            break;
                        }
                    } else {
                        if( !addChar( '(' ) )
                            break;
                        while( (ch = *ptr++) != '\0' ) {
                            if( !addChar( ch ) ) {
                                done = true;
                                break;
                            }
                        }
                        if( done ) {
                            break;
                        }
                        if( !addChar( ')' ) ) {
                            break;
                        }
                    }
                }
            } else {
                if( !addChar( ch ) ) {
                    break;
                }
            }
        }
        bPtr[bPos++] = '\0';
        memcpy( odata, result, bPos );
        recurseDepth--;
        bPos = obpos;
        bPtr = obptr;
    }
    return( odata );

} /* Expand */
