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
* Description:  Watcom's implementation of the classic YACC tool.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "yacc.h"
#include "alloc.h"
#include "walloca.h"

FILE *yaccin, *actout, *tokout;
char *loadpath, *srcname;
char *codefilename;
char *headerfilename;
char *descfilename;

char lineflag;
char bigflag;
char fastflag;
char denseflag;
char default_shiftflag;
char compactflag;
char eliminateunitflag;
char showflag;
char translateflag;
char defaultwarnflag = 1;
char verbosefileflag;
char headerfileflag = 1;

char *symprefix = "yy";

char param_requested;

index_n RR_conflicts;
index_n SR_conflicts;
index_n nstate_1_reduce;

static int warnings;
static int proflag;

static FILE *openr( char *filename )
{
    FILE *file;

    file = fopen( filename, "r" );
    if( file == NULL ) {
        msg( "Can't open %s.\n", filename );
    }
    return( file );
}

static FILE *openw( char *filename )
{
    FILE *file;

    file = fopen( filename, "w" );
    if( file == NULL ) {
        msg( "Can't open %s for output.\n", filename );
    }
    return( file );
}

static void setoptions( char *p )
{
    for( ; *p; ++p ) {
        switch( *p ) {
        case 'b':
            param_requested = 'b';
            break;
        case 'd':
            if( p[1] == 'b' ) {
                ++p;
                bigflag = 1;
            } else if( p[1] == 'c' ) {
                ++p;
                compactflag = 1;
            } else if( p[1] == 'd' ) {
                ++p;
                denseflag = 1;
            } else if( p[1] == 'f' ) {
                ++p;
                fastflag = 1;
            } else if( p[1] == 'h' ) {
                ++p;
                default_shiftflag = 1;
            } else if( p[1] == 'p' ) {
                ++p;
                proflag = 1;
            } else if( p[1] == 's' ) {
                ++p;
                showflag = 1;
            } else if( p[1] == 't' ) {
                ++p;
                translateflag = 1;
            } else if( p[1] == 'u' ) {
                ++p;
                eliminateunitflag = 1;
            } else if( p[1] == 'w' ) {
                ++p;
                defaultwarnflag = 0;
            } else {
                headerfileflag = 1;
            }
            break;
        case 'l':
            lineflag = 1;
            break;
        case 'p':
            param_requested = 'p';
            break;
        case 'v':
            verbosefileflag = 1;
            break;
        default:
            msg( "Unknown option '%c'\n", *p );
        }
    }
}

void warn( char *fmt, ... )
{
    va_list arg_ptr;

    if( srcname != NULL ) {
        printf( "%s(%d): ", srcname, lineno );
    }
    printf( "Warning! " );
    va_start( arg_ptr, fmt );
    vprintf( fmt, arg_ptr );
    va_end( arg_ptr );
    ++warnings;
}

void msg( char *fmt, ... )
{
    va_list arg_ptr;

    if( srcname != NULL ) {
        printf( "%s(%d): ", srcname, lineno );
    }
    printf( "Error! " );
    va_start( arg_ptr, fmt );
    vprintf( fmt, arg_ptr );
    va_end( arg_ptr );
    exit( 1 );
}

void dumpstatistic( char *name, unsigned stat )
{
    size_t len;

    len = strlen( name );
    printf( "%s:", name );
    while( len < 39 ) {
        putchar( ' ' );
        ++len;
    }
    printf( "%6u\n", stat );
}

int main( int argc, char **argv )
{
    int     i;
    FILE    *skeleton, *temp, *save;
    int     ch;
    char    tempfname[10];
    char    *fileprefix = "y";
    size_t  size;

    param_requested = 0;
    for( i = 1; i < argc; ++i ) {
        ch = argv[i][0];
        switch( param_requested ) {
        case 'b':
            if( ch != '-' ) {
                if( ch != '\0' )
                    fileprefix = argv[i];
                ++i;
            }
            break;
        case 'p':
            if( ch != '-' ) {
                if( ch != '\0' )
                    symprefix = argv[i];
                ++i;
            }
            break;
        default:
            break;
        }
        param_requested = 0;
        if( argv[i][0] != '-' )
            break;
        setoptions( &argv[i][1] );
    }
    if( i != argc - 1 && i != argc - 2 ) {
        puts( "usage: yacc [-dlv] [-d[bcdfhpstuw]] [-b <file_prefix>] [-p <sym_prefix>]" );
        puts( "                  <grammar> [<driver>]" );
        puts( "options:" );
        puts( "    -b <file_prefix> file prefix used in place of default 'y' prefix" );
        puts( "    -d    output header file" );
        puts( "    -db   output bigger (less optimal) tables" );
        puts( "    -dc   output compact tables (slower to execute)" );
        puts( "    -dd   use dense tokens (no '+' style tokens allowed)" );
        puts( "    -df   output fast tables (larger size)" );
        puts( "    -dh   use 'default shift' optimization" );
        puts( "    -dp   dump all productions" );
        puts( "    -ds   dump full state tables" );
        puts( "    -dt   translate 'keyword' to Y_KEYWORD, '++' to Y_PLUS_PLUS, etc." );
        puts( "    -du   eliminate useless unit production reduction" );
        puts( "    -dw   disable default action type checking" );
        puts( "    -l    output #line directives" );
        puts( "    -p <sym_prefix> symbol prefix used in place of default 'yy' prefix" );
        puts( "    -v    write description and report into file" );
        exit( 1 );
    }
    skeleton = NULL;
    if( i == argc - 2 ) {
        skeleton = openr( argv[argc - 1] );
        if( skeleton == NULL ) {
            msg( "could not open driver source code '%s'\n", argv[argc - 1] );
        }
    }
    loadpath = argv[0];
    *getname( loadpath ) = '\0';
    srcname = argv[i];
    if( !strrchr( srcname, '.' ) ) {
        srcname = alloca( strlen( argv[i] )+3 );
        srcname = strcat( strcpy( srcname, argv[i] ), ".y" );
    }
    yaccin = openr( srcname );

    size = strlen( fileprefix);
    codefilename   = MALLOC( size + 6, char );
    strcat( strcpy( codefilename, fileprefix), "tab.c" );
    headerfilename = MALLOC( size + 6, char );
    strcat( strcpy( headerfilename, fileprefix), "tab.h" );
    descfilename   = MALLOC( size + 5, char );
    strcat( strcpy( descfilename, fileprefix), ".out" );
    actout = openw( codefilename );
    tokout = openw( headerfilename );

    defs();
    temp = NULL;
    for( i = 0; i < 1000 && temp == NULL; ++i ) {
        sprintf( tempfname, "ytab.%3d", i );
        temp = fopen( tempfname, "w+" );
    }
    if( temp == NULL ) {
        msg( "Cannot create temporary file\n" );
    }
    save = actout;
    actout = temp;
    rules();
    actout = save;
    buildpro();
    CalcMinSentence();
    if( proflag || showflag ) {
        showpro();
    }
    lr0();
    lalr1();
    SetupStateTable();
    /* apply state filters */
    FindUnused();
    if( eliminateunitflag ) {
        EliminateUnitReductions();
    }
    if( default_shiftflag ) {
        if( keyword_id_low == 0 ) {
            msg( "No %%keyword_id <low> <high> specified." );
        } else {
            MarkDefaultShifts();
        }
    }
    MarkNoUnitRuleOptimizationStates();
    RemoveDeadStates();
    MarkDefaultReductions();
    if( showflag ) {
        showstates();
    }
    if( warnings ) {
        if( warnings == 1 ) {
            printf( "%s: 1 warning\n", srcname );
        } else {
            printf( "%s: %d warnings\n", srcname, warnings );
        }
        exit( 1 );
    }
    parsestats();
    dumpstatistic( "parser states", nstate );
    dumpstatistic( "# states (1 reduce only)", nstate_1_reduce );
    dumpstatistic( "reduce/reduce conflicts", RR_conflicts );
    dumpstatistic( "shift/reduce conflicts", SR_conflicts );
    show_unused();
    rewind( tokout );
    while( (ch = fgetc( tokout )) != EOF ) {
        fputc( ch, actout );
    }
    if( skeleton == NULL ) {
        skeleton = fpopen( loadpath, "yydriver.c" );
        if( skeleton == NULL ) {
            msg( "Can't find yacc skeleton yydriver.c\n" );
        }
    }
    while( (ch = fgetc( skeleton )) != '\f' && ch != EOF ) {
        fputc( ch, actout );
    }
    genobj();
    while( (ch = fgetc( skeleton )) != '\f' && ch != EOF ) {
        fputc( ch, actout );
    }
    rewind( temp );
    while( (ch = fgetc( temp )) != EOF ) {
        fputc( ch, actout );
    }
    while( (ch = fgetc( skeleton )) != EOF ) {
        fputc( ch, actout );
    }
    fclose( skeleton );
    tail();
    fclose( temp );
    remove( tempfname );
    FREE( codefilename );
    FREE( headerfilename );
    FREE( descfilename );
    return( 0 );
}
