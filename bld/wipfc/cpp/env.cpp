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
* Description:  Environment variables
*
****************************************************************************/

#include <cstdlib>
#include "env.hpp"

void Env::add( const std::string& key )
{
    char *env( std::getenv( key.c_str() ) );
    if( env ) {
        std::string val( env );
        killQuotes( val );
        data.insert( std::map< std::string, std::string >::value_type( key, val ) );
    }
    else
        data.insert( std::map< std::string, std::string >::value_type( key, "" ) );
}
/*****************************************************************************/
void Env::killQuotes( std::string& val )
{
    if ( val[0] == '"' || val[0] == '\'' )
        {
        val.erase( 0, 1 );
        if( val[ val.size() - 1 ] == '"' || val[ val.size() - 1 ] == '\'' )
            val.erase( val.size() - 1, 1 );
        }
}

