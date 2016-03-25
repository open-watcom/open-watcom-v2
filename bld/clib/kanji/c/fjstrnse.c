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
#include <string.h>
#include <jstring.h>
#include <jctype.h>

/**
*
*  Name:        _fjstrnset      ������ɕ����𖄂߂�
*
*  Synopsis:    p = _fjstrnset( str, c, n );
*
*               FJSTRING        p;      str�Ɠ����|�C���^
*               FJSTRING        str;    �����𖄂߂镶����
*               JMOJI   c;      ���߂镶���̒l
*               unsigned int n; �����𖄂߂钷��
*
*  Description: _fjstrnset�֐��͕�����str������ c �̒l�Ŗ��߂܂��Bc ��������
*               �ꍇ�A���ʂW�r�b�g���A�����̑�P�o�C�g�ŁA��ʂW�r�b�g��������
*               ��Q�o�C�g�ł��B��Q�o�C�g�� NULL�̏ꍇ�A��P�o�C�g�� NULL
*               �Ƃ��Ĉ����܂��B�����A c �ɂP�o�C�g�������w�肷��ꍇ�ɂ́A
*               ��ʃo�C�g�́A�O�ɂ���K�v������܂��Bn �̒l��������̒������
*               �傫���A���A������̒�������̏ꍇ�ɂ́Ac �������̎��ɂ́A
*               ������̍Ō�̃o�C�g��0x20�ɂȂ�܂��Bn �����ڂ������̑�P
*               �o�C�g�̏ꍇ�A��Q�o�C�g��0x20�ɕύX���܂��B
*
*  Returns:     _fjstrnset�֐��� str �Ɠ����l��Ԃ��܂��B
*
*
*  Name:        _fjstrnset      fill the string with a letter
*
*  Synopsis:    p = _fjstrnset( str, c, n );
*
*               FJSTRING        p;      return value same as "str"
*               FJSTRING        str;    pointer to the string to be filled
*               JMOJI   c;      value of letter
*               unsigned int n; length to fill letter
*
*  Description: The _fjstrnset function fills the string "str" with the value
*               of the argument "c". If you specify "c" with KANJI letter,
*               JMOJI consist with 16 bit and its' high byte consist with
*               first byte code of KANJI letter, and low byte consist with
*               second byte code of KANJI letter.
*               If second byte code is NULL then also consider the first byte
*               code to NULL. If you specify the 1 byte character code, it must
*               be into low byte of JMOJI, and high byte has to consist with
*               NULL code.
*               If the value of "n" is greater than the length of the string
*               then, and if also length of the string is odd number then,
*               and if you have specified "c" with KANJI letter then, the last
*               byte of string is changed to 0x20.
*               Also When the value which "n" points to is the first byte code
*               of KANJI letter in the "str" string, the second byte code is
*               changed to 0x20. This function is a data independent form that
*               accept far pointer argunments. It is most useful in mix memory
*               model applications.
*
*  Returns:     The jstrnset function returns the same value of "str".          *
**/

_WCRTLINK FJSTRING _WCI86FAR _fjstrnset( JCHAR _WCFAR *str, JMOJI c, size_t n )
{
    JCHAR _WCFAR *dest, _WCFAR *keep = str;
    JMOJI c2;
    size_t byte, count, len;

    if( !n ) return str;
    byte = n * ( jiszen( c ) ? 2 : 1 );
    len = _fstrlen( (char _WCFAR *)str );
    if( len <= byte ) {
        if( jiszen( c ) ) {
            for( count = 0; count < len/2; count++ ) {
                str = _fjputmoji( str, c );
            }
            if( len & 1 )
                *str = ' ';
            }
        else {
            _fmemset( str, c, len );
        }
    } else {
        for( dest = str; dest - str < byte - 1; ) {
            dest = _fjgetmoji( dest, (JMOJI _WCFAR *)&c2 );
        }
        _fjgetmoji( dest, (JMOJI _WCFAR *)&c2 );
        if( dest - str != byte  &&  jiszen( c2 ) ) {
            *( dest+1 ) = ' ';
        }
        if( jiszen( c ) ) {
            for( count = 0; count < n; count++ ) {
                str = _fjputmoji( str, c );
            }
        } else {
            _fmemset( str, c, n );
        }
    }
    return keep;
}
