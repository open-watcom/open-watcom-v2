/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Keyword processing, all grammar variants.
*
****************************************************************************/

#include <string.h>
#include <ctype.h>
#if defined( GRAMMAR_OS2 )
    #include "yydrivr2.gh"
    #include "weights2.gh"
#elif defined( GRAMMAR_WIN16 )
    #include "yydrivr6.gh"
    #include "weightsw.gh"
#elif defined( GRAMMAR_WIN32 )
    #include "yydrivrw.gh"
    #include "weightsw.gh"
#endif
#include "kwhashic.h"

#include "clibext.h"


/*
 * NOTE:
 *
 * WIN16 and WIN32 use the same keyword list because keyword
 * processing is shared and keyword codes must be identical
 * for both grammars.
 *
 * Therefore, both use the same weightsw.gh and keywordw.gh
 * files. Keywords that are not valid for a particular grammar
 * are filtered by the appropriate LookupKeyword... function
 * below.
 */

/*
 * This array is intended to be accessed by ASCII values of a char
 */
static const unsigned char Weights[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x00 to 0x0f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x10 to 0x1f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* ' ' to '/' */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* '0' to '?' */
    0,          /* '@' */
    W_a,        /* 'A' */
    W_b,        /* 'B' */
    W_c,        /* 'C' */
    W_d,        /* 'D' */
    W_e,        /* 'E' */
    W_f,        /* 'F' */
    W_g,        /* 'G' */
    W_h,        /* 'H' */
    W_i,        /* 'I' */
    W_j,        /* 'J' */
    W_k,        /* 'K' */
    W_l,        /* 'L' */
    W_m,        /* 'M' */
    W_n,        /* 'N' */
    W_o,        /* 'O' */
    W_p,        /* 'P' */
    W_q,        /* 'Q' */
    W_r,        /* 'R' */
    W_s,        /* 'S' */
    W_t,        /* 'T' */
    W_u,        /* 'U' */
    W_v,        /* 'V' */
    W_w,        /* 'W' */
    W_x,        /* 'X' */
    W_y,        /* 'Y' */
    W_z,        /* 'Z' */
    0,          /* '[' */
    0,          /* '\' */
    0,          /* ']' */
    0,          /* '^' */
    W__,        /* '_' */
    0,          /* '`' */
    W_a,        /* 'a' */
    W_b,        /* 'b' */
    W_c,        /* 'c' */
    W_d,        /* 'd' */
    W_e,        /* 'e' */
    W_f,        /* 'f' */
    W_g,        /* 'g' */
    W_h,        /* 'h' */
    W_i,        /* 'i' */
    W_j,        /* 'j' */
    W_k,        /* 'k' */
    W_l,        /* 'l' */
    W_m,        /* 'm' */
    W_n,        /* 'n' */
    W_o,        /* 'o' */
    W_p,        /* 'p' */
    W_q,        /* 'q' */
    W_r,        /* 'r' */
    W_s,        /* 's' */
    W_t,        /* 't' */
    W_u,        /* 'u' */
    W_v,        /* 'v' */
    W_w,        /* 'w' */
    W_x,        /* 'x' */
    W_y,        /* 'y' */
    W_z,        /* 'z' */
    0,          /* '{' */
    0,          /* '|' */
    0,          /* '}' */
    0,          /* '~' */
    0,          /* 0x7f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x80 to 0x8f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x90 to 0x9f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xa0 to 0xaf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xb0 to 0xbf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xc0 to 0xcf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xd0 to 0xdf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xe0 to 0xef */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0xf0 to 0xff */
};

typedef struct HashEntry {
    const char  *keyword;
    YYTOKENTYPE token;
} HashEntry;

#define TC_UNKNOWN  ((YYTOKENTYPE)-1)

/*
 * create the hash table by using the a pick macro and the keywords?.gh files
 */
static const HashEntry HashTable[] = {
    #define pick( a, b, c ) {b, c},
#if defined( GRAMMAR_OS2 )
    #include "keyword2.gh"
#elif defined( GRAMMAR_WIN16 )
    #include "keywordw.gh"
#elif defined( GRAMMAR_WIN32 )
    #include "keywordw.gh"
#endif
    #undef pick
};

#if defined( GRAMMAR_OS2 )
YYTOKENTYPE LookupKeywordOS2( ScanString newstring )
#elif defined( GRAMMAR_WIN16 )
YYTOKENTYPE LookupKeywordW16( ScanString newstring )
#elif defined( GRAMMAR_WIN32 )
YYTOKENTYPE LookupKeywordWIN( ScanString newstring )
#endif
/********************************************************/
{
    int         hash;
    YYTOKENTYPE token;

    hash = keyword_hash( newstring.string, Weights, newstring.length );
    token = HashTable[hash].token;
    switch( token ) {
  #if defined( GRAMMAR_WIN16 )
    case Y_DIALOG_EX:
    case Y_DLGINIT:
    case Y_MENU_EX:
    case Y_TOOLBAR:
    case Y_LANGUAGE:
    case Y_CHARACTERISTICS:
    case Y_VERSION:
  #elif defined( GRAMMAR_WIN32 )
    case Y_ERRTABLE:
  #endif
    case TC_UNKNOWN:
        break;
    default:
        if( stricmp( newstring.string, HashTable[hash].keyword ) == 0 ) {
            return( token );
        }
    }
    return( Y_NAME );
}
