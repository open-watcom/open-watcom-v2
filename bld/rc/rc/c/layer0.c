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
#include "layer0.h"
#include "iortns.h"
#include "wresset2.h"

#define MAX_OPEN_FILES          100

#define RC_MAX_FILES            20
#define RC_BUFFER_SIZE          0x4000   /* 16k */

typedef struct RcBuffer {
    int         Count;          /* number of characters in buffer */
    unsigned    BytesRead;
    char *      NextChar;
    int         IsDirty;
    char        Buffer[ RC_BUFFER_SIZE ];
} RcBuffer;
/* The buffer is layed out as follows:

    if IsDirty is TRUE last operation was a write
        <----------Count--------->
      Buffer                  NextChar
        |                         |
        +---------------------------------------------------------+
        |////////////////////////|                                |
        |////////////////////////|                                |
        +---------------------------------------------------------+
        ^
       DOS (where dos thinks the file is)

    if IsDirty is FALSE last operation was a read (or the buffer is empty if
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
    int         HasRcBuffer;
    int         FileHandle;      // If NIL_HANDLE, entry is unused
    RcBuffer    *Buffer;         // If NULL, entry is a normal file (not used yet)
} RcFileEntry;

HANDLE_INFO     Instance;

static WResFileID       openFileList[ MAX_OPEN_FILES ];
static RcFileEntry      RcFileList[ RC_MAX_FILES ];
static int              RcFindIndex( WResFileID fileno );

static RcBuffer * NewRcBuffer( void )
/***********************************/
{
    RcBuffer *  new_buff;

    new_buff = RcMemMalloc( sizeof(RcBuffer) );
    new_buff->IsDirty = FALSE;
    new_buff->Count = 0;
    new_buff->BytesRead = 0;
    new_buff->NextChar = new_buff->Buffer;
    memset( new_buff->Buffer, 0, RC_BUFFER_SIZE );

    return( new_buff );
} /* NewRcBuffer */

void RegisterOpenFile( WResFileID fhdl )
/**************************************/
{
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] == NIL_HANDLE ) {
            openFileList[i] = fhdl;
            break;
        }
    }
}

void UnRegisterOpenFile( WResFileID fhdl )
/****************************************/
{
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] == fhdl ) {
            openFileList[i] = NIL_HANDLE;
            break;
        }
    }
}

void CloseAllFiles( void ) {
/***************************/
    unsigned    i;

    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        if( openFileList[i] != NIL_HANDLE ) {
            RcClose( openFileList[i] );
        }
    }
}

WResFileID RcOpen( const char * file_name, int access, ... )
/**********************************************************/
{
    int         perms;
    va_list     args;
    WResFileID  fileno;
    int         i;

    if (access & O_CREAT) {
        va_start( args, access );
        perms = va_arg( args, int );
        va_end( args );
    } else {
        perms = 0;
    }

    fileno = open( file_name, access, perms );
    if( fileno != NIL_HANDLE ) {
        RegisterOpenFile( fileno );
        for( i = 0; i < RC_MAX_FILES; i++ ) {
            if( RcFileList[i].HasRcBuffer == FALSE) {
                RcFileList[i].HasRcBuffer = TRUE;
                RcFileList[i].FileHandle = fileno;
                RcFileList[i].Buffer = NewRcBuffer();
                break;
            }
        }
    }
    return( fileno );

} /* RcOpen */

static int FlushRcBuffer( int fileno, RcBuffer * buff )
/******************************************************/
{
    int     num_wrote;
    int     error;

    error = FALSE;
    if (buff->IsDirty) {
        num_wrote = write( fileno, buff->Buffer, buff->Count );
        error = (num_wrote != buff->Count);
        memset( buff->Buffer, 0, RC_BUFFER_SIZE );
    }

    buff->IsDirty = FALSE;
    buff->Count = 0;
    buff->BytesRead = 0;
    buff->NextChar = buff->Buffer;
    return( error );

} /* FlushRcBuffer */

int RcClose( WResFileID fileno )
/******************************/
{
    RcBuffer    *buff;
    int         error,
                i;

    i = RcFindIndex( fileno );
    if ( i < RC_MAX_FILES ) {
        buff = RcFileList[i].Buffer;
        if( buff->IsDirty ) {
            error = FlushRcBuffer( fileno, buff );
            if( error ) {
                return( -1 );
            }
        }
        RcMemFree( buff );
        RcFileList[i].HasRcBuffer = FALSE;
        RcFileList[i].FileHandle = NIL_HANDLE;
        RcFileList[i].Buffer = NULL;
    }
    UnRegisterOpenFile( fileno );
    return( close( fileno ) );

} /* RcClose */

WResFileSSize RcWrite( WResFileID fileno, const void *out_buff, WResFileSize size )
/*********************************************************************************/
{
    RcBuffer    *buff;
    int         copy_bytes;
    int         total_wrote;
    int         error;
    int         i;

    i = RcFindIndex( fileno );
    if( i >= RC_MAX_FILES ) {
        return( write( fileno, out_buff, size ) );
    }

    buff = RcFileList[i].Buffer;

    /* this is in case we have just read from the file */
    if( !buff->IsDirty ) {
        error = FlushRcBuffer( fileno, buff );
        if( error ) {
            return( -1 );
        }
    }

    total_wrote = 0;
    while (size > 0) {
        copy_bytes = min( size, RC_BUFFER_SIZE - buff->Count );

        memcpy( buff->NextChar, out_buff, copy_bytes );
        buff->IsDirty = TRUE;

        buff->NextChar += copy_bytes;
        buff->Count += copy_bytes;
        out_buff = (char *)out_buff + copy_bytes;
        size -= copy_bytes;
        total_wrote += copy_bytes;

        if (buff->Count == RC_BUFFER_SIZE) {
            error = FlushRcBuffer( fileno, buff );
            if( error ) {
                return( -1 );
            }
        }
    }

    return( total_wrote );
} /* RcWrite */

static int FillRcBuffer( int fileno, RcBuffer * buff )
/****************************************************/
{
    buff->Count = read( fileno, buff->Buffer, RC_BUFFER_SIZE );
    if (buff->Count == -1) {
        buff->Count = 0;
        return( -1 );
    }
    buff->BytesRead = buff->Count;
    buff->NextChar = buff->Buffer;

    return( buff->Count );
} /* FillRcBuffer */

WResFileSSize RcRead( WResFileID fileno, void * in_buff, WResFileSize size )
/**************************************************************************/
{
    RcBuffer    *buff;
    int         copy_bytes;
    int         total_read;
    int         error;
    int         i;
    int         bytes_added;        /* return value of FillRcBuffer */

    i = RcFindIndex( fileno );
    if( i >= RC_MAX_FILES ) {
        return( read( fileno, in_buff, size ) );
    }

    buff = RcFileList[i].Buffer;

    if (buff->IsDirty) {
        error = FlushRcBuffer( fileno, buff );
        if( error ) {
            return( -1 );
        }
    }

    total_read = 0;
    while (size > 0) {
        if (buff->Count == 0) {
            bytes_added = FillRcBuffer( fileno, buff );
            if( bytes_added < 0 ) {
                return( bytes_added );
            } else if( bytes_added == 0 ) {
                return( total_read );
            }
        }

        copy_bytes = min( size, buff->Count );

        memcpy( in_buff, buff->NextChar, copy_bytes );

        buff->NextChar += copy_bytes;
        buff->Count -= copy_bytes;
        in_buff = (char *)in_buff + copy_bytes;
        size -= copy_bytes;
        total_read += copy_bytes;
    }

    return( total_read );
} /* RcRead */

WResFileOffset RcSeek( WResFileID fileno, WResFileOffset amount, int where )
/**************************************************************************/
/* Note: Don't seek backwards in a buffer that has been writen to without */
/* flushing the buffer and doing an lseek since moving the NextChar pointer */
/* back will make it look like less data has been writen */
{
    RcBuffer    *buff;
    long        currpos;
    long        seek_rc;
    int         diff;
    int         error,
                i;

    i = RcFindIndex( fileno );
    if ( i >= RC_MAX_FILES ) {
        if( fileno == Instance.handle && where == SEEK_SET ) {
            return( lseek( fileno, amount + FileShift, where ) - FileShift );
        } else {
            return( lseek( fileno, amount, where ) );
        }
    }

    buff = RcFileList[i].Buffer;

    currpos = RcTell( fileno );

    if (buff->IsDirty) {
        switch (where) {
        case SEEK_CUR:
            amount += currpos;
            where = SEEK_SET;
            /* FALL THROUGH */
        case SEEK_SET:
            /* if we are seeking backwards any amount or forwards past the */
            /* end of the buffer */
            if (amount < currpos ||
                    amount >= currpos + (RC_BUFFER_SIZE - buff->Count)) {
                error = FlushRcBuffer( fileno, buff );
                if( error ) return( -1 );
                seek_rc = lseek( fileno, amount, SEEK_SET );
                if( seek_rc == -1 )  return( -1 );
            } else {
                diff = amount - currpos;
                /* add here because Count is chars to left of NextChar */
                /* for writing */
                buff->NextChar += diff;
                buff->Count += diff;
            }
            break;
        case SEEK_END:
            error = FlushRcBuffer( fileno, buff );
            if( error ) return( -1 );
            seek_rc = lseek( fileno, amount, where );
            if (seek_rc == -1)  return( -1 );
            break;
        default:
            return( -1 );
            break;
        }
    } else {
        switch (where) {
        case SEEK_CUR:
            amount += currpos;
            where = SEEK_SET;
            /* FALL THROUGH */
        case SEEK_SET:
            /* if the new pos is outside the buffer */
            if (amount < currpos + buff->Count - buff->BytesRead ||
                    amount >= currpos + buff->Count) {
                error = FlushRcBuffer( fileno, buff );
                if( error ) return( -1 );
                seek_rc = lseek( fileno, amount, SEEK_SET );
                if (seek_rc == -1) return( -1 );
            } else {
                diff = amount - currpos;
                /* subtract here because Count is chars to right of NextChar */
                /* for reading */
                buff->Count -= diff;
                buff->NextChar += diff;
            }
            break;
        case SEEK_END:
            error = FlushRcBuffer( fileno, buff );
            if( error ) return( -1 );
            seek_rc = lseek( fileno, amount, SEEK_END );
            if (seek_rc == -1) return( -1 );
            break;
        default:
            return( -1 );
            break;
        }
    }

    return( currpos );
} /* RcSeek */

WResFileOffset RcTell( WResFileID fileno )
/****************************************/
{
    RcBuffer *  buff;
    int         i;

    i = RcFindIndex( fileno );
    if ( i >= RC_MAX_FILES ) {
        return( tell( fileno ) );
    }

    buff = RcFileList[i].Buffer;

    if (buff->IsDirty) {
        return( tell( fileno ) + buff->Count );
    } else {
        return( tell( fileno ) - buff->Count );
    }
} /* RcTell */

void Layer0InitStatics( void )
/***********************************/
{
    int     i;

    for( i = 0; i < RC_MAX_FILES; i++ ) {
        RcFileList[i].HasRcBuffer = FALSE;
        RcFileList[i].FileHandle = NIL_HANDLE;
    }
    for( i = 0; i < MAX_OPEN_FILES; i++ ) {
        openFileList[i] = NIL_HANDLE;
    }
}

/* Find index in RcFileList table of given filehandle.
*  Return: RC_MAX_FILES if not found, else index
*/
static int RcFindIndex( WResFileID fileno )
/*****************************************/
{
    int     i;

    for( i = 0; i < RC_MAX_FILES; i++ ) {
        if( RcFileList[i].FileHandle == fileno && RcFileList[i].HasRcBuffer ) {
            break;
        }
    }
    return( i );
} /* RcFindIndex */

