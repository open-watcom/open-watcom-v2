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


#include <string.h>
#include <ctype.h>
#include "scan.h"
#include "ytab.gh"
#include "hash1.h"
#include "hash2.h"
#include "keyword.h"
#ifdef UNIX
    #include "clibext.h"
#endif

typedef struct HashEntry {
    char *  keyword;
    int     token;
} HashEntry;

/* create the hash table by using the a pick macro and the keywords?.gh files */
#define pick( a, b, c ) {b, c}

static const HashEntry HashTable1[] = {
    #include "keyword1.gh"
};
static const HashEntry HashTable2[] = {
    #include "keyword2.gh"
};

#undef pick

int LookupKeyword( ScanString newstring )
/***************************************/
/* The first character of the string indicates which hash function and table */
/* to use: a-i use first function; j-z use second hash function */
{
    int     hash;

    if (tolower(newstring.string[0]) <= 'i') {
        hash = KeywordHash1( newstring.length, newstring.string );
        if (! stricmp( newstring.string, HashTable1[ hash ].keyword) ) {
            return( HashTable1[ hash ].token );
        } else {
            return( Y_NAME );
        }
    } else {
        hash = KeywordHash2( newstring.length, newstring.string );
        if (! stricmp( newstring.string, HashTable2[ hash ].keyword) ) {
            return( HashTable2[ hash ].token );
        } else {
            return( Y_NAME );
        }
    }
}
