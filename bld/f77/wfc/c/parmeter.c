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
* Description:  Process PARAMETER statement
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "opn.h"
#include "namecod.h"
#include "global.h"
#include "segsw.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "recog.h"
#include "insert.h"
#include "utility.h"
#include "convert.h"

#include <string.h>

extern  sym_id          LkSym(void);
extern  sym_id          STLit(byte *,int);
extern  sym_id          STConst(void *,TYPE,uint);
extern  void            CkSymDeclared(sym_id);


void    CpParameter( void ) {
//=====================

// Compile PARAMETER statement.
//
//     PARAMETER (P1=E1,...,Pn=En), n > 0

    uint        parm_size;
    byte        *lit;
    byte        *string;
    int         lit_len;
    sym_id      sym;
    sym_id      value_id;
    TYPE        typ;
    byte        assign_val;

    ReqNOpn();
    AdvanceITPtr();
    ReqOpenParen();
    for(;;) {
        if( ReqName( NAME_VARIABLE ) ) {
            sym = LkSym();
            typ = sym->ns.typ;
            assign_val = TRUE;
            if( sym->ns.flags & (SY_USAGE | SY_SUB_PARM | SY_IN_EC) ) {
                IllName( sym );
                assign_val = FALSE;
            } else if( typ == FT_STRUCTURE ) {
                IllType( sym );
                assign_val = FALSE;
            } else {
                CkSymDeclared( sym );
            }
            AdvanceITPtr();
            ReqEquSign();
            parm_size = sym->ns.xt.size;
            if( typ == FT_STRUCTURE ) {
                ConstExpr( FT_NO_TYPE );
            } else if( _IsTypeLogical( typ ) ) {
                CLogicExpr();
            } else if( typ == FT_CHAR ) {
                CCharExpr();
            } else {
                CArithExpr();
            }
            if( !AError && assign_val ) {
                if( typ == FT_CHAR ) {
                    string = (byte *)CITNode->value.cstring.strptr;
                    if( CITNode->size < parm_size ) {
                        lit = FMemAlloc( parm_size );
                        lit_len = CITNode->size;
                        memcpy( lit, string, lit_len );
                        memset( lit + lit_len, ' ', parm_size - lit_len );
                        value_id = STLit( lit, parm_size );
                        FMemFree( lit );
                    } else {
                        if( parm_size == 0 ) { // *(*)
                            parm_size = CITNode->size;
                        }
                        value_id = STLit( string, parm_size );
                    }
                } else {
                    if( !_IsTypeLogical( typ ) ) {
                        CnvTo( CITNode, typ, parm_size );
                    }
                    value_id = STConst( &CITNode->value, typ, parm_size );
                }
                sym->ns.flags |= SY_USAGE | SY_PARAMETER | SY_TYPE;
                sym->ns.xt.size = parm_size;
                sym->ns.si.pc.value = value_id;
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) break;
    }
    ReqCloseParen();
    if( ReqNOpn() ) {
        AdvanceITPtr();
        ReqEOS();
    }
}
