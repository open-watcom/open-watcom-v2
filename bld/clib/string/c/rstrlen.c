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


size_t __F_NAME(strlen,wcslen)( const CHAR_TYPE *s )
/**************************************************/
{
    RISC_DATA_LOCALREF;
    int                 offset = OFFSET(s);
    UINT *              dw = ROUND(s);          /* round down to dword */
    UINT                dword;
    size_t              len = 0;

    #ifdef __WIDECHAR__
        if( offset % 2 )  return( __simple_wcslen( s ) );
    #endif

    /*** Scan until s is aligned ***/
    dword = *dw++;
    if( OFFSET_GOT_NIL(dword,offset) ) {
        #if USE_INT64
            switch( offset ) {
              case 0:
                if( !CHR1(dword) )  break;
                len++;
                /* fall through */
              case 1:
                if( !CHR2(dword) )  break;
                len++;
                /* fall through */
              case 2:
                if( !CHR3(dword) )  break;
                len++;
                /* fall through */
              case 3:
                if( !CHR4(dword) )  break;
                len++;
                /* fall through */
              case 4:
                if( !CHR5(dword) )  break;
                len++;
                /* fall through */
              case 5:
                if( !CHR6(dword) )  break;
                len++;
                /* fall through */
              case 6:
                if( !CHR7(dword) )  break;
                len++;
                /* fall through */
              default:
                break;
            }
        #else
            #ifdef __WIDECHAR__
                switch( offset ) {
                  case 0:
                    if( !CHR1(dword) )  break;
                    len++;
                    /* fall through */
                  default:              /* offset==2 (no odd offsets) */
                    break;
                }
            #else
                switch( offset ) {
                  case 0:
                    if( !CHR1(dword) )  break;
                    len++;
                    /* fall through */
                  case 1:
                    if( !CHR2(dword) )  break;
                    len++;
                    /* fall through */
                  case 2:
                    if( !CHR3(dword) )  break;
                    len++;
                    /* fall through */
                  default:
                    break;
                }
            #endif
        #endif
        return( len );
    } else {
        len += ( BYTES_PER_WORD - offset ) / CHARSIZE;
    }

    /*** Scan one word at a time until a null char is found ***/
    for( ;; ) {
        dword = *dw++;
        if( GOT_NIL(dword) )  break;

        dword = *dw++;
        if( GOT_NIL(dword) )  break;
    }

    /*** Locate the null char within the offending word ***/
    len = (CHAR_TYPE*)dw - s;
    #if USE_INT64
        if( !CHR1(dword) ) {
            len -= 8;
        } else if( !CHR2(dword) ) {
            len -= 7;
        } else if( !CHR3(dword) ) {
            len -= 6;
        } else if( !CHR4(dword) ) {
            len -= 5;
        } else if( !CHR5(dword) ) {
            len -= 4;
        } else if( !CHR6(dword) ) {
            len -= 3;
        } else if( !CHR7(dword) ) {
            len -= 2;
        } else {
            len -= 1;
        }
    #else
        #ifdef __WIDECHAR__
            if( !CHR1(dword) ) {
                len -= 2;
            } else {
                len -= 1;
            }
        #else
            if( !CHR1(dword) ) {
                len -= 4;
            } else if( !CHR2(dword) ) {
                len -= 3;
            } else if( !CHR3(dword) ) {
                len -= 2;
            } else {
                len -= 1;
            }
        #endif
    #endif

    return( len );
}
