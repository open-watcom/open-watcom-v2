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
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"

#include "ldimp.h"

extern  int      PathOpen(char *,unsigned, char *);

#define DIPSIG  0x00504944UL

dip_status DIPSysLoad( char *path, dip_client_routines *cli,
                                dip_imp_routines **imp, unsigned long *sys_hdl )
{
    int                 h;
    imp_header          *dip;
    dip_imp_routines    *(*init_func)( dip_status *, dip_client_routines * );
    dip_status          status;

    h = PathOpen( path, strlen( path ), "dip" );
    if( h < 0 ) {
        return( DS_ERR|DS_FOPEN_FAILED );
    }
    dip = ReadInImp( h );
    DIGCliClose( h );
    if( dip == NULL || dip->sig != DIPSIG ) {
        return( DS_ERR|DS_INVALID_DIP );
    }
    init_func = (void *)dip->init_rtn;
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
        DIPSysUnload( (unsigned long)dip );
        return( status );
    }
    *sys_hdl = (unsigned long)dip;
    return( DS_OK );
}

void DIPSysUnload( unsigned long sys_hdl )
{
    DIGCliFree( (void *)sys_hdl );
}
