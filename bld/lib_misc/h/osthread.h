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
* Description:  Includes Operating System specific header files required
*               for threads
*
****************************************************************************/

#ifndef _OSTHREAD_H_INCLUDED
#define _OSTHREAD_H_INCLUDED

#if !defined( _M_I86 )
  #if defined(__QNX__)
  #elif defined(__LINUX__)
  #elif defined(__NETWARE__)
  #elif defined(__NT__) || defined(__OS2__) || defined(__RDOS__)
    #if defined( __MT__ )
      #if defined(__NT__)
        #include "ntexc.h"
      #elif defined(__OS2__)
        #include "os2exc.h"
      #elif defined(__RDOS__)
        #include "rdosexc.h"
      #endif
      #define __XCPTHANDLER   (__THREADDATAPTR->xcpt_handler)
    #else
      extern struct _EXCEPTIONREGISTRATIONRECORD *__XcptHandler;
      #define __XCPTHANDLER   __XcptHandler
    #endif
  #endif
#endif

#endif
