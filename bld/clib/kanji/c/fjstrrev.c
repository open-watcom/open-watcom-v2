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
#include <jctype.h>
#include <string.h>

/**
*
*  Name:        _fjstrrev -- ������𔽓]����
*
*  Synopsis:    p = _fjstrrev( s );
*
*               FJSTRING p,s;   ������
*
*  Description: ���̊֐��͕�����𔽓]���܂��B�܂�A������̒��_�Ɋւ��āA
*               ��]������֐��̊����łł���B�����͂P�����Ƃ��Ĉ����A
*               ������̒��̊����̑�P�o�C�g�Ƒ�Q�o�C�g�̏����͕ۂ����B
*
*  Returns:     _fjstrrev �֐��́A���� s �Ɠ����l��Ԃ��B
*
*
*  Name:        _fjstrrev -- reverse a character string
*
*  Synopsis:    p = _fjstrrev( s );
*
*               FJSTRING p,s;   string pointer
*
*  Description: This function reverses a character string.  That is, it
*               "rotates" the string about its mid-point (KANJI version).
*               So the KANJI letter consider to 1 character, it to be kept
*               the order for the first byte code and second byte code of
*               a KANJI letter inward string. This function is a data
*               independent form that accept far pointer argunments.
*               It is most useful in mix memory model applications.
*
*  Returns:     The _fjstrrev function returns the same value of argument "s".
*
**/

_WCRTLINK FJSTRING _WCI86FAR _fjstrrev( JCHAR _WCFAR *s )
{
    JCHAR _WCFAR *p, _WCFAR *q, _WCFAR *r;

    p = s;
    q = p + _fstrlen( (char _WCFAR *)p );
    while( p < q ) {
        if( iskanji( *p ) ) {
            if( iskanji2( *( r = p + 1 ) ) ) {
                *p ^= *r;
                *r ^= *p;
                *p++ ^= *r;
            }
        }
        p++;
    }
    p = s;
    while( p < --q ) {
        *p ^= *q;
        *q ^= *p;
        *p++ ^= *q;
    }
    return( s );
}
