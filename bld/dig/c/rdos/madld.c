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
#include <rdos.h>
#include "mad.h"
#include "madimp.h"
#include "madcli.h"

mad_status MADSysLoad( char *path, mad_client_routines *cli,
                                mad_imp_routines **imp, unsigned long *sys_hdl )
{
    int                 dll;
    mad_imp_routines    *(*init_func)( mad_status *, mad_client_routines * );
    char                newpath[256];
    mad_status          status;

    strcpy( newpath, path );
    strcat( newpath, ".dll" );
    dll = RdosLoadDll( newpath );
    if( dll == NULL ) {
        return( MS_ERR|MS_FOPEN_FAILED );
    }
    init_func = RdosGetModuleProc( dll, "MADLOAD" );
    if( init_func == NULL ) {
        RdosFreeDll( dll );
        return( MS_ERR|MS_INVALID_MAD );
    }
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
        RdosFreeDll( dll );
        return( status );
    }
    *sys_hdl = (unsigned long)dll;
    return( MS_OK );
}

void MADSysUnload( unsigned long sys_hdl )
{
    RdosFreeDll( (int)sys_hdl );
}
