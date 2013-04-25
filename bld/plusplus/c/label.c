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
#include "fnbody.h"
#include "carve.h"
#include "cgfront.h"
#include "errdefns.h"
#include "ring.h"
#include "codegen.h"
#include "vstk.h"
#include "initdefs.h"
#include "pcheader.h"

typedef struct blk_init BLK_INIT;
struct blk_init {               // BLK_INIT -- describes blocks for init. dcls
    BLK_INIT    *next;          // - next at this level (in a ring)
    BLK_INIT    *containing;    // - containing (enclosing) block
    BLK_INIT    *contains;      // - ring of contained blocks
    SCOPE       scope;          // - scope for block
    SYMBOL      first_init;     // - first initialized symbol in block
    SYMBOL      sym;            // - last symbol defined
    SYMBOL      sym_containing; // - last symbol defined for containing block
    SYMBOL      sym_dtored;     // - last symbol DTORed
    SYMBOL      sym_dtored_containing;//- last symbol DTORed in containing block
    SYMBOL      dcled_dtored;   // - last non-temporary DTORed
    unsigned    var_no;         // - initialization no. for self
    unsigned    var_no_containing; // - initialization no. for containing block
    void        *try_id;        // - id of try, when catch block
    CGFILE_INS  open_ins;       // - location of IC_BLOCK_OPEN, IC_BLOCK_DEAD
    TOKEN_LOCN  locn;           // - location of definition
    LAB_POSN    switch_posn;    // - position at switch/try statement
    unsigned    open_zap : 1;   // - need to zap IC_BLOCK_OPEN
    unsigned    dead_zap : 1;   // - need to zap IC_BLOCK_DEAD
    unsigned    try_blk : 1;    // - is a try block
    unsigned    catch_blk : 1;  // - is a catch block
    unsigned    free : 1;       // used for precompiled headers
};

static BLK_INIT *block_init_hdr;// - header for current block
static BLK_INIT block_init_mod; // - block initialization for module
static CGFILE_INS ins_temp;     // - zapped to IC_EXPR_TEMP

                                // carving control
static carve_t carveLAB_DEF;    // - LAB_DEF
static carve_t carveLAB_REF;    // - LAB_REF
static carve_t carveBLK_INIT;   // - BLK_INIT

typedef struct                  // COND_BLK -- conditional block
{   CGFILE_INS ins_true;        // - zapped to IC_SET_TRUE
    CGFILE_INS ins_false;       // - zapped to IC_SET_FALSE
    SYMBOL sym_dtored;          // - last DTOR'ed symbol at start of block
    unsigned has_false :1;      // - TRUE ==> has false part (?:)
} COND_BLK;

static VSTK_CTL stack_cond;     // stack of conditionals
static SYMBOL expr_dtor_bound;  // last symbol requiring DTOR in block
static unsigned expr_dtor_depth;// depth of conditional DTORing

static struct {
    unsigned    has_setjmp : 1; // TRUE ==> setjmp in expression
    unsigned    has_newctor: 1; // TRUE ==> has a new-ctor
} labelFlags;


static SYMBOL blkDtorSymbol(    // GET LAST DTORABLE SYMBOL FOR A BLOCK
    BLK_INIT* blk )             // - initialization block
{
    SYMBOL dtor_sym;            // - the dtorable symbol

    dtor_sym = blk->sym_dtored;
    if( dtor_sym == NULL ) {
        dtor_sym = blk->sym_dtored_containing;
    }
    return dtor_sym;
}


static SYMBOL blkDcledSymbol(   // GET LAST DTORABLE DCL'ED SYMBOL FOR A BLOCK
    BLK_INIT* blk )             // - initialization block
{
    SYMBOL dtor_sym;            // - the dtorable symbol

    dtor_sym = blk->dcled_dtored;
    if( dtor_sym == NULL ) {
        dtor_sym = blk->sym_dtored_containing;
    }
    return dtor_sym;
}


static BLK_INIT *labelFindBlk(  // FIND INITIALIZATION BLOCK FOR SCOPE
    SCOPE scope )               // - search scope
{
    BLK_INIT *curr;             // - block for current scope
    BLK_INIT *containing;       // - block containing current scope
    BLK_INIT **container;       // - Addr( ring( contained blocks ) )
    BLK_INIT *cont;             // - current block in contained ring
    BLK_INIT *next;             // - next block in contained ring
    BLK_INIT *last;             // - last block in contained ring

    containing = NULL;
    container = &block_init_hdr;
    for( ; ; ) {
//      curr = RingLookup( *container, &labelBlockEncloses, scope );
        curr = NULL;
        RingIterBeg( *container, next ) {
            if( ScopeEnclosed( next->scope, scope ) ) {
                curr = next;
                break;
            }
        } RingIterEnd( next );
        if( curr == NULL ) {
            curr = CarveAlloc( carveBLK_INIT );
            curr->scope = scope;
            curr->contains = NULL;
            curr->containing = containing;
            if( containing == NULL ) {
                curr->var_no_containing = 0;
                curr->sym_containing = NULL;
                curr->sym_dtored_containing = NULL;
                curr->dcled_dtored = NULL;
            } else {
                curr->var_no_containing = containing->var_no;
                curr->sym_containing = containing->sym;
                curr->sym_dtored_containing = blkDcledSymbol( containing );
                curr->dcled_dtored = containing->dcled_dtored;
            }
            curr->var_no = 0;
            curr->first_init = NULL;
            curr->sym = NULL;
            curr->sym_dtored = NULL;
            curr->open_zap = FALSE;
            curr->dead_zap = FALSE;
            curr->try_blk = FALSE;
            curr->catch_blk = FALSE;
            curr->free = FALSE;
            curr->locn.src_file = NULL;
            curr->try_id = NULL;
            curr->switch_posn.scope = NULL;
            curr->switch_posn.sym = NULL;
            curr->switch_posn.var_no = 0;
            last = *container;
            if( last != NULL ) {
                next = last->next;
                do{
                    cont = next;
                    next = cont->next;
                    if( ScopeEnclosed( scope, cont->scope ) ) {
                        RingPrune( container, cont );
                        RingAppend( &curr->contains, cont );
                        cont->containing = curr;
                    }
                }while( cont != last );
            }
            RingAppend( container, curr );
            break;
        }
        if( curr->scope == scope ) {
            break;
        }
        containing = curr;
        container = &curr->contains;
    }
    return( curr );
}


static SYMBOL scopeDtorSymbol(  // GET LAST DTORABLE SYMBOL FOR A SCOPE
    SCOPE scope )
{
    return blkDtorSymbol( labelFindBlk( scope ) );
}


static SYMBOL scopeDcledSymbol( // GET LAST DCL'ED DTORABLE SYMBOL FOR A SCOPE
    SCOPE scope )
{
    return blkDcledSymbol( labelFindBlk( scope ) );
}


static SYMBOL currDtorSymbol(   // GET LAST DTORABLE SYMBOL FOR CurrScope
    void )
{
    return scopeDtorSymbol( GetCurrScope() );
}


static SYMBOL currDcledSymbol(  // GET LAST DTORABLE DCL'ED SYMBOL, CurrScope
    void )
{
    return scopeDcledSymbol( GetCurrScope() );
}


static SCOPE dtorScope(         // FIND SIGNIFICANT SCOPE FOR DTOR'ING
    BLK_INIT *blk )             // - starting initialization block
{
    SCOPE scope;                // - significant scope

    for( ; ; blk = blk->containing ) {
        if( blk == NULL ) {
            scope = NULL;
            break;
        }
        if( blk->catch_blk ) {
            scope = blk->scope;
            break;
        }
        if( blk->dcled_dtored != blk->sym_dtored_containing ) {
            scope = blk->scope;
            break;
        }
    }
    return scope;
}


static boolean labelMarkDtorSym(// MARK A SYMBOL FOR DTORing
    BLK_INIT *blk,              // - current initialization block
    SYMBOL sym )                // - symbol
{
    boolean retn;               // - TRUE ==> requires DTOR
    SYMBOL dtor;                // - DTOR for symbol

    if( SymRequiresDtoring( sym ) ) {
        if( TypeTruncByMemModel( sym->sym_type ) ) {
            CErr1( ERR_DTOR_OBJ_MEM_MODEL );
        }
        if( TypeExactDtorable( sym->sym_type ) ) {
            dtor = RoDtorFind( sym );
            dtor->flag |= SF_ADDR_TAKEN;
            if( ! SymIsModuleDtorable( sym ) ) {
                blk->sym_dtored = sym;
                blk->scope->s.dtor_reqd = TRUE;
                ScopeKeep( blk->scope );
            }
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


void LabelDeclInited(           // SIGNAL NEXT INITIALIZATION IN BLOCK
    SYMBOL sym )                // - symbol initialized
{
    BLK_INIT *blk;              // - current initialization block

    if( SymRequiresDtoring( sym ) && ! SymRequiresCtoring( sym ) ) {
        GetCurrScope()->s.dtor_naked = TRUE;
    }
    blk = labelFindBlk( SymScope( sym ) );
    if( SymIsInitialized( sym ) ) {
        blk->sym = sym;
        ++blk->var_no;
        if( blk->first_init == NULL ) {
            blk->first_init = sym;
        }
    }
    if( SymIsAutomatic( sym ) ) {
        if( labelMarkDtorSym( blk, sym ) ) {
            blk->dcled_dtored = sym;
        }
    }
}


void LabelDeclInitedFileScope(  // SIGNAL NEXT INITIALIZATION IN FILE SCOPE
    SYMBOL sym )                // - symbol requiring DTORing
{
    if( block_init_mod.scope == NULL ) {
        block_init_mod.scope = ModuleInitScope();
    }
    block_init_mod.sym = sym;
    ++block_init_mod.var_no;
    if( block_init_mod.first_init == NULL ) {
        block_init_mod.first_init = sym;
    }
    labelMarkDtorSym( &block_init_mod, sym );
}


static void labelCurrPosn(      // SET LABEL POSITION IN CURRENT SCOPE
    LAB_POSN *posn )            // - current position
{
    BLK_INIT *blk;              // - current initialization block

    ScopeKeep( GetCurrScope() );
    blk = labelFindBlk( GetCurrScope() );
    posn->sym = blkDcledSymbol( blk );
    posn->scope = blk->scope;
    posn->var_no = blk->var_no;
}


LAB_DEF *LabelAllocLabDef(      // ALLOCATE A LABEL DEFINITION
    void )
{
    LAB_DEF *def;               // - new label definition

    def = CarveAlloc( carveLAB_DEF );
    labelCurrPosn( &def->posn );
    def->forward = NULL;
    SrcFileGetTokenLocn( &def->locn );
    return( def );
}


LAB_REF *LabelAllocLabRef(      // ALLOCATE A LABEL REFERENCE
    LAB_DEF *def )              // - definition for label
{
    LAB_REF *ref;               // - new label reference

    ref = CarveAlloc( carveLAB_REF );
    ref->defn = def;
    ref->next = NULL;
    ref->ins_exit.block = 0;
    ref->ins_exit.offset = 0;
    ref->ins_enter.block = 0;
    ref->ins_enter.offset = 0;
    labelCurrPosn( &ref->posn );
    return( ref );
}


static BLK_INIT *findCommonBlk( // FIND BLK_INIT FOR COMMON SCOPE
    SCOPE src,                  // - a scope
    SCOPE tgt )                 // - a scope
{
    for( ; ! ScopeEnclosed( src, tgt ); src = src->enclosing );
    ScopeKeep( src );
    return( labelFindBlk( src ) );
}

static void bypassError(        // BYPASS OF INIT/CTOR DETECTED!
    SYMBOL var )                // - symbol by-passed
{
    if( SymRequiresCtoring( var ) ) {
        CErr2p( ERR_CTOR_BYPASSED, var );
    } else {
        CErr2p( ANSI_INIT_BYPASSED, var );
    }
}


static boolean popsTryCatch(    // CHECK IF JUMP POPS A TRY/CATCH BLOCK
    BLK_INIT* src,              // - source block
    BLK_INIT* tgt )             // - target block
{
    boolean popped;             // - TRUE ==> catch was popped

    popped = FALSE;
    for( ; tgt != src; src = src->containing ) {
        if( src->try_blk || src->catch_blk ) {
            popped = TRUE;
            break;
        }
    }
    return popped;
}


static boolean popsCatch(       // CHECK IF JUMP POPS A CATCH BLOCK
    BLK_INIT* src,              // - source block
    BLK_INIT* tgt )             // - target block
{
    boolean popped;             // - TRUE ==> catch was popped

    popped = FALSE;
    for( ; tgt != src; src = src->containing ) {
        if( src->catch_blk ) {
            popped = TRUE;
            break;
        }
    }
    return popped;
}


static boolean labelCheckJump(  // CHECK JUMP DOES NOT BY-PASS INITIALIZATION
    LAB_POSN *src,              // - source (goto)
    LAB_POSN *tgt,              // - target (label)
    BLK_INIT **cblk,            // - addr( common block ptr )
    SYMBOL *cvar )              // - addr( common variable )
{
    BLK_INIT *blk;              // - BLK_INIT's for label definition
    BLK_INIT *blk_com;          // - BLK_INIT's for common scope
    BLK_INIT *blk_src;          // - BLK_INIT for source of jump
    boolean retn;               // - TRUE ==> success
    unsigned src_var_no;        // - source: variable no.
    unsigned tgt_var_no;        // - target: variable no.
    SYMBOL tgt_sym;             // - target: init. symbol

    blk_src = labelFindBlk( src->scope );
    blk_com = findCommonBlk( src->scope, tgt->scope );
    tgt_sym = tgt->sym;
    tgt_var_no = tgt->var_no;
    for( blk = labelFindBlk( tgt->scope ); ; blk = blk->containing ) {
        if( blk == blk_com ) {
            src_var_no = src->var_no;
            for( blk = blk_src; ; blk = blk->containing ) {
                if( blk == blk_com ) {
                    if( src_var_no < tgt_var_no ) {
                        bypassError( blk->sym );
                        retn = FALSE;
                    } else {
                        *cblk = blk_com;
                        *cvar = tgt_sym;
                        retn = TRUE;
                    }
                    break;
                }
                src_var_no = blk->var_no_containing;
            }
            break;
        }
        if( blk->try_blk ) {
            CErr1( ERR_JUMP_INTO_TRY );
            InfMsgPtr( INF_PREVIOUS_TRY, &blk->locn );
            retn = FALSE;
            break;
        } else if( blk->catch_blk ) {
            CErr1( ERR_JUMP_INTO_CATCH );
            InfMsgPtr( INF_PREVIOUS_CATCH, &blk->locn );
            retn = FALSE;
            break;
        }
        if( tgt_var_no != 0 ) {
            bypassError( blk->first_init );
            retn = FALSE;
            break;
        }
        tgt_sym = blk->sym_containing;
        tgt_var_no = blk->var_no_containing;
    }
    return( retn );
}


// This routine is called for a label, after the label has been planted.
//
void LabelDefine(               // DEFINE A LABEL
    LAB_DEF *def )              // - label definition
{
    LAB_POSN curr;              // - current position
    BLK_INIT *com;              // - common BLK_INIT
    BLK_INIT *src;              // - goto source BLK_INIT
    BLK_INIT *tgt;              // - goto target BLK_INIT
    SYMBOL sym;                 // - common symbol
    boolean ok;                 // - label is ok
    LAB_REF *ref;               // - forward reference to label
    SYMBOL entry_sym;           // - DTORable symbol at label
    boolean set_label_state;    // - TRUE ==> set state at label

    labelCurrPosn( &curr );
    ok = labelCheckJump( &def->posn, &curr, &com, &sym );
    if( ok ) {
        set_label_state = FALSE;
        tgt = labelFindBlk( curr.scope );
        entry_sym = scopeDcledSymbol( curr.scope );
        RingIterBeg( def->forward, ref ) {
            SYMBOL label_dtor = curr.sym;
            src = labelFindBlk( ref->posn.scope );
            com = findCommonBlk( ref->posn.scope, curr.scope );
            if( blkDcledSymbol( src ) != entry_sym ) {
                label_dtor = entry_sym;
                CgFrontZapPtr( ref->ins_exit, IC_DESTRUCT_VAR, entry_sym );
            } else if( popsTryCatch( src, com ) ) {
                ScopeKeep( com->scope );
                CgFrontZapPtr( ref->ins_exit, IC_TRY_CATCH_DONE, com->scope );
            }
            if( com == tgt ) {
                if( ref->posn.sym != label_dtor ) {
                    set_label_state = TRUE;
                }
            } else {
                if( blkDtorSymbol( com ) != label_dtor ) {
                    set_label_state = TRUE;
                }
            }
        } RingIterEnd( ref );
        if( set_label_state ) {
            CgFrontCode( IC_SET_LABEL_SV );
        }
    }
    def->posn = curr;
    SrcFileGetTokenLocn( &def->locn );
}


static void checkLabelState(    // CHECK LABEL STATE SAME AS DEF'N STATE
    LAB_POSN* src,              // - source position
    LAB_POSN* tgt )             // - target position
{
    if( src->sym != tgt->sym ) {
        CgFrontCode( IC_SET_LABEL_SV );
    }
}


void LabelGotoFwd(              // CHECK A GOTO (FOREWARDS)
    LAB_REF *ref )              // - reference for goto (defined) label
{
    CgFrontCode( IC_NO_OP );    // zapped to IC_DESTRUCT
    ref->ins_exit = CgFrontLastIns();
    RingAppend( &ref->defn->forward, ref );
}


void LabelGotoBwd(              // CHECK A GOTO (BACKWARDS)
    LAB_REF *ref )              // - reference for goto (defined) label
{
    SYMBOL tgt_dtor;            // - dtorable symbol in target
    BLK_INIT *blk;              // - common BLK_INIT
    SYMBOL sym;                 // - common variable

    if( labelCheckJump( &ref->posn, &ref->defn->posn, &blk, &sym ) ) {
        tgt_dtor = ref->defn->posn.sym;
        if( tgt_dtor != currDcledSymbol() ) {
            CgFrontCodePtr( IC_DESTRUCT_VAR, tgt_dtor );
        } else {
            BLK_INIT *src_blk = labelFindBlk( GetCurrScope() );
            if( popsTryCatch( src_blk, blk ) ) {
                ScopeKeep( blk->scope );
                CgFrontCodePtr( IC_TRY_CATCH_DONE, blk->scope );
            }
        }
    }
    LabelRefFree( ref );
}


static void checkSwitchState(   // CHECK SWITCH STATE SAME AS DEF'N STATE
    SCOPE defn )                // - definition scope
{
    LAB_POSN curr;              // - current position
    BLK_INIT *sw_blk;           // - common BLK_INIT

    labelCurrPosn( &curr );
    sw_blk = labelFindBlk( defn );
    checkLabelState( &sw_blk->switch_posn, &curr );
}


void LabelSwitch(               // CHECK A CASE/DEFAULT LABEL
    SCOPE sw )                  // - scope containing switch
{
    LAB_POSN curr;              // - current position
    BLK_INIT *sw_blk;           // - common BLK_INIT
    SYMBOL sym;                 // - common variable

    labelCurrPosn( &curr );
    sw_blk = labelFindBlk( sw );
    labelCheckJump( &sw_blk->switch_posn, &curr, &sw_blk, &sym );
}


void LabelSwitchLabel(          // PROCESSING FOR A BLOCK OF SWITCH LABELS
    SCOPE defn,                 // - scope for switch
    boolean deadcode )          // - TRUE==> state is dead-code
{
    deadcode = deadcode;
    checkSwitchState( defn );
}


void LabelSwitchBeg(            // START OF A SWITCH STATEMENT
    void )
{
    BLK_INIT* blk;              // - block for switch/try

    blk = labelFindBlk( GetCurrScope() );
    labelCurrPosn( &blk->switch_posn );
}


void LabelSwitchEnd(            // COMPLETION OF A SWITCH STATEMENT
    void )
{
    CgFrontCode( IC_SWITCH_END );
}


void LabelReturn(               // RETURN STATEMENT PROCESSING
    void )
{
    if( NULL != currDtorSymbol()
     || popsCatch( labelFindBlk( GetCurrScope() )
                 , labelFindBlk( ScopeFunctionScopeInProgress() ) ) ) {
        if( FunctionBodyCtor() ) {
            CgFrontCode( IC_CTOR_COMPLETE );
        }
        CgFrontCode( IC_DESTRUCT );
    }
}


static void labelMemLoad(       // LOAD LABEL MEMORY FOR FUNCTION
    LAB_MEM *label_mem )        // - used to stack memory
{
    carveBLK_INIT = label_mem->carve;
    block_init_hdr = label_mem->blk_hdr;
}


static void labelMemSave(       // SAVE LABEL MEMORY FOR FUNCTION
    LAB_MEM *label_mem )        // - used to stack memory
{
    label_mem->carve = carveBLK_INIT;
    label_mem->blk_hdr = block_init_hdr;
}


void LabelInitFunc(             // INITIALIZE LABELS (FUNCTION)
    LAB_MEM *label_mem )        // - used to stack memory
{
    labelMemSave( label_mem );
    carveBLK_INIT = CarveCreate( sizeof( BLK_INIT ), BLOCK_BLK_INIT );
    block_init_hdr = NULL;
    labelFindBlk( ScopeFunctionScopeInProgress() );
}


void LabelSwitchFunc(           // SWITCH FUNCTION'S LABEL MEMORY
    LAB_MEM *label_mem )        // - used to stack memory
{
    LAB_MEM temp;               // - used to hold current info.

    labelMemSave( &temp );
    labelMemLoad( label_mem );
    *label_mem = temp;
}


void LabelFiniFunc(             // COMPLETION OF LABELS (FUNCTION)
    LAB_MEM *label_mem )        // - used to stack memory
{
    if( carveBLK_INIT != NULL ) {
        CarveDestroy( carveBLK_INIT );
    }
    if( label_mem != NULL ) {
        labelMemLoad( label_mem );
    }
}


// IC_BLOCK_OPEN is emitted with an operand of zero.
// When the block becomes significant, the operand is zapped with the
// address of the scope.
//
// Assumption: IC_BLOCK_OPEN never occurs at offset 0 in block 0
//
void LabelBlockOpen(            // EMIT OPENING OF CURRENT SCOPE
    boolean dead_code )         // - TRUE ==> in dead-code state
{
    BLK_INIT *blk;              // - BLK_INIT for scope

    blk = labelFindBlk( GetCurrScope() );
    if( dead_code ) {
        blk->dead_zap = TRUE;
        CgFrontCode( IC_BLOCK_DEAD );
    } else {
        blk->open_zap = TRUE;
        CgFrontCode( IC_BLOCK_OPEN );
    }
    blk->open_ins = CgFrontLastIns();
}


void LabelBlockClose(           // CLOSE CURRENT BLOCK SCOPE
    boolean dead_code )         // - TRUE ==> in dead-code state
{
    BLK_INIT *blk;              // - BLK_INIT for scope
    BLK_INIT *enclosing;        // - BLK_INIT for enclosing scope
    boolean  blk_end;           // - emit IC_BLOCK_END

    blk_end = FALSE;
    blk = labelFindBlk( GetCurrScope() );
    enclosing = blk->containing;
    if( GetCurrScope()->s.keep || blk->catch_blk ) {
        if( blk->open_zap ) {
            CgFrontZapPtr( blk->open_ins, IC_BLOCK_OPEN, GetCurrScope() );
            blk_end = TRUE;
        } else if( blk->dead_zap ) {
            CgFrontZapPtr( blk->open_ins, IC_BLOCK_DEAD, GetCurrScope() );
            blk_end = TRUE;
        }
        if( ! dead_code ) {
            CgFrontCodePtr( IC_BLOCK_CLOSE, GetCurrScope() );
        }
        if( blk_end ) {
            CgFrontCodePtr( IC_BLOCK_END, GetCurrScope() );
        }
    } else {
        if( enclosing != NULL ) {
            RingPrune( &enclosing->contains, blk );
            CarveFree( carveBLK_INIT, blk );
        }
        CgFrontCode( IC_BLOCK_DONE );
    }
}


void LabelDefFree(              // FREE A LABEL DEFN
    LAB_DEF *def )              // - the definition
{
    LAB_REF *ref;

    if( def == NULL ) {
        return;
    }
    RingIterBegSafe( def->forward, ref ) {
        LabelRefFree( ref );
    } RingIterEndSafe( ref )
    CarveFree( carveLAB_DEF, def );
}

void LabelRefFree(              // FREE A LABEL REFERENCE
    LAB_REF *ref )              // - the reference
{
    CarveFree( carveLAB_REF, ref );
}

void LabelBlkTry(               // INDICATE TRY BLOCK
    TOKEN_LOCN* posn,           // - position of "{"
    SYMBOL try_var )            // - try variable
{
    BLK_INIT *blk;              // - BLK_INIT for scope

    blk = labelFindBlk( GetCurrScope() );
    blk->try_blk = TRUE;
    GetCurrScope()->s.try_catch = TRUE;
    blk->locn = *posn;
    ScopeKeep( GetCurrScope() );
    CgFrontCodePtr( IC_SET_TRY_STATE, try_var );
}


void LabelTryComplete(          // TRY HAS BEEN COMPLETED
    SYMBOL try_var )            // - try variable
{
    CgFrontCodePtr( IC_TRY_DONE, try_var );
}


void LabelBlkCatch(             // INDICATE CATCH BLOCK
    TOKEN_LOCN* posn,           // - position of "{"
    void* try_id )              // - id of try block
{
    BLK_INIT *blk;              // - BLK_INIT for scope

    blk = labelFindBlk( GetCurrScope() );
    blk->catch_blk = TRUE;
    blk->try_id = try_id;
    GetCurrScope()->s.try_catch = TRUE;
    blk->locn = *posn;
    ScopeKeep( GetCurrScope() );
    CgFrontCode( IC_SET_CATCH_STATE );
}




void LabelExprBegin(            // START OF REGION FOR TEMPORARIES
    void )
{
    expr_dtor_bound = currDcledSymbol();
    expr_dtor_depth = 0;
    labelFlags.has_setjmp = FALSE;
    labelFlags.has_newctor = FALSE;
    CgFrontCode( IC_NO_OP );    // zapped to IC_EXPR_TEMP, when dtorable temps
    ins_temp = CgFrontLastIns();
}


void LabelExprEnd(              // END OF REGION FOR TEMPORARIES (AFTER CGDONE)
    void )
{
    BLK_INIT* blk;              // - current BLK_INIT
    SCOPE scope;                // - current scope

    blk = labelFindBlk( GetCurrScope() );
    if( expr_dtor_bound != blkDtorSymbol( blk )
     || labelFlags.has_newctor
     || labelFlags.has_setjmp ) {
        scope = dtorScope( blk );
        CgFrontZapUint( ins_temp, IC_EXPR_TEMP, 0 );
        if( labelFlags.has_setjmp ) {
            CgFrontCode( IC_SETJMP_DTOR );
        }
        CgFrontCode( IC_STMT_SCOPE_END );
    }
    blk->sym_dtored = blk->dcled_dtored;
}


void LabelExprSetjmp(           // MARK SETJMP REFERENCE
    void )
{
    labelFlags.has_setjmp = TRUE;
    ScopeMarkVisibleAutosInMem();
}


void LabelExprNewCtor(          // MARK NEW-CTOR REFERENCE
    void )
{
    labelFlags.has_newctor = TRUE;
    FunctionRegistrationFlag();
}


void LabelTempDtored(           // ENSURE DTOR OF TEMP IS OK
    PTREE expr,                 // - expression for errors
    SYMBOL temp )               // - a temporary symbol
{
    BLK_INIT *blk;              // - current initialization block

    expr = expr;                // ***** later, use for a better message *****
    ClassAccessDtor( temp->sym_type );
    blk = labelFindBlk( SymScope( temp ) );
    labelMarkDtorSym( blk, temp );
}


void LabelCondTrue(             // START TRUE PART OF CONDITIONAL BLOCK
    void )
{
    COND_BLK* cb;               // - conditional block

    cb = VstkPush( &stack_cond );
    CgFrontCodeUint( IC_COND_TRUE, 0 );
    cb->ins_true = CgFrontLastIns();
    cb->sym_dtored = currDtorSymbol();
    cb->has_false = FALSE;
}


void LabelCondFalse(            // START FALSE PART OF CONDITIONAL BLOCK
    void )
{
    COND_BLK* cb;               // - conditional block

    cb = VstkTop( &stack_cond );
    CgFrontCodeUint( IC_COND_FALSE, 0 );
    cb->ins_false = CgFrontLastIns();
    cb->has_false = TRUE;
}

void LabelCondEnd(              // END OF CONDITIONAL BLOCK
    void )
{
    COND_BLK* cb;               // - conditional block

    cb = VstkPop( &stack_cond );
    if( cb->sym_dtored != currDtorSymbol() ) {
        FunctionRegistrationFlag();
        if( cb->has_false ) {
            CgFrontZapUint( cb->ins_true, IC_COND_TRUE, 1 );
            CgFrontZapUint( cb->ins_false, IC_COND_FALSE, 1 );
            CgFrontCodeUint( IC_COND_END, 0 );
        } else {
            CgFrontZapUint( cb->ins_true, IC_COND_TRUE, 2 );
            CgFrontCodeUint( IC_COND_END, 1 );
        }
    }
}



static void labelInit(          // INITIALIZE LABELS (PGM)
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveLAB_DEF = CarveCreate( sizeof( LAB_DEF ), BLOCK_LAB_DEF );
    carveLAB_REF = CarveCreate( sizeof( LAB_REF ), BLOCK_LAB_REF );
    VstkOpen( &stack_cond, sizeof( COND_BLK ), 16 );
}


static void labelFini(          // COMPLETION OF LABLES (PGM)
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveLAB_DEF );
    CarveDestroy( carveLAB_REF );
    VstkClose( &stack_cond );
}


INITDEFN( labels, labelInit, labelFini );

void *LabelBlockOpenFindZap( LAB_MEM *lm, CGFILE_INS *p )
{
    BLK_INIT *h;
    BLK_INIT *b;

    RingIterBeg( lm->blk_hdr, h ) {
        if( h->open_zap || h->dead_zap ) {
            if( p->block != h->open_ins.block ) continue;
            if( p->offset != h->open_ins.offset ) continue;
            return( CarveGetIndex( lm->carve, h ) );
        }
        RingIterBeg( h->contains, b ) {
            if( b->open_zap || b->dead_zap ) {
                if( p->block != b->open_ins.block ) continue;
                if( p->offset != b->open_ins.offset ) continue;
                return( CarveGetIndex( lm->carve, b ) );
            }
        } RingIterEnd( b );
    } RingIterEnd( h )
    return( (void *)CARVE_NULL_INDEX );
}

CGFILE_INS *LabelBlockOpenAdjustZap( LAB_MEM *lm, void *h )
{
    BLK_INIT *b;

    b = CarveMapIndex( lm->carve, h );
    if( b == NULL ) {
        return( NULL );
    }
    return( &(b->open_ins) );
}

static void markFreeBLK_INIT( void *p )
{
    BLK_INIT *b = p;

    b->free = TRUE;
}

static void saveBLK_INIT( void *e, carve_walk_base *d )
{
    BLK_INIT *b = e;
    BLK_INIT *save_next;
    BLK_INIT *save_containing;
    BLK_INIT *save_contains;
    SCOPE save_scope;
    SYMBOL save_first_init;
    SYMBOL save_sym;
    SYMBOL save_sym_containing;
    SYMBOL save_sym_dtored;
    SYMBOL save_sym_dtored_containing;
    SYMBOL save_dcled_dtored;
    SRCFILE save_locn_src_file;
    SCOPE save_sw_scope;
    SYMBOL save_sw_sym;

    if( b->free ) {
        return;
    }
    DbgVerify( b->try_id == NULL, "invalid label detected during PCH write" );
    save_next = b->next;
    b->next = CarveGetIndex( d->extra, save_next );
    save_containing = b->containing;
    b->containing = CarveGetIndex( d->extra, save_containing );
    save_contains = b->contains;
    b->contains = CarveGetIndex( d->extra, save_contains );
    save_scope = b->scope;
    b->scope = ScopeGetIndex( save_scope );
    save_first_init = b->first_init;
    b->first_init = SymbolGetIndex( save_first_init );
    save_sym = b->sym;
    b->sym = SymbolGetIndex( save_sym );
    save_sym_containing = b->sym_containing;
    b->sym_containing = SymbolGetIndex( save_sym_containing );
    save_sym_dtored = b->sym_dtored;
    b->sym_dtored = SymbolGetIndex( save_sym_dtored );
    save_sym_dtored_containing = b->sym_dtored_containing;
    b->sym_dtored_containing = SymbolGetIndex( save_sym_dtored_containing );
    save_dcled_dtored = b->dcled_dtored;
    b->dcled_dtored = SymbolGetIndex( save_dcled_dtored );
    save_locn_src_file = b->locn.src_file;
    b->locn.src_file = SrcFileGetIndex( save_locn_src_file );
    save_sw_scope = b->switch_posn.scope;
    b->switch_posn.scope = ScopeGetIndex( save_sw_scope );
    save_sw_sym = b->switch_posn.sym;
    b->switch_posn.sym = SymbolGetIndex( save_sw_sym );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *b );
    b->next = save_next;
    b->containing = save_containing;
    b->contains = save_contains;
    b->scope = save_scope;
    b->first_init = save_first_init;
    b->sym = save_sym;
    b->sym_containing = save_sym_containing;
    b->sym_dtored = save_sym_dtored;
    b->sym_dtored_containing = save_sym_dtored_containing;
    b->dcled_dtored = save_dcled_dtored;
    b->locn.src_file = save_locn_src_file;
    b->switch_posn.scope = save_sw_scope;
    b->switch_posn.sym = save_sw_sym;
}

void LabelPCHWrite( LAB_MEM *p )
{
    auto carve_walk_base data;

    PCHWriteCVIndex( CarveLastValidIndex( p->carve ) );
    PCHWriteCVIndex( (cv_index)CarveGetIndex( p->carve, p->blk_hdr ) );
    data.extra = p->carve;
    CarveWalkAllFree( p->carve, markFreeBLK_INIT );
    CarveWalkAll( p->carve, saveBLK_INIT, &data );
    PCHWriteCVIndexTerm();
}

void LabelPCHRead( LAB_MEM *p )
{
    BLK_INIT *b;
    auto cvinit_t data;

    p->carve = CarveRestart( p->carve );
    CarveMapOptimize( p->carve, PCHReadCVIndex() );
    p->blk_hdr = CarveMapIndex( p->carve, (void *)PCHReadCVIndex() );
    CarveInitStart( p->carve, &data );
    for( ; (b = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *b );
        b->next = CarveMapIndex( p->carve, b->next );
        b->containing = CarveMapIndex( p->carve, b->containing );
        b->contains = CarveMapIndex( p->carve, b->contains );
        b->scope = ScopeMapIndex( b->scope );
        b->first_init = SymbolMapIndex( b->first_init );
        b->sym = SymbolMapIndex( b->sym );
        b->sym_containing = SymbolMapIndex( b->sym_containing );
        b->sym_dtored = SymbolMapIndex( b->sym_dtored );
        b->sym_dtored_containing = SymbolMapIndex( b->sym_dtored_containing );
        b->dcled_dtored = SymbolMapIndex( b->dcled_dtored );
        b->locn.src_file = SrcFileMapIndex( b->locn.src_file );
        b->switch_posn.scope = ScopeMapIndex( b->switch_posn.scope );
        b->switch_posn.sym = SymbolMapIndex( b->switch_posn.sym );
    }
    CarveMapUnoptimize( p->carve );
}
