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


#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "standard.h"
#include "coderep.h"
#include "procdef.h"
#include "cgdefs.h"
#include "cg.h"
#include "cgaux.h"
#include "sysmacro.h"
#include "offset.h"
#include "s37bead.h"
#include "model.h"
#include "s37dbg.h"

typedef int handle;
#include "s37dbsym.def"


extern    source_line_number    SrcLine;
extern    unsigned_16           TypeIdx;
extern    proc_def              *CurrProc;

extern  uint            Length(char*);
extern  byte           *Copy(void*,void*,uint);
extern  pointer         FEAuxInfo(pointer,int);
extern  fe_attr         FEAttr(pointer);
extern  dbg_type        FEDbgType(pointer);
extern  char           *FEName(pointer);
extern bool             MemNeedsReloc(pointer ,cg_class );
extern bead_def        *GetanAddr(label_handle ,bool );
extern  name           *DeAlias(name*);
extern  name           *AllocUserTemp(pointer,type_class_def);
extern  label_handle    AskForSymLabel( pointer, cg_class );
extern  sym_handle      AskForLblSym(label_handle);
extern  hw_sym         *AskForHWLabel(label_handle );
extern void             GetOpName(hwins_op_any *,name *);
extern void             HWMKAddrGen(bead_def **,hw_sym *,offset );
extern  void            DefSegment(seg_id,seg_attr,char*,uint);
extern seg_id           SetOP(seg_id );

extern    proc_def              *CurrProc;

static  dbg_rtn  *CurrProc_debug;
static short int YTagCount;
static short int STagCount;

static cdebug_sym_any    *YSymHead;
static cdebug_sym_any   **YSymNext;


extern void DBTypInit(void);
extern void DbgTypeTags(handle );

extern handle   OpenDbg( char *name );
extern void     CloseStream(handle );
extern void     PutStream(handle ,byte *,uint );

#define DBG_SEG 6 /* hardwired till i figure it out */
extern  void    DBLineNum( uint no ) {
/************************************/

    SrcLine = no;
}

extern  void    DBModSym( sym_handle sym, cg_type indirect ) {
/**Add a sym to the dbglist sym******************************/
    fe_attr          attr;

    indirect = indirect;
    attr = FEAttr( sym );
    if( (attr & FE_IMPORT) == 0 && attr & FE_PROC ) {
        BegProcDef();
    }else{
        DBModObj( sym, attr );
    }
}

static  void    BegProcDef() {
/** start procdef in parallel with obj generation**********/

    CurrProc->state.attr |= ROUTINE_WANTS_DEBUGGING;
    _Alloc( CurrProc_debug, sizeof( dbg_rtn ) );
    CurrProc_debug->blk = NULL;
    CurrProc_debug->parms = NULL;
    CurrProc_debug->reeturn.class = 0;
    MkBlock();
}

static  void    DBModObj( sym_handle sym, fe_attr attr ) {
/**Add a non procdefn to dbglist sym ************************/
    char            *name;
    uint             len;
    cdebug_sym_obj  *new;
    label_handle     label;
    seg_id           id;

    name = FEName( sym );
    len = Length( name );
    _Alloc( new, sizeof( *new )-1 +len );
    new->id.len = len;
    Copy( name, new->id.name, len );
    new->common.attr = attr;
    label =  AskForSymLabel( sym, CG_FE );
    new->common.label =  label;
    if (attr & FE_IMPORT) {
        id = SetOP( DBG_SEG ); /* stupid segments */
        new->def = GetanAddr( label, MemNeedsReloc(sym, CG_FE) );
        SetOP( id ); /* stupid segments */
    }
    new->common.tref  =  FEDbgType( sym );
    AddSym( new );
}

extern  void    DBAllocReg( name *reg, name *temp )
/*************************************************/
{
    dbg_block   *blk;
    dbg_local   *loc;

    if( temp->n.class != N_TEMP ) return;
    for( blk = CurrProc_debug->blk; blk != NULL; blk = blk->parent ) {
        for( loc = blk->locals; loc != NULL; loc = loc->link ) {
            if( temp == loc->where.where ) {
                loc->where.class = LOC_REG;
                loc->where.where = reg;
            }
        }
    }
}


extern  void    DBLocalSym( sym_handle sym, cg_type indirect ) {
/***Add a local symbol  ***************************************/
    dbg_block   *blk;
    dbg_local   *new;
    name        *temp;
    fe_attr     attr;

    indirect = indirect;
    if( CurrProc_debug != NULL ) {
        attr = FEAttr( sym );
        if( attr & FE_STATIC ) {
            DBModObj( sym, attr );
        }else if( (attr & FE_IMPORT) == 0 ) {
            blk = CurrProc_debug->blk;
            _Alloc( new, sizeof( dbg_local ) );
            new->link = blk->locals;
            blk->locals = new;
            new->sym = sym;
            new->where.class = LOC_STACK;
            temp = DeAlias( AllocUserTemp( sym, XX ) );
            temp->v.usage |= VAR_VOLATILE+NEEDS_MEMORY+USE_IN_ANOTHER_BLOCK+USE_ADDRESS;
            new->where.where = temp;
        }
    }
}

extern  void    DBBegBlock() {
/****************************/
#if 0 /* block scopes not supported */
    if( CurrProc->targ.debug != NULL ) {  scopes not supported
        MkBlock();
    }
#endif
}

static  void    MkBlock() {
/** make a new scope block**/
    dbg_block   *new;

    _Alloc( new, sizeof( dbg_block ) );
    new->parent = CurrProc_debug->blk;
    CurrProc_debug->blk = new;
    new->locals = NULL;
}

extern  void    DBEndBlock() {
/**end block scope***********/
#if 0 /* block scopes not supported yet */
    dbg_block   *blk;

    if( CurrProc->targ.debug != NULL ) {
        blk = CurrProc_debug->blk;
        CurrProc_debug->blk = blk->parent;
    }
#endif
}

extern  void    DbgParmLoc( name_def *parm ) {
/** fill in parm location********************/
    location_list       *new;
    location_list       **owner;

    owner = &CurrProc_debug->parms;
    while( *owner != NULL ) {
       owner = &(*owner)->link;
    }
    _Alloc( new, sizeof( location_list ) );
    *owner = new;
    new->link = NULL;
    new->data.class = LOC_REG;
    new->data.where = parm;
}


extern  void    DbgRetLoc() {
/**********************************************/

}

/* after object before post optimizer */

extern  void    EmitRtnEnd() {
/****************************/
    DbgRtnEnd( CurrProc_debug );
}


extern  void    DbgBlkEnd( dbg_block *blk, offset lc ) {
/****************************************************/
#if 0
    DumpDbgLocals( blk->locals );
    _Free( blk, sizeof( dbg_block ) );
#else
    blk = 0;
    lc = 0;
#endif
}


static  void    DbgRtnEnd( dbg_rtn *rtn ) {
/** Add a cdebug_sym_procdef***************/
    uint                 len;
    location_list       *parm;
    location_list       *junk;
    sym_handle           sym;
    char                *name;
    cdebug_sym_procdef  *proc;

    sym = AskForLblSym( CurrProc->label );
    name = FEName( sym  );
    len  = Length( name );
    proc = _Alloc( proc, sizeof( *proc )-1 +len );
    proc->id.len = len;
    Copy( name, proc->id.name, len );
    proc->common.label = CurrProc->label;
    proc->common.attr = FEAttr( sym );
    proc->common.tref = FEDbgType( sym );
    len = 0;
    parm = rtn->parms;
    while( parm != NULL ) {
        ++len;
        parm = parm->link;
    }
    proc->pindex = len;
    parm = rtn->parms;
    while( parm != NULL ) {
        junk = parm;
        parm = parm->link;
        _Free( junk, sizeof( location_list ) );
    }
    DumpDbgLocals( rtn->blk->locals, proc );
    _Free( rtn->blk, sizeof( dbg_block ) );
    _Free( rtn, sizeof( dbg_rtn ) );
    AddSym( proc );
}

static  void   DumpDbgLocals( dbg_local *loc, cdebug_sym_procdef *proc ) {
/*************************************************/
    dbg_local   *junk;
    int          lindex,pindex;
    char        *name;
    uint         len;
    cdebug_local *new, **next;

    lindex = 0;
    pindex = 0;
    proc->list = NULL;
    next = &proc->list;
    while( loc != NULL ) {
        name = FEName( loc->sym );
        len = Length( name );
        _Alloc( new, sizeof( *new )-1+len );
        new->id.len = len;
        Copy( name, new->id.name, len );
        new->tref = FEDbgType( loc->sym );
        if( GetLocAddr( new, loc ) ){
            pindex++;
        }else{
            lindex++;
        }
        *next = new;
        next = &new->next;
        *next = NULL;
        junk = loc;
        loc = loc->link;
        _Free( junk, sizeof( dbg_local ) );
    }
    proc->lindex = lindex;
    proc->pindex = pindex;
}

static bool  GetLocAddr( cdebug_local *new, dbg_local *loc ){
/********** Get 370 addr for local***************************/
    hwins_op_any hwop;
    name        *var;

    var = loc->where.where;
    GetOpName( &hwop, var );
    if( loc->where.class == LOC_STACK ){
        new->acc = LACC_STACK;
        new->disp = hwop.sx.disp;
        new->base = hwop.sx.b;
    }else{
        new->acc = LACC_REG;
        new->base = hwop.r;
    }
    return( var->n.class == N_TEMP && var->t.temp_flags & STACK_PARM );
}

static void AddSym( cdebug_sym_any *new ) {
/** Add sym to sym list and bump sym index */
    new->common.next = *YSymNext;
    *YSymNext = new;
    ++YTagCount;
    YSymNext = &new->common.next;
}


extern  void    FiniDbgInfo() {
/**********************************************/

}

extern  void    InitDbgInfo() {
/*****************************/
    SrcLine = 1;
    DBTypInit();
    YTagCount = 0;  /* sym count       */
    STagCount = 0;  /* source cue      */
    YSymHead  = NULL;
    YSymNext  = &YSymHead;
    DefSegment( DBG_SEG,  GLOBAL | INIT | ROM,  "_DBG", 4 );
}

extern  void    DbgIO( txtseg_rec *rec ) {
/*** Write out dbg file*****************/

    handle        dbgfile;

    dbgfile = OpenDbg( FEAuxInfo( NULL, DBG_NAME ) );
    DTags( dbgfile, rec->csect );
    LTags( dbgfile, rec->location );
    DbgTypeTags( dbgfile );
    YTags( dbgfile, YSymHead, YTagCount );
    STags( dbgfile, rec->first );
    CloseStream( dbgfile );
}

static uint MkExtName( char *to, const char *from ) {
/*swiped from s37io********************************/
    char *curr;

    curr = to;
    while( *from != '\0' ) {
        *curr = *from == '_' ? '$' :  toupper( *from );
        from++;
        curr++;
   }
   *curr = '\0';
   return( curr-to );
}

static void DTags( handle dbgfile, bead_xsym *bead ) {
/**** Put out csect tags*******/
    hw_sym        *csect;
    id_len        idlen;
    char          name[8+1];
    char          *tag, tagbuff[5+5+4+8];

    csect = ((bead_xsym *)bead)->sym;
    tag = tagbuff;
    *tag++ = 'd';
    tag = DbgFmtInt( tag, 1 );
    *tag++ = 'D';
    tag = DbgFmtInt( tag, 0 );
    idlen = MkExtName( name, csect->name );
    tag = DbgFmtStr( tag, name, idlen );
    PutStream( dbgfile, tagbuff, tag-tagbuff );
}

static void LTags( handle dbgfile, offset size ) {
/**** Put out code length tag********************/
    char     tagbuff[5];

    tagbuff[0] = 'L';
    DbgFmtInt( &tagbuff[1], size );
    PutStream( dbgfile, tagbuff, 5 );
}

static void YTags( handle dbgfile, cdebug_sym_any *list, int index ) {
/*** Put out symbol info*********************************************/
    char             *tag, tagbuff[1+4+4+4+256];
    cdebug_sym_any   *old;
    cdebug_y_class    class;
    id_entry         *id;
    fe_attr           attr;
    int               refno;

    tagbuff[0] = 'y';
    DbgFmtInt( &tagbuff[1], index );
    PutStream( dbgfile, tagbuff, 5 );
    tagbuff[0] = 'Y';
    refno = 0;
    while( list != NULL ) {
        attr = list->common.attr;
        if (attr & FE_IMPORT) {
            if( attr & FE_PROC ){
                class = CDEBUG_Y_EXTPROC;
            }else{
                class = CDEBUG_Y_EXTVAR;
            }
            id = &list->obj.id;
        }else{
            if( attr & FE_PROC ) {
                class = CDEBUG_Y_PROC;
                id = &list->procdef.id;
            }else{
                class = CDEBUG_Y_VAR;
                id = &list->obj.id;
            }
        }
        tag = DbgFmtInt( &tagbuff[1], refno );
        tag = DbgFmtInt( tag, list->common.tref );
        tag = DbgFmtInt( tag, class );
        tag = DbgFmtStr( tag, id->name, id->len );
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        switch( class ) {
        case CDEBUG_Y_PROC:
            DbgProcDef( dbgfile, list );
            break;
        case CDEBUG_Y_VAR:
            DbgVar( dbgfile, list );
            break;
        case CDEBUG_Y_EXTVAR:
            DbgExtVar( dbgfile, list );
            break;
        case CDEBUG_Y_EXTPROC:
            DbgExtProc( dbgfile, list );
            break;
        }
        old  = list;
        list = list->common.next;
        if( attr & FE_IMPORT == 0 && attr & FE_PROC ){
            _Free( old, sizeof( list->procdef )-1+id->len );
        }else{
            _Free( old, sizeof( list->obj )-1+id->len );
        }
        refno++;
    }
}

static void DbgProcDef( handle dbgfile, cdebug_sym_procdef *proc ){
/**** Put out proc defn & local defns*****************************/
    char             *tag, tagbuff[4*7];
    bead_def         *bead;
    hw_sym           *sym;
    dbg_sc            sc;

    tag = DbgFmtInt( &tagbuff[0], 0 ); /* linkage  */
    sc = ( proc->common.attr & FE_STATIC ) ? SC_STATIC : SC_EXPORT;
    tag = DbgFmtInt( tag, sc );
    tag = DbgFmtInt( tag, proc->pindex ); /* pindex  */
    tag = DbgFmtInt( tag, proc->lindex ); /* lindex  */
    sym = AskForHWLabel( proc->common.label );
    bead = sym->def;
    tag = DbgFmtInt( tag, bead->address ); /* start */
    bead = FindBead( bead, BEAD_EPILOGUE );
    tag = DbgFmtInt( tag, bead->address ); /* epilogue */
    bead = FindBead( bead, BEAD_ENDPROC );
    tag = DbgFmtInt( tag, bead->address ); /* end */
    PutStream( dbgfile, tagbuff, tag-tagbuff );
    LocalYTags( dbgfile, proc->list, proc->pindex+proc->lindex );
}

static void LocalYTags( handle dbgfile, cdebug_local *list, int index ) {
/******** Put out local sym tags*************************************/
    char             *tag, tagbuff[1+4+4+4+256];
    cdebug_local     *old;
    int              refno;

    tagbuff[0] = 'y';
    DbgFmtInt( &tagbuff[1], index );
    PutStream( dbgfile, tagbuff, 5 );
    tagbuff[0] = 'Y';
    refno = 0;
    while( list != NULL ){
        tag = DbgFmtInt( &tagbuff[1], refno );
        tag = DbgFmtInt( tag, list->tref );
        tag = DbgFmtInt( tag, CDEBUG_Y_LOCAL );
        tag = DbgFmtStr( tag, list->id.name, list->id.len );
        tag = DbgFmtInt( tag, list->acc );
        if( list->acc == LACC_REG ){
            tag = DbgFmtInt( tag, list->base );
        }else{
            tag = DbgFmtInt( tag, list->disp );
        }
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        old = list;
        list = list->next;
        _Free( old, sizeof( *old )-1 +old->id.len );
        refno++;
    }
}

static void DbgVar( handle dbgfile, cdebug_sym_obj *var ){
/**** Put out file scope var defns*****************************/
    char             *tag, tagbuff[4*4];
    hw_sym           *sym;
    dbg_sc            sc;

    sc = ( var->common.attr & FE_STATIC ) ? SC_STATIC : SC_EXPORT;
    tag = DbgFmtInt( &tagbuff[0], sc );
    tag = DbgFmtInt( tag, 0 ); /* access  */
    tag = DbgFmtInt( tag, 0 ); /* csect   */

    sym = AskForHWLabel( var->common.label );
    tag = DbgFmtInt( tag, sym->def->address ); /* address */
    PutStream( dbgfile, tagbuff, tag-tagbuff );
}

static void DbgExtVar( handle dbgfile, cdebug_sym_obj *var ){
/**** Put out extern variable s*****************************/
    char             *tag, tagbuff[4*3];
    hw_sym           *sym;

    tag = DbgFmtInt( &tagbuff[0], 0 ); /* access  */
    tag = DbgFmtInt( tag, 0 ); /* csect   */
    sym = AskForHWLabel( var->common.label );
    tag = DbgFmtInt( tag, var->def->address  ); /* address */
    PutStream( dbgfile, tagbuff, tag-tagbuff );
}

static void DbgExtProc( handle dbgfile, cdebug_sym_obj *var ){
/**** Put out extern function  ******************************/
    char             *tag, tagbuff[4*4];
    hw_sym           *sym;

    tag = DbgFmtInt( &tagbuff[0], 0 ); /* linkage */
    tag = DbgFmtInt( tag, 0 ); /* access  */
    tag = DbgFmtInt( tag, 0 ); /* csect   */
    sym = AskForHWLabel( var->common.label );
    tag = DbgFmtInt( tag, -1   ); /* address */
    PutStream( dbgfile, tagbuff, tag-tagbuff );
}

static void STags( handle dbgfile, bead_def *start ) {
/**** Put out souce cue tags*******/
//  Currently cdebug has problems with cues
//  if the linenums are the same keep the first
//  if the addresses are the same keep the last

    bead_def      *bead;
    bead_queue    *last;
    int           index;
    char          tagbuff[1+4+4];

    index = 0;
    while( start != NULL ) { /* find first cue */
        if( start->class == BEAD_QUEUE ) {
            index = 1;
            break;
        }
        start = start->next;
    }
    last = (bead_queue *)start;
    bead = start;
    while( bead != NULL ) { /* count ones wanted */
        if( bead->class == BEAD_QUEUE ) {
            if( ((bead_queue *)bead)->num == last->num ){
                /* keep first if nums same */
            }else if( bead->address == last->common.address ){
                /* keep last if addr same */
                last = (bead_queue*)bead;
            }else{
                last = (bead_queue*)bead;
                index++;
            }
        }
        bead = bead->next;
    }
    tagbuff[0] = 's';
    DbgFmtInt( &tagbuff[1], index );
    PutStream( dbgfile, tagbuff, 5 );
    if( index == 0 ){
        return;
    }

    tagbuff[0] = 'S';
    last = (bead_queue *)start;
    bead = start;
    while( index > 1 ) {
        if( bead->class == BEAD_QUEUE ) {
            if( ((bead_queue *)bead)->num == last->num ){
                /* keep first if nums same */
            }else if( bead->address == last->common.address ){
                /* keep last if addr same */
                last = (bead_queue*)bead;
            }else{
                DbgFmtInt( &tagbuff[1], last->num );
                DbgFmtInt( &tagbuff[5], last->common.address );
                PutStream( dbgfile, tagbuff, 9 );
                last = (bead_queue*)bead;
                index--;
            }
        }
        bead = bead->next;
    }
    DbgFmtInt( &tagbuff[1], last->num );
    DbgFmtInt( &tagbuff[5], last->common.address );
    PutStream( dbgfile, tagbuff, 9 );
}

extern char *DbgFmtInt( char *buff, offset num ) {
/** format 32 bit into 4byte dbg number ********/
    buff[0] = num>>24 & 0xff;
    buff[1] = num>>16 & 0xff;
    buff[2] = num>>8  & 0xff;
    buff[3] = num     & 0xff;
    return( &buff[4] );
}

extern char *DbgFmtStr( char *buff, char *str, id_len num ) {
/** format a str into int followed by chars**/
    char   *curr;

    buff[0] = num>>24 & 0xff;
    buff[1] = num>>16 & 0xff;
    buff[2] = num>>8  & 0xff;
    buff[3] = num     & 0xff;
    curr   = &buff[4]; /* skip length */
    while( num != 0 ) {
        *curr++ = *str++;
        --num;
    }
    return( curr );
}

static bead_def *FindBead( bead_def *bead, bead_class class ){
/**********find a bead***************************************/
    while( bead->class != class ) {
        bead = bead->next;
    }
    return( bead );
}

