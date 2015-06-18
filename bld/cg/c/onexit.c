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
* Description:  Abnormal termination processing.
*
****************************************************************************/


#include "cgstd.h"
#include <stdlib.h>
#include <signal.h>
#include "cgdefs.h"
#include "cg.h"
#include "cgmisc.h"
#include "coderep.h"
#include "objio.h"
#include "onexit.h"
#include "qtimer.h"
#include "feprotos.h"


static  bool volatile   BrkFlag;
static  bool            OnExitFlag = TRUE;


static void SigIntFunc( int sig_num )
/****************************/
{
    sig_num = sig_num;
    BrkFlag = TRUE;
}


static void BrkInit( void )
/******************/
{
    signal( SIGINT, SigIntFunc );
}


static void BrkFini( void )
/******************/
{
}


static void InitOnExit( void )
/*********************/
{
    if( OnExitFlag ) {
        BrkInit();
        GrabTimer();
        atexit( BrkFini );
        atexit( ReleTimer );
        OnExitFlag = FALSE;
    }
}


bool TBreak( void )
/*****************/
{
    int         brk;

    InitOnExit();
    brk = BrkFlag;
    BrkFlag = 0;
    return( brk != 0 );
}


void CauseTBreak( void )
/**********************/
{
    BrkFlag = TRUE;
}


void FatalError( const char *str )
/********************************/
{
     ReleTimer();
     BrkFini();
     ScratchObj();
     FEMessage( MSG_FATAL, (pointer)str );
}
