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


#include "optwif.h"

extern    ins_entry     *LastIns;
extern    bool          InsDelete;

extern  ins_entry       *IsolatedCode(ins_entry*);
extern  void            AddInstr(ins_entry*,ins_entry*);
extern  ins_entry       *NewInstr(any_oc*);
extern  ins_entry       *DelInstr(ins_entry*);
extern  void            UnLinkInstr(ins_entry*);
extern  ins_entry       *PrevIns(ins_entry*);
extern  oc_class        PrevClass(ins_entry*);
extern  ins_entry       *Untangle(ins_entry*);
extern  int             OptInsSize(oc_class,oc_dest_attr);


extern  bool    RetAftrLbl( ins_entry *ret ) {
/********************************************/

    ins_entry   *ref;
    code_lbl    *lbl;
    ins_entry   *instr;
    bool        change;

  optbegin
    change = FALSE;
    if( PrevClass( ret ) == OC_LABEL ) {
        lbl = _Label( PrevIns( ret ) );
        ref = lbl->refs;
        while( ref != NULL ) {
            instr = ref;
            ref = _LblRef( ref );
            if( _Class( instr ) == OC_JMP ) {
                JmpToRet( instr, ret );
                change = TRUE;
            } else if( _Class( instr ) == OC_CALL ) {
                if( (_Attr( ret ) & ATTR_POP) == 0 ) {
                    _Savings( OPT_CALLTORET, _ObjLen( instr ) );
                    DelInstr( instr );
                    change = TRUE;
                }
            }
        }
    }
    optreturn( change );
}


extern  void    JmpToRet( ins_entry *instr, ins_entry *ret ) {
/************************************************************/

  optbegin
    ret = NewInstr( &ret->oc );
    AddInstr( ret, instr );
    _Savings( OPT_JMPTORET, _ObjLen( instr ) - _ObjLen( ret ) );
    DelInstr( instr );
    RetAftrCall( ret );
  optend


extern  void    RetAftrCall( ins_entry *ret_instr ) {
/***************************************************/

    ins_entry   *call_instr;
    oc_class    call_attr;
    oc_class    ret_attr;

  optbegin
    if( _IsTargetModel( NO_CALL_RET_TRANSFORM ) ) optreturnvoid;
    call_instr = ret_instr;
    while( PrevClass( call_instr ) == OC_LABEL ) {
        call_instr = PrevIns( call_instr );
    }
    if( PrevClass( call_instr ) != OC_CALL ) optreturnvoid;
    call_instr = PrevIns( call_instr );
    call_attr = _Attr( call_instr );
    if( (call_attr & ATTR_POP) != 0 ) optreturnvoid;
    ret_attr = _Attr( ret_instr );
    if( (ret_attr & ATTR_POP) != 0 ) optreturnvoid;
#if( OPTIONS & SEGMENTED )
    if( ( (call_attr & ATTR_FAR) ^ (ret_attr & ATTR_FAR) ) != 0 ) optreturnvoid;
#endif
    InsDelete = TRUE;
    _ChgClass( call_instr, OC_JMP );
    /* NB! this code assumes that we aren't making the instruction longer */
    if( _ObjLen( call_instr ) == OptInsSize( OC_CALL, OC_DEST_CHEAP ) ) {
        _ObjLen( call_instr ) = OptInsSize( OC_JMP, OC_DEST_NEAR );
        call_instr->oc.oc_entry.class &= ~ATTR_FAR;
    } else if( _ObjLen( call_instr ) == OptInsSize( OC_CALL, OC_DEST_FAR ) ) {
        _ObjLen( call_instr ) = OptInsSize( OC_JMP, OC_DEST_FAR );
    } else {
        _ObjLen( call_instr ) = OptInsSize( OC_JMP, OC_DEST_NEAR );
    }
    IsolatedCode( call_instr );
    Untangle( PrevIns( call_instr ) );
  optend


extern  void    MultiLineNums( ins_entry *ins ) {
/***********************************************/

    ins_entry   *prev;

  optbegin
    if( ins->oc.oc_entry.class == OC_LINENUM ) {
        prev = ins->ins.prev;
        for(;;) {
            if( prev == NULL ) break;
            if( _Class( prev ) != OC_INFO ) break;
            if( prev->oc.oc_entry.class == OC_LINENUM ) {
                ins = prev->ins.next;
                UnLinkInstr( prev );
                _SetClass( prev, OC_DEAD );
                prev = ins->ins.prev;
            } else {
                prev = prev->ins.prev;
            }
        }
    }
  optend
