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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


typedef enum {

        ROUTINE_REMOVES_PARMS           = 0x00000001,
        ROUTINE_NEEDS_DS_LOADED         = 0x00000004,
        ROUTINE_HAS_SPECIAL_RETURN      = 0x00000008,
        ROUTINE_NEVER_RETURNS           = 0x00000010,
        ROUTINE_NEEDS_PROLOG            = 0x00000020,
        ROUTINE_ALLOCS_RETURN           = 0x00000040,
        ROUTINE_WANTS_DEBUGGING         = 0x00000080,
        ROUTINE_NO_STRUCT_REG_RETURNS   = 0x00000100,
        ROUTINE_NO_FLOAT_REG_RETURNS    = 0x00000200,
        ROUTINE_MODIFY_EXACT            = 0x00000400,
        ROUTINE_MODIFIES_NO_MEMORY      = 0x00000800,
        ROUTINE_READS_NO_MEMORY         = 0x00001000,
        ROUTINE_NO_8087_RETURNS         = 0x00004000,
        ROUTINE_LOADS_DS                = 0x00008000,
        ROUTINE_STACK_RESERVE           = 0x00010000,

        /* don't use these directly -- use the macros below */
        ROUTINE_LONG                    = 0x00000002,
        ROUTINE_INTERRUPT               = 0x00002000,
        ROUTINE_FAR16                   = 0x00002002,
        ROUTINE_CLASS_MASK              = 0x00002002,
} call_attributes;

#define _RoutineIsLong( x ) \
        ( ( (x) & ROUTINE_CLASS_MASK ) == ROUTINE_LONG )
#define _RoutineIsInterrupt( x ) \
        ( ( (x) & ROUTINE_CLASS_MASK ) == ROUTINE_INTERRUPT )
#define _RoutineIsFar16( x ) \
        ( ( (x) & ROUTINE_CLASS_MASK ) == ROUTINE_FAR16 )

typedef struct {
        char                    wasted_space;
} call_registers;

typedef struct target_proc_def {
        type_length             max_stack;
        type_length             ___UNUSED;
        type_length             base_adjust;
        type_length             push_local_size;
        union name              *return_points;
        pointer                 debug;
        pointer                 routine_profile_data;
        pointer                 block_profile_data;
        abspatch_handle         prolog_loc;
        abspatch_handle         stack_check;
        unsigned                sp_frame : 1;
        unsigned                sp_align : 1;
        unsigned                has_fd_temps : 1;
        unsigned                never_sp_frame : 1;
        union name              *tls_index;
} target_proc_def;
