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
* Description:  Master and extended header
*
****************************************************************************/

#include <cstring>
#include "header.hpp"
#include "errors.hpp"

IpfHeader::IpfHeader()
{
    std::memset( this, 0, sizeof( IpfHeader ) );
    id[ 0 ] = 'H';
    id[ 1 ] = 'S';
    id[ 2 ] = 'P';
    flags = 0x10;
    size = sizeof( IpfHeader );
    version_hi = 2;
    version_lo = 2;
    maxLocalIndex = 245;
};
/*****************************************************************************/
void IpfHeader::write( std::FILE *out ) const
{
    std::fseek( out, 0, SEEK_SET ); //exception: this element has a fixed position
    if( std::fwrite( this, sizeof( IpfHeader ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
}
/*****************************************************************************/
STD1::uint32_t IpfExtHeader::write( std::FILE *out ) const
{
    STD1::uint32_t start( std::ftell( out ) );
    if( std::fwrite( this, sizeof( IpfExtHeader ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    return start;
}

