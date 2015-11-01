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
#include "trpcomm.h"


typedef trap_retval service_func( void );

#if defined(WANT_FILE_INFO)
static service_func * const FileInfoRequests[] = {
    ReqFileInfo_getdate,
    ReqFileInfo_setdate,
};
#endif

#if defined(WANT_ENV)
static service_func * const EnvRequests[] = {
    ReqEnv_getvar,
    ReqEnv_setvar,
};
#endif

#if defined(WANT_FILE)
static service_func * const FileRequests[] = {
    ReqFile_get_config,
    ReqFile_open,
    ReqFile_seek,
    ReqFile_read,
    ReqFile_write,
    ReqFile_write_console,
    ReqFile_close,
    ReqFile_erase,
    ReqFile_string_to_fullpath,
    ReqFile_run_cmd,
};
#endif

#if defined(WANT_OVL)
static service_func * const OvlRequests[] = {
    ReqOvl_state_size,
    ReqOvl_get_data,
    ReqOvl_read_state,
    ReqOvl_write_state,
    ReqOvl_trans_vect_addr,
    ReqOvl_trans_ret_addr,
    ReqOvl_get_remap_entry,
};
#endif

#if defined(WANT_THREAD)
static service_func * const ThreadRequests[] = {
    ReqThread_get_next,
    ReqThread_set,
    ReqThread_freeze,
    ReqThread_thaw,
    ReqThread_get_extra,
};
#endif

#if defined(WANT_RUN_THREAD)
static service_func * const RunThreadRequests[] = {
    ReqRunThread_info,
    ReqRunThread_get_next,
    ReqRunThread_get_runtime,
    ReqRunThread_poll,
    ReqRunThread_set,
    ReqRunThread_get_name,
    ReqRunThread_stop,
    ReqRunThread_signal_stop,
};
#endif

#if defined(WANT_RFX)
static service_func * const RFXRequests[] = {
    ReqRfx_rename,
    ReqRfx_mkdir,
    ReqRfx_rmdir,
    ReqRfx_setdrive,
    ReqRfx_getdrive,
    ReqRfx_setcwd,
    ReqRfx_getcwd,
    ReqRfx_setdatetime,
    ReqRfx_getdatetime,
    ReqRfx_getfreespace,
    ReqRfx_setfileattr,
    ReqRfx_getfileattr,
    ReqRfx_nametocannonical,
    ReqRfx_findfirst,
    ReqRfx_findnext,
    ReqRfx_findclose,
};
#endif

#if defined(WANT_CAPABILITIES)
static service_func * const CapabilitiesRequests[] = {
    ReqCapabilities_get_8b_bp,
    ReqCapabilities_set_8b_bp,
    ReqCapabilities_get_exact_bp,
    ReqCapabilities_set_exact_bp,
};
#endif

#if defined(WANT_ASYNC)
static service_func * const AsyncRequests[] = {
    ReqAsync_go,
    ReqAsync_step,
    ReqAsync_poll,
    ReqAsync_stop,
};
#endif

typedef struct {
    const char *name;
    service_func * const * const vectors;
} service_entry;

static const service_entry  Services[] = {
#if defined(WANT_FILE_INFO)
    { FILE_INFO_SUPP_NAME,  FileInfoRequests },
#endif
#if defined(WANT_ENV)
    { ENV_SUPP_NAME,        EnvRequests },
#endif
#if defined(WANT_FILE)
    { FILE_SUPP_NAME,       FileRequests },
#endif
#if defined(WANT_OVL)
    { OVERLAY_SUPP_NAME,    OvlRequests  },
#endif
#if defined(WANT_THREAD)
    { THREAD_SUPP_NAME,     ThreadRequests },
#endif
#if defined(WANT_RUN_THREAD)
    { RUN_THREAD_SUPP_NAME, RunThreadRequests },
#endif
#if defined(WANT_RFX)
    { RFX_SUPP_NAME,        RFXRequests },
#endif
#if defined(WANT_CAPABILITIES)
    { CAPABILITIES_SUPP_NAME, CapabilitiesRequests },
#endif
#if defined(WANT_ASYNC)
    { ASYNC_SUPP_NAME,      AsyncRequests },
#endif
    { NULL,                 NULL }
};

trap_retval ReqGet_supplementary_service(void)
{
    char                                *name;
    get_supplementary_service_ret       *out;
    unsigned                            i;

    name = GetInPtr( sizeof( get_supplementary_service_req ) );
    out = GetOutPtr( 0 );
    out->err = 0;
    out->id = 0;
    for( i = 0; Services[i].name != NULL; ++i ) {
        if( stricmp( Services[i].name, name ) == 0 ) {
            out->id = i + 1;
            break;
        }
    }
    return( sizeof( *out ) );
}

trap_retval ReqPerform_supplementary_service( void )
{
    access_req      *sup_req;
    trap_shandle    *id;

    id = GetInPtr( sizeof( access_req ) );
    if( *id == 0 )
        return( 0 );
    sup_req = GetInPtr( sizeof( supp_prefix ) );
    return( Services[*id - 1].vectors[*sup_req]() );
}
