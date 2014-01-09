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
* Description:  Back-end control for C++.
*
****************************************************************************/


#include "plusplus.h"

#include <float.h>

#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cgaux.h"
#include "feprotos.h"
#include "ring.h"
#include "conpool.h"
#include "stringl.h"
#include "carve.h"
#include "label.h"
#include "vstk.h"
#include "context.h"
#include "name.h"
#include "ctexcept.h"
#include "symdbg.h"
#include "dwarfdbg.h"
#include "callgrph.h"
#include "cdopt.h"
#include "initdefs.h"
#include "stats.h"
#include "specfuns.h"
#include "objmodel.h"
#include "extrf.h"
#include "fmtsym.h"
#include "floatsup.h"
#include "rtti.h"
#include "cgcli.h"
#include "fold.h"

#ifndef NDEBUG
#include "pragdefn.h"
#endif



#define ST_backwards_beg( h, i )        \
    {                                   \
        SE* __HDR = h;                  \
        SE* __PRV = NULL;               \
        for( i = __HDR; __PRV != __HDR; i = __PRV ) {  \
            __PRV = i->base.prev;

#define ST_backwards_end                \
        }                               \
    }

#define ST_backwards_prev( p )          \
            __PRV = p

#define ST_backwards_setsv( s )         \
            __PRV = (s)->set_sv.se;     \
            if( __PRV == NULL ) {       \
                __PRV = __HDR;          \
            }

#define IC_PARM_STACK_SIZE      (16)
#define IC_PARM_PUSH_PTR( p )           \
        DbgAssert( ic_sp < IC_PARM_STACK_SIZE ); \
        ic_parms[ic_sp++].pvalue = p;
#define IC_PARM_PUSH_INT( i )           \
        DbgAssert( ic_sp < IC_PARM_STACK_SIZE ); \
        ic_parms[ic_sp++].ivalue = i;
#define IC_PARM_POP_PTR( p )            \
        DbgAssert( ic_sp != 0 );        \
        p = ic_parms[--ic_sp].pvalue;
#define IC_PARM_POP_INT( i )            \
        DbgAssert( ic_sp != 0 );        \
        i = ic_parms[--ic_sp].ivalue;
#define IC_PARM_DONE                    \
        DbgAssert( ic_sp == 0 );        \
        ic_sp = 0;
#define IC_PARM_SKIP                    \
        DbgAssert( ic_sp != 0 );        \
        --ic_sp;

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #define dump_label( ins ) if( PragDbgToggle.dump_labels ) ins
#else
    #define dump_label( ins )
#endif


#define TY_VOID TY_INTEGER


static VSTK_CTL stack_labs_cs;      // stack: labels (control)
static VSTK_CTL stack_goto_near;    // stack: gotos (near)

static unsigned max_inline_depth;   // maximum depth of inlining
static unsigned depth_inline;       // depth of inlining
static SYMBOL statics;              // static symbols (freed at file end)
static SYMBOL autos;                // automatic variables for procedure
static carve_t carveTRY_IMPL;       // allocations for TRY_IMPL
static carve_t carveSTAB_OBJ;       // allocations for STAB_OBJ
static SCOPE scope_exit;            // scope exited by IC_SCOPE_EXIT
static CGFILE *data_file;           // data file
static STAB_OBJ* state_tables_obj;  // state tables for objects
static bool sig_thunk_genned;       // TRUE ==> significant thunk genned

static SYMBOL thisSym;              // model for 'this' symbol
static SYMBOL cdtorSym;             // model for cdtor extra parm

static TRY_IMPL* fun_try_impls;     // function try implementations
// static SYMBOL new_ctor_ptr;         // contains ptr to new'ed area

ExtraRptCtr( ctr_ic_codes );        // number of IC codes
ExtraRptCtr( ctr_inlines );         // number of inlines
ExtraRptCtr( ctr_funcs );           // number of functions

CGBK_INFO CgbkInfo;                 // useful information


static void cgResetThis(        // RESET THIS, IF REQUIRED
    target_offset_t offset )    // - offset of object just CTOR'ed/DTOR'ed
{
    SYMBOL sym;                 // - symbol for " this"
    SYMBOL ref;                 // - not used: bound reference
    target_offset_t offset_ref; // - not used: offset from bound reference
    cg_name expr;               // - expression

    if( ! IbpReference( NULL, &sym, &ref, &offset_ref ) ) {
        expr = CgExprPop();
        if( offset > 0 ) {
           expr = CGBinary( O_MINUS, expr, CgOffset( offset ), TY_POINTER );
        }
        expr = CGLVAssign( CgSymbol( sym ), expr, TY_POINTER );
        CgExprPush( expr, TY_POINTER );
    }
}


unsigned CgNonThunkDepth(       // COMPUTE INLINE DEPTH WITHOUT THUNKS
    FN_CTL* fctl )              // - current function
{
    unsigned real_depth;        // - depth without inlines

    real_depth = depth_inline;
    if( real_depth > 0 ) {
        for( ; ; ) {
            fctl = FnCtlPrev( fctl );
            if( fctl == NULL ) break;
            if( SymIsThunk( fctl->func ) ) {
                -- real_depth;
            }
        }
    }
    return real_depth;
}

#if 0
static void push_optional_expr( // PUSH EXPRESSION IF NOT EMPTY
    cg_name expr,               // - NULL or expression
    cg_type type )              // - type, when non-null
{
    if( expr != NULL ) {
        CgExprPush( expr, type );
    }
}
#endif


#if 0
static void push_reqd_expr(     // PUSH REQUIRED EXPRESSION ON STACK
    cg_name expr,               // - expression to be pushed
    cg_type type )              // - type
{
    if( expr == NULL ) {
        CgPushGarbage();
    } else {
        CgExprPush( expr, type );
    }
}
#endif


static TRY_IMPL* tryImpl(       // FIND/ALLOCATE A TRY IMPLEMENTATION
    SYMBOL sym )                // - try variable
{
#if _CPU == _AXP
    #define JMPBUF_SIZE ( 24 * TARGET_DOUBLE )
#elif _INTEL_CPU
    #define JMPBUF_SIZE ( 13 * TARGET_INT )
#else
    #error bad target
#endif
    TRY_IMPL* try_impl;         // - try implementation
    TRY_IMPL* curr;             // - current try implementation

    sym = SymTrans( sym );
    try_impl = NULL;
    RingIterBeg( fun_try_impls, curr ) {
        if( curr->try_sym == sym ) {
            try_impl = curr;
            break;
        }
    } RingIterEnd( curr )
    if( try_impl == NULL ) {
        try_impl = RingCarveAlloc( carveTRY_IMPL, &fun_try_impls );
        try_impl->try_sym = sym;
        AutoRelRegister( try_impl->try_sym, &try_impl->offset_var );
        try_impl->jmp_sym = CgVarRw( JMPBUF_SIZE, SC_AUTO );
        AutoRelRegister( try_impl->jmp_sym, &try_impl->offset_jmpbuf );
    }
    return try_impl;
}


static void freeTryImpls(       // FREE TRY IMPLEMENTATIONS
    void )
{
    RingCarveFree( carveTRY_IMPL, &fun_try_impls );
}


static SE* stateTableTryBlk(    // GET CURRENT STATE TABLE TRY BLOCK
    TRY_IMPL* try_impl )        // - try implementation
{
    SE* try_blk;                // - last try block
    SE* curr;                   // - current state entry

    try_blk = NULL;
    ST_backwards_beg( FstabActualPosn(), curr ) {
        if( ( curr->base.se_type == DTC_TRY )
          &&( curr->try_blk.try_impl == try_impl ) ) {
            try_blk = curr;
            break;
        }
    } ST_backwards_end
    DbgVerify( try_blk != NULL, "stateTableTryBlk -- none" );
    return try_blk;
}


static void undefine_string_const(  // UN-DEFINE A STRING CONSTANT
    STRING_CONSTANT str )           // - the constant
{
    if( str->cg_handle != NULL ) {
        BEFreeBack( str->cg_handle );
        str->cg_handle = NULL;
    }
}


static SYMBOL transThisSym(     // TRANSLATE TO "this_sym" WHEN NULL
    SYMBOL sym,                 // - input symbol
    FN_CTL *fctl )              // - current function control pointer
{
    if( sym == NULL
     && ! SymIsStatic( fctl->func ) ) {
        sym = fctl->this_sym;
    }
    return sym;
}


static void funcDebugInfo(      // DEFINE FUNCTION DEBUGGING INFORMATION
    FN_CTL *fctl )              // - current function control pointer
{
    if( fctl->debug_info && ( GenSwitches & DBG_LOCALS ) ){
        if( SymIsClassMember( fctl->func ) ) {
            SYMBOL this_sym;
            this_sym = transThisSym( NULL, fctl );
            if( GenSwitches & DBG_DF ){
                DwarfDebugMemberFunc( fctl->func, this_sym );
            }else{
                SymbolicDebugMemberFunc( fctl->func, this_sym );
            }
        }else if( SymIsNameSpaceMember( fctl->func ) ){
            if( GenSwitches & DBG_DF ){
                DwarfDebugNameSpaceEnclosed( fctl->func );
            }
        }
    }
}


#define flushOverInitialization( __fc ) \
        CgioReadICUntilOpcode( __fc, IC_INIT_DONE )


static SYMBOL finalAlias(       // CHECK FOR ANONYMOUS UNION ALIASING
    SYMBOL sym )                // - the symbol
{
    SYMBOL check;

    check = SymIsAnonymous( sym );
    if( check != NULL ) {
        sym = check;
    }
    return( sym );
}


static void cgDtorThisOffset(   // DESTRUCT (*this)+offset
    FN_CTL* fctl,               // - file information
    SYMBOL dtor,                // - DTOR to be used
    target_offset_t offset,     // - offset
    unsigned cdtor )            // - CDTOR value to be used
{
    SYMBOL this_sym;            // - symbol for "this"
    SYMBOL refd_sym;            // - symbol bound to "this"
    target_offset_t refd_off;   // - offset from "refd_sym"
    cg_name expr;               // - generated expression

    if( IbpReference( NULL, &this_sym, &refd_sym, &refd_off ) ) {
        CgDone( CgDestructSymOffset( fctl
                                   , dtor
                                   , refd_sym
                                   , offset + refd_off
                                   , cdtor )
              , TY_POINTER );
    } else {
        expr = CgFetchSymbolAddOffset( this_sym, offset );
        expr = CgDestructExpr( dtor, expr, cdtor );
        CgExprPush( expr, TY_POINTER );
        cgResetThis( offset );
        CgDone( CgExprPop(), TY_POINTER );
    }
}


static void emitDtorComponent(  // EMIT CODE TO DESTRUCT COMPONENT
    FN_CTL* fctl,               // - file information
    SE* se,                     // - component entry
    unsigned cdtor )            // - CDTOR parameter
{
    if( ! fctl->ctor_complete ) {
        FstabSetDtorState( se, fctl );
        cgDtorThisOffset( fctl
                        , se->component.dtor
                        , se->component.offset
                        , cdtor );
    }
}


static SE* nextDirectSe(        // POSITION TO NEXT SE (DIRECT DESTRUCTION)
    SE* se,                     // - current entry
    DGRP_FLAGS flags )          // - control flags
{
    if( flags & DGRP_TEMPS ) {
        se = FstabPrecedes( se );
    } else {
        se = FstabPrevious( se );
    }
    return se;
}


static DGRP_FLAGS emitXstBeg(   // IF REQ'D, EMIT START DTOR-OF-CTOR CODE
    FN_CTL* fctl,               // - file information
    DGRP_FLAGS flags )
{
    #define EMIT_MASK ( DGRP_CTOR       \
                      | DGRP_CTOR_BEG )
    #define EMIT_REQD ( DGRP_CTOR )

    if( EMIT_REQD == ( flags & EMIT_MASK ) ) {
        CgCtorTestTempsRegister( fctl );
        flags |= DGRP_CTOR_BEG;
    }
    return flags;

    #undef EMIT_MASK
    #undef EMIT_REQD
}


#if 0
static DGRP_FLAGS emitXstEnd(   // IF REQ'D, EMIT END DTOR-OF-CTOR CODE
    FN_CTL* fctl,               // - file information
    DGRP_FLAGS flags )
{
    #define EMIT_MASK ( DGRP_CTOR       \
                      | DGRP_XST        \
                      | DGRP_CTOR_BEG   \
                      | DGRP_CTOR_END )
    #define EMIT_REQD ( DGRP_CTOR       \
                      | DGRP_XST        \
                      | DGRP_CTOR_BEG )

    if( EMIT_REQD == ( flags & EMIT_MASK ) ) {
        CgCtorTestTempsDeregister( fctl );
        flags |= DGRP_CTOR_END;
    }
    return flags;

    #undef EMIT_MASK
    #undef EMIT_REQD
}
#endif


static DGRP_FLAGS emitTryedPosn // EMIT POSN AFTER POPPING TRY
    ( SE* try_se                // - the position
    , FN_CTL* fctl              // - file information
    , DGRP_FLAGS flags )        // - flags
{
    if( flags & DGRP_TRY_EMIT ) {
        FstabEmitStateVarExpr( try_se, fctl );
        flags &= ~ DGRP_TRY_EMIT;
    }
    return flags;
}


static void cgDtorTabCall(      // DESTRUCT UP TO ENTRY (TABULAR)
    SE* start,                  // - starting entry
    SE* bound )                 // - bounding entry
{
    bound = SeSetSvPosition( bound );
    if( bound == NULL ) {
        if( NULL != SeSetSvPosition( start ) ) {
            CgDtorAll();
        }
    } else {
        CgDtorSe( bound );
    }
}


static unsigned cgDestructGroup(// DESTRUCT UP TO STATE ENTRY
    FN_CTL* fctl,               // - file information
    SE* start,                  // - starting state entry
    SE* bound,                  // - bounding state entry
    DGRP_FLAGS flags )          // - control flags
{
    SE* se;                     // - state entry in state table
    SE* try_se;                 // - position preceding last try
    unsigned cdtor;             // - current CDTOR parameter
    label_handle virt_label = NULL; // - label for virtual by-pass test
    unsigned destructions;      // - # destructions
    COND_LABEL* lab_ring;       // - ring hdr. for conditional labels

    lab_ring = NULL;
    destructions = 0;
    cdtor = DTOR_NULL;
    try_se = NULL;
    switch( fctl->dtor_method ) {
      case DTM_DIRECT_TABLE :
        flags |= DGRP_LIVE | DGRP_XST;
        break;
      default :
        flags |= DGRP_LIVE;
        break;
    }
    for( se = start; se != NULL && se != bound; ) {
        if( flags & DGRP_DIRECT ) {
            if( CondLabelEmit( &lab_ring, se ) ) {
                flags |= DGRP_LIVE;
            }
        }
        if( 0 == ( flags & DGRP_LIVE ) ) {
            se = nextDirectSe( se, flags );
            continue;
        }
        switch( se->base.se_type ) {
          case DTC_SYM_AUTO :
            if( flags & DGRP_DIRECT ) {
                flags = emitXstBeg( fctl, flags );
                FstabSetDtorState( se, fctl );
                CgDone( CgDestructSymOffset( fctl
                                           , se->sym_auto.dtor
                                           , se->sym_auto.sym
                                           , 0
                                           , cdtor )
                      , TY_POINTER );
            } else {
                ++ destructions;
                flags |= DGRP_TAB_CALL;
            }
            flags &= ~ DGRP_TRY_EMIT;
            se = nextDirectSe( se, flags );
            continue;
          case DTC_TEST_FLAG :
            if( flags & DGRP_DIRECT ) {
                COND_INFO cond;             // - conditional information
                label_handle lab;           // - conditional label
                cg_name expr;               // - expression being generated
                flags = emitXstBeg( fctl, flags );
                lab = CondLabelAdd( &lab_ring, se->test_flag.se_true );
                CondInfoSetup( se->test_flag.index, &cond, fctl );
                expr = CgSymbolPlusOffset( cond.sym, cond.offset );
                expr = CgFetchType( expr, TY_UINT_1 );
                expr = CGBinary( O_AND, expr, CgOffset( cond.mask ), TY_UINT_1 );
                CgControl( O_IF_TRUE, expr, TY_UINT_1, lab );
                lab = CondLabelAdd( &lab_ring, se->test_flag.se_false );
                CGControl( O_GOTO, NULL, lab );
                flags &= ~ DGRP_LIVE;
            }
            se = nextDirectSe( se, flags );
            continue;
          case DTC_CTOR_TEST :
            se = nextDirectSe( se, flags );
            if( se == bound ) break;
            se = nextDirectSe( se, flags );
            continue;
          case DTC_SET_SV :
            if( flags & DGRP_DIRECT ) {
                label_handle lab;           // - conditional label
                flags = emitXstBeg( fctl, flags );
                lab = CondLabelAdd( &lab_ring, se->set_sv.se );
                CGControl( O_GOTO, NULL, lab );
                flags &= ~ DGRP_LIVE;
                if( flags & DGRP_TEMPS ) {
                    se = nextDirectSe( se, flags );
                } else {
                    se = CondLabelNext( &lab_ring, se->set_sv.se );
                }
            } else {
                if( flags & DGRP_TEMPS ) {
                    se = nextDirectSe( se, flags );
                } else {
                    se = se->set_sv.se;
                }
            }
            continue;
          case DTC_DLT_1 :
          case DTC_DLT_2 :
          case DTC_DLT_1_ARRAY :
          case DTC_DLT_2_ARRAY :
          case DTC_FN_EXC :
          case DTC_ARRAY_INIT :
            se = nextDirectSe( se, flags );
            continue;
          case DTC_TRY :
            se = FstabPrevious( se );
            try_se = se;
            flags |= DGRP_TRY_EMIT;
            continue;
          case DTC_CATCH :
            if( flags & DGRP_DIRECT ) {
                RT_DEF def;                 // - call definition
                flags = emitXstBeg( fctl, flags );
                flags = emitTryedPosn( try_se, fctl, flags );
                CgRtCallInit( &def, RTF_CATCH_END );
              #if _CPU == _AXP
                CgRtParamAddrSym( &def, FstabExcRw() );
              #endif
                CgRtCallExecDone( &def );
            } else {
                ++ destructions;
                flags |= DGRP_TAB_CALL;
            }
            se = se->catch_blk.try_blk;
            continue;
          case DTC_COMP_VBASE :
            if( flags & DGRP_DIRECT ) {
                if( ! fctl->ctor_complete ) {
                    if( flags & DGRP_COMPS ) {
                        flags = emitTryedPosn( try_se, fctl, flags );
                        if( (flags & DGRP_VTEST) == 0 ) {
                            cg_name expr;       // - expression
                            cg_type type;       // - expression type
                            type = CgExprType( fctl->cdtor_sym->sym_type );
                            expr = CGBinary( O_AND
                                           , CgFetchSym( fctl->cdtor_sym )
                                           , CgOffset( DTOR_COMPONENT )
                                           , type );
                            virt_label = BENewLabel();
                            CgControl( O_IF_TRUE, expr, type, virt_label );
                            flags |= DGRP_VTEST;
                        }
                        flags = emitXstBeg( fctl, flags );
                        emitDtorComponent( fctl, se, DTOR_COMPONENT );
                    }
                }
            }
            se = nextDirectSe( se, flags );
            continue;
          case DTC_ACTUAL_VBASE :
          case DTC_ACTUAL_DBASE :
          case DTC_COMP_DBASE :
            if( flags & DGRP_DIRECT ) {
                if( flags & DGRP_COMPS ) {
                    flags = emitXstBeg( fctl, flags );
                    flags = emitTryedPosn( try_se, fctl, flags );
                    emitDtorComponent( fctl, se, DTOR_COMPONENT );
                }
            }
            se = nextDirectSe( se, flags );
            continue;
          case DTC_COMP_MEMB :
            if( flags & DGRP_DIRECT ) {
                if( flags & DGRP_COMPS ) {
                    flags = emitXstBeg( fctl, flags );
                    flags = emitTryedPosn( try_se, fctl, flags );
                    emitDtorComponent( fctl, se, DTOR_NULL );
                }
            }
            se = nextDirectSe( se, flags );
            continue;
          DbgDefault( "CgDestructGroup -- bad DTC code" );
        }
        break;
    }
    if( flags & DGRP_DIRECT ) {
        CondLabelEmit( &lab_ring, se );
        flags = emitTryedPosn( try_se, fctl, flags );
        if( flags & DGRP_VTEST ) {
            CGControl( O_LABEL, NULL, virt_label );
            BEFiniLabel( virt_label );
        }
        CondLabelsEmit( &lab_ring );
    } else {
        if( (flags & DGRP_COUNT) == 0 ) {
            if( flags & DGRP_TAB_CALL ) {
                flags = emitXstBeg( fctl, flags );
                cgDtorTabCall( start, bound );
            }
            flags = emitTryedPosn( try_se, fctl, flags );
        }
    }
    if( (flags & DGRP_COUNT) == 0 ) {
        FstabMarkedPosnSet( bound );
        if( 0 == destructions && bound != start ) {
            FstabEmitStateVarExpr( bound, fctl );
        }
    }
    return destructions;
}



static void cgDestructStab(     // DESTRUCT STATE TABLE UP TO STATE ENTRY
    SE* bound,                  // - bounding state entry
    FN_CTL* fctl,               // - function information
    SE* start,                  // - starting state entry
    DGRP_FLAGS flags )          // - controlling flags
{
    int count;                  // - # of destructions

    switch( fctl->dtor_method ) {
      default :
        count = cgDestructGroup( fctl, start, bound, flags | DGRP_COUNT );
        if( count >= 2 ) {
            count = cgDestructGroup( fctl, start, bound, flags );
            break;
        }
        // drops thru
      case DTM_DIRECT :
      case DTM_DIRECT_TABLE :
      case DTM_DIRECT_SMALL :
        cgDestructGroup( fctl, start, bound, flags | DGRP_DIRECT );
        break;
    }
    FstabSetSvSe( bound );
}


static void cgDestruct(         // DESTRUCT UP TO STATE ENTRY
    SE* bound,                  // - state entry
    FN_CTL* fctl )              // - function information
{
    cgDestructStab( bound, fctl, FstabActualPosn(), 0 );
}


static SE* buildObjectSe        // BUILD SUBOBJ STATE ENTRY
    ( CDOPT_ITER* iter          // - object iterator
    , DTC_KIND kind )           // - kind of object
{
    SE* se;                     // - current state entry
    TYPE type;                  // - type of object
    TYPE array_type;            // - array type

    se = SeAlloc( DTC_SUBOBJ );
    se->subobj.original = CDoptIterOffsetComp( iter );
    se->subobj.offset = CDoptIterOffsetExact( iter );
    se->subobj.kind = kind;
    type = CDoptIterType( iter );
    array_type = ArrayType( type );
    if( NULL == array_type ) {
        se->subobj.type = StructType( type );
        se->subobj.dtor = CDoptIterFunction( iter );
    } else {
        se->subobj.type = array_type;
        se->subobj.dtor = RoDtorFindType( type );
    }
    return se;
}


static STAB_OBJ* buildObjectStateTable( // BUILD STATE TABLE FOR OBJECT
    TYPE type )                 // - type of object
{
    STAB_OBJ* curr;             // - current object ptr
    STAB_OBJ* obj;              // - object ptr for type
    CD_DESCR *dtinfo;           // - descriptive information
    CDOPT_ITER* iter;           // - iterator for components
    TITER comp_type;            // - type of component
    SE* se;                     // - current state entry

    type = StructType( type );
    if( ! TypeRequiresDtoring( type ) ) {
        obj = NULL;
    } else {
        obj = NULL;
        RingIterBeg( state_tables_obj, curr ) {
            if( type == curr->obj ) {
                obj = curr;
                break;
            }
        } RingIterEnd( curr )
        if( obj == NULL ) {
            dtinfo = CDoptDtorBuild( type );
            DbgVerify( ! CDoptErrorOccurred( dtinfo )
                     , "buildObjectStateTable -- CDOPT error" );
            iter = CDoptIterBeg( dtinfo );
            comp_type = CDoptIterNextComp( iter );
            if( TITER_NONE == comp_type ) {
                obj = NULL;
            } else {
                SE* se_stack;   // - temporary stack
                SE* se_virt;    // - first virtual
                SE* se_dir;     // - first direct
                STATE_VAR sv;   // - state # for objects
                obj = RingCarveAlloc( carveSTAB_OBJ, &state_tables_obj );
                obj->obj = type;
                obj->state_direct = 0;
                obj->state_virtual = 0;
                obj->defn = StabDefnAllocate( DTRG_OBJECT );
#ifndef NDEBUG
                if( PragDbgToggle.dump_stab ) {
                    printf( "State Table for static object: %p\n"
                          , &obj->defn->state_table );
                }
#endif
                se_stack = NULL;
                se_virt = NULL;
                se_dir = NULL;
                for( ; comp_type != TITER_NONE; ) {
                    unsigned depth; // - depth of elements
                    depth = 0;
                    for( ; ; ) {
                        TITER subobj_type = CDoptIterNextElement( iter );
                        switch( subobj_type ) {
                          case TITER_NONE :
                            if( depth == 0 ) break;
                            --depth;
                            continue;
                          case TITER_ARRAY_EXACT :
                          case TITER_ARRAY_VBASE :
                          case TITER_CLASS_EXACT :
                          case TITER_CLASS_VBASE :
                          case TITER_CLASS_DBASE :
                          case TITER_NAKED_DTOR :
                            se = buildObjectSe( iter
                                              , CDoptObjectKind( iter ) );
                            se->base.prev = se_stack;
                            se_stack = se;
                            switch( comp_type ) {
                              case TITER_VBASE :
                                if( se_virt == NULL ) {
                                    se_virt = se;
                                }
                                break;
                              case TITER_DBASE :
                                if( se_dir == NULL ) {
                                    se_dir = se;
                                }
                                break;
                            }
                            continue;
                          case TITER_MEMB :
                          case TITER_DBASE :
                          case TITER_VBASE :
                            ++depth;
                            continue;
                          DbgDefault( "buildObjectStateTable -- bad TITER" );
                        }
                        break;
                    }
                    comp_type = CDoptIterNextComp( iter );
                }
                for( sv = 1; se_stack != NULL; ++sv ) {
                    SE* curr;
                    curr = se_stack;
                    se_stack = curr->base.prev;
                    StabDefnAddSe( curr, obj->defn );
                    curr->base.state_var = sv;
                }
                obj->state_direct = SeStateOptimal( se_dir );
                obj->state_virtual = SeStateOptimal( se_virt );
#ifndef NDEBUG
                if( PragDbgToggle.dump_stab ) {
                    DbgDumpStateTableDefn( obj->defn );
                }
#endif
            }
            CDoptIterEnd( iter );
        }
    }
    return obj;
}


static bool initCDtorStateTable(// OBTAIN STATE TABLE FOR CTOR OR DTOR
    FN_CTL* fctl,               // - CTOR/DTOR function information
    TYPE type )                 // - type of object
{
    STAB_OBJ* obj;              // - object ptr for type
    bool retn;                  // - TRUE ==> set up the table
    OBJ_INIT* init;             // - initialization entry

    obj = buildObjectStateTable( type );
    fctl->obj_registration = obj;
    if( obj == NULL ) {
        retn = FALSE;
    } else {
        init = ObjInitPush( type );
        init->defn = obj;
        init->reg = DtregObj( fctl );
        retn = TRUE;
    }
    return retn;
}


static void freeObjTables(      // FREE OBJECT STATE TABLES
    void )
{
    STAB_OBJ* otab;             // - current state table

    RingIterBegSafe( state_tables_obj, otab ) {//{
        StabDefnFreeStateTable( otab->defn );
        StabDefnFree( otab->defn );
        CarveFree( carveSTAB_OBJ, otab );
    } RingIterEndSafe( otab )
}


static SE* cgAddSeComponent     // ADD STATE DTC_COMPONENT STATE ENTRY
    ( DTC_KIND se_type          // - entry type
    , target_offset_t offset    // - offset of entry
    , SYMBOL dtor )             // - destructor to be used
{
    SE* se;                     // - state entry in function table
    OBJ_INIT* init;             // - current initialization object

    se = SeAlloc( se_type );
    se->component.offset = offset;
    se->component.dtor = dtor;
    init = ObjInitTop();
    init->se = se;
    if( se_type == DTC_ACTUAL_VBASE
     || se_type == DTC_ACTUAL_DBASE ) {
        se->component.obj = DtregActualBase( FnCtlTop() );
    } else {
        se->component.obj = init->reg;
    }
    return se;
}


static void ftabAddSubobjs(     // ADD SUB-OBJECTS TO DTOR'S FUNCTION TABLE
    FN_CTL* fctl )              // - function information
{
    SE* se_obj;                 // - state entry in object table
    SE* se_fun;                 // - state entry in function table
    OBJ_INIT* odef;             // - current initialization object
    STAB_OBJ* otab;             // - object-table definition

    odef = ObjInitTop();
    otab = odef->defn;
    if( otab != NULL
     || ! fctl->has_cdtor_val
     || (fctl->cdtor_val & DTOR_DELETE_VECTOR) == 0 ) {
        bool is_component;
        if( fctl->has_cdtor_val && ( fctl->cdtor_val & DTOR_COMPONENT ) ) {
            is_component = TRUE;
        } else {
            is_component = FALSE;
        }
        se_fun = NULL;
        RingIterBeg( otab->defn->state_table, se_obj ) {
            if( ! is_component
             || se_obj->base.state_var > otab->state_virtual ) {
                se_fun = cgAddSeComponent( se_obj->subobj.kind
                                         , se_obj->subobj.offset
                                         , se_obj->subobj.dtor );
                if( DtmTabular( fctl ) ) {
                    se_fun->base.gen = TRUE;
                    if( se_fun->base.se_type == DTC_ACTUAL_DBASE
                     || se_fun->base.se_type == DTC_ACTUAL_VBASE ) {
                        cg_name expr = ObjInitRegActualBase( se_fun );
                        CGDone( expr );
                    }
                }
                se_fun = FstabAdd( se_fun );
            }
        } RingIterEnd( se_obj )
        if( se_fun != NULL ) {
            BlkPosnUpdate( se_fun );
        }
    }
}


static void registerObject(     // SETUP OBJECT REGISTRATION
    FN_CTL* fctl )              // - function information
{
    CGDone( ObjInitRegisterObj( fctl
                              , IbpFetchRef( NULL )
                              , fctl->cdtor_sym != NULL ) );
}


static void setupCtorOtab(      // SETUP OBJECT STATE TABLE FOR CTOR
    FN_CTL* fctl )              // - current file generation information
{
    TYPE type;                  // - class type for object

    type = SymClass( fctl->func );
    if( initCDtorStateTable( fctl, type )
     && DtmTabular( fctl ) ) {
        registerObject( fctl );
    }
}


static void setupDtorOtab(      // SETUP OBJECT STATE TABLE FOR DTOR
    FN_CTL* fctl )              // - current file generation information
{
    TYPE type;                  // - type for dtor
//    STAB_OBJ* obj;              // - object definition

    type = SymClass( fctl->func );
    if( initCDtorStateTable( fctl, type ) ) {
        ftabAddSubobjs( fctl );
//        obj = fctl->obj_registration;
        if( DtmTabular( fctl ) ) {
            registerObject( fctl );
        }
    }
}


static SYMBOL saveGenedExpr(    // SAVE OPTIONAL GENERATED EXPRESSION
    cg_type exprn_type )        // - current expression type
{
    SYMBOL temp;                // - for saved expression value

    temp = NULL;
    switch( CgExprStackSize() ) {
      case 0 :
        break;
      case 1 :
        if( !CgExprPopGarbage() ) {
            temp = CgVarTempTyped( exprn_type );
            CgAssign( CgSymbol( temp ), CgExprPop(), exprn_type );
        }
        break;
      DbgDefault( "saveGenedExpr -- too many temps" );
    }
    return temp;
}


static void fetchGenedExpr(     // FETCH OPTIONAL SAVED EXPRESSION VALUE
    SYMBOL temp )               // - NULL or symbol for expression
{
    if( temp != NULL ) {
        CgExprPush( CgFetchSym( temp ), CgTypeSym( temp ) );
    }
}


static SE* dtorAutoSymbol(      // SIGNAL DTOR OF AUTO SYMBOL
    FN_CTL* fctl,               // - function information
    SYMBOL sym )                // - the symbol
{
    SE* se;                     // - state entry
    SYMBOL trans;               // - translated symbol
    SYMBOL dtor;

    sym = SymTrans( sym );
    se = SeAlloc( DTC_SYM_AUTO );
    if( DTM_DIRECT == fctl->dtor_method ) {
        dtor = DtorFindCg( sym->sym_type );
    } else {
        dtor = RoDtorFind( sym );
    }
    se->sym_auto.dtor = dtor;
    trans = SymDeAlias( sym );
    if( trans != sym ) {
        trans = SymTrans( trans );
    }
    se->sym_auto.sym = trans;
    if( se->base.gen ) {
        AutoRelRegister( trans, &se->sym_auto.offset );
    }
    return se;
}


static void cdArgTest(          // TESTING CODE FOR CDARG
    FN_CTL* fctl,               // - function information
    unsigned mask,              // - test mask
    bool branch_on )            // - TRUE ==> branch if on
{
    unsigned optype;            // - type of goto
    cg_name expr;               // - expression under construction

    if( fctl->has_cdtor_val ) {
        // test not needed
    } else {
        expr = CgFetchSym( fctl->cdtor_sym );
        expr = CGBinary( O_AND, expr, CGInteger( mask, TY_UINT_1 ), TY_UINT_1 );
        expr = CGCompare( O_NE, expr, CGInteger( 0, TY_UINT_1 ), TY_BOOL );
        optype = branch_on ? O_IF_TRUE : O_IF_FALSE;
        fctl->cdarg_lab = BENewLabel();
        CGControl( optype, expr, fctl->cdarg_lab );
    }
}


static void genCtorDispInitCode(// generate ctor-disp init code
    FN_CTL* fctl,               // - function information
    TYPE class_type )           // - class type of ctor/dtor
{
    /* generated code:

        unsigned *cdp;
        unsigned cdv;
        unsigned i;

        cdp = this->vbptr;
        -- for every virtual base that requires a ctor-disp, generate:
            cdv = cdp[i] + offsetof(scope,vbptr);
            *(this + (cdv - sizeof(unsigned))) = cdv;
    */
    CLASSINFO *info;
    target_offset_t vb_offset;
    temp_handle cdp;
    temp_handle cdv;
    cg_name e1;
    cg_name e2;
    BASE_CLASS *base;

    /* unsigned const *cdp; */
    cdp = CGTemp( TY_POINTER );
    /* unsigned cdv; */
    cdv = CGTemp( TY_UNSIGNED );
    /* cdp = this->vbptr; */
    info = class_type->u.c.info;
    vb_offset = info->vb_offset;
    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e1 = CGBinary( O_PLUS, e1, CgOffset( vb_offset ), TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e2 = CGTempName( cdp, TY_POINTER );
    e1 = CGAssign( e2, e1, TY_POINTER );
    CGDone( e1 );
    RingIterBeg( info->bases, base ) {
        if( _IsVirtualBase( base ) ) {
            if( TypeCtorDispRequired( class_type, base->type ) ) {
                /* cdv = cdp[i] + offsetof(scope,vbptr); */
                e1 = CGTempName( cdp, TY_POINTER );
                e1 = CGUnary( O_POINTS, e1, TY_POINTER );
                e2 = CgOffset( base->vb_index * TARGET_UINT );
                e1 = CGBinary( O_PLUS, e1, e2, TY_POINTER );
                e1 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
                e1 = CGBinary( O_PLUS, e1, CgOffset( vb_offset ), TY_POINTER );
                e2 = CGTempName( cdv, TY_UNSIGNED );
                e1 = CGAssign( e2, e1, TY_UNSIGNED );
                CGDone( e1 );
                /* *(this + (cdv - sizeof(unsigned))) = cdv; */
                e1 = CGTempName( cdv, TY_UNSIGNED );
                e1 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
                e2 = CgOffset( TARGET_UINT );
                e2 = CGBinary( O_MINUS, e1, e2, TY_UNSIGNED );
                e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
                e1 = CGUnary( O_POINTS, e1, TY_POINTER );
                e2 = CGBinary( O_PLUS, e1, e2, TY_POINTER );
                e1 = CGTempName( cdv, TY_UNSIGNED );
                e1 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
                e1 = CGAssign( e2, e1, TY_UNSIGNED );
                CGDone( e1 );
            }
        }
    } RingIterEnd( base )
}

static void genExactVPtrInit(   // generate a vptr init with exact delta
    FN_CTL* fctl,               // - function information
    target_offset_t delta,      // - offset in class of vptr
    SYMBOL table_sym,           // - symbol of table to init vptr with
    bool vbptr )                // - a vbptr?
{
    cg_name e1;
    cg_name e2;

    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e1 = CGBinary( O_PLUS, e1, CgOffset( delta ), TY_POINTER );
    e2 = CGFEName( (cg_sym_handle)table_sym, TY_POINTER );
    if( ! vbptr ) {
        e2 = CGBinary( O_PLUS, e2, CgOffset( CgDataPtrSize() ), TY_POINTER );
    }
    e1 = CGAssign( e1, e2, TY_POINTER );
    CGDone( e1 );
}

static void genVBaseVPtrInit(   // generate a vptr init in a virtual base
    FN_CTL* fctl,               // - function information
    target_offset_t vb_offset,  // - offset in class of vbptr
    vindex vb_index,            // - vbase index in table
    target_offset_t delta,      // - offset in class of vptr
    SYMBOL table_sym,           // - symbol of table to init vptr with
    bool vbptr )                // - a vbptr?
{
    cg_name e1;
    cg_name e2;

    /*
        unsigned const *cdp;
        unsigned cdv;
        cdp = this->vbptr;
        cdv = cdp[i] + vb_offset + delta;
        *(this + cdv) = &sym;
    */
    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e1 = CGBinary( O_PLUS, e1, CgOffset( vb_offset ), TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e2 = CgOffset( vb_index * TARGET_UINT );
    e1 = CGBinary( O_PLUS, e1, e2, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
    e2 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e2 = CGUnary( O_POINTS, e2, TY_POINTER );
    e1 = CGBinary( O_PLUS, e2, e1, TY_POINTER );
    e1 = CGBinary( O_PLUS, e1, CgOffset( vb_offset + delta ), TY_POINTER );
    e2 = CGFEName( (cg_sym_handle)table_sym, TY_POINTER );
    if( ! vbptr ) {
        e2 = CGBinary( O_PLUS, e2, CgOffset( CgDataPtrSize() ), TY_POINTER );
    }
    e1 = CGAssign( e1, e2, TY_POINTER );
    CGDone( e1 );
}


static void genVthunkDelta(     // generate virtual function thunk deltas
    FN_CTL* fctl,               // - function information
    unsigned op,                // - O_PLUS or O_MINUS
    target_offset_t delta )     // - delta amount
{
    /* generated code:

        this -= delta;
        this += delta;
    */
    cg_name e1;
    cg_name e2;

    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e2 = CgOffset( delta );
    e1 = CGPreGets( op, e1, e2, TY_POINTER );
    CGDone( e1 );
}

static void genVthunkCDisp(     // generate virtual function thunk ctor-disp
    FN_CTL* fctl )              // - function information
{
    /* generated code:

        this -= ((unsigned *)this)[-1];
    */
    cg_name e1;
    cg_name e2;

    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e2 = CgOffset( TARGET_UINT );
    e1 = CGBinary( O_MINUS, e1, e2, TY_POINTER );
    e2 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGPreGets( O_MINUS, e1, e2, TY_POINTER );
    CGDone( e1 );
}

static void genVthunkVBase(     // generate virtual function thunk ctor-disp
    FN_CTL* fctl,               // - function information
    vindex index )              // - virtual base index
{
    /* generated code:

        this += *((unsigned **)this)[index];
    */
    cg_name e1;
    cg_name e2;

    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e1 = CGUnary( O_POINTS, e1, TY_POINTER );
    e2 = CgOffset( index * TARGET_UINT );
    e1 = CGBinary( O_PLUS, e1, e2, TY_POINTER );
    e2 = CGUnary( O_POINTS, e1, TY_UNSIGNED );
    e1 = CGFEName( (cg_sym_handle)fctl->this_sym, TY_POINTER );
    e1 = CGPreGets( O_PLUS, e1, e2, TY_POINTER );
    CGDone( e1 );
}

static cg_name accessAuto(      // get cg_name for an auto var
    CGFILE *file_ctl,           // - current file
    FN_CTL *fctl,               // - function information
    SYMBOL sym )                // - sym
{
    cg_name cgname = NULL;

    if( sym == file_ctl->opt_retn && CgRetnOptActive( fctl ) ) {
        if( file_ctl->u.s.opt_retn_val ) {
            cgname = CgSymbol( fctl->return_symbol );
        } else if( file_ctl->u.s.opt_retn_ref ) {
            cgname = IbpFetchRef( fctl->return_symbol );
        }
    } else {
        sym = SymTrans( sym );
        sym = finalAlias( sym );
        cgname = CgSymbol( sym );
    }
    return( cgname );
}

static void genAutoStaticInit(  // generate code to copy array into auto
    CGFILE *file_ctl,           // - current file
    FN_CTL *fctl,               // - function information
    SYMBOL dst,                 // - destination auto var
    SYMBOL src )                // - source static var
{
    cg_type type_refno;
    cg_name d1;
    cg_name s1;
    cg_name e1;

    type_refno = CgTypeOutput( src->sym_type );
    d1 = accessAuto( file_ctl, fctl, dst );
    s1 = CGFEName( (cg_sym_handle) src, type_refno );
    s1 = CGUnary( O_POINTS, s1, type_refno );
    e1 = CGLVAssign( d1, s1, type_refno );
    CGDone( e1 );
}

#if _CPU == 386
static void emitProfilingData(
    FN_CTL* fctl,               // - function information
    SYMBOL sym )                // - function symbol
{
    size_t len;
    uint_16 old_seg;
    back_handle fnh;
    VBUF data;

    if( TargetSwitches & NEW_P5_PROFILING ) return;
    if( TargetSwitches & P5_PROFILING ) {
        if( SymIsGennedComdatFun( sym ) ) {
            return;
        }
        FormatSym( sym, &data );
        old_seg = BESetSeg( SEG_PROF_REF );
        DbgVerify( 0 == ( 3 & DGTell() ), "P5 segment out of wack" );
        fnh = BENewBack( 0 );
        DGLabel( fnh );
        DGInteger( 0,   TY_INTEGER );
        DGInteger( -1,  TY_INTEGER );
        DGInteger( 0,   TY_INTEGER );
        DGInteger( 0,   TY_INTEGER );
        len = VbufLen( &data ) + 1;
        DGBytes( len, VbufString( &data ) );
        len &= 0x03;
        if( len ) {
            DGIBytes( 4 - len, 0 );
        }
        BESetSeg( old_seg );
        VbufFree( &data );
        fctl->prof_data = fnh;
    }
}

static void releaseProfilingData(
    FN_CTL* fctl )              // - function information
{
    back_handle fnh;

    fnh = fctl->prof_data;
    if( fnh != NULL ) {
        BEFiniBack( fnh );
        BEFreeBack( fnh );
    }
}
#else
#define emitProfilingData( f, s )
#define releaseProfilingData( f )
#endif

static FN_CTL* emit_virtual_file( // EMIT A VIRTUAL FILE
    CGFILE *file_ctl,           // - current file
    call_handle handle )        // - handle for call when gen'ing inline fun.
{
    register CGVALUE ins_value; // - value on intermediate-code instruction
    FN_CTL *fctl;               // - file control pointer
    CGINTER *ins;               // - next intermediate-code instruction
    unsigned ctr;               // - used as counter
    cg_type exprn_type;         // - current expression type
    cg_type lvalue_type;        // - current lvalue expression type
    cg_name op1;                // - operand [1]
    cg_name op2;                // - operand [2]
    cg_name op3;                // - operand [3]
    cg_type indexing_type;      // - type of array being indexed
    unsigned ptr_offset;        // - offset set by IC_DATA_PTR_OFFSET
    unsigned data_size;         // - size set by IC_DATA_SIZE
    segment_id curr_seg;        // - current segment
    cg_op cg_opcode;            // - opcode for code generator
    SRCFILE current_src;        // - current source file
    SYMBOL vf_exact_ind;        // - exact function for indirect virt. call
    unsigned ic_sp;             // - IC parm stack pointer
    CGVALUE ic_parms[IC_PARM_STACK_SIZE]; // - parm stack for complex ICs
    SYMBOL dtor_last_reqd;      // - dtor set by IC_SCOPE_CALL_CDTOR
    DTC_KIND dtor_kind;         // - DTC_... when ctor called
    label_handle lbl;           // - Label for IC_GOTO_NEAR

    static cg_op cg_opcodes[] ={// - opcodes for code generator
    #include "ppopscop.h"
    };

    CgioOpenInput( file_ctl );
    fctl = FnCtlPush( handle, file_ctl );
    vf_exact_ind = NULL;
    ic_sp = 0;
    dtor_kind = 0;
    dtor_last_reqd = NULL;
    for( ; ; ) {
        ins = CgioReadIC( file_ctl );
        if( ins->opcode == IC_EOF ) break;
        ExtraRptIncrementCtr( ctr_ic_codes );
        ins_value = ins->value;
        switch( ins->opcode ) {

//
//          CONTROL OPCODES
//

          case IC_DEF_SEG :                 // SET THE CURRENT SEGMENT
            curr_seg = (segment_id)ins_value.uvalue;
            BESetSeg( curr_seg );
            break;


//
//          LABELS
//


        {

          case IC_LABGET_CS :               // GET CONTROL-SEQUENCE LABELS
          { label_handle *lab;              // - next label handle
            for( ctr = ins_value.uvalue; ctr > 0; -- ctr ) {
                lab = VstkPush( &stack_labs_cs );
                *lab = BENewLabel();
                dump_label( printf( "LabCs Push %d %p\n"
                                  , VstkDimension( &stack_labs_cs )
                                  , *lab ) );
            }
          } break;

          case IC_LABFREE_CS :              // FREE CONTROL-SEQUENCE LABELS
            dump_label( printf( "LabCs Pop %d %d\n"
                              , VstkDimension( &stack_labs_cs )
                              , ins_value.uvalue
                              ) );
            CgLabelsPop( &stack_labs_cs, ins_value.uvalue );
            break;

          case IC_LABGET_GOTO :             // GET GOTO LABEL
          { label_handle *lab;              // - goto label
            lab = VstkPush( &stack_goto_near );
            *lab = BENewLabel();
            dump_label( printf( "LabGoTo Push %d %p\n"
                              , VstkDimension( &stack_goto_near )
                              , *lab ) );
          } break;

          case IC_LABDEF_CS :               // DEFINE CONTROL-SEQUENCE LABEL
          { label_handle *lab;              // - next label handle
            lab = VstkIndex( &stack_labs_cs
                           , fctl->base_labs_cs + ins_value.uvalue );
            CgLabel( *lab );
            dump_label( printf( "LabCs Def %d %p\n"
                              , fctl->base_labs_cs + ins_value.uvalue
                              , *lab ) );
          } break;

          case IC_LABDEF_GOTO :             // DEFINE GOTO LABEL
          { label_handle *lab;              // - goto label
            lab = VstkIndex( &stack_goto_near
                             , fctl->base_goto_near + ins_value.uvalue );
            CgLabel( *lab );
            dump_label( printf( "LabGoto Def %d %p\n"
                              , fctl->base_goto_near + ins_value.uvalue
                              , *lab ) );
          } break;

          case IC_LABEL_CS :                // SET CS LABEL FOR IC_GOTO_NEAR
          { label_handle *lab;              // - next label handle
            lab = VstkIndex( &stack_labs_cs, fctl->base_labs_cs + ins_value.ivalue );
            lbl = *lab;
            dump_label( printf( "LabCs Set %d %p\n", fctl->base_labs_cs + ins_value.uvalue, lbl ) );
          } break;

          case IC_LABEL_GOTO :              // SET GOTO LABEL FOR IC_GOTO_NEAR
          { label_handle *lab;              // - goto label
            lab = VstkIndex( &stack_goto_near, fctl->base_goto_near + ins_value.uvalue );
            lbl = *lab;
            dump_label( printf( "LabGoto Set %d %p\n", fctl->base_goto_near + ins_value.uvalue, lbl ) );
          } break;

          case IC_GOTO_NEAR :               // GOTO LABEL IN CURRENT ROUTINE
          { cg_type type;
            if( ( ins_value.uvalue == O_IF_TRUE ) || ( ins_value.uvalue == O_IF_FALSE ) ) {
                op1 = CgExprPopType( &type );
            } else {
                type = 0;
                op1 = NULL;
            }
            CgControl( ins_value.uvalue, op1, type, lbl );
            dump_label( printf( "Goto near %p %p\n", op1, lbl ) );
          } break;
        }
//
//          SWITCH STATEMENT
//

          case IC_SWITCH_BEG :              // SWITCH : START
          { label_handle select;
            select = CgSwitchBeg( fctl );
            CGControl( O_GOTO, NULL, select );
            dump_label( printf( "Goto switch beg %p\n", select ) );
          } break;

          case IC_SWITCH_CASE :             // SWITCH : CASE
            CgSwitchCase( ins_value.ivalue );
            break;

          case IC_SWITCH_OUTSIDE :          // SWITCH : DEFAULT (GENERATED)
          { label_handle *lab;              // - next label handle
            lab = VstkIndex( &stack_labs_cs
                           , fctl->base_labs_cs + ins_value.ivalue );
            CgSwitchDefaultGen( *lab );
          } break;

          case IC_SWITCH_DEFAULT :          // SWITCH : DEFAULT (CODED)
            CgSwitchDefault();
            break;

          case IC_SWITCH_END :              // SWITCH : END
            CgSwitchEnd();
            break;

//
//          EXPRESSIONS
//

          case IC_EXPR_TRASH :              // TRASH EXPRESSION
          { cg_name expr;                   // - expression
            cg_type type;                   // - expression type
            if( ! CgExprPopGarbage() ) {
                expr = CgExprPopType( &type );
                CgTrash( expr, type );
            }
          } break;

          case IC_EXPR_DONE :               // COMPLETE EXPRESSION
          { cg_name expr;                   // - expression
            cg_type type;                   // - expression type
            if( ! CgExprPopGarbage() ) {
                expr = CgExprPopType( &type );
                CgDone( expr, type );
            }
          } break;

          case IC_LEAF_CONST_INT :          // LEAF: INTEGER CONSTANT (01-32)
            CgExprPush( CGInteger( ins_value.ivalue, exprn_type )
                      , exprn_type );
            break;

          case IC_LEAF_CONST_INT64 :        // LEAF: INTEGER CONSTANT (33-64)
          { POOL_CON *con;                  // - constant in pool
            con = ins_value.pvalue;
            CgExprPush( CGInt64( con->u.int64_constant, exprn_type )
                      , exprn_type );
          } break;

          case IC_LEAF_CONST_FLT :          // LEAF: FLOATING-POINT CONSTANT
          { POOL_CON *con;                  // - constant in pool
            con = ins_value.pvalue;
            CgExprPush( CGFloat( con->u.s.fp_constant, exprn_type )
                      , exprn_type );
          } break;

          case IC_LEAF_CONST_STR :          // LEAF: STRING CONSTANT
          { back_handle handle;             // - back handle for literal
            handle = DgStringConst( ins_value.pvalue, NULL, DSC_CONST | DSC_CODE_OK );
            CgExprPush( CGBackName( handle, exprn_type ), exprn_type );
          } break;

          case IC_LEAF_NAME_FRONT :         // LEAF: FRONT-END SYMBOL
          { SYMBOL sym;
            cg_name cgname;
            sym = ins_value.pvalue;
            if( sym == NULL ) {
                sym = fctl->this_sym;
            }
            exprn_type = CgGetCgType( sym->sym_type );
            cgname = accessAuto( file_ctl, fctl, sym );
            CgExprPush( cgname, exprn_type );
          } break;

          case IC_SET_TYPE :                // SET TYPE FOR LEAVES, OPERATIONS
            exprn_type = CgExprType( ins_value.pvalue );
            break;

          case IC_EXPR_CONST :              // INDICATE EXPRESSION IS CONSTANT
            CgExprAttr( CG_SYM_CONSTANT );
            break;

          case IC_EXPR_VOLAT :              // INDICATE EXPRESSION IS VOLATILE
            CgExprAttr( CG_SYM_VOLATILE );
            break;

          case IC_LVALUE_TYPE :             // SET TYPE FOR LVALUE OPERATIONS
            lvalue_type = CgExprType( ins_value.pvalue );
            break;

          case IC_LEAF_THIS :               // LEAF: THIS POINTER REFERENCE
            CgExprPush( CGFEName( (cg_sym_handle)fctl->this_sym, exprn_type )
                      , exprn_type );
            break;

          case IC_LEAF_CDTOR_EXTRA :        // LEAF: EXTRA CTOR/DTOR INT PARM
            CgExprPush( CgSymbol( fctl->cdtor_sym ), CgTypeOffset() );
            break;

          case IC_CDARG_VAL :               // LEAF: EXTRA CTOR/DTOR PARAMETER
            CgCdArgDefine( ins_value.uvalue );
            break;

          case IC_CDARG_FETCH :             // GET FETCHED CTOR/DTOR VALUE
          { cg_name expr;                   // - expression for fetch
            if( fctl->has_cdtor_val ) {
                expr = CgOffset( fctl->cdtor_val );
            } else {
                expr = CgFetchSym( fctl->cdtor_sym );
            }
            CgExprPush( expr, CgTypeOffset() );
          } break;

          case IC_BIT_MASK :                // SET BIT MASK
          { TYPE type;      // - type for bit field
            type = ins_value.pvalue;
            CgExprPush( CGBitMask( CgExprPop()
                                 , type->u.b.field_start
                                 , type->u.b.field_width
                                 , exprn_type )
                      , exprn_type );
          } break;

          case IC_OPR_TERNARY :             // OPERATION : TERNARY
            op3 = CgExprPop();
            op2 = CgExprPop();
            op1 = CgExprPop();
            CgExprPush( CGChoose( op1, op2, op3, exprn_type ), exprn_type );
            break;

          case IC_OPR_BINARY :              // OPERATION : BINARY
            cg_opcode = cg_opcodes[ ins_value.uvalue ];
            if( O_COMMA == cg_opcode ) {
                if( CgExprPopGarbage() ) break;
                op2 = CgExprPop();
                CgCommaWithTopExpr( op2, exprn_type );
                break;
            } else {
                op2 = CgExprPop();
            }
            op1 = CgExprPop();
            switch( ins_value.uvalue ) {

              case CO_INIT :
              case CO_EQUAL :
                CgExprPush( CGLVAssign( op1, op2, exprn_type ), lvalue_type );
                break;

              case CO_PLUS_EQUAL :
              case CO_PERCENT_EQUAL :
              case CO_MINUS_EQUAL :
              case CO_DIVIDE_EQUAL :
              case CO_TIMES_EQUAL :
              case CO_AND_EQUAL :
              case CO_OR_EQUAL :
              case CO_XOR_EQUAL :
              case CO_LSHIFT_EQUAL :
              case CO_RSHIFT_EQUAL :
              case CO_BPRE_PLUS_PLUS :
              case CO_BPRE_BOOL_PLUS_PLUS :
              case CO_BPRE_MINUS_MINUS :
                CgExprPush( CGLVPreGets( cg_opcode, op1, op2, exprn_type ), lvalue_type );
                break;

              case CO_EQ :
              case CO_NE :
              case CO_LT :
              case CO_GE :
              case CO_GT :
              case CO_LE :
                CgExprPush( CGCompare( cg_opcode, op1, op2, exprn_type ), TY_BOOL );
                exprn_type = TY_BOOL;
                break;

              case CO_AND_AND :
              case CO_OR_OR :
                CgExprPush( CGFlow( cg_opcode, op1, op2 ), TY_BOOL );
                exprn_type = TY_BOOL;
                break;

              case CO_INDEX :
                CgExprPush( CGIndex( op1, op2, exprn_type, indexing_type ), lvalue_type );
                break;

              case CO_BPOST_PLUS_PLUS :
              case CO_BPOST_BOOL_PLUS_PLUS :
              case CO_BPOST_MINUS_MINUS :
                CgExprPush( CGPostGets( cg_opcode, op1, op2, exprn_type ), exprn_type );
                break;

              case CO_PTR_DELTA :
              { cg_name test;           // - test
                cg_name second;         // - second duplicate
                test =  CGCompare( O_NE
                                 , CgMakeTwoDups( &op1, &second, exprn_type )
                                 , CGInteger( 0, exprn_type )
                                 , exprn_type );
                op2 = CGBinary( O_PLUS, op1, op2, exprn_type );
                CgExprPush( CGChoose( test, op2, second, exprn_type ), exprn_type );
              } break;

#if _CPU == _AXP
              case CO_VASTART :
              { cg_name x;
                x = CGVarargsBasePtr( TY_POINTER );
                op1 = CGLVAssign( op1, x, TY_POINTER );
                x = CGInteger( TARGET_POINTER, TY_INTEGER );
                x = CGBinary( O_PLUS, op1, x, TY_POINTER );
                CgExprPush( CGAssign( x, op2, TY_INTEGER ), TY_INTEGER );
              } break;
#endif

              default :
                CgExprPush( CGBinary( cg_opcode, op1, op2, exprn_type ), exprn_type );
                break;
            }
            break;

          case IC_OPR_UNARY :               // OPERATION : UNARY
          { cg_type op_type;                // - type of operand
            cg_opcode = cg_opcodes[ ins_value.uvalue ];
            op1 = CgExprPopType( &op_type );
            switch( ins_value.uvalue ) {
              case CO_EXCLAMATION :
                CgExprPush( CGFlow( cg_opcode, op1, NULL ), TY_BOOL );
                exprn_type = TY_BOOL;
                break;
              case CO_UPLUS :
                CgExprPush( op1, exprn_type );
                break;
              case CO_FAR16_TO_POINTER:
                CgExprPush( CGUnary( O_PTR_TO_NATIVE, op1, TY_POINTER )
                          , TY_POINTER );
                break;
              case CO_POINTER_TO_FAR16:
                CgExprPush( CGUnary( O_PTR_TO_FOREIGN, op1, TY_POINTER )
                          , TY_POINTER );
                break;
              case CO_VOLATILE_TOP:
                CgExprPushWithAttr( op1, exprn_type, CG_SYM_VOLATILE );
                break;
              case CO_VOLATILE_UNDER_TOP:
                CgExprAttr( CG_SYM_VOLATILE );
                CgExprPush( op1, exprn_type );
                break;
              case CO_UNALIGNED_TOP:
                CgExprPushWithAttr( op1, exprn_type, CG_SYM_UNALIGNED );
                break;
              case CO_UNALIGNED_UNDER_TOP:
                CgExprAttr( CG_SYM_UNALIGNED );
                CgExprPush( op1, exprn_type );
                break;
              case CO_CONST_TOP:
                CgExprPushWithAttr( op1, exprn_type, CG_SYM_CONSTANT );
                break;
              case CO_CONST_UNDER_TOP:
                CgExprAttr( CG_SYM_CONSTANT );
                CgExprPush( op1, exprn_type );
                break;
              case CO_CONVERT :
                if( op_type == exprn_type ) {
                    CgExprPush( op1, exprn_type );
                    break;
                }
                // drops thru
              default :
                CgExprPush( CGUnary( cg_opcode, op1, exprn_type )
                          , exprn_type );
                break;
            }
          } break;

          case IC_COPY_OBJECT :             // COPY TO AN OBJECT
          { cg_name op1;                    // - object copied to
            cg_name op2;                    // - object copied from
            op2 = CgExprPop();
            op1 = CgExprPop();
            op1 = CGLVAssign( op1
                            , op2
                            , CgTypeOutput( ins_value.pvalue ) );
            CgExprPush( op1, TY_POINTER );
            exprn_type = TY_POINTER;
          } break;

          case IC_OPR_INDEX :               // INDEX : SET INDEX TYPE
            indexing_type = ins_value.ivalue;
            break;

          case IC_SEGOP_SEG :               // SEGMENT # (SET SEGMENT LABEL)
          { SYMBOL ref_var;                 // - reference symbol
            if( ins_value.uvalue == SEG_CODE ) {
                ref_var = fctl->func;
            } else {
                ref_var = SegmentLabelGen( ins_value.uvalue );
            }
            exprn_type = CgTypeSym( fctl->func );
            CgExprPush( CgSymbol( ref_var ), exprn_type );
          } break;

          case IC_SEGNAME :                 // __segname SUPPORT
          { cg_name ref;                    // - reference operand
            cg_type far_type;               // - type of pointer
            TYPE type;                      // - type of ref. variable
            fe_seg_id seg_id;               // - PC Segment ID
            seg_id = ((SYMBOL)ins_value.pvalue)->segid;
            SegmentLabelGen( seg_id );
            type = ((SYMBOL)ins_value.pvalue)->sym_type;
            if( FunctionDeclarationType( type ) == NULL ) {
                far_type = TY_LONG_POINTER;
            } else {
                far_type = TY_LONG_CODE_PTR;
            }
            ref = CGLVAssign( CGTempName( CGTemp( far_type ), far_type )
                            , CgSymbol( ins_value.pvalue )
                            , far_type );
            ref = CgOffsetExpr( ref
                              , TARGET_FAR_POINTER - TARGET_SHORT
                              , TY_POINTER );
            ref = CgFetchType( ref, TY_UINT_2 );
            CgExprPush( ref, TY_UINT_2 );
          } break;

          case IC_RARG_FETCH :              // FETCH A REFERENCE ARGUMENT
          { SYMBOL sym;                     // - symbol
            sym = transThisSym( ins_value.pvalue, fctl );
            CgExprPush( IbpFetchRef( sym ), CgTypePtrSym( sym ) );
          } break;

          case IC_RARG_SYM :                // SYMBOL FOR BOUND REF. ARG.
          { SYMBOL sym;                     // - symbol
            sym = ins_value.pvalue;
            if( ! CallStackTopInlined() ) break;
            if( CgRetnOptIsOptVar( fctl, sym ) ) {
                sym = fctl->return_symbol;
            } else {
                sym = transThisSym( sym, fctl );
            }
            IbpAdd( sym, 0, fctl );
          } break;

          case IC_RARG_PARM :               // REF. PARAMETER FOR BOUND REF.
          { SYMBOL bound;                   // - bound reference
            SYMBOL sym;                     // - current symbol
            target_offset_t offset;         // - offset of bound reference
            if( ! CallStackTopInlined() ) break;
            IbpReference( ins_value.pvalue, &sym, &bound, &offset );
            IbpAdd( bound, offset, fctl );
          } break;

          case IC_RARG_FUNC :               // ARG # FOR BOUND REF. ARG
            if( ! CallStackTopInlined() ) break;
            IbpDefineIndex( ins_value.uvalue );
            break;

          case IC_RARG_OFFSET :             // SET OFFSET FOR REFERENCE ARG.
            if( ! CallStackTopInlined() ) break;
            IbpDefineOffset( ins_value.uvalue );
            break;

          case IC_RARG_VBOFFSET :           // SET OFFSET FOR VB-REF ARG.
            if( ! CallStackTopInlined() ) break;
            IbpDefineVbOffset( ins_value.uvalue );
            break;

          case IC_EXPR_TS :                 // TYPE SIGNATURE REFERENCE
          { TYPE_SIG* sig;                  // - the type signature
            sig = BeTypeSignature( ins_value.pvalue );
            if( ! sig->cggen ) {
                sig_thunk_genned = TRUE;
            }
          } break;

          case IC_DTOR_REF :                // DESTRUCTOR REFERENCE
            break;

          case IC_RESET_THIS :              // RESET THIS, IF REQUIRED
            cgResetThis( ins_value.uvalue );
            break;

#if _CPU == _AXP
          case IC_ALLOCA :                  // ALLOCA SUPPORT
          { cg_name expr;                   // - top expression
            expr = CgExprPop();
            expr = CGUnary( O_STACK_ALLOC, expr, TY_POINTER );
            CgExprPush( expr, TY_POINTER );
          } break;
#endif

//
//          FUNCTION CALLING
//

          case IC_RETNOPT_BEG :             // START CONDITIONAL RETURN CODE
            CgRetnOpt_RETNOPT_BEG( fctl );
            break;

          case IC_RETNOPT_END :             // END CONDITIONAL RETURN CODE
            break;

          case IC_RETNOPT_VAR :             // CONDITIONAL FOR ! OPT VAR
            CgRetnOpt_RETNOPT_VAR( fctl, ins_value.pvalue );
            break;

          case IC_PROC_RETURN :             // RETURN FROM PROCEDURE
          { SE* curr;                       // - current position
#if 0
            if( ins_value.pvalue != NULL ) {
                fctl->return_symbol = SymTrans( ins_value.pvalue );
            }
#endif
            CgGotoReturnLabel( fctl );
            if( fctl->coded_return ) {
                fctl->coded_return = FALSE;
                fctl->ctor_complete = FALSE;
            }
            curr = BlkPosnCurr();
            FstabSetSvSe( curr );
          } break;

          case IC_CALL_SETUP :              // SETUP FUNCTION CALL
          { SYMBOL inlined;                 // - NULL or inlined func
            SYMBOL func;                    // - function called
            func = ins_value.pvalue;
            inlined = CgBackFuncInlined( func ) ? func : NULL;
            CallStackPush( inlined
                         , CGInitCall( CgExprPop()
                                     , exprn_type
                                     , (cg_sym_handle)func )
                         , exprn_type );
            if( ! CompFlags.has_longjmp
             && SPFN_LONGJMP == SpecialFunction( func ) ) {
                CompFlags.has_longjmp = TRUE;
            }
          } break;

          case IC_CALL_EXEC :               // EXECUTE FUNCTION CALL
          { call_handle handle;             // - handle for call
            cg_type retn_type;              // - return type
//            CALL_STAB* call_entry;          // - entry for call
            retn_type = CallStackRetnType();
            handle = CallStackPop();
//            call_entry = CgBackCallGened( handle );
            CgBackCallGened( handle );
            dtor_last_reqd = NULL;
            dtor_kind = 0;
            CgExprPush( CgFetchType( CGCall( handle ), retn_type ), exprn_type );
          } break;

          case IC_CALL_SETUP_IND :          // SETUP INDIRECT FUNCTION CALL
          { SYMBOL feedback;                // - feedback entry
            SYMBOL inlined;                 // - NULL or inlined fuction
            feedback = CallIndirectPush( ins_value.pvalue );
            if( vf_exact_ind == NULL ) {
                inlined = NULL;
            } else {
                inlined = CgBackFuncInlined( vf_exact_ind )
                                ? vf_exact_ind : NULL;
                feedback = vf_exact_ind;
                vf_exact_ind = NULL;
            }
            CallStackPush( inlined
                         , CGInitCall( CgExprPop()
                                     , exprn_type
                                     , (cg_sym_handle)feedback )
                         , exprn_type );
          } break;

          case IC_CALL_EXEC_IND :           // EXECUTE INDIRECT FUNCTION CALL
          { call_handle handle;             // - handle for call
            cg_type retn_type;              // - return type
            retn_type = CallStackRetnType();
            handle = CallStackPop();
            CgExprPush( CgFetchType( CGCall( handle ), retn_type )
                      , exprn_type );
            CallIndirectPop();
            CgCdArgRemove( handle );
          } break;

          case IC_CALL_PARM_FLT:            // SET float_used ...
            CompFlags.float_used = TRUE;
            // fall thru
          case IC_CALL_PARM :               // PARAMETER FOR CALL
            CGAddParm( CallStackTopHandle(), CgExprPop(), exprn_type );
            break;

//
//          INITIALIZATION (CODE AND DATA)
//

          case IC_INIT_BEG :                // START AN INITIALIZATION
          { SYMBOL sym;
            sym = ins_value.pvalue;
            if( ( file_ctl->symbol != NULL )
              &&( sym->id == SC_STATIC )
              &&( file_ctl->u.s.stgen ) ) {
                flushOverInitialization( file_ctl );
            } else if( CgDeclSkippableConstObj( sym ) ) {
                flushOverInitialization( file_ctl );
            } else {
                fctl->pre_init = FstabCurrPosn();
            }
          } break;

          case IC_INIT_TEST :               // START FUNCTION INIT. TEST
          { SYMBOL sym;
            sym = ins_value.pvalue;
            if( CgDeclSkippableConstObj( sym ) ) {
                flushOverInitialization( file_ctl );
            }
          } break;

          case IC_INIT_AUTO :               // COPY STATIC INIT BLOCK INTO AUTO
          { SYMBOL src;
            SYMBOL dst = ins_value.pvalue;
            IC_PARM_POP_PTR( src );
            IC_PARM_DONE;
            DbgAssert( CgMemorySize( src->sym_type ) != 0 );
            DbgAssert( CgMemorySize( dst->sym_type ) != 0 );
            DbgAssert( CgMemorySize( src->sym_type ) <= CgMemorySize( dst->sym_type ) );
            genAutoStaticInit( file_ctl, fctl, dst, src );
          } break;

          case IC_INIT_DONE :               // END AN INITIALIZATION
            break;


//
//          DATA GENERATION
//

          case IC_DATA_SYMBOL :             // GENERATE DATA FOR SYMBOL
          { SYMBOL sym;
            sym = ins_value.pvalue;
            if( ! SymIsConstant( sym ) || SymIsReferenced( sym ) ) {
                DgSymbolDefInit( sym );
            }
            CgExprType( sym->sym_type );
          } break;

          case IC_DATA_LABEL :              // PLANT A DATA LABEL
          { SYMBOL sym;
            segment_id sym_seg;
            sym = ins_value.pvalue;
            sym_seg = FESegID( sym );
            if( sym_seg > 0 ) {
                curr_seg = sym_seg;
                BESetSeg( curr_seg );
                DgAlignSymbol( sym );
                DGLabel( FEBack( sym ) );
                if( curr_seg == SEG_BSS ) {
                    DgUninitBytes( CgMemorySize( sym->sym_type ) );
                }
            }
          } break;

          case IC_DATA_SEG :                // INCREMENT THE CURRENT SEGMENT
            if( curr_seg != SEG_BSS ) {
                BESetSeg( ++curr_seg );     // new segment
            }
            break;

          case IC_DATA_PTR_OFFSET :         // SET OFFSET OF POINTER
            ptr_offset = ins_value.uvalue;
            break;

          case IC_DATA_PTR_SYM :            // GENERATE POINTER FOR SYMBOL
            if( curr_seg != SEG_BSS ) {
                DGFEPtr( (cg_sym_handle)ins_value.pvalue, exprn_type, ptr_offset );
            }
            break;

          case IC_DATA_PTR_STR :            // DATA: STRING CONSTANT
            if( curr_seg != SEG_BSS ) {
                back_handle handle;         // - back handle for literal
                uint_16 str_seg;            // - string segment
                handle = DgStringConst( ins_value.pvalue, &str_seg, DSC_CONST );
                BESetSeg( curr_seg );
                DGBackPtr( handle, str_seg, ptr_offset, exprn_type );
            }
            break;

          case IC_DATA_INT :                // GENERATE INTEGER (1-32 BITS)
            if( curr_seg != SEG_BSS ) {
                DGInteger( ins_value.uvalue, exprn_type );
            } else {
                DbgVerify( ins_value.uvalue == 0
                         , "CGBACK - IC_DATA_INT non-zero in SEG_BSS" );
            }
            break;

          case IC_DATA_INT64 :              // GENERATE INTEGER (33-64 BITS)
          { POOL_CON *con;                  // - constant in pool
            con = ins_value.pvalue;
            DbgVerify( con->i64, "NON INT-64 CONSTANT" );
            if( curr_seg != SEG_BSS ) {
                DGInteger64( con->u.int64_constant, exprn_type );
            } else {
                DbgVerify( Zero64( &con->u.int64_constant ), "CGBACK - IC_DATA_INT64 non-zero in SEG_BSS" );
            }
          } break;

          case IC_DATA_FLT :                // GENERATE FLOATING-POINT
          { POOL_CON *con;                  // - constant in pool
            con = ins_value.pvalue;
            DbgVerify( con->flt, "NON FLOAT CONSTANT" );
            if( curr_seg != SEG_BSS ) {
                DGFloat( con->u.s.fp_constant, exprn_type );
            }
          } break;

          case IC_DATA_SIZE :               // SET DATA SIZE
            data_size = ins_value.uvalue;
            break;

          case IC_DATA_TEXT :               // GENERATE TRANSLATABLE TEXT
            if( curr_seg != SEG_BSS ) {
                DgStringConst( ins_value.pvalue, NULL, DSC_NULL );
            }
            break;

          case IC_DATA_REPLICATE :          // REPLICATE BYTES
            if( curr_seg != SEG_BSS ) {
                DgInitBytes( data_size, ins_value.uvalue );
            } else {
                DbgVerify( ins_value.uvalue == 0
                         , "CGBACK - IC_DATA_REPLICATE non-zero in SEG_BSS" );
            }
            break;

          case IC_DATA_UNDEF :              // GENERATE UNDEFINED BYTES
            if( curr_seg != SEG_BSS ) {
                DgInitBytes( ins_value.uvalue, 0 );
            }
            break;

//
//          PROCEDURE DECLARATIONS
//

          case IC_FUNCTION_OPEN :           // OPEN FUNCTION SCOPE
          { SYMBOL sym;                     // - function symbol
            sym = ins_value.pvalue;
            if( depth_inline == 0 ) {
                if( CompFlags.progress_messages ) {
                    CppStartFuncMessage( sym );
                }
                SegmentCgDefineCode( sym->segid );
            }
            fctl->func = sym;
            fctl->is_ctor = FALSE;
            fctl->is_dtor = FALSE;
            if( SymIsDtor( sym ) ) {
                fctl->is_dtor = TRUE;
            } else if( SymIsCtor( sym ) ) {
                fctl->is_ctor = TRUE;
            }
            CtxFunction( sym );
            exprn_type = CgFuncRetnType( sym );
            CGProcDecl( (cg_sym_handle)sym, CgReturnType( exprn_type ) );
            if( depth_inline == 0 ) {
                emitProfilingData( fctl, sym );
            }
            if( fctl->debug_info
             && ( GenSwitches & DBG_LOCALS ) ) {
                DBModSym( (cg_sym_handle)sym, TY_DEFAULT );
            }
            BlkPosnPush( NULL );
            fctl->try_label = UNDEFINED_LABEL;
            fctl->try_depth = 0;
          } break;

          case IC_FUNCTION_ARGS :           // DEFINE FUNCTION ARG.S
            if( SymIsThisFuncMember( fctl->func ) ) {
                TYPE ftype = MakePointerTo( SymClass( fctl->func ) );
                fctl->this_sym
                    = CgDeclHiddenParm( ins_value.pvalue
                                      , ftype
                                      , thisSym
                                      , SPECIAL_THIS );
            }
            if( SymCDtorExtraParm( fctl->func ) ) {
                fctl->cdtor_sym
                    = CgDeclHiddenParm( ins_value.pvalue
                                      , MakeCDtorExtraArgType()
                                      , cdtorSym
                                      , SPECIAL_CDTOR_EXTRA );
            }
            CgDeclParms( fctl, ins_value.pvalue );
            funcDebugInfo( fctl );
            break;

          case IC_FUNCTION_RETN :           // SET RETURN SYMBOL
            fctl->return_symbol = SymTrans( ins_value.pvalue );
            break;

          case IC_FUNCTION_DTM :            // SET DTOR METHOD
          { DT_METHOD dtm;                  // - function dtor method
            dtm = ins_value.uvalue;
            if( ! file_ctl->u.s.state_table ) {
                dtm = DTM_DIRECT;
            } else if( ! file_ctl->u.s.stab_gen ) {
                dtm = DtmDirect( dtm );
            }
            fctl->dtor_method = dtm;
            fctl->func_dtor_method = dtm;
            if( FstabSetup( file_ctl, fctl ) ) {
                sig_thunk_genned = TRUE;
            }
            fctl->state_table_bound = FstabCurrPosn();
            if( fctl->is_dtor ) {
                fctl->dtor_components = fctl->state_table_bound;
            } else if( fctl->is_ctor ) {
                setupCtorOtab( fctl );
            }
            BlkPosnUpdate( fctl->state_table_bound );
          } break;

          case IC_FUNCTION_STAB :           // SET UP FUNCTION STATE-TABLE
            fctl->cond_flags = ins_value.uvalue;
            if( file_ctl->u.s.ctor_test ) {
                fctl->has_ctor_test = TRUE;
            }
            break;

          case IC_FUNCTION_EPILOGUE :       // START FUNCTION EPILOGUE
            CgLabelPlantReturn( fctl );
            break;

          case IC_CTOR_COMPLETE :           // CTOR COMPLETED (BEFORE RETURN)
            fctl->coded_return = TRUE;
            // drops thru

          case IC_CTOR_END :                // CTOR COMPLETED (AFTER CODE)
            fctl->ctor_complete = TRUE;
            break;

          case IC_CTOR_CODE :               // CTOR: START USER CODE
            fctl->ctor_components = FstabCurrPosn();
            break;

          case IC_DTOR_REG :                // REGISTER DTOR FUNCTION
            setupDtorOtab( fctl );
            fctl->state_table_bound = FstabCurrPosn();
            FstabRegister( fctl );
            break;

          case IC_DTOR_DEREG :              // DE-REGISTER DTOR FUNCTION
            FstabDeRegister( fctl );
            break;

          case IC_DTOR_DLT_BEG :            // START OF DTOR-DELETION CODE
            if( fctl->has_cdtor_val ) {
                if( (fctl->cdtor_val & DTOR_DELETE_THIS) == 0 ) {
                    CgioReadICUntilOpcode( file_ctl, IC_DTOR_DLT_END );
                } else if( fctl->cdtor_val & DTOR_DELETE_VECTOR ) {
                    CgioReadICUntilOpcode( file_ctl, IC_DTOR_DLT_END );
                }
            }
            break;

          case IC_DTOR_DLT_END :            // END OF DTOR-DELETION CODE
            // just a marker in the file
            break;

          case IC_DTOR_DAR_BEG :            // START OF DTOR-DELETION CODE
            if( fctl->has_cdtor_val ) {
                if( (fctl->cdtor_val & DTOR_DELETE_VECTOR) == 0 ) {
                    CgioReadICUntilOpcode( file_ctl, IC_DTOR_DAR_END );
                }
            }
            break;

          case IC_DTOR_DAR_END :            // END OF DTOR-DELETION CODE
            // just a marker in the file
            break;

          case IC_CTOR_COMP_BEG :           // START OF CTOR-COMPONENTRY CODE
            if( fctl->has_cdtor_val ) {
                if( ( fctl->cdtor_val & CTOR_COMPONENT ) ) {
                    CgioReadICUntilOpcode( file_ctl, IC_CTOR_COMP_END );
                }
            }
            break;

          case IC_CTOR_COMP_END :           // END OF CTOR-COMPONENTRY CODE
            // just a marker in the file
            break;

          case IC_FUNCTION_CLOSE :          // CLOSE FUNCTION SCOPE
          { SYMBOL retn_sym;
            cg_name cgname;
            cg_type cgtype;

            DbgAssert( fctl->try_depth == 0 );
            DbgAssert( fctl->try_label == UNDEFINED_LABEL );
            if( NULL != fctl->obj_registration ) {
                ObjInitPop();
            }
            CgLabelPlantReturn( fctl );
            if( 0 != depth_inline
             && fctl->marked_at_start != FstabMarkedPosn() ) {
                FstabAssignStateVar( fctl->marked_at_start );
            }
            BlkPosnTrash();
            CgLabelsFinish( &stack_goto_near, fctl->base_goto_near );
            CgLabelsFinish( &stack_labs_cs, fctl->base_labs_cs );
            file_ctl->u.s.stgen = TRUE;
            DbgVerify( depth_inline != 0 || IbpEmpty(), "ibrps unfreed" );
            FstabDeRegister( fctl );
            retn_sym = fctl->return_symbol;
            if( retn_sym == NULL ) {
                if( SpecialFunction( fctl->func ) == SPFN_MAIN ) {
                    // C++ requires main() to return 0 if nothing returned
                    cgname = CGInteger( 0, TY_INTEGER );
                    cgtype = TY_INTEGER;
                } else {
                    cgname = NULL;
                    cgtype = CgTypeOutput( GetBasicType( TYP_VOID ) );
                }
            } else {
                exprn_type = CgFuncRetnType( fctl->func );
                if( file_ctl->u.s.opt_retn_ref
                 && CgRetnOptActive( fctl ) ) {
                    cgname = IbpFetchRef( retn_sym );
                } else {
                    cgname = CgFetchType( CgSymbol( retn_sym ), exprn_type );
                }
                cgtype = CgReturnType( exprn_type );
            }
            CGReturn( cgname, cgtype );
            if( depth_inline == 0 ) {
                releaseProfilingData( fctl );
            }
          } break;

          case IC_EXPR_TEMP :               // START EXPRESSION WITH TEMPS
            if( fctl->cond_flags > 0 ) {
                FnCtlCondFlagExpr( fctl );
            }
            BlkPosnTempEndSet( FstabActualPosn() );
            break;

          case IC_DESTRUCT :                // DESTRUCT UNTIL SCOPE
          { SE* se_bound;                   // - SE for bounding var.
            if( ins_value.pvalue == NULL ) {
                if( fctl->is_ctor ) {
                    se_bound = fctl->ctor_components;
                } else {
                    se_bound = fctl->state_table_bound;
                }
            } else {
                se_bound = BlkPosnScope( ins_value.pvalue );
            }
            cgDestruct( se_bound, fctl );
          } break;

          case IC_DESTRUCT_VAR :            // DESTRUCT UNTIL VARIABLE
          { SYMBOL bound;                   // - bounding symbol
            SE* se_bound;                   // - SE for bounding var.
            SE* save_state;                 // - current state
            save_state = FstabCurrPosn();
            if( ins_value.pvalue == NULL ) {
                if( fctl->is_ctor ) {
                    se_bound = fctl->ctor_components;
                } else {
                    se_bound = fctl->state_table_bound;
                }
            } else {
                bound = SymTrans( ins_value.pvalue );
                se_bound = FstabFindAuto( bound );
            }
            cgDestruct( se_bound, fctl );
            FstabSetSvSe( save_state );
          } break;

          case IC_DTOBJ_PUSH :              // DTORABLE OBJECT: START
//          { OBJ_INIT* init;                 // - new initialization object
//            init = ObjInitPush( ins_value.pvalue );
          {                                 // - new initialization object
            ObjInitPush( ins_value.pvalue );
          } break;

          case IC_DTOBJ_SYM :               // DTORABLE OBJECT: SYMBOL
          { OBJ_INIT* init;                 // - top initialization object
            init = ObjInitTop();
            init->sym = SymTrans( ins_value.pvalue );
          } break;

          case IC_DTOBJ_OFF :               // DTORABLE OBJECT: OFFSET
          { OBJ_INIT* init;                 // - top initialization object
            SE* se;                         // - state entry for object
            TYPE array_element;             // - type of array element
            init = ObjInitTop();
            init->offset = ins_value.uvalue;
            array_element = ObjInitArrayBaseType( init );
            if( NULL == array_element ) {
                init->defn = buildObjectStateTable( init->type );
            } else {
                init = ObjInitArray();
                if( init->se == NULL ) {
                    fctl->pre_init = FstabCurrPosn();
                    se = SeAlloc( DTC_ARRAY_INIT );
                    se->array_init.reg = NULL;
                    init->se = se;
                    se = FstabAdd( se );
                    if( DtmTabular( fctl ) ) {
                        cg_name e1;                 // - expression(1)
                        cg_name e2;                 // - expression(2)
                        e1 = FstabEmitStateVar( se, fctl );
                        e2 = ObjInitAssignIndex( fctl, init, 0 );
                        e2 = CgComma( e1, e2, TY_POINTER );
                        e1 = CGLVAssign
                                ( CgSymbolPlusOffset( init->reg->sym, CgbkInfo.size_data_ptr )
                                , CgSymbol( BeTypeSignature( array_element )
                                             ->sym )
                                , TY_POINTER );
                        e2 = CgComma( e1, e2, TY_POINTER );
                        e1 = ObjInitAssignBase( fctl, init );
                        e2 = CgComma( e1, e2, TY_POINTER );
                        CgCommaBefore( e2, TY_POINTER );
                        se->array_init.reg = init->reg;
                    }
                }
            }
          } break;

          case IC_DTOBJ_POP :               // DTORABLE OBJECT: POP IT
            ObjInitPop();
            break;

          case IC_DTARRAY_INDEX :           // CTOR'ED ARRAY ELEMENT
            if( DtmTabular( fctl ) ) {
                OBJ_INIT* init;             // - top initialization object
//                SE* se;                     // - stacked entry
                cg_name expr;               // - expression pushed
                init = ObjInitArray();
//                se = init->se;
                expr = ObjInitAssignIndex( fctl, init, ins_value.uvalue + 1 );
                CgExprPush( expr, TY_POINTER );
            } else {
                CgPushGarbage();
            }
            break;

          case IC_BLOCK_OPEN :              // OPEN BLOCK SCOPE (LIVE CODE)
          { SCOPE scope;                    // - scope to be opened
            scope = ins_value.pvalue;
            BlkPosnPush( scope );
            if( scope != NULL ) {
                scope->u.s.dtor_reqd = FALSE;
                if( fctl->debug_info
                 && ( GenSwitches & DBG_LOCALS )
                 && ScopeDebugable( scope ) ) {
                    DBBegBlock();
                }
                ScopeWalkOrderedSymbols( scope, &CgDeclSym );
            }
          } break;

          case IC_BLOCK_DEAD :              // OPEN BLOCK SCOPE (DEAD CODE)
          { SCOPE scope;                    // - scope to be opened
            scope = ins_value.pvalue;
            BlkPosnPush( scope );
            if( scope != NULL ) {
                if( fctl->debug_info
                 && ( GenSwitches & DBG_LOCALS )
                 && ScopeDebugable( scope ) ) {
                    DBBegBlock();
                }
                ScopeWalkOrderedSymbols( scope, &CgDeclSym );
            }
          } break;

          case IC_BLOCK_CLOSE :             // CLOSE BLOCK SCOPE
          { SE* se;                         // - current position
            se = BlkPosnEnclosing();
            cgDestruct( se, fctl );
          } break;

          case IC_BLOCK_END :               // END SIGNIFICANT BLOCK SCOPE
          { SCOPE scope;                    // - scope to be ended
            scope = ins_value.pvalue;
            if( fctl->debug_info
             && ( GenSwitches & DBG_LOCALS )
             && ScopeDebugable( scope ) ) {
                DBEndBlock();
            }
          } // drops thru

          case IC_BLOCK_DONE :              // END INSIGNIFICANT SCOPE
            BlkPosnPop();
            break;

          case IC_BLOCK_SRC :               // SET SOURCE SCOPE
            scope_exit = ins_value.pvalue;
            break;

          case IC_INIT_SYM_BEG :    // START OF SYMBOL INITIALIZATION
            fctl->pre_init = FstabCurrPosn();
            break;

          case IC_INIT_SYM_END :    // END OF SYMBOL INITIALIZATION
          { SYMBOL sym = SymTrans( ins_value.pvalue );
            if( ArrayType( sym->sym_type ) ) {
               FstabSetSvSe( fctl->pre_init );
            }
            ObjInitDtorAuto( fctl->pre_init, sym );
          } break;

          case IC_DTOR_AUTO :       // NON-TEMPORARY AUTO SYMBOL NEEDS DTOR
          { SE* se;                 // - state entry for non-temp
            cg_name top_expr;       // - top expression
            cg_type top_type;       // - top type
            se = dtorAutoSymbol( fctl, ins_value.pvalue );
            fctl->ctored_obj = se;
            top_expr = CgExprPopType( &top_type );
            top_expr = CgCallBackAutoCtor( top_expr, top_type, se );
            CgExprPush( top_expr, top_type );
          } break;

          case IC_DTOR_TEMP :       // TEMPORARY AUTO SYMBOL NEEDS DTOR
          { SE* se;                 // - state entry for temp
            cg_name top_expr;       // - top expression
            cg_type top_type;       // - top type
            fctl->temp_dtoring = TRUE;
            se = dtorAutoSymbol( fctl, ins_value.pvalue );
            top_expr = CgExprPopType( &top_type );
            top_expr = CgCallBackTempCtor( top_expr, top_type, se );
            CgExprPush( top_expr, top_type );
          } break;

          case IC_GEN_CTOR_DISP :   // GENERATE CTOR-DISP INIT CODE
            genCtorDispInitCode( fctl, ins_value.pvalue );
            break;

          case IC_EXACT_VPTR_INIT : // GENERATE EXACT OFFSET VPTR INIT CODE
          { SYMBOL table_sym;
            bool vbptr;
            IC_PARM_POP_PTR( table_sym );
            IC_PARM_POP_INT( vbptr );
            IC_PARM_DONE;
            genExactVPtrInit( fctl, ins_value.uvalue, table_sym, vbptr );
          } break;

          case IC_VBASE_VPTR_INIT : // GENERATE VBASE OFFSET VPTR INIT CODE
          { SYMBOL table_sym;
            target_offset_t vb_offset;
            vindex vb_index;
//            target_offset_t exact_delta;
            bool vbptr;
//            IC_PARM_POP_INT( exact_delta );
            IC_PARM_SKIP;
            IC_PARM_POP_INT( vb_index );
            IC_PARM_POP_INT( vb_offset );
            IC_PARM_POP_PTR( table_sym );
            IC_PARM_POP_INT( vbptr );
            IC_PARM_DONE;
            genVBaseVPtrInit( fctl, vb_offset, vb_index, ins_value.uvalue, table_sym, vbptr );
          } break;

          case IC_DTOR_STATIC :     // STATIC SYMBOL NEEDS DTOR
            fctl->pre_init = FstabCurrPosn();
            CompFlags.genned_static_dtor = TRUE;
            CgCommaOptional( CgDtorStatic( ins_value.pvalue ), TY_POINTER );
            break;

          // values:
          // 0 - ignore
          // 1 - set flag
          // 2 - reset flag
          case IC_COND_TRUE :       // START OF CONDITIONAL TRUE BLOCK
            if( ins_value.uvalue ) {
                CondInfoPush( fctl );
                CondInfoSetFlag( fctl, ins_value.uvalue == 1 );
                CondInfoTrue();
            } else {
                CgPushGarbage();
            }
            break;

          // values:
          // 0 - ignore
          // 1 - reset flag
          case IC_COND_FALSE :      // START OF CONDITIONAL FALSE BLOCK
            if( ins_value.uvalue ) {
                CondInfoSetFlag( fctl, FALSE );
                CondInfoFalse();
            } else {
                CgPushGarbage();
            }
            break;

          // values:
          // 0 - don't set flag
          // 1 - set flag
          case IC_COND_END :        // END OF CONDITIONAL BLOCK
            if( ins_value.uvalue ) {
                cg_name old_expr;
                cg_type old_type;
                if( CgExprPopGarbage() ) {
                    old_expr = NULL;
                } else {
                    old_expr = CgExprPopType( &old_type );
                }
                CondInfoSetFlag( fctl, TRUE );
                if( old_expr != NULL ) {
                    CgCommaWithTopExpr( old_expr, old_type );
                }
            }
            CondInfoEnd();
            CondInfoPop();
            break;

        {
            cg_name set_expr;       // expression for set

          case IC_INIT_REF_BEG :    // START OF DTORABLE REF INIT
          { SE* se;                 // - state entry
            se = dtorAutoSymbol( fctl, ins_value.pvalue );
            se = FstabAdd( se );
            se = BlkPosnUpdate( se );
            fctl->pre_init = se;
            se = BlkPosnTempEndSet( se );
            set_expr = CgCallBackInitRefBeg( se );
          } break;

          case IC_INIT_REF_END :    // END OF DTORABLE REF INIT
          { cg_name top_expr;       // - top expression
            cg_type top_type;       // - top type
            BlkPosnUpdate( fctl->pre_init );
            BlkPosnTempEndSet( fctl->pre_init );
            top_expr = CgExprPopType( &top_type );
            top_expr = CgComma( set_expr, top_expr, top_type );
            top_expr = CgCallBackCtorDone( top_expr
                                         , top_type
                                         , fctl->pre_init );
            if( ! fctl->temp_dtoring ) {
                // when there are temps, state var will be set
                // by the temp-destruction process
                cg_name sv_expr;
                sv_expr = FstabEmitStateVar( fctl->pre_init, fctl );
                if( NULL != sv_expr ) {
                    top_expr = CgComma( top_expr, sv_expr, TY_POINTER );
                    top_type = TY_POINTER;
                }
            }
            CgExprPush( top_expr, top_type );
          } break;

        }

//
//          Virtual Function reference with inlined args
//
        {
            bool vf_call;               // TRUE ==> virtual call gen'ed
            target_offset_t vf_offset;  // offset to virtual function ptr
            vindex vf_index;            // index for virtual function
            target_offset_t vf_adj_this;// adjustment for "this" on virt call
            target_offset_t vf_adj_retn;// adjustment for return on virt call
            SYMBOL vf_this;             // this for function (bound ?)
            cg_name vf_ptr;             // pre-computation of vf_ptr

          case IC_CALL_EXEC_VFUN :      // EXECUTE VIRTUAL FUNCTION CALL
          { call_handle handle;         // - handle for call
            target_offset_t retn_adj;   // - return adjustment
            cg_name expr;               // - expression under construction
            cg_type retn_type;          // - return type
            retn_type = CallStackRetnType();
            retn_adj = CallStackRetnAdj();
            handle = CallStackPop();
            CgBackCallGened( handle );
            expr = CgFetchType( CGCall( handle ), retn_type );
            if( retn_adj != 0 ) {
                expr = CgOffsetExpr( expr, retn_adj, exprn_type );
            }
            CgExprPush( expr, exprn_type );
            CallIndirectPop();
          } break;


          case IC_VF_THIS_ADJ :     // ADJUST "THIS" FOR VIRTUAL CALL
          { target_offset_t adjust; // - adjustment for this (subtracted)
            cg_name expr;           // - current expression
            cg_type type;           // - current type
            adjust = CallStackThisAdj();
            if( adjust != 0 ) {
                expr = CgExprPopType( &type );
                expr = CgOffsetExpr( expr, -adjust, type );
                CgExprPush( expr, type );
            }
          } break;

          case IC_VF_CODED :        // VIRTUAL CALL HAS BEEN CODED
            vf_call = TRUE;
            vf_adj_this = 0;
            vf_adj_retn = 0;
            vf_ptr = NULL;
            break;

          case IC_VIRT_FUNC :       // MARK INDIRECT AS VIRTUAL CALL
            CallIndirectVirtual( ins_value.pvalue
                               , vf_call
                               , vf_adj_this
                               , vf_adj_retn );
            break;

          case IC_VF_OFFSET :       // SET vf_offset
            vf_offset = ins_value.uvalue;
            break;

          case IC_VF_INDEX :        // SET vf_index
            vf_index = ins_value.uvalue;
            break;

          case IC_VF_THIS :         // SET vf_this
            vf_this = ins_value.pvalue;
            break;

          case IC_VFUN_PTR :        // SET vfun ptr.
            vf_ptr = CgExprPop();
            CgPushGarbage();
            break;

          case IC_SETUP_VFUN :      // SET UP CALL TO VFUN
          { cg_name op;             // - operand
            op = IbpFetchVfRef( ins_value.pvalue
                              , vf_ptr
                              , vf_this
                              , vf_offset
                              , vf_index
                              , &vf_call
                              , &vf_adj_this
                              , &vf_adj_retn
                              , &vf_exact_ind );
            CgExprPush( op, exprn_type );
          } break;

        }

//
//          Virtual Base reference with inlined args
//
        {
            target_offset_t vb_exact;   // exact for virtual base
            target_offset_t vb_delta;   // delta for virtual base
            target_offset_t vb_offset;  // offset to virtual base ptr.
            vindex          vb_index;   // index for virtual base

          case IC_VB_EXACT :        // SET vb_exact
            vb_exact = ins_value.uvalue;
            break;

          case IC_VB_DELTA :        // SET vb_delta
            vb_delta = ins_value.uvalue;
            break;

          case IC_VB_OFFSET :       // SET vb_offset
            vb_offset = ins_value.uvalue;
            break;

          case IC_VB_INDEX :        // SET vb_index
            vb_index = ins_value.uvalue;
            break;

          case IC_VB_FETCH :        // FETCH VBASE PTR
          { cg_name op;             // - operand
            op = IbpFetchVbRef( ins_value.pvalue
                              , vb_delta
                              , vb_exact
                              , vb_offset
                              , vb_index );
            CgExprPush( op, exprn_type );
          } break;

        }

//
//          TEST OF EXTRA ARG
//

          case IC_CDARG_TEST_ON :           // TEST IF ON
            cdArgTest( fctl, ins_value.uvalue, TRUE );
            break;

          case IC_CDARG_TEST_OFF :          // TEST IF OFF
            cdArgTest( fctl, ins_value.uvalue, FALSE );
            break;

          case IC_CDARG_LABEL :             // GENERATE LABEL FOR TEST
            if( ! fctl->has_cdtor_val ) {
                CGControl( O_LABEL, NULL, fctl->cdarg_lab );
                BEFiniLabel( fctl->cdarg_lab );
            }
            break;


//
//          DEBUGGING -- for program
//

          case IC_DBG_SRCFILE :             // SET SOURCE FILE
            current_src = ins_value.pvalue;
            break;

          case IC_DBG_LINE :                // SET LINE NUMBER
          { TOKEN_LOCN locn;
            if( depth_inline == 0 ) {
                char *fname;

                fname = SrcFileFullName( current_src );
                DBSrcCue( DBSrcFile( fname ), ins_value.uvalue, 1 );
            }
            locn.src_file = current_src;
            locn.line = ins_value.uvalue;
            locn.column = 0;
            SetErrLoc( &locn );
            CtxLine( ins_value.uvalue );
          } break;

//
// EXCEPTION HANDLING
//

          // Top of stack is new'ed expression
          //
          case IC_NEW_ALLOC :               // CTORING OF NEW EXPRESSION
          { cg_name expr;                   // - expression being gen'ed
            cg_name new_expr;               // - CTOR'ed expression
            cg_type type_new_expr;          // - type of new expression
            if( DtmTabular( fctl ) ) {
                new_expr = CgExprPopType( &type_new_expr );
                expr = CGLVAssign( CgSymbol( FnCtlNewCtorPtr( fctl ) )
                                 , CgMakeDup( &new_expr, type_new_expr )
                                 , TY_POINTER );
                CgExprPush( CgComma( expr, new_expr, type_new_expr )
                          , type_new_expr );
            }
          } break;

          case IC_NEW_CTORED :              // CTOR OF NEW'ED COMPLETE
          { SYMBOL op_del;                  // - operator delete
            SE* se;                         // - new state entry
            TYPE cl_type = ins_value.pvalue;// - type of entry
            if( DtmTabular( fctl ) ) {
                op_del = CgBackOpDelete( cl_type );
                if( 1 == SymFuncArgList( op_del )->num_args ) {
                    se = SeAlloc( DTC_DLT_1 );
                    se->dlt_1.op_del = op_del;
                    if( se->base.gen ) {
                        AutoRelRegister( FnCtlNewCtorPtr( fctl )
                                       , &se->dlt_1.offset );
                    }
                } else {
                    se = SeAlloc( DTC_DLT_2 );
                    se->dlt_2.size = CgMemorySize( cl_type );
                    se->dlt_2.op_del = op_del;
                    if( se->base.gen ) {
                        AutoRelRegister( FnCtlNewCtorPtr( fctl )
                                       , &se->dlt_2.offset );
                    }
                }
                CgCallBackNewCtored( se, fctl );
            }
          } break;


        {
            target_size_t elem_size;        // size of element
            SE* se_dlt;                     // SE entry created

          case IC_DLT_DTOR_SIZE :           // SET SIZE OF ELEMENT
            elem_size = ins_value.uvalue;
            break;

          case IC_DLT_DTOR_ARR :            // DELETE FOR DTORABLE ARRAY
            se_dlt = DtorForDelBeg( fctl
                                  , elem_size
                                  , DTC_DLT_1_ARRAY
                                  , DTC_DLT_2_ARRAY
                                  , ins_value.pvalue );
            break;

          case IC_DLT_DTOR_ELM :            // DELETE FOR DTORABLE ELEMENT
            se_dlt = DtorForDelBeg( fctl
                                  , elem_size
                                  , DTC_DLT_1
                                  , DTC_DLT_2
                                  , ins_value.pvalue );
            break;

          case IC_DLT_DTORED :              // DTORABLE ELEMENT DTORED
            DtorForDelEnd( fctl, se_dlt );
            break;

        }

          case IC_DTORABLE_INIT :           // INITIALIZATION OF DTORABLE
            fctl->ctor_test = TRUE;
            break;

          case IC_TRY :                     // START A TRY BLOCK
          { RT_DEF def;                     // - defn for R/T call
            SE* se;                         // - state entry for try
            if( fctl->try_depth == 0 ) {
                fctl->try_label = BENewLabel();
                CGControl( O_LABEL, NULL, fctl->try_label );
            }
            fctl->try_depth++;
            se = SeAlloc( DTC_TRY );
            se->try_blk.try_impl = tryImpl( ins_value.pvalue );
            se->try_blk.sigs = NULL;
            se->try_blk.sym = NULL;
            se = FstabAdd( se );
            se = BlkPosnUpdate( se );
            FstabAssignStateVar( se );
            CgRtCallInit( &def, RTF_SETJMP );
            CgRtParamAddrSym( &def, se->try_blk.try_impl->jmp_sym );
            CgExprPush( CgRtCallExec( &def ), TY_INTEGER );
          } break;

          case IC_TRY_DONE :                // TRY HAS BEEN COMPLETED
          { SE* se;                         // - se entry for try
            se = stateTableTryBlk( tryImpl( ins_value.pvalue ) );
            se = FstabPrevious( se );
            se = BlkPosnUpdate( se );
            se = FstabSetSvSe( se );
            FstabAssignStateVar( se );
            DbgAssert( fctl->try_depth != 0 );
            fctl->try_depth--;
            if( fctl->try_depth == 0 ) {
                BEFiniLabel( fctl->try_label );
                fctl->try_label = UNDEFINED_LABEL;
            }
          } break;

          case IC_TRY_CATCH_DONE :          // EARLY EXIT FROM TRY/CATCH
                                            //  WITHOUT DESTRUCTION
          { SCOPE scope;                    // - scope of target
            SE* end;                        // - ending entry
            scope = ins_value.pvalue;
            DbgVerify( NULL != scope, "exitTryCatch -- bad scope" );
            end = BlkPosnScope( scope );
            cgDestruct( end, fctl );
          } break;


        { SE* try_se;                       // try SE for catch
          SE* catch_se;                     // catch SE

          case IC_CATCH_VAR :               // SET TRY_IMPL FOR CATCH
            try_se = stateTableTryBlk( tryImpl( ins_value.pvalue ) );
            FstabSetSvSe( FstabPrevious( try_se ) );
            break;

          case IC_CATCH :                   // SET TYPE OF A CATCH
          { SE* se;                         // - state entry for catch
            TYPE_SIG_ENT* ent;              // - type sig. entry for catch
            ent = BeTypeSigEnt( ins_value.pvalue );
            RingAppend( &try_se->try_blk.sigs, ent );
            se = SeAlloc( DTC_CATCH );
            catch_se = se;
            se->catch_blk.sig = ent;
            se->catch_blk.try_blk = try_se;
          } break;

          case IC_SET_CATCH_STATE :         // SET CATCH STATE
          { SE* se;                         // - state entry for catch
            se = FstabAdd( catch_se );
            se = BlkPosnUpdate( se );
            fctl->pre_init = se;
            FstabAssignStateVar( se );
            FstabSetSvSe( catch_se );
          } break;

        }

          case IC_SET_TRY_STATE :           // SET TRY STATE
          { SE* try_se = stateTableTryBlk( tryImpl( ins_value.pvalue ) );
            FstabSetSvSe( try_se );
          } break;

          case IC_EXCEPT_SPEC :             // FUNCTION EXCEPTION SPEC.
            if( (fctl->func->flag & SF_NO_LONGJUMP) == 0 ) {
                SE* fn_exc;
                if( fctl->has_fn_exc ) {
                    fn_exc = BlkPosnCurr();
                } else {
                    DT_METHOD saved = fctl->dtor_method;
                    fctl->dtor_method = fctl->func_dtor_method;
                    fctl->has_fn_exc = TRUE;
                    fn_exc = SeAlloc( DTC_FN_EXC );
                    fn_exc->base.gen = TRUE;
                    fn_exc->fn_exc.sigs = NULL;
                    fn_exc = FstabAdd( fn_exc );
                    fn_exc = BlkPosnUpdate( fn_exc );
                    FstabAssignStateVar( fn_exc );
                    fctl->dtor_method = saved;
                }
                RingAppend( &fn_exc->fn_exc.sigs
                          , BeTypeSigEnt( ins_value.pvalue ) );
            }
            break;

          case IC_EXCEPT_FUN :              // SET FUNCTION EXCEPT SPEC.
            CFatal( "CgBack -- have IC_EXCEPT_FUN" );
            break;

          case IC_THROW_RO_BLK :            // SET THROW R/O BLOCK
            sig_thunk_genned = TRUE;
            CgExprPush( ThrowRo( ins_value.pvalue ), TY_DEFAULT );
            break;

          case IC_SETJMP_DTOR :             // DESTRUCT AFTER A SETJMP
          { SYMBOL temp;                    // - temp for expression value
            temp = saveGenedExpr( exprn_type );
            if( DtmTabularFunc( fctl ) ) {
                SE* se;                     // - current state entry
                DbgAssert( file_ctl->u.s.state_table );
                se = SeSetSvPosition( BlkPosnTempEnd() );
                if( se == NULL ) {
                    CgDtorAll();
                } else {
                    CgDtorSe( se );
                }
            }
            fetchGenedExpr( temp );
          } break;

          case IC_DTOR_SUBOBJS :            // DESTRUCT SUBOBJECTS
            if( ! fctl->has_cdtor_val
             || (fctl->cdtor_val & DTOR_DELETE_VECTOR) == 0 ) {
                if( NULL != fctl->obj_registration ) {
                    SE* start = FstabActualPosn();
                    switch( fctl->dtor_method ) {
                      case DTM_DIRECT :
                      case DTM_DIRECT_TABLE :
                      case DTM_TABLE :
                        cgDestructGroup( fctl
                                       , start
                                       , fctl->dtor_components
                                       , DGRP_COMPS | DGRP_DIRECT );
                        break;
                      case DTM_TABLE_SMALL :
                      case DTM_DIRECT_SMALL :
                        cgDtorTabCall( start, fctl->dtor_components );
                        break;
                    }
                }
                BlkPosnUpdate( FstabCurrPosn() );
            }
            break;


          case IC_DTOR_USE :                // DTOR USED DIRECTLY
            break;

          case IC_SET_LABEL_SV :            // SET STATE AT LABEL
            CgDone( FstabEmitStateVar( BlkPosnCurr(), fctl ), TY_POINTER );
            break;

          case IC_DTOR_KIND :               // SET KIND OF DTOR IF REQ'D
            dtor_kind = ins_value.uvalue;
            break;

          case IC_SCOPE_CALL_CDTOR :        // SET LAST DTOR REQ'D
          case IC_SCOPE_CALL_TDTOR :        // SET LAST DTOR REQ'D
          case IC_SCOPE_CALL_BDTOR :        // SET LAST DTOR REQ'D
            dtor_last_reqd = ins_value.pvalue;
            break;

          case IC_COMPCTOR_BEG :            // START COMPONENT CTOR, TEMPS
          { OBJ_INIT* init;                 // - initialization object
            init = ObjInitClass();
            if(NULL == init){
                CFatal( "ObjInitClass returned NULL\nPossible: http://bugzilla.openwatcom.org/show_bug.cgi?id=63" );
            }
            if( init->defn != NULL ) {
                if( NULL != dtor_last_reqd ) {
                    SE* se;
                    cg_name expr;
                    se = cgAddSeComponent( dtor_kind, ins_value.uvalue, dtor_last_reqd );
                    fctl->ctored_obj = se;
                    BlkPosnTempEndSet( se );
                    expr = CgCallBackCtorStart( NULL, TY_POINTER, se );
                    CgCommaBefore( expr, TY_POINTER );
                    if( dtor_kind == DTC_ACTUAL_DBASE || dtor_kind == DTC_ACTUAL_VBASE ) {
                        if( DtmTabular( fctl ) ) {
                            expr = ObjInitRegActualBase( se );
                            CgCommaBefore( expr, TY_POINTER );
                        }
                    }
                }
            }
          } break;

          case IC_COMPCTOR_END :            // END COMPONENT CTOR, TEMPS
          { OBJ_INIT* init;                 // - initialization object
            cg_name expr;                   // - new expr
            init = ObjInitClass();
            if(NULL == init){
                CFatal( "ObjInitClass returned NULL\nPossible: http://bugzilla.openwatcom.org/show_bug.cgi?id=63" );
            }
            if( init->defn != NULL && init->se != NULL ) {
                expr = CgCallBackCtorDone( NULL, TY_POINTER, init->se );
                CgCommaOptional( expr, TY_POINTER );
            }
          } break;

          case IC_AFTER_ABORTS :            // RESET CURRENT SE AFTER ABORTS
            CgGotoReturnLabel( fctl );
            FstabSetSvSe( BlkPosnCurr() );
            break;

          case IC_AFTER_THROW :             // RESET CURRENT SE AFTER THROW
            if( fctl->try_depth != 0 ) {
                CGControl( O_GOTO, NULL, fctl->try_label );
            } else {
                CgGotoReturnLabel( fctl );
            }
            FstabSetSvSe( BlkPosnCurr() );
            break;

//
// IC parm stack support

          case IC_PARM_BIN :                // push an integer
            IC_PARM_PUSH_INT( ins_value.ivalue );
            break;
          case IC_PARM_SYM :                // push a SYMBOL
          case IC_PARM_TYP :                // push a TYPE
          case IC_PARM_SCP :                // push a SCOPE
            IC_PARM_PUSH_PTR( ins_value.pvalue );
            break;


//
// Virtual function thunk adjustments
          case IC_VTHUNK_MDELTA :           // - this -= delta;
            genVthunkDelta( fctl, O_MINUS, ins_value.ivalue );
            break;
          case IC_VTHUNK_PDELTA :           // - this += delta;
            genVthunkDelta( fctl, O_PLUS, ins_value.ivalue );
            break;
          case IC_VTHUNK_CDISP :            // - this -= *((unsigned*)this-sizeof(unsigned))
            genVthunkCDisp( fctl );
            break;
          case IC_VTHUNK_VBASE :            // - this += (*this)[i]
            genVthunkVBase( fctl, ins_value.ivalue );
            break;


//
// Generation of Virtual-Function Tables

          case IC_VFT_BEG :                 // VFT: START
          { SYMBOL vft;
            vft = ins_value.pvalue;
            if( 0 == ( vft->flag & SF_REFERENCED ) ) {
                flushOverInitialization( file_ctl );
            }
          } break;

          case IC_VFT_REF :                 // VFT: REFERENCE
            break;


          case IC_RTTI_REF :                // RTTI REFERENCE FROM VFT
          { SYMBOL rtti;
            rtti = ins_value.pvalue;
            RttiRef( rtti );
          } break;

#ifndef NDEBUG
//
//          DEBUGGING -- internal (not in production version)
//

          case IC_TRACE_BEG :               // TURN IC TRACE ON
            PragDbgToggle.dump_exec_ic = 1;
            break;

          case IC_TRACE_END :               // TURN IC TRACE OFF
            PragDbgToggle.dump_exec_ic = 0;
            break;

#endif
        }
    }
    IC_PARM_DONE;
    CgioCloseInputFile( file_ctl );
    return fctl;
}


static void remove_file(        // REMOVE FILE, IF NOT INLINE
    CGFILE *file_ctl )          // - current file
{
    if( ( file_ctl->symbol == NULL ) || ! SymIsInline( file_ctl->symbol ) ) {
        CgioFreeFile( file_ctl );
    }
}


static void writeVirtualFile(   // EMIT AND FREE A VIRTUAL FILE
    CGFILE *file_ctl )          // - current file
{
#ifndef NDEBUG
    SYMBOL func;                // - function symbol
#endif
//    FN_CTL* fctl;               // - file control

    ExtraRptIncrementCtr( ctr_funcs );
//  new_ctor_ptr = NULL;
    autos = NULL;
    FstabInit();
#ifndef NDEBUG
    func = file_ctl->symbol;
    if( PragDbgToggle.callgraph || PragDbgToggle.dump_stab ||
        PragDbgToggle.dump_exec_ic ) {
        if( func == NULL ) {
            printf( "generating module data\n" );
        } else {
            VBUF vbuf;
            printf( "generating %s\n", DbgSymNameFull( func, &vbuf ) );
            VbufFree( &vbuf );
        }
    }
    if( PragDbgToggle.genned ) {
        DbgGenned( func );
    }
#endif
//    fctl = emit_virtual_file( file_ctl, NULL );
    emit_virtual_file( file_ctl, NULL );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        FstabDump();
    }
#endif
    if( FstabGenerate() ) {
        sig_thunk_genned = TRUE;
    }
    freeTryImpls();
    FreeDtregObjs();
    SegmentLabelStackReset();
    remove_file( file_ctl );
    FnCtlPop();
    SymTransEmpty();
    AutoRelFree();
    CgBackFreeIndCalls();
}


static void process_virtual_file( // PROCESS A VIRTUAL FILE
    CGFILE *file_ctl )          // - current file
{
    SYMBOL sym;                 // - symbol for file

    sym = file_ctl->symbol;
    if( sym == NULL ) {
        data_file = file_ctl;
    } else if( file_ctl->u.s.thunk ) {
        CgioThunkStash( file_ctl );
    } else {
        if( file_ctl->u.s.refed ) {
            writeVirtualFile( file_ctl );
        }
    }
}


static void process_thunk(      // PROCESS THUNK AFTER VIRTUAL FILES
    CGFILE *thunk )             // - current file
{
    EXTRF ext_info;             // - extern-reference information (not used)

    if( thunk->u.s.refed
     && ! thunk->u.s.thunk_gen
     && NULL == ExtrefResolve( thunk->symbol, &ext_info ) ) {
        thunk->u.s.thunk_gen = TRUE;
        writeVirtualFile( thunk );
    }
}


static void cgbackInit(         // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    state_tables_obj = NULL;
    fun_try_impls = NULL;
    data_file = NULL;
    CgbkInfo.size_data_ptr = CgDataPtrSize();
    CgbkInfo.size_code_ptr = CgCodePtrSize();
    CgbkInfo.size_offset = CgMemorySize( TypeTargetSizeT() );
    if( CompFlags.rw_registration ) {
        CgbkInfo.size_fs_hand = CgbkInfo.size_code_ptr;
    } else {
        CgbkInfo.size_fs_hand = 0;
    }
#if _CPU == _AXP
    CgbkInfo.size_rw_base = CgbkInfo.size_offset
                          + CgbkInfo.size_data_ptr;
#else
    CgbkInfo.size_rw_base = CgbkInfo.size_data_ptr * 2
                          + CgbkInfo.size_fs_hand
                          + CgbkInfo.size_offset;
#endif
    VstkOpen( &stack_labs_cs, sizeof( label_handle ), 8 );
    VstkOpen( &stack_goto_near, sizeof( label_handle ), 16 );
    carveTRY_IMPL = CarveCreate( sizeof( TRY_IMPL ), 4 );
    carveSTAB_OBJ = CarveCreate( sizeof( STAB_OBJ ), 4 );
    CompFlags.has_longjmp = FALSE;
    ExtraRptRegisterCtr( &ctr_ic_codes, "number of IC codes processed" );
    ExtraRptRegisterCtr( &ctr_inlines, "number of generated inline functions" );
    ExtraRptRegisterCtr( &ctr_funcs, "number of generated non-inlined functions" );
}


static void cgbackFini(         // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_labs_cs );
    VstkClose( &stack_goto_near );
    CarveDestroy( carveTRY_IMPL );
    CarveDestroy( carveSTAB_OBJ );
}


INITDEFN( cg_back, cgbackInit, cgbackFini )


void CgBackEnd(                 // BACK-END CONTROLLER
    void )
{
    cg_init_info cg_info;       // - information from code generator

    CompFlags.codegen_active = TRUE;
    CDoptBackEnd();
    MarkFuncsToGen( max_inline_depth );
#ifndef NDEBUG
    PragDbgToggle.callgraph_scan = 0;
    if( PragDbgToggle.dump_cg ) {
        GenSwitches |= ECHO_API_CALLS;
    }
#endif
    CtxSetContext( CTX_CG_FUNC );
    if( BELoad( NULL ) ) {
        cg_info = BEInitCg( GenSwitches, TargetSwitches, OptSize, CpuSwitches );
        if( ! cg_info.success ) {
            CErr1( ERR_CODEGEN_CANT_INITIALIZE );
            CSuicide();
#ifndef NDEBUG
        } else if( cg_info.version.revision != II_REVISION ) {
            CFatal( "Incorrect Code Generator version" );
#endif
        } else {
            ExitPointAcquire( cgback );
            SegmentCgInit();
            BEStart();
            CgBackStatHandlesInit();
            thisSym = AllocSymbol();
            thisSym->id = SC_AUTO;
            cdtorSym = AllocSymbol();
            cdtorSym->id = SC_AUTO;
            statics = NULL;
            if( GenSwitches & DBG_DF ){
                DwarfDebugInit();
                DwarfDebugEmit();
            }else{
                SymbolicDebugInit();
                SymbolicDebugEmit();
            }
#ifdef FASTCG
            if( GenSwitches & NO_OPTIMIZATION ) {
                extern void InitExpressCode(int,int);
                InitExpressCode( SEG_CONST2, 1 );
            }
#endif
//            MstabInit();
            CgioWalkFiles( &process_virtual_file );
            writeVirtualFile( data_file );
//            MstabGenerate();
            do {
                CgCmdsGenerate();
                ThrowRoGen();
                BeGenTypeSignatures();
                sig_thunk_genned = FALSE;
                BeGenRttiInfo();
                CgioWalkThunks( &process_thunk );
            } while( sig_thunk_genned );
            freeObjTables();
            CgioWalkFiles( &CgioFreeFile );
            if( GenSwitches & DBG_DF ){
                DwarfDebugFini();
            }else{
                SymbolicDebugFini();
            }
            CtxSetContext( CTX_CG_OPT );
            if( ErrCount != 0 ) {
                BEAbort();
            }
            BEStop();
            // BEStop() can generate some back handles that must be freed
            CgBackFreeFileHandles();
            StringWalk( &undefine_string_const );
            CgBackStatHandlesFini();
            BEFiniCg();
            ExitPointRelease( cgback );
            FreeSymbol( thisSym );
            FreeSymbol( cdtorSym );
        }
        BEUnload();
    } else {
        CErr1( ERR_CODEGEN_CANT_INITIALIZE );
        CSuicide();
    }
}


void FEGenProc(                 // INLINE SUPPORT
    cg_sym_handle _sym,         // - function to be in-lined
    call_handle handle )        // - handle of called function
{
    CGFILE *file_ctl;           // - file control info
//    FN_CTL* fctl;               // - file-gen info. for caller
#ifndef NDEBUG
    SE* curr;                   // - current state entry for caller
#endif
    SYMBOL sym = _sym;          // - function to be in-lined

                                // - CGFILE information stacked
    CGIOBUFF *buffering;        // - - buffering control
    CGINTER *cursor;            // - - cursor in block

//    fctl = FnCtlTop();
    FnCtlTop();
    ExtraRptIncrementCtr( ctr_inlines );
#ifndef NDEBUG
    curr = CallStabStateTablePosn( handle );
    if( PragDbgToggle.callgraph || PragDbgToggle.dump_stab ) {
        VBUF vbuf;
        if( PragDbgToggle.dump_exec_ic )
            printf( "\n" );
        printf( "start of inline function: %s\n", DbgSymNameFull( sym, &vbuf ) );
        if( PragDbgToggle.dump_stab ) {
            printf( "   positions: state-table(%p) call(%p)\n"
                  , FstabCurrPosn()
                  , curr );
        }
        if( PragDbgToggle.dump_exec_ic ) {
            printf( "\n" );
        }
        VbufFree( &vbuf );
    }
#else
    CallStabStateTablePosn( handle );
#endif
    SymTransNewBlock();
    ++ depth_inline;
    file_ctl = CgioLocateAnyFile( sym );
    buffering = file_ctl->buffer;
    cursor = file_ctl->cursor;
    emit_virtual_file( file_ctl, handle );
#ifndef NDEBUG
    if( PragDbgToggle.callgraph || PragDbgToggle.dump_stab ) {
        VBUF vbuf;
        if( PragDbgToggle.dump_exec_ic )
            printf( "\n" );
        printf( "end of inline function: %s\n", DbgSymNameFull( sym, &vbuf ) );
        if( PragDbgToggle.dump_stab ) {
            printf( "   positions: state-table(%p) marked_posn(%p)\n"
                  , FstabCurrPosn()
                  , FstabMarkedPosn() );
        }
        if( PragDbgToggle.dump_exec_ic ) {
            printf( "\n" );
        }
        VbufFree( &vbuf );
    }
#endif
    file_ctl->buffer = buffering;
    file_ctl->cursor = cursor;
    FnCtlPop();
    -- depth_inline;
}


void CgBackSetInlineDepth(      // SET MAXIMUM INLINE DEPTH
    unsigned max_depth )        // - the maximum
{
    max_inline_depth = max_depth;
}

unsigned CgBackGetInlineDepth(  // GET MAXIMUM INLINE DEPTH
    void )
{
    return( max_inline_depth );
}


CALL_STAB* CgBackCallGened(     // SETUP FOR GENERATED CALL
    call_handle handle )        // - call handle
{
    return CallStabAlloc( handle, FnCtlTop() );
}


static void processEndExpr(     // END-OF-EXPRESSION PROCESSING
    FN_CTL* fctl )              // - function containing the expression
{
    IbpFlush( fctl );
    while( fctl->expr_calls != NULL ) {
        CallStabFree( fctl, fctl->expr_calls->next );
    }
    if( fctl->ctored_obj != NULL ) {
        FstabSetSvSe( fctl->ctored_obj );
        FstabEmitStateVarExpr( fctl->ctored_obj, fctl );
        fctl->ctored_obj = NULL;
    }
}


void CgDone(                    // COMPLETE CODE-GENERATION OF EXPRESSION
    cg_name expr,               // - expression
    cg_type type )              // - and its type
{
    FN_CTL* fctl;               // - function containing the expression

    if( expr != NULL ) {
        fctl = FnCtlTop();
        CgExprDtored( expr, type, DGRP_TEMPS | DGRP_DONE, fctl );
        processEndExpr( fctl );
        fctl->ctor_test = FALSE;
    }
}


void CgTrash(                   // COMPLETE TRASH OF EXPRESSION
    cg_name expr,               // - expression
    cg_type type )              // - and its type
{
    FN_CTL* fctl;               // - function containing the expression

    if( expr != NULL ) {
        fctl = FnCtlTop();
        CgExprDtored( expr, type, DGRP_TEMPS | DGRP_TRASH, fctl );
        processEndExpr( fctl );
    }
}


void CgControl(                 // CONTROL OPCODE
    cg_op operand,              // - operand
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    label_handle label )        // - label
{
    FN_CTL* fctl;               // - function containing the expression

    if( expr == NULL ) {
        CGControl( operand, expr, label );
    } else {
        fctl = FnCtlTop();
        CgExprDtored( expr, type, DGRP_TEMPS, fctl );
        CGControl( operand, CgExprPop(), label );
        processEndExpr( fctl );
    }
}


unsigned CgBackInlinedDepth(    // GET CURRENT INLINED DEPTH
    void )
{
    return depth_inline;
}


static unsigned push_base(      // PUSH BASE OF A VIRTUAL STACK
    VSTK_CTL *stack )           // - stack
{
    return( VstkDimension( stack ) + 1 );
}


FN_CTL* CgBackFnCtlInit(        // INITIALIZE FN_CTL WITH CGBKMAIN INFO
    FN_CTL* fctl )              // - current file information
{
    fctl->base_goto_near = push_base( &stack_goto_near );
    fctl->base_labs_cs   = push_base( &stack_labs_cs );
    SymTransFuncBeg( fctl );
    return fctl;
}


FN_CTL* CgBackFnCtlFini(        // COMPLETE FN_CTL WITH CGBKMAIN INFO
    FN_CTL* fctl )              // - current file information
{
    VstkTruncate( &stack_labs_cs,   fctl->base_labs_cs );
    VstkTruncate( &stack_goto_near, fctl->base_goto_near );
    SymTransFuncEnd( fctl );
    return fctl;
}


void CgDestructExprTemps(       // DESTRUCT TEMPS IN AN EXPRESSION
    DGRP_FLAGS pop_type,        // - type of popping
    FN_CTL* fctl )              // - current file information
{
    cgDestructStab( BlkPosnTempEnd(), fctl, BlkPosnTempBeg(), pop_type );
}
