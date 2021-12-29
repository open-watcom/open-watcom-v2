/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Read resources data from file
*
****************************************************************************/


// a lot of the contents of this file were liberally lifted from
// wlmsg.c in the WLIB project -- Wes

#include "guiwind.h"
#include "guiextnm.h"
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include "watcom.h"
#include "wresdefn.h"
#include "wressetr.h"
#include "wresset2.h"
#include "filefmt.h"
#include "resdiag.h"
#include "resmenu.h"
#include "seekres.h"
#include "guildstr.h"


static  HANDLE_INFO     hInstance = { 0 };

bool GUIAPI GUIIsLoadStrInitialized( void )
{
    return( hInstance.status != 0 );
}

bool GUIAPI GUILoadStrInit( const char *fname )
{
    hInstance.status = 0;
    if( OpenResFileX( &hInstance, fname, GUIGetResFileName() != NULL ) ) {
        // if we are using an external resource file then we don't have to search
        return( true );
    }
    CloseResFile( &hInstance );
    printf( NO_RES_MESSAGE_PREFIX "%s" NO_RES_MESSAGE_SUFFIX "\n", fname );
    return( false );
}

bool GUIAPI GUILoadStrFini( void )
{
    return( CloseResFile( &hInstance ) );
}

bool GUIAPI GUILoadString( gui_res_id id, char *buffer, int buffer_length )
{
    if( buffer != NULL && buffer_length > 0 ) {
        if( hInstance.status && WResLoadString( &hInstance, id, (lpstr)buffer, buffer_length ) > 0 ) {
            return( true );
        }
        buffer[0] = '\0';
    }
    return( false );
}

bool GUISeekDialogTemplate( res_name_or_id dlg_id )
{
    bool                ok;

    ok = ( hInstance.status != 0 );

    if( ok ) {
        ok = WResSeekResourceX( &hInstance, MAKEINTRESOURCE( RT_DIALOG ), dlg_id );
    }

    return( ok );
}

bool GUISeekMenuTemplate( res_name_or_id menu_id )
{
    bool                ok;

    ok = ( hInstance.status != 0 );

    if( ok ) {
        ok = WResSeekResourceX( &hInstance, MAKEINTRESOURCE( RT_MENU ), menu_id );
    }

    return( ok );
}

bool GUIResReadDialogBoxHeader( DialogBoxHeader *hdr )
{
    return( ResReadDialogBoxHeader( hdr, hInstance.fp ) );
}

bool GUIResReadDialogBoxControl( DialogBoxControl *ctl )
{
    return( ResReadDialogBoxControl( ctl, hInstance.fp ) );
}

bool GUIResReadMenuHeader( MenuHeader *hdr )
{
    return( ResReadMenuHeader( hdr, hInstance.fp ) );
}

bool GUIResReadMenuItem( MenuItem *new )
{
    return( ResReadMenuItem( new, hInstance.fp ) );
}
