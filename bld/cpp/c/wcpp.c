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
#include "iopath.h"
#include "swchar.h"


char *OptEnvVar = "";

static const char *usageMsg[] = {
    "Usage: wcpp [-c] [-d<macro>] [-i<path>] [-l] [-zk0] [-zk1] [-zk2] [-zku8] [input files]\n",
    "input files\t\tlist of input source file names\n",
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

/* forward declaration */
static bool scanEnvVar( const char *varname );

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

static int          flags = 0;
static const char   **defines = NULL;
static int          numdefs = 0;
static const char   **filenames = NULL;
static int          nofilenames = 0;

static bool ScanOptionsArg( const char * arg )
/********************************************/
{
    bool        contok;

    contok = true;

    switch( tolower( *arg ) ) {
    case 'c':
        flags |= PPFLAG_KEEP_COMMENTS;
        break;
    case 'd':
        ++arg;
        defines = realloc( (void *)defines, ( numdefs + 1 ) * sizeof( char * ) );
        defines[numdefs++] = arg;
        break;
    case 'h':
        Quit( usageMsg, NULL );
        break;
    case 'i':
        ++arg;
        PP_AddIncludePath( arg );
        break;
    case 'l':
        flags |= PPFLAG_EMIT_LINE;
        break;
    case 'z':
        ++arg;
        if( tolower( arg[0] ) == 'k' ) {
            if( arg[1] == '0' && arg[2] == '\0' ) {
                flags |= PPFLAG_DB_KANJI;
                break;
            } else if( arg[1] == '1' && arg[2] == '\0' ) {
                flags |= PPFLAG_DB_CHINESE;
                break;
            } else if( arg[1] == '2' && arg[2] == '\0' ) {
                flags |= PPFLAG_DB_KOREAN;
                break;
            } else if( tolower( arg[1] ) == 'u' ) {
                if( arg[2] == '8' && arg[3] == '\0' ) {
                    flags |= PPFLAG_UTF8;
                    break;
                }
            }
        }
        Quit( usageMsg, "Incorrect option\n" );
        break;
    default:            /* option that could have others with it */
//        contok = ScanMultiOptArg( arg ) && contok;
        break;
    }
    return( contok );
} /* ScanOptionsArg */

static bool doScanParams( int argc, char *argv[] )
/************************************************/
{
    const char *arg;
    int     switchchar;
    bool    contok;         /* continue with main execution */
    int     currarg;

    contok = true;
    switchchar = _dos_switch_char();
    for( currarg = 0; currarg < argc && contok; currarg++ ) {
        arg = argv[currarg];
        if( *arg == switchchar || *arg == '-' ) {
            contok = ScanOptionsArg( arg + 1 ) && contok;
        } else if( *arg == '@' ) {
            contok = scanEnvVar( arg + 1 ) && contok;
        } else if( *arg == '?' ) {
            Quit( usageMsg, NULL );
//            contok = false;
        } else {
            filenames = realloc( (void *)filenames, ( nofilenames + 1 ) * sizeof( char * ) );
            filenames[nofilenames++] = arg;
        }
    }
    return( contok );
}

static int ParseEnvVar( const char *env, char **argv, char *buf )
/***************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *env.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         switchchar;
    int         argc;
    char        *bufend;
    bool        got_quote;
    bool        output_data;

    switchchar = _dos_switch_char();
    output_data = ( buf != NULL ) && ( argv != NULL );
    bufend = buf;
    argc = 0;
    for( ;; ) {
        got_quote = false;
        while( isspace( *env ) && *env != '\0' )
            env++;
        start = env;
        if( output_data ) {
            argv[argc] = bufend;
        }
        if( *env == switchchar || *env == '-' ) {
            if( output_data ) {
                *bufend = *env;
                bufend++;
            }
            env ++;
        }
        while( ( got_quote || !isspace( *env ) ) && *env != '\0' ) {
            if( *env == '\"' ) {
                got_quote = !got_quote;
            }
            if( output_data ) {
                *bufend = *env;
                bufend++;
            }
            env++;
        }
        if( start != env ) {
            argc++;
            if( output_data ) {
                *bufend = '\0';
                bufend++;
            }
        }
        if( *env == '\0' ) {
            break;
        }
    }
    return( argc );
}

static bool scanEnvVar( const char *varname )
/*******************************************/
{
    /*
     * Pass nofilenames and analysis of getenv(varname) into argc and argv
     * to doScanParams. Return view on usability of data. (true is usable.)
     *
     * Recursion is supported but circularity is rejected.
     *
     * The analysis is fairly similar to that done in clib(initargv@_getargv).
     * It is possible to use that function but it is not generally exported and
     * ParseEnvVar() above is called from other places.
     */
    typedef struct EnvVarInfo {
        struct EnvVarInfo       *next;
        char                    *varname;
        char                    **argv; /* points into buf */
        char                    buf[1]; /* dynamic array */
    } EnvVarInfo;

    int                 argc;
    EnvVarInfo          *info;
    static EnvVarInfo   *stack = 0; // Needed to detect recursion.
    size_t              argvsize;
    size_t              argbufsize;
    char                *env;
    size_t              varlen;     // size to hold varname copy.
    bool                result;     // doScanParams Result.

    env = PP_GetEnv( varname );
    if( env == NULL ) {
//        RcWarning( ERR_ENV_VAR_NOT_FOUND, varname );
        return( true );
    }
    // This used to cause stack overflow: set foo=@foo && wrc @foo.
    for( info = stack; info != NULL; info = info->next ) {
#if !defined( __UNIX__ )
        if( stricmp( varname, info->varname ) == 0 ) {  // Case-insensitive
#else
        if( strcmp( varname, info->varname ) == 0 ) {   // Case-sensitive
#endif
//            RcFatalError( ERR_RCVARIABLE_RECURSIVE, varname );
        }
    }
    argc = ParseEnvVar( env, NULL, NULL );  // count parameters.
    argbufsize = strlen( env ) + 1 + argc;  // inter-parameter spaces map to 0
    argvsize = argc * sizeof( char * );     // sizeof argv[argc+1]
    varlen = strlen( varname ) + 1;         // Copy taken to detect recursion.
    info = malloc( sizeof( *info ) + argbufsize + argvsize + varlen );
    info->next = stack;
    stack = info;                           // push info on stack
    info->argv = (char **)info->buf;
    ParseEnvVar( env, info->argv, info->buf + argvsize );
    info->varname = info->buf + argvsize + argbufsize;
    strcpy( info->varname, varname );
    result = doScanParams( argc, info->argv );

    stack = info->next;                     // pop stack
    free( info );
    return( result );
}


int main( int argc, char *argv[] )
{
    int         ch;
    int         i;
    int         j;
    int         rc;

    if( argc < 2 ) {
        Quit( usageMsg, "No filename specified\n" );
    } else if( argc == 2 ) {
        if( !strcmp( argv[1], "?" ) ) {
            Quit( usageMsg, NULL );
        }
    }

    PP_IncludePathInit();

    rc = EXIT_FAILURE;
    if( doScanParams( argc - 1, argv + 1 ) && nofilenames != 0 ) {
        rc = EXIT_SUCCESS;
        for( i = 0; i < nofilenames; ++i ) {
            if( PP_Init( filenames[i], flags, NULL ) != 0 ) {
                fprintf( stderr, "Unable to open '%s'\n", filenames[i] );
                rc = EXIT_FAILURE;
                break;
            }
            for( j = 0; j < numdefs; j++ ) {
                PP_Define( (void *)defines[j] );
            }
            for( ;; ) {
                ch = PP_Char();
                if( ch == EOF )
                    break;
                putchar( ch );
            }
            PP_Fini();
        }
    }

    free( (void *)filenames );
    free( (void *)defines );

    PP_IncludePathFini();

    if( rc == EXIT_FAILURE && nofilenames == 0 ) {
        Quit( usageMsg, "No filename specified\n" );
    }

    return( rc );
}
