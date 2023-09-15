/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
#include <windows.h>
#include "pillink.h"
#include "pillimp.h"
#include "pillctrl.h"


int PILLSysLoad( const char *base_name, const pill_client_routines *cli,
                link_handle *lh, link_message *msg )
{
    HANDLE                  mod_hdl;
    char                    filename[256];
    pill_init_func          *init_func;

    msg->source = NULL;
    msg->id = LM_SYSTEM_ERROR;
    if( DIGLoader( Find )( DIG_FILETYPE_DBG, base_name, 0, ".dll", filename, sizeof( filename ) ) == 0 ) {
        return( 0 );
    }
    mod_hdl = LoadLibrary( filename );
    if( mod_hdl == NULL ) {
        msg->data.code = GetLastError();
        return( 0 );
    }
    init_func = (pill_init_func *)GetProcAddress( mod_hdl, "PILLLOAD" );
    if( init_func == NULL ) {
        msg->data.code = GetLastError();
        FreeLibrary( mod_hdl );
        return( 0 );
    }
    lh->sys = (void *)mod_hdl;
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
    FreeLibrary( (HINSTANCE)lh->sys );
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
