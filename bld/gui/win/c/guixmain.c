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


#include <stdlib.h>
#if defined( UNIX )
    #include "unsoasyc.h"
    #include "stdtypes.h"
#endif
#include "guiwind.h"


// This makes sure that the main routine is actially linked in
bool    GUIMainTouched = FALSE;

#if defined( __OS2_PM__ )
extern int          GUIXMain( int argc, char *argv[] );
/*
 * main - main entry point for PM
 */

int main( SHORT sArgc,CHAR  *ppArgv[] )
{
    return( GUIXMain( sArgc, ppArgv ) );
}

#else

#if defined( UNIX )

a_bool In_raw_mode = FALSE;
struct termios Saved_terminal_configuration;

#define _started_from_dblib()   ( 0 )

char **         SaveArgv = NULL;
char **         _argv = NULL;
int             _argc = 0;

#if !defined( TWIN )

int wu_main( int, char *[] );


int main( int argc, char *argv[] )
{
    int         stdin_isatty;

    stdin_isatty = isatty( 0 );

    _argv = argv;
    _argc = argc;

#if 0
    if( !_started_from_dblib() && stdin_isatty ) {
        startup_msg( "Press 'q' to quit\n" );
        unix_setup_async_io( 0 );
        unix_set_raw_mode( 0, &Saved_terminal_configuration );
        In_raw_mode = TRUE;
    }
#endif

    return( wu_main( argc, argv ) );
}
#endif

#endif

extern int          GUIXMain( int argc, char *argv[],
                    WPI_INST inst, WPI_INST hPrevInstance, LPSTR lpCmdLine,
                    int nShowCmd );
/*
 * WinMain -- main windows message loop
 */

int PASCAL WinMain( WPI_INST inst, WPI_INST hPrevInstance, LPSTR lpCmdLine,
                    int nShowCmd )
{
#if !defined( UNIX )
    extern char **_argv;
    extern int  _argc;
#endif

    return( GUIXMain( _argc, _argv, inst, hPrevInstance, lpCmdLine, nShowCmd ) );
}

#endif
