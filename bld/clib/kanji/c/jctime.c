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
#include <jtime.h>
#include <time.h>

/**
*
*  Name:        jctime
*
*  Synopsis:    s = jctime( timer )
*
*               char *s;                ASCII 文字列
*               time_t *timer;          秒単位の時間(long 型)
*
*  Description: timer が指す long int 型の値を jasctime() が返す形式の ASCII
*               文字列に変換する。
*
*               入力時刻は、 GMT で表示されていなければならない。この関数の
*               内部で timezone で示される時差を引くことで、地方時を算出
*               している。
*
*  Name:        jctime
*
*  Synopsis:    s = jctime( timer )
*
*               char *s;                ASCII string returned
*               time_t *timer;          long (seconds) input
*
*  Description: Converts the long integer pointed to by timer to an ASCII
*               string in the form returned by jasctime().
*
*               Note that this function expects its input to be expressed
*               as GMT and it converts to local time by subtracting the
*               timezone bias.
*
**/

_WCRTLINK unsigned char *jctime( const time_t *timer )
{
/* JBS 92/09/30
    time_t x;

    tzset();
    x = *timer - timezone;
    return( jasctime( gmtime( &x ) ) );
*/
    return( jasctime( localtime( timer ) ) );
}
