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
* Description:  Utility to strip comments from packing manifests and
*               optionally prepend specified path to each output line.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char    buf[2048];

char *skipwhite( char *s )
{
    while( *s && isspace( *s ) )
        s++;
    return *s ? s : NULL;
}

int main( int argc, char *argv[] )
{
    char        *p;
    FILE        *in;

    /* Display usage information */
    if( argc != 2 && argc != 3 ) {
        fprintf( stderr, "Usage: comstrip <file> [path]\n" );
        return -1;
    }

    /* Read all lines from input file and copy non-comment lines
     * to output file.
     */
    if( ( in = fopen( argv[1], "r" ) ) == NULL ) {
        printf( "Unable to open input file %s!\n", argv[1] );
        return -1;
    }
    while( fgets( buf, sizeof( buf ), in ) ) {
        /* Skip comment and blank lines */
        p = skipwhite( buf );
        if( p == NULL || *p == '#' )
            continue;
        if( argc == 3 )
            fputs( argv[2], stdout );
        fputs( buf, stdout );
    }
    fclose( in );
    return 0;
}

