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
* Description:  Process table/etable tags
*
*   :table / :etable
*       cols='' (space separated list of widths in characters)
*       rules=both|horiz|vert|none (default: both)
*       frame=rules|box|none (default: box)
*   Supplying more column data than columns is an error
*   Text is formatted at compile time
*
****************************************************************************/

#ifndef TABLE_DEFINED
#define TABLE_DEFINED

#include <vector>
#include "tag.hpp"

class Table : public Tag {
public:
    enum Rules {
        NO_RULES,
        HORIZONTAL,
        VERTICAL,
        BOTH
    };
    enum Frame {
        NO_FRAME,
        RULES,
        BOX
    };
    Table( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c, Tag::SPACES ), rules( BOTH ),
        frame( BOX ) { };
    ~Table() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Table( const Table& rhs );              //no copy
    Table& operator=( const Table& rhs );   //no assignment
    std::vector< unsigned char > colWidth;
    typedef std::vector< unsigned char >::iterator ColWidthIter;
    typedef std::vector< unsigned char >::const_iterator ConstColWidthIter;
    Rules rules;
    Frame frame;
    void tbBorder( bool bottom );
    void rowRule();
};

class ETable : public Tag {
public:
    ETable( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~ETable() { };
    void buildText( Cell* cell );
private:
    ETable( const ETable& rhs );            //no copy
    ETable& operator=( const ETable& rhs ); //no assignment
};

class TableCol : public Tag {
public:
    TableCol( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char w ) : Tag( d, p, f, r, c, Tag::SPACES ),
        data( 1 ), colWidth( w ) { };
    ~TableCol() { };
    unsigned int rows() const { return data.size(); };
    std::list< Element* >& rowData( unsigned int row ) { return data[ row ]; };
    Lexer::Token parse( Lexer* lexer );
    void appendData( unsigned int row, Element* e ) { data[ row ].push_back( e ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    TableCol( const TableCol& rhs );            //no copy
    TableCol& operator=( const TableCol& rhs ); //no assignment
    std::vector< std::list< Element* > > data;  //elements owned by TableRow
    unsigned char colWidth;
};

class TableRow : public Tag {
public:
    TableRow( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, std::vector< unsigned char >& w, Table::Rules rl,
        Table::Frame fr ) : Tag( d, p, f, r, c, Tag::SPACES ), colWidth( w ),
        rules( rl ), frame( fr ) { };
    ~TableRow();
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    TableRow( const TableRow& rhs );            //no copy
    TableRow& operator=( const TableRow& rhs ); //no assignment
    std::vector< TableCol* > columns;
    typedef std::vector< TableCol* >::iterator ColIter;
    typedef std::vector< TableCol* >::const_iterator ConstColIter;
    typedef std::list< Element* >::iterator ChildrenIter;
    std::vector< unsigned char >& colWidth;
    Table::Rules rules;
    Table::Frame frame;
};

#endif //TABLE_DEFINED
