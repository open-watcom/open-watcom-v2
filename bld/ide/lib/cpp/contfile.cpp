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


#include "contfile.hpp"

ContFile::ContFile( char contChar, int maxLine )
    : _count( 0 )
    , _maxLine( maxLine )
    , _contChar( contChar )
{
}

char ContFile::getByte()
{
    return( WFile::getByte() );
}

bool ContFile::putByte( char ch )
{
    if( ch == '\n' ) {
        _count = 0;
    } else if( _count++ > _maxLine ) {
        _count = 1;
        if( !WFile::putByte( _contChar ) ) {
            return( false );
        }
        if( !WFile::putByte( '\n' ) ) {
            return( false );
        }
    }
    return( WFile::putByte( ch ) );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

ContFile::~ContFile()
{
}

