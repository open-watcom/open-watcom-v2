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
* Description:  Module to load DIP libraries in PharLap REX format.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"

#include "ldimp.h"
#include "dbgmod.h"

extern  int      FullPathOpen( char const *name, char *ext, char *result, unsigned max_result );

#define DIPSIG  0x00504944UL

static int PathOpenDIP( char const *name, unsigned len, char *ext, char *dip_name, int dip_name_len )
{
    char    path[_MAX_PATH];

    len = min(len,sizeof(path));
    memcpy( path, name, len );
    path[ len ] = '\0';
    return( FullPathOpen( path, ext, dip_name, dip_name_len ) );
}

void DIPSysUnload( unsigned long sys_hdl )
{
    /* We should unload the symbols here but it's not worth the trouble */
    DIGCliFree( (void *)sys_hdl );
}

dip_status DIPSysLoad( char *path, dip_client_routines *cli,
                                dip_imp_routines **imp, unsigned long *sys_hdl )
{
    int                 h;
    imp_header          *dip;
    dip_imp_routines    *(*init_func)( dip_status *, dip_client_routines * );
    dip_status          status;
    char                dip_name[_MAX_PATH];

    h = PathOpenDIP( path, strlen( path ), "dip", dip_name, sizeof( dip_name ) );
    if( h < 0 ) {
        return( DS_ERR|DS_FOPEN_FAILED );
    }
    dip = ReadInImp( h );
    DIGCliClose( h );
    if( dip == NULL || dip->sig != DIPSIG ) {
        return( DS_ERR|DS_INVALID_DIP );
    }
#ifdef WATCOM_DEBUG_SYMBOLS
    /* Look for symbols in separate .sym files, not the .dip itself */
    strcpy( dip_name + strlen( dip_name ) - 4, ".sym" );
    NotifyWDLoad( dip_name, (unsigned long)dip );
#endif
    init_func = (void *)dip->init_rtn;
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
#ifdef WATCOM_DEBUG_SYMBOLS
        NotifyWDUnload( dip_name );
#endif
        DIPSysUnload( (unsigned long)dip );
        return( status );
    }
    *sys_hdl = (unsigned long)dip;
    return( DS_OK );
}
