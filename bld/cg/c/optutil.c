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
* Description:  Optimizer utility routines.
*
****************************************************************************/


#include "optwif.h"
#include "stackok.h"

extern  void            MultiLineNums(ins_entry*);
extern  void            DelLblRef(ins_entry*);
extern  ins_entry       *Untangle(ins_entry*);
extern  ins_entry       *IsolatedCode(ins_entry*);
extern  void            DelLblDef(ins_entry*);
extern  void            DeleteQueue(ins_entry*);
extern  void            AddLblRef(ins_entry*);
extern  void            AddLblDef(ins_entry*);
extern  void            InsertQueue(ins_entry*,ins_entry*);
extern  void            FreeInstr(ins_entry*);


extern  ins_entry       *ValidIns( ins_entry *instr ) {
/****************************************************/

  optbegin
    for(;;) {
        if( instr == NULL ) break;
        if( _Class( instr ) != OC_DEAD ) break;
        instr = instr->ins.next;
    }
    optreturn( instr );
}


extern  ins_entry       *PrevIns( ins_entry *instr ) {
/***************************************************/

  optbegin
    if( instr == NULL ) {
        instr = LastIns;
    } else if( _Class( instr ) == OC_DEAD ) {
        instr = NULL;
    } else {
        instr = instr->ins.prev;
    }
    for(;;) {
        if( instr == NULL ) break;
        if( _Class( instr ) != OC_INFO ) break;
        instr = instr->ins.prev;
    }
    optreturn( instr );
}


extern  oc_class        PrevClass( ins_entry *instr ) {
/*****************************************************/

    oc_class    class;

  optbegin
    instr = PrevIns( instr );
    if( instr != NULL ) {
        class = _Class( instr );
    } else {
        class = OC_DEAD;
    }
    optreturn( class );
}


extern  ins_entry       *NextIns( ins_entry *instr ) {
/***************************************************/

  optbegin
    if( instr == NULL ) {
        instr = FirstIns;
    } else if( _Class( instr ) == OC_DEAD ) {
        instr = NULL;
    } else {
        instr = instr->ins.next;
    }
    for(;;) {
        if( instr == NULL ) break;
        if( _Class( instr ) != OC_INFO ) break;
        instr = instr->ins.next;
    }
    optreturn( instr );
}


extern  oc_class        NextClass( ins_entry *instr ) {
/*****************************************************/

    oc_class    class;

  optbegin
    instr = NextIns( instr );
    if( instr != NULL ) {
        class = _Class( instr );
    } else {
        class = OC_DEAD;
    }
    optreturn( class );
}


extern  void    AddInstr( ins_entry *instr, ins_entry *insert ) {
/***************************************************************/

  optbegin
    InsertQueue( instr, insert );
    switch( _Class( instr ) ) {
    case OC_LABEL:
        _ValidLbl( _Label( instr ) );
        AddLblDef( instr );
        break;
    case OC_LREF:
    case OC_CALL:
    case OC_JCOND:
    case OC_JMP:
        _ValidLbl( _Label( instr ) );
        AddLblRef( instr );
        break;
    case OC_RET:
        _LblRef( instr ) = RetList;
        RetList = instr;
        break;
    }
  optend
}


extern  void    DelRef(  ins_entry **owner, ins_entry *instr  ) {
/***************************************************************/

    ins_entry   *curr;

  optbegin
    for(;;) {
        curr = *owner;
        if( curr == instr ) break;
        owner = (ins_entry **)&_LblRef( curr );
    }
    *owner = _LblRef( curr );
  optend
}


extern  void    UnLinkInstr( ins_entry *old ) {
/********************************************/

  optbegin
    InsDelete = TRUE;
    DeleteQueue( old );
    old->ins.prev = PendingDeletes;
    PendingDeletes = old;
  optend
}


static  pointer DelInstr_Helper( pointer olde ) {
/***********************************************/

    ins_entry   *old = olde;
    ins_entry   *next;

    UnLinkInstr( old );
    switch( _Class( old ) ) {
    case OC_LABEL:
        if( _Label( old ) != NULL ) {
            _ValidLbl( _Label( old ) );
        }
        DelLblDef( old );
        next = ValidIns( old->ins.next );
        switch( PrevClass( next ) ) {
        case OC_JMP:
        case OC_JMPI:
            IsolatedCode( PrevIns( next ) );
            break;
        case OC_JCOND:
            Untangle( _Label( PrevIns( next ) )->ins );
            break;
        }
        break;
    case OC_LREF:
    case OC_CALL:
    case OC_JCOND:
    case OC_JMP:
        _ValidLbl( _Label( old ) );
        DelLblRef( old );
        break;
    case OC_RET:
        DelRef( &RetList, old );
        break;
    }
    _SetClass( old, OC_DEAD );
    next = ValidIns( old->ins.next );
    for(;;) {
        if( next == NULL ) return( NULL );
        if( _Class( next ) != OC_INFO ) break;
        next = next->ins.next;
    }
    if( next->ins.prev == NULL ) return( next );
    MultiLineNums( next->ins.prev );
    return( next );
}


extern  ins_entry       *DelInstr( ins_entry *old ) {
/***************************************************/

    if( _Class( old ) != OC_DEAD ) {
        return( SafeRecurse( DelInstr_Helper, old ) );
    }
    return( ValidIns( old->ins.next ) );
}


extern  void    FreePendingDeletes() {
/************************************/

    ins_entry   *temp;

  optbegin
    while( PendingDeletes != NULL ) {
        temp = PendingDeletes->ins.prev;
        FreeInstr( PendingDeletes );
        PendingDeletes = temp;
    }
  optend
}
