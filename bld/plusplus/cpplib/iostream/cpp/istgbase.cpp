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

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <ctype.h>
#include <iostream>
#include <streambu>
#endif
#include "lock.h"
#include "isthdr.h"

std::ios::iostate __getbase( std::streambuf *sb, int &base, int &offset ) {

    int ch;

    __lock_it( sb->__b_lock );
    ch = sb->speekc();
    if( ch == EOF ) {
        if( offset == 0 ) {
            return( std::ios::eofbit );
        }
        return( std::ios::failbit );
    }
    if( ch != '0' ) {
        if( isdigit( ch ) ) {
            base = 10;
        } else {
            return( std::ios::failbit );
        }
    } else {
        offset++;
        ch = sb->snextc();
        if( tolower( ch ) == 'x' ) {
            sb->sbumpc();
            if ( isxdigit( sb->speekc() ) ) {
                base = 16;
                offset++;
            } else {
                base = 8;
                sb->sputbackc( (char)ch );
                sb->sputbackc( '0' );
                offset--;
            }
        } else {
            base = 8;
            sb->sputbackc( '0' );
            offset--;
        }
    }
    return( std::ios::goodbit );
}
