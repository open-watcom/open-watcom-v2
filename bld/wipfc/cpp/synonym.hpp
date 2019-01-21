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
* Description:  A synonym entry
*
****************************************************************************/

#ifndef SYNONYM_INCLUDED
#define SYNONYM_INCLUDED

#include <cstdio>
#include <string>
#include <vector>


class OutFile;

class Synonym {
public:
    Synonym() : _offset( 0 ) { };
    ~Synonym() { };
    void add( const std::wstring& text ) { _synonyms.push_back( text ); };
    void write( OutFile* out );
    dword location() const { return _offset; };

private:
    std::vector< std::wstring > _synonyms;
    typedef std::vector< std::wstring >::iterator SynonymWIter;
    typedef std::vector< std::wstring >::const_iterator ConstSynonymWIter;
    typedef std::vector< std::string >::iterator SynonymIter;
    typedef std::vector< std::string >::const_iterator ConstSynonymIter;
    dword                       _offset;
};

#endif //SYNONYM_INCLUDED
