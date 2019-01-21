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
* Description:  A page of panel data
*
****************************************************************************/

#ifndef PAGE_INCLUDED
#define PAGE_INCLUDED

#include <cstdio>
#include <string>
#include <vector>
#include "element.hpp"
#include "gdword.hpp"
#include "toc.hpp"

class Document; //forward reference
class Cell;

class Page {
public:
    Page( Document* d, Element* head ) : _document( d ), _elements( 1, head ),
        _index( 0 ), _searchable( true ) { };
    ~Page() { };
    void addElement( Element* e ) { _elements.push_back( e ); };
    //add a word to the local dictionary
    bool addTextToLD( GlobalDictionaryWord* gdentry );
    //the page title
    void setTitle( std::wstring& t ) { _title = t; };
    //copy data from the Hn or Fn tag
    void setChildren( std::vector< word >& c ) { _children = c; }
    void setTOC( TocEntry& t ) { _toc = t; }
    void setETOC( ExtTocEntry& e ) { _etoc = e; };
    void setOrigin( PageOrigin& o ) { _origin = o; };
    void setSize( PageSize& s ) { _size = s; };
    void setStyle( PageStyle& s ) { _style = s; };
    void setGroup( PageGroup& g ) { _group = g; };
    void SetControl( PageControl& c ) { _control = c; };
    void setIndex( word i ) { _index = i; };
    void setSearchable( bool s ) { _searchable = s; }
    //page appears in TOC
    bool isVisible() const { return !_toc.flags.s.hidden; };
    //index of page in TOC collection
    word index() const { return _index; };
    void buildTOC();
    void linearize() { ( *( _elements.begin() ))->linearize( this ); };
    void buildIndex() { ( *( _elements.begin() ))->buildIndex(); };
    void buildLocalDictionary( OutFile* out );
    //write a TOC entry
    dword write( OutFile* out );
    dword tocSize() const { return _toc.hdrsize; };
    //write child windows list
    dword writeChildren( OutFile* out ) const;

    void setOutFile( OutFile* out ) { _out = out; };
    OutFile* out() { return _out; };
private:
    Page( const Page& rhs );            //no copy
    Page& operator=( const Page& rhs ); //no assignment

    Document*               _document;
    Cell*                   _currentCell;   //the cell currently in use
    std::vector< Element* > _elements;      //all elements on this page
    typedef std::vector< Element* >::iterator ElementIter;
    typedef std::vector< Element* >::const_iterator ConstElementIter;
    std::vector< word >     _cells;
    typedef std::vector< word >::iterator CellIter;
    typedef std::vector< word >::const_iterator ConstCellIter;
    std::wstring _title;                    //page title
    std::vector< word >     _children;
    typedef std::vector< word >::iterator ChildIter;
    typedef std::vector< word >::const_iterator ConstChildxIter;
    TocEntry                _toc;
    ExtTocEntry             _etoc;
    PageOrigin              _origin;
    PageSize                _size;
    PageStyle               _style;
    PageGroup               _group;
    PageControl             _control;
    word                    _index;         // index in TOC
    bool                    _searchable;
    OutFile*                _out;
};
#endif
