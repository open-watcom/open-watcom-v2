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
* Description:  Panel control button data
*
****************************************************************************/

#ifndef CONTROLBUTTON_INCLUDED
#define CONTROLBUTTON_INCLUDED

#include <cstdio>
#include <string>

class OutFile;      // forward reference

class ControlButton {
public:
    ControlButton() : _res( 0 ), _index( 0 ) {};
    ControlButton( const std::wstring& i, word r, const std::wstring& t ) :
        _res( r ), _idnt( i ), _text( t ) { };
    void setId( std::wstring& i ) { _idnt = i; };
    const std::wstring& id() const { return _idnt; };
    void setRes( word r ) { _res = r; };
    void setText( std::wstring& t ) { _text = t; };
    void setIndex( word i ) { _index = i; };
    word index() { return _index; };
    dword write( OutFile* out ) const;
private:
    word            _res;       //message number
    word            _index;     //array index of this item
    std::wstring    _idnt;      //identifier
    std::wstring    _text;      //button text
};

#endif //CTRLBUTTON_INCLUDED
