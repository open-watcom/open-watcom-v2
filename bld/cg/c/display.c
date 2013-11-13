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
#include "objrep.h"
#include "cgmem.h"
#include "data.h"
#include "addrfold.h"
#include "makeins.h"
#include "display.h"
#include "objout.h"
#include "makeaddr.h"
#include "namelist.h"

typedef struct frame_patch {
        struct frame_patch      *next;
        abspatch_handle         patch;
} frame_patch;

extern  int             SizeDisplayReg(void);
extern  hw_reg_set      AllCacheRegs(void);
extern  hw_reg_set      StackReg(void);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  void            AddIns(instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  hw_reg_set      DisplayReg(void);


static  name    *DisplayField( int level )
/****************************************/
{
    name        *reg;

    reg = AllocRegName( DisplayReg() );
    return( AllocIndex( reg, NULL, (-2) * level, reg->n.name_class ) );
}


extern  name    *MakeDisplay( name *op, int level )
/*************************************************/
{
    name        *temp;
    name        *reg;

    reg = AllocRegName( DisplayReg() );
    temp = AllocTemp( U2 );
    AddIns( MakeMove( DisplayField( level ), temp, reg->n.name_class ) );
    op = AllocIndex( temp, NULL, op->t.location, op->n.name_class );
    return( op );
}


extern  void    BigGoto( int level )
/**********************************/
{
    name        *reg;

    if( level != 0 ) {
        reg = AllocRegName( DisplayReg() );
        AddIns( MakeMove( DisplayField( level ), reg, reg->n.name_class ) );
    }
}


extern  void    BigLabel( void )
/******************************/
{
    instruction *ins;
    name        *bp;
    name        *sp;

    if( CurrProc->lex_level != 0 ) {
        bp = AllocRegName( DisplayReg() );
        sp = AllocRegName( StackReg() );
        ins = MakeUnary( OP_LA,
                          AllocIndex( bp, NULL, -1, bp->n.name_class ),
                          sp, sp->n.name_class );
    } else {
        ins = MakeNop();
    }
    ins->zap = (register_name *) AllocRegName( AllCacheRegs() );
    ins->flags.nop_flags |= NOP_ZAP_INFO;
    AddIns( ins );
}

extern  bool    AskIsFrameIndex( name *op ) {
/*********************************************/

    name        *bp;

    bp = AllocRegName( DisplayReg() );
    return( op == AllocIndex( bp, NULL, -1, bp->n.name_class ) );
}


extern  abspatch_handle *NextFramePatch( void )
/*********************************************/
{
    frame_patch *temp;

    temp = CGAlloc( sizeof( frame_patch ) );
    temp->next = CurrProc->frame_index;
    CurrProc->frame_index = temp;
    return( &temp->patch );
}


extern  void    PatchBigLabels( offset lc ) {
/*******************************************/

    frame_patch *frame;
    frame_patch *next;

    for( frame = CurrProc->frame_index; frame != NULL; frame = next ) {
        next = frame->next;
        AbsPatch( frame->patch, lc );
        CGFree( frame );
    }
    CurrProc->frame_index = NULL;
}



extern  an      PassProcParm( an rtn ) {
/**************************************/

    name        *op;
    name        *reg;

    op = AllocTemp( XX );
    op->n.size = TypePtr->length + SizeDisplayReg();
    reg = AllocRegName( DisplayReg() );
    AddIns( MakeMove( GenIns( rtn ),
            TempOffset( op, 0, ClassPointer ),
            ClassPointer ) );
    AddIns( MakeMove( reg, TempOffset( op, TypePtr->length,
                                       reg->n.name_class ),
                      reg->n.name_class ) );
    return( AddrName( op, TypeProcParm ) );
}


extern  void    SaveDisplay( opcode_defs op ) {
/*********************************************/

    name        *reg;

    reg = AllocRegName( DisplayReg() );
    AddIns( MakeUnary( op, reg, NULL, reg->n.name_class ) );
}


extern  void    SetDisplay( name *temp ) {
/****************************************/

    name        *reg;

    reg = AllocRegName( DisplayReg() );
    AddIns( MakeMove( TempOffset( temp, TypePtr->length,
                                  reg->n.name_class ),
                      reg, reg->n.name_class ) );
}
