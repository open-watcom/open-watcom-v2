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
#include "escape.h"
#include "inslist.h"
#include "block.h"
#include "data.h"
#include "optmain.h"

extern  void            JmpToRet( ins_entry *, ins_entry * );
extern  oc_class        NextClass( ins_entry * );
extern  ins_entry       *DelInstr( ins_entry * );
extern  ins_entry       *Untangle( ins_entry * );
extern  void            ChgLblRef( ins_entry *, label_handle );
extern  label_handle    AddNewLabel( ins_entry *, obj_length );
extern  ins_entry       *NextIns( ins_entry * );
extern  ins_entry       *PrevIns( ins_entry * );
extern  oc_class        PrevClass( ins_entry * );
extern  void            AddInstr( ins_entry *, ins_entry * );
extern  ins_entry       *NewInstr( any_oc * );


extern  bool    FindShort( ins_entry *ins, ins_entry *end )
/*********************************************************/
{
    for( ; ins != NULL && ins != end; ins = NextIns( ins ) ) {
        if( _Class( ins ) == OC_LABEL ) {
            if( _Attr( ins ) & ATTR_SHORT )
                return( true );
            _ClrStatus( _Label( ins ), SHORTREACH );
        }
    }
    return( false );
}

static  void    DoCloneCode( ins_entry *jmp, ins_entry *hoist )
/*************************************************************/
{
    ins_entry   *clone_point;
    ins_entry   *clone;
    oc_class    cl;

    clone_point = PrevIns( jmp );
    for( ;; ) {
        cl = _Class( hoist );
        switch( cl ) {
        case OC_LABEL:
        case OC_INFO:
        case OC_LREF:
            break;
        default:
            clone = NewInstr( &hoist->oc );
            AddInstr( clone, clone_point );
            if( _TransferClass( cl ) )
                return;
            clone_point = clone;
            break;
        }
        hoist = NextIns( hoist );
    }
}

#define MAX_CLONE_SIZE  40

extern  void    CloneCode( label_handle lbl )
/*****************************************
    consider:

        JMP L1
            ....
  L1:   POP   AX
        RET

    we want to clone the code at L1 at the point of the jump,
    since it will be faster (and smaller since POP AX/RET is only 2 bytes
    while the JMP might be 3/5).
*/
{
    ins_entry   *next;
    ins_entry   *lbl_ins;
    ins_entry   *hoist;
    ins_entry   *jmp;
    unsigned    size;
    unsigned    max_size;

    lbl_ins = lbl->ins;
    if( lbl_ins == NULL )
        return;
    size = 0;
    hoist = NextIns( lbl_ins );
    for( next = hoist; ; next = NextIns( next ) ) {
        if( next == NULL )
            return;
        if( _Class( next ) == OC_CODE && CodeHasAbsPatch( &next->oc.oc_entry ) )
            return;
        if( _Class( next ) != OC_LABEL ) {
            size += _ObjLen( next );
            if( size > MAX_CLONE_SIZE )
                return;
            if( _TransferClass( _Class( next ) ) ) {
                break;
            }
        }
    }
    if( _Class( next ) == OC_JMP && _Label( next ) == lbl )
        return;
    for( jmp = lbl->refs; jmp != NULL; jmp = _LblRef( jmp ) ) {
        if( next == jmp )
            continue;
        if( !_TransferClass( _Class( jmp ) ) )
            continue;
        max_size = _ObjLen( jmp );
        if( size > max_size && FindShort( jmp, NULL ) )
            continue;
        if( OptForSize < 50 ) {
            max_size *= (100-OptForSize) / 25;
        }
        if( size > max_size )
            continue;
        DoCloneCode( jmp, hoist );
        DelInstr( jmp );
    }
}


extern  ins_entry       *IsolatedCode( ins_entry *instr )
/*******************************************************/
{
    ins_entry   *next;

  optbegin
    next = NextIns( instr );
    for( ;; ) {
        if( next == NULL )
            optreturn( next );
        if( _Class( next ) == OC_LABEL )
            break;
        _Savings( OPT_ISOLATED, _ObjLen( next ) );
        if( _Class( next ) == OC_INFO ) {
            next = next->ins.next;
        } else {
            next = DelInstr( next );
        }
        if( _Class( instr ) == OC_DEAD ) {
            break;
        }
    }
    next = Untangle( next );
    optreturn( next );
}


extern  bool    StraightenCode( ins_entry *jump )
/***********************************************/
{
    ins_entry   *next;
    ins_entry   *insert;
    ins_entry   *hoist;
    ins_entry   *end_hoist;
    oc_class    cl;
    obj_length  align;

  optbegin
    hoist = _Label( jump )->ins;
    if( hoist == NULL )
        optreturn( false );
    if( hoist == LastIns )
        optreturn( false );
    cl = PrevClass( hoist );
    if( !_TransferClass( cl ) )
        optreturn( false );

    end_hoist = NULL;
    for( next = hoist; ; next = NextIns( next ) ) {
        if( next == jump ) { // pushing code down to jump
            if( end_hoist == NULL )
                optreturn( false );
            if( FindShort( hoist, end_hoist ) )
                optreturn( false );
            break;
        }
        if( next == NULL ) { // hauling code up to jump
            if( FindShort( jump, hoist ) )
                optreturn( false );
            break;
        }
        cl = _Class( next );
        if( end_hoist == NULL && _TransferClass( cl ) ) {
            end_hoist = next;
        }
    }

    align = _ObjLen( hoist );
    insert = jump;
    for( ;; ) {
        if( hoist == NULL ) {
            ChgLblRef( jump, AddNewLabel( LastIns, align ) );
            next = LastIns;
            break;
        }
        next = NextIns( hoist );
        DeleteQueue( hoist );
        InsertQueue( hoist, insert );
        if( hoist == jump )
            optreturn( false );
        insert = hoist;
        cl = _Class( hoist );
        if( _TransferClass( cl ) ) {
            IsolatedCode( insert );
            break;
        }
        hoist = next;
    }
    InsDelete = true;
    Untangle( next );
    if( _Class( jump ) != OC_DEAD ) {
        Untangle( _Label( jump )->ins );
    }
    optreturn( true );
}


extern  void    CheckStraightenCode( ins_entry  *lbl_ins )
/********************************************************/
{
    label_handle    lbl;
    ins_entry       *jmp;

  optbegin
    if( lbl_ins != NULL ) {
        lbl = _Label( lbl_ins );
        for( jmp = lbl->refs; jmp != NULL; jmp = _LblRef( jmp ) ) {
            if( !_TransferClass( _Class( jmp ) ) )
                continue;
            if( StraightenCode( jmp ) ) {
                break;
            }
        }
    }
  optend
}


extern  void    CallRet( ins_entry *instr )
/*****************************************/
{
    ins_entry   *lbl;

  optbegin
    if( _Attr( instr ) & ATTR_POP )
        optreturnvoid;
    lbl = _Label( instr )->ins;
    if( lbl == NULL )
        optreturnvoid;
    if( NextClass( lbl ) != OC_RET )
        optreturnvoid;
    _Savings( OPT_CALLTORET, _ObjLen( instr ) );
    DelInstr( instr );
  optend
}


extern  void    JmpRet( ins_entry *instr )
/****************************************/
{
    ins_entry   *ret;

  optbegin
    if( InsDelete )
        optreturnvoid;
    ret = _Label( instr )->ins;
    if( ret == NULL )
        optreturnvoid;
    ret = NextIns( ret );
    if( ret == NULL )
        optreturnvoid;
    if( _Class( ret ) != OC_RET )
        optreturnvoid;
    JmpToRet( instr, ret );
  optend
}
