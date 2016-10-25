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
* Description:  Defines for Linux specific system calls. We maintain this
*               separately from the Linux kernel source code, so it may
*               need updating from time to time. We need to do this so that
*               the runtime library can be built on any platform without
*               needing the Linux kernel headers to be installed.
*
****************************************************************************/

#ifndef _SYSLINUX_H_INCLUDED
#define _SYSLINUX_H_INCLUDED

#ifndef __TYPES_H_INCLUDED
    #include <sys/types.h>
#endif

/* Include architecture specific definitions */
#if defined( __386__ )
    #include "sys386.h"
#elif defined( __PPC__ )
    #include "sysppc.h"
#elif defined( __MIPS__ )
    #include "sysmips.h"
#endif

#include "thread.h"

#endif  /* _SYSLINUX_H_INCLUDED */
