/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OMF (Object Module Format) I/O.
*
****************************************************************************/


#include <stdio.h>      /* for SEEK_SET, SEEK_CUR, SEEK_END */
#include <string.h>
#include "asmglob.h"
#include "asmalloc.h"
#include "fatal.h"
#include "omfgenio.h"
#include "omfobjre.h"
#include "myassert.h"


#define OBJ_BUFFER_SIZE 0x1000      /* 4k (must be less than 64k) */

static struct {
    uint_16     length;     /* number of bytes written since rec header     */
    uint_16     in_buf;     /* number of bytes in buffer                    */
    uint_8      checksum;   /* for current record                           */
    uint_8      in_rec : 1; /* a record is open                             */
    uint_8      buffer[1];  /* for writing                                  */
} *pobjState;               /* object file buffering info                   */

/*
    Routines for buffered writing of an object file
*/

static void safeSeek( long offset, int mode )
{
/**/myassert( pobjState != NULL );
/**/myassert( AsmFiles.file[OBJ] != NULL );

    if( fseek( AsmFiles.file[OBJ], offset, mode ) ) {
        Fatal( MSG_OBJECT_FILE_ERROR, "fseek" );
    }
}

static void safeWrite( const uint_8 *buf, uint_16 len )
{
/**/myassert( pobjState != NULL );
/**/myassert( AsmFiles.file[OBJ] != NULL );

    if( fwrite( buf, 1, len, AsmFiles.file[OBJ] ) != len ) {
        Fatal( MSG_OBJECT_FILE_ERROR, "fwrite" );
    }
}

void ObjWriteOpen( void )
/***********************/
{
/*
    pobjState.file_out = ObjWriteOpen( AsmFiles.fname[OBJ] );
    if( pobjState.file_out == NULL ) {
        Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
    }
*/
    AsmFiles.file[OBJ] = fopen( AsmFiles.fname[OBJ], "wb" );
    if( AsmFiles.file[OBJ] == NULL ) {
        Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
    }
    pobjState = AsmAlloc( sizeof( *pobjState ) + OBJ_BUFFER_SIZE );
    pobjState->in_buf = 0;
    pobjState->in_rec = false;
}

void ObjWriteClose( bool del )
/****************************/
{
/**/myassert( pobjState != NULL );

    if( pobjState->in_rec ) {
        ObjWEndRec();
    }
    if( fclose( AsmFiles.file[OBJ] ) ) {
        Fatal( MSG_CANNOT_CLOSE_FILE, AsmFiles.fname[OBJ] );
    }
    AsmFree( pobjState );
    pobjState = NULL;
    if( del ) {
        /* This remove works around an NT networking bug */
        remove( AsmFiles.fname[OBJ] );
    }
}

void ObjWBegRec( uint_8 command )
/*******************************/
{
    uint_8  buf[3];

/**/myassert( !pobjState->in_rec );

    buf[0] = command;
    buf[1] = 0;
    buf[2] = 0;
    safeWrite( buf, 3 );
    pobjState->in_rec = true;
    pobjState->checksum = command;
    pobjState->in_buf = 0;
    pobjState->length = 0;
}

static void objWFlushBuffer( void )
/*********************************/
{
    uint_16 len_to_write;
    uint_8  checksum;
    uint_8  *p;

    len_to_write = pobjState->in_buf;
    if( len_to_write == 0 )
        return;
    checksum = pobjState->checksum;
    for( p = pobjState->buffer; p < pobjState->buffer + len_to_write; ++p ) {
        checksum += *p;
    }
    pobjState->checksum = checksum;
    pobjState->length += len_to_write;
    safeWrite( pobjState->buffer, len_to_write );
    pobjState->in_buf = 0;
}

void ObjWEndRec( void )
/*********************/
{
    uint_8  buf[2];
    uint_8  checksum;

/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf > 0 ) {
        objWFlushBuffer();
    }
    pobjState->length++;                  /* add 1 for checksum byte */
    WriteU16( buf, pobjState->length );
    checksum = pobjState->checksum + buf[0] + buf[1];
    checksum = -checksum;
    safeWrite( &checksum, 1 );
        /* back up to length */
    safeSeek( -(long)pobjState->length - 2, SEEK_CUR );
    safeWrite( buf, 2 );                   /* write the length */
    safeSeek( 0L, SEEK_END );       /* move to end of file again */
    pobjState->in_rec = false;
}

void ObjWrite8( uint_8 byte )
/***************************/
{
/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf == OBJ_BUFFER_SIZE ) {
        objWFlushBuffer();
    }
    pobjState->buffer[ pobjState->in_buf++ ] = byte;
}

void ObjWrite16( uint_16 word )
/*****************************/
{
/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf >= OBJ_BUFFER_SIZE - 1 ) {
        objWFlushBuffer();
    }
    WriteU16( pobjState->buffer + pobjState->in_buf, word );
    pobjState->in_buf += 2;
}

void ObjWrite32( uint_32 dword )
/******************************/
{
/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf >= OBJ_BUFFER_SIZE - 3 ) {
        objWFlushBuffer();
    }
    WriteU32( pobjState->buffer + pobjState->in_buf, dword );
    pobjState->in_buf += 4;
}

void ObjWriteIndex( uint_16 index )
/*********************************/
{
    if( index > 0x7f ) {
        ObjWrite8( 0x80 | ( index >> 8 ) );
    }
    ObjWrite8( index & 0xff );
}

void ObjWrite( const uint_8 *buf, uint_16 length )
/************************************************/
{
    const uint_8    *write;
    uint_16         amt;

/**/myassert( buf != NULL );

    write = buf;
    for(;;) {
        amt = OBJ_BUFFER_SIZE - pobjState->in_buf;
        if( amt >= length ) {
            memcpy( &pobjState->buffer[ pobjState->in_buf ], write, length );
            pobjState->in_buf += length;
            break;
        } else if( amt > 0 ) {
            memcpy( &pobjState->buffer[ pobjState->in_buf ], write, amt );
            pobjState->in_buf += amt;
            write += amt;
            length -= amt;
        }
        objWFlushBuffer();
    }
}

static uint_8 checkSum( const uint_8 *buf, uint_16 length )
/*********************************************************/
{
    uint_8 checksum;

    checksum = 0;
    while( length ) {
        checksum += *buf;
        ++buf;
        --length;
    }
    return( checksum );
}

void ObjWriteRec( uint_8 command, uint_16 length, const uint_8 *contents )
/************************************************************************/
/*
    Contents and length don't include checksum
*/
{
    uint_8  buf[3];
    uint_8  checksum;

/**/myassert( !pobjState->in_rec );

    checksum  = buf[0] = command;
    checksum += buf[1] = ( length + 1 ) & 0xff;
    checksum += buf[2] = ( length + 1 ) >> 8;
    safeWrite( buf, 3 );
    checksum += checkSum( contents, length );
    safeWrite( contents, length );
    checksum = -checksum;
    safeWrite( &checksum, 1 );
}
