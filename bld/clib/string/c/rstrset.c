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


#ifdef __WIDECHAR__
 wchar_t *_wcsset( wchar_t *s, wchar_t c )
#else
 char *strset( char *s, int c )
#endif
/****************************************/
{
    RISC_DATA_LOCALREF;
    UINT *           dw = ROUND(s);             /* round down to dword */
    UINT             dword, cdword, tmpdword;
    INT              offset = OFFSET(s);

    #ifdef __WIDECHAR__
        if( offset % 2 )  return( __simple__wcsset( s, c ) );
    #endif

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
            if( CHR1(dword) ) {
                tmpdword |= CHR1(cdword);
            } else {
                return( s );
            }
            /* fall through */
          default:              /* offset must equal 2 (no odd offsets) */
            if( CHR2(dword) ) {
                tmpdword |= CHR2(cdword);
                *dw = tmpdword;
            } else {
                *dw = tmpdword;
                return( s );
            }
        }
    #else
        switch( offset ) {
          case 0:
            if( CHR1(dword) ) {
                tmpdword |= CHR1(cdword);
            } else {
                return( s );
            }
            /* fall through */
          case 1:
            if( CHR2(dword) ) {
                tmpdword |= CHR2(cdword);
            } else {
                tmpdword |= SKIP_CHRS(dword,2);
                *dw = tmpdword;
                return( s );
            }
            /* fall through */
          case 2:
            if( CHR3(dword) ) {
                tmpdword |= CHR3(cdword);
            } else {
                tmpdword |= SKIP_CHRS(dword,3);
                *dw = tmpdword;
                return( s );
            }
            /* fall through */
          default:
            if( CHR4(dword) ) {
                tmpdword |= CHR4(cdword);
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
        *dw++ = cdword;

        dword = *dw;
        if( GOT_NIL(dword) )  break;
        *dw++ = cdword;
    }

    /*** Write in last dword ***/
    #ifdef __WIDECHAR__
        if( !CHR1(dword) ) {
            return( s );
        } else {
            *dw = ( SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1) );
            return( s );
        }
    #else
        if( !CHR1(dword) ) {
            return( s );
        } else if( !CHR2(dword) ) {
            *dw = ( SKIP_CHRS(dword,1) | FRONT_CHRS(cdword,1) );
            return( s );
        } else if( !CHR3(dword) ) {
            *dw = ( SKIP_CHRS(dword,2) | FRONT_CHRS(cdword,2) );
            return( s );
        } else {
            *dw = ( SKIP_CHRS(dword,3) | FRONT_CHRS(cdword,3) );
            return( s );
        }
    #endif
}
