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
* Description:  Implementation of strrchr() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <string.h>
#include "riscstr.h"


#ifdef __WIDECHAR__
 wchar_t *wcsrchr( const wchar_t *s, wint_t c )
#else
 char *strrchr( const char *s, int c )
#endif
/*********************************************/
{
    RISC_DATA_LOCALREF;
    UINT                *result = NULL;
    CHAR_TYPE           *result2 = NULL;
    UINT                *dw = ROUND(s); // round down to dword
    UINT                mask, dword, cdword, tmpdword;
    size_t              len = 0;
    int                 offset = OFFSET(s);
#ifdef __WIDECHAR__
    UINT                cShl16;
#else
    UINT                cShl8, cShl16, cShl24;
#endif

#ifdef __WIDECHAR__
    if( offset % 2 )
        return( __simple_wcsrchr( s, c ) );
#endif

    /*** If searching for '\0', use different technique ***/
    if( c == NULLCHAR )
        return( (CHAR_TYPE*)s + __F_NAME(strlen,wcslen)( s ) );

    /*** Initialize locals ***/
    c &= CHR1MASK;
#ifdef __WIDECHAR__
    cShl16 = c << 16;
    cdword = cShl16 | c;
#else
    cShl8 = c << 8;
    cShl16 = c << 16;
    cShl24 = c << 24;
    cdword = cShl24 | cShl16 | cShl8 | c;
#endif
    dword = *dw;

    /*** Scan any bytes up to a 4-byte alignment ***/
    if( OFFSET_GOT_NIL(dword,offset) ) { /* there is a null char in the first word */
        tmpdword = SKIP_CHRS(dword,offset/CHARSIZE) ^ cdword;
        if( GOT_NIL(tmpdword) ) {   /* c is in the first word */
#ifdef __WIDECHAR__
            if( offset == 0 ) {     /* no odd alignments */
                tmpdword = CHR1(dword);
                if( tmpdword == c ) {
                    result2 = (CHAR_TYPE*)s;
                } else if( tmpdword == 0 ) {
                    return( NULL );
                }
                len++;
            }
#else
            switch( offset ) {
              case 0:
                tmpdword = CHR1(dword);
                if( tmpdword == c ) {
                    result2 = (CHAR_TYPE*)s;
                } else if( tmpdword == 0 ) {
                    return( NULL );
                }
                len++;
                /* fall through */
              case 1:
                tmpdword = CHR2(dword);
                if( tmpdword == cShl8 ) {
                    result2 = (CHAR_TYPE*)s+len;
                } else if( tmpdword == 0 ) {
                    return( result2 );
                }
                len++;
                /* fall through */
              case 2:
                tmpdword = CHR3(dword);
                if( tmpdword == cShl16 ) {
                    return (CHAR_TYPE*)s+len;
                } else {
                    return( result2 );
                }
            }
#endif
        }
        return( result2 );
    } else {
        tmpdword = SKIP_CHRS(dword,offset/CHARSIZE) ^ cdword;
        if( GOT_NIL(tmpdword) ) { // c is in the first word
            result = dw;
            mask = SKIP_CHRS_MASKS(offset/CHARSIZE);
        }
    }

    /*** Scan in aligned 4-byte groups ***/
    for( ;; ) {
        dword = *(++dw);
        if( GOT_NIL(dword) )
            break;
        tmpdword = dword ^ cdword;
        if( GOT_NIL(tmpdword)) {
            result = dw;
            mask = SKIP_CHRS_MASKS(0);
        }

        dword = *(++dw);
        if( GOT_NIL(dword) )
            break;
        tmpdword = dword ^ cdword;
        if( GOT_NIL(tmpdword)) {
            result = dw;
            mask = SKIP_CHRS_MASKS(0);
        }
    }

    /*** Scan the last byte(s) in the string ***/

    /* we have a null char somewhere in the last dword */
#ifdef __WIDECHAR__
    if( CHR1(dword) ) { // first char in the dword is not null
        if( CHR1(dword) == c ) {
            return( (CHAR_TYPE*)dw );
        }
    }
#else
    if( CHR1(dword) ) { // first char in the dword is not null
        if( CHR2(dword) ) { // second char in the dword is not null
            if( CHR3(dword) ) { // third char in the dword is not null
                if ( ( CHR3(dword) ) == cShl16 ) {
                    return( ((char *)dw) + 2 );
                } else if ( ( CHR2(dword) ) == cShl8 ) {
                    return( ((char *)dw) + 1 );
                } else if ( ( CHR1(dword) ) == c ) {
                    return( (char *)dw );
                }
            } else {
                if ( ( CHR2(dword) ) == cShl8 ) {
                     return ((char *)dw)+1;
                } else if ( ( CHR1(dword) ) == c ) {
                     return( (char *)dw );
                }
            }
        } else {
            if ( ( CHR1(dword) ) == c ) {
                return( (char *)dw );
            }
        }
    }
#endif

    if( result != NULL ) { // we found a dword with c
        dword = *result;
        dword &= mask;
#ifdef __WIDECHAR__
        if( CHR2(dword) == cShl16 ) {
            return( ((CHAR_TYPE*)result) + 1 );
        }
#else
        if( CHR4(dword) == cShl24 ) {
            return( ((CHAR_TYPE*)result) + 3 );
        } else if( CHR3(dword) == cShl16 ) {
            return( ((CHAR_TYPE*)result) + 2 );
        } else if( CHR2(dword) == cShl8 ) {
            return( ((CHAR_TYPE*)result) + 1 );
        }
#endif
    }
    return( (CHAR_TYPE*)result );
}
