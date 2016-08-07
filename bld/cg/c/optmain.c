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
* Description:  Instruction queue manipulation.
*
****************************************************************************/


#include "optwif.h"
#include "freelist.h"
#include "typedef.h"
#include "objout.h"
#include "inslist.h"
#include "blips.h"
#include "optmain.h"
#include "optutil.h"
#include "optmkins.h"


extern  void            OptPull( void );
extern  void            OptPush( void );
extern  pointer_int     MemInUse( void );

static  void    PullQueue( void )
/*******************************/
{
    label_handle    lbl;
    label_handle    next;
    oc_class        cl;
    ins_entry       *next_ins;
    label_handle    be_lbls;
    label_handle    *owner;

  optbegin
    for( ;; ) {
        PLBlip();
        OptPull();
        if( FirstIns == NULL )
            break;
        if( _Class( FirstIns ) == OC_LABEL ) {
            if( UniqueLabel( _Label( FirstIns ) ) ) {
                /* take off extra byte added when the ins went into queue */
                FirstIns->oc.oc_header.objlen--;
            }
            /*
                We want to order the alias list so that the labels with
                symbols dump out first. They might be FE_COMMON labels.
            */
            be_lbls = NULL;
            owner = &_Label( FirstIns );
            for( lbl = _Label( FirstIns ); lbl != NULL; lbl = next ) {
                next = lbl->alias;
                if( lbl->lbl.sym != NULL ) {
                    *owner = lbl;
                    owner = &lbl->alias;
                } else {
                    lbl->alias = be_lbls;
                    be_lbls = lbl;
                }
            }
            *owner = be_lbls;
        }
        for( ;; ) {
            next_ins = NextIns( FirstIns );
            if( next_ins != NULL && _Class( next_ins ) == OC_LABEL ) {
                OutputOC( &FirstIns->oc, &next_ins->oc );
            } else {
                OutputOC( &FirstIns->oc, NULL );
            }
            cl = _Class( FirstIns );
            if( cl != OC_LABEL )
                break;
            lbl = _Label( FirstIns );
            lbl->ins = NULL;
            next = lbl->alias;
            if( next == NULL )
                break;
            lbl->alias = NULL;
            TryScrapLabel( lbl );
            _Label( FirstIns ) = next;
        }
        DelInstr( FirstIns );
        FreePendingDeletes();
        /*
           Head of queue must be immediately after an unconditional control
           flow instruction since things may be bypassing the control
           flow optimizer.
        */
        if( _TransferClass( cl ) )
            break;
        if( FirstIns == NULL ) {
            break;
        }
    }
  optend
}


static  bool    LDone( any_oc *oc )
/*********************************/
{
    label_handle    lbl;

  optbegin
    if( oc->oc_header.class != OC_LDONE )
        optreturn( false );
    lbl = oc->oc_handle.handle;
    _ValidLbl( lbl );
    if( !_TstStatus( lbl, CODELABEL ) )
        optreturn( false );
    _SetStatus( lbl, DYINGLABEL );
    TryScrapLabel( lbl );
    optreturn( true );
}

void    InputOC( any_oc *oc )
/***************************/
{
  optbegin
    PSBlip();
    if( LDone( oc ) == false ) {
        if( (oc->oc_header.class & GET_BASE) != OC_INFO
         && (oc->oc_header.class & GET_BASE) != OC_LABEL
         && _TransferClass( PrevClass( NULL ) ) )
            optreturnvoid; /*dead code*/
        while( QCount >= Q_MAX ) {
            PullQueue();
        }
        AddInstr( NewInstr( oc ), LastIns );
        switch( _Class( LastIns ) ) {
        case OC_LABEL:
            if( _TstStatus( _Label( LastIns ), UNIQUE ) ) {
                /* Unique labels might need an addition byte of spacing */
                LastIns->oc.oc_header.objlen++;
            }
            /* fall through */
        case OC_LREF:
        case OC_JCOND:
        case OC_JMP:
        case OC_CALL:
            _ClrStatus( _Label( LastIns ), DYINGLABEL );
            break;
        }
        OptPush();
    }
  optend
}


bool    ShrinkQueue( pointer_int size )
/*************************************/
{
    pointer_int freed;
    pointer_int need;

  optbegin
    need = size;
    freed = 0;
    for( ;; ) {
        if( freed >= need )
            break;
        if( QCount <= Q_MIN )
            break;
        freed += MemInUse();
        PullQueue();
        freed -= MemInUse();
    }
    optreturn( freed >= need );
}


void    InitQueue( void )
/***********************/
{
  optbegin
    QCount = 0;
    FirstIns = NULL;
    LastIns = NULL;
    RetList = NULL;
    NoRetList = NULL;
    PendingDeletes = NULL;
    Handles = NULL;
    InitFrl( &InstrFrl );
#if( OPTIONS & SAVINGS )
    Savings = 0;
#endif
  optend
}


void    EmptyQueue( void )
/************************/
{
  optbegin
    while( QCount != 0 ) {
        PullQueue();
    }
  optend
}

void    FlushQueue( void )
/************************/
{
    segment_id      old;

  optbegin
    old = SetOP( AskCodeSeg() );
    EmptyQueue();
    SetOP( old );
  optend
}

void    FiniQueue( void )
/***********************/
{
  optbegin
    SetOP( AskCodeSeg() );
    EmptyQueue();
    _DumpSavings();
  optend
}


void    AbortQueue( void )
/************************/
{
    ins_entry   *next;

  optbegin
    for( ; FirstIns != NULL; FirstIns = next ) {
        next = FirstIns->ins.next;
        FreeInstr( FirstIns );
    }
  optend
}
