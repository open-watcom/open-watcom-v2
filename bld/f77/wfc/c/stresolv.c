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
* Description:  Common/Equivalence resolution
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "ecflags.h"
#include "errcod.h"
#include "progsw.h"
#include "fmemmgr.h"
#include "insert.h"

extern  void            STComDump(void);
extern  bool            DoSubstring(intstar4,intstar4,int);
extern  bool            DoSubscript(act_dim_list *,intstar4 *,intstar4 *);
extern  void            StructResolve(void);
extern  void            BIFiniStartOfSubroutine( void );
extern  bool            ForceStatic( unsigned_16 );

/* Forward declarations */
static  void    SetHigh( sym_id sym );
static  void    GenEquivSet( act_eq_entry *a, act_eq_entry *b,
                             intstar4 a_offset, intstar4 b_offset );
void    EquivResolve( void );
                             

void    STResolve( void ) {
//===================

// Resolve equivalence relations and dump common blocks.
// They must be done in the given order.

    ProgSw |= PS_SYMTAB_PROCESS;
    // must resolve structures first - size of structures gets computed and
    // is needed in case a structured symbol is in common
    StructResolve();
    EquivResolve();
    BIFiniStartOfSubroutine();
    STComDump();
    ProgSw &= ~PS_SYMTAB_PROCESS;
}


static  intstar4        CheckSubscr( act_eq_entry *eqv_entry ) {
//==============================================================

// Check that array is properly subscripted.

    sym_id              sym;
    act_dim_list        *dims;
    int                 dims_no;
    intstar4            offset;

    sym = eqv_entry->name_equived;
    dims_no = 0;
    dims = NULL;
    if( sym->ns.flags & SY_SUBSCRIPTED ) {
        dims = sym->ns.si.va.u.dim_ext;
        dims_no = _DimCount( dims->dim_flags );
        dims->dim_flags &= ~DIM_PVD;
    }
    if( eqv_entry->subs_no == 0 ) {
        offset = 0;
    } else if( dims_no != eqv_entry->subs_no ) {
        if( eqv_entry->subs_no == 1 ) {
            offset = eqv_entry->subscrs[0] - 1;
        } else {
            offset = 0;
            NameStmtErr( EV_SSCR_INVALID, sym, PR_EQUIV );
        }
    } else if( !DoSubscript( dims, eqv_entry->subscrs, &offset ) ) {
        offset = 0;
        NameStmtErr( EV_SSCR_INVALID, sym, PR_EQUIV );
    }
    return( offset * _SymSize( sym ) );
}


static  intstar4        CheckSubStr( act_eq_entry *eqv_entry ) {
//==============================================================

// Check for a valid substring operation.

    sym_id      sym;
    intstar4    *substr;
    intstar4    offset;
    intstar4    last;

    sym = eqv_entry->name_equived;
    substr = &eqv_entry->subscrs[ eqv_entry->subs_no ];
    if( sym->ns.u1.s.typ != FT_CHAR ) {
        NameTypeErr( EV_ONLY_IF_CHAR, sym );
        offset = 0;
    } else {
        offset = substr[0];
        if( eqv_entry->substr == 1 ) {
            last = sym->ns.xt.size;
        } else {
            last = substr[1];
        }
        if( DoSubstring( offset, last, sym->ns.xt.size ) ) {
            offset--;
        } else {
            NameStmtErr( EV_SSTR_INVALID, sym, PR_EQUIV );
            offset = 0;
        }
    }
    return( offset );
}


void    EquivResolve( void ) {
//======================

// Resolve equivalence relations.

    act_eq_entry    *eq_set;
    act_eq_entry    *eq_head;
    act_eq_entry    *eqv_entry;
    act_eq_entry    *next_eq_entry;
    intstar4        offset;
    intstar4        lead_offset;

    eq_set = EquivSets;
    while( eq_set != NULL ) {
        eq_head = eq_set;
        lead_offset = CheckSubscr( eq_head );
        if( eq_head->substr != 0 ) {
            lead_offset += CheckSubStr( eq_head );
        }
        eqv_entry = eq_head->next_eq_entry;
        // in case of an error, we may only have one member in an
        // equivalence set - make him a leader since every equivalence
        // set requires one
        if( eqv_entry == NULL ) {
            eq_head->name_equived->ns.si.va.vi.ec_ext->ec_flags |= LEADER;
        } else {
            for(;;) {
                offset = CheckSubscr( eqv_entry );
                if( eqv_entry->substr != 0 ) {
                    offset += CheckSubStr( eqv_entry );
                }
                GenEquivSet( eq_head, eqv_entry, lead_offset, offset );
                next_eq_entry = eqv_entry->next_eq_entry;
                FMemFree( eqv_entry );
                eqv_entry = next_eq_entry;
                if( eqv_entry == NULL ) break;
            }
        }
        eq_set = eq_head->next_eq_set;
        FMemFree( eq_head );
    }
    EquivSets = NULL;
}


static  byte    ClassifyType( TYPE sym_type ) {
//=============================================

// Classify the type of the specified symbol.

    byte        typ;

    typ = ES_NOT_CHAR;
    if( sym_type == FT_CHAR ) {
        typ = ES_CHAR;
    }
    return( typ );
}


static  void    GenEquivSet( act_eq_entry *a, act_eq_entry *b,
                             intstar4 a_offset, intstar4 b_offset ) {
//===================================================================

// Merge the specified equivalence relation to the corresponding
// equivalence set.

//     GenEquivSet( a, b, a_offset, b_offset )
//         for EQUIVALENCE ( a(a_offset), b(b_offset) )


    sym_id      a_name;
    sym_id      b_name;
    sym_id      p;
    sym_id      q;
    intstar4    c;
    intstar4    d;
    intstar4    dist;
    intstar4    low;
    intstar4    high;
    com_eq      *p_ext;
    com_eq      *q_ext;
    bool        p_in_common;
    bool        q_in_common;
    byte        p_type;
    byte        q_type;

    /* if an entry is marked static, then b must be too */    
    if( ForceStatic( a->name_equived->ns.flags ) ) {
        unsigned_16     sym_flags = a->name_equived->ns.flags & ( SY_DATA_INIT | SY_SAVED );
        b->name_equived->ns.flags |= sym_flags;
    } else if( ForceStatic( b->name_equived->ns.flags ) ) {
        unsigned_16     sym_flags = b->name_equived->ns.flags & ( SY_DATA_INIT | SY_SAVED );
        a->name_equived->ns.flags |= sym_flags;
    }

    a_name = a->name_equived;
    b_name = b->name_equived;
    p = a_name;
    q = b_name;
    c = 0;
    d = 0;
    dist = b_offset - a_offset;
    for(;;) {   // find leader of B
        q_ext = q->ns.si.va.vi.ec_ext;
        if( ( q_ext->ec_flags & IN_EQUIV_SET ) == 0 ) {
            SetHigh( q );
            break;
        }
        if( q_ext->ec_flags & LEADER ) break;
        d += q_ext->offset;
        q = q_ext->link_eqv;
    }
    q_in_common = ( q->ns.flags & SY_IN_COMMON ) ||
                  ( q_ext->ec_flags & MEMBER_IN_COMMON );
    q_type = q_ext->ec_flags & ES_TYPE;
    if( q_type == ES_NO_TYPE ) {
        q_type = ClassifyType( q->ns.u1.s.typ );
    }
    for(;;) {   // find leader of A
        p_ext = p->ns.si.va.vi.ec_ext;
        if( ( p_ext->ec_flags & IN_EQUIV_SET ) == 0 ) {
            SetHigh( p );
            break;
        }
        if( p_ext->ec_flags & LEADER ) break;
        c += p_ext->offset;
        p = p_ext->link_eqv;
    }
    p_in_common = ( p->ns.flags & SY_IN_COMMON ) ||
                  ( p_ext->ec_flags & MEMBER_IN_COMMON );
    p_type = p_ext->ec_flags & ES_TYPE;
    if( p_type == ES_NO_TYPE ) {
        p_type = ClassifyType( p->ns.u1.s.typ );
    }
    if( p == q ) {
        if( c - d != dist ) {
            // name equivalenced to 2 different relative positions
            NameErr( EV_DIFF_REL_POS, a_name );
        }
        // Consider:    EQUIVALENCE (A,A)
        // Every equivalence set must have a leader
        q_ext->ec_flags |= LEADER;
    } else {
        if( q_in_common && p_in_common ) {
            if( q_ext->com_blk != p_ext->com_blk ) {
                // 2 names in common equivalenced
                NamNamErr( EC_2NAM_EC, a_name, b_name );
                if( !( a_name->ns.si.va.vi.ec_ext->ec_flags & IN_EQUIV_SET ) ) {
                    a_name->ns.flags &= ~SY_IN_EQUIV;
                }
                if( !( b_name->ns.si.va.vi.ec_ext->ec_flags & IN_EQUIV_SET ) ) {
                    b_name->ns.flags &= ~SY_IN_EQUIV;
                }
                return;
            }
        }
        p_ext->ec_flags |= IN_EQUIV_SET;
        p_ext->ec_flags &= ~LEADER;
        p_ext->link_eqv = q;
        p_ext->offset = d - c + dist;
        if( q_type != p_type ) {
            q_ext->ec_flags |= ES_MIXED;
        }
        if( q_in_common ) {
            q_ext->ec_flags |= MEMBER_IN_COMMON;
        } else if( p_in_common ) {
            q_ext->com_blk = p_ext->com_blk;
            q_ext->ec_flags |= MEMBER_IN_COMMON;
        }
        if( ( q->ns.flags & SY_DATA_INIT ) ||
            ( p->ns.flags & SY_DATA_INIT ) ||
            ( p_ext->ec_flags & MEMBER_INITIALIZED ) ) {
            // This used to be set by VarList in DATA, but there was
            // a problem with the following:
            //          equivalence (i,j)
            //          integer i/19/
            // since STResolve hadn't been done, the equiv set for i
            // didn't have a LEADER.  So instead it is set here.
            q_ext->ec_flags |= MEMBER_INITIALIZED;
        }
        q_ext->ec_flags |= ( IN_EQUIV_SET | LEADER );
        low = p_ext->low + p_ext->offset;
        if( q_ext->low > low ) {
            q_ext->low = low;
        }
        high = p_ext->high + p_ext->offset;
        if( q_ext->high < high ) {
            q_ext->high = high;
        }
    }
}


static  void    SetHigh( sym_id sym ) {
//=====================================

// Set the high extent of a symbol which hasn't been put in an equivalence
// set.

    sym->ns.si.va.vi.ec_ext->high = _SymSize( sym );
    if( sym->ns.flags & SY_SUBSCRIPTED ) {
       sym->ns.si.va.vi.ec_ext->high *= sym->ns.si.va.u.dim_ext->num_elts;
    }
}
