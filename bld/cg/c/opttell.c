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

extern    code_lbl     *Handles;

extern  void            GenKillLabel(pointer);


extern  void    TellOptimizerByPassed() {
/***************************************/

/* tell the optimizer we're going around it so it won't flush while we're*/
/* in the object file generator and zap our static variables*/

  optbegin
}


extern  void    TellByPassOver() {
/********************************/

  optend


extern  void    TellAddress( code_lbl *lbl, offset addr ) {
/**********************************************************/

  optbegin
    _ValidLbl( lbl );
    lbl->lbl.address = addr;
  optend


extern  void    TellDonePatch( code_lbl *lbl ) {
/***********************************************/

  optbegin
    _ValidLbl( lbl );
    lbl->lbl.patch = NULL;
  optend


extern  void    TellReachedLabel( code_lbl *lbl ) {
/*************************************************/

  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, REACHED );
  optend

extern  void    TellProcLabel( code_lbl *lbl ) {
/**********************************************/

    _ValidLbl( lbl );
    _SetStatus( lbl, PROC );
}

extern  void    TellCommonLabel( code_lbl *lbl, unsigned hdl ) {
/**************************************************************/

  optbegin
    _ValidLbl( lbl );
    lbl->lbl.sym = (sym_handle) hdl;
    _SetStatus( lbl, COMMON_LBL );
  optend


extern  void    TellKeepLabel( code_lbl *lbl ) {
/***********************************************/

  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, KEEPLABEL );
  optend


extern  void    TellNoSymbol( code_lbl *lbl ) {
/**********************************************/

  optbegin
    if( lbl != NULL ) {
        _ValidLbl( lbl );
        lbl->lbl.sym = NULL;
        if( _TstStatus( lbl, CODELABEL ) ) {
            GenKillLabel( lbl );
        }
    }
  optend

static  void    ReallyScrapLabel( code_lbl *lbl ) {
/************************************************/

    code_lbl   **owner;

  optbegin
    owner = &Handles;
    for(;;) {
        if( *owner == NULL ) optreturnvoid;
        if( *owner == lbl ) break;
        owner = &(*owner)->lbl.link;
    }
    *owner = lbl->lbl.link;
    _Free( lbl, sizeof( code_lbl ) );
  optend


extern  void    TellScrapLabel( code_lbl *lbl ) {
/************************************************/

  optbegin
    /* data labels in the code segment can't get freed until the end */
    if( _TstStatus( lbl, CODELABEL ) ) {
        ReallyScrapLabel( lbl );
    }
  optend


extern  void    TellCondemnedLabel( code_lbl *lbl ) {
/****************************************************/

  optbegin
    _ValidLbl( lbl );
    _SetStatus( lbl, CONDEMNED );
  optend


static  code_lbl       *NextCondemned( code_lbl *lbl ) {
/********************************************************/

  optbegin
    for(;;) {
        if( lbl == NULL ) break;
        if( _TstStatus( lbl, CONDEMNED ) ) {
            _ClrStatus( lbl, CONDEMNED );
            break;
        }
        lbl = lbl->lbl.link;
    }
    optreturn( lbl );
}


extern  void    TellBeginExecutions() {
/*************************************/

    code_lbl   *dead;

  optbegin
    for(;;) {
        dead = NextCondemned( Handles );
        if( dead == NULL ) break;
        GenKillLabel( dead );
    }
  optend


extern  void    TellFreeAllLabels() {
/***********************************/

    bool        unfreed;

  optbegin
    unfreed = FALSE;
    while( Handles != NULL ) {
#ifndef PRODUCTION
        if( _TstStatus( Handles, CODELABEL )
         && Handles->lbl.sym == NULL
         && _TstStatus( Handles, REDIRECTION ) == FALSE
         && unfreed == FALSE ) {
            _Zoiks( ZOIKS_001 );
            unfreed = TRUE;
        }
#endif
        ReallyScrapLabel( Handles );
    }
  optend


extern  void    TellUnreachLabels() {
/************************************
    Mark all the labels that have come out of the control flow queue as
    unreachable by a short jump.
*/

  code_lbl      *lbl;

  optbegin
    for( lbl = Handles; lbl != NULL; lbl = lbl->lbl.link ) {
        if( lbl->lbl.address != ADDR_UNKNOWN ) {
            _SetStatus( lbl, UNREACHABLE );
        }
    }
  optend

extern  void    KillLblRedirects() {
/***********************************

*/

  code_lbl      *lbl;

  optbegin
#if  OPTIONS & SHORT_JUMPS
    for( lbl = Handles; lbl != NULL; lbl = lbl->lbl.link ) {
        lbl->redirect = NULL;
    }
#endif
    TellUnreachLabels();        // a little too conservative - stop gap
  optend

