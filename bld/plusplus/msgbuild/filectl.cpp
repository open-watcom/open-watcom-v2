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


// FileCtl -- simple file control
//
// 95/01/30 -- J.W.Welch        -- defined

#include <io.h>
#include <stdlib.h>

#include "Exc.h"
#include "FileCtl.h"


//-----------------------------------------------------------------------
// FileCtl implementation
//-----------------------------------------------------------------------


FileCtl::FileCtl                // CONSTRUCTOR
    ( void )
    : _filename( 0 )
    , _file( 0 )
{
}


FileCtl::~FileCtl               // DESTRUCTOR
    ( void )
{
    close();
}


void FileCtl::close             // CLOSE FILE
    ( void )
{
    if( 0 != _file ) {
        fclose( _file );
    }
}


void FileCtl::io_err            // THROW I/O ERROR
    ( char const *msg )         // - message
{
    throw Exc( "I/O:", msg, filename(), 0 );
}


void FileCtl::make_filename     // MAKE FILE NAME
    ( char const* fname         // - file name
    , char const* def )         // - default suffix
{
    char dev[ _MAX_DRIVE ];     // - drive
    char dir[ _MAX_DIR   ];     // - directory
    char fnm[ _MAX_FNAME ];     // - file name
    char ext[ _MAX_EXT   ];     // - extension
    char pth[ _MAX_PATH  ];     // - full name

    _splitpath( fname, dev, dir, fnm, ext );
    if( ext[0] == '\0' ) {
        _makepath( pth, dev, dir, fnm, def );
        fname = pth;
    }
    _filename = Str( fname );
}


void FileCtl::open              // OPEN THE FILE
    ( char const* fname         // - file name
    , char const* def           // - default suffix
    , char const* mode )        // - open mode
{
    make_filename( fname, def );
    _file = fopen( _filename, mode );
    if( 0 == _file ) {
        io_err( "cannot open" );
    }
}


//-----------------------------------------------------------------------
// FileCtlOutput implementation
//-----------------------------------------------------------------------


void FileCtlOutput::open        // OPEN THE FILE
    ( char const* fnm           // - file name
    , char const* def )         // - default suffix
{
    FileCtl::open( fnm, def, "wt" );
}


void FileCtlOutput::write       // WRITE A LINE
    ( char const * buf )        // - line to be written
{
    fputs( buf, _file );
    fputc( '\n', _file );
}


//-----------------------------------------------------------------------
// FileCtlInput implementation
//-----------------------------------------------------------------------


void FileCtlInput::open         // OPEN THE FILE
    ( char const* fnm           // - file name
    , char const* def )         // - default suffix
{
    FileCtl::open( fnm, def, "rt" );
}


int FileCtlInput::getline       // GET A LINE
    ( char* buf                 // - buffer
    , unsigned size )           // - buffer size
{
    int retn;
    if( 0 == fgets( buf, size, _file ) ) {
        if( 1 == eof( fileno( _file ) ) ) {
            retn = 0;
        } else {
            io_err( "error reading input file" );
        }
    } else {
        for( ; ; ++buf ) {
            if( *buf == '\0' ) break;
            if( *buf == '\n' ) {
                *buf = '\0';
                break;
            }
        }
        retn = 1;
    }
    return retn;
}
