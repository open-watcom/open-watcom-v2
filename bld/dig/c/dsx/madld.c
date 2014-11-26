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
#include "mad.h"
#include "madimp.h"
#include "madcli.h"
#include "madsys.h"
#include "ldimp.h"
#include "digio.h"

#define MADSIG  0x0044414DUL    // "MAD"

void MADSysUnload( mad_sys_handle *sys_hdl )
{
    DIGCliFree( *sys_hdl );
}

mad_status MADSysLoad( const char *path, mad_client_routines *cli,
                                mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    dig_fhandle         h;
    imp_header          *mad;
    mad_init_func       *init_func;
    mad_status          status;

    h = DIGPathOpen( path, strlen( path ), "mad", NULL, 0 );
    if( h == DIG_NIL_HANDLE ) {
        return( MS_ERR|MS_FOPEN_FAILED );
    }
    mad = ReadInImp( h );
    DIGPathClose( h );
    status = MS_ERR|MS_INVALID_MAD;
    if( mad != NULL ) {
#ifdef __WATCOMC__
        if( mad->sig == MADSIG ) {
#endif
            init_func = (mad_init_func *)mad->init_rtn;
            if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
                *sys_hdl = (mad_sys_handle)mad;
                return( MS_OK );
            }
#ifdef __WATCOMC__
        }
#endif
        DIGCliFree( (void *)mad );
    }
    return( status );
}
