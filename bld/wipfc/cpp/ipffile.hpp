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
* Description: IPF File object
*
****************************************************************************/

#ifndef IPFFILE_INCLUDED
#define IPFFILE_INCLUDED

#include <cstdio>
#include "ipfdata.hpp"

class Nls;     // forward reference
class ICULoader;
struct UConverter;

class IpfFile : public IpfData {
public:
    IpfFile( const std::wstring* wfname, Nls *nls );
    IpfFile( const std::string& sfname, const std::wstring* wfname, Nls *nls );
    ~IpfFile();
    //Set the file name for use in error messages
    void setName( const std::wstring* fileName ) { _fileName = fileName; }
    //Returns the file or buffer name for use in error messages
    virtual
    const std::wstring* name() const { return _fileName; };
    //Read a character
    virtual
    std::wint_t get();
    //Un-read a character
    virtual
    void unget( wchar_t ch );
    const std::wstring *gets( bool removeEOL );

private:
    IpfFile( const IpfFile& rhs );              //no copy
    IpfFile& operator=( const IpfFile& rhs );   //no assignment
    std::wint_t  getwc();

    const std::wstring*     _fileName;
    std::FILE*              _stream;
    wchar_t                 _ungottenChar;
    bool                    _ungotten;
    std::wstring            _wbuffer;
    std::size_t             _pos;
    ICULoader               *_icu;
    UConverter              *_converter;
};

#endif
