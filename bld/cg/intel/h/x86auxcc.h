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
*                   (x86 target specific)
*
****************************************************************************/


typedef enum {
    FECALL_X86_NONE                     = 0,
    FECALL_X86_SPECIAL_RETURN           = 0x00000001,
    FECALL_X86_SPECIAL_STRUCT_RETURN    = 0x00000002,
    FECALL_X86_FAR_CALL                 = 0x00000004,
    FECALL_X86_INTERRUPT                = 0x00000008,
    FECALL_X86_LOAD_DS_ON_CALL          = 0x00000010,
    FECALL_X86_LOAD_DS_ON_ENTRY         = 0x00000020,
    FECALL_X86_MODIFY_EXACT             = 0x00000040,
    FECALL_X86_NO_8087_RETURNS          = 0x00000080,
    FECALL_X86_NO_FLOAT_REG_RETURNS     = 0x00000100,
    FECALL_X86_NO_STRUCT_REG_RETURNS    = 0x00000200,
    FECALL_X86_ROUTINE_RETURN           = 0x00000400,
    FECALL_X86_PROLOG_FAT_WINDOWS       = 0x00000800,
    FECALL_X86_GENERATE_STACK_FRAME     = 0x00001000,
    FECALL_X86_EMIT_FUNCTION_NAME       = 0x00002000,
    FECALL_X86_GROW_STACK               = 0x00004000,
    FECALL_X86_PROLOG_HOOKS             = 0x00008000,
    FECALL_X86_THUNK_PROLOG             = 0x00010000,
    FECALL_X86_EPILOG_HOOKS             = 0x00020000,
    FECALL_X86_FAR16_CALL               = 0x00040000,
    FECALL_X86_TOUCH_STACK              = 0x00080000,
    FECALL_X86_LOAD_RDOSDEV_ON_ENTRY    = 0x00100000,
    FECALL_X86_FARSS                    = 0x00200000,
    FECALL_X86_PARMS_STACK_RESERVE      = 0x00400000,
    FECALL_X86_PARMS_PREFER_REGS        = 0x00800000,
    FECALL_X86_NEEDS_BP_CHAIN           = 0x01000000,
} call_class_target;
