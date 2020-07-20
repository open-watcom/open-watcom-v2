/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"


/*
 * SrcExpr - add a value to a variable
 */
vi_rc SrcExpr( sfile *sf, vars_list *vl )
{
    char        name[MAX_SRC_LINE], tmp[MAX_SRC_LINE];
    const char  *v1;
    long        val, oval;
    int         i;
    jmp_buf     jmpaddr;
    vars        *v;

    if( !VarName( name, sf->arg1, vl ) ) {
        return( ERR_SRC_INVALID_EXPR );
    }
    v1 = sf->arg2;
    if( sf->hasvar ) {
        v1 = Expand( tmp, v1, vl );
    }
    i = setjmp( jmpaddr );
    if( i != 0 ) {
        return( (vi_rc)i );
    }
    StartExprParse( v1, jmpaddr );
    val = GetConstExpr();

    if( sf->u.oper != EXPR_EQ ) {
        v = VarFind( name, vl );
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

    VarAddLong( name, val, vl );
    return( ERR_NO_ERR );

} /* SrcExpr */
