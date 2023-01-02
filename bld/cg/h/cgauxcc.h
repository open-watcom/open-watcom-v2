/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Aux pragma call flags shared between cg and front ends.
*
****************************************************************************/


#define FECALL_GEN_REVERSE_PARMS        0x00000001L
#define FECALL_GEN_ABORTS               0x00000002L
#define FECALL_GEN_NORETURN             0x00000004L
#define FECALL_GEN_PARMS_BY_ADDRESS     0x00000008L
#define FECALL_GEN_MAKE_CALL_INLINE     0x00000010L
#define FECALL_GEN_HAS_VARARGS          0x00000020L
#define FECALL_GEN_SETJMP_KLUGE         0x00000040L
#define FECALL_GEN_CALLER_POPS          0x00000080L
#define FECALL_GEN_NO_MEMORY_READ       0x00000100L
#define FECALL_GEN_NO_MEMORY_CHANGED    0x00000200L
#define FECALL_GEN_DLL_EXPORT           0x00000400L
#define LAST_AUX_ATTRIBUTE          0x00000400L

#define _TARG_AUX_SHIFT             11

#if ( LAST_AUX_ATTRIBUTE >> _TARG_AUX_SHIFT ) != 0
    #error too many attributes in cgaux.h
#endif

#define FECALL_GEN_MASK ( \
    FECALL_GEN_REVERSE_PARMS | \
    FECALL_GEN_ABORTS | \
    FECALL_GEN_NORETURN | \
    FECALL_GEN_PARMS_BY_ADDRESS | \
    FECALL_GEN_MAKE_CALL_INLINE | \
    FECALL_GEN_HAS_VARARGS | \
    FECALL_GEN_SETJMP_KLUGE | \
    FECALL_GEN_CALLER_POPS | \
    FECALL_GEN_NO_MEMORY_READ | \
    FECALL_GEN_NO_MEMORY_CHANGED | \
    FECALL_GEN_DLL_EXPORT | \
    0)

#include "cgauxccc.h"

