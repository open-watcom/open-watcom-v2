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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vi.h"

#ifdef __IDE__
#include "common.h"
#include "link.h"
#include "winprag.h"

/*
 * IDEInit - set up for communication with the IDE
 */
void IDEInit( void )
{
    if( !VxDPresent() ) {
        Die( "WDEBUG.386 not present!" );
    }
    while( 1 ) {
        if( VxDLink( EDITOR_LINK_NAME ) == NULL ) {
            VxDConnect();
            break;
        }
        TimeSlice();
    }

} /* IDEInit */

/*
 * IDEFini - stop communication with the IDE
 */
void IDEFini( void )
{
    if( !EditFlags.UseIDE ) {
        return;
    }
    VxDDisconnect();
    VxDUnLink();

} /* IDEFini */

/*
 * IDEGetKeys - try to get keys from the IDE
 */
void IDEGetKeys( void )
{
    char        buff[MAX_STR];
    char        path[_MAX_PATH];

    if( !EditFlags.UseIDE ) {
        return;
    }
    if( !VxDPutPending() ) {
        return;
    }
    VxDGet( buff, sizeof( buff ) );
    KeyAddString( buff );
    GetCurrentFilePath( path );
    MySprintf( buff,"(%ld, %d) %s\r\n", CurrentLineNumber, CurrentColumn, path );
    VxDPut( buff, strlen( buff ) + 1 );
    VxDPut( TERMINATE_COMMAND_STR, sizeof( TERMINATE_COMMAND_STR ) );

} /* IDEGetKeys */

#endif
