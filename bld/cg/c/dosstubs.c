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


#include "standard.h"
#include "cg.h"
#include "bckdef.h"
#include "feprotos.h"


int TBreak() {
/*************/
    return( FALSE );
}

void CauseTBreak() {
/******************/
    /* cause TBreak to return TRUE */
}

extern  void    InitBlip() {
/**************************/
}

extern  void    FiniBlip() {
/**************************/
}

extern  bool    WantZoiks2() {
/****************************/

    return( FALSE );
}

extern  void    URBlip() {
/************************/
}

extern  void    PGBlip(char *name) {
/***********************************/
}


extern  void    TGBlip() {
/************************/
}

extern  void    LPBlip() {
/************************/
}

extern  void    SXBlip() {
/************************/
}

extern  void    EXBlip() {
/************************/
}

extern  void    GRBlip() {
/************************/
}

extern  void    IMBlip() {
/************************/
}

extern  void    SCBlip() {
/************************/
}

extern  void    PSBlip() {
/************************/
}

extern  void    PLBlip() {
/************************/
}

extern  void    DGBlip() {
/************************/
}

void FatalError( char * str ) {
/*****************************/

    FEMessage( MSG_FATAL, str );
}
