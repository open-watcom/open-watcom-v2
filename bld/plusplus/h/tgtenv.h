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
* Description:  Get appropriate language environment for target.
*
****************************************************************************/


#ifndef __TGTENV_H__

#include "langenvd.h"

#if _INTEL_CPU
    #define __TGT_SYS __TGT_SYS_X86
#elif _CPU == _AXP
    #define   __TGT_SYS   __TGT_SYS_AXP_NT
#elif _CPU == _MIPS
     #define   __TGT_SYS   __TGT_SYS_MIPS
#elif _CPU == _PPC
    #define   __TGT_SYS   __TGT_SYS_PPC_NT
#else
    #error Invalid target environment
#endif

#include "langenv.h"

#endif
