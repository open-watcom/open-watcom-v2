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
* Description:  Signal table definitions.
*
****************************************************************************/


#ifndef _SIGDEFN_H_INCLUDED
#define _SIGDEFN_H_INCLUDED

#if !defined(__NT__) && !defined(__OS2__) && !defined(__NETWARE__) && !defined(__GENERIC__) && !defined(__RDOS__)
#error Must be bt=NT or bt=OS2 or bt=NETWARE or bt=GENERIC or bt=RDOS
#endif

#include "variety.h"
#include <signal.h>

#define __SIGLAST       _SIGMAX

// note that __NT__ and __NETWARE__ are always 32bit
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
    #if defined(__NETWARE__)
        typedef __sig_func sigtab;
    #else
        typedef struct sigtab {
            __sig_func  func;           /* user signal handler */
            int         os_sig_code;    /* OS signal code */
        } sigtab;
    #endif
#else
    // 16 bit OS/2 1.x
    #define INCL_DOSEXCEPTIONS
    #include <wos2.h>

    typedef struct      sigtab {
        __sig_func      func;     /* user signal handler */
        VOID (_WCI86FAR PASCAL *os_func)(USHORT, USHORT);
        USHORT  prev_action;      /* previous action */
        int     os_sig_code;      /* OS/2 1.x signal code */
    } sigtab;
#endif
#endif
