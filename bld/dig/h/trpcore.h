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


#ifndef TRPCORE_H

#include "trptypes.h"
#include <_comdef.h>

#if __WATCOMC__ > 1000
#pragma pack(push,1);
#endif

enum {
    REQ_CONNECT,                        /* 00 */
    REQ_DISCONNECT,                     /* 01 */
    REQ_SUSPEND,                        /* 02 */
    REQ_RESUME,                         /* 03 */
    REQ_GET_SUPPLEMENTARY_SERVICE,      /* 04 */
    REQ_PERFORM_SUPPLEMENTARY_SERVICE,  /* 05 */
    REQ_GET_SYS_CONFIG,                 /* 06 */
    REQ_MAP_ADDR,                       /* 07 */
    REQ_ADDR_INFO,                      /* 08 obsolete - use MACHINE_DATA */
    REQ_CHECKSUM_MEM,                   /* 09 */
    REQ_READ_MEM,                       /* 10 */
    REQ_WRITE_MEM,                      /* 11 */
    REQ_READ_IO,                        /* 12 */
    REQ_WRITE_IO,                       /* 13 */
    REQ_READ_CPU,                       /* 14 obsolete - use READ_REGS */
    REQ_READ_FPU,                       /* 15 obsolete - use READ_REGS */
    REQ_WRITE_CPU,                      /* 16 obsolete - use WRITE_REGS */
    REQ_WRITE_FPU,                      /* 17 obsolete - use WRITE_REGS */
    REQ_PROG_GO,                        /* 18 */
    REQ_PROG_STEP,                      /* 19 */
    REQ_PROG_LOAD,                      /* 20 */
    REQ_PROG_KILL,                      /* 21 */
    REQ_SET_WATCH,                      /* 22 */
    REQ_CLEAR_WATCH,                    /* 23 */
    REQ_SET_BREAK,                      /* 24 */
    REQ_CLEAR_BREAK,                    /* 25 */
    REQ_GET_NEXT_ALIAS,                 /* 26 */
    REQ_SET_USER_SCREEN,                /* 27 */
    REQ_SET_DEBUG_SCREEN,               /* 28 */
    REQ_READ_USER_KEYBOARD,             /* 29 */
    REQ_GET_LIB_NAME,                   /* 30 */
    REQ_GET_ERR_TEXT,                   /* 31 */
    REQ_GET_MESSAGE_TEXT,               /* 32 */
    REQ_REDIRECT_STDIN,                 /* 33 */
    REQ_REDIRECT_STDOUT,                /* 34 */
    REQ_SPLIT_CMD,                      /* 35 */
    REQ_READ_REGS,                      /* 36 */
    REQ_WRITE_REGS,                     /* 37 */
    REQ_MACHINE_DATA                    /* 38 */
};

typedef struct {
    access_req          req;
    trap_version        ver;
} connect_req;

typedef struct {
    unsigned_16         max_msg_size;
    /* followed by any error message */
} connect_ret;

typedef struct {
    access_req          req;
} disconnect_req;

/* no disconnect_ret */

typedef struct {
    access_req          req;
} suspend_req;

/* no suspend_ret */

typedef struct {
    access_req          req;
} resume_req;

/* no resume_ret */

typedef struct {
    access_req          req;
    /* followed by service name */
} get_supplementary_service_req;

typedef struct {
    trap_error          err;
    trap_shandle        id;
} get_supplementary_service_ret;

/* perform_supplementary_service structures defined by service providers */


/*================ REQ_GET_SYS_CONFIG =================*/

typedef struct {
    access_req          req;
} get_sys_config_req;

typedef struct {
    system_config       sys;
} get_sys_config_ret;

/*================== REQ_MAP_ADDR ====================*/

typedef struct {
    access_req          req;
    addr48_ptr          in_addr;
    trap_mhandle        handle;
} _WCUNALIGNED map_addr_req;

typedef struct {
    addr48_ptr          out_addr;
    addr48_off          lo_bound;
    addr48_off          hi_bound;
} _WCUNALIGNED map_addr_ret;

typedef struct {
    access_req          req;
    addr48_ptr          in_addr;
} _WCUNALIGNED addr_info_req;

//obsolete - use REQ_MACHINE_DATA
typedef struct {
    unsigned_8          is_32;
} addr_info_ret;

typedef struct {
    access_req          req;
    addr48_ptr          in_addr;
    unsigned_16         len;
} _WCUNALIGNED checksum_mem_req;

typedef struct {
    unsigned_32         result;
} checksum_mem_ret;

typedef struct {
    access_req          req;
    addr48_ptr          mem_addr;
    unsigned_16         len;
} _WCUNALIGNED read_mem_req;

/* read_mem_ret is just the sequences of bytes requested */

typedef struct {
    access_req          req;
    addr48_ptr          mem_addr;
    /* followed by data to write */
} _WCUNALIGNED write_mem_req;

typedef struct {
    unsigned_16         len;
} write_mem_ret;

typedef struct {
    access_req          req;
    unsigned_32         IO_offset;
    unsigned_8          len;
} _WCUNALIGNED read_io_req;

/* read_io_ret is just the sequence of bytes requested */

typedef struct {
    access_req          req;
    unsigned_32         IO_offset;
    /* followed by data to write */
} _WCUNALIGNED write_io_req;

typedef struct {
    unsigned_8          len;
} write_io_ret;

/*================== REQ_READ_CPU =====================*/
//obsolete - use REQ_READ_REGS

typedef struct {
    unsigned_32 EAX;
    unsigned_32 EBX;
    unsigned_32 ECX;
    unsigned_32 EDX;
    unsigned_32 ESI;
    unsigned_32 EDI;
    unsigned_32 EBP;
    unsigned_32 ESP;
    unsigned_32 EIP;
    unsigned_32 EFL;
    unsigned_32 CR0;
    unsigned_32 CR2;
    unsigned_32 CR3;
    unsigned_16 DS;
    unsigned_16 ES;
    unsigned_16 SS;
    unsigned_16 CS;
    unsigned_16 FS;
    unsigned_16 GS;
} trap_cpu_regs;

typedef struct {
    access_req          req;
} read_cpu_req;

typedef struct {
    trap_cpu_regs       cpu;
} read_cpu_ret;

/*=================== REQ_READ_FPU =================*/
//obsolete - use REQ_READ_REGS

typedef struct {
    unsigned_32         control;
    unsigned_32         status;
    unsigned_32         tag;
    unsigned_32         ip_err[2];
    unsigned_32         op_err[2];
    xreal               reg[8];
} trap_fpu_regs;


typedef struct {
    access_req          req;
} read_fpu_req;

typedef struct {
    trap_fpu_regs       fpu;
} read_fpu_ret;

//obsolete - use REQ_WRITE_REGS
typedef struct {
    access_req          req;
    /* followed by a trap_cpu_regs structure */
} write_cpu_req;

/* no write_cpu_ret */

//obsolete - use REQ_WRITE_REGS
typedef struct {
    access_req          req;
    /* followed by a trap_fpu_regs structure */
} write_fpu_req;

/* no write_fpu_ret */

/*=================== REQ_PROG_GO ===================*/

#include "trapbrk.h"

typedef struct {
    access_req          req;
} prog_go_req;

typedef struct {
    addr48_ptr          stack_pointer;
    addr48_ptr          program_counter;
    unsigned_16         conditions;
} _WCUNALIGNED prog_go_ret;

typedef prog_go_req     prog_step_req;
typedef prog_go_ret     prog_step_ret;

/*===================== REQ_PROG_LOAD ====================*/

enum {
    LD_FLAG_IS_32               = 0x0001,       //obsolete
    LD_FLAG_IS_PROT             = 0x0002,       //obsolete
    LD_FLAG_IS_STARTED          = 0x0004,
    LD_FLAG_IGNORE_SEGMENTS     = 0x0008,
    LD_FLAG_HAVE_RUNTIME_DLLS   = 0x0010,
    LD_FLAG_DISPLAY_DAMAGED     = 0x0020
};

typedef struct {
    access_req          req;
    unsigned_8          true_argv;
    /* followed by program name/command line */
} prog_load_req;

typedef struct {
    trap_error          err;
    trap_phandle        task_id;
    trap_mhandle        mod_handle;
    unsigned_8          flags;
} prog_load_ret;

typedef struct {
    access_req          req;
    trap_phandle        task_id;
} _WCUNALIGNED prog_kill_req;

typedef struct {
    trap_error          err;
} prog_kill_ret;

/*====================== REQ_SET_WATCH =============== */
#define USING_DEBUG_REG         0x80000000UL

typedef struct {
    access_req          req;
    addr48_ptr          watch_addr;
    unsigned_8          size;
} _WCUNALIGNED set_watch_req;

typedef struct {
    trap_error          err;
    unsigned_32         multiplier;
} set_watch_ret;

typedef struct {
    access_req          req;
    addr48_ptr          watch_addr;
    unsigned_8          size;
} _WCUNALIGNED clear_watch_req;

/* no clear_watch_ret */

typedef struct {
    access_req          req;
    addr48_ptr          break_addr;
} _WCUNALIGNED set_break_req;

typedef struct {
    unsigned_32         old;
} set_break_ret;

typedef struct {
    access_req          req;
    addr48_ptr          break_addr;
    unsigned_32         old;
} _WCUNALIGNED clear_break_req;

/* no clear_break_ret */

typedef struct {
    access_req          req;
    unsigned_16         seg;
} _WCUNALIGNED get_next_alias_req;

typedef struct {
    unsigned_16         seg;
    unsigned_16         alias;
} get_next_alias_ret;

typedef struct {
    access_req          req;
} set_user_screen_req;

/* no set_user_screen_ret */

typedef struct {
    access_req          req;
} set_debug_screen_req;

/* no set_debug_screen_ret */

typedef struct {
    access_req          req;
    unsigned_16         wait;
} _WCUNALIGNED read_user_keyboard_req;

typedef struct {
    unsigned_8          key;
} read_user_keyboard_ret;

typedef struct {
    access_req          req;
    trap_mhandle        handle;
} _WCUNALIGNED get_lib_name_req;

typedef struct {
    trap_mhandle        handle;
    /* followed by name string */
} get_lib_name_ret;

typedef struct {
    access_req          req;
    trap_error          err;
} _WCUNALIGNED get_err_text_req;

/* return message for REQ_GET_ERR_TEXT is the error message string */

/*============== REQ_GET_MESSAGE_TEXT ==================*/
enum {
    MSG_NEWLINE         = 0x01,
    MSG_MORE            = 0x02,
    MSG_WARNING         = 0x04,
    MSG_ERROR           = 0x08
};

typedef struct {
    access_req          req;
} get_message_text_req;

typedef struct {
    unsigned_8          flags;
    /* followed by message text */
} get_message_text_ret;

typedef struct {
    access_req          req;
    /* followed by filename to redirect to */
} redirect_stdin_req;

typedef struct {
    trap_error          err;
} redirect_stdin_ret;

typedef redirect_stdin_req      redirect_stdout_req;
typedef redirect_stdin_ret      redirect_stdout_ret;


typedef struct {
    access_req          req;
    /* followed by command string */
} split_cmd_req;

typedef struct {
    unsigned_16         cmd_end;
    unsigned_16         parm_start;
} split_cmd_ret;

/*====================== REQ_READ_REGS =============== */
typedef struct {
    access_req          req;
} read_regs_req;

/* send back appropriate mad_registers union member for machine */

/*====================== REQ_WRITE_REGS ============== */
typedef struct {
    access_req          req;
    /* followed by the appropriate mad_registers union member */
} write_regs_req;

/* no write_regs_ret */

/*====================== REQ_MACHINE_DATA ============ */

typedef struct {
    access_req          req;
    unsigned_8          info_type;
    addr48_ptr          addr;
    /* followed by whatever machine specific data is required */
} _WCUNALIGNED machine_data_req;

typedef struct {
    addr48_off          cache_start;
    addr48_off          cache_end;
    /* followed by whatever machine specific data is being returned */
} machine_data_ret;

#if __WATCOMC__ > 1000
#pragma pack(pop);
#endif

#define TRPCORE_H

#endif
