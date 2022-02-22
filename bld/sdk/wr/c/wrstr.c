/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include "wresall.h"
#include "wrstrdup.h"
#include "wrutili.h"


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

static size_t getUniStringLength( WResIDName *id, bool is32bit )
{
    char        *str;
    size_t      len;

    if( is32bit ) {
        str = WRStringFromWResIDName( id );
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

static size_t WRCalcStringBlockSize( StringTableBlock *block, bool is32bit )
{
    size_t      size;
    int         i;

    size = 0;

    if( block != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            size += getUniStringLength( block->String[i], is32bit );
        }
    }

    return( size );
}

static char *copyNULLWResIDNameToData( char *data, bool is32bit )
{
    if( is32bit ) {
        VALU16( data ) = 0;
        INCU16( data );
    } else {
        VALU8( data ) = 0;
        INCU8( data );
    }

    return( data );
}

static char *copyWResIDNameToData( char *data, WResIDName *name, bool is32bit )
{
    char        *str;
    char        *new_str;
    size_t      len;

    if( name == NULL ) {
        return( copyNULLWResIDNameToData( data, is32bit ) );
    }

    if( is32bit ) {
        new_str = NULL;
        len = 0;
        str = WRStringFromWResIDName( name );
        if( str != NULL ) {
            WRmbcs2unicode( str, &new_str, &len );
            MemFree( str );
        }

        if( new_str == NULL ) {
            return( copyNULLWResIDNameToData( data, is32bit ) );
        }

        // write the length of the string
        VALU16( data ) = (uint_16)(len / 2 - 1);
        INCU16( data );

        // write the string
        data = WRCopyString( data, new_str, len - 2 );

        MemFree( new_str );
    } else {
        // write the length of the string
        VALU8( data ) = name->NumChars;
        INCU8( data );

        // write the string
        data = WRCopyString( data, &name->Name[0], name->NumChars );
    }

    return( data );
}

static bool WRInitDataFromBlock( StringTableBlock *block, char *data, size_t size, bool is32bit )
{
    int         i;
    size_t      dsize;
    void        *orig_data;

    dsize = 0;
    if( block != NULL ) {
        orig_data = data;
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            data = copyWResIDNameToData( (char *)data, block->String[i], is32bit );
        }
        dsize = (char *)data - (char *)orig_data;
    }

    return( dsize == size );
}

bool WRAPI WRIsBlockEmpty( StringTableBlock *block )
{
    int         i;

    if( block != NULL ) {
        for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->String[i] != NULL ) {
                return( false );
            }
        }
    }

    return( true );
}

bool WRAPI WRMakeDataFromStringBlock( StringTableBlock *block, char **pdata, size_t *psize, bool is32bit )
{
    if( pdata != NULL && psize != NULL ) {
        *psize = WRCalcStringBlockSize( block, is32bit );
        if( *psize != 0 ) {
            *pdata = MemAlloc( *psize );
            if( *pdata != NULL ) {
                if( WRInitDataFromBlock( block, *pdata, *psize, is32bit ) ) {
                    return( true );
                } else {
                    MemFree( *pdata );
                    *pdata = NULL;
                    *psize = 0;
                }
            } else {
                *psize = 0;
            }
        } else {
            *pdata = NULL;
        }
    }
    return( false );
}

bool WRAPI WRMakeStringBlockFromData( StringTableBlock *block, char *data, size_t size, bool is32bit )
{
    char        *text;
    char        *uni_str;
    char        *str;
    size_t      str_len;
    size_t      dsize;
    int         i;
    size_t      tlen;

    if( data == NULL || size == 0 || block == NULL ) {
        return( false );
    }

    i = 0;
    dsize = size;

    while( i < STRTABLE_STRS_PER_BLOCK && dsize > 0 ) {
        text = (char *)data;
        if( is32bit ) {
            tlen = 2 * VALU16( text );
            INCU16( text );
            dsize -= tlen + SIZEU16;
        } else {
            tlen = VALU8( text );
            INCU8( text );
            dsize -= tlen + SIZEU8;
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
                return( false );
            }

            block->String[i] = WResIDNameFromStr( str );
            if( block->String[i] == NULL ) {
                return( false );
            }

            text += tlen;
        } else {
            block->String[i] = NULL;
        }
        data = text;
        i++;
    }

    return( true );
}

// copy strings in b2 into b1
bool WRAPI WRMergeStringBlock( StringTableBlock *b1, StringTableBlock *b2, bool replace )
{
    int         i;
    uint_16     size;

    if( b1 == NULL || b2 == NULL ) {
        return( false );
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
            return( false );
        }
        memcpy( b1->String[i]->Name, b2->String[i]->Name, size );
        b1->String[i]->NumChars = size;
    }

    return( true );
}

bool WRAPI WRMergeStringData( char **s1, uint_32 *sz1, char *s2, uint_32 sz2, bool is32bit, bool replace )
{
    StringTableBlock    b1;
    StringTableBlock    b2;
    char                *new_data;
    size_t              new_size;
    bool                ok;

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
        *sz1 = (uint_32)new_size;
    }

    ResFreeStringTableBlock( &b1 );
    ResFreeStringTableBlock( &b2 );

    return( ok );
}
