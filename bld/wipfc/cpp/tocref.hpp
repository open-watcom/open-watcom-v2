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
* Description:  A reference to a panel (TOC entry) used in resource number
*   and name/id mapping, and to track cross-references
*
****************************************************************************/

#ifndef TOCREF_INCLUDED
#define TOCREF_INCLUDED

#include <cstdio>
#include <set>
#include "xref.hpp"

class TocRef {
public:
    TocRef( const std::wstring* f, unsigned int r, word i ) :
        _fileName( f ), _lineNumber( r ), _tocIndex( i ) { };
    ~TocRef() { };
    word index() const { return _tocIndex; };
    void addXRef( XRef& ref ) { _xref.insert( ref ); };
    void write( std::FILE* logfp ) const;

private:
    const std::wstring*     _fileName;
    std::set< XRef >        _xref;
    typedef std::set< XRef >::iterator XRefIter;
    typedef std::set< XRef >::const_iterator ConstXRefIter;
    unsigned int            _lineNumber;
    word                    _tocIndex;
};

#endif //TOCREF_INCLUDED
