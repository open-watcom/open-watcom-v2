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


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "cgmem.h"
#include "addrname.h"
#include "tree.h"
#include "offset.h"
#include "seldef.h"
#include "makeins.h"
#include "s37sel.def"

extern  void            HWIntGen(offset,int);
extern  void            HWLTblGen(void *);
extern  void            HWLblDisp(void *);
extern  char           *AskLblRefName(label_handle);
extern  name           *GenIns(an);
extern  name           *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name           *AllocIndex(name*,name*,type_length,type_class_def);
extern  name           *AllocTemp(type_class_def);
extern  cg_name         CGInteger(signed_32,cg_type);
extern  cg_type         SelType(unsigned_32);
extern  cg_name         CGBinary(cg_op,cg_name,cg_name,cg_type);
extern  an              TreeGen(tn);
extern  an              BGDuplicate(an);
extern  label_handle    AskForNewLabel(void);
extern  void           *AskForHWLabel( label_handle lbl );
extern  type_def       *TypeAddress(cg_type);
extern  signed_32       NumValues(select_list*,signed_32);
extern  void            BGDone(an);
extern  tn              TGLeaf(an);
extern  void            CodeLabel(label_handle,unsigned);
extern  unsigned        DepthAlign(unsigned);
extern  seg_id          AskBackSeg(void);
extern  seg_id          SetOP(seg_id);
extern  type_def        *TypeAddress(cg_type);
extern  name            *LoadTemp(name*,type_class_def);
extern  int             SelCompare(signed_32,signed_32);
extern  void            AddIns(instruction*);

extern  char            OptForSize;

#define MAX_JUMPS       1000
#define MIN_JUMPS       4            /* to make it worth while for jum*/
#define MAX_VALUES      1000

#define MAX_COST        0x7FFF

/*
   There are some magic numbers in this file in the calls to Balance(). they
   were found by looking at code and counting
   bytes, and cycles (RX = 2, Branch = 2, RR = 1)
*/

static  void    TableStart( label_handle lbl ) {
/**********************************************/

    HWLTblGen( AskForHWLabel( lbl ) );
}


static  void    TableEntry( label_handle lbl ) {
/**********************************************/

    HWLblDisp( AskForHWLabel( lbl ) );
}


static  int     Balance( signed_32 size, signed_32 time ) {
/*********************************************************/

    signed_32   balance;
    byte        opt_size;

    opt_size = OptForSize;
    if( opt_size < 25 ) {
        opt_size = 25;
    }
    balance = ( size * opt_size + time * ( 100 - opt_size ) ) / 100;
    if( balance > MAX_COST ) {
        balance = MAX_COST;
    }
    return( balance );
}


extern  int     ScanCost( select_node *s_node ) {
/***********************************************/

    select_list *list;
    signed_32   hi;
    signed_32   lo;
    signed_32   values;
    int         cost;
    int         type_length;
    cg_type     tipe;

    list = s_node->list;
    hi = s_node->upper;
    lo = s_node->lower;
    values = 0;
    while( list != NULL ) {
        if( SelCompare( list->low, hi ) > 0 ) break;
        values += list->high - list->low + 1;
        list = list->next;
    }
    tipe = SelType( hi - lo );
    if( values > MAX_VALUES ) {
        cost = MAX_COST;
    } else {
        type_length = TypeAddress( tipe )->length;
        cost = Balance( 38 + values * ( WORD_SIZE + type_length ),
                        27 + 8 * ( values / 2 ) );
    }
    return( cost );
}


extern  int     JumpCost( select_node *s_node ) {
/***********************************************/

    unsigned_32 in_range;
    int cost;

    in_range = s_node->upper - s_node->lower + 1;
    if( s_node->num_cases < MIN_JUMPS ) {
        cost = MAX_COST;
//  } else if( in_range > MAX_JUMPS || in_range < MIN_JUMPS ) {
    } else if(  in_range < MIN_JUMPS ) {
        cost = MAX_COST;
    } else {
        cost = Balance( 30 + 2 * in_range, 17 );
    }
    return( cost );
}


extern  int     IfCost( select_node *s_node, int entries ) {
/**********************************************************/

    signed_32   hi;
    signed_32   lo;
    signed_32   cost;
    signed_32   size;
    int         tipe_length;
    int         log_entries;

    hi = s_node->upper;
    lo = s_node->lower;
    tipe_length = TypeAddress( SelType( hi - lo ) )->length;
    size = tipe_length == 1 ? 8*entries : 12*entries;
    log_entries = 0;
    while( entries != 0 ) {
        log_entries++;
        entries = (unsigned_32)entries >> 2;
    }
    cost = Balance( size, 5*log_entries );
    if( cost >= MAX_COST ) {
        cost = MAX_COST - 1;
    }
    return( cost );
}


extern  tbl_control     *MakeScanTab( select_list *list, signed_32 hi,
                                      label_handle other, cg_type tipe,
                                      cg_type real_tipe ) {
/*****************************************************************/

    tbl_control         *table;
    label_handle        *tab_ptr;
    unsigned_32         cases;
    signed_32           lo;
    signed_32           to_sub;
    seg_id              old;
    select_list         *scan;
    signed_32           curr;

    cases = NumValues( list, hi );
    lo = list->low;
    table = CGAlloc( sizeof( tbl_control ) + (cases-1) * sizeof( label_handle ) );
    table->size = cases;
    old = SetOP( AskBackSeg() );
    table->value_lbl = AskForNewLabel();
    CodeLabel( table->value_lbl, DepthAlign( 1 ) );
    table->lbl = AskForNewLabel();
    if( tipe != real_tipe ) {
        to_sub = lo;
    } else {
        to_sub = 0;
    }
    if( other == NULL ) {
        other = table->cases[ 0 ];  /* no otherwise? he bakes!*/
    }
    GenValuesForward( list, hi, lo, to_sub, TypeAddress( tipe ) );
    TableStart( table->lbl );
    tab_ptr = &table->cases[ 0 ];
    curr = lo;
    scan = list;
    for(;;) {
        *tab_ptr = scan->label;
        TableEntry( *tab_ptr );
        ++tab_ptr;
        if( SelCompare( curr, hi ) >= 0 ) break;
        if( SelCompare( curr, scan->high ) >= 0 ) {
            scan = scan->next;
            curr = scan->low;
        } else {
            ++curr;
        }
    }
    TableEntry( other );
    SetOP( old );
    return( table );
}


static  void    GenValuesForward( select_list *list, signed_32 hi,
                                  signed_32 lo, signed_32 to_sub,
                                  type_def *tipe ) {
/****************************************************************/

    signed_32           curr;

    curr = lo;
    for(;;) {
        HWIntGen( curr - to_sub, tipe->length );
        if( SelCompare( curr, hi ) >= 0 ) break;
        if( SelCompare( curr, list->high ) >= 0 ) {
            list = list->next;
            curr = list->low;
        }else{
            curr++;
        }
    }
}


extern  tbl_control     *MakeJmpTab( select_list *list, signed_32 lo,
                                     signed_32 hi, label_handle other ) {
/*****************************************************************/

    tbl_control         *table;
    label_handle        *tab_ptr;
    unsigned_32         cases;
    seg_id              old;

    cases = hi - lo + 1;
    table = CGAlloc( sizeof( tbl_control ) + (cases-1) * sizeof( label_handle ) );
    old = SetOP( AskBackSeg() );
    table->lbl = AskForNewLabel();
    table->value_lbl = NULL;
    TableStart( table->lbl );
    table->size = cases;
    tab_ptr = &table->cases[ 0 ];
    for(;;) {
        if( SelCompare( lo, list->low ) < 0 ) {
            *tab_ptr = other;
        } else {
            *tab_ptr = list->label;
        }
        TableEntry( *tab_ptr );
        ++tab_ptr;
        if( SelCompare( lo, hi ) >= 0 ) break;
        if( SelCompare( lo, list->high ) >= 0 ) {
            list = list->next;
        }
        ++lo;
    }
    SetOP( old );
    return( table );
}


extern  name_def        *SelIdx( tbl_control *table, an node ) {
/**************************************************************/

    an          idxan;
    name       *idx;
    name       *temp;
    instruction *ins;

    /* use CG routines here to get folding*/
    idxan = BGDuplicate( node );
    idx = GenIns( idxan );
    temp = AllocTemp( WD );
    ins = MakeConvert( idx, temp, WD, idx->n.name_class );
    AddIns( ins );
    BGDone( idxan );
    idx = AllocIndex( temp, AllocMemory( table, 0, CG_TBL, WD ), 0, WD );
    return( idx );
}

extern  type_def        *SelNodeType( an node, bool is_signed ) {
/************************************************************/

    cg_type     unsigned_t;
    cg_type     signed_t;

    switch( node->tipe->length ) {
    case 1:
        unsigned_t = TY_UINT_1;
        signed_t = TY_INT_1;
        break;
    case 2: /* no support in switch */
    case 4:
        unsigned_t = TY_UINT_4;
        signed_t = TY_INT_4;
        break;
    default: /* an error */
        unsigned_t = NULL;
        signed_t =  NULL;
        break;
    }
    return( TypeAddress( is_signed ? signed_t : unsigned_t ) );
}

extern  void    MkSelOp( name *idx, cg_type tipe ) {
/**************************************************/

    instruction         *ins;

    ins = NewIns( 2 );
    ins->head.opcode = OP_ADD;
    ins->type_class = WD;
    ins->operands[ 0 ] = idx->i.index;
    ins->operands[ 1 ] = idx->i.index;
    ins->result = idx->i.index;
    AddIns( ins );
    ins = NewIns( 1 );
    ins->operands[ 0 ] = idx;
    ins->result = idx->i.index;
    ins->head.opcode = OP_SELECT;
    ins->type_class = tipe;
    ins->ins_flags |= INS_CC_USED;
    AddIns( ins );
}
