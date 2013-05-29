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
* Description:  Emit debug information for debugging locals.
*
****************************************************************************/


#include "cgstd.h"
#include <string.h>
#include "coderep.h"
#include "pattern.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "cgaux.h"
#include "dbgstrct.h"
#include "dbcue.h"
#include "data.h"
#include "types.h"
#include "makeins.h"
#include "feprotos.h"
#include "cgprotos.h"
#ifndef NDEBUG
#include "echoapi.h"
#endif

extern  void            AddIns(instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  seg_id          SetOP(seg_id);
extern  seg_id          AskCodeSeg(void);
extern  void            EmptyQueue(void);
extern  void            InputOC(any_oc *);
extern  bool            DBNested( bool nested );
extern  dbg_loc         LocDupl(dbg_loc);
extern  dbg_loc         LocReg(dbg_loc,name*);
extern  dbg_loc         LocParm(dbg_loc,name*);
extern  dbg_loc         DBLocInit(void);
extern  dbg_loc         DBLocSym(dbg_loc,sym_handle);
extern  void            DBLocFini(dbg_loc);
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
/* WV interface */
extern  void    WVInitDbgInfo( void );
extern  void    WVFiniDbgInfo( void );
extern  void    WVGenStatic( sym_handle sym, dbg_loc loc );
extern  void    WVObjectPtr( cg_type ptr_type );
extern  void    WVSetBase( void );
extern  void    WVBlkEnd( dbg_block *blk, offset lc );
extern  void    WVRtnEnd( dbg_rtn *rtn, offset lc );
#endif
/* DF interface */
extern  void    DFInitDbgInfo( void );
extern  void    DFObjInitInfo( void );
extern  void    DFFiniDbgInfo( void );
extern  void    DFObjFiniDbgInfo( void );
extern  void    DFGenStatic( sym_handle sym, dbg_loc loc );
extern  void    DFTypedef( char *nm, dbg_type tipe );
extern  void    DFProEnd( dbg_rtn *rtn, offset lc );
extern  void    DFBlkBeg( dbg_block *blk, offset lc );
extern  void    DFBlkEnd( dbg_block *blk, offset lc );
extern  void    DFEpiBeg( dbg_rtn *rtn, offset lc );
extern  void    DFRtnEnd( dbg_rtn *rtn, offset lc );

/* CV interface */
extern  void    CVInitDbgInfo( void );
extern  void    CVObjInitInfo( void );
extern  void    CVFiniDbgInfo( void );
extern  void    CVObjFiniDbgInfo( void );
extern  void    CVGenStatic( sym_handle sym, dbg_loc loc, bool mem );
extern  void    CVTypedef( char *nm, dbg_type tipe );
extern  void    CVSetBase( void );
extern  void    CVRtnBeg( dbg_rtn *rtn, offset lc );
extern  void    CVProEnd( dbg_rtn *rtn, offset lc );
extern  void    CVBlkBeg( dbg_block *blk, offset lc );
extern  void    CVBlkEnd( dbg_block *blk, offset lc );
extern  void    CVEpiBeg( dbg_rtn *rtn, offset lc );
extern  void    CVRtnEnd( dbg_rtn *rtn, offset lc );

extern  struct opcode_entry   DbgInfo[];

static  void        EmitDbg( byte class, pointer ptr );
static  dbg_block   *MkBlock( void );
static  void        AddBlockInfo( dbg_block *blk, bool start );

cue_ctl     LineInfo;
fname_ctl   DBFiles;


static  void    SrcFileNoInit( void ){
/*****************************/
    DBFiles.lst = NULL;
    DBFiles.count = 0;
}

static  void    DBSrcFileFini( void ){
/*****************************/
    fname_lst   *curr, *old;
    curr = DBFiles.lst;
    while( curr != NULL ){
        old = curr;
        curr = curr->next;
        CGFree( old );
    }
    DBFiles.lst = NULL;
}

extern  uint    _CGAPI DBSrcFile( char *fname ) {
/***********************************************/
    int          index;
    int          len;
    fname_lst   *curr, **lnk;

#ifndef NDEBUG
    EchoAPI( "DBSrcFile( %c )", fname );
#endif
    lnk  = &DBFiles.lst;
    curr = *lnk;
    index = 0;
    while( (curr = *lnk) != NULL ) {
       if( strcmp( fname, curr->fname ) == 0 ) {
            goto found;
       }
       ++index;
       lnk = &curr->next;
    }
    len = strlen( fname );
    curr = CGAlloc( sizeof( *curr )+len );
    curr->len = len+1;
    curr->next = NULL;
    strcpy( curr->fname, fname );
    ++DBFiles.count;
    *lnk = curr;
found:
#ifndef NDEBUG
    EchoAPI( " -> %i\n", index );
#endif
    return( index );
}

extern  char *SrcFNoFind( uint fno ){
/****************************************/
    int          index;
    fname_lst   *curr;

    curr  = DBFiles.lst;
    index = 0;
    while( curr != NULL ){
       if( index == fno ){
            goto found;
       }
       ++index;
       curr = curr->next;
    }
    return( "unknown" );
found:;
    return( curr->fname );
}


static void  AddCueBlk( cue_ctl *ctl ){
/**************************************/
    cue_blk *new;

    new = CGAlloc( sizeof( *new ) );
    new->next = NULL;
    *ctl->lnk = new;
    ctl->curr = new;
    ctl->lnk = &new->next;
    ctl->next = &new->info[0];
    ctl->end  = &new->info[CUES_PER_BLK];
}


static void  SourceCueInit( cue_ctl *ctl ){
/*********************************/

    ctl->head = NULL;
    ctl->lnk  = &ctl->head;
    ctl->next = &ctl->start[0];
    ctl->end  = &ctl->start[1];
    ctl->state.cue = 0;
    ctl->state.fno = -1; /* force change */
    ctl->state.line = 0;
    ctl->state.col =  0;
    ctl->count = 0;
}

extern cue_idx CueAdd( int fno, int line, int col ){
/****************************************************/
    enum{
        EQUAL    = 0x00,
        LINE_NO  = 0x01,
        COL_NO   = 0x02,
        FNO_NO   = 0x04,
    }cmp;
    cue_ctl    *ctl;
    long diff;


    if( fno == 0 && col == 1 && line < PRIMARY_RANGE ){
        return( line );
    }
    ctl = &LineInfo;
    cmp = EQUAL;
    if( fno != ctl->state.fno ){
        cmp |= FNO_NO;
    }
    if( col != ctl->state.col ){
        cmp |= COL_NO;
    }
    diff = line - ctl->state.line;
    if( diff != 0 ){
       cmp |= LINE_NO;
    }
    if( cmp == LINE_NO && 0 <= diff && diff < MAX_LINE_DELTA  ){
        ctl->state.line = line;
        ctl->state.cue += diff;
        cmp = EQUAL;
    }
    if( cmp ){
        if( ctl->next >= ctl->end ){
            AddCueBlk( ctl );
        }
        ++ctl->state.cue;
        ctl->state.fno = fno;
        ctl->state.line = line;
        ctl->state.col = col;
        *ctl->next = ctl->state;
        ++ctl->next;
        ++ctl->count;
    }
    return( ctl->state.cue + PRIMARY_RANGE );
}

extern bool CueFind( cue_idx cue, cue_state *ret ){
/**************************************************/
    cue_ctl    *ctl;
    cue_blk    *blk;
    cue_state   *hi;
    long        diff;

    ctl = &LineInfo;
    if( ctl->count == 0 || cue < PRIMARY_RANGE ){
        ret->fno = 0;
        ret->line = cue;
        ret->col = 1;
        return( TRUE );
    }
    cue -= PRIMARY_RANGE;
    if( cue < ctl->start[0].cue ){
        Zoiks( ZOIKS_078 ); /* lower than low */
        return( FALSE );
    }
    blk = ctl->head;
    hi = &ctl->start[1];
    while( blk != NULL ){
        if( cue < blk->info[0].cue )break;
        hi = &blk->info[CUES_PER_BLK];
        blk = blk->next;
    }
    if( blk == NULL ){
    /* if ctl->head == NULL then next is == ctl->start[1] */
    /* if cue in last blk next is end of entries in blk */
        hi = ctl->next;
        if( cue > ctl->state.cue ){ /* high than high */
            Zoiks( ZOIKS_078 );
            return( FALSE );
        }
    }
    do{
        --hi;
    }while( ( diff = cue - hi->cue ) < 0 );
    *ret = *hi;
    ret->line += diff;
    return( TRUE );
}

#if 0
static void CueLen( cue_ctl *ctl ){
/***************************/
//Set map number to #cues with entry
    cue_ctl    *ctl;
    cue_blk    *blk;
    cue_state   *curr, *last;
    long        diff;

    blk = ctl->head;
    if( ctl->count > 0 ){
        last = &ctl->start[0];
    }
    while( blk != NULL ){
        curr = &blk->info[0];
        if( blk->next == NULL ){
            end = ctl->next;
        }else{
            end = &blk->info[CUES_PER_BLK];
        }
        while( curr != end ){
            last->map = curr->cue - last->cue;
            ++curr;
        }
        blk = blk->next;
    }
    last->map = ctl->state.cue- last->cue;
}

// the only time we can do this is if
// the cue guy has seen all the cues and no cue numbers have been
// released
static cue_idx  CueFMap( cue_ctl *ctl, int fno, cue_idx map ){
/***************************/
//Map cues with same file together
    cue_ctl    *ctl;
    cue_blk    *blk;
    cue_state   *base, *curr, *last;
    cue_idx    map;
    cue_idx    len;

    blk = ctl->head;
    if( ctl->count > 0 ){
        curr = &ctl->start[0];
        if( curr->fno == fno ){
            len =  curr->map;
            curr->map = map;
            map += len;
        }
    }
    while( blk != NULL ){
        curr = &blk->info[0];
        if( blk->next == NULL ){
            end = ctl->next;
        }else{
            end = &blk->info[CUES_PER_BLK];
        }
        while( curr != end ){
            if( curr->fno == fno ){
                len =  curr->map;
                curr->map = map;
                map += len;
            }
            ++curr;
        }
        blk = blk->next;
    }
    return( map );
}

extern void CueMap( cue_ctl *ctl, cue_state *base ){
/***************************/
//Add a map number so cues from the same file
//can be re-written on a continueum
    cue_ctl    *ctl;
    cue_blk    *blk;
    cue_state   *base, *curr, *last;
    int         fno;
    cue_idx     curr_idx;

    ctl = &LineInfo;
    CueLen( ctl ); /* add lengths */
    curr_idx = 0;
    for( fno = 0; fno <  DBFiles.count; ++fno ){
        curr_idx = CueFMap( ctl, fno, curr_idx );
    }
    return( TRUE );
}
#endif
#if 0
extern DmpCue( cue_idx cue  ){
     cue_state            state;
    char *fname;

    if( CueFind( cue,  &state ) ){
        fname = SrcFNoFind( state.fno );
        printf( "out %s %d %d\n" , fname, state.line, state.col );
    }else{
        printf( "bad cue %d\n", cue );
    }
}
#endif
static void SourceCueFini( cue_ctl *ctl ){
/*****************************************/
    cue_blk *old, *list;

    list = ctl->head;
    while( list != NULL ){
        old = list;
        list = list->next;
        CGFree( old );
    }
    ctl->head = NULL;
}

extern  void    InitDbgInfo() {
/******************************/
    cue_idx     idx;
    char        *fname;
    uint        fno;

    SrcFileNoInit();
    DBNested( FALSE ); /* set nesting */
    SourceCueInit( &LineInfo );
    fname = FEAuxInfo( NULL, SOURCE_NAME );
    fno = DBSrcFile( fname );
    idx = CueAdd( fno, 1, 1 );
    SrcLine = 1;
    if( _IsModel( DBG_DF ) ) {
        DFInitDbgInfo();
    }else if( _IsModel( DBG_CV ) ) {
        CVInitDbgInfo();
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVInitDbgInfo();
#endif
    }
}


extern  void    FiniDbgInfo() {
/******************************/

    DBSrcFileFini();
    SourceCueFini( &LineInfo );
    if( _IsModel( DBG_DF ) ) {
        DFFiniDbgInfo();
    }else if( _IsModel( DBG_CV ) ) {
        CVFiniDbgInfo();
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVFiniDbgInfo();
#endif
    }
}


extern  void    _CGAPI DBLineNum( uint no ) {
/*******************************************/
#ifndef NDEBUG
    EchoAPI( "\nDBLineNum( %i )\n", no );
#endif
    SrcLine = no;
}

extern  void _CGAPI     DBSrcCue( uint fno, uint line, uint col ) {
/*****************************************************************/
    cue_idx     idx;
    bool        hasxcue;
//  char       *fname;

#ifndef NDEBUG
    EchoAPI( "\nDBsrcCue( %i, %i, %i )\n", fno, line, col );
#endif
//  fname = SrcFNoFind( fno );
//  printf( "in %s %d %d\n", fname, line, col );
    hasxcue =  _IsntModel( DBG_TYPES ); // Just OMF line nums
    if( hasxcue ) {
        if( fno == 0 && col == 1 ){
            DBLineNum( line );
        }
    }else{
        idx = CueAdd( fno, line, col );
        SrcLine = idx;
    }
}

extern  void _CGAPI DBGenStMem( sym_handle sym, dbg_loc loc ) {
/*********************************************************/
#ifndef NDEBUG
    EchoAPI( "DBGenStMem( %s,%i)\n", sym, loc );
#endif
    if( _IsModel( DBG_DF ) ) {
        DFGenStatic( sym, loc );
    }else if( _IsModel( DBG_CV ) ) {
        CVGenStatic( sym, loc, TRUE );
    }else{
    }
}

static  void    AddLocal( dbg_local **owner, dbg_local *lcl ){
/************************************************************/

    dbg_local *curr;

    while( (curr = *owner) != NULL ) {
        owner = &curr->link;
    }
    lcl->link = NULL;
    *owner = lcl;
}

extern  void _CGAPI DBGenSym( sym_handle sym, dbg_loc loc, int scoped ) {
/***********************************************************************/

    fe_attr     attr;
    dbg_local   *lcl;

#ifndef NDEBUG
    EchoAPI( "DBGenSym( %s, %i, %i )\n", sym, loc, scoped );
#endif
    if( _IsModel( DBG_LOCALS ) ) {
        attr = FEAttr( sym );
        if( (attr & FE_IMPORT) == 0 ) {
            if( attr & FE_PROC ) {
                CurrProc->state.attr |= ROUTINE_WANTS_DEBUGGING;
                CurrProc->targ.debug = CGAlloc( sizeof( dbg_rtn ) );
                CurrProc->targ.debug->parms = NULL;
                CurrProc->targ.debug->reeturn = LocDupl( loc );
                CurrProc->targ.debug->obj_type = DBG_NIL_TYPE;
                CurrProc->targ.debug->obj_loc = NULL;
                CurrProc->targ.debug->rtn_blk = MkBlock();
            } else if( scoped ) {
                lcl = CGAlloc( sizeof( dbg_local ) );
                lcl->sym = sym;
                lcl->loc = LocDupl( loc );
                lcl->kind = DBG_SYM_VAR;
                AddLocal( &CurrProc->targ.debug->blk->locals, lcl );
            } else {
                if( _IsModel( DBG_DF ) ) {
                    DFGenStatic( sym, loc );
                }else if( _IsModel( DBG_CV ) ) {
                    CVGenStatic( sym, loc, FALSE );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
                }else{
                    WVGenStatic( sym , loc );
#endif
                }
            }
        }
    }
}


extern  void    _CGAPI DBModSym( sym_handle sym, cg_type indirect ) {
/*******************************************************************/


    fe_attr     attr;
    dbg_loc     loc;

#ifndef NDEBUG
    EchoAPI( "DBModSym( %s, %t )\n",  sym, indirect );
#endif
    indirect = indirect;
    if( _IsModel( DBG_LOCALS ) ) {
        attr = FEAttr( sym );
        if( (attr & FE_IMPORT) == 0 ) {
            if( attr & FE_PROC ) {
                loc = NULL;
            } else {
                loc = DBLocInit();
                loc = DBLocSym( loc, sym );
            }
            DBGenSym( sym, loc, FALSE );
            DBLocFini( loc );
        }
    }
}


extern  void _CGAPI DBObject( dbg_type tipe, dbg_loc loc, cg_type ptr_type ) {
/************************************************************************/

#ifndef NDEBUG
    EchoAPI( "DBObject( %i, %i, %t )\n", tipe, loc, ptr_type );
#endif
    ptr_type = ptr_type;
    CurrProc->targ.debug->obj_type = tipe;
    CurrProc->targ.debug->obj_loc = LocDupl( loc );
    if( _IsModel( DBG_DF ) ) {
       //
    }else if( _IsModel( DBG_CV ) ) {
      //
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVObjectPtr( ptr_type );
#endif
    }
}



extern  void    DBAllocReg( name *reg, name *temp ) {
/***************************************************/

    temp = temp;
    reg = reg;
}

extern void _CGAPI DBTypeDef( char *nm, dbg_type tipe ){
/******************************************************/
#ifndef NDEBUG
    EchoAPI( "DBTypeDef( %c, %t )\n", nm, tipe );
#endif
    if( _IsModel( DBG_DF ) ) {
         DFTypedef( nm, tipe );
    }else if( _IsModel( DBG_CV ) ) {
        CVTypedef( nm, tipe );
    }else{
     }
}

extern  void    _CGAPI DBLocalSym( sym_handle sym, cg_type indirect ) {
/*********************************************************************/


    fe_attr     attr;
    dbg_loc     loc;

#ifndef NDEBUG
    EchoAPI( "DBLocalSym( %s, %t )\n", sym, indirect );
#endif
    indirect = indirect;
    if( CurrProc->targ.debug != NULL ) {
        attr = FEAttr( sym );
        if( (attr & FE_IMPORT) == 0 ) {
            loc = DBLocInit();
            loc = DBLocSym( loc, sym );
            DBGenSym( sym, loc, TRUE );
            DBLocFini( loc );
        }
    }
}

void    _CGAPI DBLocalType( sym_handle sym, bool kind ) {
/*******************************************************/
    dbg_local   *lcl;

#ifndef NDEBUG
    EchoAPI( "DBLocalType( %s, %i)\n", sym, kind );
#endif
    if( _IsModel( DBG_LOCALS ) ) {
        if( _IsModel( DBG_CV | DBG_DF ) ) {
            lcl = CGAlloc( sizeof( dbg_local ) );
            lcl->sym = sym;
            lcl->loc = NULL;
            if( kind ){
                lcl->kind = DBG_SYM_TYPE;
            }else{
                lcl->kind = DBG_SYM_TYPEDEF;
            }
            AddLocal( &CurrProc->targ.debug->blk->locals, lcl );
        }
    }
}

extern  dbg_block *DoDBBegBlock( int fast_codegen )
/*************************************************/
{
    dbg_block   *blk;

    if( CurrProc->targ.debug == NULL ) return( NULL );
    blk = MkBlock();
    if( !fast_codegen ) {
        /*%%%% stick a NOP in the instruction stream, point it at block*/
        AddBlockInfo( blk, TRUE );
    }
    return( blk );
}

extern  void _CGAPI     DBBegBlock() {
/************************************/
#ifndef NDEBUG
    EchoAPI( "DBBegBlock()\n" );
#endif
    DoDBBegBlock( 0 );
}


static  dbg_block *MkBlock( void ) {
/********************************/


    dbg_block   *blk;

    blk = CGAlloc( sizeof( dbg_block ) );
    blk->parent = CurrProc->targ.debug->blk;
    CurrProc->targ.debug->blk = blk;
    blk->locals = NULL;
    blk->patches = NULL;
    return( blk );
}


extern  void    DoDBEndBlock( int fast_codegen ) {
/************************************************/

    dbg_block   *blk;

#ifndef NDEBUG
    EchoAPI( "DBEndBlock()\n" );
#endif
    if( CurrProc->targ.debug != NULL ) {
        blk = CurrProc->targ.debug->blk;
        if( !fast_codegen ) {
            AddBlockInfo( blk, FALSE );
        }
        CurrProc->targ.debug->blk = blk->parent;
    }
}

extern  void _CGAPI     DBEndBlock() {
/************************************/

    DoDBEndBlock( 0 );
}


static  void    AddBlockInfo( dbg_block *blk, bool start ) {
/**********************************************************/


    instruction *ins;

    ins = MakeNop();
    ins->table = DbgInfo;
    ins->u.gen_table = ins->table;
    ins->flags.nop_flags = NOP_DBGINFO;
    if( start )
        ins->flags.nop_flags |= NOP_DBGINFO_START;
    ins->operands[ 0 ] = (name *)blk;
    AddIns( ins );
}


extern  void    DbgSetBase() {
/****************************/

    if( _IsModel( DBG_DF ) ) {
    /* nothing */
    }else if( _IsModel( DBG_CV ) ) {
        CVSetBase();
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVSetBase();
#endif
    }
}


extern  void    DbgParmLoc( name_def *parm, sym_handle sym )
/**********************************************************/
// sym is NULL if no front end sym
{
    dbg_local           *lcl;
    dbg_loc             loc;

    if( _IsntModel( DBG_DF ) ){
        if( parm->class != N_REGISTER  ){
            return;
        }
    }
    lcl = CGAlloc( sizeof( dbg_local ) );
    loc = DBLocInit();
    loc = LocParm( loc, (name *)parm );
    lcl->loc = loc;
    lcl->sym = sym;
    lcl->kind = DBG_SYM_VAR;
    AddLocal( &CurrProc->targ.debug->parms, lcl );
}


extern  void    DbgRetLoc() {
/***************************/


    dbg_loc     loc;

    if( CurrProc->targ.debug->reeturn == NULL ) {
        loc = DBLocInit();
        loc = LocReg( loc, AllocRegName( CurrProc->state.return_reg ) );
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
        if( CurrProc->targ.return_points == NULL ) {
            /* nothing to do */
        } else if( CurrProc->state.attr & ROUTINE_ALLOCS_RETURN ) {
            loc->class = LOC_IND_REG + IND_RALLOC_NEAR;
        } else {
            loc->class = LOC_IND_REG + IND_CALLOC_NEAR;
        }
#endif
        CurrProc->targ.debug->reeturn = loc;
    }
}


/**/
/* Going into optimizer queue*/
/**/


extern  void    DbgRetOffset( type_length offset ) {
/*******************************************/


    CurrProc->targ.debug->ret_offset = offset;
}


extern  void    EmitRtnBeg() {
/****************************/


    EmitDbg( INFO_DBG_RTN_BEG, CurrProc->targ.debug );
}


extern  void    EmitProEnd() {
/****************************/


    EmitDbg( INFO_DBG_PRO_END, CurrProc->targ.debug );
}


extern  void    EmitDbgInfo( instruction *ins ) {
/***********************************************/


    if( ins->flags.nop_flags & NOP_DBGINFO_START ) {
        EmitDbg( INFO_DBG_BLK_BEG, ins->operands[ 0 ] );
   } else {
        EmitDbg( INFO_DBG_BLK_END, ins->operands[ 0 ] );
    }
}


extern  void    EmitEpiBeg() {
/****************************/


    EmitDbg( INFO_DBG_EPI_BEG, CurrProc->targ.debug );
}


extern  void    EmitRtnEnd() {
/****************************/


    seg_id      old;

    EmitDbg( INFO_DBG_RTN_END, CurrProc->targ.debug );
    old = SetOP( AskCodeSeg() );
    EmptyQueue();
    SetOP( old );
}


static  void    EmitDbg( byte class, pointer ptr ) {
/**************************************************/


    any_oc      temp;

    temp.oc_debug.op.class = OC_INFO + class;
    temp.oc_debug.op.reclen = sizeof( oc_debug );
    temp.oc_debug.op.objlen = 0;
    temp.oc_debug.ptr = ptr;
    InputOC( &temp );
}


/**/
/* Coming out of optimizer queue*/
/**/


extern  void    DbgRtnBeg( dbg_rtn *rtn,  offset lc ) {
/***************************************************/

    rtn->rtn_blk->start = lc;
    if( _IsModel( DBG_CV ) ) {
        CVRtnBeg( rtn, lc );
    }
}


extern  void    DbgProEnd( dbg_rtn *rtn, offset lc ) {
/**************************************************/

    rtn->pro_size = lc - rtn->rtn_blk->start;
    if( _IsModel( DBG_DF ) ) {
        DFProEnd( rtn, lc );
    }else if( _IsModel( DBG_CV ) ) {
        CVProEnd( rtn, lc );
    }
}


extern  void    DbgBlkBeg( dbg_block *blk, offset lc ) {
/****************************************************/


    blk->start = lc;
    if( _IsModel( DBG_DF ) ) {
        DFBlkBeg( blk, lc );
    }else if( _IsModel( DBG_CV ) ) {
        CVBlkBeg( blk, lc );
    }
}

extern  void    DbgBlkEnd( dbg_block *blk, offset lc ) {
/****************************************************/


    if( _IsModel( DBG_DF ) ) {
        DFBlkEnd( blk, lc );
    }else if( _IsModel( DBG_CV ) ) {
        CVBlkEnd( blk, lc );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVBlkEnd( blk, lc );
#endif
    }
    CGFree( blk );
}


extern  void    DbgEpiBeg( dbg_rtn *rtn, offset lc ) {
/****************************************************/


    rtn->epi_start = lc;
    if( _IsModel( DBG_DF ) ) {
        DFEpiBeg( rtn, lc );
    }else if( _IsModel( DBG_CV ) ) {
        CVEpiBeg( rtn, lc );
    }
}


extern  void    DbgRtnEnd( dbg_rtn *rtn, offset lc ) {
/****************************************************/


    if( _IsModel( DBG_DF ) ) {
        DFRtnEnd( rtn, lc );
    }else if( _IsModel( DBG_CV ) ) {
        CVRtnEnd( rtn, lc );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    }else{
        WVRtnEnd( rtn, lc );
#endif
    }
    CGFree( rtn->blk );
    CGFree( rtn );
}
