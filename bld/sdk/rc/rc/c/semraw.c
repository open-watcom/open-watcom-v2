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


#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "wresall.h"
#include "rcmem.h"
#include "errors.h"
#include "global.h"
#include "semantic.h"
#include "semraw.h"
#include "depend.h"
#include "iortns.h"

extern void SemWriteRawDataItem( RawDataItem item )
/*************************************************/
{
    uint_16   num;

    if( item.IsString) {
        if( ResWriteStringLen( item.Item.String, item.LongString,
                               CurrResFile.handle, item.StrLen ) ) {
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                     LastWresErrStr() );
            ErrorHasOccured = TRUE;
        }
        if( item.TmpStr ) RcMemFree( item.Item.String );
    } else {
        if( (int_32)item.Item.Num < 0 ) {
            if( (int_32)item.Item.Num < SHRT_MIN ) {
                RcError( ERR_RAW_DATA_TOO_SMALL, item.Item.Num, SHRT_MIN );
                ErrorHasOccured = TRUE;
            }
        } else {
            if( item.Item.Num > USHRT_MAX ) {
                RcError( ERR_RAW_DATA_TOO_BIG, item.Item.Num, USHRT_MAX );
                ErrorHasOccured = TRUE;
            }
        }
        if( !ErrorHasOccured ) {
            num = item.Item.Num;
            if( ResWriteUint16( &(num), CurrResFile.handle ) ) {
                RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                         LastWresErrStr() );
                ErrorHasOccured = TRUE;
            }
        }
    }

}

extern RcStatus SemCopyDataUntilEOF( long offset, int handle, void * buff,
                int buffsize, int *err_code )
/****************************************************************/
{
    int     error;
    int     numread;
    long    seekrc;

    seekrc = RcSeek( handle, offset, SEEK_SET );
    if (seekrc == -1) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    numread = RcRead( handle, buff, buffsize );
    while( numread != 0 ) {
        if (numread == -1) {
            *err_code = errno;
            return( RS_READ_ERROR );
        }
        error = ResWrite( buff, numread, CurrResFile.handle );
        if (error) {
            *err_code = LastWresErr();
            return( RS_WRITE_ERROR );
        }
        numread = RcRead( handle, buff, buffsize );
    }

    return( FALSE );
}

#define BUFFER_SIZE   0x200

extern ResLocation SemCopyRawFile( char * filename )
/**************************************************/
{
    int         handle;
    RcStatus    error;
    char *      buffer;
    char        full_filename[ _MAX_PATH ];
    ResLocation loc;
    int         err_code;
    long        pos;

    buffer = RcMemMalloc( BUFFER_SIZE );

    RcFindResource( filename, full_filename );
    if (full_filename[0] == '\0') {
        RcError( ERR_CANT_FIND_FILE, filename );
        goto HANDLE_ERROR;
    }

    if( AddDependency( full_filename ) ) goto HANDLE_ERROR;

    handle = RcIoOpenInput( full_filename, O_RDONLY | O_BINARY );
    if (handle == -1) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        goto HANDLE_ERROR;
    }

    loc.start = SemStartResource();

    pos = RcTell( handle );
    if( pos == -1 ) {
        RcError( ERR_READING_DATA, full_filename, strerror( errno ) );
        RcClose( handle );
        goto HANDLE_ERROR;
    } else {
        error = SemCopyDataUntilEOF( pos, handle, buffer, BUFFER_SIZE,
                                     &err_code );
        if( error != RS_OK ) {
            ReportCopyError( error, ERR_READING_DATA, full_filename,
                             err_code );
            RcClose( handle );
            goto HANDLE_ERROR;
        }
    }

    loc.len = SemEndResource( loc.start );

    RcClose( handle );

    RcMemFree( buffer );

    return( loc );


HANDLE_ERROR:
    ErrorHasOccured = TRUE;
    loc.start = 0;
    loc.len = 0;
    RcMemFree( buffer );
    return( loc );
}

extern DataElemList *SemNewDataElemList( RawDataItem node )
/**********************************************************/
{
    DataElemList  *head;

    head = RcMemMalloc( sizeof( DataElemList ) );
    head->data[ 0 ] = node;
    head->count = 1;
    head->next = NULL;

    return( head );
}

extern DataElemList *SemAppendDataElem( DataElemList *head, RawDataItem node )
/****************************************************************************/
{
    DataElemList    *travptr;
    DataElemList    *newnode;

    for( travptr = head; travptr->next != NULL; travptr = travptr->next )
        ;

    if( travptr->count == MAX_DATA_NODES ) {
        newnode = SemNewDataElemList( node );
        travptr->next = newnode;
    } else {
        travptr->data[ travptr->count ] = node;
        (travptr->count)++;
    }

    return( head );
}

extern ResLocation SemFlushDataElemList( DataElemList *head, char call_startend )
/*****************************************************************************/
{
    DataElemList      *curnode;
    DataElemList      *nextnode;
    ResLocation        resLoc;
    int                i;

    curnode = head;
    nextnode = head;
    if( call_startend ) {
        resLoc.start = SemStartResource();
    }
    while( nextnode != NULL ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            SemWriteRawDataItem( curnode->data[i] );
        }
        RcMemFree( curnode );
        curnode = nextnode;
    }
    if( call_startend ) {
       resLoc.len = SemEndResource( resLoc.start );
    }

    return( resLoc );
}

extern void SemFreeDataElemList( DataElemList *head )
/****************************************************/
{
    DataElemList    *curnode;
    DataElemList    *nextnode;
    int              i;

    curnode = head;
    nextnode = head;
    while( nextnode != NULL ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            if( curnode->data[i].IsString == TRUE ) {
                RcMemFree( curnode->data[i].Item.String );
            }
        }
        RcMemFree( curnode );
        curnode = nextnode;
    }
}
