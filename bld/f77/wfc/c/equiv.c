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
* Description:  compile EQUIVALENCE statement
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "opr.h"
#include "errcod.h"
#include "segsw.h"
#include "namecod.h"
#include "fmemmgr.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "equiv.h"
#include "rstalloc.h"
#include "symtab.h"

#include <string.h>


static  bool    SubStr2( intstar4 *subscripts ) {
//===============================================

// Get the second component of a substring expression.

    bool        got_colon;
    bool        hi;

    subscripts++;
    got_colon = RecColon();
    hi = false;
    if( !RecNOpn() ) {
        CIntExpr();
        *subscripts = ITIntValue( CITNode );
        hi = true;
    }
    if( got_colon ) {
        AdvanceITPtr();
    }
    ReqCloseParen();
    ReqNOpn();
    AdvanceITPtr();
    return( hi );
}


void    CpEquivalence(void) {
//=======================

// Compile EQUIVALENCE statement.

//     EQUIVALENCE (A1,...,An) {,(B1,...,Bm)} . . .

    sym_id              sym;
    int                 num_equived;
    intstar4            *subscripts;
    int                 eq_size;
    act_eq_entry        *new_eq;
    act_eq_entry        *eqv_entry;
    act_eq_entry        *eq_head;
    act_eq_entry        *eq_set;
    bool                ill_name;
    bool                sub_strung;
    act_eq_entry        equiv;

    eq_set = EquivSets;
    if( EquivSets != NULL ) {
        while( eq_set->next_eq_set != NULL ) {
            eq_set = eq_set->next_eq_set;
        }
    }
    for( ;; ) {
        if( RecNOpn() ) {
            AdvanceITPtr();
        }
        ReqOpenParen();
        eqv_entry = NULL;
        eq_head = NULL;
        num_equived = 0;
        for( ;; ) {
            AError = false;
            if( ReqName( NAME_VAR_OR_ARR ) ) {
                num_equived++;
                sym = LkSym();
                ill_name = true;
                if( ( sym->u.ns.flags & SY_CLASS ) == SY_VARIABLE ) {
                    if( sym->u.ns.flags & SY_DATA_INIT ) {
                        NameErr( ST_DATA_ALREADY, sym );
                    } else if( sym->u.ns.flags & SY_SUB_PARM ) {
                        IllName( sym );
                    } else if( ( sym->u.ns.flags & SY_SUBSCRIPTED ) &&
                                _Allocatable( sym ) ) {
                        IllName( sym );
                    } else {
                        sym->u.ns.flags |= SY_IN_EQUIV;
                        ill_name = false;
                    }
                } else {
                    IllName( sym );
                }
                AdvanceITPtr();
                equiv.name_equived = sym;
                equiv.next_eq_entry = NULL;
                equiv.next_eq_set = NULL;
                equiv.subs_no = 0;
                equiv.substr = 0;
                equiv.substr1 = 1;
                equiv.substr2 = 0;
                subscripts = equiv.subscrs;
                if( RecOpenParen() ) {
                    if( !RecNOpn() || !RecNextOpr( OPR_COL ) ) {
                        sub_strung = false;
                        for( ;; ) {
                            CIntExpr();
                            *subscripts = ITIntValue( CITNode );
                            AdvanceITPtr();
                            if( RecColon() ) {
                                sub_strung = true;
                                break;
                            }
                            subscripts++;
                            equiv.subs_no++;
                            if( equiv.subs_no == MAX_DIM )
                                break;
                            if( !RecComma() ) {
                                break;
                            }
                        }
                        if( !sub_strung ) {
                            ReqCloseParen();
                            ReqNOpn();
                            AdvanceITPtr();
                            if( RecOpenParen() ) {
                                *subscripts = 1;
                                if( !RecNOpn() ) {
                                    CIntExpr();
                                    *subscripts = ITIntValue( CITNode );
                                }
                                AdvanceITPtr();
                                sub_strung = ReqColon();
                            }
                        }
                    } else {
                        sub_strung = true;
                    }
                    if( sub_strung ) {
                        equiv.substr = 1;
                        if( SubStr2( subscripts ) ) {
                            equiv.substr = 2;
                        }
                    }
                }
                if( AError ) {
                    equiv.subs_no = 0;
                    equiv.substr = 0;
                }
                if( ( ( SgmtSw & SG_SYMTAB_RESOLVED ) == 0 ) && !ill_name ) {
                    eq_size = sizeof( eq_entry ) +
                              equiv.subs_no * sizeof( intstar4 );
                    if( equiv.substr != 0 ) {
                        eq_size += 2 * sizeof( intstar4 );
                    }
                    new_eq = FMemAlloc( eq_size );
                    memcpy( new_eq, &equiv, eq_size );
                    if( eqv_entry == NULL ) {
                        eq_head = new_eq;
                        eqv_entry = new_eq;
                    } else {
                        eqv_entry->next_eq_entry = new_eq;
                        eqv_entry = new_eq;
                    }
                    if( sym->u.ns.si.va.vi.ec_ext == NULL ) {
                        sym->u.ns.si.va.vi.ec_ext = STComEq();
                    }
                }
            } else {
                AdvanceITPtr();
            }
            if( !RecComma() ) {
                break;
            }
        }
        if( num_equived < 2 ) {
            Error( EV_EQUIV_LIST );
        }
        if( eq_set == NULL ) {
            eq_set = eq_head;
            EquivSets = eq_head;
        } else {
            eq_set->next_eq_set = eq_head;
            eq_set = eq_head;
        }
        ReqCloseParen();
        ReqNOpn();
        AdvanceITPtr();
        if( !RecComma() ) {
            break;
        }
    }
    ReqEOS();
}


void    EqPurge(void) {
//=================

// Free up all of the equivalence information

    act_eq_entry    *eq_set;
    act_eq_entry    *old;

    while( EquivSets != NULL ) {
        eq_set = EquivSets;
        EquivSets = eq_set->next_eq_set;
        for( ;; ) {
            old = eq_set;
            eq_set = old->next_eq_entry;
            FMemFree( old );
            if( eq_set == NULL ) {
                break;
            }
        }
    }
}
