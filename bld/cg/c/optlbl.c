/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "optwif.h"
#include "optutil.h"


void    AddLblDef( ins_entry *instr )
/***********************************/
{
    label_handle    lbl;

  optbegin
    _LblRef( instr ) = NULL;
    lbl = _Label( instr );
    for( ;; ) {
        _SetStatus( lbl, CODELABEL );
        lbl->ins = instr;
        lbl = lbl->alias;
        if( lbl == NULL ) {
            break;
        }
    }
  optend
}


void    DelLblDef( ins_entry *instr )
/***********************************/
{
    label_handle    lbl;

  optbegin
    lbl = _Label( instr );
    if( lbl == NULL )
        optreturnvoid;
    if( lbl->refs != NULL )
        optreturnvoid;
    if( !_TstStatus( lbl, DYINGLABEL ) )
        optreturnvoid;
    if( _TstStatus( lbl, REDIRECTION ) )
        optreturnvoid;
    ScrapCodeLabel( lbl );
  optend
}


void    AddLblRef( ins_entry *instr )
/***********************************/
{
    label_handle    lbl;

  optbegin
    lbl = _Label( instr );
    if( lbl->ins != NULL ) {
        lbl = _Label( lbl->ins );
        _Label( instr ) = lbl;
    }
    _LblRef( instr ) = lbl->refs;
    lbl->refs = instr;
  optend
}


void    DelLblRef( ins_entry *instr )
/***********************************/
{
    label_handle    old_lbl;

  optbegin
    old_lbl = _Label( instr );
    DelRef( &old_lbl->refs, instr );
    TryScrapLabel( old_lbl );
  optend
}


void    ChgLblRef( ins_entry *instr, label_handle new_lbl )
/*********************************************************/
{
    ins_entry       **owner;
    ins_entry       *curr;
    label_handle    old_lbl;

  optbegin
    old_lbl = _Label( instr );
    if( old_lbl != new_lbl ) {
        owner = &old_lbl->refs;
        for( ;; ) {
            curr = *owner;
            if( curr == instr )
                break;
            owner = (ins_entry **)&_LblRef( curr );
        }
        *owner = _LblRef( curr );
        _LblRef( curr ) = new_lbl->refs;
        new_lbl->refs = curr;
        _Label( curr ) = new_lbl;
        TryScrapLabel( old_lbl );
    }
  optend
}


bool    UniqueLabel( label_handle lbl )
/**************************************
        Does the label have any alias which is a UNIQUE label?
*/
{
  optbegin
    for( ; lbl != NULL; lbl = lbl->alias ) {
        if( _TstStatus( lbl, UNIQUE ) ) {
            optreturn( true );
        }
    }
    optreturn( false );
}


ins_entry       *AliasLabels( ins_entry *old_ins, ins_entry *new_ins )
/********************************************************************/
{
    label_handle    old_lbl;
    label_handle    new_lbl;
    ins_entry       *old_jmp;
    ins_entry       **owner;

  optbegin
    if( old_ins != new_ins ) {
        /* use maximum of the two aligment requests */
        if( _ObjLen( old_ins ) > _ObjLen( new_ins ) ) {
            _ObjLen( new_ins ) = _ObjLen( old_ins );
        }
#if _TARGET_RISC
        /* one of the line numbers (at most) is non-zero - keep it */
        if( _LblLine( old_ins ) != 0 ) {
            _LblLine( new_ins ) = _LblLine( old_ins );
        }
#endif
        _SetAttr( new_ins, _GetAttr( old_ins ) & OC_ATTR_SHORT );
        new_lbl = _Label( new_ins );
        old_lbl = _Label( old_ins );
        if( new_lbl->redirect == old_lbl ) {
            new_lbl->redirect = NULL;
            _ClrStatus( old_lbl, REDIRECTION );
        } else if( old_lbl->redirect == new_lbl ) {
            old_lbl->redirect = NULL;
            _ClrStatus( new_lbl, REDIRECTION );
        }
        for( ;; ) {
            old_lbl->ins = new_lbl->ins;
            old_lbl = old_lbl->alias;
            if( old_lbl == NULL ) {
                break;
            }
        }
        old_lbl = _Label( old_ins );
        owner = &old_lbl->refs;
        for( ;; ) {
            old_jmp = *owner;
            if( old_jmp == NULL )
                break;
            _Label( old_jmp ) = new_lbl;
            owner = (ins_entry **)&_LblRef( old_jmp );
        }
        *owner = new_lbl->refs;
        new_lbl->refs = old_lbl->refs;
        old_lbl->refs = NULL;
        while( new_lbl->alias != NULL ) {
            new_lbl = new_lbl->alias;
        }
        new_lbl->alias = old_lbl;
        _Label( old_ins ) = NULL;
        old_ins = DelInstr( old_ins );
    }
    optreturn( old_ins );
}


static  void    KillDeadLabels( ins_entry *instr )
/************************************************/
{
    label_handle    curr;
    label_handle    *owner;

  optbegin
    owner = &_Label( instr );
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
            break;
        _ValidLbl( curr );
        if( curr->refs == NULL
          && !_TstStatus( curr, REDIRECTION | KEEPLABEL )
          && _TstStatus( curr, DYINGLABEL ) ) {
            *owner = curr->alias;
            if( curr->redirect != NULL ) {
                _ClrStatus( curr->redirect, REDIRECTION );
                TryScrapLabel( curr->redirect );
                curr->redirect = NULL;
            }
            ScrapCodeLabel( curr );
        } else {
            owner = &curr->alias;
        }
    }
    if( _Label( instr ) == NULL ) {
        DelInstr( instr );
    }
  optend
}


void    ScrapCodeLabel( label_handle lbl )
/****************************************/
{
    label_handle    *owner;
    label_handle    redir;
    bool            code;

  optbegin
    owner = &Handles;
    for( ;; ) {
        if( *owner == NULL )
            optreturnvoid;
        if( *owner == lbl )
            break;
        owner = &(*owner)->lbl.link;
    }
    *owner = lbl->lbl.link;
    redir = lbl->redirect;
    code = _TstStatus( lbl, CODELABEL );
    CGFree( lbl );
    if( !code )
        optreturnvoid;
    if( redir == NULL )
        optreturnvoid;
    _ClrStatus( redir, REDIRECTION );
    TryScrapLabel( redir );
  optend
}


void    TryScrapLabel( label_handle old_lbl )
/*******************************************/
{
  optbegin
    if( old_lbl->refs != NULL )
        optreturnvoid;
    if( old_lbl->ins != NULL ) {
        KillDeadLabels( old_lbl->ins );
    } else if( _TstStatus( old_lbl, DYINGLABEL )
         && !_TstStatus( old_lbl, REDIRECTION | KEEPLABEL ) ) {
        ScrapCodeLabel( old_lbl );
    }
  optend
}
