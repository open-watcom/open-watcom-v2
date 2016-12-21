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
* Description:  Raw data semantic actions.
*
****************************************************************************/


#include <limits.h>
#include "wio.h"
#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "depend.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


void SemWriteRawDataItem( RawDataItem item )
/******************************************/
{
    bool        error;

    if( item.IsString ) {
        size_t  len = item.StrLen;

        if( item.WriteNull ) {
            ++len;
        }
        if( ResWriteStringLen( item.Item.String, item.LongItem, CurrResFile.fid, len ) ) {
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
            ErrorHasOccured = true;
        }
        if( item.TmpStr ) {
            RCFREE( item.Item.String );
        }
    } else {
        if( !item.LongItem ) {
            if( (int_32)item.Item.Num < 0 ) {
                if( (int_32)item.Item.Num < SHRT_MIN ) {
                    RcWarning( ERR_RAW_DATA_TOO_SMALL, item.Item.Num, SHRT_MIN );
                }
            } else {
                if( item.Item.Num > USHRT_MAX ) {
                    RcWarning( ERR_RAW_DATA_TOO_BIG, item.Item.Num, USHRT_MAX );
                }
            }
        }
        if( !ErrorHasOccured ) {
            if( !item.LongItem ) {
                error = ResWriteUint16( item.Item.Num, CurrResFile.fid );
            } else {
                error = ResWriteUint32( item.Item.Num, CurrResFile.fid );
            }
            if( error ) {
                RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
                ErrorHasOccured = true;
            }
        }
    }
}

RcStatus SemCopyDataUntilEOF( WResFileOffset offset, WResFileID fid,
                         void *buff, unsigned buffsize, int *err_code )
/*********************************************************************/
{
    WResFileSSize   numread;

    if( RCSEEK( fid, offset, SEEK_SET ) == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    while( (numread = RCREAD( fid, buff, buffsize )) != 0 ) {
        if( RCIOERR( fid, numread ) ) {
            *err_code = errno;
            return( RS_READ_ERROR );
        }
        if( RCWRITE( CurrResFile.fid, buff, numread ) != numread ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
}

#define BUFFER_SIZE   0x200

ResLocation SemCopyRawFile( const char *filename )
/************************************************/
{
    WResFileID      fid;
    RcStatus        ret;
    char            *buffer;
    char            full_filename[_MAX_PATH];
    ResLocation     loc;
    int             err_code;
    WResFileOffset  pos;

    buffer = RCALLOC( BUFFER_SIZE );

    if( RcFindResource( filename, full_filename ) == -1 ) {
        RcError( ERR_CANT_FIND_FILE, filename );
        goto HANDLE_ERROR;
    }

    if( AddDependency( full_filename ) )
        goto HANDLE_ERROR;

    fid = RcIoOpenInput( full_filename, false );
    if( fid == WRES_NIL_HANDLE ) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        goto HANDLE_ERROR;
    }

    loc.start = SemStartResource();

    pos = RCTELL( fid );
    if( pos == -1 ) {
        RcError( ERR_READING_DATA, full_filename, strerror( errno ) );
        RCCLOSE( fid );
        goto HANDLE_ERROR;
    } else {
        ret = SemCopyDataUntilEOF( pos, fid, buffer, BUFFER_SIZE, &err_code );
        if( ret != RS_OK ) {
            ReportCopyError( ret, ERR_READING_DATA, full_filename, err_code );
            RCCLOSE( fid );
            goto HANDLE_ERROR;
        }
    }

    loc.len = SemEndResource( loc.start );

    RCCLOSE( fid );

    RCFREE( buffer );

    return( loc );


HANDLE_ERROR:
    ErrorHasOccured = true;
    loc.start = 0;
    loc.len = 0;
    RCFREE( buffer );
    return( loc );
}

DataElemList *SemNewDataElemList( RawDataItem node )
/**************************************************/
{
    DataElemList    *head;

    head = RCALLOC( sizeof( DataElemList ) );
    head->data[0] = node;
    head->count = 1;
    head->next = NULL;

    return( head );
}

DataElemList *SemAppendDataElem( DataElemList *head, RawDataItem node )
/*********************************************************************/
{
    DataElemList    *travptr;
    DataElemList    *newnode;

    for( travptr = head; travptr->next != NULL; travptr = travptr->next )
        ;

    if( travptr->count == MAX_DATA_NODES ) {
        newnode = SemNewDataElemList( node );
        travptr->next = newnode;
    } else {
        travptr->data[travptr->count] = node;
        (travptr->count)++;
    }

    return( head );
}

ResLocation SemFlushDataElemList( DataElemList *head, bool call_startend )
/************************************************************************/
{
    DataElemList    *curnode;
    DataElemList    *nextnode;
    ResLocation     resLoc;
    int             i;

    curnode = head;
    nextnode = head;
    resLoc.len = 0;
    if( call_startend ) {
        resLoc.start = SemStartResource();
    } else {
        resLoc.start = 0;
    }
    while( nextnode != NULL ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            SemWriteRawDataItem( curnode->data[i] );
        }
        RCFREE( curnode );
        curnode = nextnode;
    }
    if( call_startend ) {
        if( CmdLineParms.MSResFormat
          && CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            ResWritePadDWord( CurrResFile.fid );
        }
        resLoc.len = SemEndResource( resLoc.start );
    }

    return( resLoc );
}

void SemFreeDataElemList( DataElemList *head )
/********************************************/
{
    DataElemList    *curnode;
    DataElemList    *nextnode;
    int             i;

    curnode = head;
    nextnode = head;
    while( nextnode != NULL ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            if( curnode->data[i].IsString ) {
                RCFREE( curnode->data[i].Item.String );
            }
        }
        RCFREE( curnode );
        curnode = nextnode;
    }
}
