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


#include <wwindows.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wresall.h"
#include "wrstrdup.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

static int getUniStringLength( WResIDName *id, int is32bit )
{
    char        *str;
    int         len;

    if( is32bit ) {
        str = WRWResIDNameToStr( id );
        if( str != NULL ) {
            len = 0;
            WRmbcs2unicode( str, NULL, &len );
            MemFree( str );
        } else {
            len = 2;
        }
    } else {
        len = 1;
        if( id != NULL ) {
            len += id->NumChars;
        }
    }

    return( len );
}

static int WRCalcStringBlockSize( StringTableBlock *block, int is32bit )
{
    int         size;
    int         i;

    size = 0;

    if( block != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            size += getUniStringLength( block->String[i], is32bit );
        }
    }

    return( size );
}

static char *copyNULLWResIDNameToData( char *data, int is32bit )
{
    if( is32bit ) {
        *(uint_16 *)data = (uint_16)0;
        data = (char *)data + sizeof( uint_16 );
    } else {
        *(uint_8 *)data = (uint_8)0;
        data = (char *)data + sizeof( uint_8 );
    }

    return( data );
}

static char *copyWResIDNameToData( char *data, WResIDName *name, int is32bit )
{
    char        *str;
    char        *new_str;
    int         len;

    if( name == NULL ) {
        return( copyNULLWResIDNameToData( data, is32bit ) );
    }

    if( is32bit ) {
        new_str = NULL;
        str = WRWResIDNameToStr( name );
        if( str != NULL ) {
            WRmbcs2unicode( str, &new_str, &len );
            MemFree( str );
        }

        if( new_str == NULL ) {
            return( copyNULLWResIDNameToData( data, is32bit ) );
        }

        // write the length of the string
        *(uint_16 *)data = (uint_16)(len / 2 - 1);
        data += sizeof( uint_16 );

        // write the string
        data = WRCopyString( data, new_str, len - 2 );

        MemFree( new_str );
    } else {
        // write the length of the string
        *(uint_8 *)data = name->NumChars;
        data += sizeof( uint_8 );

        // write the string
        data = WRCopyString( data, &name->Name[0], name->NumChars );
    }

    return( data );
}

static int WRInitDataFromBlock( StringTableBlock *block, void *data, int size, int is32bit )
{
    int         i;
    int         dsize;
    void        *orig_data;

    if( block != NULL ) {
        orig_data = data;
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            data = copyWResIDNameToData( (char *)data, block->String[i], is32bit );
        }
        dsize = (char *)data - (char *)orig_data;
    }

    return( dsize == size );
}

int WRAPI WRIsBlockEmpty( StringTableBlock *block )
{
    int         i;

    if( block != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->String[i] != NULL ) {
                return( FALSE );
            }
        }
    }

    return( TRUE );
}

int WRAPI WRMakeDataFromStringBlock( StringTableBlock *block, void **data,
                                         int *size, int is32bit )
{
    if( data != NULL && size != NULL ) {
        *size = WRCalcStringBlockSize( block, is32bit );
        if( *size != 0 ) {
            *data = MemAlloc( *size );
            if( *data != NULL ) {
                if( WRInitDataFromBlock( block, *data, *size, is32bit ) ) {
                    return( TRUE );
                } else {
                    MemFree( *data );
                    *data = NULL;
                    *size = 0;
                }
            } else {
                *size = 0;
            }
        } else {
            *data = NULL;
        }
    }

    return( FALSE );
}

int WRAPI WRMakeStringBlockFromData( StringTableBlock *block, void *data,
                                         int size, int is32bit )
{
    char        *text;
    char        *uni_str;
    char        *str;
    int         str_len;
    int         dsize;
    int         i;
    int         tlen;

    if( data == NULL || size == 0 || block == NULL ) {
        return( FALSE );
    }

    i = 0;
    dsize = size;

    while( i < STRTABLE_STRS_PER_BLOCK && dsize > 0 ) {
        text = (char *)data;
        if( is32bit ) {
            tlen = (int)*(uint_16 *)text;
            tlen *= 2;
            text += sizeof( uint_16 );
            dsize -= tlen + sizeof( uint_16 );
        } else {
            tlen = (int)*(uint_8 *)text;
            text += sizeof( uint_8 );
            dsize -= tlen + sizeof( uint_8 );
        }
        if( tlen != 0 ) {
            if( is32bit ) {
                str = NULL;
                str_len = 0;
                uni_str = (char *)MemAlloc( tlen + 2 );
                if( uni_str != NULL ) {
                    memcpy( uni_str, text, tlen );
                    uni_str[tlen] = '\0';
                    uni_str[tlen + 1] = '\0';
                    WRunicode2mbcs( uni_str, &str, &str_len );
                    MemFree( uni_str );
                }
            } else {
                str_len = tlen + 1;
                str = MemAlloc( str_len );
                memcpy( str, text, tlen );
                str[tlen] = '\0';
            }

            if( str == NULL ) {
                return( FALSE );
            }

            block->String[i] = WResIDNameFromStr( str );
            if( block->String[i] == NULL ) {
                return( FALSE );
            }

            text += tlen;
        } else {
            block->String[i] = NULL;
        }
        data = text;
        i++;
    }

    return( dsize >= 0 );
}

// copy strings in b2 into b1
int WRAPI WRMergeStringBlock( StringTableBlock *b1, StringTableBlock *b2, int replace )
{
    int         i;
    int         size;

    if( b1 == NULL || b2 == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( b2->String[i] == NULL ) {
            continue;
        }
        if( b1->String[i] != NULL ) {
            if( !replace ) {
                continue;
            }
            MemFree( b1->String[i] );
            b1->String[i] = NULL;
        }
        size = b2->String[i]->NumChars;
        b1->String[i] = MemAlloc( size + sizeof( uint_8 ) );
        if( b1->String[i] == NULL ) {
            return( FALSE );
        }
        memcpy( b1->String[i]->Name, b2->String[i]->Name, size );
        b1->String[i]->NumChars = size;
    }

    return( TRUE );
}

int WRAPI WRMergeStringData( void **s1, int *sz1, void *s2, int sz2,
                                 int is32bit, int replace )
{
    StringTableBlock    b1;
    StringTableBlock    b2;
    void                *new_data;
    int                 new_size;
    int                 ok;

    ok = (s1 != NULL && *s1 != NULL && s2 != NULL && sz1 != NULL && *sz1 != 0 && sz2 != 0);

    if( ok ) {
        ResInitStringTableBlock( &b1 );
        ResInitStringTableBlock( &b2 );
        ok = WRMakeStringBlockFromData( &b1, *s1, *sz1, is32bit );
    }

    if( ok ) {
        ok = WRMakeStringBlockFromData( &b2, s2, sz2, is32bit );
    }

    if( ok ) {
        ok = WRMergeStringBlock( &b1, &b2, replace );
    }

    if( ok ) {
        new_data = NULL;
        new_size = 0;
        ok = WRMakeDataFromStringBlock( &b1, &new_data, &new_size, is32bit );
    }

    if( ok )  {
        MemFree( *s1 );
        *s1 = new_data;
        *sz1 = new_size;
    }

    ResFreeStringTableBlock( &b1 );
    ResFreeStringTableBlock( &b2 );

    return( ok );
}
