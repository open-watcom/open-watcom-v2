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


#include <stdarg.h>
#include "wio.h"
#include "global.h"
#include "rcmem.h"
#include "iortns.h"
#include "wresset2.h"
#include "rclayer0.h"
#include "rccore.h"

#include "clibext.h"


#define MAX_OPEN_FILES          100

#define RC_MAX_FILES            20
#define RC_BUFFER_SIZE          0x4000   /* 16k */

typedef struct RcBuffer {
    size_t      Count;          /* number of characters in buffer */
    size_t      BytesRead;
    char        *NextChar;
    bool        IsDirty;
    char        Buffer[RC_BUFFER_SIZE];
} RcBuffer;
/* The buffer is layed out as follows:

    if IsDirty is true last operation was a write
        <----------Count--------->
      Buffer                  NextChar
        |                         |
        +---------------------------------------------------------+
        |////////////////////////|                                |
        |////////////////////////|                                |
        +---------------------------------------------------------+
        ^
       DOS (where dos thinks the file is)

    if IsDirty is false last operation was a read (or the buffer is empty if
    Count == 0)
                                 <--------------Count------------->
      Buffer                   NextChar
        |                         |
        +---------------------------------------------------------+
        |                        |////////////////////////////////|
        |                        |////////////////////////////////|
        +---------------------------------------------------------+
                                                                  ^
                                                                 DOS
*/

typedef struct RcFileEntry {
    bool        HasRcBuffer;
    WResFileID  fid;            // If WRES_NIL_HANDLE, entry is unused
    RcBuffer    *Buffer;        // If NULL, entry is a normal file (not used yet)
} RcFileEntry;

HANDLE_INFO     hInstance;

bool            RcIoNoBuffer = false;

static WResFileID       openFileList[MAX_OPEN_FILES];
static RcFileEntry      RcFileList[RC_MAX_FILES];

static RcBuffer *NewRcBuffer( void )
/**********************************/
{
    RcBuffer *  new_buff;

    new_buff = RcMemMalloc( sizeof( RcBuffer ) );
    new_buff->IsDirty = false;
    new_buff->Count = 0;
    new_buff->BytesRead = 0;
    new_buff->NextChar = new_buff->Buffer;
    memset( new_buff->Buffer, 0, RC_BUFFER_SIZE );

    return( new_buff );
} /* NewRcBuffer */

static void RegisterOpenFile( WResFileID fid )
/********************************************/
{
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] == WRES_NIL_HANDLE ) {
            openFileList[i] = fid;
            break;
        }
    }
}

static void UnRegisterOpenFile( WResFileID fid )
/**********************************************/
{
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] == fid ) {
            openFileList[i] = WRES_NIL_HANDLE;
            break;
        }
    }
}

/* Find index in RcFileList table of given filehandle.
*  Return: RC_MAX_FILES if not found, else index
*/
static int RcFindIndex( WResFileID fid )
/**************************************/
{
    int     i;

    for( i = 0; i < RC_MAX_FILES; i++ ) {
        if( RcFileList[i].fid == fid && RcFileList[i].HasRcBuffer ) {
            break;
        }
    }
    return( i );
} /* RcFindIndex */

void CloseAllFiles( void ) {
/***************************/
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] != WRES_NIL_HANDLE ) {
            RcClose( openFileList[i] );
        }
    }
}

WResFileID RcOpen( const char * file_name, wres_open_mode omode )
/***************************************************************/
{
    WResFileID  fid;
    int         i;

    switch( omode ) {
    default:
    case WRES_OPEN_RO:
        fid = WRES_PH2FID( open( file_name, O_BINARY | O_RDONLY ) );
        break;
    case WRES_OPEN_RW:
        fid = WRES_PH2FID( open( file_name, O_BINARY | O_RDWR | O_CREAT, PMODE_RW ) );
        break;
    case WRES_OPEN_NEW:
        fid = WRES_PH2FID( open( file_name, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW ) );
        break;
    }
    if( fid != WRES_NIL_HANDLE ) {
        RegisterOpenFile( fid );
        if( !RcIoNoBuffer ) {
            for( i = 0; i < RC_MAX_FILES; i++ ) {
                if( !RcFileList[i].HasRcBuffer ) {
                    RcFileList[i].HasRcBuffer = true;
                    RcFileList[i].fid = fid;
                    RcFileList[i].Buffer = NewRcBuffer();
                    break;
                }
            }
        }
    }
    return( fid );

} /* RcOpen */

static bool FlushRcBuffer( WResFileID fid, RcBuffer *buff )
/*********************************************************/
{
    bool    error;

    error = false;
    if( buff->IsDirty ) {
        error = ( (size_t)posix_write( WRES_FID2PH( fid ), buff->Buffer, buff->Count ) != buff->Count );
        memset( buff->Buffer, 0, RC_BUFFER_SIZE );
    }

    buff->IsDirty = false;
    buff->Count = 0;
    buff->BytesRead = 0;
    buff->NextChar = buff->Buffer;
    return( error );

} /* FlushRcBuffer */

int RcClose( WResFileID fid )
/***************************/
{
    RcBuffer    *buff;
    int         i;

    i = RcFindIndex( fid );
    if( i < RC_MAX_FILES ) {
        buff = RcFileList[i].Buffer;
        if( buff->IsDirty ) {
            if( FlushRcBuffer( fid, buff ) ) {
                return( -1 );
            }
        }
        RcMemFree( buff );
        RcFileList[i].HasRcBuffer = false;
        RcFileList[i].fid = WRES_NIL_HANDLE;
        RcFileList[i].Buffer = NULL;
    }
    UnRegisterOpenFile( fid );
    return( close( WRES_FID2PH( fid ) ) );

} /* RcClose */

WResFileSSize RcWrite( WResFileID fid, const void *out_buff, WResFileSize size )
/******************************************************************************/
{
    RcBuffer    *buff;
    size_t      copy_bytes;
    size_t      total_wrote;
    int         i;

    i = RcFindIndex( fid );
    if( i >= RC_MAX_FILES ) {
        return( posix_write( WRES_FID2PH( fid ), out_buff, size ) );
    }

    buff = RcFileList[i].Buffer;

    /* this is in case we have just read from the file */
    if( !buff->IsDirty ) {
        if( FlushRcBuffer( fid, buff ) ) {
            return( -1 );
        }
    }

    total_wrote = 0;
    while( size > 0 ) {
        copy_bytes = RC_BUFFER_SIZE - buff->Count;
        if( copy_bytes > size )
            copy_bytes = size;
        memcpy( buff->NextChar, out_buff, copy_bytes );
        buff->IsDirty = true;

        buff->NextChar += copy_bytes;
        buff->Count += copy_bytes;
        out_buff = (char *)out_buff + copy_bytes;
        size -= copy_bytes;
        total_wrote += copy_bytes;

        if( buff->Count == RC_BUFFER_SIZE ) {
            if( FlushRcBuffer( fid, buff ) ) {
                return( -1 );
            }
        }
    }

    return( total_wrote );
} /* RcWrite */

static WResFileSSize FillRcBuffer( WResFileID fid, RcBuffer * buff )
/******************************************************************/
{
    buff->Count = posix_read( WRES_FID2PH( fid ), buff->Buffer, RC_BUFFER_SIZE );
    if( buff->Count == -1 ) {
        buff->Count = 0;
        buff->BytesRead = 0;
        return( -1 );
    }
    buff->BytesRead = buff->Count;
    buff->NextChar = buff->Buffer;

    return( buff->Count );
} /* FillRcBuffer */

WResFileSSize RcRead( WResFileID fid, void * in_buff, WResFileSize size )
/***********************************************************************/
{
    RcBuffer        *buff;
    size_t          copy_bytes;
    size_t          total_read;
    int             i;
    WResFileSSize   bytes_added;        /* return value of FillRcBuffer */

    if( hInstance.fid == fid ) {
        return( posix_read( WRES_FID2PH( fid ), in_buff, size ) );
    }
    i = RcFindIndex( fid );
    if( i >= RC_MAX_FILES ) {
        return( posix_read( WRES_FID2PH( fid ), in_buff, size ) );
    }

    buff = RcFileList[i].Buffer;

    if( buff->IsDirty ) {
        if( FlushRcBuffer( fid, buff ) ) {
            return( -1 );
        }
    }

    total_read = 0;
    while( size > 0 ) {
        if( buff->Count == 0 ) {
            bytes_added = FillRcBuffer( fid, buff );
            if( bytes_added == -1 ) {
                return( bytes_added );
            } else if( bytes_added == 0 ) {
                return( total_read );
            }
        }
        copy_bytes = size;
        if( copy_bytes > buff->Count )
            copy_bytes = buff->Count;
        memcpy( in_buff, buff->NextChar, copy_bytes );

        buff->NextChar += copy_bytes;
        buff->Count -= copy_bytes;
        in_buff = (char *)in_buff + copy_bytes;
        size -= copy_bytes;
        total_read += copy_bytes;
    }

    return( total_read );
} /* RcRead */

WResFileOffset RcSeek( WResFileID fid, WResFileOffset amount, int where )
/***********************************************************************/
/* Note: Don't seek backwards in a buffer that has been writen to without */
/* flushing the buffer and doing an lseek since moving the NextChar pointer */
/* back will make it look like less data has been writen */
{
    RcBuffer        *buff;
    WResFileOffset  currpos;
    int             diff;
    int             i;

    if( hInstance.fid == fid ) {
        if( where == SEEK_SET ) {
            return( lseek( WRES_FID2PH( fid ), amount + WResFileShift, where ) - WResFileShift );
        } else {
            return( lseek( WRES_FID2PH( fid ), amount, where ) );
        }
    }
    i = RcFindIndex( fid );
    if( i >= RC_MAX_FILES ) {
        return( lseek( WRES_FID2PH( fid ), amount, where ) );
    }

    buff = RcFileList[i].Buffer;

    currpos = RcTell( fid );

    if( buff->IsDirty ) {
        switch( where ) {
        case SEEK_CUR:
            amount += currpos;
            where = SEEK_SET;
            /* FALL THROUGH */
        case SEEK_SET:
            /* if we are seeking backwards any amount or forwards past the */
            /* end of the buffer */
            if( amount < currpos || amount >= currpos + ( RC_BUFFER_SIZE - buff->Count ) ) {
                if( FlushRcBuffer( fid, buff ) )
                    return( -1 );
                if( lseek( WRES_FID2PH( fid ), amount, SEEK_SET ) == -1 ) {
                    return( -1 );
                }
            } else {
                diff = amount - currpos;
                /* add here because Count is chars to left of NextChar */
                /* for writing */
                buff->NextChar += diff;
                buff->Count += diff;
            }
            break;
        case SEEK_END:
            if( FlushRcBuffer( fid, buff ) )
                return( -1 );
            if( lseek( WRES_FID2PH( fid ), amount, where ) == -1 )
                return( -1 );
            break;
        default:
            return( -1 );
            break;
        }
    } else {
        switch( where ) {
        case SEEK_CUR:
            amount += currpos;
            where = SEEK_SET;
            /* FALL THROUGH */
        case SEEK_SET:
            /* if the new pos is outside the buffer */
            if( amount < currpos + buff->Count - buff->BytesRead || amount >= currpos + buff->Count ) {
                if( FlushRcBuffer( fid, buff ) )
                    return( -1 );
                if( lseek( WRES_FID2PH( fid ), amount, SEEK_SET ) == -1 ) {
                    return( -1 );
                }
            } else {
                diff = amount - currpos;
                /* subtract here because Count is chars to right of NextChar */
                /* for reading */
                buff->Count -= diff;
                buff->NextChar += diff;
            }
            break;
        case SEEK_END:
            if( FlushRcBuffer( fid, buff ) )
                return( -1 );
            if( lseek( WRES_FID2PH( fid ), amount, SEEK_END ) == -1 )
                return( -1 );
            break;
        default:
            return( -1 );
            break;
        }
    }

    return( currpos );
} /* RcSeek */

WResFileOffset RcTell( WResFileID fid )
/*************************************/
{
    RcBuffer *  buff;
    int         i;

    if( hInstance.fid == fid ) {
        return( tell( WRES_FID2PH( fid ) ) );
    }
    i = RcFindIndex( fid );
    if( i >= RC_MAX_FILES ) {
        return( tell( WRES_FID2PH( fid ) ) );
    }

    buff = RcFileList[i].Buffer;

    if( buff->IsDirty ) {
        return( tell( WRES_FID2PH( fid ) ) + (WResFileOffset)buff->Count );
    } else {
        return( tell( WRES_FID2PH( fid ) ) - (WResFileOffset)buff->Count );
    }
} /* RcTell */

void Layer0InitStatics( void )
/***********************************/
{
    int     i;

    for( i = 0; i < RC_MAX_FILES; i++ ) {
        RcFileList[i].HasRcBuffer = false;
        RcFileList[i].fid = WRES_NIL_HANDLE;
    }
    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        openFileList[i] = WRES_NIL_HANDLE;
    }
}
