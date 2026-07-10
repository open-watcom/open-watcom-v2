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
* Description:  String resource management.
*
****************************************************************************/


#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include "wresall.h"
#include "rcstrblk.h"
#include "rcstr.h"
#include "rcrtns.h"
#include "memfuncs.h"


#if !defined( INSIDE_WLINK ) || defined( _OS2 )
static size_t RemoveRedundantStrings( name_ptr *strlist, size_t num,
                    int (*compare)(const void *, const void *) )
/****************************************************************
 * strlist is a SORTED array of char *'s of size num
 */
{
    name_ptr        *curr;      /* element being examined */
    name_ptr        *last;      /* last unique element */

    curr = strlist;
    last = strlist - 1;     /* this is incremented in first iteration */

    while( curr < strlist + num ) {
        last++;
        *last = *curr;      /* this works on the first iteration */
        while( curr < strlist + num
               && compare( (const void *)curr, (const void *)last ) == 0 ) {
            curr++;
        }
    }

    return( last - strlist + 1 );
} /* RemoveRedundantStrings */


static size_t SortAndRemoveRedundantStrings( name_ptr *strlist, size_t num,
                            int (*compare)(const void *, const void *) )
/***********************************************************************
 * strlist is an array of char *'s of size num
 */
{
    qsort( (void *)strlist, num, sizeof( void * ), compare );
    return( RemoveRedundantStrings( strlist, num, compare ) );
} /* SortAndRemoveRedundantStrings */

static size_t InitStringList( WResDir dir, name_ptr *list, size_t len )
/*******************************************************************
 * The list will be a list of pointers to WResIDName's
 */
{
    WResDirWindow   wind;
    WResTypeInfo    *typeinfo;
    WResResInfo     *resinfo;
    name_ptr        *element;

    element = list;
    for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
        if( WResIsFirstResOfType( wind ) ) {
            typeinfo = WResGetTypeInfo( wind );
            if( typeinfo->TypeName.IsName ) {
                if( len == 0 )
                    return( 0 );      /* should never occur */
                element->u.name_id = &(typeinfo->TypeName.ID.Name);
                element++;
                len--;
            }
        }
        if( WResIsFirstLangOfRes( wind ) ) {
            resinfo = WResGetResInfo( wind );
            if( resinfo->ResName.IsName ) {
                if( len == 0 )
                    return( 0 );      /* should never occur */
                element->u.name_id = &(resinfo->ResName.ID.Name);
                element++;
                len--;
            }
        }
    }

    return( element - list );
} /* InitStringList */

static char *StrUprCopy( char *dst, const char *src, unsigned length )
/********************************************************************/
{
    *dst++ = length;
    while( length-- > 0 ) {
        *dst++ = toupper( *(unsigned char *)src );
        src++;
    }
    return( dst );
}

static char *StrUprCopyToUni( char *dst, const char *src, unsigned length )
/*************************************************************************/
{
    MPUT_16( dst, (uint_16)length );
    dst += sizeof( uint_16 );
    while( length-- > 0 ) {
        *dst++ = toupper( *(unsigned char *)src );
        *dst++ = '\0';
        src++;
    }
    return( dst );
}

static void ConstructStringIDNamesBlock( StringsBlock *str )
/****************************************************
 * the string list should be filled in when this is called
 */
{
    char            *nextstring;
    WResIDName      *name_id;
    unsigned        i;
    unsigned        cnt;
    uint_16         len;

    /*
     * calculate the size of the block needed
     */
    str->StringBlockSize = 0;
    cnt = str->StringListLen;
    for( i = 0; i < cnt; i++ ) {
        len = str->StringList[i].u.name_id->NumChars + 1;
        if( str->UseUnicode ) {
            str->StringBlockSize += 2 * len;
        } else {
            str->StringBlockSize += len;
        }
    }
    /*
     * allocate the block for the strings
     */
    str->StringBlock = MemAllocSafe( str->StringBlockSize );
    /*
     * copy the strings into the block
     */
    nextstring = str->StringBlock;
    cnt = str->StringListLen;
    for( i = 0; i < cnt; i++ ) {
        name_id = str->StringList[i].u.name_id;
        str->StringList[i].u.str = nextstring;
        if( str->UseUnicode ) {
            nextstring = StrUprCopyToUni( nextstring, name_id->Name, name_id->NumChars );
        } else {
            nextstring = StrUprCopy( nextstring, name_id->Name, name_id->NumChars );
        }
    }
} /* ConstructStringIDNamesBlock */

static int CompareWResIDNames( const void *n1, const void *n2 )
/*************************************************************/
{
    return( WResIDNameCmp( *(const WResIDName **)(n1), *(const WResIDName **)(n2) ) );
}

void StringIDNamesBlockBuild( StringsBlock *str, WResDir dir, bool use_unicode )
/***********************************************************************/
{
    size_t      list_len;
    name_ptr    *new_list;

    if( WResIsEmpty( dir ) ) {
        /*
         * Empty directories are allowed since windows exe's may contain
         * no resources
         */
    } else {
        /*
         * set the initial list_len to be the max possible
         */
        list_len = dir->NumTypes + dir->NumResources;
        str->UseUnicode = use_unicode;
        str->StringList = MemAllocSafe( list_len * sizeof( void * ) );

        list_len = InitStringList( dir, str->StringList, list_len );
        list_len = SortAndRemoveRedundantStrings( str->StringList, list_len, CompareWResIDNames );
        str->StringListLen = (uint_16)list_len;

        if( list_len == 0 ) {
            MemFree( str->StringList );
            str->StringList = NULL;
            str->StringBlock = NULL;
            str->StringBlockSize = 0;
        } else {
            new_list = MemReallocSafe( str->StringList, list_len * sizeof( void * ) );
            str->StringList = new_list;
            ConstructStringIDNamesBlock( str );
        }
    }
} /* StringIDNamesBlockBuild */

static int genericCompare( const char *name1, uint_16 len1,
                           const char *name2, uint_16 len2, bool use_unicode )
/****************************************************************************/
{
    int                 char_num;
    const char          *char1;
    const char          *char2;
    uint_16             *char2u;
    uint_16             min_chars;
    int                 ch1;
    int                 ch2;

    min_chars = len1;
    if( min_chars > len2 )
        min_chars = len2;
    char_num = 0;

    if( !use_unicode ) {
        char1 = name1;
        char2 = name2;
        while( char_num < min_chars ) {
            ch1 = toupper( *(unsigned char *)char1 );
            ch2 = toupper( *(unsigned char *)char2 );
            if( ch1 < ch2 ) {
                return( -1 );
            } else if( ch1 > ch2 ) {
                return( 1 );
            }
            char_num++;
            char1++;
            char2++;
        }
    } else {
        char1 = name1;
        char2u = (uint_16 *)name2;
        while( char_num < min_chars ) {
            ch1 = toupper( *(unsigned char *)char1 );
            ch2 = toupper( *(unsigned char *)char2u );
            if( ch1 < ch2 ) {
                return( -1 );
            } else if( ch1 > ch2 ) {
                return( 1 );
            }
            char_num++;
            char1++;
            char2u++;
        }
    }
    /*
     * Longer names with the same prefix are greater
     */
    if( len1 < len2 ) {
        return( -1 );
    } else if( len1 > len2 ) {
        return( 1 );
    } else {
        return( 0 );
    }
}

static int compareStrings16( const WResIDName *key,
                             const StringItem16 **item )
/******************************************************/
{
    return( genericCompare( key->Name, key->NumChars,
                            (*item)->Name, (*item)->NumChars, false ) );
}

static int compareStrings32( const WResIDName *key,
                             const StringItem32 **item )
/******************************************************/
{
    return( genericCompare( key->Name, key->NumChars,
                            (*item)->Name, (*item)->NumChars, true ) );
}

int CompareStringItems32( const StringItem32 *item1,
                          const StringItem32 *item2 )
/***************************************************/
{
    uint_16     *ptr1;
    uint_16     *ptr2;
    int         ch1;
    int         ch2;
    uint_16     min_chars;
    int         char_num;

    char_num = 0;
    ptr1 = (uint_16 *)item1->Name;
    ptr2 = (uint_16 *)item2->Name;
    min_chars = item1->NumChars;
    if( min_chars > item2->NumChars )
        min_chars = item2->NumChars;
    while( char_num < min_chars ) {
        ch1 = toupper( *(unsigned char *)ptr1 );
        ch2 = toupper( *(unsigned char *)ptr2 );
        if( ch1 < ch2 ) {
            return( -1 );
        } else if( ch1 > ch2 ) {
            return( 1 );
        }
        char_num++;
        ptr1++;
        ptr2++;
    }
    /*
     * Longer names with the same prefix are greater
     */
    if( item1->NumChars < item2->NumChars ) {
        return( -1 );
    } else if( item1->NumChars > item2->NumChars ) {
        return( 1 );
    } else {
        return( 0 );
    }
}

int_32 StringBlockFind( StringsBlock *str, WResIDName *name_id )
/***************************************************************
 * if the return code is -1 the name was not found, otherwise it is the
 * number of bytes into the StringBlock at which to find the name.
 */
{
    uint_8          **location;

    if( str->UseUnicode ) {
        location = bsearch( name_id, str->StringList, str->StringListLen,
                    sizeof( void * ),
                    ( int (*)(const void *, const void *) )compareStrings32 );
    } else {
        location = bsearch( name_id, str->StringList, str->StringListLen,
                    sizeof( void * ),
                    ( int (*)(const void *, const void *) )compareStrings16 );
    }

    if( location == NULL ) {
        return( -1 );
    } else {
        return( (int_32)( *location - (uint_8 *)str->StringBlock ) );
    }
}
#endif
