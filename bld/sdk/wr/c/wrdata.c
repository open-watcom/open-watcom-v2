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


#include <windows.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <limits.h>
#include <string.h>

#include "wrglbl.h"
#include "wrmem.h"
#include "wrmsg.h"
#include "wresall.h"
#include "wrinfo.h"
#include "wrcmsg.h"
#include "wrdata.h"
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
    ok = ( data && length );
    while( ok && ( length - size ) > CHUNK_SIZE ) {
        ok = ( read( handle, &data[size], CHUNK_SIZE ) == CHUNK_SIZE );
        size += CHUNK_SIZE;
    }
    if( ok && ( length - size ) ) {
        ok = ( read( handle, &data[size], (length-size) ) == (length-size) );
    }

    return( ok );
}

int WRWriteResData( WResFileID handle, BYTE *data, uint_32 length )
{
    uint_32     size;
    int         ok;

    size = 0;
    ok = ( data && length );
    while( ok && ( length - size ) > CHUNK_SIZE ) {
        ok = ( write( handle, &data[size], CHUNK_SIZE ) == CHUNK_SIZE );
        size += CHUNK_SIZE;
    }
    if( ok && ( length - size ) ) {
        ok = ( write( handle, &data[size], (length-size) ) == (length-size) );
    }

    return( ok );
}

void *WRCopyExistingData( WResLangNode *lnode )
{
    void       *rdata;

    if( lnode == NULL ) {
        return( NULL );
    }

    rdata = WRMemAlloc( lnode->Info.Length );
    if( rdata ) {
        memcpy( rdata, lnode->data, lnode->Info.Length );
    }

    return ( rdata );
}

void * WR_EXPORT WRCopyResData( WRInfo *info, WResLangNode *lnode )
{
    void        *rdata;
    int         ok;

    rdata = NULL;

    ok = ( info && lnode );

    if( ok && lnode->data ) {
        return( WRCopyExistingData( lnode ) );
    }

    if( ok ) {
        ok = ( lnode->Info.Length < UINT_MAX );
        if( !ok ) {
            WRDisplayErrorMsg( WR_RESTOOLARGE );
        }
    }

    if( ok ) {
        rdata = WRLoadResData( info->tmp_file, lnode->Info.Offset,
                               lnode->Info.Length );
        ok = ( rdata != NULL );
    }

    if( !ok ) {
        if( rdata ) {
            WRMemFree( rdata );
            rdata = NULL;
        }
    }

    return( rdata );
}

void * WR_EXPORT WRLoadResData( char *file, uint_32 offset, uint_32 length )
{
    void        *data;
    WResFileID  handle;
    int         ok;

    data        = NULL;
    handle      = -1;

    ok = ( file && length );

    if( ok ) {
        ok = ( ( data = WRMemAlloc( length ) ) != NULL );
    }

    if( ok ) {
        ok = ( ( handle = ResOpenFileRO( file ) ) != -1 );
    }

    if( ok ) {
        ok = ( ( ResSeek( handle, offset, SEEK_SET ) ) != -1 );
    }

    if( ok ) {
        ok = WRReadResData( handle, (BYTE *)data, length );
    }

    if( handle != -1 ) {
        ResCloseFile( handle );
    }

    if( !ok ) {
        if( data ) {
            WRMemFree( data );
            data = NULL;
        }
    }

    return( data );
}

int WR_EXPORT WRSaveDataToFile( char *file_name, BYTE *data, uint_32 length  )
{
    WResFileID  file;
    int         ok;

    file = -1;

    ok = ( file_name && data && length );

    if( ok ) {
        file = open( file_name, O_CREAT | O_WRONLY | O_TRUNC |
                                O_BINARY, S_IWRITE | S_IREAD );
        ok = ( file  != -1 );
    }

    if( ok ) {
        ok = WRWriteResData( file, data, length );
    }

    if( file != -1 ) {
        ResCloseFile( file );
    }

    return( ok );
}

