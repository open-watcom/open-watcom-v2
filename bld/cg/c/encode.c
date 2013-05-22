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
#include "coderep.h"
#include "ocentry.h"
#include "system.h"
#include "cgaux.h"
#include "cgdefs.h"
#include "objrep.h"
#include "encode.h"
#include "data.h"


extern  void            GenSetCC( instruction * );
extern  byte            CondCode( instruction * );
extern  void            InputOC(any_oc*);
extern  seg_id          AskCodeSeg( void );
extern  int             OptInsSize(oc_class,oc_dest_attr);
extern  void            FlipCond(instruction*);
extern  seg_id          SetOP(seg_id);
extern  void            DumpString( char * );
extern  void            DumpPtr( pointer );
extern  void            DumpInt( int );
extern  void            DumpNL( void );

static  label_handle    LocateLabel( instruction *ins, int index ) {
/*******************************************************************
    find the true or false ("index") label of the block containing "ins"
*/

    if( index == NO_JUMP ) return( NULL );
    for(;;) {
        ins = ins->head.next;
        if( ins->head.opcode == OP_BLOCK ) break;
    }
    return( _BLOCK( ins )->edge[  index  ].destination );
}

#if _TARGET & _TARG_RISC
extern  void    CodeLabelLinenum( label_handle label, unsigned align, cg_linenum line ) {
/***************************************************************************************/

    oc_handle   temp;

    if( OptForSize > 50 || align == 0 ) align = 1;
    temp.op.class = OC_LABEL;
    temp.op.reclen = sizeof( oc_handle );
    temp.op.objlen = align - 1;
    temp.handle = label;
    temp.line = line;
    InputOC( (any_oc *)&temp );

}
#endif

extern  void    CodeLabel( label_handle label, unsigned align ) {
/****************************************************************
    Drop label into the queue
*/
    if( OptForSize > 50 || align == 0 ) align = 1;
    CodeHandle( OC_LABEL, align-1, label );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "L" );
        DumpPtr( label );
        DumpString( ":" );
        DumpNL();
    }
#endif
#endif
}

extern  void    CodeLineNum( cg_linenum line, bool label_line ) {
/*****************************************************************
    Dump a line number into the queue
*/

    oc_linenum  temp;

    if( line != 0 ) {
        temp.op.class = OC_LINENUM;
        temp.op.reclen = sizeof( oc_linenum );
        temp.op.objlen = 0;
        temp.label_line = label_line;
        temp.line = line;
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
        if( _IsTargetModel( ASM_OUTPUT ) ) {
            DumpString( "Source Line: " );
            DumpInt( line );
            DumpNL();
        }
#endif
#endif
        InputOC( (any_oc *)&temp );
    }
}


extern  void    CodeHandle( oc_class class, int len, label_handle handle ) {
/***************************************************************************
    Dump a label reference to "handle" of class "class" (LREF or LABEL)
    into the queue.  Len is the code space taken.
*/

    oc_handle   temp;

    temp.op.class = class;
    temp.op.reclen = sizeof( oc_handle );
    temp.op.objlen = len;
    temp.handle = handle;
#if _TARGET & _TARG_RISC
    temp.line = 0;
#endif
    InputOC( (any_oc *)&temp );
}

static  void    DoCondJump( instruction *cond ) {
/************************************************
    Generate a conditional branch
*/

    oc_jcond            temp;
    label_handle        dest_true;
    label_handle        dest_false;
    label_handle        dest_next;
    instruction         *next;

    next = cond->head.next;
    while( next->head.opcode != OP_BLOCK ) {
        next = next->head.next;
    }
    dest_next = NULL;
    if( _BLOCK( next )->next_block != NULL ) {
        dest_next = _BLOCK( next )->next_block->label;
    }
    dest_false = LocateLabel( cond, _FalseIndex( cond ) );
    dest_true = LocateLabel( cond, _TrueIndex( cond ) );
    if( dest_next != NULL ) {
        if( dest_true == dest_next && dest_false != NULL ) {
            FlipCond( cond );
            dest_true = dest_false;
            dest_false = dest_next;
        }
    }
    if( dest_true != NULL && dest_false != dest_true &&
        ( dest_true != dest_next || dest_false == NULL ) ) {
        temp.op.class = OC_JCOND;
#if _TARGET & _TARG_INTEL
        if( !_CPULevel( CPU_386 ) ) {
            temp.op.class |= ATTR_SHORT;
        }
#else
        assert( cond->operands[ 0 ]->n.class == N_REGISTER );
        temp.index = cond->operands[ 0 ]->r.arch_index;
        if( cond->operands[1]->n.class == N_REGISTER )
            temp.index2 = cond->operands[1]->r.arch_index;
        else
            temp.index2 = -1;
        if( _IsFloating( cond->type_class ) ) {
            temp.op.class |= ATTR_FLOAT;
        }
#endif
        temp.op.objlen = OptInsSize( OC_JCOND, OC_DEST_NEAR );
        temp.op.reclen = sizeof( oc_jcond );
        temp.cond = CondCode( cond );
        temp.handle = dest_true;
        InputOC( (any_oc *)&temp );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
        if( _IsTargetModel( ASM_OUTPUT ) ) {
            DumpString( "Jcc L" );
            DumpPtr( dest_true );
            DumpNL();
        }
#endif
#endif
    }
    if( dest_false != dest_next && dest_false != NULL ) {
        GenJumpLabel( dest_false );
    }
}

extern  void    GenCondJump( instruction *cond ) {
/*************************************************
    Given conditional "cond", generate the correct jcc or setcc instruction
*/

    label_handle        dest;

    if( cond->result == NULL ) {
        if( _TrueIndex( cond ) == _FalseIndex( cond ) ) {
            /* conditional jump not required - both branches go to same place*/
            dest = LocateLabel( cond, _FalseIndex( cond ) );
            if( dest != NULL ) {
                GenJumpLabel( dest );
            }
        } else {
            DoCondJump( cond );
        }
    } else {
        GenSetCC( cond );
    }
}

extern  void    GenJumpLabel( pointer label ) {
/**********************************************
    generate an unconditional jump to "label"
*/

    CodeHandle( OC_JMP, OptInsSize( OC_JMP, OC_DEST_NEAR ), label );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "JMP L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
#endif
}

extern  void    GenKillLabel( pointer label ) {
/**********************************************
    indicate that "label" won't be used again after the OC_LDONE comes
    out of the queue.
*/

    seg_id      old;

    old = SetOP( AskCodeSeg() );
    CodeHandle( OC_LDONE, 0, label );
    SetOP( old );
}
