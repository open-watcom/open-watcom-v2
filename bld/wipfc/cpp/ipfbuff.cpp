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
* Description:  IpfBuffer implementation
*
****************************************************************************/


#include "wipfc.hpp"
#include "ipfbuff.hpp"

IpfBuffer::IpfBuffer( const std::wstring* fname,    //originiating file
    unsigned int line,                              //line in that file
    unsigned int col,
    const std::wstring& text ) :
    IpfData( line, col ),
    _fileName( fname ),
    _buffer( text ),
    _ungotCh( EOB )
{
    _head = _buffer.begin();
    _tail = _buffer.end();
}
/*****************************************************************************/
IpfBuffer::IpfBuffer( const std::wstring* fname,    //originating file
    const std::wstring& text ) :                    //text to substitute
    IpfData(),
    _fileName( fname ),
    _buffer( text ),
    _ungotCh( EOB )
{
    _head = _buffer.begin();
    _tail = _buffer.end();
}
/*****************************************************************************/
//Read a character
//Returns EOB if end-of-file reached
std::wint_t IpfBuffer::get()
{
    wchar_t ch = EOB;
    if( _ungotCh != EOB ) {
        ch = _ungotCh;
        _ungotCh = EOB;
    } else if( _head != _tail ) {
        ch = *_head;
        ++_head;
        incCol();
        if( ch == L'\n' ) {
            incLine();
            resetCol();
        }
    }
    return ch;
}
/*****************************************************************************/
void IpfBuffer::unget( wchar_t ch )
{
    if( _head > _buffer.begin() ) {
        --_head;
        decCol();
        *_head = ch;
        if( ch == L'\n' ) {
            decLine();
        }
    } else {
        _ungotCh = ch;
    }
}
