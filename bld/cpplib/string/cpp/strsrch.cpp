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
#include "strsrch.def"

#define NOT_FOUND (-1)


extern "C" long StringSearch
    ( const char *  str
    , unsigned int  strlen
    , const char *  pattern
    , unsigned int  patlen )
/**************************/
{

// If the pattern is of length 0, return 0:

    if( patlen == 0 ) {
        return( 0 );
    }

// If the pattern is longer than the string, indicate NOT_FOUND:

    if( patlen > strlen ) {
        return( NOT_FOUND );
    }

// If the pattern is of length 1, use memchr:

    if( patlen == 1 ) {
        const char *  loc;

        loc = (const char *)memchr( str, *pattern, strlen );
        return( loc == NULL ? NOT_FOUND : loc - str );
    }

// Use the Boyer-Moore algorithm:

    {
        auto unsigned char  SkipTable[256];
             unsigned int   extra_patlen;   // pattern size > 255
             unsigned int   str_index;
             unsigned int   pat_index;
             unsigned int   skip;

// First, construct the SkipTable. For each character found in the pattern,
// the corresponding SkipTable entry contains the offset from the last
// character in the pattern of the LAST occurrence of that character
// in the pattern. For characters that do not occur in the pattern,
// the SkipTable entry contains the length of the pattern.

        if( patlen > 255 ) {
            extra_patlen = patlen - 255;
            patlen       = 255;
        } else {
            extra_patlen = 0;
        }
        memset( SkipTable, patlen, 256 );
        for( pat_index = 0; pat_index < patlen; ++pat_index ) {
            int c;

            c = (unsigned char) pattern[pat_index];
            SkipTable[c] = (unsigned char)(patlen - pat_index - 1);
        }

// Now, search for the string. Each comparison of "str" to "pattern" proceeds
// from right-to-left. When a mismatch occurs, the mismatched character is
// used to extract from SkipTable the amount to skip before attempting
// another match. If extra_patlen > 0, we use memcmp to compare the rest
// of the pattern.

        str_index = patlen;
        pat_index = patlen;
        for( ;; ) {
            if( pat_index == 0 ) {              // possible match?
                if( extra_patlen == 0 ) break;  // yes!
                if( memcmp( str + str_index + patlen,
                            pattern + patlen,
                            extra_patlen ) == 0 ) break;
                str_index += patlen + 1;       // advance only 1 char
                pat_index  = patlen;
            }
            --pat_index;
            --str_index;
            while( str[str_index] != pattern[pat_index] ) {
                skip       = SkipTable[str[str_index]];
                str_index += (patlen - pat_index > skip)
                                     ? (patlen - pat_index)
                                     : skip;
                if( str_index >= strlen - extra_patlen ) {
                    return( NOT_FOUND );
                }
                pat_index = patlen - 1;
            }
        }
        return( str_index );
    }
}
