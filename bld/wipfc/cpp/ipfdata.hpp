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
* Description: Base class for IPF file or buffer
*
****************************************************************************/

#ifndef IPFDATA_INCLUDED
#define IPFDATA_INCLUDED

#include <string>

#define EOB L'\0'

class IpfData {
public:
    IpfData() : _lineNum( 1 ), _charNum( 1 ) { };
    IpfData( unsigned int line, unsigned int col ) : _lineNum( line ), _charNum( col ) { };
    virtual
    ~IpfData() { };
    unsigned int currentLine() const { return _lineNum; };
    void incLine() { ++_lineNum; };
    void decLine() { --_lineNum; };
    unsigned int currentCol() const { return _charNum; };
    void incCol() { ++_charNum; };
    void decCol() { --_charNum; };
    void resetCol() { _charNum = 1; };
    //Returns the file or buffer name for use in error messages
    virtual
    const std::wstring* name() const = 0;
    //Read a character
    virtual
    std::wint_t get() = 0;
    //Un-read a character
    virtual
    void unget( wchar_t ch ) = 0;
private:
    IpfData( const IpfData& rhs );              //no copy
    IpfData& operator=( const IpfData& rhs );   //no assignment

    unsigned int        _lineNum;
    unsigned int        _charNum;
};

#endif //IPFDATA_INCLUDED

