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
#include "pillink.h"
#include "pillimp.h"
#include "pillctrl.h"

int PILLSysLoad( const char *path, const pill_client_routines *cli,
                link_handle *lh, link_message *msg )
{
    int                         dll;
    char                        newpath[256];
    const pill_imp_routines     *(*init_func)( const pill_client_routines *, link_message * );

    msg->source = NULL;
    msg->id = LM_SYSTEM_ERROR;
    strcpy( newpath, path );
    strcat( newpath, ".dll" );
    dll = RdosLoadDll( newpath );
    if( dll == NULL ) {
        msg->data.code = -1;
        return( 0 );
    }
    init_func = RdosGetModuleProc( dll, "PILLLOAD" );
    if( init_func == NULL ) {
        msg->data.code = -1;
        RdosFreeDll( dll );
        return( 0 );
    }
    lh->sys = (void *)dll;
    lh->rtns = init_func( cli, msg );
    if( lh->rtns == NULL ) {
        /* don't free DLL yet, we need the message processor */
        msg->source = lh;
        return( 0 );
    }
    return( 1 );
}

void PILLSysUnload( link_handle *lh )
{
    RdosFreeDll( (int)lh->sys );
    lh->sys = NULL;
}

void PILLSysNoMem( link_message *msg )
{
    msg->source = NULL;
    msg->id = LM_SYSTEM_ERROR;
    msg->data.code = ERROR_NOT_ENOUGH_MEMORY;
}

pill_private_func *PILLSysFixFunc( link_handle *lh, pill_private_func *func )
{
    return( func );
}

void PILLSysTriggerSet( link_instance *li )
{
}
