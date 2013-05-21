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
#include <stdlib.h>
#include <string.h>
#include "cgstd.h"
#include "cgmem.h"
#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "offset.h"
#include "model.h"
#include "s37bead.h"
#include "s37page.h"

#include "s37obj.def"

static    index_rec     *CurrIndex;
static    index_rec     *IndexRecs;
static    hw_sym        *HWSyms;
static    segdef        *SegDefs;
static    seg_id        CodeSeg;
static    seg_id        BackSeg;

extern  pointer         FEAuxInfo(pointer*,aux_class);
extern  void            FEMessage(msg_class,pointer);
extern  double          CFToF( pointer );
extern void GenTXT(bead_xsym *, txtseg_rec * );
extern void DbgIO( txtseg_rec * );
extern void HWMakePages( bead_def *start );

extern hwop_info const HWOpTable[] = {
    #define _ENC_OP( op, cl, cd ) #op, HWOP_##op, cl, cd,
    #include "s37ops.h"
    #undef _ENC_OP
};

static int const HwLength[] = {
    sizeof( bead_hwins_rr ),
    sizeof( bead_hwins_rx ),
    sizeof( bead_hwins_rs1 ),
    sizeof( bead_hwins_rs2 ),
    sizeof( bead_hwins_si ),
    sizeof( bead_hwins_s ),
    sizeof( bead_hwins_ss1 ),
    sizeof( bead_hwins_ss2 ),
    sizeof( bead_hwins_ssp )
};

int const HWOpICL[]= {
     2  /* rr  */,
     4  /* rx  */,
     4  /* rs1 */,
     4  /* rs2 */,
     4  /* si  */,
     4  /* s   */,
     6  /* ss1 */,
     6  /* ss2 */,
     6  /* ssp */
};


extern  void    ObjInit() {
/*************************/
}



extern void AbortObj() {
/**********************/

}

extern void SetDataOffsets( offset location ) {
/*********************************
 Need to collect and set addr on all data that is
 fixed at codegen time so we can base off temp in code
*/

    segdef *curr;
    index_rec *rec;
    bead_def  *bead;

    curr = SegDefs;
    while( curr != NULL ) { /* link segments to correct txtseg */
        if( !(curr->attr & ( ROM | EXEC ))  ) { /* if not code */
            rec =  AskSegIndex( curr->id );
            location = HWCalcAddr( rec->first, location );
            bead = rec->first;
            while( bead != NULL ){
                if( bead->class == BEAD_LABEL ){
                    ((bead_label *)bead)->sym->class = HW_FIXED;
                }
                bead = bead->next;
            }
        }
        curr = curr->next;
    }
}

extern  void    ObjFini() {
/**************************/
    index_rec *rec;
    txtseg_rec  code;
    txtseg_rec  data;
    bead_xsym *entry;
    bead_startproc *last;

    rec =  AskSegIndex( AskCodeSeg() );
    last = LinkProcs( rec->first );
    if( !(_IsModel( DBG_LOCALS ) || _IsModel( DBG_TYPES )) ) {
        PeepS370( last );
    }
    HWMakePages( rec->first );
    if(  FEAuxInfo( NULL, AUX_HAS_MAIN ) ) {
        entry = MKMain( &rec->first->next );
    } else {
        entry = NULL;
    }
   MKTxtSegs( &code, &data );
   if( _IsModel( DBG_LOCALS ) || _IsModel( DBG_TYPES ) ) {
       DbgIO( &code ); /* adds to code txtseg_rec */
   }
// HWShortRefs( last );
   GenTXT( entry, &code );
   FreeBeads( code.first );
   if( _IsModel( CODE_SPLIT ) || _IsModel( CODE_RENT ) ) {
       GenTXT( NULL, &data );
       FreeBeads( data.first );
   }
   FreeHWSyms( HWSyms );
   FEMessage( MSG_CODE_SIZE, (pointer)code.location );
// FEMessage( MSG_DATA_SIZE, (pointer)data.location );
}

static void MKTxtSegs( txtseg_rec *code, txtseg_rec *data ) {
/*** collect CODE and DATA segements add CSECTS ******/
    segdef *curr;
    index_rec *rec;
    bead_xsym *xcode;

    InitTxtSeg( code, TXT_CODE );
    InitTxtSeg( data, TXT_DATA );
    curr = SegDefs;
    while( curr != NULL ) { /* link segments to correct txtseg */
        rec =  AskSegIndex( curr->id );
        if( rec->first != NULL ) {
            if( rec->txtseg == code->txtseg ) {
                *code->end_lnk = rec->first;
                code->end_lnk  = rec->end_lnk;
            } else {
                *data->end_lnk = rec->first;
                data->end_lnk  = rec->end_lnk;
            }
        }
        curr = curr->next;
    }
    code->csect = MakeCsect( &code->first, TXT_CODE );
    if( _IsModel( CODE_SPLIT ) || _IsModel( CODE_RENT ) ) {
        data->csect = MakeCsect( &data->first, TXT_DATA );
        xcode = HWMKExSym( &data->first->next, HW_EXTERN );
        xcode->sym = code->csect->sym;
        data->other = xcode;
        xcode = HWMKExSym( &code->first->next, HW_EXTERN );
        xcode->sym = data->csect->sym;
        code->other = xcode;
        data->location = HWCalcAddr( data->first, 0 );
    } else if( data->first != NULL )  {/* if not split link up code to data */
        *code->end_lnk = data->first;
        code->end_lnk  = data->end_lnk;
        code->other = code->csect;
    }
    code->location = HWCalcAddr( code->first, 0 );
}

static void InitTxtSeg( txtseg_rec *rec, txtseg_class txtseg ) {
/*** Init a txt segment i.e  code or data***/
    rec->txtseg = txtseg;
    rec->first = NULL;
    rec->end_lnk = &rec->first;
    rec->csect = NULL;
    rec->other = NULL;
    rec->location = 0;
}

static bead_xsym *MakeCsect( bead_def **end_lnk, txtseg_class txtseg ) {
/***********************/
    char       fname[8+1];
    bead_xsym *bead;
    hw_sym    *sym;
    csect_mode armode;

    bead = HWMKExSym( end_lnk, XSYM_CSECT );  /* an CSECT defines a sym */
    GetCsectName( fname, txtseg );
    sym = HWSymAdd( &HWSyms, fname );
    sym->def = (bead_def *)bead;
    sym->defflag = txtseg;
    if( _IsModel( AMODE_24 ) ) {
        armode = CSECT_AMODE_24;
    }else if( _IsModel( AMODE_31 ) ) {
        armode = CSECT_AMODE_31;
    } else {
        armode = CSECT_AMODE_ANY;
    }
    if( _IsntModel( RMODE_24 ) ) {
        armode |= CSECT_RMODE_ANY;
    }
    bead->armode = armode;
    bead->sym = sym;
    return( bead );
}

static void GetCsectName( char *fname, txtseg_class txtseg ) {
/**** make a CSECT name ************************************/
    char *curr, *path;
    char *end;

    path =  FEAuxInfo( NULL, CSECT_NAME  );
    curr = fname;
    if( txtseg == TXT_CODE ) {
        *curr++ = '@';
        end = &fname[8];
    } else {
        end = &fname[7];
    }
    while( curr < end && *path != '\0' ) {
        *curr++ = *path++;
    }
    if( txtseg == TXT_DATA ) {
        *curr++ = '@';
    }
    *curr = '\0';
}

static bead_xsym *MKMain( bead_def **end_lnk ) {
/***make ENTRY  $MAIN 0 in csect with branch to $STARTUP **/
/***assume code txtsegment ***/
    hwins_op_any hwop1;
    hwins_op_any hwop2;
    hw_sym    *sym;
    bead_xsym  *entry;
    bead_xsym  *startup;

    sym = HWSymAdd( &HWSyms, "#MAIN" );
    HWMKLabel( end_lnk, sym );
    sym->class = HW_ENTRY;
    end_lnk = &(*end_lnk)->next; /*after added bead */
    entry = HWMKExSym( end_lnk, XSYM_ENTRY  );
    entry->sym = sym;
    end_lnk = &(*end_lnk)->next;
    hwop1.r = 15;    /*assume BALR 14,15 linkage **/
    hwop2.sx.ref = NULL;
    hwop2.sx.disp = 6;
    hwop2.sx.b = 0;
    hwop2.sx.a = 15; /*addressability off R15   **/
    HWMKInsGen( end_lnk, HWOP_L, &hwop1, &hwop2, NULL );
    end_lnk = &(*end_lnk)->next;
    hwop1.r = 15;    /*CC unconditional        ***/
    hwop2.r = 15;    /* 15 linkage **/
    HWMKInsGen( end_lnk, HWOP_BCR, &hwop1, &hwop2, NULL );
    end_lnk = &(*end_lnk)->next;
    sym = HWSymAdd( &HWSyms, "$STARTUP" );
    sym->class = HW_EXTERN;
    sym->defflag = TXT_CODE;
    startup = HWMKExSym( end_lnk, XSYM_EXTRN );
    startup->sym = sym;
    end_lnk = &(*end_lnk)->next;
    HWMKAddrGen( end_lnk, sym, 0 );
    return( entry );
}

extern  void    InitSegDefs() {
/**********************************************/
/* object segments */
    SegDefs = NULL;
    CodeSeg = 0;
    BackSeg = 0;
    IndexRecs = NULL;
    HWSyms = NULL;
}

extern  void    DefSegment( seg_id id, seg_attr attr, char *str, uint align ) {
/**********************************************/
    segdef              *new;
    segdef              **owner;

    new = CGAlloc( sizeof( *new ) );
    new->id = id;
    new->attr = attr;
    new->align = align;
    new->str = CGAlloc( strlen( str ) + 1 );
    strcpy( new->str,str );
    owner = &SegDefs;
    while( *owner != NULL ) { /* keep in a sorted order */
        if( (*owner)->id >= id )break;
        owner = &(*owner)->next;
    }
    new->next = *owner;
    *owner = new;
    if( AskSegIndex( id ) == NULL ){
        DoASegDef( new );
    }
    if( ( attr & EXEC ) != 0 && CodeSeg == 0 ) {
        CodeSeg = id;
    }
    if( attr & BACK ) {
        BackSeg = id;
    }

}

static void DoASegDef( segdef *seg ) {
/*****************************************/

    index_rec   *rec;
    bead_seg    *first;

    rec = CGAlloc( sizeof( *rec ) );
    if( _IsModel( CODE_SPLIT ) || _IsModel( CODE_RENT ) ) {
        if( seg->attr & ( ROM | EXEC )  ) { /* put all read only in code */
            rec->txtseg = TXT_CODE;
        } else { /* rw goes into data */
            rec->txtseg = TXT_DATA;
        }
    } else { /* no split all in code*/
        rec->txtseg = TXT_CODE;
    }
    rec->location = 0;
    first = MKSeg( seg->str, seg->id, seg->align );
    rec->end_lnk  = &first->common.next;
    rec->first = (bead_def *)first;
    rec->next = IndexRecs;
    IndexRecs   = rec;
    rec->seg = seg->id;
}

extern  seg_id  AskCodeSeg() {
/****************************/

    return( CodeSeg );
}


extern  seg_id  AskBackSeg() {
/****************************/
/* returns backend data segment id */
     return( BackSeg );
}


extern  seg_id  SetOP( seg_id seg ) {
/***********************************/

    seg_id      old;

    if( CurrIndex == NULL ) {
        old = (seg_id)-1;
    } else {
        old = CurrIndex->seg;
    }
    if( seg == (seg_id)-1 ) {
        CurrIndex = NULL;
    } else {
        CurrIndex = AskSegIndex( seg );
    }
    return( old );
}

extern  seg_id  AskOP() {
/************************/

    return( CurrIndex->seg );
}


extern  void    FlushOP( seg_id id ) {
/************************************/
/* flush segment out to file */
    id = 0;
}


extern        bool    AskSegBlank( segment_id id )
/************************************************/
{   id = 0;
    return( FALSE );
}


extern        bool    AskSegROM( segment_id id )
/**********************************************/
{
    return( _IsModel( CODE_RENT ) && AskSegIndex( id )->txtseg == TXT_CODE );
}


extern  offset  AskLocation() {
/*****************************/
/* return current offset in segment */

    return( CurrIndex->location );
}


extern  void    SetLocation( offset loc ) {
/**********************************************/
/* seek to loc in segment extend if past end */
    offset curloc;
    char fill = 0;

    curloc = CurrIndex->location;
    if( curloc < loc ) {
        if( _IsModel( NO_ZERO_INIT ) ) {
            HWStoreGen( loc-curloc );
        }else{
            HWDataGen( 1, loc-curloc, &fill );
        }
    }

}

extern  void    TellObjNewProc( sym_handle proc ) {
/***************************************************/
    proc = proc;
}

static index_rec *AskSegIndex( seg_id seg ) {
/**************************************************/

    index_rec   *rec;

    rec = IndexRecs;
    while( rec != NULL ) {
        if( rec->seg == seg ) break;
        rec = rec->next;
    }
    return( rec );
}


extern  void            *InitPatch() {
/**********************************************/
/* ??? */

    return( NULL );
}



extern void             AbsPatch(void * patch,offset lc) {
/**********************************************/
patch = 0;
lc = 0;
}



extern hw_sym *HWSymLook( char  *name ) {
/******************************************************/
    hw_sym *curr;

    curr = HWSyms;
    while( curr != NULL ) {
        if( strcmp( name, curr->name )==0 ) break;
        curr = curr->next;
    }
    return( curr );
}

extern hw_sym *HWSymHandle(void ) {
/***Make a hw sym handle *********/
    hw_sym *curr;

    curr = CGAlloc( sizeof( *curr ) );
    curr->name[0] = '\0';
    curr->class = HW_NONE;
    curr->defflag = TXT_NO;
    curr->def = NULL;
    curr->refs = NULL;
    curr->next = HWSyms;
    HWSyms = curr;
    return( curr );
}

extern hw_sym *HWFRefSym( char  *name ) {
/******************************************************/
    hw_sym *curr;

    curr = HWSymAdd( &HWSyms, name );
    return( curr );
}

static hw_sym *HWSymAdd( hw_sym **next_lnk, char *name ) {
/******************************************************/
    hw_sym *curr;

    curr = *next_lnk;
    while( curr != NULL ) {

        if( strcmp( name, curr->name )==0 ) break;
        next_lnk = &curr->next;
        curr = curr->next;
    }
    if( curr == NULL ) {
        curr = CGAlloc( sizeof( *curr ) );
        strcpy( curr->name, name );
        curr->class = HW_NONE;
        curr->defflag = TXT_NO;
        curr->def = NULL;
        curr->refs = NULL;
        *next_lnk = curr;
        curr->next = NULL;
    }
    return( curr );
}

static void FreeHWSyms( hw_sym *sym  ){
/*********************************
    Free all those beads
*/
    hw_sym *next;

    while( sym != NULL ){
        next = sym->next;
        CGFree( sym );
        sym = next;
    }
}
static bead_def *AddBead( bead_def *new, int size ) {
/******************************************************/
    /* add bead at end of list */
    new->address = CurrIndex->location;
    CurrIndex->location += size;
    new->next = NULL;
    *CurrIndex->end_lnk = new;
    CurrIndex->end_lnk = &new->next;
    return( new );
}

extern void HWLabelGen( hw_sym *sym, char align  ) {
/*********************************/
    bead_label *bead;

    sym->class = HW_LABEL;
    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_LABEL;
    bead->class = LBL_DS;
    bead->align = align;
    AddBead( (bead_def *)bead, 0 );
    bead->sym = sym;
    sym->def = (bead_def*)bead;
    sym->defflag = CurrIndex->txtseg;
}

extern hw_sym *HWMKPLabel( bead_def **end_lnk ) {
/***Insert Internal Label *********************/
    hw_sym     *sym;

    sym  = HWSymHandle();
    HWMKLabel( end_lnk, sym );
    return( sym );
}

extern void HWMKLabel( bead_def **end_lnk, hw_sym *sym ) {
/***Insert Internal Label *********************/
    bead_label *bead;

    sym->class = HW_LABEL;
    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_LABEL;
    bead->common.next = *end_lnk;
    bead->class = LBL_DS;
    bead->align = 2;
    *end_lnk = (bead_def*)bead;
    bead->sym = sym;
    sym->def = (bead_def*)bead;
    sym->defflag = TXT_CODE;
}

extern void HWExtern( hw_sym *sym ) {
/***************************/
    bead_xsym *bead;

    if( !(sym->defflag & CurrIndex->txtseg) ) { /*add extern to txtseg */
        bead = HWExSym( XSYM_EXTRN );  /* an EXTERN references a sym */
        sym->def = (bead_def *)bead;
        sym->class = HW_EXTERN;
        sym->defflag |= CurrIndex->txtseg;
        bead->sym = sym;
    }
}

extern void HWEntry( hw_sym *sym ) {
    bead_xsym *bead;

    bead = HWExSym( XSYM_ENTRY );/* an ENTRY references a sym */
    sym->defflag = CurrIndex->txtseg;
    sym->class = HW_ENTRY;
    bead->sym = sym;
}

extern bead_xsym *HWExSym( xsym_class class ) {
/*** Add a xsym to top of index_rec***********/
    bead_xsym *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_XSYM;
    AddBead( (bead_def *)bead, 0 );
    bead->class = class;
    bead->sym = NULL;
    bead->id = 0;
    return( bead );
}

extern bead_xsym *HWMKExSym( bead_def **end_lnk, xsym_class class ) {
/*********************************************/
    bead_xsym *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_XSYM;
    bead->common.address = 0;
    bead->common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;
    bead->class = class;
    bead->sym = NULL;
    bead->id = 0;
    return( bead );
}

extern bead_using *HWUsing( hw_sym *sym, char reg ) {
/** set up using sym,reg if sym null *,reg **/
    bead_using *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_USING;
    AddBead( (bead_def *)bead, 0 );
    bead->sym = sym;
    bead->reg = reg;
    return( bead );
}

extern void HWMKUsing( bead_def **end_lnk, hw_sym *sym, char reg ) {
/** set up using sym,reg if sym null *,reg **/
    bead_using *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_USING;
    bead->common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;
    bead->sym = sym;
    bead->reg = reg;
}

extern bead_drop *HWDrop( char reg ) {
/** drop base reg **/
    bead_drop *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_DROP;
    AddBead( (bead_def *)bead, 0 );
    bead->reg = reg;
    return( bead );
}

extern void HWMKDrop(bead_def **end_lnk, char reg ) {
/** drop base reg **/
    bead_drop *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_DROP;
    bead->common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;
    bead->reg = reg;
}

extern void HWStartProc(void ) {
/** mark start of a routine **/
    bead_startproc *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_STARTPROC;
    AddBead( (bead_def *)bead, 0 );
}

extern void HWEpilogue(void ) {
/** mark start of a routine **/
    bead_startproc *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_EPILOGUE;
    AddBead( (bead_def *)bead, 0 );
}

extern void HWEndProc(void ) {
/** mark end  of a routine **/
    bead_endproc *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_ENDPROC;
    AddBead( (bead_def *)bead, 0 );
}

extern void HWQueue( int num ) {
/** mark a place in code linenum whatever **/
    bead_queue *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_QUEUE;
    bead->num = num;
    AddBead( (bead_def *)bead, 0 );
}

static bead_seg *MKSeg( char *str, seg_id id, char align ) {
/** mark a place in code where segment starts **/
    bead_seg *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_SEG;
    bead->common.address = 0;
    bead->common.next = NULL;
    bead->str = str;
    bead->id = id;
    bead->align = align;
    return( bead );
}

extern void HWInsGen( hwins_opcode opcode, hwins_op_any *hwop1,
                                    hwins_op_any *hwop2,
                                    hwins_op_any *hwop3 )
/*** Add an instruction to current segment***************/
  {
    any_bead_hwins *bead;
    hwins_class    hwclass;


    HWMKInsGen( CurrIndex->end_lnk, opcode, hwop1, hwop2, hwop3 );

    bead = (any_bead_hwins*)*CurrIndex->end_lnk;
    bead->ins.common.address = CurrIndex->location;
    CurrIndex->end_lnk = &bead->ins.common.next;
    hwclass = HWOpTable[opcode].class;
    CurrIndex->location += HWOpICL[hwclass];

}

extern void HWMKInsGen( bead_def **end_lnk, hwins_opcode opcode,
                                    hwins_op_any *hwop1,
                                    hwins_op_any *hwop2,
                                    hwins_op_any *hwop3 )
/*** insert instruction at end_lnk**************************/
  {
    any_bead_hwins *bead;
    hwins_class    hwclass;

    hwclass = HWOpTable[opcode].class;
    bead = CGAlloc( HwLength[hwclass] );
    bead->ins.common.class = BEAD_HWINS;
    bead->ins.common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;

    bead->ins.opcode = opcode;
    bead->ins.class = hwclass;
    switch( hwclass ) {
    case HWINS_RR:
        bead->rr.r1 = hwop1->r;
        bead->rr.r2 = hwop2->r;
        break;
    case HWINS_RX:
        bead->rx.r1 = hwop1->r;
        bead->rx.s2 = hwop2->sx;
        break;
    case HWINS_RS1:
        bead->rs1.r1 = hwop1->r;
        bead->rs1.r3 = hwop3->r;
        bead->rs1.s2 = hwop2->sx;
        break;
    case HWINS_RS2:
        bead->rs2.r1 = hwop1->r;
        bead->rs2.s2 = hwop2->sx;
        break;
    case HWINS_SI:
        bead->si.s1 = hwop1->sx;
        bead->si.i2 = hwop2->i;
        break;
    case HWINS_S:
        bead->s.s2 = hwop2->sx;
        break;
    case HWINS_SS1:
        bead->ss1.s1 = hwop1->sx;
        bead->ss1.s2 = hwop2->sx;
        break;
    case HWINS_SS2:
        bead->ss2.s1 = hwop1->sx;
        bead->ss2.s2 = hwop2->sx;
        break;
    case HWINS_SSP: /* SRP is oddball */
        bead->ssp.s1 = hwop1->sx;
        bead->ssp.s2 = hwop2->sx;
        break;
    }
}

extern void HWBRGen(  char cc, hwins_op_any *hwop2 ) {
/*** add branch statement to segement ***************/
    bead_def       *bead;
    hwins_class     hwclass;

    HWMKBRGen( CurrIndex->end_lnk, cc, hwop2 );
    bead = *CurrIndex->end_lnk;
    bead->address = CurrIndex->location;
    CurrIndex->end_lnk = &bead->next;
    hwclass = HWOpTable[HWOP_BC].class;
    CurrIndex->location += HWOpICL[hwclass];
}

extern void HWMKBRGen(  bead_def **end_lnk, char cc, hwins_op_any *hwop2 ) {
/*** insert  branch at end_lnk***********************************************/
    bead_hwins_rx *bead;
    hwins_class    hwclass;
    ref_any       *ref;

    hwclass = HWOpTable[HWOP_BC].class;
    bead = CGAlloc( HwLength[hwclass] );
    bead->common.class = BEAD_BR;
    bead->common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;

    bead->opcode = HWOP_BC;
    bead->class = hwclass;
    bead->r1 = cc;
    bead->s2 = hwop2->sx;
    ref = hwop2->sx.ref;
    if( ref != NULL && ref->entry.class == REF_SYM ) {  /* fill ref in with bead  */
        ref->sym.use = (bead_def *)bead;
    }
}

extern void HWBIndexGen(  char reg, hw_sym *table ) {
/*** Add branch through reg that has table addr *****/
/*** Get's expanded later into some intructions**/
    bead_bindex     *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_BINDEX;
    bead->size  = 2+4+4+4+4+4;
    /* AR, A,  AH, LH,  BC, =A(table)   */
    AddBead( (bead_def *)bead, bead->size  );

    bead->reg = reg;
    bead->br  = 0;
    bead->table = table;
    bead->lit = HWLitAddr( table, 0, FALSE );
}

extern void HWDataGen( int length, int rep, byte *value ) {
/****** do a DC value *********************/
    bead_data *bead;
    int bead_length;

    bead_length = sizeof( *bead ) +length-1;
    bead = CGAlloc( bead_length );
    bead->common.class = BEAD_DATA;
    AddBead( (bead_def *)bead,length*rep );
    bead->length = length;
    bead->rep   = rep;
    memcpy( bead->value, value, length );
}

extern void HWStoreGen( int length ) {
/****** do a DS value *********************/
    bead_store *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_STORE;
    AddBead( (bead_def *)bead, length );
    bead->length = length;
}

extern void HWIntGen( offset value, int size ) {
/****** do a DC on an int value *********/
    bead_int *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_INT;
    AddBead( (bead_def *)bead, size );
    bead->size = size;
    bead->value = value;
}

extern void HWFltGen( pointer value, int size ) {
/**** add a float to the current segement ******/
    bead_flt *bead;

    bead = MakeAFlt( value, size );
    AddBead( (bead_def *)bead, size );

}

static bead_flt *MakeAFlt( pointer value, int size ) {
/****** do a DC on an float value *********/
    bead_flt *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_FLT;
    bead->common.next = NULL;
    bead->common.address = 0;
    bead->size = size;
    bead->value = CFToF( value );
    return( bead );
}

static void HWMKAddrGen( bead_def **end_lnk, hw_sym *sym, offset val ) {
/*** Insert a DC A(sym+val) *************/
    bead_addr *bead;

    bead = MKSymAddr( sym, val, FALSE );
    bead->common.next = *end_lnk;
    *end_lnk = (bead_def*)bead;
}

extern void HWLTblGen( hw_sym *sym ) {
/****** start a label table for switch stmnt **********/
    HWLabelGen( sym, 2 );
}

extern void HWLblDisp( hw_sym *sym  ) {
/****** do a DC AL2(sym-base) for switch statement**/
    bead_disp *bead;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.class = BEAD_DISP;
    AddBead( (bead_def *)bead, 2 );
    bead->val = 0;
    bead->ref = sym;
    bead->base = NULL; /* don't know base yet */
    bead->op_len = DISP_SUB|2;
}

extern ref_any  *HWSymRef( hw_sym *sym ) {
/***************************************/
    ref_sym *ref;

    ref = CGAlloc( sizeof( *ref ) );
    ref->common.next = NULL;
    ref->common.class = REF_SYM;
    ref->sym = sym;
    ref->use = NULL;
    ref->common.next = sym->refs; /* link references */
    sym->refs = ref;
    return((ref_any *) ref );
}


extern ref_any  *HWLitIntGen( offset value, int size ) {
/****** do a DC value for a lit *********************/
    bead_int  *bead;
    ref_lit   *ref;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.next = NULL;
    bead->common.class = BEAD_INT;
    bead->common.address = 0;
    bead->size = size;
    bead->value = value;
    ref = CGAlloc( sizeof( *ref ) );
    ref->common.next = NULL;
    ref->common.class = REF_LIT;
    ref->val = (bead_def *)bead;
    ref->def = NULL;
    return( (ref_any *)ref );
}

extern ref_any  *HWLitFltGen( pointer value, int size ) {
/****** do a DC value for a lit *********************/
    bead_flt  *bead;
    ref_lit   *ref;

    bead = MakeAFlt( value, size );
    ref = CGAlloc( sizeof( *ref ) );
    ref->common.next = NULL;
    ref->common.class = REF_LIT;
    ref->val = (bead_def *)bead;
    ref->def = NULL;
    return( (ref_any *)ref );
}

extern ref_any  *HWDispRef( hw_sym *sym, hw_sym *base ) {
/***************************************/
    ref_disp  *ref;

    ref = CGAlloc( sizeof( *ref ) );
    ref->common.next = NULL;
    ref->common.class = REF_DISP;
    ref->sym = sym;
    ref->base = base;
    ref->common.next = NULL;
    return((ref_any *) ref );
}

extern ref_any  *HWLitAddr( hw_sym *sym, offset val, bool reloc ){
/****** do a DC A(sym+val-rel) for a lit ********/
/***** used to ref a rent symbol in code     ********/
    bead_addr *bead;
    ref_lit   *ref;

    bead = MKSymAddr( sym, val, reloc );
    ref = CGAlloc( sizeof( *ref ) );
    ref->common.next = NULL;
    ref->common.class = REF_LIT;
    ref->val = (bead_def *)bead;
    ref->def = NULL;
    return( (ref_any *)ref );
}

extern bead_addr *HWSymAddr( hw_sym *sym, offset val, bool reloc ){
/****** do a DC A(sym+val-rel) if relocation needed       ********/
    bead_addr *bead;

    bead = MKSymAddr( sym, val, reloc );
    AddBead( (bead_def *)bead, 4  );
    return( bead );
}

static bead_addr *MKSymAddr( hw_sym *sym, offset val, bool reloc ){
/****** make a  DC A(sym+val-rel) if relocation needed    ********/
    bead_addr *bead;
    hw_sym    *rel;

    bead = CGAlloc( sizeof( *bead ) );
    bead->common.next = NULL;
    bead->common.class = BEAD_ADDR;
    bead->common.address = 0;
    bead->val = val;
    bead->ref = sym;
    if( reloc ){
        rel = HWSymAdd( &HWSyms, "$CRWDAT" );
        HWExtern( rel );
    }else{
        rel = NULL;
    }
    bead->rel = rel;
    return( bead );
}

extern offset HWCalcAddr( bead_def *bead, offset address ) {
/***Calc the offsets of the beads**************************/
    offset align;

    while( bead != NULL ) {
        switch( bead->class ) {
        case BEAD_HWINS :
            bead->address = address;
            address += HWOpICL[ ((any_bead_hwins*)bead)->ins.class ];
            break;
        case BEAD_LABEL :
            align = ((bead_label *)bead)->align;
            address += align-1;
            address &= (-align);
            bead->address = address;
            break;
        case BEAD_BR    :
            bead->address = address;
            address += 4;
            break;
        case BEAD_BINDEX:
            bead->address = address;
            address += ((bead_bindex*)bead)->size;
            break;
        case BEAD_DATA  :
            bead->address = address;
            address += ( (bead_data*)bead)->length*( (bead_data*)bead)->rep;
            break;
        case BEAD_ADDR  :
            bead->address = address;
            address += 4;
            break;
        case BEAD_INT :
            bead->address = address;
            address += ((bead_int*)bead)->size;
            break;
        case BEAD_FLT :
            bead->address = address;
            address += ((bead_flt*)bead)->size;
            break;
        case BEAD_DISP:
            bead->address = address;
            address += ((bead_disp*)bead)->op_len & DISP_LEN;
            break;
        case BEAD_XSYM  :
            bead->address = address;
            break;
        case BEAD_USING :
            bead->address = address;
            break;
        case BEAD_DROP  :
            bead->address = address;
            break;
        case BEAD_LTORG :
            align = ((bead_ltorg *)bead)->align;
            address += align-1;
            address &= (-align);
            bead->address = address;
            break;
        case BEAD_STARTPROC:
            bead->address = address;
            break;
        case BEAD_EPILOGUE:
            bead->address = address;
            break;
        case BEAD_ENDPROC:
            bead->address = address;
            break;
        case BEAD_QUEUE:
            bead->address = address;
            break;
        case BEAD_SEG:
            align = ((bead_seg *)bead)->align;
            address += align-1;
            address &= (-align);
            bead->address = address;
            break;
        case BEAD_STORE :
            bead->address = address;
            address += ( (bead_store*)bead)->length;
            break;
        default :
            printf( "Unknown bead(calc) \n" );
        }
        bead = bead->next;
    }
    return( address );
}

extern int  HWLitAlign(  bead_def *val ) {
/******* return  log 2 implied alignment of bead ***/
    int align;

    switch( val->class ) {
    case BEAD_ADDR:
        align = 4;
        break;
    case BEAD_INT:
        align = ((bead_int *)val)->size;
        break;
    case BEAD_FLT:
        align =((bead_flt *)val)->size;
        break;
    default:
        align = 1;
        break;
    }
    if( align == 8 ){
        align = 3;
    }else{
        align >>= 1;
    }
    return( align );
}

static void Remove( bead_def **lnk ){
/*****************************************************
 Remove bead at lnk
*/
    bead_def *dead;
    dead = *lnk;
    *lnk = dead->next;
    free( dead );
}

static bead_startproc  *LinkProcs( bead_def *list ){
/***************************************
 Link startproc with endproc and  last_proc,
 this probably could be done in s37proc
*/
    bead_startproc  *curr, *last;

    last = NULL;
    while( (curr = (bead_startproc *)SkipToProc( list )) != NULL ){
        list = curr->common.next;
        while( list->class !=  BEAD_ENDPROC ){
            list = list->next;
        }
        curr->endproc = (bead_endproc *)list;
        curr->prev = last;
        last = curr;

    }
    return( last );
}

static  bead_def *SkipQueue( bead_def *bead ){
/*** skip queues ****************************/
    while( (bead = bead->next)->class == BEAD_QUEUE );
    return( bead );
}

static  bead_def *SkipToProc( bead_def *bead ){
/*** skip queues ****************************/
    while( bead != NULL && bead->class != BEAD_STARTPROC ){
        bead = bead->next;

    }
    return( bead );
}

extern offset PeepS370( bead_startproc *list ) {
/***Go for some lame optimizations   ****/
    offset change;
    bead_def *bead, *endproc, **lnk;

    change = 0;
    while(  list != NULL ){
        lnk = &list->common.next;
        endproc = (bead_def *)list->endproc;
        while( (bead = *lnk) != endproc ){
            if( bead->class == BEAD_HWINS ) {
                if( ((any_bead_hwins*)bead)->ins.class == HWINS_SS1 ){
                    any_bead_hwins  *next;

                    next = (any_bead_hwins*)SkipQueue( bead );
                    if( next->ins.common.class == BEAD_HWINS
                     && next->ins.opcode == ((any_bead_hwins*)bead)->ins.opcode ){
                        if( TryCoalesce( lnk, next ) ){
                            change += 6;
                            continue;
                        }
                    }
                }
            }
            lnk = &bead->next;
        }
        list = list->prev;
    }
    return( change );
}

static int TryCoalesce( bead_def **lnk,  bead_hwins_ss1 *next ){
/************************************************
       a OP disp( l1, r1 ),disp(r2)
       b OP disp+l1(l2,r1),disp+l1(r2)
    ==>a OP disp(l1+l2,r1),disp(r2)   l1+l2 <= 256
*/
    bead_hwins_ss1 *first;
    int l1, l2;

    first = (bead_hwins_ss1 *)*lnk;
    l1 = first->s1.a;
    l2 = next->s1.a;
    if( AdjacentSX( &first->s1, &next->s1, l1  )
      && AdjacentSX( &first->s2, &next->s2, l1  )
      && l1+l2 <= 256 ){
      next->s1.disp = first->s1.disp;
      next->s1.a = l1+l2;
      next->s2.disp = first->s2.disp;
      Remove( lnk );
      return( TRUE );
    }else{
      return( FALSE );
    }
}

static int AdjacentSX( hwins_op_sx  *s1, hwins_op_sx  *s2, int l ){
/*** see if s1 is right beside s2 ***/
    if( s1->ref != s2->ref ){
        return( FALSE );
    }
    if( s1->b  != s2->b ){
        return( FALSE );
    }
    if( s1->disp+l != s2->disp ){
        return( FALSE );
    }
    return( TRUE );
}

static void FreeRefs( any_bead_hwins *bead ) {
/*********************************************
    free any ref beads note lits are a problem cause they may follow
    an ltorg
*/
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
    if( op1 != NULL && op1->ref != NULL ){
      // if( op1->ref->entry.class == REF_LIT ){
      //     CGFree( op1->ref->lit.val );
      // }
        CGFree( op1->ref );
    }
    if( op2 != NULL && op2->ref != NULL ){
     //  if( op2->ref->entry.class == REF_LIT ){
     //      CGFree( op2->ref->lit.val );
     //  }
        CGFree( op2->ref );
    }
}

static void FreeBeads( bead_def *bead  ){
/*********************************
    Free all those beads
*/
    bead_def *next;

    while( bead != NULL ){
        if( bead->class == BEAD_HWINS ){
            FreeRefs( (any_bead_hwins *)bead );
        }
        next = bead->next;
        CGFree( bead );
        bead = next;
    }
}

#if 1
extern   DumpBeads( bead_def *bead ) {
/***Calc the offsets of the beads**************************/
    offset align;
    char  *name;
    offset size;

    while( bead != NULL ) {
        size = 0;
        switch( bead->class ) {
        case BEAD_HWINS :
            name = "BEAD_HWINS";
            size = HWOpICL[ ((any_bead_hwins*)bead)->ins.class ];
            break;
        case BEAD_LABEL :
            name = "BEAD_LABEL";
            align = ((bead_label *)bead)->align;
            break;
        case BEAD_BR    :
            name = "BEAD_BR";
            size = 4;
            break;
        case BEAD_BINDEX:
            name = "BEAD_BINDEX";
            size = ((bead_bindex*)bead)->size;
            break;
        case BEAD_DATA  :
            name = "BEAD_DATA";
            size = ( (bead_data*)bead)->length*( (bead_data*)bead)->rep;
            break;
        case BEAD_ADDR  :
            name = "BEAD_ADDR";
            size = 4;
            break;
        case BEAD_INT :
            name = "BEAD_INT";
            size = ((bead_int*)bead)->size;
            break;
        case BEAD_FLT :
            name = "BEAD_FLT";
            size = ((bead_flt*)bead)->size;
            break;
        case BEAD_DISP:
            name = "BEAD_DISP";
            size = ((bead_disp*)bead)->op_len & DISP_LEN;
            break;
        case BEAD_XSYM  :
            name = "BEAD_XSYM";
            break;
        case BEAD_USING :
            name = "BEAD_USING";
            break;
        case BEAD_DROP  :
            name = "BEAD_DROP";
            break;
        case BEAD_LTORG :
            name = "BEAD_LTORG";
            align = ((bead_ltorg *)bead)->align;
            break;
        case BEAD_STARTPROC:
            name = "BEAD_STARTPROC";
            break;
        case BEAD_EPILOGUE:
            name = "BEAD_EPILOGUE";
            break;
        case BEAD_ENDPROC:
            name = "BEAD_ENDPROC";
            break;
        case BEAD_QUEUE:
            name = "BEAD_QUEUE";
            break;
        case BEAD_SEG:
            name = "BEAD_SEG";
            align = ((bead_seg *)bead)->align;
            break;
        case BEAD_STORE :
            name = "BEAD_STORE";
            size = ( (bead_store*)bead)->length;
            break;
        default :
            printf( "Unknown bead(calc) \n" );
        }
        printf( "%08X: %08X %s %d\n", bead,  bead->address, name, size );
        bead = bead->next;
    }
}
#endif
