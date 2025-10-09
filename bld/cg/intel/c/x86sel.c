/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
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
#include "bldsel.h"
#include "bldselco.h"
#include "cgaux.h"
#include "x86enc2.h"
#include "encode.h"
#include "makeblk.h"
#include "bldins.h"
#include "i64.h"


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

#define MAX_IN_RANGE    (MAX_COST/1000) /* so no overflow */

#if _TARGET & _TARG_8086
    #define LONG_JUMP 5
    #define SHORT_JUMP 2
    static const byte CmpSize[] = { 0, 2, 3, 0, 9 };
#elif _TARGET & _TARG_80386
    #define LONG_JUMP 6
    #define SHORT_JUMP 2
    static const byte CmpSize[] = { 0, 2, 4, 0, 5 };
#endif


static cost_val Balance( uint_32 size, uint_32 time )
/***************************************************/
{
    uint_32     balance;
    byte        opt_size;

    opt_size = OptForSize;
    if( opt_size < 25 ) {
        opt_size = 25;
    }
    balance = ( size * opt_size + 10 * time * ( 100 - opt_size ) ) / 100;
    if( (int_32)balance < 0 || balance > MAX_COST )
        return( MAX_COST );
    return( balance );
}


cost_val ScanCost( sel_handle s_node )
/************************************/
{
    const select_list   *list;
    uint_32             values;
    cost_val            cost;
    uint_32             type_length;
    cg_type             type;
    unsigned_64         tmp;

    values = 0;
    for( list = s_node->list; list != NULL; list = list->next ) {
        if( SelCompare( &list->low, &s_node->upper ) > 0 )
            break;
        values += list->count;
    }
    U64Sub( &s_node->upper, &s_node->lower, &tmp );
    type = SelType( &tmp );
    if( ( type == TY_UINT_4
      && values < MIN_LVALUES )
      || ( type != TY_UINT_4
      && values < MIN_SVALUES ) ) {
        cost = MAX_COST;
    } else {
        type_length = TypeAddress( type )->length;
        cost = Balance( MIN_SCAN_SETUP + values * ( WORD_SIZE + type_length ), values / 2 );
    }
    return( cost );
}


cost_val JumpCost( sel_handle s_node )
/************************************/
{
    uint_32         in_range;
    cost_val        cost;
    unsigned_64     tmp;

    U64Sub( &s_node->upper, &s_node->lower, &tmp );
    U64IncDec( &tmp, 1 );
    in_range = tmp.u._32[I64LO32];
    if( in_range > MAX_IN_RANGE
      || (int_32)in_range < 0 ) {
        in_range = MAX_IN_RANGE;
    }
    if( s_node->num_cases < MIN_JUMPS ) {
        cost = MAX_COST;
    } else if( in_range < MIN_JUMPS ) {
        cost = MAX_COST;
    } else {
        uint_32 size;

        size = MIN_JUMPS_SETUP + WORD_SIZE * in_range;
        /*
         * an extra two bytes are needed to zero the high part before
         * the jump
         */
        U64Set1M( tmp );
        if( SelType( &tmp ) == TY_UINT_1 )
            size += 2;
        cost = Balance( size, 1 );
    }
    return( cost );
}


cost_val IfCost( sel_handle s_node, uint_32 entries )
/***************************************************/
{
    int             type_length;
    uint_32         log_entries;
    uint_32         jumpsize;
    uint_32         size;
    unsigned_64     tmp;

    U64Sub( &s_node->upper, &s_node->lower, &tmp );
    type_length = TypeAddress( SelType( &tmp ) )->length;
    if( entries > 20 ) {
        jumpsize = LONG_JUMP;
    } else {
        jumpsize = SHORT_JUMP;
    }
    size = jumpsize + CmpSize[type_length];
    /*
     * for char-sized switches, often the two-byte "cmp al,xx" is used.
     * otherwise we need three bytes
     */
    U64Set1M( tmp );
    if( SelType( &tmp ) != TY_UINT_1
      && type_length == 1 ) {
        size++;
    }
    size *= entries;
    log_entries = 0;
    while( entries != 0 ) {
        log_entries++;
        entries >>= 2;
    }
    /*
     * add cost for extra jumps generated for grandparents and
     * every other child except the last one
     */
    size += ( entries / 4 ) * 2 * jumpsize;
    return( Balance( size, log_entries ) );
}

static void GenValuesForward( const select_list *list, const signed_64 *hi,
                const signed_64 *lo, const signed_64 *to_sub, cg_type type )
/**************************************************************************/
{
    signed_64       curr;
    unsigned_64     tmp;

    curr = *lo;
    for( ;; ) {
        U64Sub( &curr, to_sub, &tmp );
        switch( type ) {
        case TY_UINT_1:
            Gen1ByteValue( tmp.u._8[I64LO8] );
            break;
        case TY_UINT_2:
            Gen2ByteValue( tmp.u._16[I64LO16] );
            break;
        case TY_UINT_4:
            Gen4ByteValue( tmp.u._32[I64LO32] );
            break;
        case TY_UINT_8:
            Gen8ByteValue( tmp );
            break;
        }
        if( SelCompare( &curr, hi ) >= 0 )
            break;
        if( SelCompare( &curr, &list->high ) >= 0 ) {
            list = list->next;
            curr = list->low;
        } else {
            U64IncDec( &curr, 1 );
        }
    }
}


static void GenValuesBackward( const select_list *list, const signed_64 *hi,
                    const signed_64 *lo, const signed_64 *to_sub, cg_type type )
{
    const select_list     *scan;
    const select_list     *next;
    signed_64       curr;
    unsigned_64     tmp;

    scan = list;
    while( U64Eq( scan->high, *hi ) ) {
        scan = scan->next;
    }
    curr = *hi;
    for( ;; ) {
        U64Sub( &curr, to_sub, &tmp );
        switch( type ) {
        case TY_UINT_1:
            Gen1ByteValue( tmp.u._8[I64LO8] );
            break;
        case TY_UINT_2:
            Gen2ByteValue( tmp.u._16[I64LO16] );
            break;
        case TY_UINT_4:
            Gen4ByteValue( tmp.u._32[I64LO32] );
            break;
        case TY_UINT_8:
            Gen8ByteValue( tmp );
            break;
        }
        if( SelCompare( &curr, lo ) <= 0 )
            break;
        if( SelCompare( &curr, &scan->low ) <= 0 ) {
            next = scan;
            scan = list;
            while( scan->next != next ) {
                scan = scan->next;
            }
            curr = scan->high;
        } else {
            U64IncDec( &curr, -1 );
        }
    }
}


tbl_control *MakeScanTab( sel_handle s_node, cg_type value_type, cg_type real_type )
/**********************************************************************************/
{
    tbl_control         *table;
    label_handle        *tab_ptr;
    uint_32             cases;
    signed_64           curr;
    signed_64           to_sub;
    const select_list   *scan;
    const select_list   *list;
    label_handle        other;

    list = s_node->list;
    cases = NumValues( list, &s_node->upper );
    table = CGAlloc( sizeof( tbl_control ) + ( cases - 1 ) * sizeof( label_handle ) );
    table->size = cases;
    other = s_node->other_wise;
    PUSH_OP( AskCodeSeg() );
        table->value_lbl = AskForNewLabel();
        CodeLabel( table->value_lbl, TypeAddress( TY_NEAR_CODE_PTR )->length );
        GenSelEntry( true );
        table->lbl = AskForNewLabel();
        if( value_type != real_type ) {
            to_sub = s_node->lower;
        } else {
            U64Clear( to_sub );
        }
        if( other == NULL ) {
            /*
             * no otherwise? he bakes!
             */
            other = table->cases[0];
        }
        if( value_type == TY_WORD ) {
            GenValuesForward( list, &s_node->upper, &s_node->lower, &to_sub, value_type );
        } else {
            GenValuesBackward( list, &s_node->upper, &s_node->lower, &to_sub, value_type );
        }
        GenSelEntry( false );
        CodeLabel( table->lbl, 0 );
        tab_ptr = &table->cases[0];
        if( value_type != TY_WORD ) {
            GenCodePtr( other );
        }
        scan = list;
        curr = s_node->lower;
        for( ;; ) {
            *tab_ptr = scan->label;
            GenCodePtr( *tab_ptr );
            ++tab_ptr;
            if( SelCompare( &curr, &s_node->upper ) >= 0 )
                break;
            if( SelCompare( &curr, &scan->high ) >= 0 ) {
                scan = scan->next;
                curr = scan->low;
            } else {
                U64IncDec( &curr, 1 );
            }
        }
        if( value_type == TY_WORD ) {
            GenCodePtr( other );
        }
    POP_OP();
    return( table );
}

tbl_control     *MakeJmpTab( sel_handle s_node )
/**********************************************/
{
    tbl_control         *table;
    label_handle        *tab_ptr;
    uint_32             cases;
    const select_list   *list;
    signed_64           curr;
    label_handle        other;
    unsigned_64         tmp;

    U64Sub( &s_node->upper, &s_node->lower, &tmp );
    U64IncDec( &tmp, 1 );
    cases = tmp.u._32[I64LO32];
    table = CGAlloc( sizeof( tbl_control ) + ( cases - 1 ) * sizeof( label_handle ) );
    table->size = cases;
    PUSH_OP( AskCodeSeg() );
        table->lbl = AskForNewLabel();
        table->value_lbl = NULL;
        CodeLabel( table->lbl, TypeAddress( TY_NEAR_CODE_PTR )->length );
        tab_ptr = &table->cases[0];
        other = s_node->other_wise;
        list = s_node->list;
        curr = s_node->lower;
        for( ;; ) {
            if( SelCompare( &curr, &list->low ) < 0 ) {
                *tab_ptr = other;
            } else {
                *tab_ptr = list->label;
            }
            GenCodePtr( *tab_ptr );
            ++tab_ptr;
            if( SelCompare( &curr, &s_node->upper ) >= 0 )
                break;
            if( SelCompare( &curr, &list->high ) >= 0 ) {
                list = list->next;
            }
            U64IncDec( &curr, 1 );
        }
    POP_OP();
    return( table );
}


name        *SelIdx( tbl_control *table, an node )
/************************************************/
{
    an          idxan;
    name        *idx;

    /*
     * use CG routines here to get folding
     */
    idxan = TreeGen( TGBinary( O_TIMES, TGLeaf( BGDuplicate( node ) ),
                                TGLeaf( BGInteger( WORD_SIZE, TypeAddress( TY_WORD ) ) ), TypeAddress( TY_WORD ) ) );
    idx = GenIns( idxan );
    BGDone( idxan );
    return( AllocIndex( idx, AllocMemory( table, 0, CG_TBL, WD ), 0, WD ) );
}


const type_def  *SelNodeType( an node, bool is_signed )
/*****************************************************/
{
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


void    MkSelOp( name *idx, type_class_def type_class )
/*****************************************************/
{
    instruction         *ins;

    ins = NewIns( 1 );
    ins->operands[0] = idx;
    ins->head.opcode = OP_SELECT;
    ins->type_class = type_class;
    ins->result = NULL;
    AddIns( ins );
}
