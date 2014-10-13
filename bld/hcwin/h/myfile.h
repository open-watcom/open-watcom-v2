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
//
// MYFILE.H -- Classes to encapsulate (FILE*)'s.
//             For the Watcom .HLP compiler.
//
#ifndef MYFILE_H
//
//  myfile.h    --Classes to encapsulate (FILE*)'s.
//        For the Watcom .HLP compiler.
//
#define MYFILE_H

#include <watcom.h>
#include <stdio.h>
#include "hcmem.h"


//
//  File    --Base class for file-manipulating classes.
//

class File
{
    static const uint_8 _isOpen;
    uint_8      _flags;

    // Assignment of File's is not allowed.
    File( File const & ) {};
    File &  operator=( File const & ) { return *this; };

protected:
    FILE    *_fp;
    bool    _badFile;
    char    *_fullName;     // Path + name
    char    *_shortName;        // Name as passed to the object.

    File( char const filename[], uint_8 type=0x09 );
    File();
    virtual ~File();

public:

    // File access types.
    enum { READ=0x01, WRITE=0x02, TEXT=0x04, BIN=0x08 };

    // Access functions.
    bool    bad() const { return _badFile; };
    char const *name() const { return (char const*) _shortName; };

    bool    open( char const filename[], uint_8 type=READ|BIN );
    bool    open();         // Re-open the previous file.
    void    close();        // Close the current file.

    int reset( long pos = 0, int where = SEEK_SET )
        { return fseek( _fp, pos, where ); };

    long tell()
        { return ftell( _fp ); };
};


//
//  OutFile --A File which only writes output.
//

class OutFile : public File
{
public:
    OutFile( char const filename[] ) : File( filename, WRITE|BIN ) {};

    int write( uint_8 c )
        { return fputc( c, _fp ); };

    size_t write( uint_16 d )
        { return fwrite( &d, sizeof( d ), 1, _fp ); };

    size_t write( uint_32 d )
        { return fwrite( &d, sizeof( d ), 1, _fp ); };

    size_t write( const void *buf, size_t el_size, size_t nelems )
        { return fwrite( buf, el_size, nelems, _fp ); };
};


//
//  InFile  --A very simple class for reading input.
//

class InFile : public File
{
public:
    InFile( char const filename[], bool is_binary=false );
    InFile() : File() {};

    bool open( char const filename[], bool is_binary=false );
    bool open() { return File::open(); };

    int nextch()
        { return fgetc( _fp ); };

    size_t readbuf( void *buf, size_t num, size_t el_size = 1 )
        { return fread( buf, el_size, num, _fp ); };

    int putback( int c )
        { return ungetc( c, _fp ); };
};


// Some more vaguely file-related things.


//
//  Dumpable    --Abstract base class to represent anything
//        which could be dumped to a .HLP file.
//

struct Dumpable
{
    virtual ~Dumpable() {};

    virtual uint_32 size() { return 0; };
    virtual int dump( OutFile * ) { return 1; };
};


//  Hash    --The "WinHelp standard" Hash function.

extern uint_32 Hash( char const *str );


//  Pointers    --A structure for passing around important pointers.

class HFTopic;
class HFSystem;
class HFFont;
class HFContext;
class HFCtxomap;
class HFKwbtree;
class HFTtlbtree;
class HFPhrases;
class HFBitmaps;

struct Pointers
{
    HFTopic *_topFile;
    HFPhrases   *_phrFile;
    HFSystem    *_sysFile;
    HFFont  *_fontFile;
    HFContext   *_hashFile;
    HFCtxomap   *_mapFile;
    HFKwbtree   *_keyFile;
    HFTtlbtree  *_titleFile;
    HFBitmaps   *_bitFiles;
};

#endif
