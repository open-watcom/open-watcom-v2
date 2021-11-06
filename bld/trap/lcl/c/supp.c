/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
    #define REQ_FILE_INFO_DEF(sym,func)     TRAP_FILE_INFO( func ),
    REQ_FILE_INFO_DEFS()
    #undef REQ_FILE_INFO_DEF
};
#endif

#if defined(WANT_ENV)
static service_func * const EnvRequests[] = {
    #define REQ_ENV_DEF(sym,func)           TRAP_ENV( func ),
    REQ_ENV_DEFS()
    #undef REQ_ENV_DEF
};
#endif

#if defined(WANT_FILE)
static service_func * const FileRequests[] = {
    #define REQ_FILE_DEF(sym,func)          TRAP_FILE( func ),
    REQ_FILE_DEFS()
    #undef REQ_FILE_DEF
};
#endif

#if defined(WANT_OVL)
static service_func * const OvlRequests[] = {
    #define REQ_OVL_DEF(sym,func)           TRAP_OVERLAY( func ),
    REQ_OVL_DEFS()
    #undef REQ_OVL_DEF
};
#endif

#if defined(WANT_THREAD)
static service_func * const ThreadRequests[] = {
    #define REQ_THREAD_DEF(sym,func)        TRAP_THREAD( func ),
    REQ_THREAD_DEFS()
    #undef REQ_THREAD_DEF
};
#endif

#if defined(WANT_RUN_THREAD)
static service_func * const RunThreadRequests[] = {
    #define REQ_RUN_THREAD_DEF(sym,func)    TRAP_RUN_THREAD( func ),
    REQ_RUN_THREAD_DEFS()
    #undef REQ_RUN_THREAD_DEF
};
#endif

#if defined(WANT_RFX)
static service_func * const RFXRequests[] = {
    #define REQ_RFX_DEF(sym,func)           TRAP_RFX( func ),
    REQ_RFX_DEFS()
    #undef REQ_RFX_DEF
};
#endif

#if defined(WANT_CAPABILITIES)
static service_func * const CapabilitiesRequests[] = {
    #define REQ_CAPABILITIES_DEF(sym,func)  TRAP_CAPABILITIES( func ),
    REQ_CAPABILITIES_DEFS()
    #undef REQ_CAPABILITIES_DEF
};
#endif

#if defined(WANT_ASYNC)
static service_func * const AsyncRequests[] = {
    #define REQ_ASYNC_DEF(sym,func)         TRAP_ASYNC( func ),
    REQ_ASYNC_DEFS()
    #undef REQ_ASYNC_DEF
};
#endif

typedef struct {
    const char *name;
    service_func * const * const vectors;
} service_entry;

static const service_entry  Services[] = {
#if defined(WANT_FILE_INFO)
    { QUOTED( FILE_INFO_SUPP_NAME ),  FileInfoRequests },
#endif
#if defined(WANT_ENV)
    { QUOTED( ENV_SUPP_NAME ),        EnvRequests },
#endif
#if defined(WANT_FILE)
    { QUOTED( FILE_SUPP_NAME ),       FileRequests },
#endif
#if defined(WANT_OVL)
    { QUOTED( OVERLAY_SUPP_NAME ),    OvlRequests  },
#endif
#if defined(WANT_THREAD)
    { QUOTED( THREAD_SUPP_NAME ),     ThreadRequests },
#endif
#if defined(WANT_RUN_THREAD)
    { QUOTED( RUN_THREAD_SUPP_NAME ), RunThreadRequests },
#endif
#if defined(WANT_RFX)
    { QUOTED( RFX_SUPP_NAME ),        RFXRequests },
#endif
#if defined(WANT_CAPABILITIES)
    { QUOTED( CAPABILITIES_SUPP_NAME ), CapabilitiesRequests },
#endif
#if defined(WANT_ASYNC)
    { QUOTED( ASYNC_SUPP_NAME ),      AsyncRequests },
#endif
    { NULL,                 NULL }
};

trap_retval TRAP_CORE( Get_supplementary_service )(void)
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

trap_retval TRAP_CORE( Perform_supplementary_service )( void )
{
    access_req      *sup_req;
    trap_shandle    *id;

    id = GetInPtr( sizeof( access_req ) );
    if( *id == 0 )
        return( 0 );
    sup_req = GetInPtr( sizeof( supp_prefix ) );
    return( Services[*id - 1].vectors[*sup_req]() );
}
