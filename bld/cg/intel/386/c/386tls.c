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
* Description:  386 TLS support gear (for Win32).
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "pattern.h"
#include "zoiks.h"
#include "data.h"
#include "feprotos.h"
#include "makeins.h"
#include "rtrtn.h"
#include "namelist.h"

extern  name            *AllocRegName( hw_reg_set );
extern  name            *AllocIndex( name *, name *, type_length, type_class_def );
extern  name            *ScaleIndex( name *, name *,
                                type_length, type_class_def, type_length, int, i_flags );
extern  void            PrefixIns( instruction *, instruction * );

static  name    *RTMemRef( rt_class rt_index )
/*********************************************
    create a memory_name to reference the given runtime label.
*/
{
    label_handle    lbl;

    lbl = RTLabel( rt_index );
    return( AllocMemory( lbl, 0, CG_LBL, WD ) );
}

static  void    AddSegOverride( instruction *ins, hw_reg_set reg )
/****************************************************************/
{
    name                *reg_name;

    reg_name = AllocRegName( reg );
    ins->operands[ ins->num_operands++ ] = reg_name;
}

static  name    *GetNTTLSDataRef( instruction *ins, name *op, type_class_def tipe )
/**********************************************************************************
    Emit instructions to load allow a reference to op (a piece of
    TLS data) and return the resulting index name.
*/
{
    name                *tls_index;
    name                *tls_array;
    name                *t1;
    name                *t2;
    name                *t3;
    name                *temp_index;
    name                *result_index;
    instruction         *new_ins;

    tls_index = RTMemRef( RT_TLS_INDEX );
    tls_array = RTMemRef( RT_TLS_ARRAY );
    t1 = AllocTemp( WD );
    t2 = AllocTemp( WD );
    t3 = AllocTemp( WD );
    new_ins = MakeMove( tls_array, t1, WD );
    AddSegOverride( new_ins, HW_FS );
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( tls_index, t2, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_MUL, t2, AllocS32Const( 4 ), t2, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_ADD, t1, t2, t1, WD );
    PrefixIns( ins, new_ins );
    temp_index = AllocIndex( t1, NULL, 0, WD );
    new_ins = MakeMove( temp_index, t3, WD );
    PrefixIns( ins, new_ins );
    result_index = ScaleIndex( t3, op, op->v.offset, tipe, TypeClassSize[ tipe ], 0, 0 );
    return( result_index );
}

static  void    DropCall( instruction *ins, name *temp )
/******************************************************/
{
    name                *eax_name;
    name                *null_name;
    instruction         *new_ins;

    null_name = AllocRegName( HW_EMPTY );
    eax_name = AllocRegName( HW_EAX );
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = WD;
    new_ins->operands[ CALL_OP_USED ] = null_name;
    new_ins->operands[ CALL_OP_USED2 ] = null_name;
    new_ins->operands[ CALL_OP_ADDR ]= RTMemRef( RT_TLS_REGION );
    new_ins->result = eax_name;
    new_ins->zap = &eax_name->r;
    new_ins->num_operands = 2;         /* special case for OP_CALL*/
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( eax_name, temp, WD );
    PrefixIns( ins, new_ins );
}

static  name    *GetGenericTLSDataRef( instruction *ins, name *op, type_class_def tipe )
/**************************************************************************************/
{
    name                *tls;
    name                *result_index;
    name                *temp;
    instruction         *new_ins;


    tls = CurrProc->targ.tls_index;
    if( tls == NULL ) {
        tls = AllocTemp( WD );
        CurrProc->targ.tls_index = tls;
        if( !BlockByBlock ) {
            /* 2007-06-28 RomanT
             * Grrr... DropCall() inserts new ins _before_ old one, screwing
             * startup sequence. So keep call in first block, but make it
             * before next instruction of last one  (== after last one).
             */
            DropCall( HeadBlock->ins.hd.prev->head.next, tls );
        }
    }
    if( BlockByBlock ) {
        DropCall( ins, tls );
    }
    temp = AllocTemp( WD );
    new_ins = MakeMove( tls, temp, WD );
    PrefixIns( ins, new_ins );
    result_index = ScaleIndex( temp, op, op->v.offset, tipe, TypeClassSize[ tipe ], 0, 0 );
    return( result_index );
}

static  name    *GetTLSDataRef( instruction *ins, name *op, type_class_def tipe )
/*******************************************************************************/
{
    if( _IsTargetModel( GENERIC_TLS ) ) {
        return( GetNTTLSDataRef( ins, op, tipe ) );
    } else {
        return( GetGenericTLSDataRef( ins, op, tipe ) );
    }
}

static  void    ExpandTlsOp( instruction *ins, name **pop )
/**********************************************************
    If *pop is a ref to a piece of thread-local data, replace
    it by a ref to an index [t1] and prepend the magic sequence
    to get the address of a piece of tls data to the instruction.
    Here is the sequence to access variable foo:
        mov fs:__tls_array -> t1
        mov __tls_index -> t2
        mov t2 * 4 -> t2
        add t1, t2 -> t1
        mov [ t1 ] -> t3
        mov foo[ t3 ] -> result
*/
{
    fe_attr             attr;
    name                *op;
    name                *temp;
    name                *tls_data;
    name                *index;
    name                *base;
    instruction         *new_ins;

    op = *pop;
    switch( op->n.class ) {
    case N_MEMORY:
        if( op->m.memory_type == CG_FE ) {
            attr = FEAttr( op->v.symbol );
            if( ( attr & FE_THREAD_DATA ) != 0 ) {
                *pop = GetTLSDataRef( ins, op, _OpClass(ins) );
            }
        }
        break;
    case N_INDEXED:
        // gotta check for the base being one of these stupid TLS things
        if( op->i.base != NULL && ( op->i.index_flags & X_FAKE_BASE ) == 0 ) {
            base = op->i.base;
            if( base->n.class != N_MEMORY || base->m.memory_type != CG_FE ) break;
            attr = FEAttr( base->v.symbol );
            if( ( attr & FE_THREAD_DATA ) == 0 ) break;
            tls_data = GetTLSDataRef( ins, base, _OpClass(ins) );
            temp = AllocTemp( WD );
            new_ins = MakeUnary( OP_LA, tls_data, temp, WD );
            PrefixIns( ins, new_ins );
            index = op->i.index;
            if( op->i.scale != 0 ) {
                const signed_32 values[] = { 1, 2, 4, 8, 16 };
                if( op->i.scale > 4 ) _Zoiks( ZOIKS_134 );
                index = AllocTemp( WD );
                new_ins = MakeBinary( OP_MUL, op->i.index,
                                AllocS32Const( values[ op->i.scale ] ),
                                index, WD );
                PrefixIns( ins, new_ins );
            }
            new_ins = MakeBinary( OP_ADD, temp, index, temp, WD );
            PrefixIns( ins, new_ins );
            *pop = ScaleIndex( temp, NULL, 0,
                            _OpClass(ins),
                            TypeClassSize[ _OpClass(ins) ],
                            0, 0 );
        }
        break;
    }
}

extern  void    ExpandThreadDataRef( instruction *ins )
/******************************************************
    Expand any references to thread-local data into the
    appropriate magical sequence of instructions.  Note
    that since this is done independantly for each
    reference to tls data, we end up with horrible,
    redundant code.  However, the optimizer should be
    able to clean this up providing -oa has been
    used.
*/
{
    int                 i;

    for( i = 0; i < ins->num_operands; i++ ) {
        ExpandTlsOp( ins, &ins->operands[i] );
    }
    if( ins->result != NULL ) {
        ExpandTlsOp( ins, &ins->result );
    }
}
