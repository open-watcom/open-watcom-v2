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
* Description: Base class for IPF file or buffer
*
****************************************************************************/

#ifndef IPFDATA_INCLUDED
#define IPFDATA_INCLUDED

#include <string>

#define EOB L'\0'

class IpfData {
public:
    IpfData() : lineNum( 1 ), charNum( 1 ) { };
    IpfData( unsigned int line, unsigned int col ) : lineNum( line ), charNum( col ) { };
    virtual
    ~IpfData() { };
    unsigned int currentLine() const { return lineNum; };
    void incLine() { ++lineNum; };
    void decLine() { --lineNum; };
    unsigned int currentCol() const { return charNum; };
    void incCol() { ++charNum; };
    void decCol() { --charNum; };
    void resetCol() { charNum = 1; };
    //Returns the file or buffer name for use in error messages
    virtual
    const std::wstring* name() const = 0;
    //Read a character
    virtual
    std::wint_t get() = 0;
    //Un-read a character
    virtual
    void unget( wchar_t ch ) = 0;
    //Seek to beginning
    virtual
    void reset() = 0;
    //Seek to position relative to beginning
    virtual
    void setPos( long int offset ) = 0;
    //Get the current position
    virtual
    long int pos() = 0;
private:
    IpfData( const IpfData& rhs );              //no copy
    IpfData& operator=( const IpfData& rhs );   //no assignment
    unsigned int lineNum;
    unsigned int charNum;
};

#endif //IPFDATA_INCLUDED

