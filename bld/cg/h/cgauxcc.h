/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
*                 (generic flags)
*
****************************************************************************/


#include "cgauxccc.h"

typedef enum {
    FECALL_GEN_NONE                 = 0,
    FECALL_GEN_REVERSE_PARMS        = 0x00000001,
    FECALL_GEN_ABORTS               = 0x00000002,
    FECALL_GEN_NORETURN             = 0x00000004,
    FECALL_GEN_PARMS_BY_ADDRESS     = 0x00000008,
    FECALL_GEN_MAKE_CALL_INLINE     = 0x00000010,
    FECALL_GEN_HAS_VARARGS          = 0x00000020,
    FECALL_GEN_SETJMP_KLUGE         = 0x00000040,
    FECALL_GEN_CALLER_POPS          = 0x00000080,
    FECALL_GEN_NO_MEMORY_READ       = 0x00000100,
    FECALL_GEN_NO_MEMORY_CHANGED    = 0x00000200,
    FECALL_GEN_DLL_EXPORT           = 0x00000400,
} call_class;
