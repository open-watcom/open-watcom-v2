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

#define print( a,b ) printf( "#define %s \"%s\"\n", #a, b );

#ifdef JAPANESE_MESSAGES
#define trappick( a,b,c ) print( a,c )
#else
#define trappick( a,b,c ) print( a,b )
#endif

int main( int argc, char *argv[] )
{
    if( argc <= 1 || freopen( argv[1], "w", stdout ) != stdout ) {
        fprintf( stderr, "Can't open output file\n" );
    }
    printf( "#ifndef TRPERR_H_INCLUDED\n" );
    printf( "#define TRPERR_H_INCLUDED\n" );
    #include "trap.msg"
    printf( "#endif\n" );
    return( 0 );
}
