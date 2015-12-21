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
#include <stdio.h>
#include <jtime.h>

#define TMSZ 37

static const char   *dtbl[] = {"日","月","火","水","木","金","土"};

static char         s[TMSZ];

/**
*
*  Name:        jasctime
*
*  Synopsis:    s = jasctime( t )
*
*               unsigned char *s;       ASCII 文字列
*               struct tm *t;           入力の tm 構造体
*
*  Description: tm 構造体の値を以下の形式の文字列に変換する。
*
*                       YYYY 年 MM 月 dd 日  (DD)  hh:mm:ss\n\0
*
*               DD は曜日、MM は月、dd は日付、hh:mm:ss は時：分：秒、
*               YYYY は年である。
*
*
*  Name:        jasctime
*
*  Synopsis:    s = jasctime( t )
*
*               unsigned char *s;       ASCII string returned
*               struct tm *t;           input tm structure
*
*  Description: Represent the tm structure as a string in the form
*
*                       YYYY 年 MM 月 dd 日  (DD)  hh:mm:ss\n\0
*
*               where DD is the day of the week, MM is the month, dd is
*               the day of the month, hh:mm:ss is the hour:minute:seconds,
*               and YYYY is the year.
*
**/

_WCRTLINK unsigned char *jasctime( const struct tm *t )
{
    /*           1234567890 123456 7890123456789  012  345  6 7*/
    sprintf( s, "%04d 年 %2d 月 %2d 日 （%s） %02d:%02d:%02d\n",
        t->tm_year+1900,
        t->tm_mon+1,
        t->tm_mday,
        dtbl[t->tm_wday],
        t->tm_hour,
        t->tm_min,
        t->tm_sec );
    return( (unsigned char *)s );
}
