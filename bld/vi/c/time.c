/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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
    int         i, j;

    tod = time( NULL );
    s = ctime( &tod );
    j = 0;
    for( i = 11; i <= 15; i++ ) {
        st[j++] = s[i];
    }
    st[j] = '\0';

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
