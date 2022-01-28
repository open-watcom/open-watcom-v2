/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "_preproc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "misc.h"
#include "iopath.h"
#include "swchar.h"


typedef enum {
    MBCP_NONE       = 0,
    MBCP_KANJI      = 1,
    MBCP_CHINESE    = 2,
    MBCP_KOREAN     = 3,
    MBCP_UTF8       = 4,
} mb_codepage;

static char     **defines = NULL;
static int      numdefs = 0;
static char     **filenames = NULL;
static int      nofilenames = 0;
static char     *out_filename = NULL;

static const char * const usageMsg[] = {
    "Usage: wcpp [options] input files",
    "",
    "  options:",
    "    -c             preserve comments",
    "    -d<macro>      define macro",
    "    -i<path>       include path",
    "    -l             generate #line directives",
    "    -o<file>       output file",
    "    -zk{0,1,2,u8}  source file character encoding",
    "       0           Japanese (Kanji, CP 932) double-byte encoding",
    "       1           Chinese (Traditional, CP 950) double-byte encoding",
    "       2           Korean (Wansung, CP 949) double-byte encoding",
    "       u8          Unicode UTF-8 encoding",
    "    -h or ?        display usage",
    "",
    "    input files    list of input source file names",
    NULL
};

static char         MBCharLen[256];         // multi-byte character len table
static int          mbcp = MBCP_NONE;

/* forward declaration */
static bool doScanParams( int argc, char *argv[], pp_flags *ppflags );

static void wcpp_quit( const char * const usage_msg[], const char *str, ... )
/***************************************************************************/
{
    va_list     al;

    if( str != NULL ) {
        va_start( al, str );
        vfprintf( stderr, str, al );
        va_end( al );
    }
    if( usage_msg != NULL ) {
        if( str != NULL ) {
            fprintf( stderr, "%s\n", *usage_msg );
        } else {
            for( ; *usage_msg != NULL; ++usage_msg ) {
                fprintf( stderr, "%s\n", *usage_msg );
            }
        }
    }
    exit( EXIT_FAILURE );
}

static char *my_strdup( const char *str )
/***************************************/
{
    size_t     len;
    char       *ptr;

    len = strlen( str ) + 1;
    ptr = malloc( len );
    return( memcpy( ptr, str, len ) );
}

static bool scanString( char *buf, const char *str, size_t len )
/**************************************************************/
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

static bool ScanOptionsArg( const char * arg, pp_flags *ppflags )
/***************************************************************/
{
    bool        contok;
    size_t      len;

    contok = true;

    switch( tolower( *arg ) ) {
    case 'c':
        *ppflags |= PPFLAG_KEEP_COMMENTS;
        break;
    case 'd':
        ++arg;
        defines = realloc( (void *)defines, ( numdefs + 1 ) * sizeof( char * ) );
        defines[numdefs++] = my_strdup( arg );
        break;
    case 'h':
        wcpp_quit( usageMsg, NULL );
        break;
    case 'i':
        {
            char    *p;

            ++arg;
            len = strlen( arg );
            p = malloc( len + 1 );
            scanString( p, arg, len );
            PP_IncludePathAdd( p );
            free( p );
        }
        break;
    case 'l':
        *ppflags |= PPFLAG_EMIT_LINE;
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
                mbcp = MBCP_KANJI;
                break;
            } else if( arg[1] == '1' && arg[2] == '\0' ) {
                mbcp = MBCP_CHINESE;
                break;
            } else if( arg[1] == '2' && arg[2] == '\0' ) {
                mbcp = MBCP_KOREAN;
                break;
            } else if( tolower( arg[1] ) == 'u' ) {
                if( arg[2] == '8' && arg[3] == '\0' ) {
                    mbcp = MBCP_UTF8;
                    break;
                }
            }
        }
        // fall down
    default:
        wcpp_quit( usageMsg, "Incorrect option" );
        break;
    }
    return( contok );
} /* ScanOptionsArg */

static int ParseVariable( const char *var, char **argv, char *buf )
/*****************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *var.
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
    bufstart = buf;
    bufend = buf;
    argc = 0;
    for( ;; ) {
        got_quote = false;
        while( isspace( *var ) && *var != '\0' )
            var++;
        start = var;
        if( output_data ) {
            bufstart = bufend;
        }
        if( *var == switchchar || *var == '-' ) {
            if( output_data ) {
                *bufend++ = *var;
            }
            var++;
        }
        while( ( got_quote || !isspace( *var ) ) && *var != '\0' ) {
            if( *var == '\"' ) {
                got_quote = !got_quote;
            }
            if( output_data ) {
                *bufend++ = *var;
            }
            var++;
        }
        if( start != var ) {
            if( output_data ) {
                *bufend++ = '\0';
                argv[argc] = bufstart;
            }
            argc++;
        }
        if( *var == '\0' ) {
            break;
        }
    }
    return( argc );
}

static bool scanEnvVarOrFile( const char *name, pp_flags *ppflags )
/*****************************************************************/
{
    /*
     * Pass nofilenames and analysis of getenv(name) into argc and argv
     * to doScanParams. Return view on usability of data. (true is usable.)
     *
     * Recursion is supported but circularity is rejected.
     */
    typedef struct VarInfo {
        struct VarInfo      *next;
        char                *name;
        char                **argv; /* points into buf */
        char                buf[1]; /* dynamic array */
    } VarInfo;

    int                 argc;
    VarInfo             *info;
    static VarInfo      *stack = NULL;  // Needed to detect recursion.
    size_t              argvsize;
    size_t              argbufsize;
    const char          *optstring;
    size_t              varlen;         // size to hold name copy.
    bool                contok;         // doScanParams Result.
    char                fbuf[512];

    optstring = PP_GetEnv( name );
    if( optstring == NULL ) {
        FILE *fh;

        fh = fopen( name, "rt" );
        if( fh == NULL ) {
//            RcWarning( ERR_ENV_VAR_NOT_FOUND, name );
            return( true );
        }
        optstring = fgets( fbuf, sizeof( fbuf ), fh );
        fclose( fh );
        if( optstring == NULL ) {
            fprintf( stderr, "Error reading file '%s'\n", name );
            return( true );
        }
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
    contok = doScanParams( argc, info->argv, ppflags );

    stack = info->next;                             // pop stack
    free( info );
    return( contok );
}

static bool doScanParams( int argc, char *argv[], pp_flags *ppflags )
/*******************************************************************/
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
            contok = ScanOptionsArg( arg + 1, ppflags ) && contok;
        } else if( *arg == '@' ) {
            contok = scanEnvVarOrFile( arg + 1, ppflags ) && contok;
        } else if( *arg == '?' ) {
            wcpp_quit( usageMsg, NULL );
//            contok = false;
        } else {
            filenames = realloc( (void *)filenames, ( nofilenames + 1 ) * sizeof( char * ) );
            filenames[nofilenames++] = my_strdup( arg );
        }
    }
    return( contok );
}

static void SetRange( char *p, int low, int high, char data )
/***********************************************************/
{
    int     i;

    for( i = low; i <= high; ++i ) {
        p[i] = data;
    }
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    return( MBCharLen[*(unsigned char *)p] + 1 );
}

extern void (* PPErrorCallback)( const char * );

static void myErrorMsg( const char *str )
/***************************************/
{
    fprintf( stderr, "%s\n", str );
}

int main( int argc, char *argv[] )
/********************************/
{
    int         ch;
    int         i;
    int         j;
    int         rc;
    FILE        *fo;
    pp_flags    ppflags;

    if( argc < 2 ) {
        wcpp_quit( usageMsg, "No filename specified" );
    } else if( argc == 2 ) {
        if( !strcmp( argv[1], "?" ) ) {
            wcpp_quit( usageMsg, NULL );
        }
    }
    PPErrorCallback = myErrorMsg;

    PP_IncludePathInit();

    rc = 0;
    memset( MBCharLen, 0, 256 );
    ppflags = PPFLAG_NONE | PPFLAG_TRUNCATE_FILE_NAME;
    if( doScanParams( argc - 1, argv + 1, &ppflags ) && nofilenames != 0 ) {
        switch( mbcp ) {
        case MBCP_KANJI:
            SetRange( MBCharLen, 0x81, 0x9f, 1 );
            SetRange( MBCharLen, 0xe0, 0xfc, 1 );
            break;
        case MBCP_CHINESE:
            SetRange( MBCharLen, 0x81, 0xfc, 1 );
            break;
        case MBCP_KOREAN:
            SetRange( MBCharLen, 0x81, 0xfd, 1 );
            break;
        case MBCP_UTF8:
            SetRange( MBCharLen, 0xc0, 0xdf, 1 );
            SetRange( MBCharLen, 0xe0, 0xef, 2 );
            SetRange( MBCharLen, 0xf0, 0xf7, 3 );
            SetRange( MBCharLen, 0xf8, 0xfb, 4 );
            SetRange( MBCharLen, 0xfc, 0xfd, 5 );
            break;
        }
        PP_Init( '#' );
        fo = stdout;
        if( out_filename != NULL ) {
            fo = fopen( out_filename, "wt" );
        }
        for( i = 0; i < nofilenames; ++i ) {
            if( PP_FileInit( filenames[i], ppflags, NULL ) != 0 ) {
                fprintf( stderr, "Unable to open '%s'\n", filenames[i] );
                rc = 1;
                break;
            }
            for( j = 0; j < numdefs; j++ ) {
                PP_Define( defines[j] );
            }
            for( ;; ) {
                ch = PP_Char();
                if( ch == EOF )
                    break;
                fputc( ch, fo );
            }
            PP_FileFini();
        }
        if( fo == stdout ) {
            fflush( fo );
        } else if( fo != NULL ) {
            fclose( fo );
        }
        rc = PP_Fini() | rc;
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

    if( rc == 0 && nofilenames == 0 ) {
        wcpp_quit( usageMsg, "No filename specified" );
    }

    return( ( rc ) ? EXIT_FAILURE : EXIT_SUCCESS );
}
