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
#include <time.h>
#include "expr.h"
#include "rxsupp.h"

#include "clibext.h"


/*
 * SrcAssign - assign a value to a variable
 */
vi_rc SrcAssign( const char *data, vlist *vl )
{
    int         i, j, k, l;
    long        val;
    bool        rxflag = false;
    bool        envflag = false;
    bool        setflag = false;
    bool        expflag = false;
    bool        timeflag = false;
    bool        lnumflag = false;
    char        tmp[MAX_SRC_LINE], tmp1[MAX_SRC_LINE], name[MAX_SRC_LINE];
    const char  *v1;
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
    data = GetNextWord1( data, tmp );
    if( *tmp == '\0' ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    if( !VarName( name, tmp, vl ) ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    data = GetNextWord1( data, tmp );
    if( *tmp == '\0' ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    if( stricmp( tmp, "=" ) != 0 ) {
        return( ERR_SRC_INVALID_ASSIGN );
    }
    data = SkipLeadingSpaces( data );

    if( data[0] == '/' || data[0] == '"' ) {
        check_end = false;
        if( data[0] == '"' ) {
            data = GetNextWord( data, tmp, SingleQuote );
            if( data[0] == '"' ) {
                check_end = true;
            }
        } else {
            data = GetNextWord( data, tmp, SingleSlash );
            if( data[0] == '/' ) {
                check_end = true;
            }
        }
        if( check_end ) {
            ++data;
            while( data[0] != '\0' ) {
                switch( data[0] ) {
                case 't':
                    timeflag = true;
                    break;
                case 'r':
                    rxflag = true;
                    break;
                case '$':
                    envflag = true;
                    break;
                case '@':
                    setflag = true;
                    break;
                case 'x':
                    expflag = true;
                    break;
                case 'l':
                    lnumflag = true;
                    break;
                }
                ++data;
            }
        }
        Expand( tmp1, tmp, vl );
    } else {
        data = GetNextWord1( data, tmp );
        if( *tmp == '\0' ) {
            return( ERR_SRC_INVALID_ASSIGN );
        }
        j = Tokenize( StrTokens, tmp, false );
        if( j != TOK_INVALID ) {
            data = GetNextWord1( data, tmp );
            if( *tmp == '\0' ) {
                return( ERR_SRC_INVALID_ASSIGN );
            }
            if( !VarName( tmp1, tmp, vl ) ) {
                return( ERR_SRC_INVALID_ASSIGN );
            }
            v = VarFind( tmp1, vl );
            switch( j ) {
            case STR_T_STRLEN:
                if( v != NULL ) {
                    sprintf( tmp1, "%d", v->len );
                } else {
                    strcpy( tmp1, "0" );
                }
                break;
            case STR_T_SUBSTR:
                data = GetNextWord1( data, tmp );
                if( *tmp == '\0' ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( tmp1, tmp, vl );
                i = atoi( tmp1 ) - 1;
                data = GetNextWord1( data, tmp );
                if( *tmp == '\0' ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( tmp1, tmp, vl );
                j = atoi( tmp1 ) - 1;
                if( v == NULL ) {
                    tmp1[0] = '\0';
                    break;
                }
                if( j >= v->len || i < 0 ) {
                    tmp1[0] = '\0';
                } else {
                    l = 0;
                    for( k = i; k <= j; k++ ) {
                        tmp1[l++] = v->value[k];
                    }
                    tmp1[l] = '\0';
                }
                break;
            case STR_T_STRCHR:
                data = GetNextWord1( data, tmp );
                if( *tmp == '\0' ) {
                    return( ERR_SRC_INVALID_ASSIGN );
                }
                Expand( tmp1, tmp, vl );
                if( v == NULL ) {
                    j = -1;
                } else {
                    t = strchr( v->value, tmp1[0] );
                    if( t != NULL ) {
                        j = t - v->value;
                    } else {
                        j = -1;
                    }
                    j++;
                }
                sprintf( tmp1, "%d", j );
                break;
            }
        } else {
            Expand( tmp1, tmp, vl );
        }

    }
    /*
     * regular expression subs.
     */
    if( rxflag && CurrentRegularExpression != NULL ) {
        RegSub( CurrentRegularExpression, tmp1, tmp, CurrentPos.line );
        strcpy( tmp1, tmp );
    }

    /*
     * special processing
     */
    if( envflag ) {
        v1 = getenv( tmp1 );
        if( v1 == NULL ) {
            v1 = "";
        }
    } else if( setflag ) {
        v1 = GetASetVal( tmp1 );
    } else if( timeflag ) {
        tod = time( NULL );
        strftime( tmp, sizeof( tmp ), tmp1, localtime( &tod ) );
        v1 = tmp;
    } else if( expflag || lnumflag ) {
        i = setjmp( jmpaddr );
        if( i != 0 ) {
            return( (vi_rc)i );
        }
        StartExprParse( tmp1, jmpaddr );
        val = GetConstExpr();
        if( lnumflag ) {
            fcb         *cfcb;
            line        *cline;
            vi_rc       rc;
            rc = CGimmeLinePtr( val, &cfcb, &cline );
            v1 = cline->data;
            if( rc != ERR_NO_ERR ) {
                v1 = "";
            }
        } else {
            v1 = ltoa( val, tmp1, EditVars.Radix );
        }
    } else {
        v1 = tmp1;
    }

    VarAddStr( name, v1, vl );
    return( ERR_NO_ERR );

} /* SrcAssign */
