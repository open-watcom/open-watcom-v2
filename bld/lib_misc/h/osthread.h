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

#include "variety.h"
#if defined(__QNX__)
  #include <semaphor.h>
#elif defined(__LINUX__)
// TODO: Include Linux specific thread header files
#elif defined(__NT__)
  #include <windows.h>
  #include <ntex.h>
#elif defined(__OS2__)
  #ifndef OS2_INCLUDED
    #define INCL_DOSEXCEPTIONS
    #include <wos2.h>
  #endif
  #ifdef OS2_INCLUDED
    #if defined(__386__) || defined(__PPC__)
      #undef  __EXCEPTION_RECORD
      #define __EXCEPTION_RECORD EXCEPTIONREGISTRATIONRECORD
    #endif
  #endif
#endif
#include <thread.h>

#define _OSTHREAD_H_INCLUDED
#endif

