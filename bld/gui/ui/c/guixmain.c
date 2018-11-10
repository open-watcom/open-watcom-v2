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
* Description:  Mainline for applications using GUI library.
*
****************************************************************************/


#include "guiwind.h"
#include "guixmain.h"
#include <stdlib.h>
#ifdef __UNIX__
    #include <termios.h>
#elif defined( __WINDOWS__ )
    #include <windows.h>
#endif

#if defined(__WATCOMC__)
    #include "clibint.h"
#else
    #include "clibext.h"
#endif


// This makes sure that the main routine is actually linked in
bool GUIMainTouched = false;

#ifdef __WINDOWS__

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd )
{
    hInstance = hInstance;
    hPrevInstance = hPrevInstance;
    lpCmdLine = lpCmdLine;
    nShowCmd = nShowCmd;

    return( GUIXMain( _argc, _argv ) );
}

#else

#ifdef __UNIX__
    bool                In_raw_mode = false;
    struct termios      Saved_terminal_configuration;
#endif

int main( int argc, char *argv[] )
{
#if !defined(__WATCOMC__)
    _argv = argv;
    _argc = argc;
#endif
    return( GUIXMain( argc, argv ) );
}

#endif
