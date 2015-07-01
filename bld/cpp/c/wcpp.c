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
#include "misc.h"
#include "iopath.h"
#include "swchar.h"


static const char *usageMsg[] = {
    "Usage: wcpp [-c] [-d<macro>] [-i<path>] [-l] [-o<file>] [-zk0] [-zk1] [-zk2]\n"
    "\t\t[-zku8] [input files]\n",
    "input files\t\tlist of input source file names\n",
    "-c\t\tpreserve comments\n",
    "-d<macro>\t\tdefine macro\n",
    "-i<path>\t\tinclude path\n",
    "-l\t\tgenerate #line directives\n",
    "-o<file>\t\toutput file\n",
    "-zk{0,1,2,u8}\t\tsource file character encoding\n",
    "-zk0\t\tJapanese (Kanji, CP 932) double-byte encoding\n",
    "-zk1\t\tChinese (Traditional, CP 950) double-byte encoding\n",
    "-zk2\t\tKorean (Wansung, CP 949) double-byte encoding\n",
    "-zku8\t\tUnicode UTF-8 encoding\n",
    "-h\t\tdisplay usage\n",
    NULL
};

/* forward declaration */
static bool doScanParams( int argc, char *argv[] );

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

static char *my_strdup( const char *str )
{
    size_t     len;
    char       *ptr;

    len = strlen( str ) + 1;
    ptr = malloc( len );
    return( memcpy( ptr, str, len ) );
}

static int      flags = 0;
static char     **defines = NULL;
static int      numdefs = 0;
static char     **filenames = NULL;
static int      nofilenames = 0;
static char     *out_filename = NULL;

static bool scanString( char *buf, const char *str, unsigned len )
/*****************************************************************/
{
    bool        have_quote;
    char        c;

    have_quote = false;
    while( isspace( *str ) )
        ++str;
    while( (c = *str++) != '\0' && len > 0 ) {
        if( c == '\"' ) {
            have_quote = !have_quote;
        } else {
            *buf++ = c;
            len--;
        }
    }
    *buf = '\0';
    return( have_quote );
}

static bool ScanOptionsArg( const char * arg )
/********************************************/
{
    bool        contok;
    size_t      len;

    contok = true;

    switch( tolower( *arg ) ) {
    case 'c':
        flags |= PPFLAG_KEEP_COMMENTS;
        break;
    case 'd':
        ++arg;
        defines = realloc( (void *)defines, ( numdefs + 1 ) * sizeof( char * ) );
        defines[numdefs++] = my_strdup( arg );
        break;
    case 'h':
        Quit( usageMsg, NULL );
        break;
    case 'i':
        {
            char    *p;

            ++arg;
            len = strlen( arg );
            p = malloc( len + 1 );
            scanString( p, arg, len );
            PP_AddIncludePath( p );
            free( p );
        }
        break;
    case 'l':
        flags |= PPFLAG_EMIT_LINE;
        break;
    case 'o':
        ++arg;
        if( out_filename != NULL ) {
            free( out_filename );
        }
        len = strlen( arg );
        out_filename = malloc( len + 1 );
        scanString( out_filename, arg, len );
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
        // fall down
    default:
        Quit( usageMsg, "Incorrect option\n" );
        break;
    }
    return( contok );
} /* ScanOptionsArg */

static int ParseVariable( const char *env, char **argv, char *buf )
/*****************************************************************/
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
    char        *bufstart;
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
            bufstart = bufend;
        }
        if( *env == switchchar || *env == '-' ) {
            if( output_data ) {
                *bufend++ = *env;
            }
            env ++;
        }
        while( ( got_quote || !isspace( *env ) ) && *env != '\0' ) {
            if( *env == '\"' ) {
                got_quote = !got_quote;
            }
            if( output_data ) {
                *bufend++ = *env;
            }
            env++;
        }
        if( start != env ) {
            if( output_data ) {
                *bufend++ = '\0';
                argv[argc] = bufstart;
            }
            argc++;
        }
        if( *env == '\0' ) {
            break;
        }
    }
    return( argc );
}

static bool scanEnvVarOrFile( const char *name )
/**********************************************/
{
    /*
     * Pass nofilenames and analysis of getenv(name) into argc and argv
     * to doScanParams. Return view on usability of data. (true is usable.)
     *
     * Recursion is supported but circularity is rejected.
     */
    typedef struct EnvVarInfo {
        struct EnvVarInfo       *next;
        char                    *name;
        char                    **argv; /* points into buf */
        char                    buf[1]; /* dynamic array */
    } EnvVarInfo;

    int                 argc;
    EnvVarInfo          *info;
    static EnvVarInfo   *stack = NULL;  // Needed to detect recursion.
    size_t              argvsize;
    size_t              argbufsize;
    const char          *optstring;
    size_t              varlen;         // size to hold name copy.
    bool                result;         // doScanParams Result.
    char                fbuf[512];

    optstring = PP_GetEnv( name );
    if( optstring == NULL ) {
        FILE *fh;

        fh = fopen( name, "rt" );
        if( fh == NULL ) {
//            RcWarning( ERR_ENV_VAR_NOT_FOUND, name );
            return( true );
        }
        fgets( fbuf, sizeof( fbuf ), fh );
        fclose( fh );
        optstring = fbuf;
    }
    // This used to cause stack overflow: set foo=@foo && wrc @foo.
    for( info = stack; info != NULL; info = info->next ) {
#if defined( __UNIX__ )
        if( strcmp( name, info->name ) == 0 ) {     // Case-sensitive
#else
        if( stricmp( name, info->name ) == 0 ) {    // Case-insensitive
#endif
//            RcFatalError( ERR_RCVARIABLE_RECURSIVE, name );
        }
    }
    argc = ParseVariable( optstring, NULL, NULL );  // count parameters.
    argbufsize = strlen( optstring ) + 1 + argc;    // inter-parameter spaces map to 0
    argvsize = argc * sizeof( char * );             // sizeof argv[argc+1]
    varlen = strlen( name ) + 1;                    // Copy taken to detect recursion.
    info = malloc( sizeof( *info ) + argbufsize + argvsize + varlen );
    info->next = stack;
    stack = info;                                   // push info on stack
    info->argv = (char **)info->buf;
    ParseVariable( optstring, info->argv, info->buf + argvsize );
    info->name = info->buf + argvsize + argbufsize;
    strcpy( info->name, name );
    result = doScanParams( argc, info->argv );

    stack = info->next;                             // pop stack
    free( info );
    return( result );
}

static bool doScanParams( int argc, char *argv[] )
/************************************************/
{
    const char  *arg;
    int         switchchar;
    bool        contok;         /* continue with main execution */
    int         currarg;

    contok = true;
    switchchar = _dos_switch_char();
    for( currarg = 0; currarg < argc && contok; currarg++ ) {
        arg = argv[currarg];
        if( *arg == switchchar || *arg == '-' ) {
            contok = ScanOptionsArg( arg + 1 ) && contok;
        } else if( *arg == '@' ) {
            contok = scanEnvVarOrFile( arg + 1 ) && contok;
        } else if( *arg == '?' ) {
            Quit( usageMsg, NULL );
//            contok = false;
        } else {
            filenames = realloc( (void *)filenames, ( nofilenames + 1 ) * sizeof( char * ) );
            filenames[nofilenames++] = my_strdup( arg );
        }
    }
    return( contok );
}

int main( int argc, char *argv[] )
{
    int         ch;
    int         i;
    int         j;
    int         rc;
    FILE        *fo;

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
        fo = stdout;
        if( out_filename != NULL ) {
            fo = fopen( out_filename, "wb" );
        }
        rc = EXIT_SUCCESS;
        for( i = 0; i < nofilenames; ++i ) {
            if( PP_Init( filenames[i], flags, NULL ) != 0 ) {
                fprintf( stderr, "Unable to open '%s'\n", filenames[i] );
                rc = EXIT_FAILURE;
                break;
            }
            for( j = 0; j < numdefs; j++ ) {
                PP_Define( defines[j] );
            }
            for( ;; ) {
                ch = PP_Char();
                if( ch == EOF )
                    break;
#ifndef __UNIX__
                if( ch == '\n' )
                    fputc( '\r', fo );
#endif
                fputc( ch, fo );
            }
            PP_Fini();
        }
        if( fo == stdout ) {
            fflush( fo );
        } else if( fo != NULL ) {
            fclose( fo );
        }
    }

    if( out_filename != NULL ) {
        free( out_filename );
    }
    for( i = 0; i < nofilenames; ++i ) {
        free( filenames[i] );
    }
    free( (void *)filenames );
    for( i = 0; i < numdefs; i++ ) {
        free( defines[i] );
    }
    free( (void *)defines );

    PP_IncludePathFini();

    if( rc == EXIT_FAILURE && nofilenames == 0 ) {
        Quit( usageMsg, "No filename specified\n" );
    }

    return( rc );
}
