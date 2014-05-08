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
* Description:  Non-exhaustive test of x86 specific functions.
*
****************************************************************************/


/*
 *  Currently, this program only tests the sound() and nosound() functions.
 */

#include <stdlib.h>
#include <conio.h>
#ifdef _M_IX86
#include <i86.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#if defined(__DOS__) || defined(__QNX__) || defined(__WINDOWS__)
    #define TEST_SOUND
#endif


char    ProgramName[128];       /* executable filename */
int     NumErrors;              /* number of errors */
int     Interactive;            /* run tests requiring user input */

#ifdef TEST_SOUND

void TestSound( void )
/********************/
{
    static char *           notenames[] = {
        "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    static unsigned short   notes[] = {
        19327, 18242, 17218, 16252, 15340, 14479, 13666, 12899,
        12175, 11492, 10847, 10238,  9664,     0
    };
    int                     i;

    for( i = 0; notes[i]; i++ ) {
        sound( 1193180 / (notes[i]/(1<<4)) );
        if( Interactive ) {
            printf( "%s: Now playing %s\n", ProgramName, notenames[i] );
            fgetc( stdin );
        } else {
            delay( 100 );
        }
        nosound();
    }
}

#endif


int main( int argc, char *argv[] )
/********************************/
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( EXIT_FAILURE );
    }
#endif
    strcpy( ProgramName, strlwr( argv[0] ) ); /* store executable filename */

    if( argc == 2 && !strcmp( argv[1], "-i" ) )
        Interactive = 1;

#ifdef TEST_SOUND
    TestSound();
#else
    printf( "%s: Skipping sound() and nosound() tests.\n", ProgramName );
#endif

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    printf( "Tests completed (%s).\n", ProgramName );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif

    return( EXIT_SUCCESS );
}
