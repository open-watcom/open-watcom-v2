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
* Description:  Global Names data
* Obtained from the "id" or "name" attribute of an :hn tag iff the "global"
* attribute flag is set
* unsigned short dictIndex[ IpfHeader.panelCount ]; //in ascending order
* unsigned short TOCIndex[ IpfHeader.panelCount ];
*
****************************************************************************/

#ifndef GNAMES_INCLUDED
#define GNAMES_INCLUDED

#include "config.hpp"
#include <map>
#include "gdword.hpp"
#include "ptrops.hpp"

class GNames {
public:
    GNames() { };
    ~GNames() { };
    void insert( GlobalDictionaryWord* word, STD1::uint16_t toc );
    //the number of names in the collection
    STD1::uint16_t size() const { return static_cast< STD1::uint16_t >( names.size() ); };
    STD1::uint32_t write( std::FILE *out ) const;
private:
    GNames( const GNames& rhs );            //no copy
    GNames& operator=( const GNames& rhs ); //no assignment
    std::map< GlobalDictionaryWord*, STD1::uint16_t, ptrLess< GlobalDictionaryWord* > > names;
    typedef std::map< GlobalDictionaryWord*, STD1::uint16_t, ptrLess< GlobalDictionaryWord* > >::const_iterator ConstNameIter;
    typedef std::map< GlobalDictionaryWord*, STD1::uint16_t, ptrLess< GlobalDictionaryWord* > >::iterator NameIter;
};

#endif //GNAMES_INCLUDED
