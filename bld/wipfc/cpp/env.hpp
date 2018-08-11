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
* Description:  Environment variables
*
****************************************************************************/

#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#include <map>
#include <string>

class Env {
public:
    Env() { };
    void add( const std::string& key );
    std::string& value( const std::string& key ) { return _data[key]; };
private:
    void killQuotes( std::string& value );

    std::map< std::string, std::string >    _data;
    typedef std::map< std::string, std::string >::iterator DataIter;
    typedef std::map< std::string, std::string >::const_iterator ConstDataIter;
};

extern class Env    Environment;

#endif //ENV_INCLUDED
