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

extern  ins_entry       *ValidIns( ins_entry * );
extern  ins_entry       *AliasLabels( ins_entry *, ins_entry * );
extern  oc_class        PrevClass( ins_entry * );
extern  oc_class        NextClass( ins_entry * );
extern  void            ChgLblRef( ins_entry *, code_lbl * );
extern  byte            ReverseCondition( byte );
extern  ins_entry       *DelInstr( ins_entry * );
extern  void            SetBranches( void );
extern  ins_entry       *NextIns( ins_entry * );
extern  bool            StraightenCode( ins_entry * );
extern  void            CheckStraightenCode( ins_entry * );
extern  ins_entry       *IsolatedCode( ins_entry * );
extern  void            FreePendingDeletes( void );
extern  bool            ComTail( ins_entry *, ins_entry * );
extern  void            RetAftrCall( ins_entry * );
extern  bool            RetAftrLbl( ins_entry * );
extern  void            JmpRet( ins_entry * );
extern  bool            ComCode( ins_entry * );
extern  ins_entry       *PrevIns( ins_entry * );
extern  void            CallRet( ins_entry * );
extern  void            TraceCommon( ins_entry * );
extern  void            MultiLineNums( ins_entry * );
extern  void            CloneCode( code_lbl* );
extern  bool            UniqueLabel( code_lbl* );

static  ins_entry       *Redirect( ins_entry *, ins_entry * );


static  bool    LineLabel( ins_entry *label )
/*******************************************/
{
#if _TARGET & _TARG_RISC
    if( _LblLine( label ) != 0 ) {
        return( TRUE );
    }
#else
    label = label;
#endif
    return( FALSE );
}


static  bool    CompressLabels( ins_entry *label )
/************************************************/
{
    ins_entry   *other_label;
    bool        lbl_unique;

  optbegin
    lbl_unique = UniqueLabel( _Label( label ) );
    for( ;; ) {
        if( PrevClass( label ) != OC_LABEL )
            break;
        other_label = PrevIns( label );
        if( lbl_unique && UniqueLabel( _Label( other_label ) ) )
            break;
        if( LineLabel( label ) && LineLabel( other_label ) )
            break;
        AliasLabels( other_label, label );
        if( _Class( label ) == OC_DEAD ) {
            optreturn( FALSE );
        }
    }
    for( ;; ) {
        if( NextClass( label ) != OC_LABEL )
            break;
        other_label = NextIns( label );
        if( lbl_unique && UniqueLabel( _Label( other_label ) ) )
            break;
        if( LineLabel( label ) && LineLabel( other_label ) )
            break;
        AliasLabels( other_label, label );
        if( _Class( label ) == OC_DEAD ) {
            optreturn( FALSE );
        }
    }
    optreturn( TRUE );
}


static  bool    UnTangle1( ins_entry *jmp, ins_entry **instr )
/************************************************************/
{
    ins_entry   *c_jmp;
    oc_class    cl;

    if( jmp == NULL )
        return( FALSE );
    cl = _Class( jmp );
    if( (cl != OC_JCOND) && (cl != OC_JMP) )
        return( FALSE );
    if( _Label( jmp ) == _Label( *instr ) ) {
        /* jump next*/
        *instr = DelInstr( jmp );
        return( TRUE );
    }
    if( cl != OC_JMP )
        return( FALSE );
    c_jmp = PrevIns( jmp );
    if( c_jmp == NULL )
        return( FALSE );
    if( _Class( c_jmp ) != OC_JCOND )
        return( FALSE );
#if( OPTIONS & SEGMENTED )
    if( _Label( c_jmp ) == _Label( *instr ) && ( _Attr( jmp ) & ATTR_FAR ) == 0 ) {
#else
    if( _Label( c_jmp ) == _Label( *instr ) ) {
#endif
        /* conditional jump around a jump*/
        _JmpCond( c_jmp ) = ReverseCondition( _JmpCond( c_jmp ) );
        cl = jmp->oc.oc_header.class;
        jmp->oc.oc_header.class = OC_DEAD_JMP; /* to stop dead code removal*/
        ChgLblRef( c_jmp, _Label( jmp ) );
        jmp->oc.oc_header.class = cl;
        *instr = DelInstr( jmp );
        return( TRUE );
    } else if( _Label( c_jmp ) == _Label( jmp ) ) {
        /* conditional jump followed by a jump to the same label*/
        DelInstr( c_jmp );
        return( TRUE );
    }
    return( FALSE );
}


static  bool    UnTangle2( ins_entry *jmp, ins_entry **instr )
/************************************************************/
{
    oc_class    cl;
    ins_entry   *ins;

    if( _IsModel( NO_OPTIMIZATION ) )
        return( FALSE );
    if( jmp == NULL )
        return( FALSE );
    cl = _Class( jmp );
    if( cl == OC_RET ) {
        /* label followed by a return*/
        return( RetAftrLbl( jmp ) );
    }
    if( cl != OC_JMP )
        return( FALSE );
#if( OPTIONS & SEGMENTED )
    if( _Attr( jmp ) & ATTR_FAR )
        return( FALSE );
#endif
    if( _Label( *instr )->ins == NULL )
        return( FALSE );
    if( _Label( jmp )->redirect == _Label( *instr ) )
        return( FALSE );
    if( _Label( jmp ) == _Label( *instr ) )
        return( FALSE );
    /* jump to jump*/
    *instr = Redirect( *instr, jmp );
    if( *instr == NULL )
        return( FALSE );
    cl = PrevClass( *instr );
    if( !_TransferClass( cl ) )
        return( TRUE );
    /* dead code*/
    ins = *instr;
    for(;;) {
        if( ins == NULL )
            break;
        if( _Class( ins ) == OC_LABEL )
            break;
        if( _Class( ins ) == OC_INFO ) {
            ins = ins->ins.next;
        } else {
            ins = DelInstr( ins );
        }
    }
    *instr = ins;
    return( TRUE );
}

extern  ins_entry       *Untangle( ins_entry *instr )
/***************************************************/
{
    ins_entry   *jmp;
    bool        change;

  optbegin
    for( ;; ) {
        if( instr == NULL )
            break;
        if( _Class( instr ) != OC_LABEL )
            break;
        if( !CompressLabels( instr ) )
            break;
        change = FALSE;
        jmp = PrevIns( instr );
        change |= UnTangle1( jmp, &instr );
        if( instr == NULL )
            break;
        if( _Class( instr ) != OC_LABEL )
            break;
        if( !CompressLabels( instr ) )
            break;
        jmp = NextIns( instr );
        change |= UnTangle2( jmp, &instr );
        if( !change ) {
            break;
        }
    }
    optreturn( instr );
}


static  ins_entry       *Redirect( ins_entry *l_ins, ins_entry *j_ins )
/*********************************************************************/
// Redirect all refs to l_ins to the target of j_ins
{
    ins_entry   *ref;
    code_lbl    *new;
    ins_entry   *ins;
    ins_entry   *new_ins;

  optbegin
    new = _Label( j_ins );
    new_ins = new->ins;
    if( UniqueLabel( _Label( l_ins ) ) && UniqueLabel( new ) ) {
        optreturn( NULL );
    }
    ref = _Label( l_ins )->refs;
    while( ref != NULL ) {
        ins = ref;
        ref = _LblRef( ref );
        ChgLblRef( ins, new );
    }
    if( new_ins != NULL && _Class( new_ins ) != OC_DEAD ) {
        Untangle( new_ins );
    }
    if( _Class( l_ins ) == OC_DEAD ) {
        /* got deleted*/
        optreturn( ValidIns( l_ins ) );
    }
    if( new_ins == NULL
     || _Class( new_ins ) == OC_DEAD
     || ( _Attr( l_ins ) & ATTR_SHORT )
     || _TstStatus( _Label( l_ins ), REDIRECTION ) ) {
         optreturn( NextIns( l_ins ) );
    } else {
         optreturn(  AliasLabels( l_ins, new->ins ) );
    }
}


extern  void    OptPush( void )
/*****************************/
{
    ins_entry   *ins;

  optbegin
    ins = LastIns;
    for( ;; ) {
        InsDelete = FALSE;
        switch( _Class( ins ) ) {
        case OC_INFO:
            MultiLineNums( ins );
            break;
        case OC_LABEL:
            Untangle( ins );
            if( !InsDelete ) {
                TraceCommon( ins );
            }
            break;
        case OC_CALL:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                CallRet( ins );
            }
            break;
        case OC_JMP:
            Untangle( PrevIns( ins ) );
            if( !InsDelete ) {
                ComCode( ins );
                JmpRet( ins );
            }
            break;
        case OC_RET:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                RetAftrLbl( ins );
                RetAftrCall( ins );
            }
            if( !InsDelete ) {
                ComTail( RetList, ins );
            }
            break;
        }
        if( !InsDelete )
            break;      /* nothing happened*/
        FreePendingDeletes();
        /* find the last interesting queue entry and try some more*/
        ins = LastIns;
        for(;;) {
            if( ins == NULL )
                optreturnvoid;
            if( _Class( ins ) != OC_INFO )
                break;
            ins = ins->ins.prev;
        }
    }
  optend
}


extern  void    OptPull( void )
/*****************************/
{
    oc_class    ins_class;

  optbegin
    for( ;; ) {
        InsDelete = FALSE;
        ins_class = _Class( FirstIns );
        switch( ins_class ) {
        case OC_LABEL:
            Untangle( FirstIns );
            if( _IsntModel( NO_OPTIMIZATION ) && !InsDelete ) {
                CloneCode( _Label( FirstIns ) );
            }
            break;
        case OC_CALL:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                CallRet( FirstIns );
            }
            break;
        case OC_JMP:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                IsolatedCode( FirstIns );
                if( !InsDelete ) {
                    StraightenCode( FirstIns );
                    JmpRet( FirstIns );
                    if( !InsDelete ) {
                        CheckStraightenCode( NextIns( FirstIns ) );
                        if( !InsDelete ) {
                            CloneCode( _Label( FirstIns ) );
                        }
                    }
                }
            }
            break;
        case OC_JMPI:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                IsolatedCode( FirstIns );
                if( !InsDelete ) {
                    CheckStraightenCode( NextIns( FirstIns ) );
                }
            }
            break;
        case OC_JCOND:
            Untangle( NextIns( FirstIns ) );
            break;
        case OC_RET:
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                IsolatedCode( FirstIns );
                if( !InsDelete ) {
                    CheckStraightenCode( NextIns( FirstIns ) );
                }
            }
            break;
        }
        FreePendingDeletes();
        if( FirstIns == NULL )
            optreturnvoid;
        if( !InsDelete ) {
            break;
        }
    }
    switch( ins_class ) {
    case OC_LREF:
    case OC_CALL:
        _SetStatus( _Label( FirstIns ), KEEPLABEL );
        break;
    case OC_JCOND:
    case OC_JMP:
#if( OPTIONS & SHORT_JUMPS )
        if( !(_Attr( FirstIns ) & ATTR_FAR) ) {
            SetBranches();
        }
#endif
        _SetStatus( _Label( FirstIns ), KEEPLABEL );
        break;
    }
  optend
}
