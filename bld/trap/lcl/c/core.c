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


#include <string.h>
#include "trpimp.h"

static unsigned (* const CoreRequests[])(void) = {
        ReqConnect,
        ReqDisconnect,
        ReqSuspend,
        ReqResume,
        ReqGet_supplementary_service,
        ReqPerform_supplementary_service,
        ReqGet_sys_config,
        ReqMap_addr,
        ReqAddr_info,   //obsolete
        ReqChecksum_mem,
        ReqRead_mem,
        ReqWrite_mem,
        ReqRead_io,
        ReqWrite_io,
        ReqRead_cpu,    //obsolete
        ReqRead_fpu,    //obsolete
        ReqWrite_cpu,   //obsolete
        ReqWrite_fpu,   //obsolete
        ReqProg_go,
        ReqProg_step,
        ReqProg_load,
        ReqProg_kill,
        ReqSet_watch,
        ReqClear_watch,
        ReqSet_break,
        ReqClear_break,
        ReqGet_next_alias,
        ReqSet_user_screen,
        ReqSet_debug_screen,
        ReqRead_user_keyboard,
        ReqGet_lib_name,
        ReqGet_err_text,
        ReqGet_message_text,
        ReqRedirect_stdin,
        ReqRedirect_stdout,
        ReqSplit_cmd,
        ReqRead_regs,
        ReqWrite_regs,
        ReqMachine_data,
};


extern unsigned         MaxPacketSize(void);
extern void             RemoteDisco();
extern char             RemoteConnect(void);

extern char             *InitDebugging(void);
extern void             FinishDebugging(void);
extern void             far pascal SetEventHook( void far * );
extern void             far *HookRtn;

#if defined(DOSXTRAP) || defined(DOSXHELP)
    #define DOSX
#elif defined(__WINDOWS__) && !defined(__386__)
    #define WIN16
#endif

unsigned ReqConnect()
{
    connect_ret *ret;
    char        *err;

    ret = GetOutPtr(0);
#if defined(DOSX)
    ret->max_msg_size = MaxPacketSize();
    RemoteConnect();
#else
    ret->max_msg_size = 0xFFFF;
#endif
    /* version ok... already checked by initialization  */
    err = GetOutPtr(sizeof(*ret));
#if defined(WIN16)
    strcpy( err, InitDebugging() );
#else
    *err = '\0';
#endif
    return( sizeof( *ret ) + strlen( err ) + 1 );
}

unsigned ReqDisconnect()
{
#if defined(DOSX)
    RemoteDisco();
#elif defined(WIN16)
    FinishDebugging();
    SetEventHook( NULL );
    HookRtn = NULL;
#endif
    return( 0 );
}

unsigned ReqSuspend()
{
    return( 0 );
}

unsigned ReqResume()
{
    return( 0 );
}

unsigned TRAPENTRY TrapRequest( unsigned num_in_mx, mx_entry *mx_in,
                            unsigned num_out_mx, mx_entry *mx_out )
{
    unsigned    len;

    In_Mx_Num = num_in_mx;
    Out_Mx_Num = num_out_mx;
    In_Mx_Ptr = mx_in;
    Out_Mx_Ptr = mx_out;

#if defined(WIN16)
    if( HookRtn != NULL ) SetEventHook( NULL );
#endif
    /* The first item must be the request! */
    len = CoreRequests[ *(access_req *)mx_in[0].ptr ]();
#if defined(WIN16)
    if( HookRtn != NULL ) SetEventHook( HookRtn );
#endif
    return( len );
}
