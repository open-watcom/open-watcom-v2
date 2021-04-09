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
#include "packet.h"

#if defined( DOSXTRAP ) || defined( DOSXHELP )
    #define DOSX
    #include "dosxlink.h"
#elif defined( __WINDOWS__ ) && defined( _M_I86 )
    #define WIN16
    #include "winctrl.h"
#endif

static trap_retval (* const CoreRequests[])(void) = {
    #define pick(sym,dumbfunc,stdfunc)  TRAP_CORE( stdfunc ),
    #include "_trpreq.h"
    #undef pick
};

trap_retval TRAP_CORE( Connect )( void )
{
    connect_ret *ret;
    char        *err;

    ret = GetOutPtr(0);
#if defined( DOSX )
    ret->max_msg_size = MaxPacketSize();
    RemoteConnect();
#else
    ret->max_msg_size = 0xFFFF;
#endif
    /* version ok... already checked by initialization  */
    err = GetOutPtr(sizeof(*ret));
#if defined( WIN16 )
    strcpy( err, InitDebugging() );
#else
    *err = '\0';
#endif
    return( sizeof( *ret ) + strlen( err ) + 1 );
}

trap_retval TRAP_CORE( Disconnect )( void )
{
#if defined( DOSX )
    RemoteDisco();
#elif defined( WIN16 )
    FinishDebugging();
#endif
    return( 0 );
}

trap_retval TRAP_CORE( Suspend )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Resume )( void )
{
    return( 0 );
}

trap_retval TRAPENTRY TrapRequest( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_retval     result;

    In_Mx_Num = num_in_mx;
    Out_Mx_Num = num_out_mx;
    In_Mx_Ptr = mx_in;
    Out_Mx_Ptr = mx_out;

#if defined( WIN16 )
    DisableHookEvents();
#endif
    /* The first item must be the request! */
    result = CoreRequests[TRP_REQUEST( mx_in )]();
#if defined( WIN16 )
    EnableHookEvents();
#endif
    return( result );
}
