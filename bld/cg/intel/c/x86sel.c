/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#define MIN_JUMPS_SETUP 12           /* minimal size of jump code */
/* for instance:
0010    83 F8 03                  cmp       eax,0x00000003
0013    77 0D                     ja        L$3
0015    FF 24 85 00 00 00 00      jmp       dword ptr L$1[eax*4]
*/
#define MIN_SCAN_SETUP  12           /* minimal size of scan code */
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
#elif _TARGET & _TARG_80386
    #define LONG_JUMP 6
    #define SHORT_JUMP 2
#endif

#define CMPSIZEDEFS \
        CMPSIZEDEF(1,2,2) \
        CMPSIZEDEF(2,3,4) \
        CMPSIZEDEF(4,9,5) \
        CMPSIZEDEF(8,0,0)

static int GetCmpSize( int type_len )
{
    int     size;

    switch( type_len ) {
#if _TARGET & _TARG_8086
        #define CMPSIZEDEF(a,b,c) case a: size=b; break;
#else
        #define CMPSIZEDEF(a,b,c) case a: size=c; break;
#endif
            CMPSIZEDEFS
        #undef CMPSIZEDEF
    default:
        size = 0;
    }
    return( size );
}

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
    uint_32             values;
    cost_val            cost;
    uint_32             type_length;
    cg_type             type;
    unsigned_64         tmp;

    /*
     * initial value, means not used
     */
    cost = MAX_COST;

    U64Sub( &tmp, &s_node->upper, &s_node->lower );
    type = SelType( &tmp );
    if( type != TY_UINT_8 ) {
        if( U64High( s_node->num_cases ) == 0 ) {
            values = U64Low( s_node->num_cases );
            if( values >= MIN_SVALUES
              || type == TY_UINT_4
              && values >= MIN_LVALUES ) {
                type_length = TypeAddress( type )->length;
                cost = Balance( MIN_SCAN_SETUP + values * ( WORD_SIZE + type_length ), values / 2 );
            }
        }
    }
    return( cost );
}


cost_val JumpCost( sel_handle s_node )
/************************************/
{
    uint_32         in_range;
    cost_val        cost;
    unsigned_64     tmp;

    /*
     * initial value, means not used
     */
    cost = MAX_COST;

    U64Sub( &tmp, &s_node->upper, &s_node->lower );
    U64Inc( tmp );
    in_range = U64Low( tmp );
    if( U64High( tmp ) == 0
      && in_range <= MAX_IN_RANGE
      && U64Low( s_node->num_cases ) >= MIN_JUMPS
      && in_range >= MIN_JUMPS ) {
        uint_32 size;

        size = MIN_JUMPS_SETUP + WORD_SIZE * in_range;
        /*
         * an extra two bytes are needed to zero the high part before
         * the jump
         */
        Set64Val1m( tmp );
        if( SelType( &tmp ) == TY_UINT_1 )
            size += 2;
        cost = Balance( size, 1 );
    }
    return( cost );
}


cost_val IfCost( sel_handle s_node, uint_32 entries )
/***************************************************/
{
    cost_val        cost;
    int             type_length;
    uint_32         log_entries;
    uint_32         jumpsize;
    uint_32         size;
    unsigned_64     tmp;

    U64Sub( &tmp, &s_node->upper, &s_node->lower );
    type_length = TypeAddress( SelType( &tmp ) )->length;
    if( entries > 20 ) {
        jumpsize = LONG_JUMP;
    } else {
        jumpsize = SHORT_JUMP;
    }
    size = jumpsize + GetCmpSize( type_length );
    /*
     * for char-sized switches, often the two-byte "cmp al,xx" is used.
     * otherwise we need three bytes
     */
    Set64Val1m( tmp );
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
    cost = Balance( size, log_entries );
    if( cost >= MAX_COST ) {
        cost = MAX_COST - 1;
    }
    return( cost );
}

static void GenValuesForward( sel_handle s_node, const signed_64 *to_sub, cg_type type )
/**************************************************************************************/
{
    signed_64           curr;
    unsigned_64         tmp;
    const select_list   *list;

    list = s_node->list;
    curr = s_node->lower;
    for( ;; ) {
        U64Sub( &tmp, &curr, to_sub );
        switch( type ) {
        case TY_UINT_1:
            Gen1ByteValue( U64LowByte( tmp ) );
            break;
        case TY_UINT_2:
            Gen2ByteValue( U64LowWord( tmp ) );
            break;
        case TY_UINT_4:
            Gen4ByteValue( U64Low( tmp ) );
            break;
        case TY_UINT_8:
            Gen8ByteValue( &tmp );
            break;
        }
        if( SelCompare( &curr, &s_node->upper ) >= 0 )
            break;
        if( SelCompare( &curr, &list->high ) >= 0 ) {
            list = list->next;
            curr = list->low;
        } else {
            U64Inc( curr );
        }
    }
}


static void GenValuesBackward( sel_handle s_node, const signed_64 *to_sub, cg_type type )
{
    const select_list   *list;
    const select_list   *next;
    signed_64           curr;
    unsigned_64         tmp;

    list = s_node->list;
    while( U64isntEq( list->high, s_node->upper ) ) {
        list = list->next;
    }
    curr = s_node->upper;
    for( ;; ) {
        U64Sub( &tmp, &curr, to_sub );
        switch( type ) {
        case TY_UINT_1:
            Gen1ByteValue( U64LowByte( tmp ) );
            break;
        case TY_UINT_2:
            Gen2ByteValue( U64LowWord( tmp ) );
            break;
        case TY_UINT_4:
            Gen4ByteValue( U64Low( tmp ) );
            break;
        case TY_UINT_8:
            Gen8ByteValue( &tmp );
            break;
        }
        if( SelCompare( &curr, &s_node->lower ) <= 0 )
            break;
        if( SelCompare( &curr, &list->low ) <= 0 ) {
            next = list;
            list = s_node->list;
            while( list->next != next ) {
                list = list->next;
            }
            curr = list->high;
        } else {
            U64Dec( curr );
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
    const select_list   *list;
    label_handle        other;

    cases = U64Low( s_node->num_cases );
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
            Set64ValZero( to_sub );
        }
        if( other == NULL ) {
            /*
             * no otherwise? he bakes!
             */
            other = table->cases[0];
        }
        if( value_type == TY_WORD ) {
            GenValuesForward( s_node, &to_sub, value_type );
        } else {
            GenValuesBackward( s_node, &to_sub, value_type );
        }
        GenSelEntry( false );
        CodeLabel( table->lbl, 0 );
        tab_ptr = &table->cases[0];
        if( value_type != TY_WORD ) {
            GenCodePtr( other );
        }
        list = s_node->list;
        curr = s_node->lower;
        for( ;; ) {
            *tab_ptr = list->label;
            GenCodePtr( *tab_ptr );
            ++tab_ptr;
            if( SelCompare( &curr, &s_node->upper ) >= 0 )
                break;
            if( SelCompare( &curr, &list->high ) >= 0 ) {
                list = list->next;
                curr = list->low;
            } else {
                U64Inc( curr );
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

    U64Sub( &tmp, &s_node->upper, &s_node->lower );
    U64Inc( tmp );
    cases = U64Low( tmp );
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
            U64Inc( curr );
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
