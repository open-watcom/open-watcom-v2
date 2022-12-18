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
* Description:  Aux pragma flags and data shared between cg and front ends.
*
****************************************************************************/


#ifndef _CG_AUX_INCLUDED
#define _CG_AUX_INCLUDED

#include "hwreg.h"
#include "cgauxa.h"

typedef enum {
    FEINF_AUX_LOOKUP,
    FEINF_CALL_BYTES,
    FEINF_CALL_CLASS,
    FEINF_FREE_SEGMENT,
    FEINF_OBJECT_FILE_NAME,
    FEINF_PARM_REGS,
    FEINF_RETURN_REG,
    FEINF_REVISION_NUMBER,
    FEINF_SAVE_REGS,
    FEINF_SHADOW_SYMBOL,
    FEINF_SOURCE_NAME,
    FEINF_TEMP_LOC_NAME,
    FEINF_TEMP_LOC_TELL,
    FEINF_NEXT_DEPENDENCY,
    FEINF_DEPENDENCY_TIMESTAMP,
    FEINF_DEPENDENCY_NAME,
    FEINF_SOURCE_LANGUAGE,
    FEINF_DEFAULT_IMPORT_RESOLVE,
    FEINF_UNROLL_COUNT,
    FEINF_DBG_PREDEF_SYM,
    FEINF_DBG_PCH_SYM,
    FEINF_DBG_SYM_ACCESS,
    FEINF_DBG_DWARF_PRODUCER,
    #include "cgauxc.h"
    LAST_FEAUX
} aux_class;

typedef enum {
    IMPORT_IS_WEAK,
    IMPORT_IS_LAZY,
    IMPORT_IS_CONDITIONAL,
    IMPORT_IS_CONDITIONAL_PURE,
} import_type;

typedef enum  {
    SYM_ACC_PUBLIC,
    SYM_ACC_PROTECTED,
    SYM_ACC_PRIVATE,
} sym_access;

#define TEMP_LOC_QUIT   0
#define TEMP_LOC_NO     1
#define TEMP_LOC_YES    2

#define REVERSE_PARMS           0x00000001L
#define ABORTS                  0x00000002L
#define NORETURN                0x00000004L
#define PARMS_BY_ADDRESS        0x00000008L
#define MAKE_CALL_INLINE        0x00000010L
#define HAS_VARARGS             0x00000020L
#define PARMS_STACK_RESERVE     0x00000040L
#define SETJMP_KLUGE            0x00000080L
#define PARMS_PREFER_REGS       0x00000100L
#define LAST_AUX_ATTRIBUTE      0x00000100L

#define _TARG_AUX_SHIFT         9

#if ( LAST_AUX_ATTRIBUTE >> _TARG_AUX_SHIFT ) != 0
    #error too many attributes
#endif

#endif
