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
#include "cgfront.h"
#include "ring.h"
#include "initdefs.h"
#include "fnbody.h"
#include "pcheader.h"

#define PTD_DEFS                                                          \
                            /* PTD_KIND -- kinds of entries             */\
 PTD_DEF( PTD_CTORCOMP     )/* - ctored component (code)                */\
,PTD_DEF( PTD_CTORED_COMP  )/* - ctored component (scope-call)          */\
,PTD_DEF( PTD_DEL_DTORED   )/* - complete dtor of element to be del'ed  */\
,PTD_DEF( PTD_DEL_DTOR_ARR )/* - dtor of array to be deleted            */\
,PTD_DEF( PTD_DEL_DTOR_ELM )/* - dtor of element to be deleted          */\
,PTD_DEF( PTD_DEL_DTOR_SIZE)/* - size of element in object to be del'd  */\
,PTD_DEF( PTD_DTOR_EXPR    )/* - dtorable object in expression          */\
,PTD_DEF( PTD_DTOR_KIND    )/* - dtor kind                              */\
,PTD_DEF( PTD_DTOR_REF     )/* - dtor reference                         */\
,PTD_DEF( PTD_DTOR_SCOPE   )/* - dtorable object in scope               */\
,PTD_DEF( PTD_DTOR_USE     )/* - dtor usage                             */\
,PTD_DEF( PTD_EXPR_CONST   )/* - expression is constant                 */\
,PTD_DEF( PTD_INIT_SYM_END )/* - end of initialization of symbol        */\
,PTD_DEF( PTD_NEW_ALLOC    )/* - signal new allocation                  */\
,PTD_DEF( PTD_NEW_CTOR     )/* - ctoring new'ed expression              */\
,PTD_DEF( PTD_OBJ_OFFSET   )/* - push object: offset                    */\
,PTD_DEF( PTD_OBJ_PUSH     )/* - push object                            */\
,PTD_DEF( PTD_OBJ_POP      )/* - pop object                             */\
,PTD_DEF( PTD_OBJ_SYM      )/* - push object: symbol                    */\
,PTD_DEF( PTD_RETNOPT_VAR  )/* - gen IC_RETNOPT_VAR                     */\
,PTD_DEF( PTD_RETNOPT_END  )/* - gen IC_RETNOPT_END                     */\
,PTD_DEF( PTD_SCOPE_CALL   )/* - call to possible throwable function    */\
,PTD_DEF( PTD_SCOPE_GEN    )/* - force scope to have statement table    */\
,PTD_DEF( PTD_THROW        )/* - throw (or equivalent) in expression    */\
,PTD_DEF( PTD_VB_DELTA     )/* - virtual base delta                     */\
,PTD_DEF( PTD_VB_EXACT     )/* - virtual base exact                     */\
,PTD_DEF( PTD_VB_INDEX     )/* - virtual base index                     */\
,PTD_DEF( PTD_VB_OFFSET    )/* - virtual base offset                    */\
,PTD_DEF( PTD_VF_INDEX     )/* - virtual function index                 */\
,PTD_DEF( PTD_VF_OFFSET    )/* - virtual function offset                */\
,PTD_DEF( PTD_VF_SYM       )/* - virtual function symbol                */\
,PTD_DEF( PTD_OFFSETOF_EXPR)/* - offsetof field name                    */\
,PTD_DEF( PTD_PCH_FREE     )/* - used to mark free PTDs for PCH write   */\
,PTD_DEF( MAX_PTD_KIND )    /* bound for debugging                      */

typedef enum                    // PTD_KIND -- kinds of entries
{
    #define PTD_DEF( a ) a
    PTD_DEFS
    #undef  PTD_DEF
} PTD_KIND;

typedef enum                    // PTD_FMT -- formats of entries
{   PTD_FMT_BASE                // - base only
,   PTD_FMT_SYMBOL              // - symbol
,   PTD_FMT_OFFSET              // - offset
,   PTD_FMT_SIZE                // - size
,   PTD_FMT_TYPE                // - type
,   PTD_FMT_PTREE               // - parse tree
} PTD_FMT;

typedef struct ptd_base         PTD_BASE;
typedef struct ptd_symbol       PTD_SYMBOL;
typedef struct ptd_offset       PTD_OFFSET;
typedef struct ptd_size         PTD_SIZE;
typedef struct ptd_type         PTD_TYPE;
typedef struct ptd_ptree        PTD_PTREE;

struct ptd_base                 // PTD_BASE -- base entry
{   PTD* next;                  // - next in list
    PTD_KIND kind;              // - type of entry
    PTD_FMT fmt;                // - format of entry
    unsigned :0;                // - force alignment
};

struct ptd_symbol               // PTD_SYMBOL -- entry containing symbol
{   PTD_BASE base;              // - base
    SYMBOL sym;                 // - symbol
};

struct ptd_type                 // PTD_TYPE -- entry containing type
{   PTD_BASE base;              // - base
    TYPE type;
};

struct ptd_ptree                // PTD_PTREE -- entry containing parse tree
{   PTD_BASE base;              // - base
    PTREE tree;
};

struct ptd_offset               // PTD_OFFSET -- entry containing offset
{   PTD_BASE base;              // - base
    target_offset_t offset;     // - offset
};

struct ptd_size                 // PTD_SIZE -- entry containing size
{   PTD_BASE base;              // - base
    target_size_t size;         // - size
};


union ptd                       // PTD - union of all decorations
{   PTD_BASE base;              // - base
    PTD_SYMBOL symbol;          // - symbol entry
    PTD_OFFSET off;             // - offset entry
    PTD_SIZE size;              // - size entry
    PTD_TYPE type;              // - type entry
    PTD_PTREE ptree;            // - ptree entry
};



static carve_t carvePTD;        // - carve info


static PTD* ptdAlloc            // ALLOCATE AND INITIALIZE BASE ENTRY
    ( PTD** ring                // - expression
    , PTD_KIND kind             // - kind of entry
    , PTD_FMT fmt )             // - format of entry
{
    PTD* ptd;                   // - allocated entry

    ptd = RingCarveAlloc( carvePTD, ring );
    ptd->base.kind = kind;
    ptd->base.fmt = fmt;
    return ptd;
}


static PTREE ptdOffset          // ALLOCATE AN OFFSET ENTRY
    ( PTREE expr                // - expression
    , target_offset_t offset    // - offset
    , PTD_KIND kind )           // - kind of entry
{
    PTD* pto;                   // - new entry

    pto = ptdAlloc( &expr->decor, kind, PTD_FMT_OFFSET );
    pto->off.offset = offset;
    return expr;
}


static PTREE ptdSize            // ALLOCATE A SIZE ENTRY
    ( PTREE expr                // - expression
    , target_size_t size        // - size
    , PTD_KIND kind )           // - kind of entry
{
    PTD* pto;                   // - new entry

    pto = ptdAlloc( &expr->decor, kind, PTD_FMT_SIZE );
    pto->size.size = size;
    return expr;
}


static PTREE ptdType            // ALLOCATE A TYPE ENTRY
    ( PTREE expr                // - expression
    , TYPE type                 // - type to be stashed
    , PTD_KIND kind )           // - kind of entry
{
    PTD* pto;                   // - new entry

    pto = ptdAlloc( &expr->decor, kind, PTD_FMT_TYPE );
    pto->type.type = type;
    return expr;
}


static PTREE ptdBase            // DECORATE WITH BASIC ENTRY
    ( PTREE expr                // - expression
    , PTD_KIND kind )           // - kind of entry
{
    ptdAlloc( &expr->decor, kind, PTD_FMT_BASE );
    return expr;
}


static PTREE ptdSymbol          // DECORATE WITH A SYMBOL
    ( PTREE expr                // - expression
    , SYMBOL sym                // - symbol
    , PTD_KIND kind )           // - kind of decoration
{
    PTD* ptd;                   // - allocated entry

    ptd = ptdAlloc( &expr->decor, kind, PTD_FMT_SYMBOL );
    ptd->symbol.sym = sym;
    return expr;
}


static PTREE ptdPTree           // DECORATE WITH A PTREE
    ( PTREE expr                // - expression
    , PTREE tree                // - parse tree
    , PTD_KIND kind )           // - kind of decoration
{
    PTD* ptd;                   // - allocated entry

    ptd = ptdAlloc( &expr->decor, kind, PTD_FMT_PTREE );
    ptd->ptree.tree = tree;
    return expr;
}


PTREE PtdFree                   // FREE ENTRIES FOR A PARSE TREE NODE
    ( PTREE expr )              // - parse tree node
{
    RingCarveFree( carvePTD, &expr->decor );
    return expr;
}


PTD* PtdDuplicateReloc          // DUPLICATE DECORATION, RELOC SYMBOLS
    ( PTREE src                 // - source PTREE
    , RELOC_LIST *reloc_list )  // - relocation list
{
    PTD* ring;                  // - ring to be used
    PTD* curr;                  // - current entry
    PTD* copy;                  // - copied entry

    ring = NULL;
    RingIterBeg( src->decor, curr ) {
        copy = ptdAlloc( &ring, curr->base.kind, curr->base.fmt );
        switch( curr->base.fmt ) {
          case PTD_FMT_BASE :
            break;
          case PTD_FMT_SYMBOL :
            if( reloc_list == NULL ) {
                copy->symbol.sym = curr->symbol.sym;
            } else {
                copy->symbol.sym = SymReloc( curr->symbol.sym, reloc_list );
            }
            break;
          case PTD_FMT_TYPE :
            copy->type.type = curr->type.type;
            break;
          case PTD_FMT_OFFSET :
            copy->off.offset = curr->off.offset;
            break;
          case PTD_FMT_SIZE :
            copy->size.size = curr->size.size;
            break;
          DbgDefault( "PtdDuplicate -- bad format" );
        }
    } RingIterEnd( curr );
    return ring;
}

PTD* PtdDuplicate               // DUPLICATE DECORATION
    ( PTREE src )               // - source PTREE
{
    return( PtdDuplicateReloc( src, NULL ) );
}


PTREE PtdInsert                 // INSERT A DECORATION RING
    ( PTREE expr                // - expression
    , PTD* ring )               // - ring of decoration
{
    if( NULL == expr->decor ) {
        expr->decor = ring;
    } else if( NULL != ring ) {
        PTD* first = ring->base.next;
        ring->base.next = expr->decor->base.next;
        expr->decor->base.next = first;
    }
    return expr;
}


static SYMBOL dtorForType       // LOCATE DTOR FOR TYPE
    ( TYPE type )               // - elemental type
{
    SYMBOL dtor;                // - NULL or DTOR for type

    type = StructType( type );
    if( NULL == type ) {
        dtor = NULL;
    } else if( TypeReallyDtorable( type ) ) {
        dtor = RoDtorFindType( type );
    } else {
        dtor = NULL;
    }
    return dtor;
}


PTREE PtdRetnOptVar             // DECORATE WITH IC_RETNOPT_VAR
    ( PTREE expr                // - expression
    , SYMBOL var )              // - symbol which could be optimized away
{
    if( FnRetnOpt() ) {
        if( expr == NULL ) {
            CgFrontCodePtr( IC_RETNOPT_VAR, var );
            expr = NULL;
        } else {
            expr = ptdSymbol( expr, var, PTD_RETNOPT_VAR );
        }
    }
    return expr;
}


PTREE PtdRetnOptEnd             // DECORATE WITH IC_RETNOPT_END
    ( PTREE expr )              // - expression
{
    if( FnRetnOpt() ) {
        if( expr == NULL ) {
            CgFrontCode( IC_RETNOPT_END );
            expr = NULL;
        } else {
            expr = ptdBase( expr, PTD_RETNOPT_END );
        }
    }
    return expr;
}


PTREE PtdDtorUseSym             // DECORATE WITH DTOR USAGE (SYMBOL)
    ( PTREE expr                // - expression
    , SYMBOL dtor )             // - destructor symbol
{
    if( dtor != NULL ) {
        dtor->flag |= SF_REFERENCED;
        expr = FunctionCalled( expr, dtor );
        expr = ptdSymbol( expr, dtor, PTD_DTOR_USE );
    }
    return expr;
}


static PTREE dtorDecorate       // DTOR DECORATION
    ( PTREE expr                // - expression
    , SYMBOL dtor               // - destructor
    , PTD_KIND kind )           // - decorative kind
{
    if( dtor != NULL ) {
        FunctionHasRegistration();
        expr = FunctionCalled( expr, dtor );
        expr = ptdSymbol( expr, dtor, kind );
    }
    return expr;
}


PTREE PtdCtoredComponent        // CTORED COMPONENT (BASE OR ELEMENT)
    ( PTREE expr                // - expression
    , TYPE type )               // - type of component
{
    return dtorDecorate( expr, dtorForType( type ), PTD_CTORED_COMP );
}


PTREE PtdDtorScopeSym           // DTOR OF SCOPE (SYMBOL)
    ( PTREE expr                // - expression
    , SYMBOL dtor )             // - destructor
{
    return dtorDecorate( expr, dtor, PTD_DTOR_SCOPE );
}


PTREE PtdDtorScopeType          // DTOR OF SCOPE (TYPE)
    ( PTREE expr                // - expression
    , TYPE type )               // - type dtored
{
    return PtdDtorScopeSym( expr, dtorForType( type ) );
}


PTREE PtdScopeCall              // SCOPE-CALL RECORDING
    ( PTREE expr                // - expression
    , SYMBOL fun )              // - function/ctor called
{
    if( fun != NULL && ! ( fun->flag & SF_NO_LONGJUMP ) ) {
        expr = ptdSymbol( expr, fun, PTD_SCOPE_CALL );
    }
    return expr;
}


static PTREE exprCtoring        // EXPRESSION CTORING
    ( PTREE expr                // - expression
    , SYMBOL ctor               // - ctor used
    , TYPE cl_type              // - class type
    , PTD_KIND kind )           // - kind of dtoring
{
    boolean ctor_called;        // - TRUE ==> CTOR-call already present
    boolean dtor_decorated;     // - TRUE ==> DTOR decoration present
    PTD* ptd;                   // - current decoration
    SYMBOL dtor;                // - dtor for type

    dtor = dtorForType( cl_type );
    if( dtor != NULL || ctor != NULL ) {
        ctor_called = FALSE;
        dtor_decorated = FALSE;
        RingIterBeg( expr->decor, ptd ) {
          switch( ptd->base.kind ) {
            case PTD_SCOPE_CALL :
              if( ptd->symbol.sym == ctor ) {
                  ctor_called = TRUE;
              }
              break;
            case PTD_DTOR_EXPR :
              DbgVerify( dtor == ptd->symbol.sym
                       , "exprCtoring -- dtor mismatch" );
              ptd->base.kind = kind;
              dtor_decorated = TRUE;
              break;
            case PTD_DTOR_SCOPE :
            case PTD_CTORED_COMP :
              DbgVerify( dtor == ptd->symbol.sym
                       , "exprCtoring -- dtor mismatch" );
              DbgVerify( kind == PTD_DTOR_EXPR
                       , "exprCtoring -- not PTD_EXPR" );
              dtor_decorated = TRUE;
              break;
          }
        } RingIterEnd( ptd );
        if( ! ctor_called ) {
            expr = PtdScopeCall( expr, ctor );
        }
        if( ! dtor_decorated ) {
            expr = dtorDecorate( expr, dtor, kind );
        }
    }
    return expr;
}


PTREE PtdCtoredExprType         // DECORATE TEMP AFTER CTORING
    ( PTREE expr                // - expression
    , SYMBOL ctor               // - ctor used
    , TYPE cl_type )            // - class type
{
    return exprCtoring( expr, ctor, cl_type, PTD_DTOR_EXPR );
}


PTREE PtdCtoredScopeType        // DECORATE AUTO AFTER CTORING
    ( PTREE expr                // - expression
    , SYMBOL ctor               // - ctor used
    , TYPE cl_type )            // - class type
{
    return exprCtoring( expr, ctor, cl_type, PTD_DTOR_SCOPE );
}


PTREE PtdThrow                  // DECORATE TO INDICATE THROW (OR EQUIVALENT)
    ( PTREE expr )              // - expression
{
    return ptdBase( expr, PTD_THROW );
}


PTREE PtdExprConst              // DECORATE A CONSTANT EXPRESSION
    ( PTREE expr )              // - expression
{
    return ptdBase( expr, PTD_EXPR_CONST );
}


static void emitDone            // EMIT CGDONE
    ( void )
{
    CgFrontCode( IC_EXPR_DONE );
}


static void emitCode            // EMIT STAND-ALONE OPCODE
    ( CGINTEROP opcode )        // - opcode
{
    CgFrontCode( opcode );
    emitDone();
}


static void emitCodeUint        // EMIT STAND-ALONE OPCODE + UNSIGNED
    ( CGINTEROP opcode          // - opcode
    , unsigned value )          // - value
{
    CgFrontCodeUint( opcode, value );
    emitDone();
}


PTREE PtdCompCtored             // DECORATE INDICATING COMPONENT CTORED
    ( PTREE expr                // - expression
    , target_offset_t offset    // - offset
    , unsigned dtc_kind )       // - kind of component
{
    if( NULL == expr ) {
        CgFrontCodeUint( IC_DTOR_KIND, dtc_kind );
        emitCodeUint( IC_COMPCTOR_BEG, offset );
        emitCodeUint( IC_COMPCTOR_END, offset );
    } else {
        expr = ptdOffset( expr, dtc_kind, PTD_DTOR_KIND );
        expr = ptdOffset( expr, offset, PTD_CTORCOMP );
    }
    return expr;
}


PTREE PtdObjPush                // DECORATE FOR START OF DTORABLE OBJECT
    ( PTREE expr                // - expression
    , TYPE type                 // - type of object
    , SYMBOL sym                // - NULL or symbol
    , target_offset_t offset )  // - offset
{
    if( NULL == expr ) {
        CgFrontCodePtr( IC_DTOBJ_PUSH, type );
        if( NULL != sym ) {
            CgFrontCodePtr( IC_DTOBJ_SYM, sym );
        }
        emitCodeUint( IC_DTOBJ_OFF, offset );
    } else {
        expr = ptdType( expr, type, PTD_OBJ_PUSH );
        if( NULL != sym ) {
            expr = ptdSymbol( expr, sym, PTD_OBJ_SYM );
        }
        expr = ptdOffset( expr, offset, PTD_OBJ_OFFSET );
    }
    return expr;
}


PTREE PtdObjPop                 // DECORATE FOR END OF DTORABLE OBJECT
    ( PTREE expr )              // - expression
{
    if( NULL == expr ) {
        emitCode( IC_DTOBJ_POP );
    } else {
        expr = ptdBase( expr, PTD_OBJ_POP );
    }
    return expr;
}


PTREE PtdInitSymEnd             // DECORATE FOR END OF SYMBOL INITIALIZATION
    ( PTREE expr                // - expression
    , SYMBOL sym )              // - symbol
{
    if( NULL == expr ) {
        boolean br = CgFrontRetnOptVar( sym );
        CgFrontCodePtr( IC_INIT_SYM_END, sym );
        if( br ) {
            CgFrontRetnOptEnd();
        }
    } else {
        expr = ptdSymbol( expr, sym, PTD_INIT_SYM_END );
    }
    return expr;
}


PTREE PtdNewAlloc               // DECORATE NEW ALLOCATION
    ( PTREE expr )              // - expression
{
    return ptdBase( expr, PTD_NEW_ALLOC );
}


PTREE PtdNewCtor                // DECORATE FOR CTORING NEW'ED EXPRESSION
    ( PTREE expr                // - expression
    , TYPE type )               // - class type
{
    return ptdType( expr, type, PTD_NEW_CTOR );
}


PTREE PtdVfunAccess             // ACCESS VIRTUAL FUNCTION
    ( PTREE expr                // - expression
    , unsigned vf_index         // - index into VF table
    , target_offset_t vf_offset // - offset to VF table ptr
    , SYMBOL baser )            // - basing "this" symbol
{
    expr = ptdOffset( expr, vf_index, PTD_VF_INDEX );
    expr = ptdOffset( expr, vf_offset, PTD_VF_OFFSET );
    expr = ptdSymbol( expr, baser, PTD_VF_SYM );
    return expr;
}


PTREE PtdVbaseFetch             // FETCH OF VIRTUAL BASE
    ( PTREE expr                // - expression
    , target_offset_t vb_offset // - virtual base offset
    , unsigned vb_index         // - virtual base index
    , target_offset_t vb_delta  // - virtual base delta
    , target_offset_t vb_exact )// - virtual base exact offset
{
    expr = ptdOffset( expr, vb_offset, PTD_VB_OFFSET );
    expr = ptdOffset( expr, vb_index,  PTD_VB_INDEX  );
    expr = ptdOffset( expr, vb_delta,  PTD_VB_DELTA  );
    expr = ptdOffset( expr, vb_exact,  PTD_VB_EXACT  );
    return expr;
}


static target_offset_t getOffset // SEARCH FOR OFFSET IN DECORATION
    ( PTREE expr                // - expression
    , PTD_KIND kind )           // - decoration kind
{
#ifndef NDEBUG
    boolean found = FALSE;      // - checks for found
#endif
    PTD* ptd;                   // - current entry
    target_offset_t retn;       // - offset in decoration

    RingIterBeg( expr->decor, ptd ) {
        if( kind == ptd->base.kind ) {
            retn = ptd->off.offset;
#ifndef NDEBUG
            DbgVerify( ! found, "Ptd::getOffset -- two decorations" );
            found = TRUE;
#else
            break;
#endif
        }
    } RingIterEnd( ptd );
    DbgVerify( found, "Ptd::getOffset -- no offset" );
    return retn;
}


target_offset_t PtdGetVbExact   // GET VBASE EXACT DECORATION
    ( PTREE expr )              // - expression
{
    return getOffset( expr, PTD_VB_EXACT );
}


target_offset_t PtdGetVbOffset  // GET VBASE OFFSET DECORATION
    ( PTREE expr )              // - expression
{
    return getOffset( expr, PTD_VB_OFFSET );
}


PTREE PtdDltDtorArr             // DECORATE FOR DELETE OF DTORABLE-ARRAY
    ( PTREE expr                // - expression
    , SYMBOL del )              // - operator delete
{
    return ptdSymbol( expr, del, PTD_DEL_DTOR_ARR );
}


PTREE PtdDltDtorElm             // DECORATE FOR DELETE OF DTORABLE-ELEMENT
    ( PTREE expr                // - expression
    , SYMBOL del )              // - operator delete
{
    return ptdSymbol( expr, del, PTD_DEL_DTOR_ELM );
}


PTREE PtdDltDtorSize            // DECORATE FOR SIZE OF DTORABLE-ELEMENT
    ( PTREE expr                // - expression
    , target_size_t size )      // - size
{
    return ptdSize( expr, size, PTD_DEL_DTOR_SIZE );
}


PTREE PtdDltDtorEnd             // DECORATE FOR END OF DTORABLE-ELEMENT ON DEL
    ( PTREE expr )              // - expression
{
    ScopeKeep( GetCurrScope() );
    FunctionHasRegistration();
    return ptdBase( expr, PTD_DEL_DTORED );
}


PTREE PtdDtorKind               // SPECIFY KIND OF DTOR ENTRY
    ( PTREE expr                // - expression
    , target_offset_t kind )    // - kind (DTC_COMP_...)
{
//    return ptdOffset( expr, kind, PTD_DTOR_KIND );
    kind = kind;
    return expr;
}


PTREE PtdOffsetofExpr           // DECORATE FOR OFFSETOF EXPR
    ( PTREE expr                // - expression
    , PTREE tree )              // - operator delete
{
    return ptdPTree( expr, tree, PTD_OFFSETOF_EXPR );
}

PTREE PtdGetOffsetofExpr(       // GET OFFSETOF EXPR
    PTREE expr )                // - expr
{
    PTREE tree;
    PTD *ptd;

    RingIterBeg( expr->decor, ptd ) {
        if( ptd->base.kind == PTD_OFFSETOF_EXPR ) {
            // detach and return
            tree = ptd->ptree.tree;
            ptd->ptree.tree = NULL;
            return( tree );
        }
    } RingIterEnd( ptd );
    return( NULL );
}


void PtdGenBefore               // GENERATE BEFORE NODE PROCESSED
    ( PTD* ring )               // - ring of entries
{
    PTD* ptd;                   // - current decoration

    RingIterBeg( ring, ptd ) {
        switch( ptd->base.kind ) {
          case PTD_CTORCOMP :
            CgFrontCodeUint( IC_COMPCTOR_BEG, ptd->off.offset );
            break;
          case PTD_DTOR_KIND :
            CgFrontCodeUint( IC_DTOR_KIND, ptd->off.offset );
            break;
          case PTD_OBJ_OFFSET :
            CgFrontCodeUint( IC_DTOBJ_OFF, ptd->off.offset );
            break;
          case PTD_OBJ_PUSH :
            CgFrontCodePtr( IC_DTOBJ_PUSH, ptd->type.type );
            break;
          case PTD_OBJ_SYM :
            CgFrontCodePtr( IC_DTOBJ_SYM, ptd->symbol.sym );
            break;
          case PTD_VB_DELTA :
            CgFrontCodeUint( IC_VB_DELTA, ptd->off.offset );
            break;
          case PTD_VB_EXACT :
            CgFrontCodeUint( IC_VB_EXACT, ptd->off.offset );
            break;
          case PTD_VB_INDEX :
            CgFrontCodeUint( IC_VB_INDEX, ptd->off.offset );
            break;
          case PTD_VB_OFFSET :
            CgFrontCodeUint( IC_VB_OFFSET, ptd->off.offset );
            break;
          case PTD_VF_INDEX :
            CgFrontCodeUint( IC_VF_INDEX, ptd->off.offset );
            break;
          case PTD_VF_OFFSET :
            CgFrontCodeUint( IC_VF_OFFSET, ptd->off.offset );
            break;
          case PTD_VF_SYM :
            CgFrontCodePtr( IC_VF_THIS, ptd->symbol.sym );
            break;
        }
    } RingIterEnd( ptd );
}


void PtdGenAfter                // GENERATE AFTER NODE PROCESSED
    ( PTD* ring )               // - ring of entries
{
    PTD* ptd;                   // - current decoration
    SYMBOL fun;                 // - function called
    SYMBOL dtor;                // - destructor
    DTOR_KIND kind;             // - kind of dtoring

    fun = NULL;
    dtor = NULL;
    kind = DTORING_NONE;
    RingIterBeg( ring, ptd ) {
        switch( ptd->base.kind ) {
          case PTD_CTORED_COMP :
            DbgVerify( kind == DTORING_NONE, "PtdGenAfter: dtoring twice" );
            kind = DTORING_COMPONENT;
            dtor = ptd->symbol.sym;
            break;
          case PTD_CTORCOMP :
            CgFrontCodeUint( IC_COMPCTOR_END, ptd->off.offset );
            break;
          case PTD_DEL_DTORED :
            CgFrontCode( IC_DLT_DTORED );
            break;
          case PTD_DEL_DTOR_ARR :
            CgFrontCodePtr( IC_DLT_DTOR_ARR, ptd->symbol.sym );
            break;
          case PTD_DEL_DTOR_ELM :
            CgFrontCodePtr( IC_DLT_DTOR_ELM, ptd->symbol.sym );
            break;
          case PTD_DEL_DTOR_SIZE :
            CgFrontCodeUint( IC_DLT_DTOR_SIZE, ptd->size.size );
            break;
          case PTD_DTOR_EXPR :
            DbgVerify( kind == DTORING_NONE, "PtdGenAfter: dtoring twice" );
            kind = DTORING_TEMP;
            dtor = ptd->symbol.sym;
            break;
          case PTD_DTOR_REF :
            CgFrontCodePtr( IC_DTOR_REF, ptd->symbol.sym );
            break;
          case PTD_DTOR_SCOPE :
            DbgVerify( kind == DTORING_NONE, "PtdGenAfter: dtoring twice" );
            kind = DTORING_SCOPE;
            dtor = ptd->symbol.sym;
            break;
          case PTD_DTOR_USE :
            CgFrontCodePtr( IC_DTOR_USE, ptd->symbol.sym );
            break;
          case PTD_EXPR_CONST :
            CgFrontCode( IC_EXPR_CONST );
            break;
          case PTD_INIT_SYM_END :
            PtdInitSymEnd( NULL, ptd->symbol.sym );
            break;
          case PTD_NEW_ALLOC :
            CgFrontCode( IC_NEW_ALLOC );
            break;
          case PTD_NEW_CTOR :
            LabelExprNewCtor();
            CgFrontCodePtr( IC_NEW_CTORED, ptd->type.type );
            break;
          case PTD_OBJ_POP :
            CgFrontCode( IC_DTOBJ_POP );
            break;
          case PTD_SCOPE_CALL :
            fun = ptd->symbol.sym;
            break;
          case PTD_RETNOPT_VAR :
            PtdRetnOptVar( NULL, ptd->symbol.sym );
            break;
          case PTD_RETNOPT_END :
            PtdRetnOptEnd( NULL );
            break;
          case PTD_THROW :
            CgFrontCode( IC_SCOPE_THROW );
            break;
        }
    } RingIterEnd( ptd );
    if( fun != NULL || dtor != NULL ) {
        CgFrontScopeCall( fun, dtor, kind );
    }
}


static void init(               // INITIALIZE PTREEDEC
    INITFINI* defn )            // - definition
{
    defn = defn;
    carvePTD = CarveCreate( sizeof( PTD ), 128 );
}


static void fini(               // COMPLETE PTREEDEC
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carvePTD );
}


INITDEFN( parse_tree_decoration, init, fini )

PTD *PtdGetIndex( PTD *e )
{
    return( CarveGetIndex( carvePTD, e ) );
}

PTD *PtdMapIndex( PTD *e )
{
    return( CarveMapIndex( carvePTD, e ) );
}

static void markFreePtd( void *p )
{
    PTD *s = p;

    s->base.kind = PTD_PCH_FREE;
}

static void savePtd( void *p, carve_walk_base *d )
{
    PTD *s = p;
    PTD *save_next;
    PTREE save_tree;
    SYMBOL save_sym;
    TYPE save_type;

    if( s->base.kind == PTD_PCH_FREE ) {
        return;
    }
    save_next = s->base.next;
    s->base.next = PtdGetIndex( save_next );
    switch( s->base.fmt ) {
    case PTD_FMT_SYMBOL:
        save_sym = s->symbol.sym;
        s->symbol.sym = SymbolGetIndex( save_sym );
        break;
    case PTD_FMT_TYPE:
        save_type = s->type.type;
        s->type.type = TypeGetIndex( save_type );
        break;
    case PTD_FMT_PTREE:
        save_tree = s->ptree.tree;
        s->ptree.tree = PTreeGetIndex( save_tree );
        break;
    }
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->base.next = save_next;
    switch( s->base.fmt ) {
    case PTD_FMT_SYMBOL:
        s->symbol.sym = save_sym;
        break;
    case PTD_FMT_TYPE:
        s->type.type = save_type;
        break;
    case PTD_FMT_PTREE:
        s->ptree.tree = save_tree;
        break;
    }
}

pch_status PCHWritePtds( void )
{
    auto carve_walk_base data;

    CarveWalkAllFree( carvePTD, markFreePtd );
    CarveWalkAll( carvePTD, savePtd, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadPtds( void )
{
    PTD *r;
    auto cvinit_t data;

    CarveInitStart( carvePTD, &data );
    for( ; (r = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *r );
        r->base.next = PtdMapIndex( r->base.next );
        switch( r->base.fmt ) {
        case PTD_FMT_BASE:
            break;
        case PTD_FMT_OFFSET:
            break;
        case PTD_FMT_SIZE:
            break;
        case PTD_FMT_SYMBOL:
            r->symbol.sym = SymbolMapIndex( r->symbol.sym );
            break;
        case PTD_FMT_TYPE:
            r->type.type = TypeMapIndex( r->type.type );
            break;
        case PTD_FMT_PTREE:
            r->ptree.tree = PTreeMapIndex( r->ptree.tree );
            break;
        DbgDefault( "incorrect PTD format" );
        }
    }
    return( PCHCB_OK );
}

pch_status PCHInitPtds( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carvePTD ) );
    } else {
        carvePTD = CarveRestart( carvePTD );
        CarveMapOptimize( carvePTD, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniPtds( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carvePTD );
    }
    return( PCHCB_OK );
}


#ifndef NDEBUG

#include <stdio.h>
#include "dbg.h"
#include "fmttype.h"

static char const *name_kind[] =    // names
{
    #define PTD_DEF( a ) #a
    PTD_DEFS
    #undef  PTD_DEF
};

void PtdPrint(                  // DEBUG: print decoration for a node
    int numb,                   // - number for node
    PTREE node )                // - the node
{
    PTD* curr;                  // - current entry
    char const * name;          // - name to be printed
    VBUF vbuf;

    printf( "%d. Decorated Node: %x\n", numb, node );
    RingIterBeg( node->decor, curr ) {
        if( curr->base.kind >= MAX_PTD_KIND ) {
            name = "PtdPrint -- bad PTD type";
        } else {
            name = name_kind[ curr->base.kind ];
        }
        printf( "  %s:", name );
        switch( curr->base.fmt ) {
          DbgDefault( "PtdPrint -- bad PTD format" );
          case PTD_FMT_BASE :
            printf( "\n" );
            break;
          case PTD_FMT_SYMBOL :
            printf( " %s\n", DbgSymNameFull( curr->symbol.sym, &vbuf ) );
            VbufFree( &vbuf );
            break;
          case PTD_FMT_OFFSET :
            printf( " offset = %x\n", curr->off.offset );
            break;
          case PTD_FMT_SIZE :
            printf( " size = %x\n", curr->size.size );
            break;
          case PTD_FMT_TYPE :
          { VBUF fmt_prefix, fmt_suffix;
            FormatType( curr->type.type, &fmt_prefix, &fmt_suffix );
            printf( " %s<id>%s\n", VbufString( &fmt_prefix ), VbufString( &fmt_suffix ) );
            VbufFree( &fmt_prefix );
            VbufFree( &fmt_suffix );
          } break;
        }
    } RingIterEnd( curr );
}


#endif
