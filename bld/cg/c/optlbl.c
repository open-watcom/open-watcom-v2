/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "optutil.h"


void    AddLblDef( ins_entry *instr )
/***********************************/
{
    label_handle    lbl;

  optbegin
    _LblRef( instr ) = NULL;
    lbl = _Label( instr );
    for(;;) {
        _SetStatus( lbl, CODELABEL );
        lbl->ins = instr;
        lbl = lbl->alias;
        if( lbl == NULL ) break;
    }
  optend
}


void    DelLblDef( ins_entry *instr )
/***********************************/
{
    label_handle    lbl;

  optbegin
    lbl = _Label( instr );
    if( lbl == NULL ) optreturnvoid;
    if( lbl->refs != NULL ) optreturnvoid;
    if( !_TstStatus( lbl, DYINGLABEL ) ) optreturnvoid;
    if( _TstStatus( lbl, REDIRECTION ) ) optreturnvoid;
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
    label_handle    old;

  optbegin
    old = _Label( instr );
    DelRef( &old->refs, instr );
    TryScrapLabel( old );
  optend
}


void    ChgLblRef( ins_entry *instr, label_handle new )
/*****************************************************/
{
    ins_entry       **owner;
    ins_entry       *curr;
    label_handle    old;

  optbegin
    old = _Label( instr );
    if( old != new ) {
        owner = &old->refs;
        for(;;) {
            curr = *owner;
            if( curr == instr ) break;
            owner = (ins_entry **)&_LblRef( curr );
        }
        *owner = _LblRef( curr );
        _LblRef( curr ) = new->refs;
        new->refs = curr;
        _Label( curr ) = new;
        TryScrapLabel( old );
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


ins_entry       *AliasLabels( ins_entry *oldlbl, ins_entry *newlbl )
/******************************************************************/
{
    label_handle    old;
    label_handle    new;
    ins_entry       *old_jmp;
    ins_entry       **owner;

  optbegin
    if( oldlbl != newlbl ) {
        /* use maximum of the two aligment requests */
        if( _ObjLen( oldlbl ) > _ObjLen( newlbl ) ) {
            _ObjLen( newlbl ) = _ObjLen( oldlbl );
        }
#if _TARGET & _TARG_RISC
        /* one of the line numbers (at most) is non-zero - keep it */
        if( _LblLine( oldlbl ) != 0 ) {
            _LblLine( newlbl ) = _LblLine( oldlbl );
        }
#endif
        _SetAttr( newlbl, _Attr( oldlbl ) & ATTR_SHORT );
        new = _Label( newlbl );
        old = _Label( oldlbl );
        if( new->redirect == old ) {
            new->redirect = NULL;
            _ClrStatus( old, REDIRECTION );
        } else if( old->redirect == new ) {
            old->redirect = NULL;
            _ClrStatus( new, REDIRECTION );
        }
        for(;;) {
            old->ins = new->ins;
            old = old->alias;
            if( old == NULL ) break;
        }
        old = _Label( oldlbl );
        owner = &old->refs;
        for(;;) {
            old_jmp = *owner;
            if( old_jmp == NULL ) break;
            _Label( old_jmp ) = new;
            owner = (ins_entry **)&_LblRef( old_jmp );
        }
        *owner = new->refs;
        new->refs = old->refs;
        old->refs = NULL;
        while( new->alias != NULL ) {
            new = new->alias;
        }
        new->alias = old;
        _Label( oldlbl ) = NULL;
        oldlbl = DelInstr( oldlbl );
    }
    optreturn( oldlbl );
}


static  void    KillDeadLabels( ins_entry *instr )
/************************************************/
{
    label_handle    curr;
    label_handle    *owner;

  optbegin
    owner = &_Label( instr );
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
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
    for(;;) {
        if( *owner == NULL ) optreturnvoid;
        if( *owner == lbl ) break;
        owner = &(*owner)->lbl.link;
    }
    *owner = lbl->lbl.link;
    redir = lbl->redirect;
    code = _TstStatus( lbl, CODELABEL );
    CGFree( lbl );
    if( !code ) optreturnvoid;
    if( redir == NULL ) optreturnvoid;
    _ClrStatus( redir, REDIRECTION );
    TryScrapLabel( redir );
  optend
}


void    TryScrapLabel( label_handle old )
/***************************************/
{
  optbegin
    if( old->refs != NULL ) optreturnvoid;
    if( old->ins != NULL ) {
        KillDeadLabels( old->ins );
    } else if( _TstStatus( old, DYINGLABEL )
         && !_TstStatus( old, REDIRECTION | KEEPLABEL ) ) {
        ScrapCodeLabel( old );
    }
  optend
}
