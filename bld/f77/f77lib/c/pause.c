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
* Description:  PAUSE statement (for optimizing compiler)
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "rtenv.h"
#include "ftnio.h"

#include <string.h>

// this is used by the load and go debugger in _SA_LIBRARY

static void noHook( void ) {}
void (*PAUSE_HOOK)(void) = noHook;


void    Pause( string PGM *ptr ) {
//================================

// Process a PAUSE statement.

    RTSysInit();
    if( ptr != NULL ) {
        _AccessFIO();
        // Before writing the error message, flush standard output files so that
        // if unit 6 is connected to the standard output device which has been
        // redirected to a file, the file gets flushed before the error message
        // is written.
        FlushStdUnit();
        StdBuffer();
        StdWrite( ptr->strptr, ptr->len );
        StdFlush();
        _ReleaseFIO();
    }
    WaitForEnter();
}
