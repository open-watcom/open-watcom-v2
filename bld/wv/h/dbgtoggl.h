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


typedef struct {
    unsigned SW_CASE_SENSITIVE          : 1; // used by SSL - use SW_CASE_IGNORE
    unsigned SW_REMOTE_FILES            : 1;
    unsigned SW_TOUCH_SCREEN_BUFF       : 1;
    unsigned SW_REMOTE_LINK             : 1;
    unsigned SW_TASK_RUNNING            : 1;
    unsigned SW_HAVE_TASK               : 1;
    unsigned SW_MIGHT_HAVE_LOST_DISPLAY : 1;
    unsigned SW_IMPLICIT                : 1;
    unsigned SW_BELL                    : 1;
    unsigned SW_FLIP                    : 1;
    unsigned SW_ASM_SOURCE              : 1;
    unsigned SW_LOAD_SYMS               : 1;
    unsigned SW_USE_MOUSE               : 1;
    unsigned SW_HAVE_RUNTIME_DLLS       : 1;
    unsigned SW_SIDE_EFFECT             : 1;
    unsigned SW_ASM_HEX                 : 1;
    unsigned SW_RECURSE_CHECK           : 1;
    unsigned SW_ERROR_RETURNS           : 1;
    unsigned SW_THREAD_EXTRA_CHANGED    : 1;
    unsigned SW_ERR_IN_TXTBUFF          : 1;
    unsigned SW_VAR_WHOLE_EXPR          : 1;
    unsigned SW_NO_EXPORT_SYMS          : 1;
    unsigned SW_KNOW_EMULATOR           : 1;
    unsigned SW_HAVE_SET_CALL           : 1;
    unsigned SW_COL_MAJ_ARRAYS          : 1;
    unsigned SW_PENDING_REPAINT         : 1;
    unsigned SW_STACK_GROWS_UP          : 1;
    unsigned SW_EXECUTE_LONG            : 1;
    unsigned SW_TRUE_ARGV               : 1;
    unsigned SW_PROC_ALREADY_STARTED    : 1;
    unsigned SW_ADDING_SYMFILE          : 1;
    unsigned SW_IGNORE_SEGMENTS         : 1;
    unsigned SW_EXPR_IS_CALL            : 1;
    unsigned SW_WND_DOING_INPUT         : 1;
    unsigned SW_OPEN_NO_SHOW            : 1;
    unsigned SW_DOT_IS_DATA             : 1;
    unsigned SW_DEFER_SYM_LOAD          : 1;
    unsigned SW_CASE_IGNORE             : 1;
    unsigned SW_AUTO_SAVE_CONFIG        : 1;
    unsigned SW_CMD_INTERACTIVE         : 1;
    unsigned SW_VAR_SHOW_CODE           : 1;
    unsigned SW_VAR_SHOW_INHERIT        : 1;
    unsigned SW_VAR_SHOW_COMPILER       : 1;
    unsigned SW_VAR_SHOW_PRIVATE        : 1;
    unsigned SW_VAR_SHOW_PROTECTED      : 1;
    unsigned SW_FUNC_D2_ONLY            : 1;
    unsigned SW_GLOB_D2_ONLY            : 1;
    unsigned SW_AMBIGUITY_FATAL         : 1;
    unsigned SW_CALL_FATAL              : 1;
    unsigned SW_VAR_SHOW_MEMBERS        : 1;
    unsigned SW_CONTROL_HOTKEYS         : 1;
    unsigned SW_ERROR_PRESERVES_RADIX   : 1;
    unsigned SW_RECORD_LOCATION_ASSIGN  : 1;
    unsigned SW_IN_REPLAY_MODE          : 1;
    unsigned SW_EXECUTE_ABORTED         : 1;
    unsigned SW_BREAK_ON_DLL            : 1;
    unsigned SW_HAD_ASYNCH_EVENT        : 1;
    unsigned SW_EVENT_RECORDED_SINCE_ASYNCH     : 1;
    unsigned SW_RUNNING_PROFILE         : 1;
    unsigned SW_DETACHABLE_WINDOWS      : 1;
    unsigned SW_MOD_ALL_MODULES         : 1;
    unsigned SW_NO_DISAMBIGUATOR        : 1;
    unsigned SW_POWERBUILDER            : 1;
    unsigned SW_TRAP_CMDS_PUSHED        : 1;
    unsigned SW_HAVE_SEGMENTS           : 1;
    unsigned SW_ERROR_STARTUP           : 1;
    unsigned SW_VAR_SHOW_STATIC         : 1;
    unsigned SW_BREAK_ON_DEBUG_MESSAGE  : 1;
    unsigned SW_CHECK_SOURCE_EXISTS     : 1;
    unsigned SW_CONTINUE_UNEXPECTED_BREAK:1;
    unsigned SW_KEEP_HEAP_ENABLED       : 1;
} dbg_switches;

extern dbg_switches DbgSwitches;

#define _SwitchOn( switch )     ((DbgSwitches.switch) = 1)
#define _SwitchOff( switch )    ((DbgSwitches.switch) = 0)
#define _SwitchToggle( switch ) ((DbgSwitches.switch) = !(DbgSwitches.switch))
#define _SwitchSet( switch, i ) ((DbgSwitches.switch) = ( (i) != 0 ) )

#define _IsOn( switch )         ((DbgSwitches.switch)!=0)
#define _IsOff( switch )        ((DbgSwitches.switch)==0)
