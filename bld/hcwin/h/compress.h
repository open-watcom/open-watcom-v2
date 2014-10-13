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


//
//  compress.h  --Classes to support WinHelp-style LZ77 compression.
//
//
#include <watcom.h> // for uint_8, etc.
#include "myfile.h"
#include "hcmem.h"


//
//  CompWriter  --Base class to accept characters & codes and
//        do something with them.
//

class CompWriter
{
    // Assignment of CompWriter's is not permitted.
    CompWriter( CompWriter const & ) {};
    CompWriter &    operator=( CompWriter const & ) { return *this; };

protected:
    uint_8  *_buffer;
    size_t  _numBytes;
    int     _numTokens;
    uint_8  _bitMask;
public:
    CompWriter();
    virtual ~CompWriter();

    virtual void    dump();
    unsigned    putChr( uint_8 c );
    unsigned    putCode( unsigned distance, unsigned length );
};


//
//  CompOutFile --Version of CompWriter to write to a file.
//

class CompOutFile : public CompWriter
{
    OutFile * _dest;
public:
    CompOutFile( OutFile * d_file ) : _dest(d_file) {};

    void    dump();
};


//
//  CompReader  --Class to compress text.
//

class CompReader
{
    Buffer<uint_8>  _buffer;    // Stores the text being compressed.

    Buffer<short>   _indices;   // Used to find patterns within
    Buffer<short>   _htable;    // the text.

    CompWriter  *_dest;
    short   _last, _first, _current;

    void    shuffle();  // Free space in the buffer.
public:
    CompReader( CompWriter *riter );

    unsigned compress( char const source[], size_t amount );

    // Add text to the compressed stream in a 'stupid' manner;
    // i.e. it pretends it's uncompressible.
    unsigned add( char const source[], size_t amount );
    unsigned skip( size_t amount );

    void    flush( bool nodump = false );

    void    reset( CompWriter *riter, bool nodump = false );
};
