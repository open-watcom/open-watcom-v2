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
* Description:  A page of panel data
*
****************************************************************************/

#ifndef PAGE_INCLUDED
#define PAGE_INCLUDED

#include "config.hpp"
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
    Page( Document* d, Element* head ) : document( d ), elements( 1, head ),
        idx( 0 ), searchable( true ) { };
    ~Page() { };
    void addElement( Element* e ) { elements.push_back( e ); };
    //add a word to the local dictionary
    bool addWord( GlobalDictionaryWord* word );
    //the page title
    void setTitle( std::string& t ) { title = t; };
    //copy data from the Hn or Fn tag
    void setChildren( std::vector< STD1::uint16_t >& c ) { children = c; }
    void setTOC( TocEntry& t ) { toc = t; }
    void setETOC( ExtTocEntry& e ) { etoc = e; };
    void setOrigin( PageOrigin& o ) { origin = o; };
    void setSize( PageSize& s ) { size = s; };
    void setStyle( PageStyle& s ) { style = s; };
    void setGroup( PageGroup& g ) { group = g; };
    void SetControls( PageControls& c ) { controls = c; };
    void setIndex( STD1::uint16_t i ) { idx = i; };
    void setSearchable( bool s ) { searchable = s; }
    //page appears in TOC
    bool isVisible() const { return !toc.hidden; };
    //index of page in TOC collection
    STD1::uint16_t index() const { return idx; };
    void buildTOC();
    void linearize() { ( *( elements.begin() ))->linearize( this ); };
    void buildIndex() { ( *( elements.begin() ))->buildIndex(); };
    void buildLocalDictionary();
    //write a TOC entry
    STD1::uint32_t write( std::FILE* out );
    STD1::uint32_t tocSize() const { return toc.size; };
    //write child windows list
    STD1::uint32_t writeChildren( std::FILE* out ) const;
private:
    Page( const Page& rhs );            //no copy
    Page& operator=( const Page& rhs ); //no assignment
    Document* document;
    Cell* currentCell;                  //the cell currently in use
    std::vector< Element* > elements;   //all elements on this page
    typedef std::vector< Element* >::iterator ElementIter;
    typedef std::vector< Element* >::const_iterator ConstElementIter;
    std::vector< STD1::uint16_t > cells;
    typedef std::vector< STD1::uint16_t >::iterator CellIter;
    typedef std::vector< STD1::uint16_t >::const_iterator ConstCellIter;
    std::string title;                  //page title
    std::vector< STD1::uint16_t > children;
    typedef std::vector< STD1::uint16_t >::iterator ChildIter;
    typedef std::vector< STD1::uint16_t >::const_iterator ConstChildxIter;
    TocEntry toc;
    ExtTocEntry etoc;
    PageOrigin origin;
    PageSize size;
    PageStyle style;
    PageGroup group;
    PageControls controls;
    STD1::uint16_t idx;            //index in TOC
    bool searchable;
};
#endif
