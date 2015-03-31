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


// a lot of the contents of this file were liberally lifted from
// wlmsg.c in the WLIB project -- Wes

#include "guiwind.h"
#include "guiextnm.h"
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include "wio.h"
#include "wresdefn.h"
#include "wressetr.h"
#include "wresset2.h"

#include "clibext.h"

#if defined( __QNX__ ) || defined( __UNIX__ )
    #define _newline "\n"
#else
    #define _newline "\r\n"
#endif

#define NO_RES_MESSAGE_PREFIX "Error: could not open message resource file ("
#define NO_RES_MESSAGE_SUFFIX ")." _newline

static  HANDLE_INFO     hInstance = { 0 };
static  bool            GUIMsgInitFlag = false;

static WResFileOffset res_seek( WResFileID handle, WResFileOffset position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, GUIMemAlloc, GUIMemFree );

bool GUIIsLoadStrInitialized( void )
{
    return( GUIMsgInitFlag );
}

bool GUILoadStrInit( const char *fname )
{
    hInstance.handle = NIL_HANDLE;
    if( !OpenResFile( &hInstance, fname ) ) {
        // if we are using an external resource file then we don't have to search
        FileShift = 0;
        if( GUIGetExtName() != NULL || !FindResources( &hInstance ) ) {
            if( !InitResources( &hInstance ) ) {
                GUIMsgInitFlag = true;
                return( true );
            }
        }
        CloseResFile( &hInstance );
    }
    write( fileno(stdout), NO_RES_MESSAGE_PREFIX, sizeof( NO_RES_MESSAGE_PREFIX ) - 1 );
    write( fileno(stdout), fname,                 strlen( fname ) );
    write( fileno(stdout), NO_RES_MESSAGE_SUFFIX, sizeof( NO_RES_MESSAGE_SUFFIX ) - 1 );
    GUIMsgInitFlag = false;
    return( false );
}

bool GUILoadStrFini( void )
{
    if( GUIMsgInitFlag ) {
        if( !CloseResFile( &hInstance ) ) {
            GUIMsgInitFlag = false;
        } else {
            return( false );
        }
    }

    return( true );
}

bool GUILoadString( gui_res_id id, char *buffer, int buffer_length )
{
    if( GUIMsgInitFlag && buffer != NULL && buffer_length != 0 ) {
        if( LoadString( &hInstance, id, (LPSTR)buffer, buffer_length ) == 0 ) {
            return( true );
        } else {
            buffer[0] = '\0';
        }
    }

    return( false );
}

bool GUILoadDialogTemplate( res_name_or_id dlg_id, char **template, int *length )
{
    bool                ok;

    ok = ( GUIMsgInitFlag && template != NULL && length != NULL );

    if( ok ) {
        ok = ( WResLoadResourceX( &hInstance, GUI_MAKEINTRESOURCE( RT_DIALOG ), dlg_id,
                                 (LPSTR *)template, length ) == 0 );
    }

    return( ok );
}

bool GUILoadMenuTemplate( res_name_or_id menu_id, char **template, int *length )
{
    bool                ok;

    ok = ( GUIMsgInitFlag && template != NULL && length != NULL );

    if( ok ) {
        ok = ( WResLoadResourceX( &hInstance, GUI_MAKEINTRESOURCE( RT_MENU ), menu_id,
                                 (LPSTR *)template, length ) == 0 );
    }

    return( ok );
}
