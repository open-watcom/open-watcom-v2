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


#ifndef TRPIMP_H

#define TRPIMP_H

#if defined(__386__)
    #define TRAPFAR
#elif defined(__ALPHA__) || defined(__PPC__)
    #define TRAPFAR
#elif defined(M_I86)
    #define TRAPFAR __far
#else
    #error TRAPFAR macro not configured
#endif

#if defined(__DOS__) || defined(__DSX__)
    #if defined(DOSXHELP)
        /* protected mode helper */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #undef          WANT_FILE
        #undef          WANT_OVL
        #undef          WANT_THREAD
        #undef          WANT_RFX
    #elif defined(DOSXTRAP)
        /* real mode trap file talking to protected mode helper */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #define         WANT_FILE
        #undef          WANT_OVL
        #undef          WANT_THREAD
        #define         WANT_RFX
    #else
        /* straight dos */
        #undef          WANT_FILE_INFO
        #undef          WANT_ENV
        #undef          WANT_ASYNC
        #define         WANT_FILE
        #define         WANT_OVL
        #undef          WANT_THREAD
        #define         WANT_RFX
    #endif
    #define     TRAPENTRY TRAPFAR __saveregs
#elif defined(__OS2__)
    #undef          WANT_FILE_INFO
    #undef          WANT_ENV
    #undef          WANT_ASYNC
    #define     WANT_FILE
    #undef          WANT_OVL
    #define         WANT_THREAD
    #define     WANT_RFX
    #define     TRAPENTRY TRAPFAR __saveregs
#elif defined(__OS2V2__)
    #undef          WANT_FILE_INFO
    #undef          WANT_ENV
    #undef          WANT_ASYNC
    #define     WANT_FILE
    #undef          WANT_OVL
    #define         WANT_THREAD
    #define     WANT_RFX
    #define     TRAPENTRY TRAPFAR
#elif defined(__NT__)
    #if defined(JVMXHELP) || defined(MSJXHELP)
        #undef  WANT_FILE_INFO
        #undef  WANT_ENV
        #undef  WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RFX
        #define TRAPENTRY TRAPFAR
    #elif defined(MSJXTRAP)
        #define WANT_FILE_INFO
        #define WANT_ENV
        #define WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RFX
        #define TRAPENTRY TRAPFAR
    #else
        #define WANT_FILE_INFO
        #define WANT_ENV
        #define WANT_ASYNC
        #define WANT_FILE
        #undef  WANT_OVL
        #define WANT_THREAD
        #undef  WANT_RFX
        #define TRAPENTRY TRAPFAR
    #endif
#elif defined(__WINDOWS__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #undef      WANT_THREAD
    #define     WANT_RFX
    #define     TRAPENTRY TRAPFAR __pascal
#elif defined(__QNX__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #define     WANT_THREAD
    #undef      WANT_RFX
    #define     TRAPENTRY TRAPFAR __saveregs
#elif defined(__NETWARE__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #define     WANT_THREAD
    #undef      WANT_RFX
    #define     TRAPENTRY TRAPFAR
#elif defined(__PENPOINT__)
    #undef      WANT_FILE_INFO
    #undef      WANT_ENV
    #undef      WANT_ASYNC
    #define     WANT_FILE
    #undef      WANT_OVL
    #define     WANT_THREAD
    #undef      WANT_RFX
    #define     TRAPENTRY TRAPFAR
#else
    #error Unknown operating system
#endif

#include "trpcore.h"

extern unsigned ReqConnect(void);
extern unsigned ReqDisconnect(void);
extern unsigned ReqSuspend(void);
extern unsigned ReqResume(void);
extern unsigned ReqGet_supplementary_service(void);
extern unsigned ReqPerform_supplementary_service(void);
extern unsigned ReqGet_sys_config(void);
extern unsigned ReqMap_addr(void);      //obsolete
extern unsigned ReqAddr_info(void);
extern unsigned ReqChecksum_mem(void);
extern unsigned ReqRead_mem(void);
extern unsigned ReqWrite_mem(void);
extern unsigned ReqRead_io(void);
extern unsigned ReqWrite_io(void);
extern unsigned ReqRead_cpu(void);      //obsolete
extern unsigned ReqRead_fpu(void);      //obsolete
extern unsigned ReqWrite_cpu(void);     //obsolete
extern unsigned ReqWrite_fpu(void);     //obsolete
extern unsigned ReqProg_go(void);
extern unsigned ReqProg_step(void);
extern unsigned ReqProg_load(void);
extern unsigned ReqProg_kill(void);
extern unsigned ReqSet_watch(void);
extern unsigned ReqClear_watch(void);
extern unsigned ReqSet_break(void);
extern unsigned ReqClear_break(void);
extern unsigned ReqGet_next_alias(void);
extern unsigned ReqSet_user_screen(void);
extern unsigned ReqSet_debug_screen(void);
extern unsigned ReqRead_user_keyboard(void);
extern unsigned ReqGet_lib_name(void);
extern unsigned ReqGet_err_text(void);
extern unsigned ReqGet_message_text(void);
extern unsigned ReqRedirect_stdin(void);
extern unsigned ReqRedirect_stdout(void);
extern unsigned ReqSplit_cmd(void);
extern unsigned ReqRead_regs(void);
extern unsigned ReqWrite_regs(void);
extern unsigned ReqMachine_data(void);

#ifdef WANT_FILE_INFO
#include "trpfinfo.h"

extern unsigned  ReqFileInfo_getdate(void);
extern unsigned  ReqFileInfo_setdate(void);

#endif

#ifdef WANT_ENV
#include "trpenv.h"

extern unsigned  ReqEnv_getvar(void);
extern unsigned  ReqEnv_setvar(void);

#endif

#ifdef WANT_ASYNC
#include "trpasync.h"

extern unsigned  ReqAsync_go(void);
extern unsigned  ReqAsync_stop(void);
extern unsigned  ReqAsync_interrupt(void);
extern unsigned  ReqAsync_poll(void);
extern unsigned  ReqAsync_check(void);

#endif

#ifdef WANT_FILE
#include "trpfile.h"

extern unsigned  ReqFile_get_config(void);
extern unsigned  ReqFile_open(void);
extern unsigned  ReqFile_seek(void);
extern unsigned  ReqFile_read(void);
extern unsigned  ReqFile_write(void);
extern unsigned  ReqFile_write_console(void);
extern unsigned  ReqFile_close(void);
extern unsigned  ReqFile_erase(void);
extern unsigned  ReqFile_string_to_fullpath(void);
extern unsigned  ReqFile_run_cmd(void);

#endif

#ifdef WANT_OVL
#include "trpovl.h"

extern unsigned ReqOvl_state_size(void);
extern unsigned ReqOvl_get_data(void);
extern unsigned ReqOvl_read_state(void);
extern unsigned ReqOvl_write_state(void);
extern unsigned ReqOvl_trans_vect_addr(void);
extern unsigned ReqOvl_trans_ret_addr(void);
extern unsigned ReqOvl_get_remap_entry(void);

#endif

#ifdef WANT_THREAD
#include "trpthrd.h"

extern unsigned ReqThread_get_next(void);
extern unsigned ReqThread_set(void);
extern unsigned ReqThread_freeze(void);
extern unsigned ReqThread_thaw(void);
extern unsigned ReqThread_get_extra(void);

#endif

#ifdef WANT_RFX
#include "trprfx.h"

extern unsigned ReqRfx_rename(void);
extern unsigned ReqRfx_mkdir(void);
extern unsigned ReqRfx_rmdir(void);
extern unsigned ReqRfx_setdrive(void);
extern unsigned ReqRfx_getdrive(void);
extern unsigned ReqRfx_setcwd(void);
extern unsigned ReqRfx_getcwd(void);
extern unsigned ReqRfx_setdatetime(void);
extern unsigned ReqRfx_getdatetime(void);
extern unsigned ReqRfx_getfreespace(void);
extern unsigned ReqRfx_setfileattr(void);
extern unsigned ReqRfx_getfileattr(void);
extern unsigned ReqRfx_nametocannonical(void);
extern unsigned ReqRfx_findfirst(void);
extern unsigned ReqRfx_findnext(void);
extern unsigned ReqRfx_findclose(void);

#endif

extern unsigned_8       In_Mx_Num;
extern unsigned_8       Out_Mx_Num;
extern mx_entry         *In_Mx_Ptr;
extern mx_entry         *Out_Mx_Ptr;

extern void             *GetInPtr( unsigned );
extern void             *GetOutPtr( unsigned );
extern unsigned         GetTotalSize( void );

#include "bool.h"

#define BUFF_SIZE       256

extern trap_version TRAPENTRY TrapInit( char *, char *, unsigned_8 );
extern unsigned     TRAPENTRY TrapRequest( unsigned, mx_entry *,
                                          unsigned, mx_entry * );
extern void         TRAPENTRY TrapFini( void );

#endif
