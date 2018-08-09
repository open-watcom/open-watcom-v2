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
* Description:  Table of Contents data
*
****************************************************************************/


#include "wipfc.hpp"
#include "toc.hpp"
#include "errors.hpp"
#include "outfile.hpp"


TocEntry::dword TocEntry::write( OutFile *out ) const
{
    dword offset( out->tell() );
    if( out->write( this, sizeof( TocEntry ), 1 ) )
        throw FatalError( ERR_WRITE );
    return offset;
}
/***************************************************************************/
void ExtTocEntry::write( OutFile *out ) const
{
    if( out->write( this, sizeof( ExtTocEntry ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void PageOrigin::write( OutFile *out ) const
{
    if( out->write( this, sizeof( PageOrigin ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void PageSize::write( OutFile *out ) const
{
    if( out->write( this, sizeof( PageSize ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void PageStyle::write( OutFile *out ) const
{
    if( out->write( this, sizeof( PageStyle ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void PageGroup::write( OutFile *out ) const
{
    if( out->write( this, sizeof( PageGroup ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void PageControls::write( OutFile *out ) const
{
    if( out->write( this, sizeof( PageControls ), 1 ) ) {
        throw FatalError( ERR_WRITE );
    }
}

