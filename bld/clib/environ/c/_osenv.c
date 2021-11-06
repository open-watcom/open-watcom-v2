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
* Description:  Narrow/Wide character version of OS Environment update function.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#endif
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "_environ.h"


int __F_NAME(__os_env_update_narrow,__os_env_update_wide)( const CHAR_TYPE *name, const CHAR_TYPE *value )
{
#ifdef __NT__
    /*** Update the process environment if using Win32 ***/
    if( __lib_SetEnvironmentVariable( name, value ) == FALSE ) {
        return( -1 );
    }
#elif defined( __RDOS__ )
    /*** Update the process environment if using RDOS ***/
    int handle;

    handle = RdosOpenProcessEnv();
    RdosDeleteEnvVar( handle, name );
    if( value != NULL && *value != NULLCHAR )
        RdosAddEnvVar( handle, name, value );
    RdosCloseEnv( handle );
#elif defined( __RDOSDEV__ )
    /*** Update the process environment if using RDOSDEV ***/
    int handle;

    handle = RdosOpenSysEnv();
    RdosDeleteEnvVar( handle, name );
    if( value != NULL && *value != NULLCHAR )
        RdosAddEnvVar( handle, name, value );
    RdosCloseEnv( handle );
#endif
    return( 0 );
}
