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
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#include "vi.h"
#include "source.h"
#include "expr.h"
#include "rxsupp.h"

/*
 * SrcAssign - assign a value to a variable
 */
int SrcAssign( char *data, vlist *vl )
{
    int         i,j,k,l,rc;
    long        val;
    bool        rxflag=FALSE,envflag=FALSE,setflag=FALSE,expflag=FALSE;
    bool        timeflag=FALSE,lnumflag=FALSE;
    char        tmp[MAX_SRC_LINE],v1[MAX_SRC_LINE],name[MAX_SRC_LINE];
    char        *t;
    vars        *v;
    jmp_buf     jmpaddr;
    time_t      tod;
    bool        check_end;

    /*
     * get assign syntax :
     * ASSIGN %v = /v1/(r)($)(@)(x)(l)(t)
     * possible v1:
     *  strlen %a
     *  strchr %a ch
     *  substr %a n1 n2
     */
    if( NextWord1( data, name ) <= 0 ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    if( !VarName( name, vl ) ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    if( NextWord1( data, tmp ) <= 0 ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    if( stricmp( tmp, "=" ) ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    RemoveLeadingSpaces( data );

    if( data[0] == '/' || data[0] == '"' ) {
        check_end = FALSE;
        if( data[0] == '"' ) {
            NextWord( data, v1, "\"" );
            if( data[0] == '"' ) {
                check_end = TRUE;
            }
        } else {
            NextWordSlash( data, v1 );
            if( data[0] == '/' ) {
                check_end = TRUE;
            }
        }
        if( check_end ) {
            EliminateFirstN( data,1 );
            while( data[0] != 0 ) {
                switch( data[0] ) {
                case 't':
                    timeflag = TRUE;
                    break;
                case 'r':
                    rxflag = TRUE;
                    break;
                case '$':
                    envflag = TRUE;
                    break;
                case '@':
                    setflag = TRUE;
                    break;
                case 'x':
                    expflag = TRUE;
                    break;
                case 'l':
                    lnumflag = TRUE;
                    break;
                }
                EliminateFirstN( data,1 );
            }
        }
        Expand( v1, vl );
    } else {
        if( NextWord1( data, v1 ) <= 0 ) {
            return( ERR_SRC_INVALID_ASSIGN );
        }
        j = Tokenize( StrTokens, v1, FALSE );
        if( j>=0 ) {
            if( NextWord1( data, v1 ) <= 0 ) {
                return( ERR_SRC_INVALID_ASSIGN );
            }
            if( !VarName( v1, vl ) ) {
                return( ERR_SRC_INVALID_ASSIGN );
            }
            v = VarFind( v1, vl );
            switch( j ) {
            case STR_T_STRLEN:
                if( v != NULL ) {
                    j = v->len;
                } else {
                    j = 0;
                }
                itoa( j,v1, 10 );
                break;
            case STR_T_SUBSTR:
                if( NextWord1( data, v1 ) <= 0 ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( v1, vl );
                i = atoi( v1 )-1;
                if( NextWord1( data, v1 ) <= 0 ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( v1, vl );
                j = atoi( v1 )-1;
                if( v == NULL ) {
                    v1[0] = 0;
                    break;
                }
                k = v->len;
                if( j >= k || i < 0 ) {
                    v1[0] =0;
                } else {
                    l = 0;
                    for( k=i;k<=j;k++ ) {
                        v1[l++] = v->value[k];
                    }
                    v1[l] = 0;
                }
                break;
            case STR_T_STRCHR:
                if( NextWord1( data, v1 ) <= 0 ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( v1, vl );
                if( v == NULL ) {
                    j = -1;
                } else {
                    t = strchr( v->value, v1[0] );
                    if( t != NULL ) {
                        j = t - v->value;
                    } else {
                        j = -1;
                    }
                    j++;
                }
                itoa( j,v1, 10 );
                break;
            }
        } else {
            Expand( v1, vl );
        }

    }
    /*
     * regular expression subs.
     */
    if( rxflag && CurrentRegularExpression != NULL ) {
        RegSub( CurrentRegularExpression, v1, tmp, CurrentLineNumber );
        strcpy( v1, tmp );
    }

    /*
     * special processing
     */
    if( envflag ) {
        t = getenv( v1 );
        if( t != NULL ) {
            strcpy( v1, t );
        } else {
            v1[0] = 0;
        }
    } else if( setflag ) {
        strcpy( v1, GetASetVal( v1 ) );
    } else if( timeflag ) {
        tod = time( NULL );
        strftime( tmp, sizeof( tmp ), v1, localtime( &tod ) );
        strcpy( v1, tmp );
    } else if( expflag || lnumflag ) {

        rc = setjmp( jmpaddr );
        if( rc == 0 ) {
            StartExprParse( v1, jmpaddr );
            val = GetConstExpr();
        } else {
            return( rc );
        }
        if( lnumflag ) {
            fcb         *cfcb;
            line        *cline;
            rc = CGimmeLinePtr( val, &cfcb, &cline );
            if( rc ) {
                VarAdd( name, "", vl );
            } else {
                VarAdd( name, cline->data, vl );
            }
            return( ERR_NO_ERR );
        } else {
            ltoa( val, v1, Radix );
        }
    }

    VarAdd( name, v1, vl );
    return( ERR_NO_ERR );

} /* SrcAssign */
