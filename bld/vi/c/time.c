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


#include "vi.h"
#include <time.h>

/*
 * GetTimeString - get string for current time/date
 */
void GetTimeString( char *st )
{
    time_t      tod;
    char        *s;
    int         i, j, start;
    char        am_pm;

    tod = time( NULL );
    s = ctime( &tod );
    am_pm = 'a';
    if( s[11] == '1' && s[12] == '2' ) {
        am_pm = 'p';
    } else if( (s[11] == '1' && s[12] > '2') || s[11] == '2' ) {
        am_pm = 'p';
        i = (s[11] - '0') * 10 + s[12] - '0';
        i -= 12;
        s[11] = (i / 10) + '0';
        s[12] = i % 10 + '0';
    }
    start = 11;
    if( s[11] == '0' ) {
        start = 12;
    }
    j = 0;
    for( i = start; i <= 15; i++ ) {
        st[j++] = s[i];
    }
#ifdef __WIN__
    st[j] = ' ';
    st[j + 1] = am_pm;
    st[j + 2] = 'm';
    st[j + 3] = '\0';
#else
    st[j] = am_pm;
    st[j + 1] = '\0';
#endif

} /* GetTimeString */

/*
 * GetDateString - get string for current time/date
 */
void GetDateTimeString( char *st )
{
    time_t      tod;
    char        *s;
    int         i, j;

    tod = time( NULL );
    s = ctime( &tod );
    j = 0;
    for( i = 0; i < 11; i++ ) {
        st[j++] = s[i];
    }
    for( i = 20; i < 24; i++ ) {
        st[j++] = s[i];
    }
    for( i = 10; i <= 18; i++ ) {
        st[j++] = s[i];
    }
    st[j] = '\0';

} /* GetDateTimeString */

/*
 * GetDateString - get string for current date
 */
void GetDateString( char *st )
{
    time_t      tod;

    tod = time( NULL );
    strftime( st, 20, "%A, %b %d", localtime( &tod ) );

} /* GetDateString */
