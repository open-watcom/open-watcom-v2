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
* Description:  Process docprof tag
*   :docprof
*       toc=[1-6]+ (default: 123)
*       dll=''
*       objectname=''
*       objectinfo=''
*       ctrlarea=page|coverpage|both|none
*       Must be a child of :userdoc
* Must follow :title
*
****************************************************************************/

#ifndef DOCPROF_INCLUDED
#define DOCPROF_INCLUDED

#include "lexer.hpp"

class Document; //forward references
class Controls;
class StringTable;

class DocProf {
public:
    enum CtrlArea {
        NONE,
        COVERPAGE,
        PAGE,
        BOTH
    };
    DocProf( Document* d) : document( d ), headerCutOff( 3 ), area( COVERPAGE ) { };
    ~DocProf() { };
    Lexer::Token parse( Lexer* lexer );
    void build( Controls* ctrls, StringTable* strs );
private:
    DocProf( const DocProf& rhs );              //no copy
    DocProf& operator=( const DocProf& rhs );   //no assignment
    Document* document;
    unsigned int headerCutOff;
    std::wstring dll;
    std::wstring objName;
    std::wstring objInfo;
    enum CtrlArea area;
};

#endif //DOCPROF_INCLUDED

