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
#include <jstring.h>

/**
*
*  Name:        jstradv         文字数分先のポインタを得る
*
*  Synopsis:    news = jstradv( s, nm );
*
*               JSTRING news;   新文字列
*               JSTRING s;      文字列
*               int nm;         文字数
*
*  Description: 文字列ｓの先頭からｎｍで指定した文字数だけ先の文字を指す
*               ポインタを返す。漢字コードは２バイトで１文字と数える。
*
*  Returns:     文字列ｓが文字数ｎｍよりも短い場合は、文字列の最後にある
*               ヌル文字を指すポインタを返す。
*
*
*  Name:        jstradv         get forward string position in specified string.*
*  Synopsis:    news = jstradv( s, nm );
*
*               JSTRING news;   forward position
*               JSTRING s;      current position of string
*               int     nm;     number of letters for forwarding
*
*  Description: This function returns the forward position where is advanced nm
*               times per letter from current position for s string. A kanji
*               letter consists of 2 byte.
*
*  Returns:     If the length of s string is shorter than nm number then this
*               function will return the end of position for s string.
**/


_WCRTLINK JSTRING jstradv( const JCHAR *s, size_t nm )
{
    return( (JSTRING) (s + mtob( s, nm )) );
}
