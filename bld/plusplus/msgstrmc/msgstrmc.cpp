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
#include <fstream.h>

#include "truefals.h"
#include "excfile.h"
#include "lineout.h"
#include "iterfrec.h"
#include "conshelp.h"

static ExcPgm pgm_exception( "MSGSTRMC", 1 );

class IterOverErrFile           // ITERATE OVER AN ERROR FILE
    : public IterOverFileRec
{
public:
    IterOverErrFile(            // CONSTRUCTOR
        const char* filename,   // - file name
        LineOut *out_info );    // - info. for output file

    void process(               // PROCESS AN OUTPUT LINE
        void *line,             // - output line
        void *ctl );            // - control information
};


IterOverErrFile::IterOverErrFile( // CONSTRUCTOR
    const char* filename,       // - file name
    LineOut *out_info )         // - info. for output file
{
    ifstream in;                // - input stream
    Boolean opened;             // - TRUE ==> opened ok
    IterFileRec iterator( filename, in, opened, FALSE ); // - iterator

    if( opened ) {
        cout << "processing: " << filename << endl;
        iterateOverElements( iterator, out_info );
    } else {
        out_info->write( "*** no errors ***" );
    }
}


void IterOverErrFile::process(  // PROCESS AN OUTPUT LINE
    void *buffer,               // - output line
    void *ctl )                 // - control information
{
    char* rec = (char*)buffer;  // - record
    LineOut& out = *(LineOut*)ctl; // - output control

    out.write( rec );
}



class IterOverSrcFile           // ITERATE OVER A SOURCE FILE
    : public IterOverFileRec
{
    unsigned line_no;           // current line no.
public:
    IterOverSrcFile(            // CONSTRUCTOR
        const char* filename,   // - file name
        LineOut *out_info );    // - info. for output file

    void process(               // PROCESS AN OUTPUT LINE
        void *line,             // - output line
        void *ctl );            // - control information
};


IterOverSrcFile::IterOverSrcFile( // CONSTRUCTOR
    const char* filename,       // - file name
    LineOut *out_info )         // - info. for output file
    : line_no( 0 )
{
    ifstream in;                // - input stream
    Boolean opened;             // - TRUE ==> opened ok

    cout << "processing: " << filename << endl;
    out_info->write_strings( "=========== "
                           , filename
                           , " ==========="
                           , 0 );
    iterateOverElements( IterFileRec( filename, in, opened, TRUE )
                       , out_info );
}


void IterOverSrcFile::process(  // PROCESS AN OUTPUT LINE
    void *buffer,               // - output line
    void *ctl )                 // - control information
{
    char line[8];               // - line number (character)
    char* rec = line;           // - record
    LineOut& out = *(LineOut*)ctl; // - output control

    ++line_no;
    line[0] = ' ';
    line[1] = ' ';
    itoa( line_no, line + 2, 10 );
    out.write_strings( "["
                     , line + strlen( line ) - 3
                     , "] "
                     , buffer
                     , NULL );
}


class IterOverInpFile           // ITERATE OVER INPUT FILE
    : public IterOverFileRec
{
    unsigned line_no;           // current line no.
public:
    IterOverInpFile(            // CONSTRUCTOR
        const char* filename,   // - file name
        LineOut *out_info );    // - info. for output file

    void process(               // PROCESS AN OUTPUT LINE
        void *line,             // - output line
        void *ctl );            // - control information
};


IterOverInpFile::IterOverInpFile( // CONSTRUCTOR
    const char* filename,       // - file name
    LineOut *out_info )         // - info. for output file
    : line_no( 0 )
{
    ifstream in;                // - input stream
    Boolean opened;             // - TRUE ==> opened ok

    iterateOverElements( IterFileRec( filename, in, opened, TRUE )
                       , out_info );
}


void IterOverInpFile::process(  // PROCESS AN OUTPUT LINE
    void *buffer,               // - output line
    void *ctl )                 // - control information
{
    char* filename = (char*)buffer; // - file name
    LineOut& out = *(LineOut*)ctl;  // - output control
    {   IterOverSrcFile( filename, &out );
    }
    {
        char drive[ _MAX_DRIVE ];   // - drive
        char dir  [ _MAX_DIR   ];   // - directory
        char fname[ _MAX_FNAME ];   // - name
        char extn [ _MAX_EXT   ];   // - extension
        _splitpath( filename, drive, dir, fname, extn );
        _makepath( filename, drive, dir, fname, ".err" );
    }
    {   IterOverErrFile( filename, &out );
    }
}


static void helpText(           // DISPLAY HELP TEXT
    void )
{
    static const char *help_text[] =
    {   "MSGSTRMC -- concatenate test and error files"
    ,   ""
    ,   "usage: msgstrmc in out"
    ,   "  where \"in\" is file of test names"
    ,   "        \"out\" is target file"
    ,   0
    };

    auto ConsoleHelp display( help_text );
}


int main(                       // MAIN-LINE
    int argc,                   // - # args
    char *args[] )              // - arguments
{
    int retn;                   // - return code

    if( ( argc != 3 )
      ||( *args[1] == '?' )
      ||( *args[2] == '?' ) ) {
        helpText();
        retn = 0;
    } else {
        try {
            IterOverInpFile( args[1]
                           , &LineOut( args[2] ) );
            retn = 0;
        } catch( ExcFile& err ) {
            retn = pgm_exception.diagnose_error( err );
        }
    }
    return retn;
}
