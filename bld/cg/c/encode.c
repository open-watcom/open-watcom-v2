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
#include "objout.h"
#include "dumpio.h"


extern  void            GenSetCC( instruction * );
extern  byte            CondCode( instruction * );
extern  void            InputOC(any_oc*);
extern  obj_length      OptInsSize(oc_class,oc_dest_attr);
extern  void            FlipCond(instruction*);

static  label_handle    LocateLabel( instruction *ins, int index ) {
/*******************************************************************
    find the true or false ("index") label of the block containing "ins"
*/

    if( index == NO_JUMP ) return( NULL );
    for( ins = ins->head.next; ins->head.opcode != OP_BLOCK; ) {
        ins = ins->head.next;
    }
    return( _BLOCK( ins )->edge[  index  ].destination.u.lbl );
}

#if _TARGET & _TARG_RISC
extern  void    CodeLabelLinenum( label_handle label, unsigned align, cg_linenum line ) {
/***************************************************************************************/

    any_oc      oc;

    if( OptForSize > 50 || align == 0 )
        align = 1;
    oc.oc_handle.hdr.class = OC_LABEL;
    oc.oc_handle.hdr.reclen = sizeof( oc_handle );
    oc.oc_handle.hdr.objlen = align - 1;
    oc.oc_handle.ref = NULL;
    oc.oc_handle.handle = label;
    oc.oc_handle.line = line;
    InputOC( &oc );

}
#endif

extern  void    CodeLabel( label_handle label, unsigned align ) {
/****************************************************************
    Drop label into the queue
*/
    if( OptForSize > 50 || align == 0 )
        align = 1;
    CodeHandle( OC_LABEL, align - 1, label );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpChar( 'L' );
        DumpPtr( label );
        DumpChar( ':' );
        DumpNL();
    }
#endif
#endif
}

extern  void    CodeLineNum( cg_linenum line, bool label_line ) {
/*****************************************************************
    Dump a line number into the queue
*/

    any_oc      oc;

    if( line != 0 ) {
        oc.oc_linenum.hdr.class = OC_LINENUM;
        oc.oc_linenum.hdr.reclen = sizeof( oc_linenum );
        oc.oc_linenum.hdr.objlen = 0;
        oc.oc_linenum.label_line = label_line;
        oc.oc_linenum.line = line;
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
        if( _IsTargetModel( ASM_OUTPUT ) ) {
            DumpLiteral( "Source Line: " );
            DumpInt( line );
            DumpNL();
        }
#endif
#endif
        InputOC( &oc );
    }
}


extern  void    CodeHandle( oc_class class, obj_length len, label_handle handle ) {
/***************************************************************************
    Dump a label reference to "handle" of class "class" (LREF or LABEL)
    into the queue.  Len is the code space taken.
*/

    any_oc      oc;

    oc.oc_handle.hdr.class = class;
    oc.oc_handle.hdr.reclen = sizeof( oc_handle );
    oc.oc_handle.hdr.objlen = len;
    oc.oc_handle.ref = NULL;
    oc.oc_handle.handle = handle;
#if _TARGET & _TARG_RISC
    oc.oc_handle.line = 0;
#endif
    InputOC( &oc );
}

static  void    DoCondJump( instruction *cond ) {
/************************************************
    Generate a conditional branch
*/

    any_oc              oc;
    label_handle        dest_true;
    label_handle        dest_false;
    label_handle        dest_next;
    instruction         *next;

    for( next = cond->head.next; next->head.opcode != OP_BLOCK; ) {
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
        oc.oc_jcond.hdr.class = OC_JCOND;
#if _TARGET & _TARG_INTEL
        if( !_CPULevel( CPU_386 ) ) {
            oc.oc_jcond.hdr.class |= ATTR_SHORT;
        }
#endif
#if _TARGET & _TARG_RISC
        if( _IsFloating( cond->type_class ) ) {
            oc.oc_jcond.hdr.class |= ATTR_FLOAT;
        }
#endif
        oc.oc_jcond.hdr.reclen = sizeof( oc_jcond );
        oc.oc_jcond.hdr.objlen = OptInsSize( OC_JCOND, OC_DEST_NEAR );
        oc.oc_jcond.ref = NULL;
        oc.oc_jcond.cond = CondCode( cond );
        oc.oc_jcond.handle = dest_true;
#if _TARGET & _TARG_RISC
        assert( cond->operands[ 0 ]->n.class == N_REGISTER );
        oc.oc_jcond.index = cond->operands[ 0 ]->r.arch_index;
        if( cond->operands[1]->n.class == N_REGISTER ) {
            oc.oc_jcond.index2 = cond->operands[1]->r.arch_index;
        } else {
            oc.oc_jcond.index2 = -1;
        }
#endif
        InputOC( &oc );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
        if( _IsTargetModel( ASM_OUTPUT ) ) {
            DumpLiteral( "Jcc L" );
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

    label_handle    dest;

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

extern  void    GenJumpLabel( label_handle label ) {
/**********************************************
    generate an unconditional jump to "label"
*/

    CodeHandle( OC_JMP, OptInsSize( OC_JMP, OC_DEST_NEAR ), label );
#if _TARGET & _TARG_RISC
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpLiteral( "JMP L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
#endif
}

extern  void    GenKillLabel( label_handle label ) {
/**********************************************
    indicate that "label" won't be used again after the OC_LDONE comes
    out of the queue.
*/

    segment_id      old;

    old = SetOP( AskCodeSeg() );
    CodeHandle( OC_LDONE, 0, label );
    SetOP( old );
}
