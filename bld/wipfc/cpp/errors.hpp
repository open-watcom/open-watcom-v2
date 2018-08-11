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
* Description:  Exceptions and errors
*
* Error message format is <fullfilename:line:col> errnum: text [optional info]
*
****************************************************************************/

#ifndef ERRORS_INCLUDED
#define ERRORS_INCLUDED

#include <string>

enum ErrCode {
    ERR_NO_ERR = 0,
    #define PICK(a,b) a,
    #include "err.hpp"
    ERR_LAST_ERR
};

extern const char* ErrText[ ERR_LAST_ERR + 1 ];

class FatalError {
public:
    FatalError ( ErrCode c ) : _code( c ) { }

    ErrCode         _code;
};
/*****************************************************************************/
class FatalIOError {
public:
    FatalIOError ( ErrCode c, const std::wstring& f ) : _code( c ), _fname( f ) { }
    FatalIOError ( ErrCode c, const wchar_t* f ) : _code( c ), _fname( f ) { }

    ErrCode         _code;
    std::wstring    _fname;
};
/*****************************************************************************/
class Class1Error {
public:
    Class1Error ( ErrCode c ) : _code( c ) { }

    ErrCode         _code;
};
/*****************************************************************************/
class Class2Error {
public:
    Class2Error ( ErrCode c ) : _code( c ) { }

    ErrCode         _code;
};
/*****************************************************************************/
class Class3Error {
public:
    Class3Error ( ErrCode c ) : _code( c ) { }

    ErrCode         _code;
};

#endif //ERRORS_INCLUDED
