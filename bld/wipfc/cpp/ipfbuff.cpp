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
* Description:  IpfBuffer implementation
*
****************************************************************************/

#include "ipfbuff.hpp"

IpfBuffer::IpfBuffer( const std::wstring* fname,    //originiating file
    unsigned int line,                              //line in that file
    unsigned int col,
    const std::wstring& text ) :
    IpfData( line, col ),
    fileName( fname ),
    buffer( text ),
    ungotCh( EOB )
{
    head = buffer.begin();
    tail = buffer.end();
}
/*****************************************************************************/
IpfBuffer::IpfBuffer( const std::wstring* fname,    //originating file
    const std::wstring& text ) :                    //text to substitute
    IpfData(),
    fileName( fname ),
    buffer( text ),
    ungotCh( EOB )
{
    head = buffer.begin();
    tail = buffer.end();
}
/*****************************************************************************/
//Read a character
//Returns EOB if end-of-file reached
std::wint_t IpfBuffer::get()
{
    wchar_t ch = EOB;
    if ( ungotCh != EOB ) {
        ch = ungotCh;
        ungotCh = EOB;
    }
    else if( head != tail ) {
        ch = *head;
        ++head;
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
    if( head > buffer.begin() ) {
        --head;
        decCol();
        *head = ch;
        if( ch == L'\n' )
            decLine();
    }
    else
        ungotCh = ch;
}

