/****************************************************************
Copyright (C) Lucent Technologies 1997
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name Lucent Technologies or any of
its entities not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

const char  *version = "version 20110810";

#define DEBUG
#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "awk.h"
#include "ytab.h"

extern  int     nfields;

int         dbg     = 0;
Awkfloat    srand_seed = 1;
char        *cmdname;           /* gets argv[0] for error messages */
extern FILE *yyin;              /* lex input file */
char        *lexprog;           /* points to program argument if it exists */
extern int  errorflag;          /* non-zero if any syntax errors; set by yyerror */
int         compile_time = 2;   /* for error printing: */
                                /* 2 = cmdline, 1 = compile, 0 = running */

#define MAX_PFILE       20      /* max number of -f's */

char    *pfile[MAX_PFILE];      /* program filenames from -f's */
int     npfile = 0;             /* number of filenames */
int     curpfile = 0;           /* current filename */

bool    safe = false;           /* true => "safe" mode */

static int ParseEnvVar( const char *env, char **argv, char *buf )
/***************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *env.
     * Unless argv is NULL, both argv and buf are completed.
     */

    const char  *start;
    int         argc;
    char        *bufend;
    int         got_quote;
    int         output_data;

    output_data = ( buf != NULL ) && ( argv != NULL );
    bufend = buf;
    argc = 1;
    for( ;; ) {
        got_quote = 0;
        while( isspace( *env ) && *env != '\0' )
            env++;
        start = env;
        if( output_data ) {
            argv[argc] = bufend;
        }
        if( *env == '-' ) {
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

static char *unquote( char *str )
{
    size_t  len;

    len = strlen( str );
    if( len > 1 && str[0] == '"' && str[len - 1] == '"' ) {
        str[len - 1] = '\0';
        ++str;
    }
    return( str );
}

int main( int argc, char *argv[] )
{
    const char  *fs = NULL;
    char        **eargv = NULL;
    int         eargc;

    setlocale( LC_CTYPE, "" );
    setlocale( LC_NUMERIC, "C" ); /* for parsing cmdline & prog */
    cmdname = argv[0];
    if( argc == 1 ) {
        fprintf( stderr,
          "usage: %s [-F fs] [-v var=value] [-f progfile | 'prog'] [file ...]\n",
          cmdname );
        return( 1 );
    }
    signal( SIGFPE, fpecatch );

    srand_seed = 1;
    srand( (unsigned)srand_seed );

    yyin = NULL;
    symtab = makesymtab( NSYMTAB/NSYMTAB );
    if( argc == 2 && argv[1][0] == '@' && argv[1][1] != '\0' ) {
        const char  *env;

        env = getenv( &argv[1][1] );
        if( env != NULL ) {
            eargc = ParseEnvVar( env, NULL, NULL );  // count parameters.
            eargv = malloc( eargc * sizeof( char * ) + strlen( env ) + 1 + eargc );
            ParseEnvVar( env, eargv, (char *)( eargv + eargc ) );
            argc = eargc;
            argv = eargv;
        }
    }
    while( argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0' ) {
        if( strcmp( argv[1], "-version" ) == 0 || strcmp( argv[1], "--version" ) == 0 ) {
            printf( "awk %s\n", version );
            return( 0 );
        }
        if( strncmp( argv[1], "--", 2 ) == 0 ) {   /* explicit end of args */
            argc--;
            argv++;
            break;
        }
        switch( argv[1][1] ) {
        case 's':
            if( strcmp( argv[1], "-safe" ) == 0 )
                safe = true;
            break;
        case 'f':       /* next argument is program filename */
            if( argv[1][2] != '\0' ) {  /* arg is -fsomething */
                if( npfile >= MAX_PFILE - 1 ) {
                    FATAL( "too many -f options" );
                }
                pfile[npfile++] = unquote( &argv[1][2] );
            } else {        /* arg is -f something */
                argc--; argv++;
                if( argc <= 1 ) {
                    FATAL( "no program filename" );
                }
                if( npfile >= MAX_PFILE - 1 ) {
                    FATAL( "too many -f options" );
                }
                pfile[npfile++] = unquote( argv[1] );
            }
            break;
        case 'F':       /* set field separator */
            if( argv[1][2] != '\0' ) {  /* arg is -Fsomething */
                if( argv[1][2] == 't' && argv[1][3] == '\0' ) {     /* wart: t=>\t */
                    fs = "\t";
                } else if( argv[1][2] != '\0' ) {
                    fs = &argv[1][2];
                }
            } else {        /* arg is -F something */
                argc--; argv++;
                if( argc > 1 && argv[1][0] == 't' && argv[1][1] == '\0' ) { /* wart: t=>\t */
                    fs = "\t";
                } else if( argc > 1 && argv[1][0] != '\0' ) {
                    fs = &argv[1][0];
                }
            }
            if( fs == NULL || *fs == '\0' )
                WARNING( "field separator FS is empty" );
            break;
        case 'v':       /* -v a=1 to be done NOW.  one -v for each */
            if( argv[1][2] != '\0' ) {  /* arg is -vsomething */
                char *p;

                p = unquote( &argv[1][2] );
                if( isclvar( p ) ) {
                    setclvar( p );
                } else {
                    FATAL( "invalid -v option argument: %s", p );
                }
            } else {        /* arg is -v something */
                char *p;

                argc--; argv++;
                if( argc <= 1 ) {
                    FATAL( "no variable name" );
                }
                p = unquote( argv[1] );
                if( isclvar( p ) ) {
                    setclvar( p );
                } else {
                    FATAL( "invalid -v option argument: %s", p );
                }
            }
            break;
        case 'd':
            dbg = atoi( &argv[1][2] );
            if( dbg == 0 )
                dbg = 1;
            printf( "awk %s\n", version );
            break;
        default:
            WARNING( "unknown option %s ignored", argv[1] );
            break;
        }
        argc--;
        argv++;
    }
    /* argv[1] is now the first argument */
    if( npfile == 0 ) {      /* no -f; first argument is program */
        char *p;

        if( argc <= 1 ) {
            if( dbg )
                return( 0 );
            FATAL( "no program given" );
        }
        p = unquote( argv[1] );
        dprintf(( "program = |%s|\n", p ));
        lexprog = p;
        argc--;
        argv++;
    }
    for( eargc = 1; eargc < argc; ++eargc ) {
        argv[eargc] = unquote( argv[eargc] );
    }
    recinit( recsize );
    syminit();
    compile_time = 1;
    argv[0] = cmdname;      /* put prog name at front of arglist */
    dprintf(( "argc=%d, argv[0]=%s\n", argc, argv[0] ));
    arginit( argc, argv );
    if( !safe )
        envinit( environ );
    yyparse();
    setlocale( LC_NUMERIC, "" ); /* back to whatever it is locally */
    if( fs )
        *FS = qstring( fs, '\0' );
    dprintf(( "errorflag=%d\n", errorflag ));
    if( errorflag == 0 ) {
        compile_time = 0;
        run( winner );
    } else {
        bracecheck();
    }
    return( errorflag );
}

int pgetc( void )     /* get 1 character from awk program */
{
    int c;

    for( ;; ) {
        if( yyin == NULL ) {
            if( curpfile >= npfile )
                return( EOF );
            if( strcmp( pfile[curpfile], "-" ) == 0 ) {
                yyin = stdin;
            } else if( (yyin = fopen( pfile[curpfile], "r" )) == NULL ) {
                FATAL( "can't open file %s", pfile[curpfile] );
            }
            lineno = 1;
        }
        if( (c = getc( yyin )) != EOF )
            return( c );
        if( yyin != stdin )
            fclose( yyin );
        yyin = NULL;
        curpfile++;
    }
}

char *cursource( void )   /* current source file name */
{
    if( npfile > 0 ) {
        return( pfile[curpfile] );
    } else {
        return( NULL );
    }
}
