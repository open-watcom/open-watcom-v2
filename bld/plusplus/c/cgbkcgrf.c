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


#include <float.h>
#include <stdlib.h>

#include "plusplus.h"
#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "errdefns.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cppdwarf.h"
#include "cgaux.h"
#include "cginfo.h"
#include "ring.h"
#include "stringl.h"
#include "rtfuncod.h"
#include "carve.h"
#include "label.h"
#include "vstk.h"
#include "context.h"
#include "name.h"
#include "ctexcept.h"
#include "symdbg.h"
#include "callgrph.h"
#include "cdopt.h"
#include "stats.h"
#include "reposit.h"
#include "scoperes.h"
#include "icopmask.h"
#include "pragdefn.h"
#include "rtti.h"

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #include "pragdefn.h"

    static void _DUMP_CGRF( char *msg, SYMBOL sym ) {
        if( PragDbgToggle.callgraph ) {
            printf( msg, DbgSymNameFull( sym ) );
        }
    }
#else
    #define _DUMP_CGRF( msg, sym )
#endif

typedef struct vft_defn VFT_DEFN;
struct vft_defn {               // VFT_DEFN -- definition for VFT
    VFT_DEFN* next;             // - next in ring
    SYMBOL vft;                 // - symbol for vft
    CGFILE* cgfile;             // - virtual file containing definition
    CGFILE_INS location;        // - where to scan for definition
};

typedef struct scope_dtor SCOPE_DTOR;
struct scope_dtor               // SCOPE_DTOR -- delayed DTORing for a scope
{   SCOPE_DTOR* next;           // - next in ring
    SYMBOL dtor;                // - dtor
    DTOR_KIND kind;             // - type of dtor
    unsigned :0;                // - aligning
};

typedef struct                  // SCOPE_INFO -- scope information
{   SCOPE scope;                // - scope in question
    SCOPE_STATE state;          // - current state
    SCOPE_DTOR* dtoring;        // - ring of delayed dtoring
} SCOPE_INFO;

typedef struct                  // DTOR_SCOPE -- dtoring in a scope
{   unsigned opcode;            // - type of dtoring
    SYMBOL dtor;                // - dtor symbol
    SCOPE scope;                // - scope for dtoring
} DTOR_SCOPE;

static VFT_DEFN *vft_defs;          // ring of vftable defns
static carve_t carve_vft;           // carver for vft definitions
static CALLGRAPH* call_graph;       // call graph information
static unsigned max_inline_depth;   // maximum depth of inlining
static unsigned oe_size;            // size for inlining static functions

static struct {
    unsigned    inline_recursion:1; // TRUE ==> inline recursion allowed
    unsigned    any_state_tables:1; // TRUE ==> state tables somewhere
    unsigned    only_once_found :1; // TRUE ==> ->once_inl fn somewhere
//  unsigned    not_inlined_set :1; // TRUE ==> not_inline set somewhere
} callGraphFlags;

typedef enum                    // TCF -- types of functions
{   TCF_MOD_INIT                // - module-init function
,   TCF_INLINE                  // - inline function
,   TCF_STATIC                  // - static, non-member
,   TCF_VFT                     // - virtual-function table
,   TCF_OTHER_FUNC              // - other function
,   TCF_NOT_FUNC                // - not a function
,   TCF_NULL                    // - NULL pointer
} TCF;

typedef struct                  // SCAN_INFO -- scanning information
{   CALLNODE* cnode;            // - current node
    CGFILE *file_ctl;           // - current file
    unsigned has_except_spec :1;// - TRUE ==> has except spec.
    SYMBOL scope_call_cmp_dtor; // - component dtor for scope-call optimization
    SYMBOL scope_call_tmp_dtor; // - temporary dtor for scope-call optimization
    SYMBOL scope_call_blk_dtor; // - scope dtor for scope-call optimization
    SCOPE curr_scope;           // - current scope
    unsigned func_dtm;          // - dtor method for function
} SCAN_INFO;

static void forceGeneration(    // FORCE CODE FILE TO BE GENERATED
    CALLNODE *node );           // - function in graph


#ifndef NDEBUG
    void CallGraphDump( void )
    {
        CgrfDump( call_graph );
    }

    static void _printScanInt( const char* msg, unsigned val )
    {
        if( PragDbgToggle.callgraph_scan ) {
            printf( msg, val );
        }
    }

#else
    #define _printScanInt( a, b )
#endif


ExtraRptCtr( ctr_funs_scanned );    // functions scanned
ExtraRptCtr( ctr_funs_rescanned );  // functions re-scanned
ExtraRptCtr( ctr_vfts_scanned );    // VFT's scanned
ExtraRptCtr( ctr_vfts_genned );     // VFT's generated
ExtraRptCtr( ctr_nodes_visited );   // nodes visited (measure of work)
ExtraRptCtr( ctr_gened );           // functions left after processing


static SYMBOL callNodeCaller(   // GET SYMBOL FOR CALLER FROM CALLNODE
    CALLNODE* cnode )           // - call node
{
    SYMBOL sym;                 // - caller symbol

    sym = cnode->base.object;
    return sym;
}


void CgrfMarkNodeGen(           // MARK NODE AS GEN-ED, IF REQ'D
    CALLNODE* cnode )           // - the node
{
    switch( cnode->dtor_method ) {
      case DTM_DIRECT :
        break;
      case DTM_DIRECT_SMALL :
      { SYMBOL func = callNodeCaller( cnode );
        if( NULL == func
         || ! SymIsDtor( func ) ) break;
      }
        // drops thru
      default :
        cnode->stab_gen = TRUE;
        break;
    }
}


static TCF cgbackFuncType(      // DETERMINE TYPE OF FUNCTION
    SYMBOL sym )                // - symbol
{
    TCF retn;                   // - type of function

    if( sym == NULL ) {
        retn = TCF_MOD_INIT;
    } else if( ! SymIsFunction( sym ) ) {
        if( SymIsVft( sym ) ) {
            retn = TCF_VFT;
        } else {
            retn = TCF_NOT_FUNC;
        }
    } else if( ! SymIsInitialized( sym ) ) {
        retn = TCF_OTHER_FUNC;
    } else if( SymIsInline( sym ) ) {
        retn = TCF_INLINE;
    } else if( oe_size != 0 ) {
        retn = TCF_STATIC;
    } else if( SymIsRegularStaticFunc( sym ) ) {
        retn = TCF_STATIC;
    } else {
        retn = TCF_OTHER_FUNC;
    }
    return retn;
}


static CGFILE* nodeCgFile(      // GET CGFILE FOR NODE
    CALLNODE* node )            // - the node
{
    CGFILE* retn;               // - the CGFILE

    retn = node->cgfile;
    if( retn == NULL ) {
        retn = CgioLocateFile( node->base.object );
        node->cgfile = retn;
    }
    return retn;
}


static boolean funcInlineable(  // DETERMINE IF FUNCTION INLINE-ABLE
    SYMBOL sym )                // - function symbol
{
    void *pragma;

    DbgAssert( sym != NULL );
    if( max_inline_depth == 0 ) {
        // no inlining allowed
        return( FALSE );
    }
    if( ! SymIsInitialized( sym ) ) {
        // sym has no code to inline
        return( FALSE );
    }
    if( SymIsUninlineable( sym ) ) {
        // sym has been tagged as not being inlineable
        // e.g., contains _asm code that references auto vars
        return( FALSE );
    }
    if( SymIsEllipsisFunc( sym ) ) {
        // function could not access parms through <stdarg.h> if inlined
        return( FALSE );
    }
    // used to have: if( TypeHasPragma( sym->sym_type ) ) (why? AFS)
    pragma = TypeHasPragma( sym->sym_type );
    if( pragma != NULL ) {
        if( ! PragmaOKForInlines( pragma ) ) {
            // something weird and system specific would cause a problem
            return( FALSE );
        }
    }
    return( TRUE );
}


static boolean oeInlineable(    // DETERMINE IF /oe CAN INLINE THE FUNCTION
    SYMBOL sym,                 // - symbol for node
    CGFILE *cgfile )            // - code gen file for function
{
    boolean retn;               // - TRUE ==> CAN BE /oe inlined

    if( funcInlineable( sym ) ) {
        if( cgfile == NULL ) {
            retn = FALSE;
        } else if( cgfile->oe_inl ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static boolean canBeOeInlined(  // DETERMINE IF /oe CAN INLINE THE FUNCTION
    CALLNODE* node )            // - node for function
{
    return oeInlineable( node->base.object, nodeCgFile( node ) );
}


static boolean shouldBeInlined( // DETERMINE IF INLINING AN INLINABLE FN IS OK
    SYMBOL sym )                // - function symbol
{
    FN_CTL *fctl;               // - current file control

    if( CgBackInlinedDepth() >= max_inline_depth ) {
        return( FALSE );
    }
    fctl = FnCtlTop();
    if( fctl == NULL || SymIsThunk( fctl->func ) ) {
        return FALSE;
    }
    if( ! callGraphFlags.inline_recursion ) {
        for( ; fctl != NULL; fctl = FnCtlPrev( fctl ) ) {
            if( sym == fctl->func ) return( FALSE );
        }
    }
    return( TRUE );
}


static CALLNODE* addNode(       // ADD A CALL NODE
    SYMBOL sym )                // - symbol for node
{
    CALLNODE* node = CgrfAddFunction( call_graph, sym );
    switch( cgbackFuncType( sym ) ) {
      case TCF_VFT :
        node->is_vft = TRUE;
        break;
      case TCF_INLINE :
        node->inline_fun = TRUE;
        if( funcInlineable( sym ) ) {
            node->inlineable = TRUE;
            sym->flag |= SF_CG_INLINEABLE;
        }
        break;
      case TCF_OTHER_FUNC :
      case TCF_STATIC :
        if( funcInlineable( sym ) ) {
            node->inlineable = TRUE;
            if( oe_size > 0 ) {
                node->inlineable_oe = canBeOeInlined( node );
                if( node->inlineable_oe ) {
                    sym->flag |= SF_CG_INLINEABLE;
                }
            }
        }
        break;
      case TCF_MOD_INIT :
        break;
      DbgDefault( "addNode -- bad type" );
    }
    return node;
}


static CALLNODE *addCallee(     // ADD CALL TO CALL GRAPH
    SYMBOL callee )             // - symbol called
{
    CALLNODE *retn;             // - node for callee
    SYMBOL *pfun;               // - addr[ stacked symbol ]

    retn = addNode( callee );
    if( ( retn->refs == 0 ) && ( retn->addrs == 0 ) ) {
        switch( cgbackFuncType( callee ) ) {
          case TCF_VFT :
            pfun = (SYMBOL*)VstkPush( &call_graph->calls );
            *pfun = callee;
            break;
          case TCF_INLINE :
          case TCF_STATIC :
            pfun = (SYMBOL*)VstkPush( &call_graph->calls );
            *pfun = callee;
            break;
        }
    }
    return retn;
}


static TCF addAddrOf(           // ADD ADDRESS-OF IF REQUIRED
    CALLNODE *cnode,            // - node in call graph for calling function
    SYMBOL callee )             // - function possibly addressed
{
    TCF tcf;
    CALLNODE *rnode;            // - node of referenced function

    if( callee != NULL ) {
        tcf = cgbackFuncType( callee );
        switch( tcf ) {
          case TCF_INLINE :
          case TCF_STATIC :
          case TCF_VFT :
            rnode = addCallee( callee );
            CgrfAddAddrOf( call_graph, cnode, rnode );
            break;
          case TCF_NOT_FUNC :
            break;
          default :
            callee->flag |= SF_ADDR_TAKEN;
        }
        return( tcf );
    }
    return( TCF_NULL );
}


// type signature reference causes address-of references from source node
// to member functions which are inline.
//
static void addTypeSigRefs(     // ADD REFERENCES FOR TYPE SIGNATURE
    CALLNODE *cnode,            // - node in call graph for calling function
    TYPE type )                 // - signature type
{
    TYPE_SIG *tsig;             // - type-signature for type

    tsig = BeTypeSignature( type );
    addAddrOf( cnode, tsig->dtor );
    addAddrOf( cnode, tsig->default_ctor );
    addAddrOf( cnode, tsig->copy_ctor );
}


static void addCalleeFunc(      // ADD FUNCTION CALL TO CALL GRAPH
    CALLNODE *cnode,            // - node in call graph for calling function
    SYMBOL callee )             // - called symbol
{
    switch( cgbackFuncType( callee ) ) {
      case TCF_INLINE :
      case TCF_STATIC :
        CgrfAddCall( call_graph, cnode, addCallee( callee ) );
        // drops thru
      case TCF_OTHER_FUNC :
//      funCalled( callee );
        break;
    }
}


// For a function to be generated, there has not been an addr-of taken
// for the function ==> generate one here.
//
static void addCalleeFuncToGen( // ADD FUNCTION CALL FOR FUNC. TO BE GEN'ED
    CALLNODE *cnode,            // - node in call graph for calling function
    SYMBOL callee )             // - called symbol
{
    addAddrOf( cnode, callee );
    addCalleeFunc( cnode, callee );
}


static void scanInit(           // INIT FOR COMMONALITY BETWEEN SCAN, RE-SCAN
    CALLNODE *cnode,            // - node in call graph for calling function
    CGFILE *file_ctl,           // - current file
    SCAN_INFO* scan )           // - scan information
{
    call_graph->scanning = cnode;
    call_graph->stmt_scope = FALSE;
    cnode->stmt_state = STS_NONE;
    cnode->stab_gen = FALSE;
    cnode->dtor_method = DTM_DIRECT;
    scan->cnode = cnode;
    scan->scope_call_blk_dtor = NULL;
    scan->scope_call_tmp_dtor = NULL;
    scan->scope_call_cmp_dtor = NULL;
    scan->has_except_spec = FALSE;
    scan->file_ctl = file_ctl;
    CgioOpenInput( file_ctl );
}

static void dtorRef(            // reference a dtor in state table
    CGINTER *ins,               // - IC instr
    SCAN_INFO *sc,              // - scan info
    CALLNODE *cnode )           // - node in call graph for calling function
{
    SYMBOL dtor = ins->value.pvalue;

    switch( sc->func_dtm ) {
      case DTM_DIRECT_TABLE :
        addAddrOf( cnode, dtor );
        // drops thru
      case DTM_DIRECT :
        addCalleeFuncToGen( cnode, dtor );
        break;
      default :
        addAddrOf( cnode, dtor );
        break;
    }
}


// Note: there are some conditions under which we could safely flush
//       the return optimizations.  These are not considered now to
//       remain conservative.
//
static void scanFunctionBody(   // SCAN FUNCTION FOR CALLS
    CALLNODE *cnode,            // - node in call graph for calling function
    CGFILE *file_ctl )          // - current file
{
    SCAN_INFO sc;               // - scan information
    CGINTER *ins;               // - current instruction
    unsigned opcodes;           // - number of significant opcodes
    TYPE curr_type;             // - current type in virtual file
    SYMBOL func;                // - name of function
    SYMBOL sym;                 // - symbol from IC
    TCF tcf;                    // - symbol classification

    if( file_ctl == NULL ) {
        return;
    }
    if( cnode->calls_done ) {
        return;
    }
    scanInit( cnode, file_ctl, &sc );
    ExtraRptIncrementCtr( ctr_funs_scanned );
    _DUMP_CGRF( "scan function body: %s\n", sc.file_ctl->symbol );
    func = sc.file_ctl->symbol;
    if( NULL != func ) {
        func->flag &= ~SF_ADDR_TAKEN;
        _printScanInt( "-- function flags: %x\n", func->flag );
    }
    opcodes = 0;
//  call_graph->assumed_longjump = FALSE;
//  call_graph->does_longjump = FALSE;
    for( ; ; ) {
        ins = CgioReadICMaskCount( sc.file_ctl, ICOPM_CALLGRAPH, ICOPM_OE_COUNT, &opcodes );
        // The following comment is a trigger for the ICMASK program to start
        // scanning for case IC_* patterns.
        // ICMASK BEGIN CALLGRAPH (do not remove)
        switch( ins->opcode ) {
          case IC_DTOR_DLT_BEG :
//          CgioReadICUntilOpcode( sc.file_ctl, IC_DTOR_DLT_END );
            continue;
          case IC_DTOR_DAR_BEG :
//          CgioReadICUntilOpcode( sc.file_ctl, IC_DTOR_DAR_END );
            continue;
          case IC_EXPR_TS :
            addTypeSigRefs( cnode, ins->value.pvalue );
            continue;
          case IC_DATA_PTR_SYM :
            addAddrOf( cnode, ins->value.pvalue );
            continue;
          case IC_DTOR_STATIC :
            addAddrOf( cnode, RoDtorFind( ins->value.pvalue ) );
            continue;
          case IC_SCOPE_CALL_BDTOR :
            if( call_graph->scope_call_opt ) {
                sc.scope_call_blk_dtor = ins->value.pvalue;
            } else {
                dtorRef( ins, &sc, cnode );
            }
            continue;
          case IC_SCOPE_CALL_CDTOR :
            if( call_graph->scope_call_opt ) {
                sc.scope_call_cmp_dtor = ins->value.pvalue;
            } else {
                dtorRef( ins, &sc, cnode );
            }
            continue;
          case IC_SCOPE_CALL_TDTOR :
            if( call_graph->scope_call_opt ) {
                sc.scope_call_tmp_dtor = ins->value.pvalue;
            } else {
                dtorRef( ins, &sc, cnode );
            }
            continue;
          case IC_SCOPE_CALL_FUN :
            if( ! call_graph->scope_call_opt ) continue;
            CgResScopeCall( cnode
                          , ins->value.pvalue
                          , sc.scope_call_cmp_dtor
                          , sc.scope_call_tmp_dtor
                          , sc.scope_call_blk_dtor );
            sc.scope_call_tmp_dtor = NULL;
            sc.scope_call_cmp_dtor = NULL;
            sc.scope_call_blk_dtor = NULL;
            continue;
          case IC_BLOCK_OPEN :
          case IC_BLOCK_DEAD :
          { SCOPE scope = ins->value.pvalue;
            if( NULL != scope && scope->try_catch ) {
                CgrfMarkNodeGen( cnode );
                scope->cg_stab = TRUE;
            }
            if( call_graph->scope_call_opt ) {
                sc.curr_scope = scope;
                CgResScBlkScanBegin( sc.curr_scope, cnode, sc.func_dtm );
            } else {
                if( NULL != scope ) {
                    switch( sc.func_dtm ) {
                      case DTM_DIRECT :
                        break;
                      default :
                        CgrfMarkNodeGen( cnode );
                        scope->cg_stab = TRUE;
                        break;
                    }
                }
            }
          } continue;
          case IC_BLOCK_DONE :
          case IC_BLOCK_END :
            if( ! call_graph->scope_call_opt ) continue;
            sc.curr_scope = CgResScScanEnd();
            continue;
          case IC_EXPR_TEMP:
            if( ! call_graph->scope_call_opt ) continue;
            CgResScStmtScanBegin( sc.curr_scope, cnode, sc.func_dtm );
            call_graph->stmt_scope = TRUE;
            continue;
          case IC_STMT_SCOPE_END :
            if( ! call_graph->scope_call_opt ) continue;
            if( call_graph->stmt_scope ) {
                CgResScScanEnd();
                call_graph->stmt_scope = FALSE;
            }
            continue;
          case IC_EXCEPT_SPEC :
            if( call_graph->scope_call_opt ) {
                CgResScopeGen( cnode );
            }
            sc.has_except_spec = TRUE;
            cnode->stab_gen = TRUE;
            // drops thru
          case IC_CATCH :
            addTypeSigRefs( cnode, ins->value.pvalue );
            continue;
          case IC_TRY :
            if( call_graph->scope_call_opt ) {
                CgResScopeGen( cnode );
                cnode->stmt_state = STS_GEN;
            }
            cnode->stab_gen = TRUE;
            continue;
          case IC_CALL_SETUP :
          { if( NULL != func
             && SymIsThunk( func ) ) {
              SYMBOL called = ins->value.pvalue;
              CALLNODE* called_node = addNode( called );
              forceGeneration( called_node );
            }
            addCalleeFunc( cnode, ins->value.pvalue );
          } continue;
          case IC_DTOR_SUBOBJS :
            cnode->state_table = TRUE;
            if( sc.func_dtm != DTM_DIRECT_SMALL ) continue;
            // drops thru
          case IC_DLT_DTORED :
            if( ! call_graph->scope_call_opt ) {
                cnode->state_table = TRUE;
                CgrfMarkNodeGen( cnode );
                continue;
            }
            // drops thru
          case IC_SCOPE_CALL_GEN :
            if( call_graph->scope_call_opt ) {
                CgResScopeGen( cnode );
            }
            continue;
          case IC_SCOPE_THROW :
            if( ! call_graph->scope_call_opt ) continue;
            CgResScopeThrow( cnode );
            continue;
          case IC_NEW_CTORED :
          { SYMBOL opdel = CgBackOpDelete( ins->value.pvalue );
            addAddrOf( cnode, opdel );
            if( call_graph->scope_call_opt ) {
                CgResScopeCall( cnode, NULL, NULL, opdel, NULL );
            }
          } continue;
          case IC_RTTI_REF :     // REFERENCE TO RTTI CLASS INFO
            RttiRef( ins->value.pvalue );
            continue;
          case IC_TYPEID_REF :     // REFERENCE TO TYPEID INFO
            TypeidRef( ins->value.pvalue );
            continue;
          case IC_VFT_REF :     // REFERENCE TO VFT
            addAddrOf( cnode, ins->value.pvalue );
            continue;
          case IC_FUNCTION_STAB :
            cnode->state_table = TRUE;
            cnode->cond_flags = ins->value.uvalue;
            continue;
          case IC_THROW_RO_BLK :
          { THROW_CNV_CTL ctl;  // - controls conversions
            target_size_t not_used; // - offset not used
            TYPE type;          // - a conversion from thrown object
            ThrowCnvInit( &ctl, ins->value.pvalue );
            for( ; ; ) {
                type = ThrowCnvType( &ctl, &not_used );
                if( type == NULL ) break;
                addTypeSigRefs( cnode, type );
            }
            ThrowCnvFini( &ctl );
          } continue;
          case IC_SET_TYPE :
            curr_type = ins->value.pvalue;
            continue;
          case IC_LEAF_NAME_FRONT :
            sym = ins->value.pvalue;
            curr_type = sym->sym_type;
            tcf = addAddrOf( cnode, sym );
            if( tcf == TCF_NOT_FUNC ) {
                type_flag flags;
                TypeGetActualFlags( sym->sym_type, &flags );
                if( flags & TF1_THREAD ) {
                    SegmentMarkUsed( SEG_TLS );
                }
            }
            continue;
          case IC_FUNCTION_DTM :
            sc.func_dtm = ins->value.uvalue;
            cnode->dtor_method = sc.func_dtm;
            if( DTM_DIRECT_SMALL == sc.func_dtm
             || DTM_TABLE_SMALL == sc.func_dtm ) {
                cnode->depth = max_inline_depth + 1;
                sc.file_ctl->not_inline = TRUE;
//              callGraphFlags.not_inlined_set = TRUE;
                _DUMP_CGRF( "static'ed dtor for -xds: %s\n", func );
            }
            continue;
          case IC_VFT_BEG :     // START OF VFT DEFINITION
          { VFT_DEFN* pvft;     // - points at vft definition
            SYMBOL sym;
            ExtraRptIncrementCtr( ctr_vfts_scanned );
            pvft = RingCarveAlloc( carve_vft, &vft_defs );
            sym = ins->value.pvalue;
            pvft->vft = sym;
            sym->flag &= ~SF_REFERENCED;
            pvft->cgfile = sc.file_ctl;
            pvft->location = CgioLastRead( sc.file_ctl );
            CgioReadICUntilOpcode( sc.file_ctl, IC_INIT_DONE );
          } continue;
          case IC_EOF :
            break;
          default:
            DbgNever();
        }
        // ICMASK END (do not remove)
        break;
    }
    cnode->opcodes = opcodes;
    CgioCloseInputFile( sc.file_ctl );
    func = callNodeCaller( cnode );
    _printScanInt( "-- function flags: %x\n"
                 , func == NULL ? 0 : func->flag );
}


static void pushCaller(         // PUSH A CALLER FOR DEPTH COMPUTATION
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    INLINEE *inl;               // - pushed inline entry

    inl = VstkPush( &ctl->calls );
    inl->callee = node;
    inl->expanded = FALSE;
}


static void scanVftDefn(        // SCAN VFT DEFINITION
    CALLNODE *cnode,            // - node in call graph for calling function
    SYMBOL vft,                 // - symbol for VFT
    CGFILE **pvfcg )            // - cached CGFILE pointer
{
    VFT_DEFN* srch;             // - current VFT in search
    VFT_DEFN* pvft;             // - VFT entry for symbol
    CGINTER* ins;               // - current instruction
    SYMBOL sym;                 // - vfn symbol

    if( cnode->calls_done ) {
        return;
    }
    call_graph->scanning = NULL;
    cnode->calls_done = TRUE;
    ExtraRptIncrementCtr( ctr_vfts_genned );
    _DUMP_CGRF( "scan VFT body: %s\n", vft );
    pvft = NULL;
    vft->flag |= SF_REFERENCED;
    RingIterBeg( vft_defs, srch ) {
        if( srch->vft == vft ) {
            pvft = srch;
            break;
        }
    } RingIterEnd( srch );
    DbgVerify( pvft != NULL, "ScanVftDefn -- can't find VFT" );
    if( pvft->cgfile != *pvfcg ) {
        if( *pvfcg != NULL ) {
            CgioCloseInputFile( *pvfcg );
        }
        *pvfcg = pvft->cgfile;
        CgioOpenInput( *pvfcg );
    }
    ins = CgioSeek( *pvfcg, &(pvft->location) );
    for(;;) {
        ins = CgioReadICMask( *pvfcg, ICOPM_VFT_SCAN );
        // The following comment is a trigger for the ICMASK program to start
        // scanning for case IC_* patterns.
        // ICMASK BEGIN VFT_SCAN (do not remove)
        switch( ins->opcode ) {
        case IC_INIT_DONE:
            break;
        case IC_DATA_PTR_SYM:
            sym = ins->value.pvalue;
            sym->flag |= SF_IN_VFT;
            addAddrOf( cnode, sym );
            continue;
        default:
            DbgNever();
        }
        // ICMASK END (do not remove)
        break;
    }
}


// initial pass building call graph
// - add only nodes for inlines, statics
// - otherwise, add node and scan body
// - inlines which cannot be inlined are made static
//
static void scanFunction(       // SCAN FUNCTION FOR CALLS
    CGFILE *file_ctl )          // - current file
{
    SYMBOL func;                // - function symbol
    CALLNODE *cnode;            // - node in call graph for calling function

    func = file_ctl->symbol;
    cnode = addNode( func );
    if( SymIsThunk( func ) ) {
        SegmentMarkUsed( func->segid );
    }
    switch( cgbackFuncType( func ) ) {
      case TCF_INLINE :
        if( ! cnode->inlineable ) {
            cnode->depth = max_inline_depth + 1;
            _DUMP_CGRF( "static'ed inline function: %s\n", func );
        }
        if( SymIsMustGen( func ) ) {
            scanFunctionBody( cnode, file_ctl );
            forceGeneration( cnode );
        }
        break;
      case TCF_STATIC :
#if 1
        if( ! SymIsRegularStaticFunc( func ) || SymIsMustGen( func ) ) {
            scanFunctionBody( cnode, file_ctl );
            forceGeneration( cnode );
        }
#else
        if( SymIsMustGen( func ) ) {
            scanFunctionBody( cnode, file_ctl );
            forceGeneration( cnode );
        } else if( ! SymIsRegularStaticFunc( func ) ) {
            scanFunctionBody( cnode, file_ctl );
            if( ! cnode->inlineable ) {
                forceGeneration( cnode );
            }
        }
#endif
        break;
      default :
        /* func may be NULL in this path */
        scanFunctionBody( cnode, file_ctl );
        if( func != NULL && SymIsMustGen( func ) ) {
            forceGeneration( cnode );
        }
    }
}


static void checkForExpandOnceInlines(  // scan for "at most 1" inline requests
    CALLGRAPH *ctl,                     // - control info
    INLINEE *last )                     // - last call to check
{
    INLINEE *stk;                       // - stacked inlinee
    CGFILE *cgfile;                     // - function codegen file
    SYMBOL func;                        // - called function

    stk = VstkTop( &ctl->calls );
    for(;;) {
        if( stk == NULL ) break;
        if( stk->expanded ) {
            func = stk->callee->base.object;
            if( func != NULL ) {
                cgfile = nodeCgFile( stk->callee );
                if( cgfile != NULL ) {
                    // found a max 1 inline function
                    if( cgfile->once_inl ) {
                        // make it out of line
                        cgfile->oe_inl = FALSE;
                        cgfile->once_inl = FALSE;
                    }
                }
            }
        }
        if( stk == last ) break;
        stk = VstkNext( &ctl->calls, stk );
    }
}


static boolean recursiveExpand( // DETERMINE IF RECURSIVE EXPANSION
    CALLGRAPH *ctl,             // - control information
    CALLNODE *target )          // - node for caller
{
    INLINEE *stk;               // - stacked inlinee

    stk = VstkTop( &ctl->calls );
    while( stk != NULL ) {
        if( stk->expanded && ( stk->callee == target ) ) {
            if( callGraphFlags.only_once_found ) {
                // a recursion will cause multiple expansions of an inline
                // in this call path (at least one inline + one out of line)
                checkForExpandOnceInlines( ctl, stk );
            }
            return( TRUE );
        }
        stk = VstkNext( &ctl->calls, stk );
    }
    return( FALSE );
}


static void markAsGen(          // MARK CODE FILE TO BE GENERATED
    CALLNODE *node )            // - function in graph
{
    SYMBOL func;                // - the function
    CGFILE *cgfile;             // - code gen file

    if( ! node->is_vft ) {
        cgfile = nodeCgFile( node );
        if( cgfile != NULL && ! cgfile->refed ) {
            func = callNodeCaller( node );
            ExtraRptIncrementCtr( ctr_gened );
            cgfile->refed = TRUE;
            if( NULL != func ) {
                SegmentMarkUsed( func->segid );
            }
        #ifndef NDEBUG
            if( PragDbgToggle.dump_emit_ic ) {
                printf( "Selected code file: %s\n"
                      , DbgSymNameFull( func ) );
            }
        #endif
        }
    }
}

static void forceGeneration(    // FORCE CODE FILE TO BE GENERATED
    CALLNODE *node )            // - function in graph
{
    markAsGen( node );
    // make sure we won't be pruned!
    node->addrs++;
}


static boolean procEdge(        // PROCESS EDGE IN CALL GRAPH
    CALLGRAPH *ctl,             // - control information
    CALLEDGE *edge )            // - call to inline/static
{
    CALLNODE *target;           // - node for caller

    ExtraRptIncrementCtr( ctr_nodes_visited );
    target = (CALLNODE*)edge->base.target;
    if( target->inlineable || target->inlineable_oe ) {
        if( recursiveExpand( ctl, target ) ) {
            target->depth = max_inline_depth + 1;
            markAsGen( target );
            if( ! target->flowed_recurse ) {
                target->flowed_recurse = TRUE;
                pushCaller( ctl, target );
            }
        } else {
            CALLNODE* curr_node;
            curr_node = ctl->curr_node;
            if( ! curr_node->is_vft ) {
                CGFILE* cgfile;
                cgfile = nodeCgFile( curr_node );
                cgfile->calls_inline = TRUE;
            }
            pushCaller( ctl, target );
        }
    } else {
        if( target->inline_fun ) {
            target->depth = max_inline_depth + 1;
            markAsGen( target );
        }
        target->depth = 1;
    }
    return FALSE;
}


static void genFunction(        // INDICATE FUNCTION NEEDS TO BE GEN'ED
    CALLNODE *node )            // - function in graph
{
    SYMBOL func;                // - function symbol

    func = node->base.object;
    switch( cgbackFuncType( func ) ) {
      case TCF_STATIC :
      { CGFILE* cgfile = nodeCgFile( node );
        if( ! cgfile->once_inl
         && ! cgfile->oe_inl ) {
            markAsGen( node );
            break;
        }
      } // drops thru
      case TCF_INLINE :
        if( node->depth > max_inline_depth ) {
            markAsGen( node );
        }
        break;
      case TCF_VFT :
        break;
      default :
        markAsGen( node );
        break;
    }
}


static boolean procInlineFunction( // PROCESS INLINE FUNCTIONS IN CALL GRAPH
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    INLINEE *inl;               // - stacked inline
    unsigned depth;             // - current depth
    SYMBOL func;                // - starting function

    ExtraRptIncrementCtr( ctr_nodes_visited );
    func = node->base.object;
    if( func != NULL
     && node->addrs == 0
     && node->inline_fun
     && node->inlineable ) {
        return( FALSE );
    }
    ctl->depth = 1;
    pushCaller( ctl, node );
    depth = 0;
    for( ; ; ) {
        inl = VstkTop( &ctl->calls );
        if( inl == NULL ) break;
        if( inl->expanded ) {
            inl = VstkPop( &ctl->calls );
            -- depth;
            if( depth == 0 ) {
                depth = max_inline_depth + 1;
            }
        } else {
            ExtraRptIncrementCtr( ctr_nodes_visited );
            inl->expanded = TRUE;
            node = inl->callee;
            if( depth > node->depth ) {
                node->depth = depth;
            }
            if( depth > max_inline_depth ) {
                genFunction( node );
                depth = 0;
            }
            ++ depth;
            ctl->depth = depth;
            CgrfWalkCalls( ctl, node, &procEdge );
        }
    }
    return FALSE;
}


// Static functions include all defined functions when /oe is specified
//
static boolean procStaticFunction( // PROCESS STATIC FUNCTIONS IN CALL GRAPH
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    SYMBOL func;                // - function
    CGFILE *cgfile;             // - cg file for function
    struct {
        unsigned oe_small : 1;  // - function is small enough to be -oe inlined
        unsigned oe_static : 1; // - static function is called once
    } flags;

    ctl = ctl;
    ExtraRptIncrementCtr( ctr_nodes_visited );
//  if( node->inlineable && node->addrs == 0 ) {
    if( node->inlineable ) {
        func = node->base.object;
        if( TCF_STATIC == cgbackFuncType( func ) ) {
            flags.oe_small = FALSE;
            flags.oe_static = FALSE;
            if( node->opcodes <= oe_size ) {
                flags.oe_small = TRUE;
#if 0
            } else if( node->refs == 1 && SymIsRegularStaticFunc( func ) ) {
                flags.oe_static = TRUE;
#else
            } else if( node->refs == 1  ) {
                flags.oe_static = TRUE;
#endif
            }
            if( flags.oe_small || flags.oe_static ) {
                cgfile = nodeCgFile( node );
                if( cgfile != NULL ) {
                    cgfile->oe_inl = TRUE;
                    if( flags.oe_static ) {
                    // we are inlining a static function that is called once
                    // (mark it so that it only gets inlined once!)
                        cgfile->once_inl = TRUE;
                        callGraphFlags.only_once_found = TRUE;
                        _DUMP_CGRF( "inlined once-called static function: %s\n", func );
                    } else {
                        _DUMP_CGRF( "inlined a global function: %s\n", func );
                    }
                }
            }
        }
    }
    return FALSE;
}


static void removeCodeFile(     // REMOVE CODE FILE FOR FUNCTION
    CALLNODE* node )            // - node of function to be removed
{
    CGFILE *cgfile;             // - code gen file
    SYMBOL func;                // - the function

    func = node->base.object;
    cgfile = nodeCgFile( node );
    if( node->inline_fun ) {
#ifndef NDEBUG
        if( PragDbgToggle.dump_emit_ic || PragDbgToggle.callgraph ) {
            printf( "Removed inline code file: %s\n"
                  , DbgSymNameFull( func ) );
        }
#endif
        CgioFreeFile( cgfile );
        // inlines that aren't going to be generated aren't really referenced
        func->flag &= ~SF_REFERENCED;
    } else if ( SymIsRegularStaticFunc( func ) ) {
#ifndef NDEBUG
        if( PragDbgToggle.dump_emit_ic || PragDbgToggle.callgraph ) {
            printf( "Removed static code file: %s\n"
                  , DbgSymNameFull( func ) );
        }
#endif
        CgioFreeFile( cgfile );
#ifndef NDEBUG
    } else {
        CFatal( "cgbkcgrf: removeCodeFile function not removable" );
#endif
    }
}


static boolean procFunction(    // POST-PROCESS FUNCTION IN CALL GRAPH
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    SYMBOL func;                // - function
    CGFILE* cgfile;             // - for function

    ctl = ctl;
    ExtraRptIncrementCtr( ctr_nodes_visited );
    if( node->state_table ) {
        callGraphFlags.any_state_tables = TRUE;
    }
    func = node->base.object;
    if( node->addrs > 0 ) {
        if( func != NULL ) {
            func->flag |= SF_ADDR_TAKEN;
        }
        markAsGen( node );
    } else if( ! CompFlags.inline_functions ) {
        markAsGen( node );
    } else {
        genFunction( node );
    }
    cgfile = nodeCgFile( node );
    if( cgfile != NULL ) {
        cgfile->state_table = node->state_table;
        cgfile->stab_gen = node->stab_gen;
        cgfile->cond_flags = node->cond_flags;
        if( cgfile->oe_inl ) {
            func->flag |= SF_CG_INLINEABLE;
        }
        if( cgfile->not_inline ) {
            func->flag &= ~SF_CG_INLINEABLE;
        }
    }
    return FALSE;
}


static boolean pruneFunction(   // PRUNE UNREFERENCED FUNCTION
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    SYMBOL sym;                 // - symbol for node

    ExtraRptIncrementCtr( ctr_nodes_visited );
    sym = node->base.object;
    switch( cgbackFuncType( sym ) ) {
      case TCF_STATIC :
        if( ! SymIsRegularStaticFunc( sym ) ) break;
      case TCF_INLINE :
        if( ( node->refs == 0 ) && ( node->addrs == 0 ) ) {
            _DUMP_CGRF( "pruned unreferenced function: %s\n", sym );
            removeCodeFile( node );
            CgrfPruneFunction( ctl, node );
        }
        break;
    }
    return FALSE;
}


static boolean procStabEdge(    // PROCESS INLINE-CALL EDGE FROM NODE
    CALLGRAPH *ctl,             // - control information
    CALLEDGE *edge )            // - call to inline/static
{
    CALLNODE *target;           // - node for caller

    ExtraRptIncrementCtr( ctr_nodes_visited );
    target = (CALLNODE*)edge->base.target;
    if( ! target->is_vft ) {
        if( target->inlineable || target->inlineable_oe ) {
            if( ! recursiveExpand( ctl, target ) ) {
                pushCaller( ctl, target );
            }
        }
    }
    return FALSE;
}


static boolean setFunctionStab( // SET STATE-TABLE INFO. FOR FUNCTION
    CALLGRAPH *ctl,             // - control information
    CALLNODE *node )            // - function in graph
{
    CGFILE *cgfile;             // - code gen file
    INLINEE *inl;               // - stacked inline
    unsigned depth;             // - current depth
    unsigned cond_flags;        // - # conditional flags
    unsigned max_cond_flags;    // - maximum # conditional flags
    boolean state_table;        // - TRUE ==> requires state table
    boolean stab_gen;           // - TRUE ==> state table to be genned

    ExtraRptIncrementCtr( ctr_nodes_visited );
    if( ! node->is_vft ) {
        cgfile = nodeCgFile( node );
        if( cgfile != NULL ) {
            state_table = node->state_table;
            stab_gen = node->stab_gen;
            cond_flags = 0;
            max_cond_flags = 0;
            pushCaller( ctl, node );
            for( depth = 0; ; ) {
                inl = VstkTop( &ctl->calls );
                if( inl == NULL ) break;
                if( inl->expanded ) {
                    inl = VstkPop( &ctl->calls );
                    -- depth;
                    node = inl->callee;
                    cond_flags -= node->cond_flags;
                } else {
                    inl->expanded = TRUE;
                    node = inl->callee;
                    if( node->state_table ) {
                        state_table = TRUE;
                        cond_flags += node->cond_flags;
                        if( cond_flags > max_cond_flags ) {
                            max_cond_flags = cond_flags;
                        }
                        if( node->stab_gen ) {
                            stab_gen = TRUE;
                        }
                    }
                    ++ depth;
                    if( depth <= max_inline_depth ) {
                        ctl->depth = depth;
                        CgrfWalkCalls( ctl, node, &procStabEdge );
                    }
                }
            }
            cgfile->state_table = state_table;
            cgfile->stab_gen = stab_gen;
            cgfile->cond_flags = max_cond_flags;
#ifndef NDEBUG
            if( PragDbgToggle.dump_emit_ic ||
                PragDbgToggle.callgraph         ||
                PragDbgToggle.dump_stab ) {
                SYMBOL func = cgfile->symbol;
                if( state_table ) {
                    printf( "%s has state table: flags = %d gen=%d\n"
                          , DbgSymNameFull( func )
                          , max_cond_flags
                          , stab_gen );
                } else {
                    printf( "%s has no state table\n"
                          , DbgSymNameFull( func ) );
                }
            }
#endif
        }
    }
    return FALSE;
}


void MarkFuncsToGen(            // DETERMINE FUNCTIONS TO BE GENERATED
    unsigned bounding_depth )   // - maximum inline depth
{
    CALLGRAPH ctl;              // - control information
    SYMBOL *pfunc;              // - ptr[ stacked inline ]
    CGFILE *vfcg;               // - current VFTDefn CGFILE

#ifndef NDEBUG
    unsigned int dbg_dump_exec = PragDbgToggle.dump_exec_ic;
    PragDbgToggle.dump_exec_ic = 0;
    if( PragDbgToggle.callgraph_scan ) {
        PragDbgToggle.callgraph = 1;
    }
#endif
    vft_defs = NULL;
    DbgCgioEndFront();
    carve_vft = CarveCreate( sizeof( VFT_DEFN ), 16 );
    SegmentCodeCgInit();
    callGraphFlags.any_state_tables = FALSE;
    callGraphFlags.only_once_found = FALSE;
//  callGraphFlags.not_inlined_set = FALSE;
    if( ! CompFlags.inline_functions ) {
        max_inline_depth = 0;
        ctl.scope_call_opt = FALSE;
    } else {
        max_inline_depth = bounding_depth;
        if( ! CompFlags.dt_method_pragma
         && ( CompInfo.dt_method_speced == DTM_DIRECT
           || CompInfo.dt_method_speced == DTM_DIRECT_SMALL
           || CompInfo.dt_method_speced == DTM_TABLE_SMALL ) ) {
            ctl.scope_call_opt = FALSE;
        } else {
            ctl.scope_call_opt = TRUE;
        }
    }
    call_graph = &ctl;
    CgrfInit( &ctl );
    ExtraRptRegisterCtr( &ctr_funs_scanned
                       , "# call-graph functions scanned" );
    ExtraRptRegisterCtr( &ctr_funs_rescanned
                       , "# call-graph functions re-scanned" );
    ExtraRptRegisterCtr( &ctr_nodes_visited
                       , "# call-graph nodes visited" );
    ExtraRptRegisterCtr( &ctr_gened
                       , "# call-graph functions to be gen'ed" );
    ExtraRptRegisterAvg( &ctr_nodes_visited
                       , &ctr_gened
                       , "average visits per function to be gen'ed" );
    ExtraRptRegisterCtr( &ctr_vfts_scanned
                       , "# call-graph VFT's scanned" );
    ExtraRptRegisterCtr( &ctr_vfts_genned
                       , "# call-graph VFT's generated" );
    // start with functions that will be generated and expand a call graph
    VstkOpen( &ctl.calls, sizeof( SYMBOL ), 32 );
    if( ctl.scope_call_opt ) {
        CgResInit();
    }
    CgioWalkFiles( &scanFunction );
    vfcg = NULL;
    for( ; ; ) {
        for(;;) {
            SYMBOL sym;
            CALLNODE* node;
            pfunc = VstkPop( &ctl.calls );
            if( pfunc == NULL ) break;
            sym = *pfunc;
            node = addNode( sym );
            if( TCF_VFT == cgbackFuncType( sym ) ) {
                scanVftDefn( node, sym, &vfcg );
            } else {
                CGFILE* code_file = nodeCgFile( node );
                if( code_file == NULL ) {   // default was not gen'ed
                    CgrfPruneFunction( &ctl, node );
                } else {
                    scanFunctionBody( node, code_file );
                }
            }
        }
        if( ctl.scope_call_opt ) {
            CgrfWalkFunctions( &ctl, &CgResolveNonThrow );
        }
        if( NULL == VstkTop( &ctl.calls ) ) break;
    }
    if( vfcg != NULL ) {
        CgioCloseInputFile( vfcg );
    }
    if( ctl.scope_call_opt ) {
        CgResFini();
    }
    VstkClose( &ctl.calls );
    // prune out internal linkage functions that are never called
    // or do not have their address taken
    VstkOpen( &ctl.calls, sizeof( INLINEE ), 32 );
    do {
        ctl.pruned = FALSE;
        CgrfWalkFunctions( &ctl, &pruneFunction );
    } while( ctl.pruned );
    if( max_inline_depth > 0 ) {
        if( oe_size > 0 ) {
            CgrfWalkFunctions( &ctl, &procStaticFunction );
        }
        CgrfWalkFunctions( &ctl, &procInlineFunction );
        CgrfWalkFunctions( &ctl, &procFunction );
        if( callGraphFlags.any_state_tables ) {
            CgrfWalkFunctions( &ctl, &setFunctionStab );
        }
    } else {
        CgrfWalkFunctions( &ctl, &procFunction );
    }
    VstkClose( &ctl.calls );
#ifndef NDEBUG
    PragDbgToggle.dump_exec_ic = dbg_dump_exec;
    if( PragDbgToggle.callgraph ) {
        CgrfDump( &ctl );
    }
#endif
    CgrfFini( &ctl );
    CarveDestroy( carve_vft );
    DbgCgioEndCallGr();
}


void CgBackSetOeSize(           // SET SIZE FOR INLINING STATICS
    unsigned value )            // - size specified with -oe
{
    oe_size = value;
}


boolean CgBackFuncInlined(      // DETERMINE IF FUNCTION INVOCATION INLINED
    SYMBOL sym )                // - function symbol
{
    return ( sym->flag & SF_CG_INLINEABLE )
        && shouldBeInlined( sym );
}


void CgBackSetInlineRecursion(  // SET INLINE RECURSION
    boolean allowed )           // - TRUE ==> inline recursion allowed
{
    callGraphFlags.inline_recursion = allowed;
}


boolean CgBackGetInlineRecursion(// GET INLINE RECURSION
    void )
{
    return( callGraphFlags.inline_recursion );
}



// Support for CgrfScop


CALLNODE* CgrfCallNode(         // GET CALLNODE FOR FUNCTION
    SYMBOL fun )                // - function
{
    return addCallee( fun );
}


CALLNODE* CgrfDtorCall(         // DTOR CALL HAS BEEN ESTABLISHED
    CALLNODE* owner,            // - owner
    SYMBOL dtor )               // - dtor called
{
    dtor->flag |= SF_REFERENCED;
    addCalleeFuncToGen( owner, dtor );
    return owner;
}


CALLNODE* CgrfDtorAddr(         // DTOR ADDR-OF HAS BEEN ESTABLISHED
    CALLNODE* owner,            // - owner
    SYMBOL dtor )               // - dtor called
{
    dtor->flag |= SF_REFERENCED | SF_ADDR_TAKEN;
    addAddrOf( owner, dtor );
    return owner;
}

#ifndef NDEBUG

void* DbgCallGraph( void )
{
    return call_graph;
}

#endif
