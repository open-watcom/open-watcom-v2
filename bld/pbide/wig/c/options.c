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
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "global.h"
#include "options.h"
#include "error.h"

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
    char        outputdrive[_MAX_DRIVE];        // output drive
    char        outputdir[_MAX_DIR];            // output directory
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
#define HPP_EXT         ".hpp"
#define CPP_EXT         ".cpp"
#define C_EXT           ".c"
#define DLL_EXT         ".dll"
#define ERR_EXT         ".err"
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

void getFname( const char *uoname, char prefix, char *fname ) {
/********************************************************/
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

void    SetBaseName( const char *src ) {
/**************************************/

    char        fname[_MAX_FNAME];

    assert( src );
    getFname( src, 0, fname );

    /* derive configuration strings from the provided base name */

    strncpy( wigOpts.basename, src, MAX_PATH );

    if( !( Options & OPT_LIBRARY_NAME_SET ) ) {
        _makepath( wigOpts.libname, wigOpts.outputdrive, wigOpts.outputdir,
                    fname, DLL_EXT );
    }
    if( !( Options & OPT_HEADER_FILE_SET ) ) {
        _makepath( wigOpts.headerfile, wigOpts.outputdrive, wigOpts.outputdir,
                    fname, HPP_EXT );
    }
    if( !( Options & OPT_ERROR_FILE_SET ) ) {
        _makepath( wigOpts.errfile, wigOpts.outputdrive, wigOpts.outputdir,
                    fname, ERR_EXT );
    }
    if( !( Options & OPT_CODE_FILE_SET ) ) {
        if( Options & OPT_GEN_C_CODE ) {
            _makepath( wigOpts.codefile, wigOpts.outputdrive,
                        wigOpts.outputdir, fname, C_EXT );
        } else {
            _makepath( wigOpts.codefile, wigOpts.outputdrive,
                        wigOpts.outputdir, fname, CPP_EXT );
        }
    }
    if( !( Options & OPT_COVER_FILE_SET ) ) {
        getFname( src, 'c', fname );
        _makepath( wigOpts.coverfile, wigOpts.outputdrive, wigOpts.outputdir,
                    fname, CPP_EXT );
    }
}

void PostProcessOptions( void ) {
/********************************/
    char        fname[ _MAX_FNAME ];

    if( Options & OPT_PARENT_SET ) {
        if( !( Options & OPT_PARENT_HEADER_SET ) ) {
            getFname( wigOpts.parentclass, 0, fname );
            _makepath( wigOpts.parentheader, NULL, NULL, fname, HPP_EXT );
        }
    }
    _splitpath( wigOpts.infile, wigOpts.outputdrive, wigOpts.outputdir,
                NULL, NULL );
    _makepath( wigOpts.lmainfile, wigOpts.outputdrive, wigOpts.outputdir,
                LIBMAIN_FNAME, CPP_EXT );
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

BOOL    ProcessOptions( int argc, char **argv ) {
/***********************************************/

    int         count = 1;
    BOOL        err = FALSE;
    BOOL        stop = FALSE;
    BOOL        multi_ch = FALSE;

    assert( argc );
    assert( argv );

    argc--;
    Options = 0;

    if( !argc ) {
        showUsage();
        return( TRUE );
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
                    multi_ch = TRUE;
                }
                break;
            case( 'f' ):
            case( 'F' ):
                multi_ch = TRUE;
                if( ( argv[count][3] != '=' ) || !strlen(&(argv[count][4]) ) ) {
                    err = TRUE;
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
                    err = TRUE;
                    break;
                }
                break;
            case( 'i' ):
            case( 'I' ):
                switch( argv[count][2] ) {
                case( 'n' ):
                case( 'N' ):
                    multi_ch = TRUE;
                    if( argv[count][3] != '=' || !strlen( &(argv[count][4]) ) ){
                        err = TRUE;
                    } else {
                        strcpy( wigOpts.parentclass, argv[count] + 4 );
                        Options |= OPT_PARENT_SET;
                    }
                    break;
                case( 'h' ):
                case( 'H' ):
                    multi_ch = TRUE;
                    if( argv[count][3] != '=' || !strlen( &(argv[count][4]) ) ){
                        err = TRUE;
                    } else {
                        strcpy( wigOpts.parentheader, argv[count] + 4 );
                        Options |= OPT_PARENT_HEADER_SET;
                    }
                    break;
                default:
                    err = TRUE;
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
                multi_ch = TRUE;
                if( ( argv[count][2] != '=' ) || !strlen( &(argv[count][3]) ) ){
                    err = TRUE;
                    break;
                }
                strncpy( wigOpts.libname, &(argv[count][3]), MAX_PATH );
                Options |= OPT_LIBRARY_NAME_SET;
                break;
            case( 'h' ):
            case( 'H' ):
            case( '?' ):
                showUsage();
                stop = TRUE;
                break;
            default:
                err = TRUE;
            }
            if( err || ( ( strlen( argv[count] ) > 2 ) && !multi_ch ) ) {
                Error( CLI_BAD_CMD_OPT, argv[ count ] );
                stop = TRUE;
            }
        } else {
            if( Options & OPT_INPUT_FILE_SET ) {
                Error( CLI_MULTI_INPUTS, argv[count] );
                stop = TRUE;
            } else {
                strncpy( wigOpts.infile, argv[count], MAX_PATH );
                Options |= OPT_INPUT_FILE_SET;
                if( !(Options & OPT_OUTPUT_FILE_SET ) ) {
                    strncpy( wigOpts.outfile, wigOpts.infile, MAX_PATH );
                }
            }
        }
        err = FALSE;
        multi_ch = FALSE;
        count++;
        argc--;
    }
    if( !stop && !(Options & OPT_INPUT_FILE_SET) ) {
        Error( CLI_NO_INPUT_FILE );
        stop = TRUE;
    }
    if( stop ) return( TRUE );
    PostProcessOptions();
    return( FALSE );
}
