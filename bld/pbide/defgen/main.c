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
#include "defgen.h"
#include "scan.h"
#include "semantic.h"

CmdLineInfo     Config;
BOOL            ErrorHasOccured;

static char     outName[ _MAX_FNAME + _MAX_EXT  ];
static char     nuoName[ _MAX_FNAME + max( _MAX_EXT, 3 ) ];

extern yyparse();

static void SetExtension( char *buf, char *file, char *ext ) {
    char        dir[ _MAX_DIR ];
    char        drive[ _MAX_DRIVE ];
    char        fname[ _MAX_FNAME ];

    _splitpath( file, drive, dir, fname, NULL );
    _makepath( buf, dir, drive, fname, ext );
}

static BOOL FillParms( void ) {

    char        fname[ _MAX_FNAME ];

    if( Config.def_file == NULL ) {
        ReportError( "No .def file specified" );
        return( TRUE );
    }
    if( Config.dll_name == NULL ) {
        Config.dll_name = "noname.dll";
    }
    if( Config.out_file == NULL ) {
        SetExtension( outName, Config.dll_name, ".pbf" );
        Config.out_file = outName;
    }
    if( Config.nuo_name == NULL && Config.nuo_file != NULL ) {
        Config.nuo_name = nuoName;
        _splitpath( Config.dll_name, NULL, NULL, fname, NULL );
        sprintf( nuoName, "uo_%s", fname );
    } else if( Config.nuo_file == NULL && Config.nuo_name != NULL ) {
        Config.nuo_file = nuoName;
        memset( nuoName, 0, sizeof( nuoName ) );
        strncpy( nuoName, Config.nuo_name, _MAX_FNAME-1 );
        strcat( nuoName, ".sru" );
    }
    return( FALSE );
}

static BOOL ScanParms( int argc, char *argv[] ) {
    unsigned    i;
    char        *str;
    unsigned    allocated;
    unsigned    used;

    allocated = 0;
    used = 0;
    for( i=1; i < argc; i++ ) {
        str = argv[i];
        if( *str == '-' || *str == '/' ) {
            str++;
            if( *str == 'v' ) {
                Config.debug = TRUE;
            } else if( str[0] == 'd' && str[1] == 'l' && str[2] == 'l'
                        && str[3] == '=' ) {
                str += 4;
                Config.dll_name = str;
            } else if( str[0] == 'f' && str[1] == 'o' && str[2] == '=' ) {
                Config.out_file = str + 3;
            } else if( str[0] == 'n' && str[1] == 'u' ) {
                if( str[2] == '=' ) {
                    Config.nuo_name = str + 3;
                } else if( str[2] == 'o' && str[3] == '=' ) {
                    Config.nuo_file = str + 4;
                }
            } else {
                ReportError( "Unrecognized Command Line Option" );
                return( TRUE );
            }
        } else {
            if( used == allocated ) {
                Config.def_file = realloc( Config.def_file,
                                      ( allocated + 10 ) * sizeof( char * ) );
                allocated += 10;
            }
            Config.def_file[used] = str;
            used++;
        }
    }
    Config.def_cnt = used;
    return( FillParms() );
}


static void PrintUsage( void ) {
    printf( "Usage:\n" );
    printf( "    defgen [options] <list_of_def_files> [options]\n\n" );
    printf( "Options\n" );
    printf( "    -v              print scanner information\n" );
    printf( "    -dll=<dllname>  specify name of library\n" );
    printf( "    -fo=<outfile>   specify name of output file\n" );
    printf( "    -nu=<obj name>  generate non visual user object\n" );
    printf( "    -nuo=<outfile>  specify file name for non visual object\n" );
    printf( "\n" );
    printf( "All files in <list_of_def_files> are assumed to have the\n" );
    printf( "extension .def regardless of what extension is specified.\n" );
}

static void Fini( void ) {
}

int main( int argc, char *argv[] ) {

    unsigned    i;
    char        *def_file;

    if( argc == 1 ) {
        PrintUsage();
        return( 0 );
    }
    if( !ScanParms( argc, argv ) ) {
        def_file = malloc( _MAX_PATH );
        for( i=0; i < Config.def_cnt; i++ ) {
            SetExtension( def_file, Config.def_file[i], ".def" );
            if( !ScanInit( def_file ) ) {
                yyparse();
                ScanFini();
            }
        }
        free( def_file );
        if( !ErrorHasOccured ) DoOutput();
        Fini();
    }
    if( ErrorHasOccured ) {
        return( 1 );
    } else {
        return( 0 );
    }
}
