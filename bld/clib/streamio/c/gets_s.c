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
* Description:  Implementation of gets_s().
*
****************************************************************************/

#include "variety.h"
#include "saferlib.h"
#include <stdio.h>

_WCRTLINK extern char *gets_s( char *s, rsize_t n )
/*************************************************/
{
    int		c;
    char        *cs;
    rsize_t     len;
    unsigned	oflag;

    /* Check runtime constraint: s shall not be a null pointer */
    if( ! __check_constraint_nullptr( s ) ) {
        /* Just trigger the constraint handler if necessary */
    }

    /* Check runtime constraint: n shall not be zero nor
       greater than RSIZE_MAX */
    if(  ! __check_constraint_zero( n )
       || ! __check_constraint_maxsize( n ) ) {
        n = 0;
    }

    /* Filter out stream error and EOF status during this function */
    oflag = stdin->_flag & (_SFERR | _EOF);
    stdin->_flag &= ~(_SFERR | _EOF);

    cs = s;
    len = n;

    /*
     Read data until any of following conditions is met:
     - Received EOF
     - Read error
     - New line character received
     */
    while( (c = getc( stdin )) != EOF
        && (c != '\n') ) {
        if( cs && len ) {
            *cs++ = c;
            len--;
        }
    }

    /* Post-process runtime constraints, return NULL when catched */
    if( s == NULL || n == 0) {
        stdin->_flag |= oflag;
        return( NULL );
    }

    /* After this point, it is assumed that s is valid and
       contains at least one character */

    /* Catch too small buffer and I/O error */
    if( ! __check_constraint_toosmall( s, len )
      || ferror( stdin ) ) {
        stdin->_flag |= oflag;
        *s = 0;
        return( NULL );
    }

    /* Restore stream error states if they were present previously */
    stdin->_flag |= oflag;

    /* If we got EOF and didn't get any characters, return NULL */
    if( c == EOF && cs == s ) {
        *s = 0;
        return( NULL );
    }

    /* Terminate string */
    *cs = 0;

    return( s );
}
