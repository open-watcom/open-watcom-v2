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
* Description:  Aux pragma flags and data shared between cg and front ends.
*
****************************************************************************/


#ifndef _CG_AUX_INCLUDED

#include "hwreg.h"
#include "cgauxa.h"

typedef enum {
        AUX_LOOKUP,
        CALL_BYTES,
        CALL_CLASS,
        FREE_SEGMENT,
        OBJECT_FILE_NAME,
        PARM_REGS,
        RETURN_REG,
        REVISION_NUMBER,
        SAVE_REGS,
        SHADOW_SYMBOL,
        SOURCE_NAME,
        TEMP_LOC_NAME,
        TEMP_LOC_TELL,
        NEXT_DEPENDENCY,
        DEPENDENCY_TIMESTAMP,
        DEPENDENCY_NAME,
        SOURCE_LANGUAGE,
        DEFAULT_IMPORT_RESOLVE,
        UNROLL_COUNT,
        DBG_PREDEF_SYM,
        DBG_PCH_SYM,
        DBG_SYM_ACCESS,
        __UNUSED_AUX_CLASS_13,
        __UNUSED_AUX_CLASS_14,
        __UNUSED_AUX_CLASS_15,
        __UNUSED_AUX_CLASS_16,
        __UNUSED_AUX_CLASS_17,
        __UNUSED_AUX_CLASS_18,
        __UNUSED_AUX_CLASS_19,
        #include "cgauxc.h"
        __LAST_AUX_CLASS
} aux_class;

typedef enum  {
    SYM_ACC_PUBLIC,
    SYM_ACC_PROTECTED,
    SYM_ACC_PRIVATE,
}sym_access;

#define TEMP_LOC_QUIT   0
#define TEMP_LOC_NO     1
#define TEMP_LOC_YES    2

#define REVERSE_PARMS           0x00000001L
#define SUICIDAL                0x00000002L
#define PARMS_BY_ADDRESS        0x00000004L
#define MAKE_CALL_INLINE        0x00000008L
#define HAS_VARARGS             0x00000010L
#define PARMS_STACK_RESERVE     0x00000020L
#define SETJMP_KLUGE            0x00000040L
#define PARMS_PREFER_REGS       0x00000080L
#define LAST_AUX_ATTRIBUTE      0x00000080L

#define _TARG_AUX_SHIFT         8

#if ( LAST_AUX_ATTRIBUTE >> _TARG_AUX_SHIFT ) != 0
    #error too many attributes
#endif

#define _CG_AUX_INCLUDED
#endif
