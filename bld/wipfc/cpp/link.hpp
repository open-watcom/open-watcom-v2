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
* Description:  link and elink tags
*   :link / :elink
*       reftype=hd (to header, requires refid, may use database)
*               fn (to footnote, requires refid)
*               launch (exec program, requires object and data attributes)
*               inform (send a message, requires res and not :elink)
*       res=[0-9]+
*       refid=[a-zA-z][a-zA-z0-9]*
*       database='' (name of external ipf/hlp file)
*       object='' (name of executable)
*       data='' (parameters to pass to executable)
*       auto (automatically open this link)
*       viewport
*       dependent
*       split
*       child
*       group=[0-9]+
*       vpx=([0-9]+[c|x|p|%]) | (left|center|right|top|bottom)
*       vpy=([0-9]+[c|x|p|%]) | (left|center|right|top|bottom)
*       vpcx=([0-9]+[c|x|p|%])
*       vpcy=([0-9]+[c|x|p|%])
*       titlebar=yes|sysmenu|minmax|both|none (default: both)
*       scroll=horizontal|vertical|both|none (default: both)
*       rules=border|sizeborder|none (default: sizeborder)
*       x=[0-9]+
*       y=[0-9]+
*       cx=[0-9]+
*       cy=[0-9]+
*   Terminated by :elink unless it is a child of :artlink, or type == inform
*
****************************************************************************/

#ifndef LINK_INCLUDED
#define LINK_INCLUDED

#include "config.hpp"
#include "tag.hpp"
#include "toc.hpp"

class GlobalDictionaryWord; //forward references

class Link : public Tag {
public:
    enum LinkType {
        TOPIC,
        FOOTNOTE,
        LAUNCH,
        INFORM
    };
    Link( Document* d, Element *p, const std::wstring* f, unsigned int r, \
        unsigned int c, WsHandling ws = Tag::NONE ) : Tag( d, p, f, r, c, ws ),
        refid( 0 ), res( 0 ), x( 0 ), y( 0 ), cx( 0 ), cy( 0 ), type( TOPIC ),
        automatic( false ), child( false ), dependent( false ), doGroup( false ),
        doOrigin( false ), doSize( false ), doStyle( false ), split( false ),
        viewport( false ), noElink( false ), hypergraphic( false ) { };
    ~Link();
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
    //don't look for an :elink tag for closure
    void setNoEndTag() { noElink = true; };
    void setHypergraphic() { hypergraphic = true; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Link( const Link& rhs );                //no copy
    Link& operator=( const Link& rhs );     //no assignment
    std::wstring database;
    std::wstring object;
    std::wstring data;
    PageOrigin origin;
    PageSize size;
    PageStyle style;
    PageGroup group;
    GlobalDictionaryWord* refid;    //of link target
    STD1::uint16_t res;              //of link target
    STD1::uint16_t x;                //hypergraphic hotspot
    STD1::uint16_t y;
    STD1::uint16_t cx;
    STD1::uint16_t cy;
    LinkType type;
    bool automatic;
    bool child;
    bool dependent;
    bool doGroup;
    bool doOrigin;
    bool doSize;
    bool doStyle;
    bool split;
    bool viewport;
    bool noElink;
    bool hypergraphic;
    void doTopic( Cell* cell );
    void doFootnote( Cell* cell );
    void doLaunch( Cell* cell );
    void doInform( Cell* cell );
};

class ELink : public Tag {
public:
    ELink( Document* d, Element *p, const std::wstring* f, unsigned int r, \
    unsigned int c, WsHandling ws = Tag::NONE ) : Tag( d, p, f, r, c, ws ) { };
    ~ELink() { };
    void buildText( Cell* cell );
private:
    ELink( const ELink& rhs );              //no copy
    ELink& operator=( const ELink& rhs );   //no assignment

};

#endif //LINK_INCLUDED
