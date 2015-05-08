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
* Description:  Module to init multi-threading initialization.
*
****************************************************************************/


#include "variety.h"
#if defined( __OS2__ )
#include <wos2.h>
#endif
#include "rtinit.h"
#include "rtdata.h"
#include "thread.h"
#include "mthread.h"

#if defined(__OS2_286__) || defined(__NETWARE__)
    #if defined( __MT__ )
        int __imthread;
    #endif
#else
    static void __imthread_fn( void ) {
        #if defined(__NT__)
            if( !__NTThreadInit() ) return;
        #elif defined(_NETWARE_LIB)
            if( !__LibCThreadInit() ) return;
        #elif defined(__QNX__)
        #elif defined(__LINUX__)
        #elif defined(__WARP__)
            if( __InitThreadProcessing() == 0 ) return;
        #elif defined(__RDOS__)            
            if( !__RdosThreadInit() ) return;
        #endif
        __InitMultipleThread();
    }

    _WCRTDATA XI( __imthread, __imthread_fn, INIT_PRIORITY_RUNTIME + 1 )
#endif

#if defined(_M_IX86)
  #pragma aux __imthread "*";
#endif
