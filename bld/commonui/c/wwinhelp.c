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
* Description:  Look for Japanese help files, fall back to English ones.
*
****************************************************************************/


#include "commonui.h"
#include <stdlib.h>
#include <string.h>
#include "mbstring.h"
#include "bool.h"
#include "wwinhelp.h"
#ifdef __NT__
    #include <htmlhelp.h>
#endif


#ifdef __NT__
typedef HWND (WINAPI *PFNHH)( HWND, LPCSTR, UINT, DWORD_PTR );

static PFNHH    pfnHtmlHelp = NULL;
#endif

/*
 * WWinHelp - open an WinHelp file
 */
bool WWinHelp( HWND hwnd, LPCSTR helpFile, UINT fuCommand, HELP_DATA data )
{
    char        buff[_MAX_PATH];
    static bool open = false;

    if( fuCommand == HELP_QUIT && !open ) {
        return( false );
    }
    open = true;

    if( helpFile != NULL ) {
#if !defined( _WIN64 )
        if( __IsDBCS ) {
            /* Look for Japanese version of help file first */
            char        drive[_MAX_DRIVE];
            char        dir[_MAX_DIR];
            char        fname[_MAX_FNAME];
            char        ext[_MAX_EXT];
            char        new_filename[_MAX_PATH];

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
                    return( WinHelp( hwnd, helpFile, fuCommand, data ) != 0 );
                }
                _searchenv( new_filename, "PATH", buff );
                if( buff[0] != '\0' ) {
                    helpFile = buff;
                    return( WinHelp( hwnd, helpFile, fuCommand, data ) != 0 );
                }
            }
        }
#endif

        /* Can't find the Japanese version, just look for the english one */
        _searchenv( helpFile, "WWINHELP", buff );
        if( buff[0] != '\0' ) {
            helpFile = buff;
        }
    }
    return( WinHelp( hwnd, helpFile, fuCommand, data ) != 0 );

} /* WWinHelp */

/*
 * WHtmlHelp - open an HTML Help file
 */
bool WHtmlHelp( HWND hwnd, LPCSTR helpFile, UINT fuCommand, HELP_DATA data )
{
#ifdef __NT__
    char    buff[_MAX_PATH];
    if( pfnHtmlHelp == NULL ) {
        HINSTANCE hInstance = LoadLibrary( "HHCTRL.OCX" );
        if( hInstance == NULL ) {
            return( false );
        }
        pfnHtmlHelp = (PFNHH)GetProcAddress( hInstance, "HtmlHelpA" );
        if( pfnHtmlHelp == NULL ) {
            return( false );
        }
    }
    switch( fuCommand ) {
    case HELP_CONTENTS:
        fuCommand = HH_DISPLAY_TOC;
        break;
    case HELP_CONTEXT:
        fuCommand = HH_HELP_CONTEXT;
        break;
    case HELP_PARTIALKEY:
    case HELP_KEY:
        fuCommand = HH_DISPLAY_INDEX;
        break;
    default:
        return( false );
    }
    _searchenv( helpFile, "WHTMLHELP", buff );
    if( buff[0] != '\0' ) {
        helpFile = buff;
    }
    return( pfnHtmlHelp( hwnd, helpFile, fuCommand, data ) != NULL );
#else
    hwnd = hwnd;
    helpFile = helpFile;
    fuCommand = fuCommand;
    data = data;
    return( false );
#endif

} /* WHtmlHelp */
