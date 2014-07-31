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
* Description:  Buffered POSIX style I/O.
*
****************************************************************************/


#include "cgstd.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "cgdefs.h"
#include "cgmem.h"
#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "system.h"
#include "pcobj.h"
#include "zoiks.h"
#include "banner.h"
#include "utils.h"
#include "objio.h"
#include "feprotos.h"


#define INVALID_HANDLE  NULL

#define BOUNDARY        64
#define LIMIT           0x7FFFFFFF
#define BIGGEST         0x7FFFFFFF
#define MASK            0x80000000U

#define IOBUFSIZE       8192

typedef FILE            *handle;

typedef objhandle       objoffset;

typedef struct buf {
    struct buf  *nextbuf;
    char        *bufptr;        // current position within buffer
    char        *buf;           // start of buffer
    uint        bytes_left;     // number of bytes remaining in buffer
    uint        bytes_written;  // number of bytes written to buffer
} buf;

extern  void            FatalError( const char * );

static  objoffset       ObjOffset;
static  handle          ObjFile;
static  bool            NeedSeek;
static  bool            EraseObj;
static  buf             *BufList;       // start of list of buffers
static  buf             *CurBuf;        // current buffer


static buf *NewBuffer( void )
/***************************/
{
    buf     *newbuf;

    newbuf = CGAlloc( sizeof( buf ) );
    newbuf->buf = CGAlloc( IOBUFSIZE );
    newbuf->bufptr = newbuf->buf;
    newbuf->bytes_left = IOBUFSIZE;
    newbuf->bytes_written = 0;
    newbuf->nextbuf = NULL;
    return( newbuf );
}


static  void    CloseStream( handle h )
/*************************************/
{
    fclose( h );
}


static  void    EraseStream( const char *name )
/*********************************************/
{
    remove( name );
}


static void cleanupLastBuffer( buf *pbuf )
{
    uint amt_used;

    amt_used = IOBUFSIZE - pbuf->bytes_left;
    if( pbuf->bytes_written < amt_used ) {
        pbuf->bytes_written = amt_used;
    }
}


static  void    ObjError( int errcode )
/*************************************/
{
    FatalError( strerror(  errcode ) );
}


static  handle  CreateStream( const char *name )
/**********************************************/
{
    handle      retc;

    retc = fopen( name, "wb" );
    if( retc == INVALID_HANDLE ) {
        ObjError( errno );
    }
    return( retc );
}


bool    CGOpenf( void )
/*********************/
{
    ObjFile = CreateStream( FEAuxInfo( NULL, OBJECT_FILE_NAME ) );
    if( ObjFile == INVALID_HANDLE )
        return( FALSE );
    BufList = NewBuffer();              // allocate first buffer
    CurBuf = BufList;
    return( TRUE );
}


void    OpenObj( void )
/*********************/
{
    ObjOffset = INVALID_OBJHANDLE;
    NeedSeek = FALSE;
    EraseObj = FALSE;
}


static  byte    DoSum( const byte *buff, uint len )
/*************************************************/
{
    byte        sum;

    sum = 0;
    while( len > 0 ) {
        sum += *buff++;
        --len;
    }
    return( sum );
}

static  objoffset   Byte( objhandle i )
/*************************************/
{
    return( i );
}


static  objhandle   Offset( objoffset offset )
/********************************************/
{
    if( offset > BIGGEST ) {
        FatalError( "Object file too large" );
        return( INVALID_OBJHANDLE );
    } else {
        return( offset );
    }
}


static  void    PutStream( handle h, const byte *b, uint len )
/************************************************************/
{
    uint        n;

    h = h;
    for( ;; ) {
        n = len;
        if( n > CurBuf->bytes_left ) {
            n = CurBuf->bytes_left;
        }
        memcpy( CurBuf->bufptr, b, n );
        b += n;
        CurBuf->bufptr += n;
        CurBuf->bytes_left -= n;
        if( CurBuf->nextbuf == NULL ) {         // if this is last buffer
            cleanupLastBuffer( CurBuf );
        }
        len -= n;
        if( len == 0 )
            break;
        if( CurBuf->nextbuf == NULL ) {
            CurBuf->nextbuf = NewBuffer();
            CurBuf = CurBuf->nextbuf;
        } else {
            CurBuf = CurBuf->nextbuf;
            CurBuf->bufptr = CurBuf->buf;
            CurBuf->bytes_left = IOBUFSIZE;
        }
    }
}


static  void    GetStream( handle h, byte *b, uint len )
/******************************************************/
{
    uint        n;

    h = h;
    for( ;; ) {
        n = len;
        if( n > CurBuf->bytes_left ) {
            n = CurBuf->bytes_left;
        }
        memcpy( b, CurBuf->bufptr, n );
        b += n;
        CurBuf->bufptr += n;
        CurBuf->bytes_left -= n;
        len -= n;
        if( len == 0 )
            break;
        CurBuf = CurBuf->nextbuf;
        if( CurBuf == NULL ) {
            _Zoiks( ZOIKS_006 );
        }
        CurBuf->bufptr = CurBuf->buf;
        CurBuf->bytes_left = IOBUFSIZE;
    }
}


static  void    SeekStream( handle h, objoffset offset )
/******************************************************/
{
    buf         *pbuf;
    objoffset   n;

    h = h;
    pbuf = BufList;
    n = 0;
    for( ;; ) {
        n += IOBUFSIZE;
        if( n > offset )
            break;
        if( pbuf->nextbuf == NULL ) {
            // seeking past the end of the file (extend file)
            pbuf->bytes_written = IOBUFSIZE;
            pbuf->nextbuf = NewBuffer();
        }
        pbuf = pbuf->nextbuf;
    }
    n = offset - (n - IOBUFSIZE);
    pbuf->bufptr = pbuf->buf + n;
    pbuf->bytes_left = IOBUFSIZE - n;
    if( pbuf->nextbuf == NULL ) {               // if this is last buffer
        cleanupLastBuffer( pbuf );
    }
    CurBuf = pbuf;
}


objhandle       AskObjHandle( void )
/**********************************/
{
    return( Offset( ObjOffset ) );
}

void    PutObjBytes( const void *buff, uint len )
/***********************************************/
{
    if( NeedSeek ) {
        SeekStream( ObjFile, ObjOffset );
        NeedSeek = FALSE;
    }
    PutStream( ObjFile, buff, len );
    ObjOffset += len;
}

void    PutObjOMFRec( byte class, const void *buff, uint len )
/************************************************************/
{
    unsigned_16     blen;
    byte            cksum;

    if( NeedSeek ) {
        SeekStream( ObjFile, ObjOffset );
        NeedSeek = FALSE;
    }
    blen = _TargetShort( len + 1 );
    cksum = class;
    cksum += DoSum( (const void *)&blen, sizeof( blen ) );
    cksum += DoSum( buff, len );
    cksum = -cksum;
    PutStream( ObjFile, &class, 1 );
    PutStream( ObjFile, (const byte *)&blen, sizeof( blen ) );
    PutStream( ObjFile, buff, len );
    PutStream( ObjFile, &cksum, 1 );
    ObjOffset += len + 4;
}


void    PatchObj( objhandle rec, uint roffset, const byte *buff, uint len )
/*************************************************************************/
{
    objoffset       recoffset;
    byte            cksum;
    unsigned_16     reclen;
    byte            inbuff[80];

    recoffset = Byte( rec );

    SeekStream( ObjFile, recoffset + 1 );
    GetStream( ObjFile, (byte *)&reclen, 2 );
    reclen = _HostShort( reclen );
    SeekStream( ObjFile, recoffset + roffset + 3 );
    GetStream( ObjFile, inbuff, len );

    SeekStream( ObjFile, recoffset + roffset + 3 );
    PutStream( ObjFile, buff, len );

    SeekStream( ObjFile, recoffset + 2 + reclen );
    GetStream( ObjFile, &cksum, 1 );

    cksum += DoSum( inbuff, len );
    cksum -= DoSum( buff, len );

    SeekStream( ObjFile, recoffset + 2 + reclen );
    PutStream( ObjFile, &cksum, 1 );

    NeedSeek = TRUE;
}


void    GetFromObj( objhandle rec, uint roffset, byte *buff, uint len )
/*********************************************************************/
{
    SeekStream( ObjFile, Byte( rec ) + roffset + 3 );
    GetStream( ObjFile, buff, len );
    NeedSeek = TRUE;
}


void    AbortObj( void )
/**********************/
{
    EraseObj = TRUE;
}


static void FlushBuffers( handle h )
/**********************************/
{
    buf         *pbuf;
    int         retc;

    for( ; (pbuf = BufList) != NULL; ) {
        BufList = pbuf->nextbuf;
        retc = fwrite( pbuf->buf, 1, pbuf->bytes_written, h );
        if( (unsigned_16)retc != pbuf->bytes_written ) {
            FatalError( "Error writing object file" );
        }
        CGFree( pbuf->buf );
        CGFree( pbuf );
    }
    CurBuf = NULL;
}


void    CloseObj( void )
/**********************/
{
    if( ObjFile != INVALID_HANDLE ) {
        FlushBuffers( ObjFile );
        CloseStream( ObjFile );
        if( EraseObj ) {
            EraseStream( FEAuxInfo( NULL, OBJECT_FILE_NAME ) );
        }
        ObjFile = INVALID_HANDLE;
    }
}


void    ScratchObj( void )
/************************/
{
    EraseObj = TRUE;
    CloseObj();
}
