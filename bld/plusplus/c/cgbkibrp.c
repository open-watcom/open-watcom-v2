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


#include "plusplus.h"

#include <float.h>

#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "initdefs.h"
#include "dumpapi.h"


typedef struct ibrp IBRP;

struct ibrp                     // IBRP -- inline bound reference parameters
{   IBRP *next;                 // - next in ring
    cg_name handle;             // - handle of called function
    SYMBOL func;                // - inline function called
    SYMBOL refed;               // - referenced symbol (caller)
    target_size_t offset;       // - offset added to reference arg
    FN_CTL *source;             // - handle of generator of IBRPs
    union {
        unsigned index;         // - argument # (before inlining)
        SYMBOL parm;            // - symbol (after IC_FUNCTION_ARGS)
    } u;
};


static carve_t carveIBRP;           // allocations for IBRPs
static IBRP *ibrps;                 // ring of IBRPs scheduled
static unsigned parm_no;            // parm # being defined


#ifdef NDEBUG
    #define dump_ibrp( ibrp, text )
#else
    #include "dbg.h"
    #include "pragdefn.h"

    static void prt_ibrp(           // PRINT IBRP ENTRY
        IBRP* ibrp,                 // - entry
        const char *text )          // - text string
    {
        printf ( "[%p]%s\n    hdl=%p func=%p refed=%p off=%x parm=%p\n"
               , ibrp
               , text
               , ibrp->handle
               , ibrp->func
               , ibrp->refed
               , ibrp->offset
               , ibrp->u.parm );
    }

    static void dump_ibrp(          // DUMP IBRP ENTRY
        IBRP* ibrp,                 // - entry
        const char *text )          // - text string
    {
        if( PragDbgToggle.dump_exec_ic ) {
            prt_ibrp( ibrp, text );
        }
    }

    bool IbpEmpty(                  // DEBUG -- verify empty
        void )
    {
        return ibrps == NULL;
    }

    void IbpDump( void )            // DEBUG -- dump all entries
    {
        if( IbpEmpty() ) {
            printf( "IBRP -- no bound references\n" );
        } else {
            IBRP* curr;
            RingIterBeg( ibrps, curr ) {
                prt_ibrp( curr, "IBRP" );
            } RingIterEnd( curr )
        }
    }
#endif


static void ibpRemove(          // REMOVE AN IBRP ENTRY
    IBRP *ibrp )                // - item to be removed
{
    dump_ibrp( ibrp, "IBRP(popped)" );
    RingPrune( &ibrps, ibrp );
    CarveFree( carveIBRP, ibrp );
}


void IbpFlush(                  // REMOVE ALL IBRP ENTRIES FOR THIS CALL CONTEXT
    FN_CTL* fctl )              // - current file control
{
    IBRP *ibrp;                 // - current reference

    RingIterBegSafe( ibrps, ibrp ) {
        if( ibrp->source == fctl ) {
            ibpRemove( ibrp );
        }
    } RingIterEndSafe( ibrp )
}


void IbpAdd(                    // ADD AN IBRP ENTRY
    SYMBOL binding,             // - symbol to bind reference to
    target_offset_t offset,     // - offset into symbol
    FN_CTL* fctl )              // - current file information
{
    IBRP *ibrp;                 // - new entry
    SYMBOL trans;               // - translated symbol
    SYMBOL bound;               // - bound reference
    target_offset_t bound_off;  // - bound offset

    if( SymIsArgument( binding ) ) {
        IbpReference( binding, &trans, &bound, &bound_off );
        trans = bound;
        offset += bound_off;
    } else {
        trans = SymTrans( binding );
    }
    ibrp = RingCarveAlloc( carveIBRP, &ibrps );
    ibrp->handle = CallStackTopHandle();
    ibrp->func = CallStackTopFunction();
    ibrp->refed = trans;
    ibrp->offset = offset;
    ibrp->source = fctl;
    ibrp->u.index = 0;
    dump_ibrp( ibrp, "IBRP(created)" );
}


void IbpDefineSym(              // DEFINE SYMBOL FOR BOUND PARAMETER
    call_handle handle,         // - handle for call
    SYMBOL sym )                // - the symbol
{
    IBRP *ibrp;                 // - current inline bound reference parameter

    if( handle != NULL ) {
        RingIterBeg( ibrps, ibrp ) {
            if( ( handle == ibrp->handle )
              &&( parm_no == ibrp->u.index ) ) {
                ibrp->u.parm = sym;
                dump_ibrp( ibrp, "IBRP(defined parm)" );
                break;
            }
        } RingIterEnd( ibrp )
        ++parm_no;
    }
}


void IbpDefineOffset(           // DEFINE OFFSET FOR BOUND REFERENCE PARAMETER
    target_offset_t offset )    // - the offset
{
    IBRP *ibrp;                 // - current IBRP

    ibrp = ibrps;
    if( ibrp->refed != NULL ) {
        ibrp->offset += offset;
        dump_ibrp( ibrp, "IBRP(defined offset)" );
    }
}


void IbpDefineParms(            // START DEFINING PARAMETERS
    void )
{
    parm_no = 0;
}


void IbpDefineIndex(            // DEFINE CURRENT PARAMETER INDEX
    unsigned index )            // - index
{
    IBRP *ibrp;                 // - current IBRP

    ibrp = ibrps;
    if( ibrp->refed == NULL ) {
        ibpRemove( ibrp );
    } else {
        ibrp->u.index = index;
        dump_ibrp( ibrp, "IBRP(defined index)" );
    }
}


bool IbpReference(              // LOCATE A BOUND REFERENCE
    SYMBOL sym,                 // - original symbol
    SYMBOL *trans,              // - addr[ translated symbol ]
    SYMBOL *bound,              // - addr[ bound reference ]
    target_offset_t *offset )   // - addr[ offset from bound reference ]
{
    IBRP *ibrp;                 // - current inline bound reference parm.
    bool retn;                  // - TRUE ==> bound was located
    FN_CTL* fctl;               // - current file control

    fctl = FnCtlTop();
    if( sym == NULL ) {
        sym = fctl->this_sym;
    } else {
        sym = SymTrans( sym );
    }
    *trans = sym;
    *bound = NULL;
    *offset = 0;
    retn = FALSE;
    RingIterBeg( ibrps, ibrp ) {
        if( ( sym == ibrp->u.parm )
          &&( fctl->handle == ibrp->handle ) ) {
            *bound = ibrp->refed;
            *offset = ibrp->offset;
            dump_ibrp( ibrp, "IBRP(used)" );
#if 0
            if( PragDbgToggle.dump_exec_ic ) {
                printf( "ibrp->func: " );
                DumpSymbol( ibrp->func );
                printf( "ibrp->refed: " );
                DumpSymbol( ibrp->refed );
                printf( "ibrp->u.parm: " );
                DumpSymbol( ibrp->u.parm );
            }
#endif
            retn = TRUE;
            break;
        }
    } RingIterEnd( ibrp )
    return retn;
}


static cg_name directRef(       // DO DIRECT REFERENCE IF POSSIBLE
    SYMBOL orig_sym,            // - original symbol
    SYMBOL* a_sym,              // - addr[ translated symbol ]
    target_offset_t ref_off )   // - offset to item
{
    SYMBOL bound;               // - bound reference
    target_offset_t offset;     // - offset from bound reference
    cg_name op;                 // - NULL or direct-ref expression

    ref_off = ref_off;
    if( IbpReference( orig_sym, a_sym, &bound, &offset ) ) {
        op = CgSymbolPlusOffset( bound, offset );
    } else {
        op = NULL;
    }
    return op;
}


cg_name IbpFetchRef(            // FETCH A REFERENCE PARAMETER
    SYMBOL orig_sym )           // - NULL or original symbol
{
    SYMBOL sym;                 // - current symbol
    cg_name op;                 // - operand

    op = directRef( orig_sym, &sym, 0 );
    if( NULL == op ) {
        op = CgFetchType( CgSymbol( sym ), CgGetCgType( sym->sym_type ) );
    }
    return op;
}


typedef struct {                // SRCH_DATA -- search data
    BASE_CLASS* vbase_orig;     // - virtual base class, from orig
    BASE_CLASS* vbase_bound;    // - virtual base class, from bound
    target_offset_t exact_orig; // - exact, from original
} SRCH_DATA;


static SCOPE scopeForSymType(   // GET SCOPE FOR TYPE OF SYMBOL
    SYMBOL sym )                // - the symbol
{
    TYPE type;                  // - type under reduction
    TYPE ptype;                 // - type pointed at

    type = sym->sym_type;
    ptype = TypePointedAtModified( type );
    if( ptype != NULL ) {
        type = ptype;
    }
    return TypeScope( ClassTypeForType( type ) );
}


// this finds the virtual base used to access the class from the original
//
// it is the one whose offset from the original is less or equal the exact
// offset and whose offset is maximal.
//
static void checkOrig(          // CHECK EACH VIRTUAL FROM ORIGINAL
    BASE_CLASS* base,           // - base class
    void* _data )               // - search info
{
    SRCH_DATA* inf = _data;     // - search info

    if( inf->exact_orig >= base->delta ) {
        BASE_CLASS* vbase_orig = inf->vbase_orig;
        if( NULL == vbase_orig
         || vbase_orig->delta < base->delta ) {
            inf->vbase_orig = base;
        }
    }
}


// this finds the virtual base used to access the class from the bound
//
static void checkBound(         // CHECK EACH VIRTUAL FROM BOUND
    BASE_CLASS* base,           // - base class
    void* _data )               // - search info
{
    SRCH_DATA* inf = _data;     // - search info

    if( base->type == inf->vbase_orig->type ) {
        inf->vbase_bound = base;
    }
}


// this computes the offset from the bound variable to the required spot
// by subtracting the difference in the deltas from original to vbase
// and bound to base.
//
// this works because the type of the original matches or is a base of
// the type of the bound.
//
// it is required because the layout of the virtuals in the bound type may
// differ from the layout in the original type. "exact_orig" is based on the
// layout of the original type.
//
static target_offset_t offsetOfBase( // GET OFFSET TO BASE
    SYMBOL orig,                // - original symbol
    SYMBOL bound,               // - bound reference symbol
    target_offset_t exact_orig )// - offset from original to base
{
    SRCH_DATA inf;              // - search data
    SCOPE scope_orig;           // - scope for original
    SCOPE scope_bound;          // - scope for bound

    scope_orig =  scopeForSymType( orig );
    scope_bound =  scopeForSymType( bound );
    if( scope_bound != scope_orig ) {
        inf.vbase_orig = NULL;
        inf.vbase_bound = NULL;
        inf.exact_orig = exact_orig;
        ScopeWalkVirtualBases( scopeForSymType( orig ), &checkOrig, &inf );
        DbgVerify( inf.vbase_orig != NULL, "offsetOfBase -- no original base" );
        ScopeWalkVirtualBases( scopeForSymType( bound ), &checkBound, &inf );
        DbgVerify( inf.vbase_bound != NULL, "offsetOfBase -- no bound base" );
        exact_orig -= inf.vbase_orig->delta - inf.vbase_bound->delta;
    }
    return exact_orig;
}


void IbpDefineVbOffset(         // DEFINE OFFSET FOR BOUND VB-REF PARAMETER
    target_offset_t vb_exact )  // - offset from original sym to base
{
    IBRP *ibrp;                 // - current IBRP
    SYMBOL sym;                 // - translated symbol
    SYMBOL bound;               // - bound reference
    target_offset_t b_offset;   // - offset from bound reference (bound)

    ibrp = ibrps;
    if( ibrp->refed != NULL ) {
//      IbpReference( ibrp->u.parm, &sym, &bound, &b_offset );
        IbpReference( ibrp->refed, &sym, &bound, &b_offset );
        ibrp->offset = offsetOfBase( sym, bound, vb_exact );
        ibrp->refed = bound;
        dump_ibrp( ibrp, "IBRP(defined vb offset)" );
    }
}


static cg_type typeVbVfPtr(     // MAKE CG TYPE FOR VB/VF PTR
    SYMBOL sym )                // - base symbol
{
    return CgExprType( MakePointerTo( sym->sym_type ) );
}


static cg_name fetchVbVfPtr(    // FETCH VB/VF PTR
    cg_name op,                 // - expr for ptr to vb/vf table
    vindex index,               // - index into table
    TYPE vfvbtbl_cpp,           // - type for vb/vf table
    cg_type* a_type )           // - addr[ resultant type ]
{
    cg_type vfvbtbl_type;       // - type for vb-table

    vfvbtbl_type = CgExprType( vfvbtbl_cpp );
    op = CgFetchType( op, vfvbtbl_type );
    vfvbtbl_cpp = TypePointedAtModified( vfvbtbl_cpp );
    op = CgOffsetExpr( op, index * CgMemorySize( vfvbtbl_cpp ), vfvbtbl_type );
    vfvbtbl_type = CgExprType( vfvbtbl_cpp );
    *a_type = vfvbtbl_type;
    op = CgFetchType( op, vfvbtbl_type );
    return op;
}


cg_name IbpFetchVbRef(          // MAKE A REFERENCE FROM A BOUND PARAMETER
    SYMBOL orig_sym,            // - original symbol
    target_offset_t delta,      // - offset after vb computation
    target_offset_t vb_exact,   // - offset from original sym to base
    target_offset_t vb_offset,  // - offset to vb table
    vindex vb_index )           // - index in vb table
{
    SYMBOL sym;                 // - translated symbol
    cg_name op;                 // - operand
    temp_handle handle;         // - handle for temp
    cg_type vbptr_type;         // - type of temp
    cg_type vboff_type;         // - type of offset
    cg_name temp;               // - assignment before
    SYMBOL bound;               // - bound reference
    target_offset_t b_offset;   // - offset from bound reference (bound)
    target_offset_t c_offset;   // - offset from bound reference (class)

    if( IbpReference( orig_sym, &sym, &bound, &b_offset ) ) {
        c_offset = offsetOfBase( sym, bound, vb_exact );
        op = CgSymbolPlusOffset( bound, c_offset );
    } else {
        vbptr_type = typeVbVfPtr( sym );
        op = CgFetchType( CgSymbol( sym ), vbptr_type );
        op = CgOffsetExpr( op, vb_offset, vbptr_type );
        temp = CgSaveAsTemp( &handle, op, vbptr_type );
        op = fetchVbVfPtr( CgFetchTemp( handle, vbptr_type )
                         , vb_index
                         , MakeVBTableFieldType( TRUE )
                         , &vboff_type );
        op = CGBinary( O_PLUS
                     , CgFetchTemp( handle, vbptr_type )
                     , op
                     , vbptr_type );
        op = CgOffsetExpr( op, delta, vbptr_type );
        op = CgComma( temp, op, vbptr_type );
    }
    return op;
}


// Note: we still do the original virtual call when the exact is to be
//       inlined and any of the following is true:
//  - inline has state-table requirements and caller didn't
//  - inline uses conditional bits
//  - inline calls another inline (inline will not necessarily force out
//    other inlines)
//
static bool locatedVFun(        // LOCATE VIRTUAL FUNCTION FOR BASE
    SYMBOL sym,                 // - symbol
    SYMBOL vfun,                // - original virtual function
    SYMBOL* a_vfun,             // - addr[ virtual function for base ]
    target_offset_t* a_adj_this,// - addr[ this adjustment ]
    target_offset_t* a_adj_retn)// - addr[ return adjustment ]
{
    bool retn;                  // - TRUE ==> can directly call *a_vfun
    SYMBOL exact_vfun;          // - exact vfun called

    exact_vfun = ScopeFindExactVfun( vfun
                                   , scopeForSymType( sym )
                                   , a_adj_this
                                   , a_adj_retn );
    if( NULL == exact_vfun ) {
        retn = FALSE;
    } else if( 0 != *a_adj_retn ) {
        retn = FALSE;
    } else if( CgBackFuncInlined( exact_vfun ) ) {
        CGFILE* cgfile;
        cgfile = CgioLocateAnyFile( exact_vfun );
        DbgVerify( cgfile != NULL, "locatedVfun -- no CGFILE" );
        if( cgfile->u.s.calls_inline ) {
            retn = FALSE;
        } else if( cgfile->cond_flags != 0 ) {
            retn = FALSE;
        } else {
            if( cgfile->u.s.state_table && FstabHasStateTable() ) {
                retn = FALSE;
            } else {
                *a_vfun = exact_vfun;
                retn = TRUE;
            }
        }
    } else {
        *a_vfun = exact_vfun;
        retn = TRUE;
    }
    return retn;
}


// We wipe out the unused "this" computation by putting it in a left-comma
// operand.  This could generate bad code in member-ptr thunks, where
// the code really is unused. In vfun calls, the LHS assigns to a temporary
// which is fetched as part of the "this" argument and so is ok.
//
cg_name IbpFetchVfRef(          // FETCH A VIRTUAL FUNCTION ADDRESS
    SYMBOL vfun,                // - virtual function
    cg_name this_expr,          // - expression for this
    SYMBOL vf_this,             // - original symbol (for access)
    target_offset_t vf_offset,  // - offset to vf table ptr
    vindex vf_index,            // - index in vf table
    bool *is_vcall,             // - addr[ TRUE ==> real virtual call ]
    target_offset_t* a_adj_this,// - addr[ this adjustment ]
    target_offset_t* a_adj_retn,// - addr[ return adjustment ]
    SYMBOL* a_exact_vfun )      // - addr[ exact vfun to be used ]
{
    SYMBOL trans;               // - translated symbol
    SYMBOL bound;               // - bound symbol
    target_offset_t offset;     // - offset to vf_this (not used)
    cg_name expr;               // - expression under construction
    cg_type vfptr_type;         // - type of vf-ptr (CG)
    SYMBOL act_fun;             // - actual virtual function to call

    if( IbpReference( vf_this, &trans, &bound, &offset )
     && locatedVFun( bound, vfun, &act_fun, a_adj_this, a_adj_retn ) ) {
        *is_vcall = FALSE;
        *a_exact_vfun = act_fun;
        expr = CgComma( this_expr
                      , CgSymbol( act_fun )
                      , CgGetCgType( act_fun->sym_type ) );
    } else {
        vfptr_type = typeVbVfPtr( trans );
        *is_vcall = TRUE;
        *a_exact_vfun = NULL;
        expr = CgOffsetExpr( this_expr, vf_offset, vfptr_type );
        expr = fetchVbVfPtr( expr
                           , vf_index
                           , MakeVFTableFieldType( TRUE )
                           , &vfptr_type );
    }
    return expr;
}


static void ibpInit(            // INITIALIZE CGBKIBRP
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveIBRP = CarveCreate( sizeof( IBRP ), 32 );
}


static void ibpFini(            // COMPLETE CGBKIBRP
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveIBRP );
}


INITDEFN( ibp, ibpInit, ibpFini )
