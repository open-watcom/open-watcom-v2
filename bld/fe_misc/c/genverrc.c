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
#include <stdlib.h>
#include <time.h>

#ifndef _VERSION
 #include "banner.h"
 #define _VERSION (_BANVER/10)
#endif

#if 0   // example has embedded ISO C comments

    Example: genverrc skeleton.rc output.rc $ file.dll

=== skeleton.rc ===
#include <windows.h>

VS_VERSION_INFO VERSIONINFO
    FILEVERSION 1,0,$h,$l
    PRODUCTVERSION 1,0,0,1
    FILEFLAGSMASK VS_FFI_FILEFLAGSMASK
    FILEFLAGS 0x0L
    FILEOS VOS_NT_WINDOWS32
    FILETYPE VFT_DLL
    FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904b0"        /* US English, Unicode */
        BEGIN
            VALUE "CompanyName", "Watcom International Corp.\0"
            VALUE "FileDescription", "Watcom C++ Compiler (DLL)\0"
            VALUE "FileVersion", "1.0\0"
            VALUE "InternalName", "C++ Compiler\0"
            VALUE "LegalCopyright", "Copyright \251 Watcom International Corp. 1995\0"
            VALUE "OriginalFilename", "$0\0"
            VALUE "ProductName", "Watcom C++\0"
            VALUE "ProductVersion", "1.0\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1200        /* US English, Unicode */
    END
END

#endif

unsigned line;

void fatal( char *msg ) {
    fprintf( stderr, "%s\n", msg );
    exit( EXIT_FAILURE );
}

#define ARGC_ESC_BASE   4

int main( int argc, char **argv ) {
    FILE *ifp;
    FILE *ofp;
    int c;
    int e;
    int a;
    time_t gen_time;

    if( argc < ARGC_ESC_BASE || argc > ( ARGC_ESC_BASE + 9 ) ) {
        fatal( "usage: genverrc <skeleton-rc> <output-rc> <esc-char> [<arg0>] [<arg1>] ... [<arg9>]" );
    }
    ifp = fopen( argv[1], "r" );
    if( ifp == NULL  ) {
        fatal( "cannot open input .rc file" );
    }
    ofp = fopen( argv[2], "w" );
    if( ofp == NULL  ) {
        fatal( "cannot open output .rc file" );
    }
    gen_time = time( NULL );
    line = 1;
    e = argv[3][0];
    for(;;) {
        c = fgetc( ifp );
        if( c == EOF ) break;
        if( c == '\n' ) {
            ++line;
        }
        if( c == e ) {
            c = fgetc( ifp );
            switch( c ) {
            case 'h':
                fprintf( ofp, "0x%x", ( gen_time >> 16 ) & 0x0ffff );
                break;
            case 'l':
                fprintf( ofp, "0x%x", ( gen_time >> 0 ) & 0x0ffff );
                break;
            case 'j':
                fprintf( ofp, "%d", (_VERSION / 10) );
                break;
            case 'n':
                fprintf( ofp, "%d", (_VERSION % 10) );
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                a = ( c - '0' ) + ARGC_ESC_BASE;
                if( a > ( argc - 1 ) ) {
                    fatal( "escape arg number exceeds # of arguments provided" );
                }
                fprintf( ofp, "%s", argv[a] );
                break;
            default:
                fatal( "invalid escape sequence" );
            }
        } else {
            fputc( c, ofp );
        }
    }
    fclose( ifp );
    fclose( ofp );
    return( EXIT_SUCCESS );
}
