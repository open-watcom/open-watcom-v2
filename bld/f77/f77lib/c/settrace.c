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
* Description:  set traceback information
*
****************************************************************************/

#include "ftnstd.h"
#include <string.h>
#include "frtdata.h"
#include "trcback.h"
#include "fthread.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rundat.h"
#include "rtenv.h"
#include "errcod.h"
#include "thread.h"


static  void    TraceInfo( char *buff ) {
//=======================================

    traceback   *tb;

    MsgBuffer( MS_TRACE_INFO, buff, _RWD_ExCurr->line, _RWD_ExCurr->name );
    StdWriteNL( buff, strlen( buff ) );
    for( tb = _RWD_ExCurr->link; tb != NULL; tb = tb->link ) {
        MsgBuffer( MS_CALLED_FROM, buff, tb->line, tb->name );
        StdWriteNL( buff, strlen( buff ) );
    }
}


void            SetLine( uint src_line ) {
//========================================

    RTSysInit();
    _RWD_ExCurr->line = src_line;
}


void            SetModule( traceback *tb ) {
//==========================================

    RTSysInit();
    // if TraceRoutine is not NULL the first time SetModule() is called,
    // we can assume we have linked with WATFOR-77 and will use its method
    // of printing traceback information
    if( TraceRoutine == NULL ) {
        TraceRoutine = &TraceInfo;
    }
    if( _RWD_ExCurr == tb ) {        // delete entry from traceback chain
        _RWD_ExCurr = _RWD_ExCurr->link;
    } else {                    // add entry to traceback chain
        tb->link = _RWD_ExCurr;
        _RWD_ExCurr = tb;
    }
}
