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
* Description:  Implementation of strncmp() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include <string.h>
#include "riscstr.h"


int __F_NAME(strncmp,wcsncmp)( const CHAR_TYPE *s1, const CHAR_TYPE *s2,
                               size_t n )
/**********************************************************************/
{
    RISC_DATA_LOCALREF;
    UINT                *dw1 = ROUND(s1); // round down to dword
    UINT                *dw2 = ROUND(s2);
    UINT                dword1, dword2;
    INT                 shr1, shr2, shl1, shl2;
    INT                 tmpchar, tmpdword1, tmpdword2;

#ifdef __WIDECHAR__
    if( OFFSET(s1) % 2  ||  OFFSET(s2) % 2 )
        return( __simple_wcsncmp( s1, s2, n ) );
#endif

    /*** Initialize locals ***/
    shr1 = OFFSET(s1) << 3; // sift right  = offset * 8
    shr2 = OFFSET(s2) << 3;
    shl1 = INT_SIZE - shr1; // shift left = 32 - shift right
    shl2 = INT_SIZE - shr2;

    if (shr1 != 0) {
        dword1 = *dw1++;
    }
    if (shr2 != 0) {
        dword2 = *dw2++;
    }

    /*** Scan in aligned 4-byte groups ***/
    for( ;; ) {
        if( n == 0 )
            return( 0 ); // nothing left to compare

        if( shr1 == 0 ) {
            tmpdword1 = *dw1++;
        } else {
            tmpdword1 = dword1 >> shr1;
            dword1 = *dw1++;
            tmpdword1 |= (dword1 << shl1);
        }

        if( shr2 == 0 ) {
            tmpdword2 = *dw2++;
        } else {
            tmpdword2 = dword2 >> shr2;
            dword2 = *dw2++;
            tmpdword2 |= (dword2 << shl2);
        }

        if( n < CHARS_PER_WORD ) {      // some bytes can be set to 00
            tmpdword1 = FRONT_CHRS(tmpdword1,n);
            tmpdword2 = FRONT_CHRS(tmpdword2,n);
            break;
        }

        /*** Did s1 end already? ***/
        if( GOT_NIL(tmpdword1) )
            break;
        /*** Are s1 and s2 still the same? ***/
#ifdef __WIDECHAR__
        if( tmpdword1 != tmpdword2 ) {
            tmpchar = CHR1(tmpdword1) - CHR1(tmpdword2);
            if( tmpchar )
                return( tmpchar );

            return( CHR2(tmpdword1) - CHR2(tmpdword2) );
        }
#else
        if( tmpdword1 != tmpdword2 ) {
            tmpchar = CHR1(tmpdword1) - CHR1(tmpdword2);
            if( tmpchar )
                return( tmpchar );

            tmpchar = CHR2(tmpdword1) - CHR2(tmpdword2);
            if( tmpchar )
                return( tmpchar );

            tmpchar = CHR3(tmpdword1) - CHR3(tmpdword2);
            if( tmpchar )
                return( tmpchar );

            return( CHR4(tmpdword1) - CHR4(tmpdword2) );
        }
#endif

        n -= CHARS_PER_WORD;
    }


    /*** Scan the last byte(s) in the string ***/
    if( tmpdword1 == tmpdword2 )
        return( 0 );

    /* we have a null char somewhere in the last dword */
#ifdef __WIDECHAR__
    tmpchar = CHR1(tmpdword1);
    if( tmpchar == 0 ) {                            /* 1st char is null */
        return( tmpchar - CHR1(tmpdword2) );
    }

    tmpchar = CHR1(tmpdword1) - CHR1(tmpdword2);    /* 2nd char is null */
    if( tmpchar )
        return( tmpchar );
    return( CHR2(tmpdword1) - CHR2(tmpdword2) );
#else
    tmpchar = CHR1(tmpdword1);
    if( tmpchar == 0 ) {            // first char in the dword is null
        return( tmpchar - CHR1(tmpdword2));
    }

    if( CHR2(tmpdword1) == 0 ) {     // second char in the dword is null
        tmpchar = tmpchar - CHR1(tmpdword2);
        if( tmpchar )
            return( tmpchar) ;

        return( CHR2(tmpdword1) - CHR2(tmpdword2) ) ;
    }

    if( CHR3(tmpdword1) == 0 ) {     // third char in the dword is null
        tmpchar = tmpchar - CHR1(tmpdword2);
        if( tmpchar )
            return( tmpchar) ;

        tmpchar = CHR2(tmpdword1) - CHR2(tmpdword2);
        if( tmpchar )
            return( tmpchar) ;

        return( CHR3(tmpdword1) - CHR3(tmpdword2) ) ;
    }

    // 4th char in the dword is null
    tmpchar = tmpchar - CHR1(tmpdword2);
    if( tmpchar )
        return( tmpchar );

    tmpchar = CHR2(tmpdword1) - CHR2(tmpdword2);
    if( tmpchar )
        return( tmpchar );

    tmpchar = CHR3(tmpdword1) - CHR3(tmpdword2);
    if( tmpchar )
        return( tmpchar );

    return( CHR4(tmpdword1) - CHR4(tmpdword2) );
#endif
}
