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


#include "wpch.hpp"
#include "brmfile.hpp"


// BrmFile Implementation

static const int BrmFile::_bufMax = 0x1000;

BrmFile::BrmFile()
{
    _file = NULL;
    _buf = new char[_bufMax];
}


BrmFile::~BrmFile()
{
    if( _file ){
        fclose(_file);
    }
    delete[] _buf;
}


void BrmFile::Open( char const filename[] )
{
    _file = fopen( filename, "rb" );
    if( _file ){
        setvbuf( _file, _buf, _IOFBF, _bufMax );
        fseek( _file, 0, SEEK_END );
        _fileSize = ftell( _file );
        fseek( _file, 0, SEEK_SET );
        _start = 0;
    }
}


void BrmFile::Close()
{
    fclose( _file );
}


// BrmFilePch Implementation

#define _PCH_HEADER_ONLY
#include "pcheader.h"

BrmFilePch::BrmFilePch()
{
}

BrmFilePch::~BrmFilePch()
{
}


void BrmFilePch::Open( char const filename[] )
{
    BrmFile::Open( filename );
    if( _file ) {
        auto precompiled_header_header hdr;
        fread( &hdr, sizeof( hdr ), 1, _file );
        fseek( _file, hdr.browse_info, SEEK_SET );
        _start = ftell( _file );
        _fileSize -= _start;
    }
}
