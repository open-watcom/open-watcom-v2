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


#include <stdlib.h>
#include <ctype.h>
#include "wresall.h"
#include "rcstr.h"
#ifndef INSIDE_WLINK
#include "rcmem.h"
#endif

static int RemoveRedundantStrings( void ** strlist, unsigned int num,
                    int (* compare) (const void **, const void **) )
/********************************************************************/
/* strlist is a SORTED array of char *'s of size num */
{
    void **     curr;       /* element being examined */
    void **     last;       /* last unique element */

    curr = strlist;
    last = strlist - 1;     /* this is incremented in first iteration */

    while( curr < strlist + num ) {
        last++;
        *last = *curr;      /* this works on the first iteration */
        while( curr < strlist + num
               && compare( (const void **)curr, (const void **)last ) == 0 ) {
            curr++;
        }
    }

    return( last - strlist + 1 );
} /* RemoveRedundantStrings */


static int SortAndRemoveRedundantStrings( void ** strlist, unsigned int num,
                            int (* compare) (const void **, const void **) )
/***************************************************************************/
/* strlist is an array of char *'s of size num */
{
    qsort( (void *)strlist, (size_t)num, sizeof(void *),
           (int(*)(const void *, const void *))compare );
    return( RemoveRedundantStrings( strlist, num, compare ) );
} /* SortAndRemoveRedundantStrings */

static int InitStringList( WResDir dir, void ** list, int len )
/*************************************************************/
/* The list will be a list of pointers to WResIDName's */
{
    WResDirWindow   wind;
    WResTypeInfo *  type;
    WResResInfo *   res;
    void **         element;

    wind = WResFirstResource( dir );
    element = list;

    while (!WResIsEmptyWindow( wind )) {
        if (WResIsFirstResOfType( wind )) {
            type = WResGetTypeInfo( wind );
            if (type->TypeName.IsName) {
                if (len <= 0) return( 0 );      /* should never occur */
                *element = &(type->TypeName.ID.Name);
                element++;
                len--;
            }
        }
        if( WResIsFirstLangOfRes( wind ) ) {
            res = WResGetResInfo( wind );
            if (res->ResName.IsName) {
                if (len <= 0) return( 0 );      /* should never occur */
                *element = &(res->ResName.ID.Name);
                element++;
                len--;
            }
        }
        wind = WResNextResource( wind, dir );
    }

    return( element - list );
} /* InitStringList */

static void * MemUprCpy( void * dst, const void * src, size_t length )
/********************************************************************/
{
    char *          c_dst;
    const char *    c_src;

    for (c_dst = dst, c_src = src; length > 0; c_dst++, c_src++, length--) {
        *c_dst = toupper( *c_src );
    }

    return( dst );
} /* MemUprCpy */

static void * MemUprCpyUni( void * dst, const void * src, size_t length )
/***********************************************************************/
{
    uint_16 *       c_dst;
    const char *    c_src;

    for (c_dst = dst, c_src = src; length > 0; c_dst++, c_src++, length--) {
        *c_dst = toupper( *c_src );
    }

    return( dst );
} /* MemUprCpy */

static void CopyString( void **nextstr, WResIDName **name, int use_unicode )
/****************************************************************************/
{
    WResIDName          *currname;
    StringItem16        *name16;
    StringItem32        *name32;

    currname = *name;
    *name = *nextstr;

    if( use_unicode ) {
        name32 = *nextstr;
        name32->NumChars = currname->NumChars;
        MemUprCpyUni( &name32->Name, &currname->Name, currname->NumChars );
        *nextstr = (uint_8 *)(*nextstr) + 2 * ( currname->NumChars - 1 )
                                        + sizeof( StringItem32 );
    } else {
        name16 = *nextstr;
        name16->NumChars = currname->NumChars;
        MemUprCpy( &name16->Name, &currname->Name, currname->NumChars );
        *nextstr = (uint_8 *)(*nextstr) + currname->NumChars - 1
                                        + sizeof( StringItem16 );
    }
} /* CopyString */

static void ConstructStringBlock( StringBlock * str )
/***************************************************/
/* the string list should be filled in when this is called */
{
    char *          nextstring;     /* make this a char * so we can add by */
                                    /* by bytes */
    WResIDName      *currname;
    unsigned        i;
    unsigned        cnt;

    /* calculate the size of the block needed */
    str->StringBlockSize = 0;
#if(0) //may 24 1996 DRW
    for (currname = str->StringList;
                currname < str->StringList + str->StringListLen;
                currname++) {
        if( str->UseUnicode ) {
            str->StringBlockSize += sizeof( StringItem32 )
                                    + 2 * ((**currname).NumChars - 1);
        } else {
            str->StringBlockSize += sizeof( StringItem16 )
                                    + (**currname).NumChars - 1;
        }
    }
#else
    cnt = str->StringListLen;
    for( i=0; i < cnt; i++ ) {
        currname = str->StringList[i];
        if( str->UseUnicode ) {
            str->StringBlockSize += sizeof( StringItem32 )
                                    + 2 * ((*currname).NumChars - 1);
        } else {
            str->StringBlockSize += sizeof( StringItem16 )
                                    + (*currname).NumChars - 1;
        }
    }
#endif

    /* allocate the block for the strings */
    str->StringBlock = RcMemMalloc( str->StringBlockSize );

    /* copy the strings into the block */
    nextstring = str->StringBlock;
#if(0) //may 24 1996 DRW
    for (currname = str->StringList; currname < str->StringList
                + str->StringListLen; currname++) {
        CopyString( &nextstring, currname, str->UseUnicode );
    }
#else
    cnt = str->StringListLen;
    for( i=0; i < cnt; i++ ) {
        currname = str->StringList[i];
        str->StringList[i] = nextstring;
        CopyString( &nextstring, &currname, str->UseUnicode );
    }
#endif
} /* ConstructStringBlock */

static int CompareWResIDNames( const void ** name1, const void ** name2 )
/*************************************************************************/
{
    return( WResIDNameCmp( *name1, *name2 ) );
} /* CompareWResIDNames */

extern void StringBlockBuild( StringBlock * str, WResDir dir, int use_unicode )
/*****************************************************************************/
{
    int     list_len;
    void ** new_list;

    if (WResIsEmpty( dir )) {
        /* Empty directories are allowed since windows exe's may contain */
        /* no resources */
    } else {
        /* set the initial list_len to be the max possible */
        list_len = WResGetNumTypes( dir ) + WResGetNumResources( dir );
        str->UseUnicode = use_unicode;
        str->StringList = RcMemMalloc( list_len * sizeof(void *) );

        list_len = InitStringList( dir, str->StringList, list_len );
        list_len = SortAndRemoveRedundantStrings( str->StringList,
                    list_len, CompareWResIDNames );
        str->StringListLen = list_len;

        if( list_len == 0 ) {
            RcMemFree( str->StringList );
            str->StringList = NULL;
            str->StringBlock = NULL;
            str->StringBlockSize = 0;
        } else {
            new_list = RcMemRealloc( str->StringList, list_len * sizeof(void *) );
            if( new_list != NULL ) {
                str->StringList = new_list;
            }
            ConstructStringBlock( str );
        }
    }
} /* StringBlockBuild */

static int genericCompare( const char *name1, uint_16 len1,
                           const char *name2, uint_16 len2, int use_unicode ) {
/**************************************************************************/
    int                 char_num;
    uint_16             ch1;
    uint_16             ch2;
    const char          *char1;
    const char          *char2;
    uint_16             *char2u;
    uint_16             min_chars;

    min_chars = min( len1, len2 );
    char_num = 0;

    if( !use_unicode ) {
        char1 = name1;
        char2 = name2;
        while( char_num < min_chars ) {
            ch1 = toupper( *char1 );
            ch2 = toupper( *char2 );
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
            ch1 = toupper( *char1 );
            ch2 = toupper( *char2u );
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

    /* Longer names with the same prefix are greater */
    if( len1 < len2 ) {
        return( -1 );
    } else if( len1 > len2 ) {
        return( 1 );
    } else {
        return( 0 );
    }
}

static int compareStrings16( const WResIDName *key,
                             const StringItem16 **item ) {
/********************************************************************/

    return( genericCompare( key->Name, key->NumChars,
                            (*item)->Name, (*item)->NumChars, FALSE ) );
}

static int compareStrings32( const WResIDName *key,
                             const StringItem32 **item ) {
/**********************************************************************/
    return( genericCompare( key->Name, key->NumChars,
                            (*item)->Name, (*item)->NumChars, TRUE ) );
}

int CompareStringItems32( const StringItem32 *item1,
                          const StringItem32 *item2 ) {
/********************************************************************/
    uint_16     *ptr1;
    uint_16     *ptr2;
    uint_16      ch1;
    uint_16      ch2;
    uint_16      min_chars;
    int          char_num;

    char_num = 0;
    ptr1 = (uint_16 *)item1->Name;
    ptr2 = (uint_16 *)item2->Name;
    min_chars = min( item1->NumChars, item2->NumChars );
    while( char_num < min_chars ) {
        ch1 = toupper( *ptr1 );
        ch2 = toupper( *ptr2 );
        if( ch1 < ch2 ) {
            return( -1 );
        } else if( ch1 > ch2 ) {
            return( 1 );
        }
        char_num++;
        ptr1++;
        ptr2++;
    }
    /* Longer names with the same prefix are greater */
    if( item1->NumChars < item2->NumChars ) {
        return( -1 );
    } else if( item1->NumChars > item2->NumChars ) {
        return( 1 );
    } else {
        return( 0 );
    }
}

extern int_32 StringBlockFind( StringBlock * str, WResIDName * name )
/*******************************************************************/
/* if the return code is -1 the name was not found, otherwise it is the */
/* number of bytes into the StringBlock at which to find the name. */
{
    uint_8 **       location;

    if( str->UseUnicode ) {
        location = bsearch( name, str->StringList, str->StringListLen,
                    sizeof(void *),
                    ( int (*)(const void *, const void *) )compareStrings32 );
    } else {
        location = bsearch( name, str->StringList, str->StringListLen,
                    sizeof(void *),
                    ( int (*)(const void *, const void *) )compareStrings16 );
    }

    if( location == NULL ) {
        return( -1 );
    } else {
        return( *location - (uint_8 *)str->StringBlock );
    }
}
