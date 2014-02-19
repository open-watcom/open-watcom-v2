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
#include "targsys.h"
#include "typedef.h"
#include "objout.h"
#include "feprotos.h"


static  label_handle DoAskForLabel( cg_sym_handle sym )
/**************************************************/
{
    label_handle    new;

  optbegin
    new = CGAlloc( sizeof( code_lbl ) );
    new->lbl.link    = Handles;
    Handles = new;
    new->lbl.sym     = sym;
    new->lbl.address = ADDR_UNKNOWN;
    new->lbl.patch   = NULL;
    new->lbl.status  = 0;
    new->alias       = NULL;
    new->ins         = NULL;
    new->refs        = NULL;
#if( OPTIONS & SHORT_JUMPS )
    new->redirect    = NULL;
#endif
#if _TARGET & _TARG_RISC
    new->ppc_alt_name = NULL;
    new->owl_symbol   = NULL;
#endif
#ifndef NDEBUG
    new->useinfo.hdltype = NO_HANDLE;
    new->useinfo.used = FALSE;
#endif
    optreturn( new );
}


extern  label_handle AskForNewLabel( void )
/*****************************************/
{
    label_handle    new;

  optbegin
    new = DoAskForLabel( NULL );
    /* the DYINGLABEL bit will cause unused labels to be scrapped*/
    /* without the LDONE travelling through the optimizer queue*/
    new->lbl.status = CODELABEL | DYINGLABEL;
    optreturn( new )
}


extern  label_handle AskRTLabel( cg_sym_handle sym )
/**************************************************/
{
    label_handle    lbl;

  optbegin
    lbl = DoAskForLabel( sym );
    _SetStatus( lbl, RUNTIME );
    optreturn( lbl );
}


extern  label_handle AskForLabel( cg_sym_handle sym )
/************************************************/
{
    label_handle    new;
    fe_attr         attr;

  optbegin
    new = DoAskForLabel( sym );
    if( sym != NULL ) {
        attr = FEAttr( sym );
        if( attr & FE_UNIQUE ) _SetStatus( new, UNIQUE );
    }
    optreturn( new );
}


extern  bool    AskIfReachedLabel( label_handle lbl )
/************************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( _TstStatus( lbl, REACHED ) );
}


extern  bool    AskIfRTLabel( label_handle lbl )
/*******************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( _TstStatus( lbl, RUNTIME ) );
}


extern  bool    AskIfUniqueLabel( label_handle lbl )
/***********************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( _TstStatus( lbl, UNIQUE ) != 0 );
}


extern  bool    AskIfCommonLabel( label_handle lbl )
/***********************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( _TstStatus( lbl, COMMON_LBL ) != 0 );
}


extern  offset  AskAddress( label_handle lbl )
/*****************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( lbl->lbl.address );
}


extern  pointer AskLblPatch( label_handle lbl )
/******************************************/
{
  optbegin
    _ValidLbl( lbl );
    if( lbl->lbl.patch == NULL ) {
        lbl->lbl.patch = InitPatch();
    }
    optreturn( lbl->lbl.patch );
}


extern  cg_sym_handle   AskForLblSym( label_handle lbl )
/***************************************************/
{
  optbegin
    _ValidLbl( lbl );
    optreturn( lbl->lbl.sym );
}
