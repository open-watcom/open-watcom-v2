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


#include <stdlib.h>
#include <signal.h>
#include "standard.h"
#include "cg.h"
#include "coderep.h"
#include "feprotos.h"

extern void             GrabTimer();
extern void             ReleTimer();
extern void             ScratchObj(void);

static  bool volatile   BrkFlag;
static  bool            OnExitFlag = TRUE;

bool TBreak() {
/*************/

    int         brk;

    InitOnExit();
    brk = BrkFlag;
    BrkFlag = 0;
    return( brk );
}

void CauseTBreak() {
/******************/
    BrkFlag = TRUE;
}

void SigIntFunc( int sig_num ) {
/******************************/

    sig_num = sig_num;
    BrkFlag = TRUE;
}

void BrkInit() {
/**************/

    signal( SIGINT, SigIntFunc );
}

void BrkFini() {
/**************/
}

void InitOnExit() {
/*****************/

    if( OnExitFlag ) {
        BrkInit();
        GrabTimer();
        atexit( BrkFini );
        atexit( ReleTimer );
        OnExitFlag = FALSE;
    }
}

void FatalError( char * str ) {
/*****************************/

     ReleTimer();
     BrkFini();
     ScratchObj();
     FEMessage( MSG_FATAL, str );
}
