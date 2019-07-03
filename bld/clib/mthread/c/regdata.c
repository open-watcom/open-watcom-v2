/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Thread data size registration. Allows the caller to override
*               the default. Used by the C++ runtime.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#if defined( __OS2__ )
    #define INCL_DOSSEMAPHORES
    #define INCL_DOSPROCESS
    #include <wos2.h>
#elif defined( __NT__ )
    #include <time.h>
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include "ntext.h"
#elif defined( __UNIX__ )
    #include <sys/types.h>
    #include <unistd.h>
  #if defined( __LINUX__ )
    #include <process.h>
  #endif
#elif defined( __RDOS__ )
    #include <rdos.h>
#elif defined( __RDOSDEV__ )
    #include <rdos.h>
    #include <rdosdev.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "thread.h"
#include "trdlist.h"
#include "trdlstac.h"
#include "mthread.h"


_WCRTLINK unsigned __RegisterThreadDataSize( unsigned size )
/**********************************************************/
{
    unsigned    offset;

    _AccessTDList();
    offset = __ThreadDataSize;
    __ThreadDataSize += size;
#if !defined(__UNIX__) && !defined(_NETWARE_CLIB) && !defined(__RDOSDEV__)
    __ResizeThreadDataList();
#endif
    _ReleaseTDList();
    return( offset );
}

