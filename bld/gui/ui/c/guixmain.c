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


#include "guiwind.h"
#include "guix.h"
#include "guiscale.h"
#include "guixloop.h"
#include "guixutil.h"
#include "guicolor.h"
#include "guixmain.h"
#include "guisysfi.h"
#include "guimenu.h"
#include "guiwhole.h"
#include "guiwnclr.h"
#include "guihook.h"
#include "guiutil.h"
#include "guigadgt.h"
#include "guizlist.h"
#include "guideath.h"
#include "guidead.h"
#ifdef UNIX
    #include "clibext.h"
    #include "stdtypes.h"
    #ifdef HP
        #include <termios.h>
    #else
        #include <termio.h>
    #endif
#endif

extern int GUIXMain( int argc, char * argv[] );

// This makes sure that the main routine is actually linked in
bool GUIMainTouched = FALSE;

#ifdef __WINDOWS__
#include "swap.h"
#include <windows.h>
#include "win1632.h"
#include <dos.h>

int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd )
{
    extern char **_argv;
    extern int  _argc;

    hInstance = hInstance;
    hPrevInstance = hPrevInstance;
    lpCmdLine = lpCmdLine;
    nShowCmd = nShowCmd;

    return( GUIXMain( _argc, _argv ) );
}
#else
#ifdef UNIX
    char **             _argv;
    int                 _argc;

    a_bool              In_raw_mode = FALSE;
    struct termios      Saved_terminal_configuration;
#endif

int main( int argc, char *argv[] )
{
#ifdef UNIX
    _argv = argv;
    _argc = argc;
#endif
    return( GUIXMain( argc, argv ) );
}
#endif

