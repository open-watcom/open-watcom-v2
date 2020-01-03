/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <limits.h>
#include "wrmsg.h"
#include "wresall.h"
#include "wrdatai.h"
#include "rcrtns.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define CHUNK_SIZE 0x7fff

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRReadResData( FILE *fp, BYTE *data, size_t length )
{
    bool        ok;
    size_t      numread;

    ok = ( data != NULL && length > 0 );
    for( numread = CHUNK_SIZE; ok && length > 0; length -= numread ) {
        if( numread > length )
            numread = length;
        ok = ( RESREAD( fp, data, numread ) == numread );
        data += numread;
    }
    return( ok );
}

bool WRWriteResData( FILE *fp, BYTE *data, size_t length )
{
    bool        ok;
    size_t      numwrite;

    ok = ( data != NULL && length > 0 );
    for( numwrite = CHUNK_SIZE; ok && length > 0; length -= numwrite ) {
        if( numwrite > length )
            numwrite = length;
        ok = ( RESWRITE( fp, data, numwrite ) == numwrite );
        data += numwrite;
    }
    return( ok );
}

void *WRCopyExistingData( WResLangNode *lnode )
{
    void       *rdata;

    if( lnode == NULL ) {
        return( NULL );
    }

    rdata = MemAlloc( lnode->Info.Length );
    if( rdata != NULL ) {
        memcpy( rdata, lnode->data, lnode->Info.Length );
    }

    return( rdata );
}

void * WRAPI WRCopyResData( WRInfo *info, WResLangNode *lnode )
{
    void        *rdata;
    bool        ok;

    rdata = NULL;

    ok = ( info != NULL && lnode != NULL );

    if( ok && lnode->data != NULL ) {
        return( WRCopyExistingData( lnode ) );
    }

    if( ok ) {
        ok = ( lnode->Info.Length < UINT_MAX );
        if( !ok ) {
            WRDisplayErrorMsg( WR_RESTOOLARGE );
        }
    }

    if( ok ) {
        rdata = WRLoadResData( info->tmp_file, lnode->Info.Offset, lnode->Info.Length );
        ok = ( rdata != NULL );
    }

    if( !ok ) {
        if( rdata != NULL ) {
            MemFree( rdata );
            rdata = NULL;
        }
    }

    return( rdata );;
}

void * WRAPI WRLoadResData( const char *fname, uint_32 offset, size_t length )
{
    char        *data;
    FILE        *fh;
    bool        ok;
    char        *buf;

    data = NULL;
    fh = NULL;

    ok = ( fname != NULL && length > 0 );

    if( ok ) {
        ok = ( (data = MemAlloc( length )) != NULL );
    }

    if( ok ) {
        ok = ( (fh = fopen( fname, "rb" )) != NULL );
    }

    if( ok ) {
        ok = ( fseek( fh, offset, SEEK_SET ) == 0 );
    }

    buf = data;
    while( ok && length > CHUNK_SIZE ) {
        ok = ( fread( buf, 1, CHUNK_SIZE, fh ) == CHUNK_SIZE );
        buf += CHUNK_SIZE;
        length -= CHUNK_SIZE;
    }
    if( ok && length > 0 ) {
        ok = ( fread( buf, 1, length, fh ) == length );
    }

    if( fh != NULL ) {
        fclose( fh );
    }

    if( !ok ) {
        if( data != NULL ) {
            MemFree( data );
            data = NULL;
        }
    }

    return( data );
}

bool WRAPI WRSaveResDataToFile( const char *fname, BYTE *data, size_t length )
{
    FILE        *fh;
    bool        ok;

    fh = NULL;
    ok = ( fname != NULL && data != NULL && length != 0 );

    if( ok ) {
        ok = ( (fh = fopen( fname, "wb" )) != NULL );
    }

    while( ok && length > CHUNK_SIZE ) {
        ok = ( fwrite( data, 1, CHUNK_SIZE, fh ) == CHUNK_SIZE );
        data += CHUNK_SIZE;
        length -= CHUNK_SIZE;
    }
    if( ok && length > 0 ) {
        ok = ( fwrite( data, 1, length, fh ) == length );
    }

    if( fh != NULL ) {
        fclose( fh );
    }

    return( ok );
}
