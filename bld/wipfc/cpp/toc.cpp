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
#include "cell.hpp"
#include "errors.hpp"
#include "outfile.hpp"


void TocEntry::buildText( Cell* cell ) const
/******************************************/
{
    cell->add( hdrsize );
    cell->add( flags.data );
    cell->add( cellCount );
}

dword TocEntry::write( OutFile* out ) const
/*****************************************/
{
    dword offset( out->tell() );
    if( out->put( hdrsize ) )
        throw FatalError( ERR_WRITE );
    if( out->put( flags.data ) )
        throw FatalError( ERR_WRITE );
    if( out->put( cellCount ) )
        throw FatalError( ERR_WRITE );
    return offset;
}

void ExtTocEntry::buildText( Cell* cell ) const
/*********************************************/
{
    cell->add( flags.data );
}

void ExtTocEntry::write( OutFile* out ) const
/*******************************************/
{
    if( out->put( flags.data ) ) {
        throw FatalError( ERR_WRITE );
    }
}

void PageOrigin::buildText( Cell* cell ) const
/********************************************/
{
    cell->add( static_cast< byte >( ( (xPosType & 0x0f) << 4 ) | (yPosType & 0x0f) ) );
    cell->add( xpos );
    cell->add( ypos );
}

void PageOrigin::write( OutFile* out ) const
/******************************************/
{
    if( out->put( static_cast< byte >( ( (xPosType & 0x0f) << 4 ) | (yPosType & 0x0f) ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( xpos ) )
        throw FatalError( ERR_WRITE );
    if( out->put( ypos ) ) {
        throw FatalError( ERR_WRITE );
    }
}

void PageSize::buildText( Cell* cell ) const
/******************************************/
{
    cell->add( static_cast< byte >( ( (heightType & 0x0f) << 4 ) | (widthType & 0x0f) ) );
    cell->add( width );
    cell->add( height );
}

void PageSize::write( OutFile* out ) const
/****************************************/
{
    if( out->put( static_cast< byte >( ( (heightType & 0x0f) << 4 ) | (widthType & 0x0f) ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( width ) )
        throw FatalError( ERR_WRITE );
    if( out->put( height ) ) {
        throw FatalError( ERR_WRITE );
    }
}

void PageStyle::buildText( Cell* cell ) const
/*******************************************/
{
    cell->add( attrs );
}

void PageStyle::write( OutFile* out ) const
/*****************************************/
{
    if( out->put( attrs ) ) {
        throw FatalError( ERR_WRITE );
    }
}

void PageGroup::buildText( Cell* cell ) const
/*******************************************/
{
    cell->add( id );
}

void PageGroup::write( OutFile* out ) const
/*****************************************/
{
    if( out->put( id ) ) {
        throw FatalError( ERR_WRITE );
    }
}

void PageControl::buildText( Cell* cell ) const
/*********************************************/
{
    cell->add( refid );
}

void PageControl::write( OutFile* out ) const
/*******************************************/
{
    if( out->put( refid ) ) {
        throw FatalError( ERR_WRITE );
    }
}
