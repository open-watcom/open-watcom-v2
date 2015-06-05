/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. 
*    Portions Copyright (c) 2015 Open Watcom contributors.
*    All Rights Reserved.
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
* Description:  Implementation of _fstrtok() - far strtok().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include "rtdata.h"
#include "nextftok.h"
#include "setbits.h"
#include "thread.h"


_WCRTLINK char _WCFAR *_fstrtok_r( char _WCFAR *str, const char _WCFAR *charset, char _WCFAR **ptr )
{
    char            tc;
    unsigned char   vector[ CHARVECTOR_SIZE ];
    char _WCFAR     *p1;

    
    if( str == NULL ) {
        str = *ptr;            /* use previous value   */
        if( str == NULL ) return( NULL );
    }
    __fsetbits( vector, charset );
    for( ; tc = *str; ++str ) {
        /* quit if we find any char not in charset */
        if( GETCHARBIT( vector, tc ) == 0 )
            break;
    }
    if( tc == '\0' )
        return( NULL );
    p1 = str;
    for( ; tc = *p1; ++p1 ) {
        /* quit when we find any char in charset */
        if( GETCHARBIT( vector, tc ) != 0 ) {
            *p1 = '\0';             /* terminate the token  */
            p1++;                   /* start of next token  */
            *ptr = p1;
            return( str );
        }
    }
    *ptr = NULL;
    return( str );
}

_WCRTLINK char _WCFAR *_fstrtok( char _WCFAR *str, const char _WCFAR *charset )
{
    _INITNEXTFTOK
    return _fstrtok_r(str, charset, &_RWD_nextftok);
}
