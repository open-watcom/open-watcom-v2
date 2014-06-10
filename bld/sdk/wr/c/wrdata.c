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
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <io.h>
#include <limits.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmsg.h"
#include "wresall.h"
#include "wrdatai.h"

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

int WRReadResData( WResFileID handle, BYTE *data, uint_32 length )
{
    uint_32     size;
    int         ok;

    size = 0;
    ok = (data != NULL && length != 0);
    while( ok && length - size > CHUNK_SIZE ) {
        ok = (read( handle, &data[size], CHUNK_SIZE ) == CHUNK_SIZE);
        size += CHUNK_SIZE;
    }
    if( ok && length - size != 0 ) {
        ok = (read( handle, &data[size], length - size ) == length - size);
    }

    return( ok );
}

int WRWriteResData( WResFileID handle, BYTE *data, uint_32 length )
{
    uint_32     size;
    int         ok;

    size = 0;
    ok = (data != NULL && length != 0);
    while( ok && length - size > CHUNK_SIZE ) {
        ok = (write( handle, &data[size], CHUNK_SIZE ) == CHUNK_SIZE);
        size += CHUNK_SIZE;
    }
    if( ok && length - size != 0 ) {
        ok = (write( handle, &data[size], length - size ) == length - size);
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
    int         ok;

    rdata = NULL;

    ok = (info != NULL && lnode != NULL);

    if( ok && lnode->data != NULL ) {
        return( WRCopyExistingData( lnode ) );
    }

    if( ok ) {
        ok = (lnode->Info.Length < UINT_MAX);
        if( !ok ) {
            WRDisplayErrorMsg( WR_RESTOOLARGE );
        }
    }

    if( ok ) {
        rdata = WRLoadResData( info->tmp_file, lnode->Info.Offset, lnode->Info.Length );
        ok = (rdata != NULL);
    }

    if( !ok ) {
        if( rdata != NULL ) {
            MemFree( rdata );
            rdata = NULL;
        }
    }

    return( rdata );
}

void * WRAPI WRLoadResData( char *file, uint_32 offset, uint_32 length )
{
    void        *data;
    WResFileID  handle;
    int         ok;

    data = NULL;
    handle = -1;

    ok = (file != NULL && length != 0);

    if( ok ) {
        ok = ((data = MemAlloc( length )) != NULL);
    }

    if( ok ) {
        ok = ((handle = ResOpenFileRO( file )) != -1);
    }

    if( ok ) {
        ok = ((ResSeek( handle, offset, SEEK_SET )) != -1);
    }

    if( ok ) {
        ok = WRReadResData( handle, (BYTE *)data, length );
    }

    if( handle != -1 ) {
        ResCloseFile( handle );
    }

    if( !ok ) {
        if( data != NULL ) {
            MemFree( data );
            data = NULL;
        }
    }

    return( data );
}

int WRAPI WRSaveDataToFile( char *file_name, BYTE *data, uint_32 length )
{
    WResFileID  file;
    int         ok;

    file = -1;

    ok = (file_name != NULL && data != NULL && length != 0);

    if( ok ) {
        file = open( file_name, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY,
                     S_IWRITE | S_IREAD );
        ok = (file != -1);
    }

    if( ok ) {
        ok = WRWriteResData( file, data, length );
    }

    if( file != -1 ) {
        ResCloseFile( file );
    }

    return( ok );
}
