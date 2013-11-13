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
* Description:  Relative branch optimizations.
*
****************************************************************************/


#include "optwif.h"
#include "objout.h"

extern  void            TryScrapLabel(label_handle);
extern  byte            ReverseCondition(byte);
extern  void            AddNewJump(ins_entry*,label_handle);
extern  label_handle    AddNewLabel(ins_entry*,int);
extern  ins_entry       *PrevIns(ins_entry*);
extern  void            ChgLblRef(ins_entry*,label_handle);
extern  ins_entry       *NextIns(ins_entry*);
extern  int             OptInsSize(oc_class,oc_dest_attr);

static  label_handle    Handle;


static  bool    Jmp_to_lbl( ins_entry *instr )
/********************************************/
{
  optbegin
    if( _Class( instr ) != OC_JMP ) optreturn( FALSE );
    if( _Label( instr ) != Handle ) optreturn( FALSE );
    optreturn( TRUE );
}


static  bool    CanReach( label_handle lbl, ins_entry **add_ptr,
                                ins_entry **jmp_ptr )
/************************************************************
    Can a short branch at instruction 'FirstIns' reach label 'lbl'?
*/
{
    ins_entry   *add;
    ins_entry   *jmp;
    ins_entry   *instr;
    ins_entry   *lbl_ins;
    offset      obj_len;

    add = NULL;
    jmp = NULL;
    if( _TstStatus( lbl, UNREACHABLE ) ) {
        /* can't do anything with it */
    } else if( lbl->lbl.address == ADDR_UNKNOWN ) {
        if( _TstStatus( lbl, SHORTREACH ) ) return( TRUE );
        obj_len = OptInsSize( OC_JMP, OC_DEST_SHORT );
        lbl_ins = Handle->ins;
        instr = FirstIns;
        for(;;) {
            instr = NextIns( instr );
            if( instr == NULL ) break;
            obj_len += _ObjLen( instr );
            if( obj_len > MAX_SHORT_FWD ) break;
            if( Jmp_to_lbl( instr ) ) {
                jmp = instr;
            } else if( _TransferClass( _Class( instr ) ) ) {
                add = instr;
            }
            if( instr == lbl_ins ) return( TRUE );
        }
    } else if( (AskLocation() - lbl->lbl.address) <= MAX_SHORT_BWD ) {
        return( TRUE );
    }
    if( add_ptr != NULL ) *add_ptr = add;
    if( jmp_ptr != NULL ) *jmp_ptr = jmp;
    return( FALSE );
}


static  void    HndlRedirect( label_handle new )
/*******************************************/
{
    label_handle    redir;

  optbegin
    redir = Handle->redirect;
    if( redir != NULL && redir != new ) {
        _ClrStatus( redir, REDIRECTION );
        TryScrapLabel( redir );
    }
    Handle->redirect = new;
    if( new != NULL ) {
        _SetStatus( new, REDIRECTION );
    }
  optend
}


static  bool    InRange( void )
/*****************************/
/* Check if redirection is still within range*/
{
    label_handle    jmp_lbl;

  optbegin
    if( _IsModel( NO_OPTIMIZATION ) ) optreturn( FALSE );
    jmp_lbl = Handle->redirect;
    /* we don't have a redirection for this label*/
    if( jmp_lbl == NULL ) optreturn( FALSE );
    /* forward jump: must still be in range*/
    if( jmp_lbl->lbl.address == ADDR_UNKNOWN ) optreturn( TRUE );
    /* can't redirect the redirection jump to itself*/
    if( jmp_lbl->lbl.address == AskLocation() ) optreturn( FALSE );
    /* can still reach old redirection with short backward jump*/
    if( (AskLocation()-jmp_lbl->lbl.address)<=MAX_SHORT_BWD ) optreturn( TRUE );
    /* can't get at old redirection any more*/
    HndlRedirect( NULL );
    optreturn( FALSE );
}


static  void    BigBranch( ins_entry *add, ins_entry *jmp )
/**********************************************************
    We can't get to where we want to go, have to redirect
*/
{
  optbegin
    if( jmp != NULL && _IsntModel( NO_OPTIMIZATION ) ) {
        /* jump to a jump that's going where we want*/
        HndlRedirect( AddNewLabel( PrevIns( jmp ), 0 ) );
        ChgLblRef( FirstIns, Handle->redirect );
    } else if( InRange() ) {
        /* use old redirection label*/
        ChgLblRef( FirstIns, Handle->redirect );
    } else if( add != NULL && _IsntModel( NO_OPTIMIZATION ) ) {
        /* add a jump that's in range*/
        AddNewJump( add, Handle );
        _Savings( OPT_JUMPS, -_ObjLen( NextIns( add ) ) );
        HndlRedirect( AddNewLabel( add, 0 ) );
        ChgLblRef( FirstIns, Handle->redirect );
    } else {
        /* reverse the branch*/
        _JmpCond( FirstIns ) = ReverseCondition( _JmpCond( FirstIns ) );
        AddNewJump( FirstIns, Handle );
        ChgLblRef( FirstIns, AddNewLabel( NextIns( FirstIns ), 0 ) );
        HndlRedirect( AddNewLabel( FirstIns, 0 ) );
    }
  optend
}


static  void    SetShort( void )
/******************************/
{
    ins_entry   *l_ins;
    int         size;
    bool        floating;

  optbegin
    floating = FALSE;
    if( _Attr( FirstIns ) & ATTR_FLOAT ) {
        floating = TRUE;
    }
    size = OptInsSize( _Class( FirstIns ), OC_DEST_SHORT );
    if( _Class( FirstIns ) == OC_JMP ) {
        _Savings( OPT_JUMPS, _ObjLen( FirstIns ) - size );
        _ObjLen( FirstIns ) = size;
        _SetClass( FirstIns, OC_JMP );
        if( floating ) {
            _SetAttr( FirstIns, ATTR_FLOAT );
        }
    } else {
        _Savings( OPT_JCONDS, _ObjLen( FirstIns ) - size );
        _ObjLen( FirstIns ) = size;
        _SetClass( FirstIns, OC_JCOND );
        if( floating ) {
            _SetAttr( FirstIns, ATTR_FLOAT );
        }
    }
    l_ins = _Label( FirstIns )->ins;
    if( l_ins != NULL ) {
        _SetAttr( l_ins, ATTR_SHORT );
    }
  optend
}


extern  void    SetBranches( void )
/*********************************/
/* Check whether the actual label itself can be targeted*/
{
  ins_entry     *add;
  ins_entry     *jmp;
  ins_entry     *next;
  bool          was_keep;

  optbegin
    Handle = _Label( FirstIns );
    if( CanReach( Handle, &add, &jmp ) ) {
        _SetStatus( Handle, SHORTREACH );
        HndlRedirect( NULL );
        SetShort();
    } else {
        if( _Attr( FirstIns ) & ATTR_SHORT ) {
            /* HAS to be a short branch */
            next = NextIns( FirstIns );
            if( _Class(next) == OC_JMP && CanReach(_Label(next), NULL, NULL) ) {
                /* we have:
                        Jcond   L1
                        JMP     L2
                   and L2 is reachable, but L1 is not.
                   Turn it into:
                        J~cond  L2
                        JMP     L1
                */
                _JmpCond( FirstIns ) =
                                ReverseCondition( _JmpCond( FirstIns ) );
                was_keep = ( _TstStatus( Handle, KEEPLABEL ) != 0 );
                _SetStatus( Handle, KEEPLABEL );
                ChgLblRef( FirstIns, _Label( next ) );
                ChgLblRef( next, Handle );
                if( !was_keep ) _ClrStatus( Handle, KEEPLABEL );
            } else {
                BigBranch( add, jmp );
            }
            SetShort();
        } else if( OptForSize > 50 && InRange() ) {
            /* use an old redirection label*/
            ChgLblRef( FirstIns, Handle->redirect );
            SetShort();
        }
    }
  optend
}
