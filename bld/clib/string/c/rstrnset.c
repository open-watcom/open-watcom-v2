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
* Description:  Implementation of strnset() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strnset,_wcsnset)( CHAR_TYPE *s, int c, size_t n )
/********************************************************************/
{
    RISC_DATA_LOCALREF;
    UINT                *dw = ROUND(s); /* round down to dword */
    UINT                dword, cdword, tmpdword;
    INT                 offset = OFFSET(s);

#ifdef __WIDECHAR__
    if( offset % 2 )
        return( __simple__wcsnset( s, c, n ) );
#endif

    if( n == 0 )
        return( s );    // nothing left to set

    /*** Initialize locals ***/
    c &= CHR1MASK;
#ifdef __WIDECHAR__
    cdword = (c<<16) | c;
#else
    cdword = (c<<24) | (c<<16) | (c<<8) | c;
#endif
    dword = *dw;

    /*** Set any bytes up to a 4-byte alignment ***/
    tmpdword = FRONT_CHRS(dword,offset);
#ifdef __WIDECHAR__
    switch( offset ) {
      case 0:
        if( CHR1(dword) && n ) {
            tmpdword |= CHR1(cdword);
            --n;
        } else {
            return( s );
        }
        /* fall through */
      default:              /* offset must equal 2 (no odd offsets) */
        if( CHR2(dword) && n ) {
            tmpdword |= CHR2(cdword);
            *dw = tmpdword;
            --n;
        } else {
            tmpdword |= SKIP_CHRS(dword,1);
            *dw = tmpdword;
            return( s );
        }
    }
#else
    switch( offset ) {
      case 0:
        if( CHR1(dword) && n ) {
            tmpdword |= CHR1(cdword);
            --n;
        } else {
            return( s );
        }
        /* fall through */
      case 1:
        if( CHR2(dword) && n ) {
            tmpdword |= CHR2(cdword);
            --n;
        } else {
            tmpdword |= SKIP_CHRS(dword,1);
            *dw = tmpdword;
            return( s );
        }
      case 2:
        if( CHR3(dword) && n ) {
            tmpdword |= CHR3(cdword);
            --n;
        } else {
            tmpdword |= SKIP_CHRS(dword,2);
            *dw = tmpdword;
            return( s );
        }
      default:
        if( CHR4(dword) && n ) {
            tmpdword |= CHR4(cdword);
            *dw = tmpdword;
            --n;
        } else {
            tmpdword |= SKIP_CHRS(dword,3);
            *dw = tmpdword;
            return( s );
        }
    }
#endif

    dw++;

    /*** Write in aligned 4-byte groups ***/
    for( ;; ) {
        if( n == 0 )
            return( s );    // nothing left to set
        dword = *dw;
        if( GOT_NIL(dword) )  break;
        if( n >= CHARS_PER_WORD ) {
            *dw++ = cdword;
            n -= CHARS_PER_WORD;
        } else {
            *dw = FRONT_CHRS(cdword,n) | SKIP_CHRS(dword,n);
            return( s );
        }
    }

    /*** Write in last dword ***/
#ifdef __WIDECHAR__
    if( !CHR1(dword) ) {
        return( s );
    } else {
        if( n >= 2 ) {
            *dw = SKIP_CHRS(dword,2) | FRONT_CHRS(cdword,2);
        } else if( n == 1 ) {
            *dw = SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1);
        }
        return( s );
    }
#else
    if( !CHR1(dword) ) {
        return( s );
    } else if( !CHR2(dword) ) {
        if( n >= 1 ) {
            *dw = (SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1));
        }
        return( s );
    } else if( !CHR3(dword) ) {
        if( n >= 2 ) {
            *dw = (SKIP_CHRS(dword,2) | FRONT_CHRS(cdword,2));
        } else if( n == 1 ) {
            *dw = (SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1));
        }
        return( s );
    } else {
        if( n >= 3 ) {
            *dw = (SKIP_CHRS(dword,3) | FRONT_CHRS(cdword,3));
        } else if( n == 2 ) {
            *dw = (SKIP_CHRS(dword,2) | FRONT_CHRS(cdword,2));
        } else if( n == 1 ) {
            *dw = (SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1));
        }
        return( s );
    }
#endif
}
