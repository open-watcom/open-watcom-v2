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

#include "ldimp.h"

extern  int      PathOpen(char *,unsigned, char *);

#define MADSIG  0x0044414DUL

void MADSysUnload( unsigned long sys_hdl )
{
    DIGCliFree( (void *)sys_hdl );
}

mad_status MADSysLoad( char *path, mad_client_routines *cli,
                                mad_imp_routines **imp, unsigned long *sys_hdl )
{
    int                 h;
    imp_header          *mad;
    mad_imp_routines    *(*init_func)( mad_status *, mad_client_routines * );
    mad_status          status;

    h = PathOpen( path, strlen( path ), "mad" );
    if( h < 0 ) {
        return( MS_ERR|MS_FOPEN_FAILED );
    }
    mad = ReadInImp( h );
    DIGCliClose( h );
    if( mad == NULL || mad->sig != MADSIG ) {
        return( MS_ERR|MS_INVALID_MAD );
    }
    init_func = (void *)mad->init_rtn;
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
        MADSysUnload( (unsigned long)mad );
        return( status );
    }
    *sys_hdl = (unsigned long)mad;
    return( MS_OK );
}
