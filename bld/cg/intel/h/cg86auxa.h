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


#define CALLER_POPS                    ( 0x00000001L << _TARG_AUX_SHIFT )
#define DLL_EXPORT                     ( 0x00000002L << _TARG_AUX_SHIFT )
#define SPECIAL_RETURN                 ( 0x00000004L << _TARG_AUX_SHIFT )
#define SPECIAL_STRUCT_RETURN          ( 0x00000008L << _TARG_AUX_SHIFT )
#define FAR                            ( 0x00000010L << _TARG_AUX_SHIFT )
#define INTERRUPT                      ( 0x00000020L << _TARG_AUX_SHIFT )
#define LOAD_DS_ON_CALL                ( 0x00000040L << _TARG_AUX_SHIFT )
#define LOAD_DS_ON_ENTRY               ( 0x00000080L << _TARG_AUX_SHIFT )
#define MODIFY_EXACT                   ( 0x00000100L << _TARG_AUX_SHIFT )
#define NO_8087_RETURNS                ( 0x00000200L << _TARG_AUX_SHIFT )
#define NO_FLOAT_REG_RETURNS           ( 0x00000400L << _TARG_AUX_SHIFT )
#define NO_MEMORY_CHANGED              ( 0x00000800L << _TARG_AUX_SHIFT )
#define NO_MEMORY_READ                 ( 0x00001000L << _TARG_AUX_SHIFT )
#define NO_STRUCT_REG_RETURNS          ( 0x00002000L << _TARG_AUX_SHIFT )
#define ROUTINE_RETURN                 ( 0x00004000L << _TARG_AUX_SHIFT )
#define FAT_WINDOWS_PROLOG             ( 0x00008000L << _TARG_AUX_SHIFT )
#define GENERATE_STACK_FRAME           ( 0x00010000L << _TARG_AUX_SHIFT )
#define EMIT_FUNCTION_NAME             ( 0x00020000L << _TARG_AUX_SHIFT )
#define GROW_STACK                     ( 0x00040000L << _TARG_AUX_SHIFT )
#define PROLOG_HOOKS                   ( 0x00080000L << _TARG_AUX_SHIFT )
#define THUNK_PROLOG                   ( 0x00100000L << _TARG_AUX_SHIFT )
#define EPILOG_HOOKS                   ( 0x00200000L << _TARG_AUX_SHIFT )
#define FAR16_CALL                     ( 0x00400000L << _TARG_AUX_SHIFT )
#define TOUCH_STACK                    ( 0x00800000L << _TARG_AUX_SHIFT )
#define LAST_TARG_AUX_ATTRIBUTE        ( 0x00800000L << _TARG_AUX_SHIFT )

#if LAST_TARG_AUX_ATTRIBUTE == 0
    #error Overflowed a long
#endif

#define DO_FLOATING_FIXUPS      0x80000000
#define FLOATING_FIXUP_BYTE     0xFF

#define DO_SYM_FIXUPS           DO_FLOATING_FIXUPS      /* must be the same */
#define SYM_FIXUP_BYTE          FLOATING_FIXUP_BYTE     /* must be the same */
#define FIX_SYM_OFFSET          0x00    /* followed by a long */
#define FIX_SYM_SEGMENT         0x01    /* .. */
#define FIX_SYM_RELOFF          0x02    /* .. */
