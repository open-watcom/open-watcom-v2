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


#include "windows.h"
#include <stdlib.h>
#include <string.h>
#include "mbstring.h"
#include "wwinhelp.h"

BOOL WWinHelp( HWND hwnd, LPCSTR helpFile, UINT fuCommand, DWORD data )
{
    char        buff[_MAX_PATH];
    static char open=FALSE;

    if( fuCommand == HELP_QUIT && !open ) return( FALSE );
    open = TRUE;

    if( helpFile != NULL ) {
        if( __IsDBCS ) {
            /* Look for Japanese version of help file first */

            char        drive[_MAX_DRIVE];
            char        dir[_MAX_DIR];
            char        fname[_MAX_FNAME];
            char        ext[_MAX_EXT];
            char    new_filename[_MAX_PATH];

            _splitpath( helpFile, drive, dir, fname, ext );
            if( strlen( fname ) < 8 ) {
                strcat( fname, "j" );
            } else {
                fname[7] = 'j';
            }
            _makepath( new_filename, drive, dir, fname, ext );

            if( new_filename != NULL ) {
                _searchenv( new_filename, "WWINHELP", buff );
                if( buff[0] != '\0' ) {
                    helpFile = buff;
                    return( WinHelp( hwnd, helpFile, fuCommand, data ) );
                }
                _searchenv( new_filename, "PATH", buff );
                if( buff[0] != '\0' ) {
                    helpFile = buff;
                    return( WinHelp( hwnd, helpFile, fuCommand, data ) );
                }
            }
        }

        /* Can't find the Japanese version, just look for the english one */

        _searchenv( helpFile, "WWINHELP", buff );
        if( buff[0] != '\0' ) helpFile = buff;
    }
    return( WinHelp( hwnd, helpFile, fuCommand, data ) );
}
