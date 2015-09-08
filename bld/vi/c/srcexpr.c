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
#include "source.h"
#include "expr.h"

#include "clibext.h"


/*
 * SrcExpr - add a value to a variable
 */
vi_rc SrcExpr( sfile *sf, vlist *vl )
{
    char        tmp[MAX_SRC_LINE], v1[MAX_SRC_LINE];
    long        val, oval;
    int         i;
    jmp_buf     jmpaddr;
    vars        *v;

    strcpy( tmp, sf->arg1 );
    strcpy( v1, sf->arg2 );
    if( !VarName( tmp, vl ) ) {
        return( ERR_SRC_INVALID_EXPR );
    }
    if( sf->hasvar ) {
        Expand( v1, vl  );
    }
    i = setjmp( jmpaddr );
    if( i != 0 ) {
        return( (vi_rc)i );
    }
    StartExprParse( v1, jmpaddr );
    val = GetConstExpr();

    if( sf->u.oper != EXPR_EQ ) {
        v = VarFind( tmp, vl );
        if( v != NULL ) {
            oval = strtol( v->value, NULL, 0 );
            switch( sf->u.oper ) {
            case EXPR_PLUSEQ:
                oval += val;
                break;
            case EXPR_MINUSEQ:
                oval -= val;
                break;
            case EXPR_DIVIDEEQ:
                oval /= val;
                break;
            case EXPR_TIMESEQ:
                oval *= val;
                break;
            }
            val = oval;
        }
    }

    VarAddStr( tmp, ltoa( val, v1, 10 ), vl );
    return( ERR_NO_ERR );

} /* SrcExpr */
