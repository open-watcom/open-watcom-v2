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
* Description: IPF Buffer object
* Used for parsing internally generated text blocks (e.g., note prompts )
*
****************************************************************************/

#ifndef IPFBUFFER_INCLUDED
#define IPFBUFFER_INCLUDED

#include "ipfdata.hpp"

class IpfBuffer : public IpfData {
public:
    IpfBuffer( const std::wstring* fname, unsigned int line, unsigned int col, const std::wstring& text );
    IpfBuffer( const std::wstring* fname, const std::wstring& text );
    ~IpfBuffer() { };
    //Returns the file or buffer name for use in error messages
    virtual
    const std::wstring* name() const { return _fileName; };
    //Read a character
    virtual
    std::wint_t get();
    //Un-read a character
    virtual
    void unget( wchar_t ch );
private:
    IpfBuffer( const IpfBuffer& rhs );              //no copy
    IpfBuffer& operator=( const IpfBuffer& rhs );   //no assignment

    const std::wstring*     _fileName;
    std::wstring            _buffer;
    std::wstring::iterator  _head;
    std::wstring::iterator  _tail;
    wchar_t                 _ungotCh;
};

#endif //IPFBUFFER_INCLUDED
