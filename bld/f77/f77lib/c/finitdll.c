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
* Description:  DLL initialization
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rmemmgr.h"
#include "rtdata.h"
#include "fthread.h"

static void NullTrapRtn( void ) {
//=========================

}


static void __InitExceptionVectors( void ) {
//====================================

    _ExceptionInit = &NullTrapRtn;
    _ExceptionFini = &NullTrapRtn;
}


unsigned        __FInitDLL( void ) {
//============================

    __InitExceptionVectors();
    if( __InitFThreadProcessing() != 0 ) return( 0 );
#if defined( __NT__ )
    {
        if( __ASTACKSIZ != 0 ) {
            __ASTACKPTR = RMemAlloc( __ASTACKSIZ );
            if( __ASTACKPTR == NULL ) return( 0 );
            __ASTACKPTR += __ASTACKSIZ;
        }
    }
#endif
    return( 1 );
}
