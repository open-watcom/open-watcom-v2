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


#include "widechar.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strupr,_wcsupr)( CHAR_TYPE *s )
/*************************************************/
{
    RISC_DATA_LOCALREF;
    UINT *           dw = ROUND(s);
    UINT             dword;
    INT              offset = OFFSET(s);
    INT              tmpdword, tmpchr;

    #ifdef __WIDECHAR__
        if( offset % 2 )  return( __simple__wcsupr( s ) );
    #endif

    /*** Initialize locals ***/
    dword = *dw;

    /*** Set any bytes up to a 4-byte alignment ***/
    tmpdword = FRONT_CHRS(dword,offset/CHARSIZE);
    #ifdef __WIDECHAR__
        switch( offset ) {
          case 0:
            tmpchr = CHR1(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR1(tmpchr);
            } else {
                return( s );
            }
            /* fall through */
          default:              /* offset must equal 2 (no odd offsets) */
            tmpchr = CHR2(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR2(tmpchr);
                *dw = tmpdword;
            } else {
                *dw = tmpdword;
                return( s );
            }
        }
    #else
        switch( offset ) {
          case 0:
            tmpchr = CHR1(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR1(tmpchr);
            } else {
                return( s );
            }
            /* fall through */
          case 1:
            tmpchr = CHR2(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR2(tmpchr);
            } else {
                tmpdword |= SKIP_CHRS(dword,2);
                *dw = tmpdword;
                return( s );
            }
            /* fall through */
          case 2:
            tmpchr = CHR3(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR3(tmpchr);
            } else {
                tmpdword |= SKIP_CHRS(dword,3);
                *dw = tmpdword;
                return( s );
            }
            /* fall through */
          default:
            tmpchr = CHR4(dword);
            if( tmpchr ) {
                tmpdword |= TO_UPR_CHR4(tmpchr);
                *dw = tmpdword;
            } else {
                *dw = tmpdword;
                return( s );
            }
        }
    #endif

    dw++;

    /*** Write in aligned 4-byte groups ***/
    for( ;; ) {
        dword = *dw;
        if( GOT_NIL(dword) )  break;
        tmpdword = 0;
        tmpchr = CHR1(dword);
        tmpdword |= TO_UPR_CHR1(tmpchr);
        tmpchr = CHR2(dword);
        tmpdword |= TO_UPR_CHR2(tmpchr);
        #ifndef __WIDECHAR__
            tmpchr = CHR3(dword);
            tmpdword |= TO_UPR_CHR3(tmpchr);
            tmpchr = CHR4(dword);
            tmpdword |= TO_UPR_CHR4(tmpchr);
        #endif
        *dw++ = tmpdword;
    }

    /*** Write in last dword ***/
    #ifdef __WIDECHAR__
        if( !CHR1(dword) ) {
            return( s );
        } else {
            tmpdword = SKIP_CHRS(dword,1);
            tmpchr = CHR1(dword);
            tmpdword |= TO_UPR_CHR1(tmpchr);
            *dw = tmpdword;
            return( s );
        }
    #else
        if( !CHR1(dword) ) {
            return( s );
        } else if( !CHR2(dword) ) {
            tmpdword = SKIP_CHRS(dword,1);
            tmpchr = CHR1(dword);
            tmpdword |= TO_UPR_CHR1(tmpchr);
            *dw = tmpdword;
            return( s );
        } else if( !CHR3(dword) ) {
            tmpdword = SKIP_CHRS(dword,2);
            tmpchr = CHR1(dword);
            tmpdword |= TO_UPR_CHR1(tmpchr);
            tmpchr = CHR2(dword);
            tmpdword |= TO_UPR_CHR2(tmpchr);
            *dw = tmpdword;
            return( s );
        } else {
            tmpdword = SKIP_CHRS(dword,3);
            tmpchr = CHR1(dword);
            tmpdword |= TO_UPR_CHR1(tmpchr);
            tmpchr = CHR2(dword);
            tmpdword |= TO_UPR_CHR2(tmpchr);
            tmpchr = CHR3(dword);
            tmpdword |= TO_UPR_CHR3(tmpchr);
            *dw = tmpdword;
            return( s );
        }
    #endif
}
