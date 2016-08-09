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
#include "zoiks.h"
#include "inslist.h"
#include "object.h"
#include "opttell.h"
#include "encode.h"


void    TellOptimizerByPassed( void )
/***********************************/
/* tell the optimizer we're going around it so it won't flush while we're*/
/* in the object file generator and zap our static variables*/
{
  optbegin
}


void    TellByPassOver( void )
/****************************/
{
  optend
}


void    TellAddress( label_handle lbl, offset addr )
/**************************************************/
{
  optbegin
    _ValidLbl( lbl );
    lbl->lbl.address = addr;
  optend
}


void    TellDonePatch( label_handle lbl )
/***************************************/
{
  optbegin
    _ValidLbl( lbl );
    lbl->lbl.patch = NULL;
  optend
}


void    TellReachedLabel( label_handle lbl )
/******************************************/
{
  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, REACHED );
  optend
}

void    TellProcLabel( label_handle lbl )
/***************************************/
{
    _ValidLbl( lbl );
    _SetStatus( lbl, PROCEDURE );
}

void    TellCommonLabel( label_handle lbl, import_handle imphdl )
/***************************************************************/
{
  optbegin
    _ValidLbl( lbl );
    lbl->lbl.sym = IMPHDL2SYM( imphdl );
    _SetStatus( lbl, COMMON_LBL );
  optend
}


void    TellKeepLabel( label_handle lbl )
/***************************************/
{
  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, KEEPLABEL );
  optend
}


void    TellNoSymbol( label_handle lbl )
/**************************************/
{
  optbegin
    if( lbl != NULL ) {
        _ValidLbl( lbl );
        lbl->lbl.sym = NULL;
        if( _TstStatus( lbl, CODELABEL ) ) {
            GenKillLabel( lbl );
        }
    }
  optend
}

static  void    ReallyScrapLabel( label_handle lbl )
/**************************************************/
{
    label_handle *owner;

  optbegin
    owner = &Handles;
    for(;;) {
        if( *owner == NULL ) optreturnvoid;
        if( *owner == lbl ) break;
        owner = &(*owner)->lbl.link;
    }
    *owner = lbl->lbl.link;
    CGFree( lbl );
  optend
}


void    TellScrapLabel( label_handle lbl )
/****************************************/
{
  optbegin
    /* data labels in the code segment can't get freed until the end */
    if( _TstStatus( lbl, CODELABEL ) ) {
        ReallyScrapLabel( lbl );
    }
  optend
}


void    TellCondemnedLabel( label_handle lbl )
/********************************************/
{
  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, CONDEMNED );
  optend
}


static  label_handle    NextCondemned( label_handle lbl )
/*******************************************************/
{
  optbegin
    for( ;; ) {
        if( lbl == NULL ) break;
        if( _TstStatus( lbl, CONDEMNED ) ) {
            _ClrStatus( lbl, CONDEMNED );
            break;
        }
        lbl = lbl->lbl.link;
    }
    optreturn( lbl );
}


void    TellBeginExecutions( void )
/*********************************/
{
    label_handle    dead;

  optbegin
    for(;;) {
        dead = NextCondemned( Handles );
        if( dead == NULL ) break;
        GenKillLabel( dead );
    }
  optend
}


void    TellFreeAllLabels( void )
/*******************************/
{
    bool        unfreed;

  optbegin
    unfreed = false;
    while( Handles != NULL ) {
#ifndef PRODUCTION
        if( _TstStatus( Handles, CODELABEL )
         && Handles->lbl.sym == NULL
         && !_TstStatus( Handles, REDIRECTION )
         && unfreed == false ) {
            _Zoiks( ZOIKS_001 );
            unfreed = true;
        }
#endif
        ReallyScrapLabel( Handles );
    }
  optend
}


void    TellUnreachLabels( void )
/********************************
    Mark all the labels that have come out of the control flow queue as
    unreachable by a short jump.
*/
{
    label_handle    lbl;

  optbegin
    for( lbl = Handles; lbl != NULL; lbl = lbl->lbl.link ) {
        if( lbl->lbl.address != ADDR_UNKNOWN ) {
            _SetStatus( lbl, UNREACHABLE );
        }
    }
  optend
}

void    KillLblRedirects( void )
/******************************/
{
    label_handle    lbl;

  optbegin
#if  OPTIONS & SHORT_JUMPS
    for( lbl = Handles; lbl != NULL; lbl = lbl->lbl.link ) {
        lbl->redirect = NULL;
    }
#endif
    TellUnreachLabels();        // a little too conservative - stop gap
  optend
}
