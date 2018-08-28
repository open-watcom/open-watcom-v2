/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Master and extended header
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstring>
#include "header.hpp"
#include "errors.hpp"
#include "outfile.hpp"


IpfHeader::IpfHeader()
/********************/
{
    std::memset( this, 0, sizeof( IpfHeader ) );
    id[0] = 'H';
    id[1] = 'S';
    id[2] = 'P';
    flags = 0x10;
    hdrsize = static_cast< word >( size() );
    version_hi = 2;
    version_lo = 2;
    maxLocalIndex = 245;
};

void IpfHeader::write( OutFile* out ) const
/*****************************************/
{
    out->seek( 0, SEEK_SET ); //exception: this element has a fixed position

    if( out->write( id, 3, 1 ) )
        throw FatalError( ERR_WRITE );
    if( out->put( flags ) )
        throw FatalError( ERR_WRITE );
    if( out->put( hdrsize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( version_hi ) )
        throw FatalError( ERR_WRITE );
    if( out->put( version_lo ) )
        throw FatalError( ERR_WRITE );
    if( out->put( tocCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( tocOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( tocSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( tocOffsetOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( panelCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( panelOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( nameCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( nameOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( indexCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( indexOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( indexSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( icmdCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( icmdOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( icmdSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( searchOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( searchSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( cellCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( cellOffsetOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dictSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dictCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dictOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( imageOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( maxLocalIndex ) )
        throw FatalError( ERR_WRITE );
    if( out->put( nlsOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( nlsSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( extOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->write( reserved, 12, 1 ) )
        throw FatalError( ERR_WRITE );
    if( out->write( title, TITLE_SIZE, 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}

bool IpfHeader::isBigFTS()
/************************/
{
    return( (searchOffset & (1L << 31)) != 0 );
}

void IpfHeader::setBigFTS( bool big )
/***********************************/
{
    searchOffset |= static_cast< dword >( big ) << 31;
}

dword IpfExtHeader::write( OutFile* out ) const
/*********************************************/
{
    dword start( out->tell() );

    if( out->put( fontCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( fontOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dbCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dbOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( dbSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( gNameCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( gNameOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( stringsOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( stringsSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( childPagesOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( childPagesSize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( gIndexCount ) )
        throw FatalError( ERR_WRITE );
    if( out->put( ctrlOffset ) )
        throw FatalError( ERR_WRITE );
    if( out->put( ctrlSize ) )
        throw FatalError( ERR_WRITE );
    if( out->write( reserved, 4, 4 ) )
        throw FatalError( ERR_WRITE );
    return start;
}
