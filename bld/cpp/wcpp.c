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
* Description:  Preprocessor utility (trivial 'cpp').
*
****************************************************************************/


#include "preproc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "getopt.h"
#include "misc.h"


char *OptEnvVar = "";

static const char *usageMsg[] = {
    "Usage: wcpp [-c] [-d<macro>] [-i<path>] [-l] [-zk0] [-zk1] [-zk2] [-zku8] [input file]\n",
    "input file\t\tname of input source file\n",
    "-c\t\tpreserve comments\n",
    "-d<macro>\t\tdefine macro\n",
    "-i<path>\t\tinclude path\n",
    "-l\t\tgenerate #line directives\n",
    "-zk{0,1,2,u8}\t\tsource file character encoding\n",
    "-zk0\t\tJapanese (Kanji, CP 932) double-byte encoding\n",
    "-zk1\t\tChinese (Traditional, CP 950) double-byte encoding\n",
    "-zk2\t\tKorean (Wansung, CP 949) double-byte encoding\n",
    "-zku8\t\tUnicode UTF-8 encoding\n",
    "-h\t\tdisplay usage\n",
    NULL
};

void Quit( const char *usage_msg[], const char *str, ... )
{
    va_list     al;

    if( str != NULL ) {
        va_start( al, str );
        vfprintf( stderr, str, al );
        va_end( al );
    }
    if( usage_msg == NULL ) {
        exit( EXIT_FAILURE );
    }
    if( str != NULL ) {
        fprintf( stderr, "%s\n", *usage_msg );
        exit( EXIT_FAILURE );
    }
    for( ; *usage_msg != NULL; ++usage_msg ) {
        fprintf( stderr, "%s\n", *usage_msg );
    }
    exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] )
{
    int         flags = 0;
    char        *inc_path = NULL;
    int         len;
    char        **defines = NULL;
    char        **curr_def = NULL;
    int         numdefs = 0;
    int         ch;

#ifdef __UNIX__
    AltOptChar = '-';
#endif
    /*
     * get options
     */
    while( 1 ) {
        ch = GetOpt( &argc, argv, "cCd:D:i:I:lLz:", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( tolower( ch ) ) {
        case 'c':
            flags |= PPFLAG_KEEP_COMMENTS;
            break;
        case 'd':
            numdefs = 2;    // new item + NULL
            if( defines != NULL ) {
                for( curr_def = defines; *curr_def != NULL; curr_def++ ) {
                    ++numdefs;
                }
            }
            defines = realloc( defines, numdefs * sizeof( char * ) );
            defines[numdefs - 2] = OptArg;
            defines[numdefs - 1] = NULL;
            break;
        case 'h':
            Quit( usageMsg, NULL );
            break;
        case 'i':
            len = 2 + strlen( OptArg );
            if( inc_path != NULL ) {
                len += strlen( inc_path );
                inc_path = realloc( inc_path, len );
            } else {
                inc_path = malloc( len );
                *inc_path = '\0';
            }
            strcat( inc_path, ";" );
            strcat( inc_path, OptArg );
            break;
        case 'l':
            flags |= PPFLAG_EMIT_LINE;
            break;
        case 'z':
            if( tolower( OptArg[0] ) == 'k' ) {
                if( OptArg[1] == '0' && OptArg[2] == '\0' ) {
                    flags |= PPFLAG_DB_KANJI;
                    break;
                } else if( OptArg[1] == '1' && OptArg[2] == '\0' ) {
                    flags |= PPFLAG_DB_CHINESE;
                    break;
                } else if( OptArg[1] == '2' && OptArg[2] == '\0' ) {
                    flags |= PPFLAG_DB_KOREAN;
                    break;
                } else if( tolower( OptArg[1] ) == 'u' ) {
                    if( OptArg[2] == '8' && OptArg[3] == '\0' ) {
                        flags |= PPFLAG_UTF8;
                        break;
                    }
                }
            }
            Quit( usageMsg, "Incorrect option\n" );
            break;
        }
    }
    /*
     * get destination directory/file, and validate it
     */
    if( argc < 2 ) {
        Quit( usageMsg, "No filename specified\n" );
    }
    if( argc == 2 ) {
        if( !strcmp( argv[1], "?" ) ) {
            Quit( usageMsg, NULL );
        }
    }
    if( PP_Init( argv[1], flags, inc_path ) != 0 ) {
        fprintf( stderr, "Unable to open '%s'\n", argv[1] );
        return( EXIT_FAILURE );
    }
    if( defines != NULL ) {
        for( curr_def = defines; *curr_def != NULL; curr_def++ ) {
            PP_Define( *curr_def );
        }
    }
    for( ;; ) {
        ch = PP_Char();
        if( ch == EOF )
            break;
        putchar( ch );
    }
    PP_Fini();
    return( EXIT_SUCCESS );
}
