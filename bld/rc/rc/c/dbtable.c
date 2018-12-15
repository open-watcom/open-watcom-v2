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


#include "global.h"
#include "dbtable.h"
#include "write.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


typedef struct {
    DBTableHeader       header;
    char                begchars[256];
    DBIndexEntry        *index;
    uint_16             *entries;
}DBCharInfo;

static DBCharInfo       charInfo;

static RcStatus readDBHeader( FILE *fh )
{
    size_t      numread;

    numread = fread( &charInfo.header, 1, sizeof( DBTableHeader ), fh );
    if( numread != sizeof( DBTableHeader ) ) {
        return( feof( fh ) ? RS_READ_INCMPLT : RS_READ_ERROR );
    }
    if( charInfo.header.sig[0] != DB_TABLE_SIG_1 ||
        charInfo.header.sig[1] != DB_TABLE_SIG_2 ) {
        return( RS_BAD_FILE_FMT );
    }
    if( charInfo.header.ver != DB_TABLE_VER ) {
        return( RS_WRONG_VER );
    }
    return( RS_OK );
}

static RcStatus readDBRanges( FILE *fh )
{
    size_t      numread;

    numread = fread( &charInfo.begchars, 1, 256, fh );
    if( numread != 256 ) {
        return( feof( fh ) ? RS_READ_INCMPLT : RS_READ_ERROR );
    }
    return( RS_OK );
}

static RcStatus readDBIndex( FILE *fh )
{
    size_t      numread;
    size_t      size;

    size = charInfo.header.num_indices * sizeof( DBIndexEntry );
    charInfo.index = RESALLOC( size );
    numread = fread( charInfo.index, 1, size, fh );
    if( numread != size ) {
        return( feof( fh ) ? RS_READ_INCMPLT : RS_READ_ERROR );
    }
    return( RS_OK );
}

static RcStatus readDBTable( FILE *fh )
{
    size_t      numread;
    size_t      size;

    size = charInfo.header.num_entries * sizeof( uint_16 );
    charInfo.entries = RESALLOC( size );
    numread = fread( charInfo.entries, 1, size, fh );
    if( numread != size ) {
        return( feof( fh ) ? RS_READ_INCMPLT : RS_READ_ERROR );
    }
    return( RS_OK );
}

RcStatus LoadCharTable( const char *fname, char *path )
{
    FILE        *fh;
    RcStatus    ret;

    ret = RS_OK;
    _searchenv( fname, "PATH", path );
    if( path[0] == '\0' )
        return( RS_FILE_NOT_FOUND );
    fh = fopen( path, "rb" );
    if( fh == NULL ) {
        ret = RS_OPEN_ERROR;
    }
    if( ret == RS_OK )
        ret = readDBHeader( fh );
    if( ret == RS_OK )
        ret = readDBRanges( fh );
    if( ret == RS_OK )
        ret = readDBIndex( fh );
    if( ret == RS_OK )
        ret = readDBTable( fh );
    if( ret != RS_OPEN_ERROR )
        fclose( fh );
    if( ret == RS_OK ) {
        ConvToUnicode = DBStringToUnicode;
    }
    return( ret );
}

static uint_16 lookUpDBChar( uint_16 ch ) {

    int         i;
    int         index;

    for( i = 0; i < charInfo.header.num_indices; i++ ) {
        if( ch >= charInfo.index[i].min && ch <= charInfo.index[i].max ) {
            index = charInfo.index[i].base + ch - charInfo.index[i].min;
            return( charInfo.entries[index] );
        }
    }
    return( (uint_16)-1 );
}

const char *GetLeadBytes( void ) {
    return( charInfo.begchars );
}

size_t DBStringToUnicode( size_t len, const char *str, char *buf ) {

    const uint_8    *ptr;
    const uint_8    *end;
    uint_16         *ubuf;
    uint_16         dbchar;
    size_t          ret;

    ret = 0;
    ubuf = (uint_16 *)buf;
    end = (uint_8 *)str + len;
    for( ptr = (uint_8 *)str; ptr < end; ptr++ ) {
        if( charInfo.begchars[*ptr] == DB_BEG_CHAR ) {
            dbchar = *ptr << 8;
            ptr++;
            dbchar |= *ptr;
        } else {
            dbchar = *ptr;
        }
        if( ubuf != NULL ) {
            *ubuf = lookUpDBChar( dbchar );
            ubuf++;
        }
        ret += sizeof( *ubuf );
    }
    return( ret );
}

void FreeCharTable( void )
/************************/
{
    if( charInfo.index != NULL ) {
        RESFREE( charInfo.index );
        charInfo.index = NULL;
    }
    if( charInfo.entries != NULL ) {
        RESFREE( charInfo.entries );
        charInfo.entries = NULL;
    }
}

extern void DbtableInitStatics( void )
/************************************/
{
    memset( &charInfo, 0, sizeof( DBCharInfo ) );
}
