/****************************************************************************
*
*                            Open Watcom Project
*
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
* Description:  Implementation of POSIX getdelim and getline
*
* Author: J. Armstrong
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "rtdata.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif

_WCRTLINK ssize_t getdelim( char **s, size_t *n, int delim, FILE *fp )
{
    int     c;
    size_t  linelength;
    size_t  size;
    char    *buff;

    if( n == NULL || s == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    buff = *s;
    size = *n;
    if( buff == NULL )
        size = 0;

    linelength = 0;
    while( (c = getc( fp )) != EOF ) {
        if( linelength + 1 >= size ) {
            size = linelength * 2 | 0x7F;
            buff = realloc( buff, size );
            if( buff == NULL ) {
                _RWD_errno = ENOMEM;
                return( -1 );
            }
            *s = buff;
            *n = size;
        }
        buff[linelength++] = c;
        if( c == delim ) {
            break;
        }
    }
    if( linelength == 0 )
        return( -1 );

    buff[linelength] = '\0';    /* Add the null character */
    return( linelength );
}

_WCRTLINK ssize_t getline( char **s, size_t *n, FILE *fp )
{
    return( getdelim( s, n, '\n', fp ) );
}
