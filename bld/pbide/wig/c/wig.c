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
#include <ctype.h>
#include "global.h"
#include "lexxer.h"
#include "srusuprt.h"
#include "options.h"
#include "error.h"
#include "mem.h"
#include "filelist.h"
#include "error.h"

extern FILE *yyin;
extern int wig_parse( void );

static void finiProgram( void )
/*****************************/
{
    FreeSru();
    FiniError();
    FiniLex();
    FiniFileList();
    FiniMem();
}


int Wigmain( int argc, char **argv )
/**********************************/
{
    int         rc;

    InitMem();
    rc = setjmp( ErrorEnv );
    if( rc == RC_SPAWN_SET ) {
        // Program main line
        InitFileList();
        if( !ProcessOptions( argc, argv ) ) {

            /* initialize various components */
            InitError();
            InitLex( GetInputFile() );
            InitSru();

            /* parse loop */
            while( !wig_parse() );

            /* do any post processing */
            DoPostProcessing();

            /* generate new sru file */
            if( Options & OPT_MODIFY_SRU ) FiniSru();

            /* generare new cpp/c/hpp files */
            GenCPPInterface();
        }
    }
    finiProgram();
    return( rc == RC_ERROR );
}

#if defined( __DLL__ )
#elif defined( __WINDOWS__ )
int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    char        *ptr;
    char        *line;
    unsigned    argc;
    unsigned    i;
    char        **argv;
    int         rc;

    currinst = currinst;
    previnst = previnst;
    cmdshow = cmdshow;
    argc = 1;
    ptr = cmdline;
    for( ;; ) {
        while( isspace( *ptr ) ) ptr++;
        if( *ptr == '\0' ) break;
        while( !isspace( *ptr ) && *ptr != '\0' ) ptr++;
        argc++;
        if( *ptr == '\0' ) break;
    }
    argv = MemMalloc( argc * sizeof( char * ) );
    line = MemStrDup( cmdline );
    ptr = line;
    argv[0] = "";
    for( i=1; i < argc; i++ ) {
        while( isspace( *ptr ) ) ptr++;
        argv[i] = ptr;
        while( !isspace( *ptr ) && *ptr != '\0' ) ptr++;
        *ptr = '\0';
        ptr++;
    }
    rc = Wigmain( argc, argv );
    MemFree( line );
    MemFree( argv );
    return( rc );
}
#else
int main( int argc, char **argv )
/*******************************/
{
    printf( "%s", WIG_BANNER );
    return( Wigmain( argc, argv ) );
}
#endif
