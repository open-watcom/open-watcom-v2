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
* Description:  Implementation of strstr() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strstr,wcsstr)( const CHAR_TYPE *str,
                                    const CHAR_TYPE *substr )
/***********************************************************/
{
    size_t              strLen;
    size_t              substrLen;
    int                 i, maxi;
    CHAR_TYPE           ch;

    substrLen = __F_NAME(strlen,wcslen)( substr );
    if( substrLen == 0 )
        return( (CHAR_TYPE*)str );

    strLen = __F_NAME(strlen,wcslen)( str );
    maxi = strLen - substrLen + 1;
    ch = substr[0];

    for( i = 0; i < maxi; i++ ) {
        if( str[i] == ch ) {
            if( !__F_NAME(strncmp,wcsncmp)( str + i, substr, substrLen ) ) {
                return( (CHAR_TYPE*)str + i );
            }
        }
    }
    return( NULL );
}
