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


#include "standard.h"
#include "model.h"
#include "coderep.h"
#include "procdef.h"
#include "conflict.h"
#include "sysmacro.h"
#include "memcheck.h"
#include "opcodes.h"
#include "typedef.h"
#include "ptrint.h"
#include "feprotos.h"

extern  void            FreeAName(name*);
extern  instruction*    MakeNop();
extern  void            AddAnIns(block*,instruction*);
extern  void            UnFixEdges();
extern  void            AssignTemps();
extern  void            AssgnMoreTemps(block_num);
extern  void            FixEdges();
extern  bool            CommonSex(bool);
extern  bool            SetOnCondition();
extern  void            BlockTrim();
extern  void            MakeFlowGraph();
extern  void            InitRegTbl();
extern  bool            LoopInvariant();
extern  bool            LoopEnregister();
extern  bool            CommonInvariant();
extern  bool            TransLoops( bool );
extern  bool            IndVars();
extern  void            BlowAwayFreeLists();
extern  bool            BlkTooBig();
extern  void            FindReferences();
extern  void            FreeConflicts();
extern  bool            SplitConflicts();
extern  void            NullConflicts(var_usage);
extern  void            FreeAConflict(conflict_node*);
extern  conflict_node   *InMemory(conflict_node*);
extern  void            FreeProc();
extern  void            GenProlog();
extern  void            GenObject();
extern  void            SortBlocks();
extern  void            InitNames();
extern  void            InitMakeAddr();
extern  void            RegTreeInit();
extern  void            InitConflict();
extern  void            InitRT();
extern  void            InitIns();
extern  void            InitSegment();
extern  void            FiniSegment();
extern  void            PushPostOps();
extern  void            DeadTemps();
extern  void            AxeDeadCode();
extern  bool            InsDead();
extern  void            OptSegs();
extern  void            Conditions();
extern  void            MakeConflicts();
extern  void            MakeLiveInfo();
extern  void            LiveInfoUpdate(void);
extern  int             ExpandOps(bool);
extern  void            FPRegAlloc();
extern  void            FixIndex();
extern  void            FixSegments();
extern  void            FixMemRefs();
extern  bool            RegAlloc(bool);
extern  void            LoopRegInvariant();
extern  void            Score();
extern  void            MergeIndex(void);
extern  void            FPExpand();
extern  void            FPOptimize();
extern  void            FPParms();
extern  void            ScoreInit();
extern  void            ScoreFini();
extern  type_class_def  TypeClass(type_def*);
extern  void            TypeInit();
extern  void            TypeFini();
extern  void            ObjInit();
extern  void            InitFP();
extern  void            ObjFini();
extern  void            AbortObj();
extern  void            FlushOpt();
extern  hw_reg_set      AllCacheRegs();
extern  void            AllocALocal(name*);
extern  void            ParmPropagate();
extern  void            InitStackMap();
extern  void            FiniStackMap();
extern  void            ProcMessage(msg_class);
extern  sym_handle      AskForLblSym(label_handle);
extern  void            InitQueue();
extern  void            FiniQueue();
extern  void            AbortQueue();
extern  void            TellFreeAllLabels();
extern  bool            FixReturns();
extern  instruction_id  Renumber();
extern  void            GenEpilog();
extern  void            SplitVars();
extern  name            *DeAlias(name*);
extern  void            AssignOtherLocals();
extern  void            MakeMovAddrConsts();
extern  void            KillMovAddrConsts();
extern  void            BuildIndex();
extern  bool            CreateBreak();
extern  void            FixBreak();
extern  void            RemoveBreak();
extern  instruction     *NeedIndex(instruction*);
extern  bool            ConstFold(block *);
extern  void            DeadInstructions(void);
extern  void            Schedule(void);
extern  bool            CharsAndShortsToInts(void);
extern  void            LNBlip(source_line_number);
extern  void            SetInOut(block *);
extern  bool            LdStAlloc(void);
extern  void            LdStCompress(void);
extern  void            MemtoBaseTemp( void );
extern  void            FixMemBases(void );
extern  bool            BGInInline();
extern  void            AddCacheRegs();
extern  void            MulToShiftAdd();
extern  bool            PropagateMoves(void);
extern  bool            TailRecursion(void);
extern  bool            PeepOpt( block *, block *(*)(block *,void *), block *, bool );
extern  void            PropNullInfo();
extern  void            FixP5Divs();
extern  void            ReConstFold();
extern  void            FlushQueue();
extern  bool            CalcDominatorInfo();

extern proc_def                 *CurrProc;
extern block                    *HeadBlock;
extern block                    *BlockList;
extern block                    *CurrBlock;
extern conflict_node            *ConfList;
extern int                      InsId;
extern type_class_def           ClassPointer;
extern pointer                  TypePtr;
extern name                     *Names[];
extern name                     *LastTemp;
extern bool                     BlockByBlock;
extern int                      InOptimizer;
extern bool                     HaveLiveInfo;
extern bool                     HaveDominatorInfo;
extern pointer_int              FrlSize;
extern source_line_number       SrcLine;

static  bool                    abortCG;


extern  void    InitCG() {
/************************/

    InOptimizer = 0;
    InsId = 0;
    CurrProc = NULL;
    CurrBlock = NULL;
    BlockList = NULL;
    HeadBlock = NULL;
    BlockByBlock = FALSE;
    abortCG = FALSE;
    InitFP();/* must be before InitRegTbl */
    InitRegTbl();
    ScoreInit();
    InitQueue();
    InitMakeAddr();
    RegTreeInit();
    InitIns();
    InitConflict();
    InitRT();
    InitNames();
    ObjInit();
    ClassPointer = TypeClass( TypePtr );
    InitSegment();
}


extern  void    AbortCG() {
/*************************/

    abortCG = TRUE;
}


extern  void    FiniCG() {
/************************/

    FiniQueue();
    FiniSegment();
    if( abortCG ) {
        AbortObj();
    }
    ObjFini();
    TellFreeAllLabels();
    BlowAwayFreeLists();
    ScoreFini();
}


extern  void    Generate( bool routine_done ) {
/*********************************************/


    if( BGInInline() ) return;
    HaveLiveInfo = FALSE;
    HaveDominatorInfo = FALSE;
    #if ( _TARGET & ( _TARG_370 | _TARG_AXP | _TARG_PPC ) ) == 0
        /* if we want to go fast, generate statement at a time */
        if( _IsModel( NO_OPTIMIZATION ) ) {
            if( !BlockByBlock ) {
                InitStackMap();
                BlockByBlock = TRUE;
            }
            LNBlip( SrcLine );
            FlushBlocks( FALSE );
            FreeExtraSyms( LastTemp );
            if( _MemLow ) {
                BlowAwayFreeLists();
            }
            return;
        }
    #endif

    /* if we couldn't get the whole procedure in memory, generate part of it */
    if( BlockByBlock ) {
        if( _MemLow || routine_done ) {
            GenPartialRoutine( routine_done );
        } else {
            BlkTooBig();
        }
        return;
    }

    /* if we're low on memory, go into BlockByBlock mode */
    if( _MemLow ) {
        InitStackMap();
        GenPartialRoutine( routine_done );
        BlowAwayFreeLists();
        return;
    }

    /* check to see that no basic block gets too unwieldy */
    if( routine_done == FALSE ) {
        BlkTooBig();
        return;
    }

    /* The routine is all in memory. Optimize and generate it */
    FixReturns();
    FixEdges();
    Renumber();
    BlockTrim();
    FindReferences();
    TailRecursion();
    NullConflicts( USE_IN_ANOTHER_BLOCK );
    InsDead();
    FixMemRefs();
    FindReferences();
    PreOptimize();
    PropNullInfo();
    MemtoBaseTemp();
    if( _MemCritical ) {
        Panic( FALSE );
        return;
    }
    MakeConflicts();
    if( _IsModel( LOOP_OPTIMIZATION ) ) {
        SplitVars();
    }
    AddCacheRegs();
    MakeLiveInfo();
    HaveLiveInfo = TRUE;
    AxeDeadCode();
    FixIndex();
    FixSegments();
    FPRegAlloc();
    if( RegAlloc( FALSE ) == FALSE ) {
        Panic( TRUE );
        HaveLiveInfo = FALSE;
        return;
    }
    FPParms();
    FixMemBases();
    PostOptimize();
    InitStackMap();
    AssignTemps();
    FiniStackMap();
    FreeConflicts();
    SortBlocks();
    if( CalcDominatorInfo() ) {
        HaveDominatorInfo = TRUE;
    }
    GenProlog();
    UnFixEdges();
    OptSegs();
    GenObject();
    if( ( CurrProc->prolog_state & GENERATED_EPILOG ) == 0 ) {
        GenEpilog();
    }
    FreeProc();
    HaveLiveInfo = FALSE;
#if _TARGET & _TARG_IAPX86
    if( _IsModel( NEW_P5_PROFILING ) ) {
        FlushQueue();
    }
#else
    FlushQueue();
#endif

}


static  void            GenPartialRoutine( bool routine_done ) {
/**************************************************************/

    while( routine_done || _MemLow ) {
        if( CreateBreak() ) {
            BlockByBlock = TRUE;
            BlockTrim();
            FindReferences();
            PreOptimize();
            FixBreak();
            SortBlocks();
            /* so the front gets told we're panicking */
            BlockByBlock = FALSE;
            FlushBlocks( FALSE );
            RemoveBreak();
            if( HeadBlock == NULL ) {
                FreeExtraSyms( NULL );
            }
        } else {
            FlushBlocks( FALSE );
            FreeExtraSyms( NULL );
            break;
        }
    }
}


static  void            PreOptimize() {
/*************************************/

    bool        change;

    if( _IsntModel( NO_OPTIMIZATION ) ) {
//      CharsAndShortsToInts();
        MakeMovAddrConsts();
        PushPostOps();
        DeadTemps();
        InsDead();
        MakeFlowGraph();
        BlockTrim();
        CommonSex( _IsModel( LOOP_OPTIMIZATION ) );
        SetOnCondition();
        BlockTrim();
        AddANop();
        if( _IsModel( LOOP_OPTIMIZATION ) ) {
            change = FALSE;
            if( TransLoops( FALSE ) ) {
                change = TRUE;
            }
            if( LoopInvariant() ) {
                change = TRUE;
            }
            if( change ) {
                CommonSex(TRUE);
                InsDead();
                CommonInvariant();
            }
            if( IndVars() ) {
                CommonSex(FALSE);
                InsDead();
                change = TRUE;
            }
            BlockTrim();
            if( TransLoops( TRUE ) ) {
                BlockTrim();
                CommonSex( FALSE );
                change = TRUE;
            }
            if( change ) {
                ReConstFold();
            }
            LoopEnregister();
            if( change ) {
                BlockTrim();
            }
        }
        MulToShiftAdd();
        KillMovAddrConsts();
        FindReferences();
    } else {
        MakeFlowGraph();
        AddANop();
        FindReferences();
    }
}

#if 0
static  block           *NextBlock( block *blk, void *parm ) {
/************************************************************/

    parm = parm;
    return( blk->next_block );
}
#endif

static  void            PostOptimize() {
/**************************************/

    if( _IsntModel( NO_OPTIMIZATION ) ) {
    #if 0
        // peep opt doesn't check to make sure it can still generate
        // instructions in many case - unsafe to run afte RegAlloc
        if( PeepOpt( HeadBlock, NextBlock, NULL, TRUE ) ) {
            LiveInfoUpdate();
        }
    #endif
        // this is important as BuildIndex cannot handle instructions with no operands
        DeadInstructions();
        BuildIndex();
        DeadInstructions();
    }
    MergeIndex();
    if( _IsntModel( NO_OPTIMIZATION ) ) {
        LdStAlloc();
        Score();
        if( !BlockByBlock ) LoopRegInvariant();
    #if !( _TARGET & ( _TARG_AXP | _TARG_PPC ) )
        if( _IsntTargetModel( STATEMENT_COUNTING ) ) {
            Conditions();
        }
    #endif
    }
    FPExpand();
    if( _IsntModel( NO_OPTIMIZATION ) ) {
        DeadInstructions();
        if( _IsModel( INS_SCHEDULING ) ) {
            HaveLiveInfo = FALSE;
            Schedule(); /* NOTE: Schedule messes up live information */
            LiveInfoUpdate();
            HaveLiveInfo = TRUE;
        }
        LdStCompress();
    #if 0
        // see above...
        // run this again in case Scheduler messed around with indices
        if( PeepOpt( HeadBlock, NextBlock, NULL, TRUE ) ) {
            LiveInfoUpdate();
        }
    #endif
    }
    FPOptimize();
}


static  void    Panic( bool partly_done ) {
/*****************************************/

    proc_def    *curr_proc;

    if( partly_done ) {
        FinishIndex();
    }
    ForceConflictsMemory();
    SortBlocks();
    UnFixEdges();
    InitStackMap();
    AssgnMoreTemps( NO_BLOCK_ID );
    curr_proc = CurrProc;
    FlushBlocks( partly_done );
    if( curr_proc == CurrProc /* if not freed (dummy!) */
     && ( CurrProc->prolog_state & GENERATED_EPILOG ) == 0 ) {
        GenEpilog();
        FiniStackMap();
        FreeProc();
    }
}


static  void    FlushBlocks( bool partly_done ) {
/***********************************************/

/* we're in deep trouble. Try to get out of it using as little memory as*/
/* possible*/

    block       *blk;
    block       *next;
    block       *curr;
    block_class classes;

    if( BlockByBlock == FALSE && _IsntModel( NO_OPTIMIZATION ) ) {
        ProcMessage( MSG_REGALLOC_DIED );
    }
    if( partly_done == FALSE ) {
        Renumber();
    }
    curr = CurrBlock;
    BlockByBlock = TRUE;
    blk = HeadBlock;
    classes = EMPTY;
    while( blk != NULL ) {
        next = blk->next_block;
        classes |= blk->class;
        CurrBlock = blk;
        BlockToCode( partly_done );
        FlushOpt();
        blk = next;
    }
    CurrBlock = curr;
    HeadBlock = NULL;
    BlockList = NULL;
}


static  void    BlockToCode( bool partly_done ) {
/***********************************************/

    block_num           inputs;
    block_num           targets;
    block_edge          *input_edges;
    conflict_node       *curr;
    conflict_node       **owner;
    conflict_node       *conflist;
    block_num           id;



    /* try to get back some memory*/
    _MemLow;

    /* make the block look like an entire procedure*/

    HeadBlock = CurrBlock;
    BlockList = CurrBlock;
    HeadBlock->prev_block = NULL;
    if( HeadBlock->next_block != NULL ) {
        HeadBlock->next_block->prev_block = NULL;
    }
    /* Kludge - need a pointer to the next block for CALL_LABEL - puke! */
    if( HeadBlock->class & CALL_LABEL ) {
        HeadBlock->v.next = HeadBlock->next_block;
    }
    HeadBlock->next_block = NULL;

    /* force anything that spans blocks to memory*/

    HeadBlock->u.partition = HeadBlock;
    ConstFold( HeadBlock );
    HeadBlock->u.partition = NULL;

    ForceTempsMemory();
    if( partly_done == FALSE ) {
        FixMemRefs();
        HaveLiveInfo = FALSE;
        if( _IsntModel( NO_OPTIMIZATION | FORTRAN_ALIASING ) ) {
            FindReferences();
            CommonSex(FALSE);
            PushPostOps();
        }
        FindReferences();
        DeadTemps();
        if( _IsModel( NO_OPTIMIZATION ) ) {
            SetInOut( HeadBlock );
        } else {
            MakeConflicts();
        }
        MakeLiveInfo();
        HaveLiveInfo = TRUE;
        AxeDeadCode();
        FixIndex();
        FixSegments();
        FPRegAlloc();
        RegAlloc( TRUE );
        FreeConflicts();
        HaveLiveInfo = FALSE;
    } else {
        conflist = NULL;
        owner = &ConfList;
        for(;;) {
            curr = *owner;
            if( curr == NULL ) break;
            if( curr->start_block == HeadBlock ) {
                *owner = curr->next_conflict;
                curr->next_conflict = conflist;
                conflist = curr;
            } else {
                owner = &curr->next_conflict;
            }
        }
        curr = ConfList;
        ConfList = conflist;
        RegAlloc( TRUE );
        FreeConflicts();
        ConfList = curr;
    }
    input_edges = HeadBlock->input_edges;
    inputs = HeadBlock->inputs;
    targets = HeadBlock->targets;
    HeadBlock->inputs = 0;
    HeadBlock->input_edges = NULL;
    HeadBlock->targets = 0;
    FPParms();
    PostOptimize();
    HeadBlock->input_edges = input_edges;
    HeadBlock->inputs = inputs;
    HeadBlock->targets = targets;

    /* generate a prolog that saves all registers*/

    if( ( CurrProc->prolog_state & GENERATED_PROLOG ) == 0 ) {
        CurrProc->state.used = AllCacheRegs();
        GenProlog();
    }

    id = CurrBlock->id;
    AssgnMoreTemps( id );
    OptSegs();

    /* generate the code for the block*/

    if( CurrBlock->class & RETURN ) {
        GenObject();
        FiniStackMap();
        FreeProc();
    } else {
        GenObject();
    }
    if( partly_done ) {
        FreeExtraTemps( NULL, id );
    } else {
        FreeExtraTemps( LastTemp, id );
    }
}


static  void    FreeExtraTemps( name *last, block_num id ) {
/**********************************************************/

    name        **owner;
    name        *temp;

    owner = &Names[ N_TEMP ];
    for(;;) {
        temp = *owner;
        if( temp == last ) break;
        if( ( temp->v.usage & USE_IN_ANOTHER_BLOCK ) == 0
         && !_FrontEndTmp( temp )
         && temp->t.u.block_id == id ) {
            *owner = temp->n.next_name;
            FreeAName( temp );
        } else {
            owner = &temp->n.next_name;
        }
    }
}


static  void    FreeExtraSyms( name *last ) {
/*******************************************/

    name        **owner;
    name        *temp;
    name        *junk;

    owner = &Names[ N_TEMP ];
    for(;;) {
        temp = *owner;
        if( temp == last ) break;
        if( ( temp->v.usage & USE_IN_ANOTHER_BLOCK ) == 0
         && !_FrontEndTmp( temp ) ) {
            *owner = temp->n.next_name;
            FreeAName( temp );
        } else {
            owner = &temp->n.next_name;
        }
    }
    temp = Names[ N_CONSTANT ];
    while( temp != NULL ) {
        junk = temp;
        temp = temp->n.next_name;
        FreeAName( junk );
    }
    Names[ N_CONSTANT ] = NULL;
    temp = Names[ N_INDEXED ];
    while( temp != NULL ) {
        junk = temp;
        temp = temp->n.next_name;
        FreeAName( junk );
    }
    Names[ N_INDEXED ] = NULL;
}


static  void    FinishIndex() {
/*****************************/

    block       *blk;
    instruction *ins;
    instruction *old;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            old = ins;
            ins = NeedIndex( ins );
            if( ins == old ) {
                ins = ins->head.next;
            }
        }
        blk = blk->next_block;
    }
}


static  void    ForceConflictsMemory() {
/**************************************/

    conflict_node       *conf;
    conflict_node       *next;

    ParmPropagate();
    conf = ConfList;
    while( conf != NULL ) {
        _SetFalse( conf, ( NEEDS_INDEX_SPLIT | NEEDS_SEGMENT_SPLIT ) );
        conf = conf->next_conflict;
    }
    conf = ConfList;
    while( conf != NULL ) {
        next = conf->next_conflict;
        if( !_GBitEmpty( conf->id.out_of_block )
         || ( conf->name->n.class == N_TEMP && _FrontEndTmp( conf->name ) )
         || ( conf->name->n.class == N_MEMORY && conf->name->v.symbol != NULL )
         || ( conf->name->v.usage & USE_IN_ANOTHER_BLOCK ) != EMPTY ) {
            next = InMemory( conf );
        }
        conf = next;
    }
}


static  void    ForceTempsMemory() {
/**********************************/

    name        *op;
    name        *next;

    ParmPropagate();
    op = Names[  N_TEMP  ];
    while( op != LastTemp ) {
        next = op->n.next_name;
        if( ( op->v.usage & USE_IN_ANOTHER_BLOCK ) || _FrontEndTmp( op ) ) {
            op = DeAlias( op );
            op->v.usage |= NEEDS_MEMORY | USE_MEMORY;
            while( op->v.conflict != NULL ) {
                FreeAConflict( op->v.conflict );
            }
        }
        op = next;
    }
    AssignOtherLocals();
    op = Names[  N_MEMORY  ];
    while( op != NULL ) {
        op->v.usage |= USE_IN_ANOTHER_BLOCK | USE_MEMORY;
        if( op->v.conflict != NULL ) {
            FreeAConflict( op->v.conflict );
            op->v.conflict = NULL;
        }
        op = op->n.next_name;
    }
    LastTemp = Names[ N_TEMP ];
}


static  void    AddANop() {
/*************************/

    AddAnIns( BlockList, MakeNop() );
}

extern  void    ProcMessage( msg_class msg ) {
/********************************************/

    static proc_def *lastproc = NULL;

    if( _IsntModel( NO_OPTIMIZATION ) && lastproc != CurrProc ) {
        FEMessage( msg, AskForLblSym( CurrProc->label ) );
        lastproc = CurrProc;
    }
}
