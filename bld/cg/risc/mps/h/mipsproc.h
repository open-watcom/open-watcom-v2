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
* Description:  Structures and constant for MIPS linkage conventions.
*
****************************************************************************/


typedef enum {
        ROUTINE_MODIFY_EXACT            = 0x0001,
        ROUTINE_MODIFIES_NO_MEMORY      = 0x0002,
        ROUTINE_READS_NO_MEMORY         = 0x0004,
        ROUTINE_REMOVES_PARMS           = 0x0008,
        ROUTINE_NEVER_RETURNS           = 0x0010,
        ROUTINE_WANTS_DEBUGGING         = 0x0020,
        ROUTINE_IS_SETJMP               = 0x0040,
} call_attributes;

typedef byte    reg_num;

#define NO_REGISTER     ((reg_num)-1)

typedef struct {
        char                    wasted_space;
} call_registers;

typedef struct stack_record {
        type_length             start;
        type_length             size;
} stack_record;

typedef struct stack_map {
        stack_record            slop;           // to keep stack aligned
        stack_record            frame_save;
        stack_record            varargs;
        stack_record            saved_regs;
        stack_record            locals;
        stack_record            parm_cache;
} stack_map;

typedef struct target_proc_def {
        type_length             max_stack;
        pointer                 debug;
        name                    *return_points;
        stack_map               stack_map;
        uint_32                 gpr_mask;
        uint_32                 fpr_mask;
        uint_32                 proc_start;
        uint_32                 pro_size;
        type_length             frame_size;
        bool                    base_is_fp;
        bool                    leaf;
} target_proc_def;
