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
    BITB     SW_CASE_SENSITIVE              : 1; // used by SSL - use SW_CASE_IGNORE
    BITB     SW_REMOTE_FILES                : 1;
    BITB     SW_TOUCH_SCREEN_BUFF           : 1;
    BITB     SW_REMOTE_LINK                 : 1;
    BITB     SW_TASK_RUNNING                : 1;
    BITB     SW_HAVE_TASK                   : 1;
    BITB     SW_MIGHT_HAVE_LOST_DISPLAY     : 1;
    BITB     SW_IMPLICIT                    : 1;
    BITB     SW_BELL                        : 1;
    BITB     SW_FLIP                        : 1;
    BITB     SW_ASM_SOURCE                  : 1;
    BITB     SW_LOAD_SYMS                   : 1;
    BITB     SW_USE_MOUSE                   : 1;
    BITB     SW_HAVE_RUNTIME_DLLS           : 1;
    BITB     SW_SIDE_EFFECT                 : 1;
    BITB     SW_ASM_HEX                     : 1;
    BITB     SW_RECURSE_CHECK               : 1;
    BITB     SW_ERROR_RETURNS               : 1;
    BITB     SW_THREAD_EXTRA_CHANGED        : 1;
    BITB     SW_ERR_IN_TXTBUFF              : 1;
    BITB     SW_VAR_WHOLE_EXPR              : 1;
    BITB     SW_NO_EXPORT_SYMS              : 1;
    BITB     SW_KNOW_EMULATOR               : 1;
    BITB     SW_HAVE_SET_CALL               : 1;
    BITB     SW_COL_MAJ_ARRAYS              : 1;
    BITB     SW_PENDING_REPAINT             : 1;
    BITB     SW_STACK_GROWS_UP              : 1;
    BITB     SW_EXECUTE_LONG                : 1;
    BITB     SW_TRUE_ARGV                   : 1;
    BITB     SW_PROC_ALREADY_STARTED        : 1;
    BITB     SW_ADDING_SYMFILE              : 1;
    BITB     SW_IGNORE_SEGMENTS             : 1;
    BITB     SW_EXPR_IS_CALL                : 1;
    BITB     SW_WND_DOING_INPUT             : 1;
    BITB     SW_OPEN_NO_SHOW                : 1;
    BITB     SW_DOT_IS_DATA                 : 1;
    BITB     SW_DEFER_SYM_LOAD              : 1;
    BITB     SW_CASE_IGNORE                 : 1;
    BITB     SW_AUTO_SAVE_CONFIG            : 1;
    BITB     SW_CMD_INTERACTIVE             : 1;
    BITB     SW_VAR_SHOW_CODE               : 1;
    BITB     SW_VAR_SHOW_INHERIT            : 1;
    BITB     SW_VAR_SHOW_COMPILER           : 1;
    BITB     SW_VAR_SHOW_PRIVATE            : 1;
    BITB     SW_VAR_SHOW_PROTECTED          : 1;
    BITB     SW_FUNC_D2_ONLY                : 1;
    BITB     SW_GLOB_D2_ONLY                : 1;
    BITB     SW_AMBIGUITY_FATAL             : 1;
    BITB     SW_CALL_FATAL                  : 1;
    BITB     SW_VAR_SHOW_MEMBERS            : 1;
    BITB     SW_CONTROL_HOTKEYS             : 1;
    BITB     SW_ERROR_PRESERVES_RADIX       : 1;
    BITB     SW_RECORD_LOCATION_ASSIGN      : 1;
    BITB     SW_IN_REPLAY_MODE              : 1;
    BITB     SW_EXECUTE_ABORTED             : 1;
    BITB     SW_BREAK_ON_DLL                : 1;
    BITB     SW_HAD_ASYNCH_EVENT            : 1;
    BITB     SW_EVENT_RECORDED_SINCE_ASYNCH : 1;
    BITB     SW_RUNNING_PROFILE             : 1;
    BITB     SW_DETACHABLE_WINDOWS          : 1;
    BITB     SW_MOD_ALL_MODULES             : 1;
    BITB     SW_NO_DISAMBIGUATOR            : 1;
    BITB     SW_POWERBUILDER                : 1;
    BITB     SW_TRAP_CMDS_PUSHED            : 1;
    BITB     SW_HAVE_SEGMENTS               : 1;
    BITB     SW_ERROR_STARTUP               : 1;
    BITB     SW_VAR_SHOW_STATIC             : 1;
    BITB     SW_BREAK_ON_DEBUG_MESSAGE      : 1;
    BITB     SW_CHECK_SOURCE_EXISTS         : 1;
    BITB     SW_CONTINUE_UNEXPECTED_BREAK   : 1;
    BITB     SW_KEEP_HEAP_ENABLED           : 1;
    BITB     SW_BREAK_ON_WRITE              : 1;
    BITB     SW_DONT_EXPAND_HEX             : 1;    /* If set, display hex in shortest form - 0xf rather than 0x000f, etc */
} dbg_switches;


#define _SwitchOn( switch )     ( ( DbgSwitches.switch ) = 1 )
#define _SwitchOff( switch )    ( ( DbgSwitches.switch ) = 0)
#define _SwitchToggle( switch ) ( ( DbgSwitches.switch ) = !( DbgSwitches.switch ) )
#define _SwitchSet( switch, i ) ( ( DbgSwitches.switch ) = ( ( i ) != 0 ) )

#define _IsOn( switch )         ( ( DbgSwitches.switch ) != 0 )
#define _IsOff( switch )        ( ( DbgSwitches.switch ) == 0 )
