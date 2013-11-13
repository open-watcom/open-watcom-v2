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
* Description:  Determine the cost of switch/case -style statement code, and
*               generate the scan and jump tables.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "cgmem.h"
#include "tree.h"
#include "seldef.h"
#include "cgswitch.h"
#include "data.h"
#include "types.h"
#include "makeins.h"
#include "objout.h"
#include "treeprot.h"
#include "makeaddr.h"
#include "namelist.h"
#include "cgprotos.h"

#define MIN_JUMPS       4            /* to make it worth while for jum*/
#define MIN_LVALUES     5            /* to make it worth while for long sca*/
#define MIN_SVALUES     7            /* to make it worth while for short sca*/
#define MIN_JUMPS_SETUP 12           /* minimal size of jum code */
/* for instance:
0010    83 F8 03                  cmp       eax,0x00000003
0013    77 0D                     ja        L$3
0015    FF 24 85 00 00 00 00      jmp       dword ptr L$1[eax*4]
*/
#define MIN_SCAN_SETUP  12           /* minimal size of sca code */
/* for instance:
0022    B9 08 00                  mov       cx,0x0008
0025    BF 00 00                  mov       di,offset L$1
0028    F2 AF                     repne scasw 
002A    26 FF 65 0C               jmp       word ptr es:0xc[di]
*/

#define MAX_COST        0x7FFFFFFFL
#define MAX_IN_RANGE    (MAX_COST/1000) /* so no overflow */

extern  cg_type         SelType(unsigned_32);
extern  an              BGDuplicate(an);
extern  void            Gen4ByteValue(unsigned_32);
extern  an              TreeGen(tn);
extern  void            CodeLabel(label_handle, unsigned );
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  void            GenCodePtr(pointer);
extern  void            GenSelEntry(bool);
extern  void            Gen2ByteValue(unsigned_16);
extern  void            BGDone(an);
extern  void            Gen1ByteValue(byte);
extern  signed_32       NumValues(select_list*,signed_32);
extern  int             SelCompare(signed_32,signed_32);
extern  void            AddIns(instruction*);
extern  an              BGInteger( signed_32, type_def * );

/* forward declarations */
static  void    GenValuesBackward( select_list *list, signed_32 hi,
                                   signed_32 lo, signed_32 to_sub,
                                   cg_type tipe );
static  void    GenValuesForward( select_list *list, signed_32 hi,
                                  signed_32 lo, signed_32 to_sub,
                                  cg_type tipe );


static  signed_32 Balance( signed_32 size, signed_32 time ) {
/*********************************************************/

    signed_32   balance;
    byte        opt_size;
    signed_32   cost_size;
    signed_32   cost_time;

    opt_size = OptForSize;
    if( opt_size < 25 ) {
        opt_size = 25;
    }
    cost_size = size * opt_size;
    if( cost_size < 0 ) return( MAX_COST );     // overflow
    cost_time = 10 * time * ( 100 - opt_size );
    if( cost_time < 0 ) return( MAX_COST );     // overflow
    // balance = ( size * opt_size + 10 * time * ( 100 - opt_size ) ) / 100;
    balance = ( cost_time + cost_size ) / 100;
    return( balance );
}


extern  signed_32       ScanCost( select_node *s_node ) {
/*******************************************************/

    select_list *list;
    signed_32   hi;
    signed_32   lo;
    signed_32   values;
    signed_32   cost;
    int         type_length;
    cg_type     tipe;

    hi = s_node->upper;
    lo = s_node->lower;
    values = 0;
    for( list = s_node->list; list != NULL; list = list->next ) {
        if( SelCompare( list->low, hi ) > 0 )
            break;
        values += list->high - list->low + 1;
    }
    tipe = SelType( hi - lo );
    if( ( tipe == TY_UINT_4 && values < MIN_LVALUES ) || ( tipe != TY_UINT_4 && values < MIN_SVALUES ) ) {
        cost = MAX_COST;
    } else {
        type_length = TypeAddress( tipe )->length;
        cost = Balance( MIN_SCAN_SETUP + values * ( WORD_SIZE + type_length ), values / 2 );
    }
    return( cost );
}


extern  signed_32       JumpCost( select_node *s_node ) {
/*******************************************************/

    signed_32   in_range;
    signed_32   cost;

    in_range = s_node->upper - s_node->lower + 1;
    if( in_range > MAX_IN_RANGE || in_range < 0 ) {
        in_range = MAX_IN_RANGE;
    }
    if( s_node->num_cases < MIN_JUMPS ) {
        cost = MAX_COST;
    } else if( in_range < MIN_JUMPS ) {
        cost = MAX_COST;
    } else {
        cost = MIN_JUMPS_SETUP + WORD_SIZE * in_range;
        /* an extra two bytes are needed to zero the high part before
           the jump */
        if ( SelType( 0xffffffff ) == TY_UINT_1 )
            cost += 2;
        cost = Balance( cost, 1 );
    }
    return( cost );
}


#if _TARGET == _TARG_80386
    #define LONG_JUMP 6
    #define SHORT_JUMP 2
    static byte CmpSize[] = { 0, 2, 4, 0, 5 };
#elif _TARGET == _TARG_IAPX86
    #define LONG_JUMP 5
    #define SHORT_JUMP 2
    static byte CmpSize[] = { 0, 2, 3, 0, 9 };
#endif

extern  signed_32       IfCost( select_node *s_node, int entries ) {
/******************************************************************/

    signed_32   hi;
    signed_32   lo;
    signed_32   cost;
    signed_32   jumpsize;
    int         log_entries;
    int         tipe_length;

    hi = s_node->upper;
    lo = s_node->lower;
    tipe_length = TypeAddress( SelType( hi - lo ) )->length;
    if( entries > 20 ) {
        jumpsize = LONG_JUMP;
    } else {
        jumpsize = SHORT_JUMP;
    }
    cost = jumpsize + CmpSize[ tipe_length ];
    /* for char-sized switches, often the two-byte "cmp al,xx" is used.
       otherwise we need three bytes */
    if ( SelType( 0xffffffff ) != TY_UINT_1 && tipe_length == 1 )
        cost++;
    cost *= entries;
    log_entries = 0;
    while( entries != 0 ) {
        log_entries++;
        entries = (unsigned_32)entries >> 2;
    }
    /* add cost for extra jumps generated for grandparents and 
       every other child except the last one */
    cost += (entries / 4) * 2 * jumpsize;
    cost = Balance( cost, log_entries );
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
    segment_id          old;
    select_list         *scan;
    signed_32           curr;

    cases = NumValues( list, hi );
    lo = list->low;
    table = CGAlloc( sizeof( tbl_control ) + (cases-1) * sizeof( label_handle ) );
    table->size = cases;
    old = SetOP( AskCodeSeg() );
    table->value_lbl = AskForNewLabel();
    CodeLabel( table->value_lbl, TypeAddress( TY_NEAR_CODE_PTR )->length );
    GenSelEntry( TRUE );
    table->lbl = AskForNewLabel();
    if( tipe != real_tipe ) {
        to_sub = lo;
    } else {
        to_sub = 0;
    }
    if( other == NULL ) {
        other = table->cases[ 0 ];  /* no otherwise? he bakes!*/
    }
    if( tipe == TY_WORD ) {
        GenValuesForward( list, hi, lo, to_sub, tipe );
    } else {
        GenValuesBackward( list, hi, lo, to_sub, tipe );
    }
    GenSelEntry( FALSE );
    CodeLabel( table->lbl, 0 );
    tab_ptr = &table->cases[ 0 ];
    curr = lo;
    scan = list;
    if( tipe != TY_WORD ) {
        GenCodePtr( other );
    }
    for(;;) {
        *tab_ptr = scan->label;
        GenCodePtr( *tab_ptr );
        ++tab_ptr;
        if( SelCompare( curr, hi ) >= 0 ) break;
        if( SelCompare( curr, scan->high ) >= 0 ) {
            scan = scan->next;
            curr = scan->low;
        } else {
            ++curr;
        }
    }
    if( tipe == TY_WORD ) {
        GenCodePtr( other );
    }
    SetOP( old );
    return( table );
}


static  void    GenValuesForward( select_list *list, signed_32 hi,
                                  signed_32 lo, signed_32 to_sub,
                                  cg_type tipe ) {
/****************************************************************/

    signed_32           curr;

    curr = lo;
    for(;;) {
        switch( tipe ) {
        case TY_UINT_1:
            Gen1ByteValue( curr - to_sub );
            break;
        case TY_UINT_2:
            Gen2ByteValue( curr - to_sub );
            break;
        case TY_UINT_4:
            Gen4ByteValue( curr - to_sub );
            break;
        }
        if( SelCompare( curr, hi ) >= 0 ) break;
        if( SelCompare( curr, list->high ) >= 0 ) {
            list = list->next;
            curr = list->low;
        } else {
            ++curr;
        }
    }
}


static  void    GenValuesBackward( select_list *list, signed_32 hi,
                                   signed_32 lo, signed_32 to_sub,
                                   cg_type tipe ) {

    select_list         *scan;
    select_list         *next;
    signed_32           curr;

    curr = hi;
    for( scan = list; scan->high != hi; ) {
        scan = scan->next;
    }
    for(;;) {
        switch( tipe ) {
        case TY_UINT_1:
            Gen1ByteValue( curr - to_sub );
            break;
        case TY_UINT_2:
            Gen2ByteValue( curr - to_sub );
            break;
        case TY_UINT_4:
            Gen4ByteValue( curr - to_sub );
            break;
        }
        if( SelCompare( curr, lo ) <= 0 ) break;
        if( SelCompare( curr, scan->low ) <= 0 ) {
            next = scan;
            for( scan = list; scan->next != next; ) {
                scan = scan->next;
            }
            curr = scan->high;
        } else {
            --curr;
        }
    }
}

extern  tbl_control     *MakeJmpTab( select_list *list, signed_32 lo,
                                     signed_32 hi, label_handle other ) {
/*****************************************************************/

    tbl_control         *table;
    label_handle        *tab_ptr;
    unsigned_32         cases;
    segment_id          old;

    cases = hi - lo + 1;
    table = CGAlloc( sizeof( tbl_control ) + (cases-1) * sizeof( label_handle ) );
    old = SetOP( AskCodeSeg() );
    table->lbl = AskForNewLabel();
    table->value_lbl = NULL;
    CodeLabel( table->lbl, TypeAddress( TY_NEAR_CODE_PTR )->length );
    table->size = cases;
    tab_ptr = &table->cases[ 0 ];
    for(;;) {
        if( SelCompare( lo, list->low ) < 0 ) {
            *tab_ptr = other;
        } else {
            *tab_ptr = list->label;
        }
        GenCodePtr( *tab_ptr );
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


extern  name        *SelIdx( tbl_control *table, an node ) {
/**********************************************************/

    an          idxan;
    name        *idx;

    /* use CG routines here to get folding*/
    idxan = TreeGen( TGBinary( O_TIMES, TGLeaf( BGDuplicate( node ) ),
                                TGLeaf( BGInteger( WORD_SIZE, TypeAddress( TY_WORD ) ) ), TypeAddress( TY_WORD ) ) );
    idx = GenIns( idxan );
    BGDone( idxan );
    return( AllocIndex( idx, AllocMemory( table, 0, CG_TBL, WD ), 0, WD ) );
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
    case 4:
        unsigned_t = TY_UINT_4;
        signed_t = TY_INT_4;
        break;
    case 8:
        unsigned_t = TY_UINT_8;
        signed_t = TY_INT_8;
        break;
    default:
        unsigned_t = TY_UINT_2;
        signed_t = TY_INT_2;
        break;
    }
    return( TypeAddress( is_signed ? signed_t : unsigned_t ) );
}


extern  void    MkSelOp( name *idx, type_class_def class )
/********************************************************/
{
    instruction         *ins;

    ins = NewIns( 1 );
    ins->operands[ 0 ] = idx;
    ins->head.opcode = OP_SELECT;
    ins->type_class = class;
    ins->result = NULL;
    AddIns( ins );
}
