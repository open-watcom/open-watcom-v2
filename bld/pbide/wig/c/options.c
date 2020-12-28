/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "global.h"
#include "options.h"
#include "error.h"
#include "pathgrp2.h"

#include "clibext.h"


/* required constants */
#ifndef MAX_PATH
#define MAX_PATH                _MAX_PATH
#endif
#define BRAIN_DEAD_DOS_SIZE     8
// max size of a Powerbuilder identifier
#define CPP_ID_LEN              50

unsigned long Options;

/* configuration strings */
typedef struct wig_opts {
    char        basename[CPP_ID_LEN];           // user object name
    char        libname[MAX_PATH];              // DLL name
    char        infile[MAX_PATH];               // input SRU file
    char        outfile[MAX_PATH];              // output SRU file
    char        codefile[MAX_PATH];             // C++ file
    char        coverfile[MAX_PATH];            // cover function file
    char        headerfile[MAX_PATH];           // header file
    char        errfile[MAX_PATH];              // error file
    char        lmainfile[MAX_PATH];            // libmain and WEP file
    char        parentclass[CPP_ID_LEN];        // parent class name
    char        parentheader[MAX_PATH];         // parent's header
} wig_opts;

static wig_opts wigOpts;

/* text prefixxes and suffixxes */
#define LIBMAIN_FNAME   "lmain"

char    *GetInputFile( void ) {
/*****************************/

    return( wigOpts.infile );
}


char    *GetErrorFile( void ) {
/******************************/

    return( wigOpts.errfile );
}

char    *GetOutputFile( void ) {
/******************************/

    return( wigOpts.outfile );
}


char    *GetCoverFile( void ) {
/*****************************/

    return( wigOpts.coverfile );
}


char    *GetHeaderFile( void ) {
/******************************/

    return( wigOpts.headerfile );
}


char    *GetCodeFile( void ) {
/****************************/

    return( wigOpts.codefile );
}


char    *GetClass( void ) {
/*****************************/

    return( wigOpts.basename );
}


char    *GetLibName( void ) {
/***************************/

    return( wigOpts.libname );
}


char    *GetParentClass( void ) {
/********************************/

    if( !( Options & OPT_PARENT_SET ) ) {
        return( NULL );
    } else {
        return( wigOpts.parentclass );
    }
}

char    *GetParentHeader( void ) {
/*********************************/
    return( wigOpts.parentheader );
}

char    *GetLmainName( void ) {
/******************************/
    return( wigOpts.lmainfile );
}

#define HASHLEN         2
static const char hashChars[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };

void GetHash( const char *uoname, char *hash, unsigned len ) {
/************************************************************/

    unsigned long       mod;
    unsigned long       value;
    unsigned            numchars;
    unsigned            i;

    value = 0;
    numchars = sizeof( hashChars ) - 1;
    mod = 1;
    for( i=0; i < len; i++ ) {
        mod *= numchars;
    }
    while( *uoname != '\0' ) {
        value += tolower( *uoname );
        value = value % mod;
        uoname++;
    }
    for( i = 0; i < len; i++ ) {
        mod /= numchars;
        hash[i] = hashChars[ value / mod ];
        value = value % mod;
    }
    hash[len] = '\0';
}

#define HASH_LEN        2

#undef _MAX_FNAME
#define _MAX_FNAME 9    // for consistent naming across platforms

static void getFname( const char *uoname, char prefix, char *fname )
/******************************************************************/
{
    char        hash[HASHLEN + 1];
    unsigned    len;
    unsigned    prefix_len;


    len = strlen( uoname );
    if( len < _MAX_FNAME - 2 ) {
        if( prefix != '\0' ) {
            *fname = prefix;
            fname ++;
        }
        strcpy( fname, uoname );
    } else {
        GetHash( uoname, hash, HASH_LEN );
        memset( fname, 0, _MAX_FNAME );
        if( prefix != '\0' ) {
            fname[0] = prefix;
            fname++;
            prefix_len = 1;
        } else {
            prefix_len = 0;
        }
        strncpy( fname, uoname, _MAX_FNAME - 1 - HASHLEN - prefix_len );
        strcat( fname, hash );
    }
}

void    SetBaseName( const char *src )
/************************************/
{
    char        fname[_MAX_FNAME];
    pgroup2     pg;

    assert( src );
    getFname( src, 0, fname );

    /* derive configuration strings from the provided base name */

    strncpy( wigOpts.basename, src, MAX_PATH );

    _splitpath2( wigOpts.infile, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );

    if( !( Options & OPT_LIBRARY_NAME_SET ) ) {
        _makepath( wigOpts.libname, pg.drive, pg.dir, fname, DLL_EXT );
    }
    if( !( Options & OPT_HEADER_FILE_SET ) ) {
        _makepath( wigOpts.headerfile, pg.drive, pg.dir, fname, HPP_EXT );
    }
    if( !( Options & OPT_ERROR_FILE_SET ) ) {
        _makepath( wigOpts.errfile, pg.drive, pg.dir, fname, ERR_EXT );
    }
    if( !( Options & OPT_CODE_FILE_SET ) ) {
        if( Options & OPT_GEN_C_CODE ) {
            _makepath( wigOpts.codefile, pg.drive, pg.dir, fname, C_EXT );
        } else {
            _makepath( wigOpts.codefile, pg.drive, pg.dir, fname, CPP_EXT );
        }
    }
    if( !( Options & OPT_COVER_FILE_SET ) ) {
        getFname( src, 'c', fname );
        _makepath( wigOpts.coverfile, pg.drive, pg.dir, fname, CPP_EXT );
    }
}

void PostProcessOptions( void )
/*****************************/
{
    char        fname[_MAX_FNAME];
    pgroup2     pg;

    if( Options & OPT_PARENT_SET ) {
        if( (Options & OPT_PARENT_HEADER_SET) == 0 ) {
            getFname( wigOpts.parentclass, 0, fname );
            _makepath( wigOpts.parentheader, NULL, NULL, fname, HPP_EXT );
        }
    }
    _splitpath2( wigOpts.infile, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
    _makepath( wigOpts.lmainfile, pg.drive, pg.dir, LIBMAIN_FNAME, CPP_EXT );
}

static void showUsage( void ) {
/*****************************/

#ifndef __WINDOWS__
    puts("Usage:  wig [options] [input file] [options]");
    puts("Options:");
    puts("            ( /option is also accepted )");
//    puts("    -c                 Generate interface to C (default C++)");
    puts("    -fc=<file name>    Set cover functions file name");
    puts("    -fe=<file name>    Set error file name");
    puts("    -fh=<file name>    Set header file name");
    puts("    -fo=<file name>    Set stub file name");
//    puts("    -fs=<file name>    Set output .SRU file (default same as input)");
    puts("    -(h|H|?)           Display this screen");
    puts("    -in=<obj name>     Specify user object to inherit from");
    puts("    -ih=<file name>    Specify header file to inherit from");
    puts("    -l=<dll name>      Set library name to be used");
    puts("    -r                 Generate returns for functions");
    puts("");
#endif
}

bool    ProcessOptions( int argc, char **argv )
/*********************************************/
{
    int         count = 1;
    bool        err = false;
    bool        stop = false;
    bool        multi_ch = false;

    assert( argc );
    assert( argv );

    argc--;
    Options = 0;

    if( !argc ) {
        showUsage();
        return( true );
    }

    /* loop through all options and record them, and issue errors if necessary*/

    while( argc ) {
        assert( argv[count] );

        if( ( argv[count][0] == '-' ) || ( argv[count][0] == '/' ) ) {
            switch( argv[count][1] ) {
            case( 'e' ):        // undocumented feature for use until
                                // the new PB C++ UO painter is complete
                if( argv[count][2] == 's' ) {
                    Options |= OPT_MODIFY_SRU;
                    multi_ch = true;
                }
                break;
            case( 'f' ):
            case( 'F' ):
                multi_ch = true;
                if( ( argv[count][3] != '=' ) || !strlen(&(argv[count][4]) ) ) {
                    err = true;
                    break;
                }
                switch( argv[count][2] ) {
                case( 'c' ):
                case( 'C' ):
                    strncpy(wigOpts.coverfile, &(argv[count][4]), MAX_PATH);
                    Options |= OPT_COVER_FILE_SET;
                    break;
                case( 'h' ):
                case( 'H' ):
                    strncpy(wigOpts.headerfile, &(argv[count][4]), MAX_PATH);
                    Options |= OPT_HEADER_FILE_SET;
                    break;
                case( 'o' ):
                case( 'O' ):
                    strncpy(wigOpts.codefile, &(argv[count][4]), MAX_PATH);
                    Options |= OPT_CODE_FILE_SET;
                    break;
                case( 'e' ):
                case( 'E' ):
                    strncpy( wigOpts.errfile, &(argv[count][4]), MAX_PATH );
                    Options |= OPT_ERROR_FILE_SET;
                    break;
//              case( 's' ):
//              case( 'S' ):
//                  strncpy(wigOpts.outfile, &(argv[count][4]), MAX_PATH);
//                  Options |= OPT_OUTPUT_FILE_SET;
//                  break;
                default:
                    err = true;
                    break;
                }
                break;
            case( 'i' ):
            case( 'I' ):
                switch( argv[count][2] ) {
                case( 'n' ):
                case( 'N' ):
                    multi_ch = true;
                    if( argv[count][3] != '=' || !strlen( &(argv[count][4]) ) ){
                        err = true;
                    } else {
                        strcpy( wigOpts.parentclass, argv[count] + 4 );
                        Options |= OPT_PARENT_SET;
                    }
                    break;
                case( 'h' ):
                case( 'H' ):
                    multi_ch = true;
                    if( argv[count][3] != '=' || !strlen( &(argv[count][4]) ) ){
                        err = true;
                    } else {
                        strcpy( wigOpts.parentheader, argv[count] + 4 );
                        Options |= OPT_PARENT_HEADER_SET;
                    }
                    break;
                default:
                    err = true;
                    break;
                }
                break;
            case( 'r' ):
            case( 'R' ):
                Options |= OPT_GEN_RETURN;
                break;
//          case( 'C' ):
//          case( 'c' ):
//              Options |= OPT_GEN_C_CODE;
//              break;
            case( 'L' ):
            case( 'l' ):
                multi_ch = true;
                if( ( argv[count][2] != '=' ) || !strlen( &(argv[count][3]) ) ){
                    err = true;
                    break;
                }
                strncpy( wigOpts.libname, &(argv[count][3]), MAX_PATH );
                Options |= OPT_LIBRARY_NAME_SET;
                break;
            case( 'h' ):
            case( 'H' ):
            case( '?' ):
                showUsage();
                stop = true;
                break;
            default:
                err = true;
            }
            if( err || ( ( strlen( argv[count] ) > 2 ) && !multi_ch ) ) {
                Error( CLI_BAD_CMD_OPT, argv[ count ] );
                stop = true;
            }
        } else {
            if( Options & OPT_INPUT_FILE_SET ) {
                Error( CLI_MULTI_INPUTS, argv[count] );
                stop = true;
            } else {
                strncpy( wigOpts.infile, argv[count], MAX_PATH );
                Options |= OPT_INPUT_FILE_SET;
                if( !(Options & OPT_OUTPUT_FILE_SET ) ) {
                    strncpy( wigOpts.outfile, wigOpts.infile, MAX_PATH );
                }
            }
        }
        err = false;
        multi_ch = false;
        count++;
        argc--;
    }
    if( !stop && !(Options & OPT_INPUT_FILE_SET) ) {
        Error( CLI_NO_INPUT_FILE );
        stop = true;
    }
    if( stop )
        return( true );
    PostProcessOptions();
    return( false );
}
