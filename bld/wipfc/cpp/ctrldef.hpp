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
* Description:  ctrldef/ectrldef tag sequence
*   :ctrldef / :ectrldef
*   Follows :docprof
*   May contain :pbutton, :ctrl
*
****************************************************************************/

#ifndef CTRLDEF_INCLUDED
#define CTRLDEF_INCLUDED

#include <vector>
#include "lexer.hpp"

class Controls;
class CtrlTag;
class Document;

class CtrlDef {
public:
    CtrlDef( Document* d ) : document( d ) { };
    ~CtrlDef();
    Lexer::Token parse( Lexer* lexer );
    void build( Controls* ctrls );
    void appendChild( CtrlTag* e ) { children.push_back( e ); };
private:
    CtrlDef( const CtrlDef& rhs );              //no copy
    CtrlDef& operator=( const CtrlDef& rhs );   //no assignment
    Document* document;
    std::vector< CtrlTag* > children;
    typedef std::vector< CtrlTag* >::iterator ChildrenIter;
    typedef std::vector< CtrlTag* >::const_iterator ConstChildrenIter;
};

#endif //CTRLDEF_INCLUDED
