/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Assembly alias stubs generation utility.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif

#define ASM_FILE_EXT    ".gas"
#define OBJ_FILE_EXT    ".obj"

struct SysElem {
    char *              system;
    struct SysElem *    next;
};

struct Alias {
    char *              filename;
    char *              realname;
    char *              aliasname;
    struct SysElem *    systems;
};


/*
 * Automagically pass line number and file name to InternalError.
 */
#define Zoinks()        InternalError( __LINE__, __FILE__ )


/*
 * If format==NULL, a default error message is displayed using perror().
 * Otherwise, format is treated as the format string, and passed to
 * vfprintf() with any arguments specified after it.  This function
 * terminates the program, returning exit status EXIT_FAILURE.
 */
void FatalError( char *format, ... )
/**********************************/
{
    va_list             args;

    if( format == NULL ) {
        perror( "Fatal error" );
    } else {
        va_start( args, format );
        fprintf( stderr, "Fatal error: " );
        vfprintf( stderr, format, args );
        fprintf( stderr, "\n" );
        va_end( args );
    }

    exit( EXIT_FAILURE );
}


/*
 * Point out the offending location and exit with status EXIT_FAILURE.
 */
void InternalError( int line, char *file )
/****************************************/
{
    fprintf( stderr, "Internal error on line %d of %s.\n"
                     "Please contact the Open Watcom project at "
                     "https://github.com/open-watcom/open-watcom-v2\n",
                     line, file );
    exit( EXIT_FAILURE );
}


/*
 * Print a warning message.
 */
void Warning( char *format, ... )
/*******************************/
{
    va_list             args;

    va_start( args, format );
    fprintf( stderr, "Warning: " );
    vfprintf( stderr, format, args );
    fprintf( stderr, "\n" );
    va_end( args );
}


/*
 * Allocate a block of memory, aborting program execution with an error
 * message if there is insufficient memory.
 */
void *AllocMem( size_t bytes )
/****************************/
{
    void *              p;

    p = malloc( bytes );
    if( p == NULL )
        FatalError( "Insufficient memory." );
    return( p );
}


/*
 * Reallocate a block of memory, aborting program execution with an error
 * message if there is insufficient memory.
 */
void *ReallocMem( void *buf, size_t bytes )
/*****************************************/
{
    void *              p;

    p = realloc( buf, bytes );
    if( p == NULL )
        FatalError( "Insufficient memory." );
    return( p );
}


/*
 * Free a block of memory.
 */
void FreeMem( void *buf )
/***********************/
{
    free( buf );
}


/*
 * Duplicate a string.
 */
char *DupStrMem( char *str )
/**************************/
{
    char *              p;

    p = AllocMem( strlen( str ) + 1 );
    strcpy( p, str );
    return( p );
}


/*
 * Returns the number of whitespace-delimited words in 'command'.  If 'words'
 * is non-NULL, then each word will have memory allocated to hold it and will
 * be copied into the appropriate space in the array (e.g. the second word
 * will be copied into words[1]).  The array must be large enough, so just
 * call parse_words with 'words'==NULL before to get the proper size, then
 * do an alloca((n+1)*sizeof(char*)).  The caller is responsible for freeing
 * any memory allocated for the words.  Returns -1 on error.
 */

static int parse_words( const char *command, char **words )
/*********************************************************/
{
    int                 numWords = 0;
    const char *        p = command;
    const char *        pLookAhead;
    size_t              len;

    while( *p != '\0' ) {
        /*** Skip any leading whitespace ***/
        while( isspace( *p ) )
            p++;

        /*** Handle the word ***/
        if( *p == '\0' )
            break;
        pLookAhead = p;
        while( *pLookAhead != '\0'  &&  !isspace( *pLookAhead ) ) {
            pLookAhead++;
        }
        if( words != NULL ) {
            len = pLookAhead - p;       /* # of chars, excluding the null */
            words[numWords] = (char *)AllocMem( ( len + 1 ) * sizeof( char ) );
            strncpy( words[numWords], p, len );
            words[numWords][len] = '\0';
        }

        p = pLookAhead;
        numWords++;
    }
    if( words != NULL )
        words[numWords] = NULL;        /* last string */

    return( numWords );
}


/*
 * Add a system name to an Alias' system list.
 */
static void add_system( struct Alias *alias, char *system )
/*********************************************************/
{
    struct SysElem *    syselem;

    syselem = (struct SysElem*)AllocMem( sizeof( struct SysElem ) );
    syselem->system = system;
    syselem->next = alias->systems;
    alias->systems = syselem;
}

static void free_systems( struct SysElem *syselem )
{
    struct SysElem *syselem_next;

    for( ; syselem != NULL; syselem = syselem_next ) {
        syselem_next = syselem->next;
        FreeMem( syselem );
    }
}

/*
 * Add a line to the MIF file.
 */
static void update_mif_file( FILE *miffile, char *obj, struct Alias *alias )
/**************************************************************************/
{
    struct SysElem *    syselem;

    fprintf( miffile, "!inject %s", obj );
    syselem = alias->systems;
    while( syselem != NULL ) {
        fprintf( miffile, " %s", syselem->system );
        syselem = syselem->next;
    }
    fprintf( miffile, "\n" );

    if( ferror( miffile ) ) {
        FatalError( "Cannot write to MIF file." );
    }
}


/*
 * Make an AXP assembler file defining the alias.
 */
static void make_asm_axp( FILE *miffile, struct Alias *alias, char *outdir )
/**************************************************************************/
{
    char                filename[_MAX_PATH];
    FILE *              asmfile;

    /*** Open the assembler file ***/
    sprintf( filename, "%s_a%6s" ASM_FILE_EXT, outdir, alias->filename );
    asmfile = fopen( filename, "wt" );
    if( asmfile == NULL ) {
        FatalError( "Cannot create '%s'.", filename );
    }

    /*** Write data to it ***/
    fprintf( asmfile, "\t.text\n" );
    fprintf( asmfile, "\t.globl\t%s\n", alias->aliasname );
    fprintf( asmfile, "%s:\n", alias->aliasname );
    fprintf( asmfile, "\tbr\t%s\n", alias->realname );
    if( ferror( asmfile ) ) {
        FatalError( "Cannot write to '%s'.", filename );
    }
    fclose( asmfile );

    sprintf( filename, "_a%6s" OBJ_FILE_EXT, alias->filename );
    update_mif_file( miffile, filename, alias );
}


/*
 * Make an ix86 assembler file defining the alias.
 */
static void make_asm_ix86( FILE *miffile, struct Alias *alias, char *outdir )
/***************************************************************************/
{
    char                filename[_MAX_PATH];
    FILE *              asmfile;

    /*** Open the assembler file ***/
    sprintf( filename, "%s_i%6s" ASM_FILE_EXT, outdir, alias->filename );
    asmfile = fopen( filename, "wt" );
    if( asmfile == NULL ) {
        FatalError( "Cannot create '%s'.", filename );
    }

    /*** Write data to it ***/
    fprintf( asmfile, "include mdef.inc\n" );
    fprintf( asmfile, "alias_function %s, %s\n", alias->aliasname,
             alias->realname );
    fprintf( asmfile, "end\n" );
    if( ferror( asmfile ) ) {
        FatalError( "Cannot write to '%s'.", filename );
    }
    fclose( asmfile );

    sprintf( filename, "_i%6s" OBJ_FILE_EXT, alias->filename );
    update_mif_file( miffile, filename, alias );
}


/*
 * Make a PowerPC assembler file defining the alias.
 */
static void make_asm_ppc( FILE *miffile, struct Alias *alias, char *outdir )
/**************************************************************************/
{
    char                filename[_MAX_PATH];
    FILE *              asmfile;

    /*** Open the assembler file ***/
    sprintf( filename, "%s_p%6s" ASM_FILE_EXT, outdir, alias->filename );
    asmfile = fopen( filename, "wt" );
    if( asmfile == NULL ) {
        FatalError( "Cannot create '%s'.", filename );
    }

    /*** Write data to it ***/
    fprintf( asmfile, "\t.text\n" );
    fprintf( asmfile, "\t.globl\t%s\n", alias->aliasname );
    fprintf( asmfile, "%s:\n", alias->aliasname );
    fprintf( asmfile, "\tb\t%s\n", alias->realname );
    if( ferror( asmfile ) ) {
        FatalError( "Cannot write to '%s'.", filename );
    }
    fclose( asmfile );

    sprintf( filename, "_p%6s" OBJ_FILE_EXT, alias->filename );
    update_mif_file( miffile, filename, alias );
}


/*
 * Make a MIPS assembler file defining the alias.
 */
static void make_asm_mips( FILE *miffile, struct Alias *alias, char *outdir )
/***************************************************************************/
{
    char                filename[_MAX_PATH];
    FILE *              asmfile;

    /*** Open the assembler file ***/
    sprintf( filename, "%s_m%6s" ASM_FILE_EXT, outdir, alias->filename );
    asmfile = fopen( filename, "wt" );
    if( asmfile == NULL ) {
        FatalError( "Cannot create '%s'.", filename );
    }

    /*** Write data to it ***/
    fprintf( asmfile, "\t.text\n" );
    fprintf( asmfile, "\t.globl\t%s\n", alias->aliasname );
    fprintf( asmfile, "%s:\n", alias->aliasname );
    fprintf( asmfile, "\tj\t%s\n", alias->realname );
    if( ferror( asmfile ) ) {
        FatalError( "Cannot write to '%s'.", filename );
    }
    fclose( asmfile );

    sprintf( filename, "_m%6s" OBJ_FILE_EXT, alias->filename );
    update_mif_file( miffile, filename, alias );
}


/*
 * Create an alias.
 */
static void do_alias( FILE *miffile, struct Alias *alias, char *outdir )
/**********************************************************************/
{
    struct Alias        aliasaxp;
    struct Alias        aliasix86;
    struct Alias        aliasppc;
    struct Alias        aliasmips;
    struct SysElem *    syselem;

    /*** Initialize the CPU specific aliases ***/
    aliasix86.filename = alias->filename;
    aliasix86.realname = alias->realname;
    aliasix86.aliasname = alias->aliasname;
    aliasix86.systems = NULL;
    aliasaxp.filename = alias->filename;
    aliasaxp.realname = alias->realname;
    aliasaxp.aliasname = alias->aliasname;
    aliasaxp.systems = NULL;
    aliasppc.filename = alias->filename;
    aliasppc.realname = alias->realname;
    aliasppc.aliasname = alias->aliasname;
    aliasppc.systems = NULL;
    aliasmips.filename = alias->filename;
    aliasmips.realname = alias->realname;
    aliasmips.aliasname = alias->aliasname;
    aliasmips.systems = NULL;

    /*** Separate into groups by CPU type ***/
    for( syselem = alias->systems; syselem != NULL; syselem = syselem->next ) {
        if( !strcmp( syselem->system, "nta" ) ) {               /* AXP */
            add_system( &aliasaxp, syselem->system );
        } else if( !strcmp( syselem->system, "ntp" ) ) {        /* PPC */
            add_system( &aliasppc, syselem->system );
        } else if( !strcmp( syselem->system, "opc" ) ) {        /* PPC */
            add_system( &aliasppc, syselem->system );
        } else if( !strcmp( syselem->system, "lpc" ) ) {        /* PPC */
            add_system( &aliasppc, syselem->system );
        } else if( !strcmp( syselem->system, "ppc" ) ) {        /* PPC */
            add_system( &aliasppc, syselem->system );
        } else if( !strcmp( syselem->system, "lmp" ) ) {        /* MIPS */
            add_system( &aliasmips, syselem->system );
        } else if( !strcmp( syselem->system, "mps" ) ) {        /* MIPS */
            add_system( &aliasmips, syselem->system );
        } else if( !strcmp( syselem->system, "ntm" ) ) {        /* MIPS */
            add_system( &aliasmips, syselem->system );
        } else {                                                /* x86 */
            add_system( &aliasix86, syselem->system );
        }
    }

    /*** Build any necessary assembler files ***/
    if( aliasix86.systems != NULL ) {
        make_asm_ix86( miffile, &aliasix86, outdir );
        free_systems( aliasix86.systems );
    }
    if( aliasaxp.systems != NULL ) {
        make_asm_axp( miffile, &aliasaxp, outdir );
        free_systems( aliasaxp.systems );
    }
    if( aliasppc.systems != NULL ) {
        make_asm_ppc( miffile, &aliasppc, outdir );
        free_systems( aliasppc.systems );
    }
    if( aliasmips.systems != NULL ) {
        make_asm_mips( miffile, &aliasmips, outdir );
        free_systems( aliasmips.systems );
    }
}


/*
 * Handle one line.  Returns 0 if at EOF, and a positive value if there's
 * more data to handle.
 */
static int do_line( FILE *infile, FILE *miffile, char *outdir )
/*************************************************************/
{
    static int          curline;
    char                line[1024];
    char *              p;
    int                 numwords;
    char **             words;
    struct Alias        alias;
    int                 count;
    int                 rc;

    /*** Prepare the next line ***/
    p = fgets( line, sizeof( line ), infile );
    if( p == NULL ) {
        if( ferror( infile ) ) {
            FatalError( "Error reading line %d", curline );
        } else if( feof( infile ) ) {
            return( 0 );
        } else {
            Zoinks();
        }
    }
    if( line[0] == '#' ) {
        curline++;
        return( 2 );                    /* skip comment lines */
    }

    /*** Extract the individual words ***/
    numwords = parse_words( line, NULL );
    if( numwords == 0 ) {        /* skip blank lines */
        rc = 2;
        curline++;
    } else if( numwords < 4 ) {
        rc = -1;
        FatalError( "Error on line %d", curline );
    } else {
        words = (char **)AllocMem( ( numwords + 1 ) * sizeof( char * ) );
        if( words != NULL ) {
            numwords = parse_words( line, words );

            /*** Construct an Alias structure and create the alias ***/
            alias.filename = words[0];
            alias.realname = words[1];
            alias.aliasname = words[2];
            alias.systems = NULL;
            for( count=3; words[count]!=NULL; count++ ) {       /* build system list */
                add_system( &alias, words[count] );
            }
            do_alias( miffile, &alias, outdir );
            for( count = 0; words[count] != NULL; count++ ) {   /* free all words */
                FreeMem( words[count] );
            }
            FreeMem( words );
            free_systems( alias.systems );
        }
        rc = 1;
        curline++;
    }
    return( rc );
}


/*
 * Program entry point.
 */
int main( int argc, char *argv[] )
/*********************************/
{
    FILE *              infile;
    FILE *              miffile;
    char *              outdir;
    int                 alive = 1;
    int                 rc;

    /*** Parse the command line ***/
    if( argc != 4 ) {
        printf( "Usage: ALIASGEN <alias_file> <output_dir\\> <objects_mif_file>\n" );
        return( EXIT_FAILURE );
    }
    infile = fopen( argv[1], "rt" );    /* open alias file */
    if( infile == NULL ) {
        FatalError( "Cannot open '%s'.", argv[1] );
    }
    outdir = argv[2];                   /* get output directory */
    miffile = fopen( argv[3], "wt" );   /* open objects.mif file */
    if( miffile == NULL ) {
        FatalError( "Cannot create '%s'.", argv[3] );
    }

    /*** Handle the alias entries, one by one ***/
    while( alive ) {
        rc = do_line( infile, miffile, outdir );
        switch( rc ) {
        case 0:                         /* ok, but not done */
            alive = 0;
            break;
        case 2:                         /* comment */
            break;
        case 1:                         /* got one more */
            fputc( '.', stdout );
            fflush( stdout );
            break;
        default:
            Zoinks();
            break;
        }
    }
    fputc( '\n', stdout );

    return( EXIT_SUCCESS );
}
