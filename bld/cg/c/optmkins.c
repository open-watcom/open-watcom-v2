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
* Description:  Instruction creation and deletion.
*
****************************************************************************/


#include "optwif.h"
#include "freelist.h"

extern  void            AddInstr( ins_entry *, ins_entry * );
extern  label_handle    AskForNewLabel( void );
extern  ins_entry       *NextIns( ins_entry * );
extern  oc_class        NextClass( ins_entry * );
extern  pointer         Copy( pointer, pointer, uint );
extern  int             OptInsSize( oc_class, oc_dest_attr );


extern  ins_entry       *NewInstr( any_oc *oc )
/*********************************************/
{
    ins_entry   *instr;
    oc_length   len;

  optbegin
    len = oc->oc_entry.reclen + sizeof( ins_link );
    if( len <= INSTR_FRLSIZE ) {
        instr = AllocFrl( &InstrFrl, INSTR_FRLSIZE );
    } else {
        instr = CGAlloc( oc->oc_entry.reclen + sizeof( ins_link ) );
    }
    instr->ins.prev = NULL;
    instr->ins.next = NULL;
    Copy( oc, &instr->oc, oc->oc_entry.reclen );
    optreturn( instr );
}


extern  void    FreeInstr( ins_entry *instr )
/*******************************************/
{
    oc_length   len;

    len = instr->oc.oc_entry.reclen + sizeof( ins_link );
    if( len <= INSTR_FRLSIZE ) {
        FrlFreeSize( &InstrFrl, (pointer *)instr, INSTR_FRLSIZE );
    } else {
        CGFree( instr );
    }
}


extern  bool    InstrFrlFree( void )
/**********************************/
{
    return( FrlFreeAll( &InstrFrl, INSTR_FRLSIZE ) );
}


extern  code_lbl        *AddNewLabel( ins_entry *new, int align )
/***************************************************************/
{
    code_lbl    *lbl;
    any_oc      lbl_oc;

  optbegin
    if( NextClass( new ) == OC_LABEL )
        optreturn( _Label( NextIns( new ) ) );
    if( new != NULL && _Class( new ) == OC_LABEL )
        optreturn( _Label( new ) );
    lbl_oc.oc_entry.class = OC_LABEL;
    lbl_oc.oc_entry.objlen = align;
    lbl_oc.oc_entry.reclen = sizeof( oc_handle );
    lbl = AskForNewLabel();
    lbl_oc.oc_handle.handle = lbl;
#if _TARGET & _TARG_RISC
    lbl_oc.oc_handle.line = 0;
#endif
    AddInstr( NewInstr( &lbl_oc ), new );
    _SetStatus( lbl, DYINGLABEL );
    optreturn( lbl );
}


extern  void    AddNewJump( ins_entry *new, code_lbl *lbl )
/*********************************************************/
{
    any_oc     jmp_oc;

  optbegin
    jmp_oc.oc_header.class = OC_JMP;
    jmp_oc.oc_header.objlen = OptInsSize( OC_JMP, OC_DEST_NEAR );
    jmp_oc.oc_header.reclen = sizeof( oc_handle );
    jmp_oc.oc_handle.handle = lbl;
    AddInstr( NewInstr( &jmp_oc ), new );
  optend
}
