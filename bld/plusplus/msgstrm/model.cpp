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

#include "msgstrm.h"
#include "files.h"
#include "inout.h"
#include "readgml.h"
#include "fini.h"

const char subchar = '%';
const char subgoodchar = 'G';
const char subbadchar = 'B';

static void TooManyTypeError( ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    cout << "Error - Cannot have both " << subchar << subgoodchar <<
    " and " << subchar << subbadchar << " on the same line." << endl;
    DieInFlames( );
};

void ProcessModelFile(          // PROCESS MODEL FILE
    MsgDscr& file_ok,           // - description of good files
    MsgDscr& file_er,           // - description of bad files
    GMLfilestat g )             // - statistics
{
    char filename[ _MAX_PATH ];         // - test file name
    char            buffer[ 256 ];
    char *          buf = buffer;
    int             counter = 0;
    bool            dotflag = FALSE;
    strstream       out_text;

    char *          sub;
    int             idx, jdx, kdx;

    int             tot;
    GoodBad         gb;                 // - indicates type of test file name

    cout << "Processing model file..." << endl;

    if( WOpenForRead( ModelFile, model_file ) ) {
        cout << "Error - could not open \"" << model_file << "\"." << endl;
        DieInFlames( );
    };
    if( WOpenForWrite( DestFile, dest_file ) ) {
        cout << "Error - could not open \"" << dest_file << "\"." << endl;
        DieInFlames( );
    };

    memset( buffer, 0, 256 );

    for( ;; ) {

//      for( idx = 0; idx < 20; idx ++ ) {
//          sublocs[ idx ] = 0;
//      };

        ModelFile.getline( buffer, 252 );
        buf = buffer;
        counter++;
        if( counter == 1000 ) {
            counter = 0;
            dotflag = TRUE;
            cout << '.' << flush;
        };

        if( ( ModelFile.eof( ) ) || ( ModelFile.fail( ) ) ) {
            break;
        };
        if( strchr( buffer, '\n' ) == NULL ) {
            strcat( buffer, "\n" );
        };

        if( ( sub = strchr( buffer, subchar ) ) != NULL ) {
            if( sub[ 1 ] == subgoodchar ) {
                if( g.last_good == 0 ) {
                    cout << "Warning: Model file, line " << counter << ":" << endl;
                    cout << "There are no GOOD.CPP files to process" << endl;
                    continue;
                };
                gb = GOOD;
            } else if( sub[ 1 ] == subbadchar ) {
                if( g.last_bad == 0 ) {
                    cout << "Warning: Model file, line " << counter << ":" << endl;
                    cout << "There are no BAD.CPP files to process" << endl;
                    continue;
                };
                gb = BAD;
            } else {
                DestFile << buffer;
                continue;
            };
            kdx = strlen( buffer );
            for( idx = 0; idx <= kdx; idx++ ) {
                if( buffer[ idx ] == subchar ) {
                    if( ( ( buffer[ idx + 1 ] == subgoodchar )
                        &&( gb == GOOD ) )
                      ||( ( buffer[ idx + 1 ] == subbadchar )
                        &&( gb == BAD ) )
                      ) {
                        buffer[ idx ] = 0;
                    } else if( ( buffer[ idx + 1 ] == subgoodchar )
                             ||( buffer[ idx + 1 ] == subbadchar  ) ) {
                        TooManyTypeError( );
                    };
                };
            }; // Substitution strings done.
            buffer[ idx ] = 'E';
            buffer[ idx + 1 ] = '\0';
            {
                MsgDscr& descr = (gb == GOOD ) ? file_ok : file_er;
                tot = descr.GetMaxMsgNo();
                for( jdx = 1; jdx < tot; jdx++ ) {
                    for( int fdx = 0; ; ) {
                        fdx = descr.GetNextFile( jdx, fdx );
                        if( fdx == 0 ) break;
                        MakeTestFileName( filename, descr, jdx, fdx );
                        kdx = 0;
                        sub = buffer;
                        for( ;; ) {
                            out_text << sub;
                            kdx = strlen( sub );
                            if( sub[ kdx + 1 ] == 'E' ) {
                                break;
                            } else {
                                out_text << filename;
                            };
                            sub = &sub[ kdx + 2 ];
                        }
                    }
                    out_text << ends;
                    Write( DestFile, out_text );
                    Purge( out_text );
                }
            }
        } else {
            DestFile << buffer;
        };
    };
    if( dotflag ) {
        cout << endl;
    };
};
