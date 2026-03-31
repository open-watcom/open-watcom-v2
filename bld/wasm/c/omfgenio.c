/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "omfgenio.h"
#include "omfobjre.h"
#include "myassert.h"


#define SEEK_POSBACK(p) (-(long)(p))

#define OBJ_BUFFER_SIZE 0x1000      /* 4k (must be less than 64k) */

static struct {
    uint_16     length;     /* number of bytes written since rec header     */
    uint_16     in_buf;     /* number of bytes in buffer                    */
    uint_8      checksum;   /* for current record                           */
    uint_8      in_rec : 1; /* a record is open                             */
    uint_8      buffer[1];  /* for writing                                  */
} *pobjState;               /* object file buffering info                   */

uint_16 get16le( uint_8 *p )
/***************************
 * read unaligned 16-bit LE data
 */
{
    uint_16 u16;

#if defined( __BIG_ENDIAN__ )
    ((uint_8 *)&u16)[1] = *p++;
    ((uint_8 *)&u16)[0] = *p++;
#else
    ((uint_8 *)&u16)[0] = *p++;
    ((uint_8 *)&u16)[1] = *p++;
#endif
    return( u16 );
}

uint_32 get32le( uint_8 *p )
/***************************
 * read unaligned 32-bit LE data
 */
{
    uint_32 u32;

#if defined( __BIG_ENDIAN__ )
    ((uint_8 *)&u32)[3] = *p++;
    ((uint_8 *)&u32)[2] = *p++;
    ((uint_8 *)&u32)[1] = *p++;
    ((uint_8 *)&u32)[0] = *p++;
#else
    ((uint_8 *)&u32)[0] = *p++;
    ((uint_8 *)&u32)[1] = *p++;
    ((uint_8 *)&u32)[2] = *p++;
    ((uint_8 *)&u32)[3] = *p++;
#endif
    return( u32 );
}

void *put16le( uint_8 *p, uint_16 word )
/****************************************
 * write unaligned 16-bit LE data
 */
{
#if defined( __BIG_ENDIAN__ )
    *p++ = ((uint_8 *)&word)[1];
    *p++ = ((uint_8 *)&word)[0];
#else
    *p++ = ((uint_8 *)&word)[0];
    *p++ = ((uint_8 *)&word)[1];
#endif
    return( p );
}

void *put32le( uint_8 *p, uint_32 dword )
/****************************************
 * write unaligned 32-bit LE data
 */
{
#if defined( __BIG_ENDIAN__ )
    *p++ = ((uint_8 *)&dword)[3];
    *p++ = ((uint_8 *)&dword)[2];
    *p++ = ((uint_8 *)&dword)[1];
    *p++ = ((uint_8 *)&dword)[0];
#else
    *p++ = ((uint_8 *)&dword)[0];
    *p++ = ((uint_8 *)&dword)[1];
    *p++ = ((uint_8 *)&dword)[2];
    *p++ = ((uint_8 *)&dword)[3];
#endif
    return( p );
}

/*
    Routines for buffered writing of an object file
*/

static void safeSeek( long offset, int mode )
{
/**/myassert( pobjState != NULL );
/**/myassert( AsmFiles.fp[OBJ] != NULL );

    if( fseek( AsmFiles.fp[OBJ], offset, mode ) ) {
        Fatal( OBJECT_FILE_ERROR, "fseek" );
    }
}

static void safeWrite( const uint_8 *buf, uint_16 len )
{
/**/myassert( pobjState != NULL );
/**/myassert( AsmFiles.fp[OBJ] != NULL );

    if( fwrite( buf, 1, len, AsmFiles.fp[OBJ] ) != len ) {
        Fatal( OBJECT_FILE_ERROR, "fwrite" );
    }
}

void ObjWriteOpen( void )
/***********************/
{
/*
    pobjState.file_out = ObjWriteOpen( AsmFiles.fname[OBJ] );
    if( pobjState.file_out == NULL ) {
        Fatal( CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
    }
*/
    char    *obj_name;

    obj_name = CreateFileName( AsmFiles.fname[OBJ], OBJ_EXT, false );
    AsmFiles.fp[OBJ] = fopen( obj_name, "wb" );
    if( AsmFiles.fp[OBJ] == NULL ) {
        Fatal( CANNOT_OPEN_FILE, obj_name );
    }
    pobjState = MemAllocSafe( sizeof( *pobjState ) + OBJ_BUFFER_SIZE );
    pobjState->in_buf = 0;
    pobjState->in_rec = false;
}

void ObjWriteClose( bool del )
/****************************/
{
    char    *obj_name;

/**/myassert( pobjState != NULL );
    if( pobjState->in_rec ) {
        ObjWEndRec();
    }
    obj_name = CreateFileName( AsmFiles.fname[OBJ], OBJ_EXT, false );
    CloseAsmFile( OBJ );
    MemFree( pobjState );
    pobjState = NULL;
    if( del ) {
        /* This remove works around an NT networking bug */
        remove( obj_name );
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
    uint_16 length;
    uint_8  checksum;

/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf > 0 ) {
        objWFlushBuffer();
    }
    pobjState->length++;                  /* add 1 for checksum byte */
    put16le( (uint_8 *)&length, pobjState->length );
    checksum = pobjState->checksum + ((unsigned char *)&length)[0] + ((unsigned char *)&length)[1];
    checksum = -checksum;
    safeWrite( &checksum, 1 );
        /* back up to length */
    safeSeek( SEEK_POSBACK( pobjState->length + 2 ), SEEK_CUR );
    safeWrite( (uint_8 *)&length, 2 );                   /* write the length */
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
    put16le( pobjState->buffer + pobjState->in_buf, word );
    pobjState->in_buf += 2;
}

void ObjWrite32( uint_32 dword )
/******************************/
{
/**/myassert( pobjState->in_rec );

    if( pobjState->in_buf >= OBJ_BUFFER_SIZE - 3 ) {
        objWFlushBuffer();
    }
    put32le( pobjState->buffer + pobjState->in_buf, dword );
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
