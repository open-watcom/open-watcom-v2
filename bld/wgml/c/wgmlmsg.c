/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML message definition.
*               adapted from wlink file (bld\wl\c\wlnkmsg.c)
*
****************************************************************************/


#include "wgml.h"
#include "wreslang.h"
#include "wgmlmsg.rh"
#ifdef USE_WRESLIB
    #include "wressetr.h"
    #include "wresset2.h"
#else
    #include <windows.h>
#endif

#include "clibext.h"


#ifdef USE_WRESLIB
static HANDLE_INFO  hInstance = { 0 };
#else
static HINSTANCE    hInstance;
#endif
static unsigned     msgShift;               // 0 = english, 1000 for japanese


/***************************************************************************/
/*  initialize messages from resource file                                 */
/***************************************************************************/

bool init_msgs( void )
{
#ifdef USE_WRESLIB
    char        fname[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( fname ) != NULL && OpenResFile( &hInstance, fname ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( get_msg( ERR_DUMMY, fname, sizeof( fname ) ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    out_msg( NO_RES_MESSAGE "\n" );
    g_suicide();
    return( false );
#else
    hInstance = GetModuleHandle( NULL );
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
    return( true );
#endif
}


/***************************************************************************/
/*  get a msg text string                                                  */
/***************************************************************************/

bool get_msg( msg_ids resid, char *buff, size_t buff_len )
{
#ifdef USE_WRESLIB
    if( hInstance.status == 0 || WResLoadString( &hInstance, resid + msgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#else
    if( LoadString( hInstance, resid + msgShift, buff, (int)buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

/***************************************************************************/
/*  end of msg processing                                                  */
/***************************************************************************/

void fini_msgs( void )
{
#ifdef USE_WRESLIB
    CloseResFile( &hInstance );
#else
    CloseHandle( hInstance );
#endif
}
