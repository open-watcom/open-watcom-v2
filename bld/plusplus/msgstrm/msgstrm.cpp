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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "msgstrm.h"
#include "inout.h"
#include "files.h"
#include "readgml.h"
#include "fini.h"
#include "model.h"

void usage( ) {
//~~~~~~~~~~~

    cout << "MSGSTRM - simple gml file processor for cpp files" <<
    endl << "usage: msgstrm < message-file > < model-file > [ options ]" <<
    endl << "OPTIONS: ( '-' options also accepted )" <<
    endl << "\t/o=< dest-file > Name of final run-file. Defaults to model-file.BAT" <<
    endl << "\t/d=< dest_dir > Directory in which to place cpp files. Defaults to '.'" <<
    endl << "\t/wig Don't create good & bad files, just process model file." <<
    endl << "\t/b=number starting message number for processing." <<
    endl << "\t/e=number ending message number for processing." <<
    endl;
};

const char * default_msg_ext = ".GML";
const char * default_model_ext = ".WIG";
const char * default_dest_ext = ".BAT";

char msg_file[ 256 ];
char model_file[ 256 ];
char dest_file[ 256 ];
char dest_dir[ 256 ];


static void MakeFileName(       // BUILD A FILE NAME
    char * s,                   // - target name
    const char * t,             // - argument
    const char * ext = NULL )   // - default extension
{
    char drive[ _MAX_DRIVE ];
    char dir  [ _MAX_DIR   ];
    char fname[ _MAX_FNAME ];
    char extn [ _MAX_EXT   ];

    _splitpath( t, drive, dir, fname, extn );
    if( ( extn[0] == '\0' ) && ( ext != NULL ) ) {
        _makepath( s, drive, dir, fname, ext );
    } else {
        _makepath( s, drive, dir, fname, extn );
    }
}


static unsigned numericOption(  // PROCESS A NUMERIC OPTION
    const char *p,              // - scanner
    const char *opt )           // - name of option
{
    unsigned value;             // - option value

    ++p;
    if( ( *p != '=' ) || ( p[ 1 ] == '\0' ) ) {
        cout << "Error - no" << opt << " specified." << endl;
        DieInFlames( );
    } else {
        p++;
        if( p[ strspn( p, "0123456789" ) ] == '\0' ) {
            value = atoi( p );
        } else {
            cout << "Error - invalid number: " << p << endl;
            DieInFlames( );
        }
    }
    return value;
}

static void DoOption( const char * s ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    char * p;

    s++;
    switch( *s ) {
      case 'd':
        p = strchr( s, '=' );
        if( ( p == NULL ) || ( p[ 1 ] == '\0' ) ) {
            cout << "Error - no directory specified." << endl;
            DieInFlames( );
        } else {
            p++;
            strcpy( dest_dir, p );
        };
        break;
      case 'o':
        p = strchr( s, '=' );
        if( ( p == NULL ) || ( p[ 1 ] == '\0' ) ) {
            cout << "Error - no destination file specified." << endl;
            DieInFlames( );
        } else {
            p++;
            MakeFileName( dest_file, p );
        };
        break;
      case 'w':
        if( strcmp( s, "wig" ) == 0 ) {
            DontActuallyDoAnything = 1;
            cout << "Employing the SECRET option, you clever guy" << endl;
        };
        break;
      case 'b' :
        msgs_start = numericOption( s, "beginning message number" );
        break;
      case 'e' :
        msgs_end = numericOption( s, "ending message number" );
        break;
      default:
        cout << "Error - invalid option " << s << endl;
        DieInFlames( );
    };
};

static MsgDscr FilesOk( "OK_" );// description for OK files
static MsgDscr FilesEr( "ER_" );// description for ERROR files

extern void main( int argc, char * argv[ ] ) {
//==========================================

    memset( msg_file, 0, 256 );
    memset( model_file, 0, 256 );
    memset( dest_file, 0, 256 );

    strcpy( dest_dir, "." ) ;

    char step = 0;

    ifstream Infile;
    ofstream Outfile;

    DontActuallyDoAnything = 0;

    if( ( argc <= 2 ) || ( ( argc >= 2 ) && ( *argv[ 1 ] == '?' ) ) ) {
        usage( );
        DiePeacefully( );
    };

    for( int i = 1; i < argc; i++ ) {
        if( ( *argv[ i ] == '\/' ) || ( *argv[ i ] == '-' ) ) {
            DoOption( argv[ i ] );
        } else {
            if( step == 0 ) {
                step = 1;
                MakeFileName( msg_file, argv[ 1 ], default_msg_ext );
            } else if( step == 1 ) {
                step = 2;
                MakeFileName( model_file, argv[ 2 ], default_model_ext );
            } else {
                cout << "Error - too many files specified." << endl;
                DieInFlames( );
            };
        };
    };
    if( step == 0 ) {
        cout << "Error - No message file specified" << endl;
        DieInFlames( );
    } else if( step == 1 ) {
        cout << "Error - No model file specified" << endl;
        DieInFlames( );
    };
    if( *dest_file == '\0' ) {
        strcpy( dest_file, model_file );
        char * q = strrchr( dest_file, '.' );
        if( q == NULL ) {
            strcat( dest_file, default_dest_ext );
        } else {
            strcpy( q, default_dest_ext );
        };
    };

    GMLfilestat gfs = ReadGMLFile( FilesOk, FilesEr );
    ProcessModelFile( FilesOk, FilesEr, gfs );

    DiePeacefully( );
};
