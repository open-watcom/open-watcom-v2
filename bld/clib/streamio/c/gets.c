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
* Description:  Implementation of gets() - read string from stdin.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>


_WCRTLINK CHAR_TYPE *__F_NAME(gets,getws)( CHAR_TYPE *s )
{
    INTCHAR_TYPE    c;
    CHAR_TYPE       *cs;
    unsigned        oflag;

    oflag = stdin->_flag & (_SFERR | _EOF);             /* 06-sep-91 */
    stdin->_flag &= ~(_SFERR | _EOF);
    cs = s;
    while( (c = __F_NAME((getc),(getwc))( stdin )) != __F_NAME(EOF,WEOF)
        && c != STRING( '\n' ) ) {
        *cs++ = c;
    }
    if( c == __F_NAME(EOF,WEOF)  &&  (cs == s || ferror( stdin )) ) {
        s = NULL;
    } else {
        *cs = NULLCHAR;
    }
    stdin->_flag |= oflag;                              /* 06-sep-91 */
    return( s );
}
