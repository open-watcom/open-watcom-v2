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
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>

#define BUF_SIZE                ( 8 * 1024 )
static char Buffer[ BUF_SIZE ];

int main( int argc, char *argv[] ) {

    int         inhdl;
    int         outhdl;
    int         read_len;
    int         write_len;
    int         ret;

    ret = 0;
    if( argc != 3 ) {
        printf( "Invalid command line\n" );
        return( 1 );
    }

    inhdl = open( argv[1], O_RDONLY | O_BINARY );
    if( inhdl == -1 ) {
        printf( "Could not open '%s'\n", argv[1] );
        return( 1 );
    }

    outhdl = open( argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                   S_IRWXU | S_IRWXG | S_IRWXO );
    if( outhdl == -1 ) {
        printf( "Could not open '%s'\n", argv[2] );
        close( inhdl );
        return( 1 );
    }

    for( ;; ) {
        read_len = read( inhdl, Buffer, BUF_SIZE );
        if( read_len == -1 ) {
            printf( "Error reading '%s'\n", argv[1] );
            ret = 1;
            break;
        }
        write_len = write( outhdl, Buffer, read_len );
        if( write_len != read_len ) {
            printf( "Error writting '%s'\n", argv[2] );
            ret = 1;
            break;
        }
        if( read_len < BUF_SIZE ) break;
    }
    close( outhdl );
    close( inhdl );
    if( ret ) {
        remove( argv[2] );
    }
    return( ret );
}
