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
#include "tables.h"
#include "data.h"
#include "feprotos.h"
#include <assert.h>
#include "makeins.h"
#include "namelist.h"

extern  opcode_entry    String[];

extern  void            SuffixIns(instruction*,instruction*);
extern  bool            IsIndexReg(hw_reg_set,type_class_def,bool);
extern  reg_set_index   MarkIndex(instruction*,name*,bool);
extern  void            PrefixIns(instruction*,instruction*);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  name            *IndexToTemp( instruction *, name * );
extern  name            *FindIndex( instruction * );
extern  name            *ScaleIndex( name *, name *, type_length, type_class_def, type_length, int, i_flags );
extern  i_flags         AlignmentToFlags( type_length );
extern  name            *AllocRegName( hw_reg_set );

extern  bool    IndexOkay( instruction *ins, name *index ) {
/**********************************************************/

    name                *name;
    conflict_node       *conf;

    name = index->i.index;
    if( name->n.class == N_REGISTER ) {
        return( IsIndexReg( name->r.reg, name->n.name_class, 0 ) );
    }
    if( name->v.conflict == NULL ) return( FALSE );
    if( name->v.usage & USE_MEMORY ) return( FALSE );
    if( name->n.class != N_TEMP ) return( FALSE );
    conf = NameConflict( ins, name );
    if( conf == NULL ) return( FALSE );
    if( _Is( conf, NEEDS_INDEX_SPLIT ) ) {
        _SetFalse( conf, NEEDS_INDEX );
        return( FALSE );
    } else {
        _SetTrue( conf, NEEDS_INDEX );
        ins->head.state = OPERANDS_NEED_WORK;
        ins->t.index_needs = MarkIndex( ins, name, 0 );
        return( TRUE );
    }
}


extern  instruction     *NeedIndex( instruction *ins ) {
/******************************************************/

    name                *temp;
    name                *index;
    conflict_node       *conf;

    index = FindIndex( ins );
    if( index != NULL ) {
        temp = IndexToTemp( ins, index );
        conf = NameConflict( ins, temp );
        _SetTrue( conf, INDEX_SPLIT );
        MarkIndex( ins, temp, 0 );
        ins = ins->head.prev;
    }
    return( ins );
}


extern  void    FixChoices() {
/****************************/
}

static  const int AlignmentMap[] = {
    4,
    1,
    2,
    1,
    4,
    1,
    2,
    1,
};

static  name    *MakeIndex( instruction *memory_ins, name *memory, type_class_def class ) {
/*****************************************************************************************/

    name        *op;
    name        *temp;
    instruction *ins;
    i_flags     flags;
    fe_attr     attr;

    temp = AllocTemp( WD );
    ins = MakeUnary( OP_LA, memory, temp, WD );
    PrefixIns( memory_ins, ins );
    // Note: this assumes we put Memory names on 8-byte boundaries
    flags = AlignmentToFlags( AlignmentMap[ memory->v.offset & 0x7 ] );
    if( memory->m.alignment != 0 ) {
        flags = AlignmentToFlags( memory->m.alignment );
    }
    if( memory->m.memory_type == CG_FE ) {
        attr = FEAttr( memory->v.symbol );
        if( attr & FE_VOLATILE ) {
            flags |= X_VOLATILE;
        }
        if( attr & FE_THREAD_DATA ) {
            // this is for the kooky expansion in axpenc.c - we have
            // to call rdteb, which destroys R0
            ins->zap = &AllocRegName( HW_R0 )->r;
        }
    }
    op = ScaleIndex( temp, NULL, 0, class, memory->n.size, 0, flags );
    return( op );
}

static  name    *TruncImmediate( instruction *mem_ins, name *index ) {
/********************************************************************/

    name        *result;
    instruction *ins;
    name        *temp;

    assert( index->n.class == N_INDEXED );
    result = index;
    if( index->i.constant != (signed_16)index->i.constant ) {
        // too big to fit into a signed-16 displacement on a memory operand
        temp = AllocTemp( I4 );
        ins = MakeBinary( OP_ADD, index->i.index, AllocS32Const( index->i.constant ), temp, I4 );
        PrefixIns( mem_ins, ins );
        result = ScaleIndex( temp, NULL, 0, index->n.name_class, index->n.size, index->i.scale, index->i.index_flags );
    }
    return( result );
}

static  name    *MakeSimpleIndex( instruction *mem_ins, name *index, type_class_def class ) {
/*******************************************************************************************/

    name        *op;
    instruction *ins;
    name        *temp;

    op = index;
    if( index->i.index_flags & X_FAKE_BASE ) return( index );
    if( index->i.base != NULL ) {
        temp = AllocTemp( CP );
        ins = MakeUnary( OP_LA, index->i.base, temp, WD );
        PrefixIns( mem_ins, ins );
        ins = MakeBinary( OP_ADD, temp, index->i.index, temp, U4 );
        PrefixIns( mem_ins, ins );
        op = ScaleIndex( temp, NULL, index->i.constant, class, index->n.size, 0, index->i.index_flags );
    }
    return( TruncImmediate( mem_ins, op ) );
}

extern  void    FixMemRefs() {
/****************************/

    block       *blk;
    instruction *ins;
    name        **op;
    unsigned    i;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            op = &ins->result;
            for( i = 0; i <= ins->num_operands; i++ ) {
                if( *op != NULL ) {
                    switch( (*op)->n.class ) {
                    case N_MEMORY:
                        *op = MakeIndex( ins, *op, ins->type_class );
                        break;
                    case N_INDEXED:
                        *op = MakeSimpleIndex( ins, *op, ins->type_class );
                        break;
                    }
                }
                op = &ins->operands[i];
            }
        }
    }
}

