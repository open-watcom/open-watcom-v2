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
#include <string.h>
#include "cgstd.h"
#include "cgmem.h"
#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "offset.h"
#include "model.h"
#include "zoiks.h"
#include "s37bead.h"
#include "s37page.h"

#include "s37page.def"

extern offset HWCalcAddr(bead_def *,offset );
extern hw_sym *HWMKPLabel(bead_def **);
extern void HWMKUsing(bead_def **,hw_sym *,char );
extern void HWMKDrop(bead_def **,char );
extern ref_any *HWDispRef(hw_sym *,hw_sym *);
extern void HWMKBRGen(bead_def **,char ,hwins_op_any *);
extern void HWMKInsGen(bead_def **,hwins_opcode,
                                   hwins_op_any *,
                                   hwins_op_any *,
                                   hwins_op_any *);
extern ref_any *HWLitAddr( hw_sym *,offset,bool );
extern int HWLitAlign(bead_def *);

extern void HWMakePages( bead_def *bead ) {
/******* add long refs and literals **********/
    page_entry  *head;
    page_entry  *curr_page;
    page_entry  **end_ptr;
    char        br;

    for(;;){
        head = NULL;
        end_ptr = &head;
        while( bead != NULL ) {
            if(  bead->class == BEAD_STARTPROC) break;
            bead = bead->next;
        }
        if( bead == NULL )break;
        bead = bead->next;
        while( bead->class != BEAD_EPILOGUE ) {
            curr_page = PageInit( bead );
            *end_ptr = curr_page;
            end_ptr = &curr_page->next;
            bead = CollectPage( curr_page );
        }
        bead = CollectToEnd( curr_page, bead );
        if( head->next == NULL ){ /* small routine */
            bead = CollectSmall( bead, head );
            continue; /* either retry routine large or NULL */
        }
        br = FirstPage( head );
        AddUsing( head->next, head->code.end, br );
        CodePages( head, br );
        FreePages( head );
    }
}

static bead_def *CollectSmall( bead_def *bead, page_entry *head ) {
/***********************************************
    collect routines < PAGE_MAX with a floating LTORG
*/
    ltorg_entry lstack;
    bead_def    *startproc;
    char        br;

    InitLtorg( &lstack, head );
    for(;;){
        br = FirstPage( head );
        AddUsing( head->next, head->code.end, br );
        CodePages( head, br );
        FreePages( head );
        while( bead != NULL ) { /* tab to routine */
            if(  bead->class == BEAD_STARTPROC) break;
            bead = bead->next;
        }
        startproc = bead;
        if( bead == NULL )break; /* done */
        bead = bead->next;
        head = PageInit( bead );
        CopyPool( &head->pool, &lstack.pool ); /* inherit pool */
        head->size = lstack.pool.size; /* set starting size */
        bead = CollectPage( head );
        if( bead->class != BEAD_EPILOGUE ){ /* bigger than max */
            bead = startproc;
            break; /* collect big */
        }
        bead = CollectToEnd( head, bead );
        if( head->next != NULL ){
            /* EPILOGUE over */
            bead = startproc;
            FreePages( head );
            break; /* collect big */
        }
        FloatLtorg( &lstack, head );
        if( lstack.has_page == FALSE ){
         /* LTORG got dumped better start over*/
            bead = startproc;
            FreePages( head );
            break; /* collect big */
        }
    }
    if( lstack.has_page ){
       /*still have last page's lits */
        DumpFloatLtorg( &lstack );
    }
    return( bead );
}

static void InitLtorg( ltorg_entry *lstack, page_entry *page ){
/*************************************************************
 Init lstack with values to force FloatLtorg to copy page
 */
     lstack->prev_code = 0;
     lstack->code = page->code;
     CopyPool( &lstack->pool, &page->pool );
     lstack->has_page = TRUE;
     page->pool.head = NULL; /* won't get dumped by code pages */

 }

static void CopyPool( lit_pool *to, lit_pool *from ){
/*************************************************************
 Copy the from pool to to
 */
     *to = *from;
     if( to->head == NULL ){
         to->end_lnk = &to->head;
     }
 }

static void FloatLtorg( ltorg_entry *lstack, page_entry *page ){
/**********************************************************************
 Try to float the ltorg as long as possible so people won't gripe.
 If the current page  + accured lits puts starting code out of range
 dump lits. Lits currently are sorted by size cause IBM assembler does
 this could change so new lits are added to end.
 A stack isn't really needed for this scheme.
*/
    int  prev_code; /* size of previous code */

    prev_code = page->code.start_addr-lstack->code.start_addr;
    prev_code += lstack->prev_code; /* plus before it */
    if( prev_code+page->size > PAGE_MAX ){ /* current code plus lits */
        DumpFloatLtorg( lstack );
    }else{
        lstack->prev_code = prev_code;
        lstack->code = page->code;
        CopyPool( &lstack->pool, &page->pool );
        page->pool.head = NULL; /* won't get dumped by code pages */
    }
}

static void DumpFloatLtorg( ltorg_entry *lstack  ){
/************************************************
    dump ltorg at head
*/
    DoLits( &lstack->pool, lstack->code.end );
    lstack->prev_code = 0;
    lstack->has_page = FALSE;
}

static bead_def *CollectPage( page_entry *page ) {
/******* collect until a PAGE_MAX is         ****/
/******* or the base epilogue is reached     ****/
    int       size;
    bead_def *bead;
    bead_def *next;

    bead = page->code.start;
    next = bead->next;
    while( bead->class != BEAD_EPILOGUE ) {
    /* not dead on for size cause of alignment gaps */
        size = next->address - page->code.end_addr+page->size;
        if( size > PAGE_MAX ) break;
        page->size = size;
        if(  bead->class == BEAD_HWINS ) {
            if( ChkRefLit( page, (any_bead_hwins *)bead ) )break;
        }else if(  bead->class == BEAD_BR ) {
            if( ChkBrRef( page,(bead_hwins_rx  *) bead ) )break;
        }else if(  bead->class == BEAD_BINDEX ) {
            AddITab( page, (bead_bindex *) bead );
        }else if(  bead->class == BEAD_LABEL ) {
            ChkLabel( page, (bead_label *) bead );
        }
        page->code.end = bead;
        page->code.end_addr = next->address;
        bead = next;
        next = bead->next;
    }
//  printf( "total %d code %d lits %d \n", page->size,
//           page->code.end_addr - page->code.start->address,
//           page->pool.size );
    return( bead );
}

static bead_def *CollectToEnd( page_entry *page, bead_def *start ) {
/** If the epilogue fits on the last page stick it there else have  **/
/** it start on a page by itself,return ENDPROC bead                **/
    bead_def   *bead;
    bead_def   *last;
    page_entry *new;
    int         size;

    bead = start;
    while( bead->class != BEAD_ENDPROC ) {
        last = bead;
        bead = bead->next;
    }
    size = last->address - start->address;
    if( page->size + size <= PAGE_MAX
     && page->refs.frefs == 0 ) { /* can't have longrefs in epilogue*/
        page->size += size;
        page->code.end = last;
        page->code.end_addr = last->address;
    }else{
        new = PageInit( start );
        new->size = size;
        new->code.end = last;
        new->code.end_addr = last->address;
        page->next = new;
    }
    return( bead );
}

static bool ChkRefLit( page_entry *page, any_bead_hwins *bead ) {
/************ add lit to pool if not over PAGE_MAX **************/
    ref_lit  *lit;
    bead_def *litdef;
    bool     over;
    int      size;
    int      lit_size;

    over = FALSE;
    lit = GetLit( (any_bead_hwins *) bead );
    if( lit != NULL ) {
        lit->common.next = NULL;
        litdef = FindLit( page->pool.head, lit->val );
        if( litdef != NULL ) {
            lit->def = litdef;
        }else { /* then value is def */
            litdef = lit->val;
            litdef->next = NULL;
            lit_size =  HWCalcAddr( litdef, 0 );
            size = page->size + lit_size;
            if( size > PAGE_MAX ) {
                over = TRUE;
            }else {
                page->size = size;
                lit->def = litdef;
                *page->pool.end_lnk = litdef;
                page->pool.size += lit_size;
                page->pool.end = litdef;
                page->pool.end_lnk = &litdef->next;
            }
        }
    }
    return( over );
}

static bool ChkBrRef( page_entry *page, bead_hwins_rx *bead ) {
/***** check for long refs add if not over PAGE_MAX *****/
    bool      over;
    int       size;
    ref_sym  *ref;
    hw_sym   *sym;
    sym_list *lsyms;
    sym_list **link;

    over = FALSE;
    ref = bead->s2.ref;
    sym = ref->sym;
    link = &page->refs.lsyms;
    lsyms = *link;
    while( lsyms != NULL ) {
        if( lsyms->sym == sym )break;
        link  = &lsyms->next;
        lsyms = *link;
    }
    if( lsyms == NULL ) {
        size = page->size + PAGE_REF_COST;
        if( size > PAGE_MAX ) {
            over = TRUE;
        }else {
            page->size = size;
            page->refs.frefs++;
            lsyms = CGAlloc( sizeof( *lsyms ) );
            lsyms->sym = sym;
            lsyms->next = NULL;
            lsyms->defined = FALSE;
            lsyms->frefs = 1;
            lsyms->brefs = 0;
            *link = lsyms;
        }
    }else {
        if( lsyms->defined ) {
            lsyms->brefs--;
            page->refs.brefs--;
        } else {
            lsyms->frefs++;
            page->refs.frefs++;
        }
    }

    return( over );
}

static void ChkLabel( page_entry *page, bead_label *bead ) {
/****** say sym is defined on sym list ***********/
    hw_sym   *sym;
    sym_list *lsyms;
    sym_list **link;

    sym = bead->sym;
    link = &page->refs.lsyms;
    lsyms = *link;
    while( lsyms != NULL ) {
        if( lsyms->sym == sym )break;
        link  = &lsyms->next;
        lsyms = *link;
    }
    if( lsyms == NULL ) {
        lsyms = CGAlloc( sizeof( *lsyms ) );
        lsyms->sym = sym;
        lsyms->next = NULL;
        lsyms->frefs = 0;
        *link = lsyms;
    } else { /* subtract fref size cost and sym fref count */
        page->refs.frefs -= lsyms->frefs;
        page->size -= PAGE_REF_COST;
    }
    lsyms->brefs = CalcBackRefs( sym );
    page->refs.brefs += lsyms->brefs;
    lsyms->defined = TRUE;
 }

static void AddITab( page_entry *page, bead_bindex *bead ) {
/**** Add index table to table list ************************/
    bindex_list *new;
    ref_lit     *lit;
    bead_def *litdef;

    new = CGAlloc( sizeof( *new ) );
    new->bri = bead;
    new->next = page->bri_list;
    page->bri_list = new;
    lit = bead->lit;
    litdef = lit->val;
    litdef->next = NULL;
    lit->def = litdef; /* lit only used by bindex */
    *page->pool.end_lnk = litdef;
    page->pool.size += 4; /* lit for table */
    page->pool.end_lnk = &litdef->next;
}

static bead_def *FindLit( bead_def *curr, bead_def *litval ) {
/***** Find equivalent bead in list or return NULL   ********/
    while( curr != NULL ) {
        if( curr->class == litval->class  ) {
            switch( curr->class ) {
            case BEAD_ADDR:
                if( ((bead_addr*) curr)->ref==((bead_addr *)litval)->ref
                &&  ((bead_addr*) curr)->val==((bead_addr *)litval)->val ) {
                return( curr );
                }
                break;
            case BEAD_INT:
                if( ((bead_int*) curr)->value==((bead_int *)litval)->value
                &&  ((bead_int*) curr)->size ==((bead_int *)litval)->size ) {
                return( curr );
                }
                break;
            case BEAD_FLT:
                if( ((bead_flt*) curr)->value==((bead_flt *)litval)->value
                &&  ((bead_flt*) curr)->size ==((bead_flt *)litval)->size ) {
                return( curr );
                }
                break;
            }
        }
        curr = curr->next;
    }
    return( curr );
}

static int  SortLitPool( lit_pool *pool ){
/**** sort the lit pool by D F H X alignment ***/
/**** return alignment                       ***/
    int i;
    int align;
    bead_def *list;
    struct { bead_def *head; bead_def **end_lnk;} cell[4];

    for( i = 0; i < 4; ++i ){
        cell[i].head = NULL;
        cell[i].end_lnk = &cell[i].head;
    }
    list = pool->head;
    while( list != NULL ){
        i = HWLitAlign( list );
        *cell[i].end_lnk = list;
        cell[i].end_lnk = &list->next;
        list = list->next;

    }
    list = NULL;
    for( i = 0; i < 4; ++i ){
        if( cell[i].head != NULL ){
            align = i;
            *cell[i].end_lnk = list;
            list = cell[i].head;
        }
    }
    pool->head = list;
    if( list != NULL ){
         while( list->next != NULL ){
             list = list->next;
         }
        pool->end = list;
        pool->end_lnk = &list->next;
    }
    return( 1<< align );
}

static ref_lit *GetLit( any_bead_hwins *bead ) {
/*******Assume one lit per bead************/
    hwins_op_sx  *op1;
    hwins_op_sx  *op2;

    op1 = NULL;
    op2 = NULL;
    switch( bead->ins.class )  {
    case HWINS_RR:
        break;
    case HWINS_RX:
        op1 = &bead->rx.s2;
        break;
    case HWINS_RS1:
        op1 = &bead->rs1.s2;
        break;
    case HWINS_RS2:
        op1 = &bead->rs2.s2;
        break;
    case HWINS_SI:
        op1 = &bead->si.s1;
        break;
    case HWINS_S:
        op1 = &bead->s.s2;
        break;
    case HWINS_SS1:
        op1 = &bead->ss1.s1;
        op2 = &bead->ss1.s2;
        break;
    case HWINS_SS2:
        op1 = &bead->ss2.s1;
        op2 = &bead->ss2.s2;
        break;
    }
    if( op1 == NULL ){
         return( NULL );
    }
    if( op1->ref != NULL && op1->ref->entry.class == REF_LIT ) {
        return( op1->ref );
    }
    if( op2 == NULL ) {
        return( NULL );
    }
    if( op2->ref != NULL && op2->ref->entry.class == REF_LIT ) {
        return( op2->ref );
    }
    return( NULL );
}

static int CalcBackRefs( hw_sym *sym  ) {
/* calc number of backward references to label */
    offset address;
    ref_sym *ref;
    int     count;

    address = sym->def->address;
    count = 0;
    for( ref = &sym->refs->sym; ref != NULL; ref = ref->common.next ) {
        if( ref->use->address > address ) {
            count++;
        }
    }
    return( count );
}

static  void CodePages( page_entry *head, char br ) {
/****Dump out jumps literal etc*******/
    page_entry *curr;

    curr = head;
    while( curr != NULL ){
        DoLongJmps( curr, head, br );
        DoIndexJmps( curr, head, br );
        DoLits( &curr->pool, curr->code.end );
        curr = curr->next;
    }
}

static char FirstPage( page_entry *page ) {
/***find entry label and main base reg for function******/
    bead_def *bead;
    char      br;

    bead = page->code.start;
    br = 0;
    if( bead->class == BEAD_LABEL ){
            page->usesym = ((bead_label*)bead)->sym;
    }else{
        Zoiks( ZOIKS_066 ); /* no entry point */
    }

    while( bead != page->code.end ) {
        if( bead->class == BEAD_USING ) {
            br = ((bead_using*)bead)->reg;
        }
        bead = bead->next;

    }
    return( br );
}

static void AddUsing( page_entry *page, bead_def *prev, char br ) {
/***Add Label if needed and using statement **/
    bead_def *start;

    while( page != NULL ) {
        start = page->code.start;
        if( start->class == BEAD_LABEL ){
            page->usesym = ((bead_label*)start)->sym;
        }else{
            page->usesym = HWMKPLabel( &prev->next );
            start = prev->next;
            start->address = page->code.start_addr;
            page->code.start = start;
        }
        HWMKUsing( &start->next, page->usesym, br );
        prev = page->code.end;
        page = page->next;

    }
}

static void DoLongJmps( page_entry *page, page_entry *head, char br ) {
/***Dump out long branches to far away places*/
    sym_list *lsyms;
    hw_sym   *jmplabel;
    hw_sym   *basesym;
    bead_def *jmpvec;
    int       jmpdisp;

    lsyms = page->refs.lsyms;
    jmpvec = page->code.end;
    if( !(jmpvec->class == BEAD_BR &&((bead_hwins_rx *)jmpvec)->r1 == 15 )){
      /* need branch to next page */
        if( page->next != NULL ) {
            basesym = page->next->usesym;
            AddLoadBase( &jmpvec->next, basesym, &page->pool, br );
            jmpvec = jmpvec->next;
            AddBCR( &jmpvec->next, br );
            jmpvec = jmpvec->next;
        }
    }
    if( lsyms != NULL ) {
        jmplabel = HWMKPLabel( &jmpvec->next );
        jmpvec = jmpvec->next;
    }
    jmpdisp = 0;
    while( lsyms != NULL ) {
        if( !lsyms->defined ) {
            basesym = GetBaseFor(  lsyms->sym, head );
            IndirectRefs( lsyms->sym, &page->code, jmplabel, jmpdisp );
            AddLoadBase( &jmpvec->next, basesym, &page->pool, br );
            jmpvec = jmpvec->next;
            AddFarJmp( &jmpvec->next, lsyms->sym, basesym, br );
            jmpvec = jmpvec->next;
            jmpdisp += PAGE_JMP_COST;
        }
        lsyms = lsyms->next;
    }
    page->code.end = jmpvec;
}

static void DoIndexJmps( page_entry *page, page_entry *head, char br ) {
/***Fixup index table for index jumps*************************/
    bindex_list *bri_list;
    bead_def    *table;
    bead_bindex *bri;
    bool        br_load;

    bri_list = page->bri_list;
    while( bri_list != NULL ) {
        bri = bri_list->bri;
        bri->br = br;
        table = bri->table->def;
        br_load = UpdateDisps(  table, page->usesym, head );
        ExpandBri( bri, br_load);
        bri_list = bri_list->next;
    }
}

static void AddFarJmp( bead_def **at, hw_sym *sym, hw_sym *base, char br  ) {
    hwins_op_any  dst;

    dst.sx.ref = HWDispRef( sym, base );
    dst.sx.disp = 0;
    dst.sx.a = 0;
    dst.sx.b = br;
    HWMKBRGen( at, 15,  &dst );
}

static void AddBCR( bead_def **at, char br  ) {
/*** to A BCR  br *****************************/
    hwins_op_any  cc;
    hwins_op_any  dst;

    cc.r = 15;
    dst.r = br;
    HWMKInsGen( at, HWOP_BCR, &cc, &dst, NULL );
}

static void AddLoadBase( bead_def **at,   hw_sym *sym,
                         lit_pool  *pool, char    br  ) {

    hwins_op_any   hwop1;
    hwins_op_any   hwop2;

    hwop1.r = br;
    hwop2.sx.ref = MakeLitAddr( pool, sym );
    hwop2.sx.disp = 0;
    hwop2.sx.a = 0;
    hwop2.sx.b = 0;
    HWMKInsGen( at, HWOP_L, &hwop1,  &hwop2, NULL );
}

static ref_lit *MakeLitAddr( lit_pool *pool, hw_sym *sym ) {
/*** Add litaddr of sym to pool ***************************/
    ref_lit       *lit;
    bead_def      *litdef;

    lit = HWLitAddr( sym, 0, FALSE );
    lit->common.next = NULL;
    litdef = FindLit( pool->head, lit->val );
    if( litdef != NULL ) {
        lit->def = litdef;
    }else { /* then value is def */
        litdef = lit->val;
        litdef->next = NULL;
        lit->def = litdef;
        *pool->end_lnk = litdef;
        pool->end = litdef;
        pool->end_lnk = &litdef->next;
    }
    return( lit );
  }

static hw_sym *GetBaseFor( hw_sym *sym, page_entry *page_list ) {
/* find the using sym for page where sym is defined in */
    offset address;

    address = sym->def->address;
    while( address > page_list->code.end_addr  ) {
        page_list = page_list->next;
    }
    return( page_list->usesym );
}

static void IndirectRefs( hw_sym *sym,   code_entry *code,
                          hw_sym *label, int         disp   ) {
/** Adjust refs of symbol in page to indirect to jmp table */
    ref_sym  *refs;
    ref_sym  **end_lnk;
    bead_def *use;

    end_lnk = &sym->refs;
    refs = *end_lnk;
    while( refs != NULL ) {
        use = refs->use;
        if( use->address >= code->start_addr
         && use->address < code->end_addr  ) {
             *end_lnk = refs->common.next;
             refs->sym = label;
             refs->common.next = label->refs;
             label->refs = refs;
             ((bead_hwins_rx*)use)->s2.disp = disp;
        }else {
            end_lnk  = &refs->common.next;
        }
        refs = *end_lnk;
    }

}

static void DoLits( lit_pool *pool, bead_def *end ) {
/** Dump literals out************************/
    bead_ltorg *ltorg;

    if( pool->head != NULL ) {
        ltorg = CGAlloc( sizeof( *ltorg ) );
        ltorg->common.class = BEAD_LTORG;
        *pool->end_lnk = NULL; /* snip pool at end */
        ltorg->align = SortLitPool( pool );
        *pool->end_lnk = end->next; /* link to rest of beads */
        ltorg->common.next = pool->head;
        ltorg->end = pool->end;
        end->next = (bead_def*)ltorg;
    }
}


static bool UpdateDisps( bead_def *table, hw_sym *curr_base, page_entry *head ){
/*** Update disp addr with base symbol where table used **/
    hw_sym  *base;
    bool    expand;
    bead_disp *disp;
    bead_disp *bead;

    expand = FALSE;
    disp = (bead_disp *)table->next;
    while( disp != NULL && disp->common.class == BEAD_DISP ) {
        base = GetBaseFor( disp->ref, head );
        disp->base = base;
        if( curr_base != base ) {
            expand = TRUE;
        }
        disp = (bead_disp *)disp->common.next;
    }
    if( expand ) {
        disp = (bead_disp *)table->next;
        while( disp != NULL && disp->common.class == BEAD_DISP ) {
            bead = CGAlloc( sizeof( *bead ) );
            bead->common.class = BEAD_DISP;
            bead->val = 0;
            bead->ref =  disp->base;
            bead->base = curr_base;
            bead->op_len = DISP_SUB|2;
            bead->common.next = (bead_def *)disp;
            table->next = (bead_def *)bead;
            table = (bead_def *)disp;
            disp = (bead_disp *)disp->common.next;
        }
    }
    return( expand );
}

static void ExpandBri( bead_bindex *bri, bool br_load){
/**** Expand Index macro into AH's and branches**/
    bead_def     *curr_ins;
    hwins_op_any hwop1;
    hwins_op_any hwop2;

    curr_ins = (bead_def *)bri;
    hwop1.r = bri->reg;
    if( br_load ) {
        HWMKInsGen( &curr_ins->next, HWOP_AR, &hwop1, &hwop1, NULL );
        curr_ins = curr_ins->next;
    }
    hwop2.sx.ref = bri->lit;
    hwop2.sx.disp = 0;
    hwop2.sx.a = 0;
    hwop2.sx.b = 0;
    HWMKInsGen( &curr_ins->next, HWOP_A, &hwop1, &hwop2, NULL );
    curr_ins = curr_ins->next;
    if( br_load ) {
        hwop1.r = bri->br;
        hwop2.sx.b = bri->reg;
        hwop2.sx.ref  = NULL;
        hwop2.sx.a = 0;
        hwop2.sx.disp = 0;
        HWMKInsGen( &curr_ins->next, HWOP_AH, &hwop1, &hwop2, NULL );
        curr_ins = curr_ins->next;
        hwop2.sx.disp = 2;
    }else{
        hwop2.sx.disp = 0;
    }
    hwop1.r = bri->reg;
    hwop2.sx.b = bri->reg;
    hwop2.sx.ref  = NULL;
    hwop2.sx.a = 0;
    HWMKInsGen( &curr_ins->next, HWOP_LH, &hwop1, &hwop2, NULL );
    curr_ins = curr_ins->next;
    hwop2.sx.ref = NULL;
    hwop2.sx.disp = 0;
    hwop2.sx.a = bri->reg;
    hwop2.sx.b = bri->br;
    HWMKBRGen( &curr_ins->next, 15,  &hwop2 );
    bri->size = 0;
}

static page_entry *PageInit( bead_def *bead ) {
/**** Init page entry to start state *********/
    page_entry *page;

    page = CGAlloc( sizeof( *page ) );
    page->next = NULL;
    page->size = 8; /* lit pool alignment adjustment */
    page->usesym = NULL;
    page->code.start = bead;
    page->code.start_addr = bead->address;
    page->code.end = bead;
    page->code.end_addr = bead->address;
    page->refs.frefs = 0;
    page->refs.brefs = 0;
    page->refs.lsyms = NULL;
    page->pool.head = NULL;
    page->pool.end = NULL;
    page->pool.end_lnk = &page->pool.head;
    page->pool.size = 8;  /* lit pool adjustment */
    page->bri_list = NULL;
    return( page );
}

static void FreePages( page_entry *page ) {
/*****************************************
    Free a page and all it's trash
*/

    while( page != NULL ){
        page_entry *next;
        /*** free syms ***/
        {
            sym_list          *lsyms, *next;

            for( lsyms = page->refs.lsyms; lsyms != NULL; lsyms = next ){
                next = lsyms->next;
                CGFree( lsyms );
            }
        }
        /*** free index branch ***/
        {
            bindex_list *bri_list, *next;

            for( bri_list = page->bri_list; bri_list != NULL; bri_list = next ){
                next = bri_list->next;
                CGFree( bri_list );
            }
        }

        next = page->next;
        CGFree( page );
        page = next;
    }
}

extern void HWShortRef( bead_startproc *list ){
/**********************************************
 convert any L r,=(A) to LA r,A if in range
*/
#if 0
    bead_def *bead, *endproc, **lnk;
    ref_lit  *lit;
    bead_def *litdef;
    hw_sym *using;

    using = NULL;
    while(  list != NULL ){
        lnk = &list->common.next;
        endproc = (bead_def *)list->endproc;
        while( (bead = *lnk) != endproc ){
            if( bead->class == BEAD_USING ) {
                using = ((bead_using*)bead->sym;
            }else if( bead->class == BEAD_HWINS ) {
                if( ((any_bead_hwins*)bead)->ins.op == HWOP_L ){
                    lit = GetLit( (any_bead_hwins *) bead );
                    litdef = lit->value;
                    if( litdef->class == BEAD_ADDR ){
                      InRange( using->sym, (bead_addr*)litdef, bead );
                    }
                }
            }
            lnk = &bead->next;
        }
        list = list->prev;
    }
#endif
}
#if 0
static void InRange( hw_sym *base, bead_addr *acon, bead_hwins_rx *ins ){
/****************************************************
    is the acon in range of base sym so we can do a LA
*/
    hw_sym *sym;

    sym = acon->ref;
    if( sym->class != HW_EXTERN ){
    }
}
#endif
