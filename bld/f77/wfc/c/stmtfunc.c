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
* Description:  process statement functions
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "namecod.h"
#include "global.h"
#include "stmtsw.h"
#include "fmemmgr.h"
#include "recog.h"
#include "ferror.h"
#include "utility.h"

extern  void            BIEndSF( sym_id ste_ptr );
extern  void            GStartSF(void);
extern  void            GEndSF(void);
extern  sym_id          LkSym(void);
extern  sym_id          STShadow(sym_id);
extern  void            STUnShadow(sym_id);
extern  void            CkTypeDeclared(void);


void    SFPrologue( void ) {
//====================

// Generate code for statement function prologue.

    sym_id      sym;
    itnode      *func_node;
    itnode      *arg_list;
    sf_parm     **parm;

    StmtSw |= SS_SF_REFERENCED;
    CkTypeDeclared();
    SFSymId = CITNode->sym_ptr;
    if( ( SFSymId->ns.u1.s.typ == FT_CHAR ) && ( SFSymId->ns.xt.size == 0 ) ) {
        Error( SF_ILL_CHAR_LEN );
    } else if( SFSymId->ns.u1.s.typ == FT_STRUCTURE ) {
        Error( SF_ILL_TYPE );
    }
    GStartSF();
    SFSymId->ns.flags = SY_USAGE | SY_TYPE | SY_SUBPROGRAM | SY_STMT_FUNC;
    CITNode->flags = SFSymId->ns.flags;
    func_node = CITNode;
    AdvanceITPtr();
    ReqOpenParen();
    SFSymId->ns.si.sf.header = FMemAlloc( sizeof( sf_header ) );
    SFSymId->ns.si.sf.header->ref_count = 1;
    parm = &SFSymId->ns.si.sf.header->parm_list;
    *parm = NULL;
    if( RecNOpn() ) {
        AdvanceITPtr();
    } else {
        for(;;) {
            if( ReqName( NAME_SF_DUMMY ) ) {
                sym = LkSym();
                sym->ns.u1.s.xflags |= SY_DEFINED;
                CkTypeDeclared();
                if( ( ( sym->ns.flags & SY_CLASS ) == SY_VARIABLE ) &&
                    ( ( sym->ns.flags & SY_SUBSCRIPTED ) == 0 ) &&
                    ( ( sym->ns.u1.s.typ != FT_CHAR ) ||
                      ( sym->ns.xt.size != 0 ) ) &&
                    ( sym->ns.u1.s.typ != FT_STRUCTURE ) ) {
                    if( sym->ns.flags & SY_SPECIAL_PARM ) {
                        Error( SF_DUPLICATE_DUMMY_PARM );
                    } else {
                        *parm = FMemAlloc( sizeof( sf_parm ) );
                        (*parm)->link = NULL;
                        (*parm)->actual = sym;
                        (*parm)->shadow = STShadow( sym );
                        parm = &((*parm)->link);
                    }
                } else {
                    Error( SF_ILL_DUMMY_PARM );
                }
            }
            AdvanceITPtr();
            if( !RecComma() ) break;
        }
    }
    ReqCloseParen();
    ReqNOpn();
    arg_list = func_node->link;
    func_node->link = CITNode->link;
    CITNode->link = NULL;
    CITNode = func_node;
    FreeITNodes( arg_list );
}


void    SFEpilogue( void ) {
//====================

// Generate code for statement function epilogue.

    sf_parm     *parm;

    GEndSF();
    parm = SFSymId->ns.si.sf.header->parm_list;
    while( parm != NULL ) {
        if( parm->shadow->ns.flags & SY_REFERENCED ) {
            parm->actual->ns.flags |= SY_REFERENCED;
        }
        STUnShadow( parm->actual );
        parm = parm->link;
    }
    BIEndSF( SFSymId ); // end statement function browsing information
}
