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


/*
 *  INTLTEST.C
 *  Currently, this program only tests the sound() and nosound() functions.
 *  They now decide at run-time whether or not they're on an IBM or a NEC
 *  machine; this program is to ensure they still work.
 *
 */

#include <conio.h>
#include <i86.h>
#include <stdio.h>
#include <string.h>

#if defined(__DOS__) || defined(__QNX__) || defined(__WINDOWS__)
    #define TEST_SOUND
#endif

extern int      __NonIBM;


char ProgramName[128];                          /* executable filename */


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

    for( i=0; notes[i]; i++ ) {
        sound( 1193180 / (notes[i]/(1<<4)) );
        printf( "%s: Now playing %s\n", ProgramName, notenames[i] );
        fgetc( stdin );
        nosound();
    }
}

#endif


void main( int argc, char *argv[] )
/*********************************/
{
    strcpy( ProgramName, strlwr( argv[0] ) ); /* store executable filename */

    printf( "%s: Machine type is %s.\n", ProgramName,
            __NonIBM ? "NEC" : "IBM" );

    #ifdef TEST_SOUND
        TestSound();
    #else
        printf( "%s: Skipping sound() and nosound() tests.\n", ProgramName );
    #endif

    printf( "%s: Done\n", ProgramName );
}
