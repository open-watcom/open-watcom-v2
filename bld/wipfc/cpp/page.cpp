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
* Description:  A page or panel data
*
****************************************************************************/


#include "wipfc.hpp"
#include "page.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "hn.hpp"
#include "outfile.hpp"


void Page::buildTOC()
{
    ( *( _elements.begin() ))->buildTOC( this );
}
/***************************************************************************/
void Page::buildLocalDictionary( OutFile* out )
{
    _out = out;
    bool autoSpacing( true );
    _currentCell = new Cell( _document->maxLocalDictionarySize() );
    _document->addCell( _currentCell );
    _cells.push_back( static_cast< word >( _currentCell->index() ) );
    for( ElementIter itr = _elements.begin(); itr != _elements.end(); ++itr ) {
        std::pair< bool, bool > flags( ( *itr )->buildLocalDict( this ) );
        if( flags.first ) {
            _currentCell = new Cell( _document->maxLocalDictionarySize() );
            _document->addCell( _currentCell );
            _cells.push_back( static_cast< word >( _currentCell->index() ) );
            if( !autoSpacing )          //autoSpacing can't cross a cell boundry
                _currentCell->addByte( Cell::TOGGLE_SPACING );  //so turn it off so we can turn
            flags = ( *itr )->buildLocalDict( this );           //it back on later
        }
        if( flags.second )
            autoSpacing = !autoSpacing;
        _currentCell->addElement( *itr );
    }
    _toc.cellCount = static_cast< byte >( _cells.size() );
}
/***************************************************************************/
bool Page::addTextToLD( GlobalDictionaryWord* gdentry )
{
    if( gdentry ) {    //can be 0 for unrecognized entity references
        if( _currentCell->dictFull() )
            return( true );
        _currentCell->addTextToLD( gdentry->index() );
        gdentry->onPage( _index );
    }
    return( false );
}
/***************************************************************************/
//Write a TOC entry
dword Page::write( OutFile* out )
{
    // calculate toc size
    std::size_t tocsize = _toc.size();
    if( _toc.flags.s.extended ) {
        tocsize += _etoc.size();
        if( _etoc.flags.s.setPos )
            tocsize += _origin.size();
        if( _etoc.flags.s.setSize )
            tocsize += _size.size();
        if( _etoc.flags.s.setStyle )
            tocsize += _style.size();
        if( _etoc.flags.s.setGroup )
            tocsize += _group.size();
        if( _etoc.flags.s.setCtrl ) {
            tocsize += _control.size();
        }
    }
    // add cells size
    tocsize += _cells.size() * sizeof( word );
    // add title size
    std::string title( out->wtomb_string( _title ) );
    if( tocsize + title.size() > 255 ) {
        Hn* hn( static_cast< Hn* >( *( _elements.begin() ) ) );
        hn->printError( ERR2_TEXTTOOLONG );
        title.erase( 255 - tocsize );  //write only part of title
    }
    tocsize += title.size();
    _toc.hdrsize = static_cast< byte >( tocsize );
    // write all out
    dword offset = _toc.write( out );
    if( _toc.flags.s.extended ) {
        _etoc.write( out );
        if( _etoc.flags.s.setPos )
            _origin.write( out );
        if( _etoc.flags.s.setSize )
            _size.write( out );
        if( _etoc.flags.s.setStyle )
            _style.write( out );
        if( _etoc.flags.s.setGroup )
            _group.write( out );
        if( _etoc.flags.s.setCtrl ) {
            _control.write( out );
        }
    }
    if( out->put( _cells ) )
        throw FatalError( ERR_WRITE );
    if( !title.empty() ) {
        if( out->put( title ) ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return( offset );
}
/***************************************************************************/
// byte size
// word parent_toc_index
// word child_toc_index
dword Page::writeChildren( OutFile* out ) const
{
    byte bytes = 0;
    if( !_children.empty() ) {
        bytes = static_cast< byte >( sizeof( byte ) + sizeof( _index ) + _children.size() * sizeof( word ) );
        if( out->put( bytes ) )
            throw FatalError( ERR_WRITE );
        if( out->put( _index ) )
            throw FatalError( ERR_WRITE );
        if( out->put( _children ) ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return( bytes );
}
