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
#include "utils.h"

extern  void            AddInstr( ins_entry *, ins_entry * );
extern  ins_entry       *NextIns( ins_entry * );
extern  oc_class        NextClass( ins_entry * );
extern  int             OptInsSize( oc_class, oc_dest_attr );


extern  ins_entry       *NewInstr( any_oc *oc )
/*********************************************/
{
    ins_entry   *instr;
    oc_length   len;

  optbegin
    len = oc->oc_header.reclen + sizeof( ins_link );
    if( len <= OCENTRY_SIZE ) {
        instr = AllocFrl( &InstrFrl, OCENTRY_SIZE );
    } else {
        instr = CGAlloc( len );
    }
    instr->ins.prev = NULL;
    instr->ins.next = NULL;
    Copy( oc, &instr->oc, oc->oc_header.reclen );
    optreturn( instr );
}


extern  void    FreeInstr( ins_entry *instr )
/*******************************************/
{
    oc_length   len;

    len = instr->oc.oc_header.reclen + sizeof( ins_link );
    if( len <= OCENTRY_SIZE ) {
        FrlFreeSize( &InstrFrl, (pointer *)instr, OCENTRY_SIZE );
    } else {
        CGFree( instr );
    }
}


extern  bool    InstrFrlFree( void )
/**********************************/
{
    return( FrlFreeAll( &InstrFrl, OCENTRY_SIZE ) );
}


extern  label_handle AddNewLabel( ins_entry *new, int align )
/***************************************************************/
{
    label_handle    lbl;
    any_oc          oc;

  optbegin
    if( NextClass( new ) == OC_LABEL )
        optreturn( _Label( NextIns( new ) ) );
    if( new != NULL && _Class( new ) == OC_LABEL )
        optreturn( _Label( new ) );
    oc.oc_handle.hdr.class = OC_LABEL;
    oc.oc_handle.hdr.reclen = sizeof( oc_handle );
    oc.oc_handle.hdr.objlen = align;
    lbl = AskForNewLabel();
    oc.oc_handle.ref = NULL;
    oc.oc_handle.handle = lbl;
#if _TARGET & _TARG_RISC
    oc.oc_handle.line = 0;
#endif
    AddInstr( NewInstr( &oc ), new );
    _SetStatus( lbl, DYINGLABEL );
    optreturn( lbl );
}


extern  void    AddNewJump( ins_entry *new, label_handle lbl )
/*********************************************************/
{
    any_oc     oc;

  optbegin
    oc.oc_handle.hdr.class = OC_JMP;
    oc.oc_handle.hdr.reclen = sizeof( oc_handle );
    oc.oc_handle.hdr.objlen = OptInsSize( OC_JMP, OC_DEST_NEAR );
    oc.oc_handle.ref = NULL;
    oc.oc_handle.handle = lbl;
#if _TARGET & _TARG_RISC
    oc.oc_handle.line = 0;
#endif
    AddInstr( NewInstr( &oc ), new );
  optend
}
