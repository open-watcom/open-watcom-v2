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


// System includes --------------------------------------------------------

#include <stdio.h>
#include <string.h>

// Project includes -------------------------------------------------------

#include "drmisc.h"
#include "fileinfo.h"
#include "errors.h"

FileInfo::FileInfo( char * filename, uint_32 dirIndex,
                            uint_32 lastModified, uint_32 length,
                            uint_32 newDirIndex )
    : DebugObject( filename )
    , _oldDirIndex( dirIndex )
    , _lastModified( lastModified )
    , _length( length )
    , _newDirIndex( newDirIndex )
/***********************************************************************/
{
}

void FileInfo::set( char * filename, uint_32 oldDirIndex,
                         uint_32 lastModified, uint_32 length,
                         uint_32 newDirIndex )
/************************************************************/
{
    setName( filename );
    _oldDirIndex = oldDirIndex;
    _lastModified = lastModified;
    _length = length;
    _newDirIndex = newDirIndex;
}

void FileInfo::encode( MemoryStream & mem )
/*****************************************/
{

    mem.write( _name, strlen( _name ) + 1 );
    mem.writeULEB128( _newDirIndex );
    mem.writeULEB128( _lastModified );
    mem.writeULEB128( _length );

#if 0
    uint_32  infoLength;
    uint_32  lengthLength;
    uint_8 * ulebData;
    uint_8   ulebDataLen[ MAX_ULEB_SIZE ];
    uint_8 * pos;

    ulebData = new uint_8[ 3 * MAX_ULEB_SIZE + strlen( name() ) + 1 ];
    if( ulebData == NULL ) {
        throw ErrOutOfMemory;
    }

    strcpy( ( char * )ulebData, ( char * )name() );
    pos = ulebData + strlen( name() ) + 1;
    pos = EncodeULEB128( pos, _newDirIndex );
    pos = EncodeULEB128( pos, _lastModified );
    pos = EncodeULEB128( pos, _length );

    infoLength = pos - ulebData;

    pos = EncodeULEB128( ulebDataLen, infoLength );
    lengthLength = pos - ulebDataLen;

    mem.write( ulebDataLen, lengthLength );
    mem.write( ulebData, infoLength );

    delete [] ulebData;
#endif
}
