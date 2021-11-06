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
* Description:  Raw data semantic actions.
*
****************************************************************************/


#include "global.h"
#include "rcerrors.h"
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
        error = ResWriteStringLen( item.Item.String, item.LongItem, CurrResFile.fp, item.StrLen );
        if( !error ) {
            if( item.WriteNull ) {
                if( item.LongItem ) {
                    error = ResWriteUint16( 0, CurrResFile.fp );
                } else {
                    error = ResWriteUint8( '\0', CurrResFile.fp );
                }
            }
        }
        if( item.TmpStr ) {
            RESFREE( item.Item.String );
        }
    } else {
        error = false;
        if( !ErrorHasOccured ) {
            if( item.LongItem ) {
                error = ResWriteUint32( item.Item.Num, CurrResFile.fp );
            } else {
                error = ResWriteUint16( item.Item.Num, CurrResFile.fp );
            }
        }
    }
    if( error ) {
        RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
        ErrorHasOccured = true;
    }
}

RcStatus SemCopyDataUntilEOF( long offset, FILE *fp,
                         void *buff, unsigned buffsize, int *err_code )
/*********************************************************************/
{
    size_t      numread;

    if( RESSEEK( fp, offset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    while( (numread = RESREAD( fp, buff, buffsize )) != 0 ) {
        if( numread != buffsize && RESIOERR( fp, numread ) ) {
            *err_code = errno;
            return( RS_READ_ERROR );
        }
        if( RESWRITE( CurrResFile.fp, buff, numread ) != numread ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
}

#define BUFFER_SIZE   0x200

static ResLocation semCopyRawFile( const char *filename, bool onlyFile )
/**********************************************************************/
{
    FILE            *fp;
    RcStatus        ret;
    char            *buffer;
    char            full_filename[_MAX_PATH];
    ResLocation     loc;
    int             err_code;
    long            pos;
    bool            error;

    error = false;
    fp = NULL;
    buffer = RESALLOC( BUFFER_SIZE );
    if( !onlyFile ) {
        if( RcFindSourceFile( filename, full_filename ) == -1 ) {
            RcError( ERR_CANT_FIND_FILE, filename );
            error = true;
        }
        if( !error ) {
            if( AddDependency( full_filename ) ) {
                error = true;
            } else {
                filename = full_filename;
            }
        }
    }
    if( !error ) {
        fp = RcIoOpenInput( filename, false );
        if( fp == NULL ) {
            RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
            error = true;
        }
    }

    loc.start = 0;
    loc.len = 0;
    if( !error ) {
        loc.start = SemStartResource();
        pos = RESTELL( fp );
        if( pos == -1L ) {
            RcError( ERR_READING_DATA, filename, strerror( errno ) );
            error = true;
        } else {
            ret = SemCopyDataUntilEOF( pos, fp, buffer, BUFFER_SIZE, &err_code );
            if( ret != RS_OK ) {
                ReportCopyError( ret, ERR_READING_DATA, filename, err_code );
                error = true;
            }
        }
    }
    if( error ) {
        ErrorHasOccured = true;
        loc.start = 0;
        loc.len = 0;
    } else {
        loc.len = SemEndResource( loc.start );
    }
    if( fp != NULL ) {
        RESCLOSE( fp );
    }
    RESFREE( buffer );
    return( loc );
}

ResLocation SemCopyRawFile( const char *filename )
/************************************************/
{
    return( semCopyRawFile( filename, false ) );
}

ResLocation SemCopyRawFileOnly( const char *filename )
/****************************************************/
{
    return( semCopyRawFile( filename, true ) );
}

DataElemList *SemNewDataElemList( RawDataItem node )
/**************************************************/
{
    DataElemList    *head;

    head = RESALLOC( sizeof( DataElemList ) );
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

    resLoc.len = 0;
    if( call_startend ) {
        resLoc.start = SemStartResource();
    } else {
        resLoc.start = 0;
    }
    for( curnode = head; curnode != NULL; curnode = nextnode ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            SemWriteRawDataItem( curnode->data[i] );
        }
        RESFREE( curnode );
    }
    if( call_startend ) {
        resLoc.len = SemEndResource( resLoc.start );
        if( CmdLineParms.MSResFormat && CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            ResWritePadDWord( CurrResFile.fp );
        }
    }

    return( resLoc );
}

void SemFreeDataElemList( DataElemList *head )
/********************************************/
{
    DataElemList    *curnode;
    DataElemList    *nextnode;
    int             i;

    for( curnode = head; curnode != NULL; curnode = nextnode ) {
        nextnode = curnode->next;
        for( i = 0; i < curnode->count; i++ ) {
            if( curnode->data[i].IsString ) {
                RESFREE( curnode->data[i].Item.String );
            }
        }
        RESFREE( curnode );
    }
}
