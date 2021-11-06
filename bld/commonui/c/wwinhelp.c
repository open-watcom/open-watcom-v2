/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <string.h>
#ifdef __NT__
    #include <htmlhelp.h>
#endif
#include "mbstring.h"
#include "bool.h"
#include "wwinhelp.h"
#include "pathgrp2.h"

#include "clibext.h"


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
            pgroup2     pg;
            char        new_filename[_MAX_PATH];
            size_t      len;

            _splitpath2( helpFile, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
            len = strlen( pg.fname );
            if( len > 0 ) {
                if( len > 7 )
                    len = 7;
                pg.fname[len++] = 'j';
                pg.fname[len] = '\0';
                _makepath( new_filename, pg.drive, pg.dir, pg.fname, pg.ext );

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

    /* unused parameters */ (void)hwnd; (void)helpFile; (void)fuCommand; (void)data;

    return( false );
#endif

} /* WHtmlHelp */
