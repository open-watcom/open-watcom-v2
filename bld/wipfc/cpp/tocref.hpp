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
* Description:  A reference to a panel (TOC entry) used in resource number
*   and name/id mapping, and to track cross-references
*
****************************************************************************/

#ifndef TOCREF_INCLUDED
#define TOCREF_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <set>
#include "xref.hpp"

class TocRef {
public:
    TocRef( const std::wstring* f, unsigned int r, STD1::uint16_t i ) :
        fileName( f ), lineNumber( r ), tocIndex( i ) { };
    ~TocRef() { };
    STD1::uint16_t index() const { return tocIndex; };
    void addXRef( XRef& ref ) { xref.insert( ref ); };
    void write( std::FILE* out ) const;
private:
    const std::wstring* fileName;
    std::set< XRef > xref;
    typedef std::set< XRef >::iterator XRefIter;
    typedef std::set< XRef >::const_iterator ConstXRefIter;
    unsigned int lineNumber;
    STD1::uint16_t tocIndex;
};

#endif //TOCREF_INCLUDED
