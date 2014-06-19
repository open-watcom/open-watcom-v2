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


#ifdef __header1
#   include __header1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wio.h"
#include "sopen.h"
#include "wreslang.h"
#include "lsspec.h"
#include "encodlng.h"
#include "intlload.h"


static uint_16 const sigCheck[] = {
    #define LS_DEF( name, sig ) sig ,
    LS_DEFS
    #undef LS_DEF
};

#define LS_DEF( name, sig )     static int do##name( IntlData *, _LocaleItem * );
LS_DEFS
#undef LS_DEF

static int (*codeDispatch[])( IntlData *, _LocaleItem * ) = {
    #define LS_DEF( name, sig ) do##name,
    LS_DEFS
    #undef LS_DEF
};

static int doErrors( IntlData *data, _LocaleItem *item )
{
    LocaleErrors *errors_item = (LocaleErrors*)item;
    unsigned count;
    int skip;
    char *p;
    char **init;

    count = errors_item->number;
    data->errors_count = count;
    data->errors_text = _MemoryAllocate( count * sizeof( char * ) );
    if( data->errors_text == NULL ) {
        return( 1 );
    }
    init = data->errors_text;
    p = errors_item->data;
    for(;;) {
        skip = *(uint_8*)p;
        *p++ = '\0';
        if( skip == 0 || skip >= 0x80 ) break;
        *init++ = p;
        --count;
        p += skip;
    }
    if( count != 0 ) {
        return( 1 );
    }
    return( 0 );
}

static int doUsage( IntlData *data, _LocaleItem *item )
{
    LocaleUsage *usage_item = (LocaleUsage*)item;

    data->usage_text = usage_item->data;
    return( 0 );
}

static int doNULL( IntlData *data, _LocaleItem *item )
{
    data = data;
    item = item;
    return( -1 );
}

IntlData *getData( int fh ) {
    unsigned raw_size;
    LocaleData intl_header;
    IntlData *data;
    char *raw_data;
    int i;
    uint_32 delta;

    if( read( fh, &intl_header, sizeof( intl_header ) ) != sizeof( intl_header ) ) {
        close( fh );
        return( NULL );
    }
    if( intl_header.signature != LSF_SIGNATURE ) {
        return( NULL );
    }
    if( intl_header.major != LSF_MAJOR ) {
        return( NULL );
    }
    if( intl_header.minor != LSF_MINOR ) {
        return( NULL );
    }
    if( intl_header.units != LS_MAX ) {
        return( NULL );
    }
    delta = intl_header.offset[0];
    for( i = 0; i <= LS_MAX; ++i ) {
        intl_header.offset[i] -= delta;
    }
    raw_size = intl_header.offset[LS_MAX];
    data = _MemoryAllocate( sizeof( IntlData ) + raw_size );
    if( data == NULL ) {
        return( data );
    }
    data->errors_count = 0;
    data->errors_text = NULL;
    data->usage_text = NULL;
    raw_data = (char *)( data + 1 );
    if( (unsigned)read( fh, raw_data, raw_size ) != raw_size ) {
        return( data );
    }
    for( i = 0; i < LS_MAX; ++i ) {
        _LocaleItem *item = (void*)( raw_data + intl_header.offset[i] );
        if( item->code != i ) {
            return( data );
        }
        if( item->signature != sigCheck[i] ) {
            return( data );
        }
        if( codeDispatch[ i ]( data, item ) ) {
            return( data );
        }
    }
    return( data );
}

static char *imageName( char *buff )
{
#if defined( __SW_BD )
#if defined(__NT__) || ( defined(__OS2__) && ! defined(__OSI__) )
    {
        extern _crtn char *_LpDllName;
        if( _LpDllName != NULL ) {
            return( strcpy( buff, _LpDllName ) );
        }
    }
#endif
#endif
    return( _cmdname( buff ) );
}

IntlData *LoadInternationalData(
    const char *file_prefix )
{
    IntlData *data;
    int fh;
    res_language_enumeration language;
    size_t len;
    char *drive;
    char *dir;
    char cmd_name[_MAX_PATH];
    char split_buff[_MAX_PATH2];
    char base[16];

    language = _WResLanguage();
    if( language == RLE_ENGLISH ) {
        return( NULL );
    }
    if( imageName( cmd_name ) == NULL ) {
        return( NULL );
    }
    _splitpath2( cmd_name, split_buff, &drive, &dir, NULL, NULL );
    len = strlen( file_prefix );
    if( len > 6 ) {
        len = 6;
    }
    memcpy( base, file_prefix, len );
    base[len++] = '0';
    base[len++] = '0' + language;
    base[len] = '\0';
    _makepath( cmd_name, drive, dir, base, "." LOCALE_DATA_EXT );
    fh = sopen3( cmd_name, O_RDONLY | O_BINARY, SH_DENYWR );
    if( fh == -1 ) {
        return( NULL );
    }
    data = getData( fh );
    if( data != NULL ) {
        if( data->usage_text == NULL || data->errors_text == NULL ) {
            FreeInternationalData( data );
            data = NULL;
        }
    }
    close( fh );
    return( data );
}

void FreeInternationalData(
    IntlData *handle )
{
    if( handle != NULL ) {
        _MemoryFree( handle->errors_text );
        _MemoryFree( handle );
    }
}
