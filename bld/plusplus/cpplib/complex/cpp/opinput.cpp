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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <complex.h>

_WPRTLINK istream &operator >> ( istream &strm, Complex &z ) {
/***********************************************************/
    int  parens;
    char c;

    strm >> c;
    if( strm.eof() ) {
        strm.clear( ios::failbit );
        return( strm );
    }
    if( c == '(' ) {
        parens = 1;
    } else {
        parens = 0;
        strm.putback( c );
    }
    strm >> z.__r;
    if( !strm.good() ) {
        return( strm );
    }
    strm >> c;
    if( !strm.good() ) {
        z.__i = 0.0;
        return( strm );
    }
    if( c != ',' ) {
        z.__i = 0.0;
        if( !parens || c != ')' ) {
            strm.putback( c );
        }
        return( strm );
    }
    strm >> z.__i;
    if( !strm.good() ) {
        return( strm );
    }
    if( parens ) {
        strm >> c;
        if( strm.eof() ) {
            return( strm );
        }
        if( c != ')' ) {
            strm.putback( c );
        }
    }
    return( strm );
}
