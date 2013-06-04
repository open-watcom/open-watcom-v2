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
* Description:  Internal debugger trap file OS specific implementation
*               header file.
*
****************************************************************************/


#ifndef TRPIMP_H
#define TRPIMP_H

#include "trpcore.h"

extern trap_retval ReqConnect(void);
extern trap_retval ReqDisconnect(void);
extern trap_retval ReqSuspend(void);
extern trap_retval ReqResume(void);
extern trap_retval ReqGet_supplementary_service(void);
extern trap_retval ReqPerform_supplementary_service(void);
extern trap_retval ReqGet_sys_config(void);
extern trap_retval ReqMap_addr(void);      //obsolete
extern trap_retval ReqAddr_info(void);
extern trap_retval ReqChecksum_mem(void);
extern trap_retval ReqRead_mem(void);
extern trap_retval ReqWrite_mem(void);
extern trap_retval ReqRead_io(void);
extern trap_retval ReqWrite_io(void);
extern trap_retval ReqRead_cpu(void);      //obsolete
extern trap_retval ReqRead_fpu(void);      //obsolete
extern trap_retval ReqWrite_cpu(void);     //obsolete
extern trap_retval ReqWrite_fpu(void);     //obsolete
extern trap_retval ReqProg_go(void);
extern trap_retval ReqProg_step(void);
extern trap_retval ReqProg_load(void);
extern trap_retval ReqProg_kill(void);
extern trap_retval ReqSet_watch(void);
extern trap_retval ReqClear_watch(void);
extern trap_retval ReqSet_break(void);
extern trap_retval ReqClear_break(void);
extern trap_retval ReqGet_next_alias(void);
extern trap_retval ReqSet_user_screen(void);
extern trap_retval ReqSet_debug_screen(void);
extern trap_retval ReqRead_user_keyboard(void);
extern trap_retval ReqGet_lib_name(void);
extern trap_retval ReqGet_err_text(void);
extern trap_retval ReqGet_message_text(void);
extern trap_retval ReqRedirect_stdin(void);
extern trap_retval ReqRedirect_stdout(void);
extern trap_retval ReqSplit_cmd(void);
extern trap_retval ReqRead_regs(void);
extern trap_retval ReqWrite_regs(void);
extern trap_retval ReqMachine_data(void);

#ifdef WANT_FILE_INFO
#include "trpfinfo.h"

extern trap_retval ReqFileInfo_getdate(void);
extern trap_retval ReqFileInfo_setdate(void);

#endif

#ifdef WANT_ENV
#include "trpenv.h"

extern trap_retval ReqEnv_getvar(void);
extern trap_retval ReqEnv_setvar(void);

#endif

#ifdef WANT_ASYNC
#include "trpasync.h"

extern trap_retval ReqAsync_go(void);
extern trap_retval ReqAsync_step(void);
extern trap_retval ReqAsync_poll(void);
extern trap_retval ReqAsync_stop(void);

#endif

#ifdef WANT_FILE
#include "trpfile.h"

extern trap_retval ReqFile_get_config(void);
extern trap_retval ReqFile_open(void);
extern trap_retval ReqFile_seek(void);
extern trap_retval ReqFile_read(void);
extern trap_retval ReqFile_write(void);
extern trap_retval ReqFile_write_console(void);
extern trap_retval ReqFile_close(void);
extern trap_retval ReqFile_erase(void);
extern trap_retval ReqFile_string_to_fullpath(void);
extern trap_retval ReqFile_run_cmd(void);

#endif

#ifdef WANT_OVL
#include "trpovl.h"

extern trap_retval ReqOvl_state_size(void);
extern trap_retval ReqOvl_get_data(void);
extern trap_retval ReqOvl_read_state(void);
extern trap_retval ReqOvl_write_state(void);
extern trap_retval ReqOvl_trans_vect_addr(void);
extern trap_retval ReqOvl_trans_ret_addr(void);
extern trap_retval ReqOvl_get_remap_entry(void);

#endif

#ifdef WANT_THREAD
#include "trpthrd.h"

extern trap_retval ReqThread_get_next(void);
extern trap_retval ReqThread_set(void);
extern trap_retval ReqThread_freeze(void);
extern trap_retval ReqThread_thaw(void);
extern trap_retval ReqThread_get_extra(void);

#endif

#ifdef WANT_RUN_THREAD
#include "trprtrd.h"

extern trap_retval ReqRunThread_info(void);
extern trap_retval ReqRunThread_get_next(void);
extern trap_retval ReqRunThread_get_runtime(void);
extern trap_retval ReqRunThread_poll(void);
extern trap_retval ReqRunThread_set(void);
extern trap_retval ReqRunThread_get_name(void);
extern trap_retval ReqRunThread_stop(void);
extern trap_retval ReqRunThread_signal_stop(void);

#endif

#ifdef WANT_RFX
#include "trprfx.h"

extern trap_retval ReqRfx_rename(void);
extern trap_retval ReqRfx_mkdir(void);
extern trap_retval ReqRfx_rmdir(void);
extern trap_retval ReqRfx_setdrive(void);
extern trap_retval ReqRfx_getdrive(void);
extern trap_retval ReqRfx_setcwd(void);
extern trap_retval ReqRfx_getcwd(void);
extern trap_retval ReqRfx_setdatetime(void);
extern trap_retval ReqRfx_getdatetime(void);
extern trap_retval ReqRfx_getfreespace(void);
extern trap_retval ReqRfx_setfileattr(void);
extern trap_retval ReqRfx_getfileattr(void);
extern trap_retval ReqRfx_nametocannonical(void);
extern trap_retval ReqRfx_findfirst(void);
extern trap_retval ReqRfx_findnext(void);
extern trap_retval ReqRfx_findclose(void);

#endif

#ifdef WANT_CAPABILITIES
#include "trpcapb.h"

extern trap_retval ReqCapabilities_get_8b_bp(void);
extern trap_retval ReqCapabilities_set_8b_bp(void);
extern trap_retval ReqCapabilities_get_exact_bp(void);
extern trap_retval ReqCapabilities_set_exact_bp(void);

#endif

extern trap_elen        In_Mx_Num;
extern trap_elen        Out_Mx_Num;
extern mx_entry_p       In_Mx_Ptr;
extern mx_entry_p       Out_Mx_Ptr;

extern void             *GetInPtr( trap_elen );
extern void             *GetOutPtr( trap_elen );
extern trap_elen        GetTotalSize( void );

#define BUFF_SIZE       256

extern trap_init_func   TrapInit;
extern trap_req_func    TrapRequest;
extern trap_fini_func   TrapFini;

#endif
