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

#include "msgstrm.h"
#include "files.h"
#include "inout.h"
#include "fini.h"
#include "readgml.h"

int DontActuallyDoAnything;

const char ErrMsg1[]       = ":MSGSYM";
const char ErrMsg2[]       = ":MSGTXT";
const char Comment[]       = "\/\/ ";
const char GoodCodeStart[] = ":errgood";
const char GoodCodeEnd[]   = ":eerrgood";
const char BadCodeStart[]  = ":errbad";
const char BadCodeEnd[]    = ":eerrbad";
const char ProgExt[]       = ".CPP";

unsigned msgs_start = 0;    // may be reset by /b switch
unsigned msgs_end = 5000;   // may be reset by /e switch
static int msg_ctr = 0;     // current message #

#define textis( buffer, text ) 0 == strncmp( buffer, text, sizeof(text)-1 )


static int processingMsg(       // TEST IF PROCESSING THIS MESSAGE
    void )
{
    return ( ! DontActuallyDoAnything )
        && ( msg_ctr >= msgs_start )
        && ( msg_ctr <= msgs_end );
}


void MakeTestFileName(          // MAKE NEW FILE NAME
    char* filename,             // - file name
    MsgDscr& file_des,          // - files descriptor
    int msg_ctr,                // - message no.
    int file_ctr )              // - file no.
{
    strcpy( filename, dest_dir );
    strcat( filename, "\\" );
    strcat( filename, file_des.GetFileName( msg_ctr, file_ctr ) );
    strcat( filename, ProgExt );
}


static char MakeNewOutFile(     // MAKE NEW TEST/ERROR FILE
    ofstream& of,               // - output stream
    MsgDscr& file_des,          // - files descriptor
    int msg_ctr )               // - message no.
{
    char filename[ _MAX_PATH ];

    MakeTestFileName( filename
                    , file_des
                    , msg_ctr
                    , file_des.AddFile( msg_ctr ) );
    if( WOpenForWrite( of, filename ) ) {
        //error
        cout << "Error - could not create \"" << filename << "\"." << endl;
        DieInFlames( );
        return( 1 );
    } else {
        return( 0 );
    }
}


static void emitTestStart(      // START EMISSION OF TEST FILE
    ofstream& of,               // - stream for file
    strstream& ot,              // - stream for text
    MsgDscr& file_des,          // - files descriptor
    int msg_ctr )               // - message counter
{
    if( processingMsg() ) {
        MakeNewOutFile( of, file_des, msg_ctr );
        ot << ends;
        Write( of, ot );
        of << endl;
    }
}

static void emitTestEnd(        // COMPLETE EMISSION OF TEST FILE
    ofstream& of )              // - stream for file
{
    if( processingMsg() ) {
        of << endl << Comment << "---------------------------------" << endl;
        WClose( of );
    }
}

enum GML_STATE                  // states while reading file
//                              //     meaning
{   ST_READING                  // - just reading
,   ST_MSGSYM                   // - :MSGSYM read
,   ST_MSGTXT                   // - :MSGTXT read
,   ST_ERRGOOD                  // - :ERRGOOD read
,   ST_ERRBAD                   // - :ERRBAD read
,   MAX_STATE                   // maximum # states
};

enum GML_READ                   // inputs
{   RD_TEXT                     // - text
,   RD_MSGSYM                   // - :MSGSYM
,   RD_MSGTXT                   // - :MSGTXT
,   RD_ERRGOOD                  // - :ERRGOOD
,   RD_EERRGOOD                 // - :eERRGOOD
,   RD_ERRBAD                   // - :ERRBAD
,   RD_EERRBAD                  // - :eERRBAD
};

enum GML_TRANS                  // transitions
{   NEW_MSGSYM                  // - get :MSGSYM text (1-st time)
,   OLD_MSGSYM                  // - get :MSGSYM text (other times)
,   GET_MSGTXT                  // - get :MSGTXT text
,   BEG_ERRGOOD                 // - start ERRGOOD file
,   BEG_ERRBAD                  // - start ERRBAD file
,   BAD_FOLLOW                  // - bad transition
,   SET_READING                 // - set READING state
,   SET_MSGTXT                  // - set MSGTXT state
,   WR_FILE                     // - write to file
,   END_FILE                    // - complete write to file
,   IGNORE                      // - ignore text
};

static GML_TRANS trans[][ MAX_STATE ] = // transitions
//\ from
// \
//to\READING     MSGSYM      MSGTXT      ERRGOOD     ERRBAD
{{   SET_READING,BAD_FOLLOW, SET_MSGTXT, WR_FILE,    WR_FILE    }//RD_TEXT
,{   NEW_MSGSYM, BAD_FOLLOW, OLD_MSGSYM, BAD_FOLLOW, BAD_FOLLOW }//RD_MSGSYM
,{   BAD_FOLLOW, GET_MSGTXT, BAD_FOLLOW, BAD_FOLLOW, BAD_FOLLOW }//RD_MSGTXT
,{   BAD_FOLLOW, BAD_FOLLOW, BEG_ERRGOOD,BAD_FOLLOW, BAD_FOLLOW }//RD_ERRGOOD
,{   BAD_FOLLOW, BAD_FOLLOW, BAD_FOLLOW, END_FILE,   BAD_FOLLOW }//RD_EERRGOOD
,{   BAD_FOLLOW, BAD_FOLLOW, BEG_ERRBAD, BAD_FOLLOW, BAD_FOLLOW }//RD_ERRBAD
,{   BAD_FOLLOW, BAD_FOLLOW, BAD_FOLLOW, BAD_FOLLOW, END_FILE   }//RD_EERRBAD
};


static char *gmlState(          // GET NAME OF STATE
    GML_STATE state )           // - state
{
    static char *name[] =       // - names for states
    {   "START"                 // - - just reading
    ,   "MSGSYM"                // - - :MSGSYM read
    ,   "MSGTXT"                // - - :MSGTXT read
    ,   "ERRGOOD"               // - - :ERRGOOD read
    ,   "ERRBAD"                // - - :ERRBAD read
    };

    return name[ state ];
}


static char *gmlTrans(          // GET NAME OF TRANSITION
    GML_READ trans )            // - transition
{
    static char *name[] =       // - names for transitions
    {   "TEXT"                  // - text
    ,   "MSGSYM"                // - :MSGSYM
    ,   "MSGTXT"                // - :MSGTXT
    ,   "ERRGOOD"               // - :ERRGOOD
    ,   "eERRGOOD"              // - :eERRGOOD
    ,   "ERRBAD"                // - :ERRBAD
    ,   "eERRBAD"               // - :eERRBAD
    };

    return name[ trans ];
}

GMLfilestat ReadGMLFile( MsgDscr& file_ok, MsgDscr& file_er ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    ofstream        outfile;
    strstream       out_text;

    int             current_good_file = 0;
    int             current_bad_file = 0;
    char            buffer[ 256 ];
    int             counter = 0;
    int             line_count = 0;

    GML_STATE       state = ST_READING;
    GML_READ        gml_input;

    if( WOpenForRead( MsgFile, msg_file ) ) {
        cout << "Error - could not open \"" << msg_file << "\"." << endl;
        DieInFlames( );
    };

    cout << "Reading message file";

    for( ;; ) {

        MsgFile.getline( buffer, 255 );
        if( MsgFile.eof() ) {
            cout << endl;
            break;
        };
        if( MsgFile.fail() ) {
            cout << endl
                 << "I/O error reading input file " << endl;
            DieInFlames( );
        };
        if( counter == 0 ) {
            counter = 1001;
            cout << '.' << flush;
        };
        --counter;
        ++line_count;

        if( *buffer == ':' ) {
            if( textis( buffer, ErrMsg1 ) ) {
                gml_input = RD_MSGSYM;
            } else if( textis( buffer, ErrMsg2 ) ) {
                gml_input = RD_MSGTXT;
            } else if( textis( buffer, GoodCodeStart ) ) {
                gml_input = RD_ERRGOOD;
            } else if( textis( buffer, GoodCodeEnd ) ) {
                gml_input = RD_EERRGOOD;
            } else if( textis( buffer, BadCodeStart ) ) {
                gml_input = RD_ERRBAD;
            } else if( textis( buffer, BadCodeEnd ) ) {
                gml_input = RD_EERRBAD;
            } else {
                gml_input = RD_TEXT;
            }
        } else {
            gml_input = RD_TEXT;
        }
        switch( trans[ gml_input ][ state ] ) {
          case OLD_MSGSYM :
            Purge( out_text );
          case NEW_MSGSYM :
            ++msg_ctr;
            state = ST_MSGSYM;
            out_text << Comment << buffer;
            break;
          case GET_MSGTXT :
            state = ST_MSGTXT;
            out_text << Comment << buffer;
            break;
          case BEG_ERRGOOD :
            emitTestStart( outfile, out_text, file_ok, msg_ctr );
            current_good_file++;
            state = ST_ERRGOOD;
            break;
          case BEG_ERRBAD :
            emitTestStart( outfile, out_text, file_er, msg_ctr );
            current_bad_file++;
            state = ST_ERRBAD;
            break;
          case WR_FILE :
            if( processingMsg() ) {
                outfile << buffer;
            }
            break;
          case END_FILE :
            emitTestEnd( outfile );
          case SET_MSGTXT :
            state = ST_MSGTXT;
            break;
          case SET_READING :
            state = ST_READING;
            break;
          case BAD_FOLLOW :
            cout << endl
                 << "GML file at line "
                 << line_count
                 << ":"
                 << "state "
                 << gmlState( state )
                 << " followed by "
                 << gmlTrans( gml_input )
                 << endl;
            DieInFlames( );
        }
    };
    return GMLfilestat( current_good_file - 1, current_bad_file - 1 );
};
