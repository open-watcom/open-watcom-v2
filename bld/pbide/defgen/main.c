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
#include "defgen.h"
#include "scan.h"
#include "semantic.h"
#include "pathgrp2.h"

#include "clibext.h"


#define DEF_EXT     "def"
#define PBF_EXT     "pbf"
#define SRU_EXT     "sru"

CmdLineInfo     Config;
bool            ErrorHasOccured;

static char     outName[_MAX_FNAME + _MAX_EXT];
static char     nuoName[_MAX_FNAME + _MAX_EXT];

static void SetExtension( char *buf, const char *file, const char *ext )
{
    pgroup2     pg;

    _splitpath2( file, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    _makepath( buf, pg.dir, pg.drive, pg.fname, ext );
}

static bool FillParms( void )
{
    pgroup2     pg;

    if( Config.def_file == NULL ) {
        ReportError( "No .def file specified" );
        return( true );
    }
    if( Config.dll_name == NULL ) {
        Config.dll_name = "noname.dll";
    }
    if( Config.out_file == NULL ) {
        SetExtension( outName, Config.dll_name, PBF_EXT );
        Config.out_file = outName;
    }
    if( Config.nu_name == NULL && Config.nuo_file != NULL ) {
        _splitpath2( Config.dll_name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        sprintf( nuoName, "uo_%s", pg.fname );
        Config.nu_name = nuoName;
    } else if( Config.nuo_file == NULL && Config.nu_name != NULL ) {
        strcpy( pg.buffer, Config.nu_name );
        SetExtension( nuoName, pg.buffer, SRU_EXT );
        Config.nuo_file = nuoName;
    }
    return( false );
}

static bool ScanParms( int argc, char *argv[] )
{
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
                Config.debug = true;
            } else if( str[0] == 'd' && str[1] == 'l' && str[2] == 'l'
                        && str[3] == '=' ) {
                str += 4;
                Config.dll_name = str;
            } else if( str[0] == 'f' && str[1] == 'o' && str[2] == '=' ) {
                Config.out_file = str + 3;
            } else if( str[0] == 'n' && str[1] == 'u' ) {
                if( str[2] == '=' ) {
                    Config.nu_name = str + 3;
                } else if( str[2] == 'o' && str[3] == '=' ) {
                    Config.nuo_file = str + 4;
                }
            } else {
                ReportError( "Unrecognized Command Line Option" );
                return( true );
            }
        } else {
            if( used == allocated ) {
                Config.def_file = realloc( Config.def_file, ( allocated + 10 ) * sizeof( char * ) );
                allocated += 10;
            }
            Config.def_file[used] = str;
            used++;
        }
    }
    Config.def_cnt = used;
    return( FillParms() );
}


static void PrintUsage( void )
{
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

static void Fini( void )
{
}

int main( int argc, char *argv[] )
{
    unsigned    i;
    char        *def_file;

    if( argc == 1 ) {
        PrintUsage();
        return( 0 );
    }
    if( !ScanParms( argc, argv ) ) {
        def_file = malloc( _MAX_PATH );
        for( i = 0; i < Config.def_cnt; i++ ) {
            SetExtension( def_file, Config.def_file[i], DEF_EXT );
            if( !ScanInit( def_file ) ) {
                yyparse();
                ScanFini();
            }
        }
        free( def_file );
        if( !ErrorHasOccured )
            DoOutput();
        Fini();
    }
    if( ErrorHasOccured ) {
        return( 1 );
    } else {
        return( 0 );
    }
}
