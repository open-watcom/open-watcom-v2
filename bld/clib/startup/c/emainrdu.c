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
* Description:  RDOS executable entry point.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <rdos.h>
#include "initfini.h"
#include "initarg.h"
#include "thread.h"
#include "initsig.h"
#include "mthread.h"
#include "osmain.h"
#include "cmain.h"
#include "cinit.h"


void __RdosMain( void )
{
    thread_data             *tdata;
    REGISTRATION_RECORD     rr;
 
    __InitRtns( INIT_PRIORITY_THREAD );
    tdata = ( thread_data* )RdosAllocateMem( __ThreadDataSize );
    memset( tdata, 0, __ThreadDataSize );
    tdata->__data_size = __ThreadDataSize;

    __InitThreadData( tdata );

    _LpPgmName = (char *)RdosGetExeName();
    __RdosInit( 0, tdata, RdosGetModuleHandle() );
    __NewExceptionFilter( &rr );
    __InitRtns( INIT_PRIORITY_LIBRARY+1 );
    __sig_init_rtn();
    __InitRtns( 255 );
    __CMain();
}
