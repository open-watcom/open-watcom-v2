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


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "wio.h"
#include "cgstd.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "objrep.h"
#include "system.h"
#include "pcobj.h"
#include "zoiks.h"
#include "banner.h"
#include "feprotos.h"
#include "utils.h"


typedef int             handle;
typedef objhandle       objoffset;

enum stdfd { HStdIn=0, HStdOut =1, HStdErr = 2 };

extern  void            FatalError( char * );


static  objoffset       ObjOffset;
static  handle          ObjFile;
static  bool            NeedSeek;
static  char            ObjName[PATH_MAX+1];
static  bool            EraseObj;


#define BOUNDARY 64
#define LIMIT    0x7FFFFFFFL
#define BIGGEST  0x7FFFFFFFL
#define MASK     0x80000000U

#define IOBUFSIZE       8192
struct buf {
    struct buf  *nextbuf;
    char        *bufptr;        // current position within buffer
    char        *buf;           // start of buffer
    uint        bytes_left;     // number of bytes remaining in buffer
    uint        bytes_written;  // number of bytes written to buffer
};
static  struct buf      *BufList;       // start of list of buffers
static  struct buf      *CurBuf;        // current buffer


static struct buf *NewBuffer( void )
/**********************************/
{
    struct buf  *newbuf;

    newbuf = CGAlloc( sizeof(struct buf) );
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
    close( h );
}


static  void    EraseStream( char *name )
/***************************************/
{
    remove( name );
}


static void cleanupLastBuffer( struct buf *pbuf )
{
    uint amt_used;

    amt_used = IOBUFSIZE - pbuf->bytes_left;
    if( pbuf->bytes_written < amt_used ) {
        pbuf->bytes_written = amt_used;
    }
}


static  void    ObjError( int    errcode )
/****************************************/
{
    FatalError( strerror(  errcode ) );
}


static  handle  CreateStream( char *name )
/****************************************/
{
    int         retc;

    retc = open( name, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, PMODE_RW );
    if( retc == -1 ) {
        ObjError( errno );
    }
    return( retc );
}


bool    CGOpenf( void )
/*********************/
{
    CopyStr( FEAuxInfo( NULL, OBJECT_FILE_NAME ), ObjName );
    ObjFile = -1;
    ObjFile = CreateStream( ObjName );
    if( ObjFile == -1 ) return( FALSE );
    BufList = NewBuffer();              // allocate first buffer
    CurBuf = BufList;
    return( TRUE );
}


void    OpenObj( void )
/*********************/
{
    ObjOffset = 0;
    NeedSeek = FALSE;
    EraseObj = FALSE;
}


static  byte    DoSum( byte *buff, uint len )
/*******************************************/
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
        return( 0 );
    } else {
        return( offset );
    }
}


static  void    PutStream( handle h, const byte *b, uint len )
/************************************************************/
{
#if 0
    int         retc;

    retc = write( h, b, len );
    if( retc == -1 ) {
        ObjError( errno );
    }
    if( (unsigned_16)retc != len ) {
        FatalError( "Error writing object file - disk is full" );
    }
#else
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
        if( len == 0 ) break;
        if( CurBuf->nextbuf == NULL ) {
            CurBuf->nextbuf = NewBuffer();
            CurBuf = CurBuf->nextbuf;
        } else {
            CurBuf = CurBuf->nextbuf;
            CurBuf->bufptr = CurBuf->buf;
            CurBuf->bytes_left = IOBUFSIZE;
        }
    }
#endif
}


static  void    GetStream( handle h, byte *b, uint len )
/******************************************************/
{
#if 0
    int         retc;

    retc = read( h, b, len );
    if( retc == -1 ) {
        ObjError( errno );
    }
    if( (unsigned_16)retc != len ) {
        _Zoiks( ZOIKS_006 );
    }
#else
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
        if( len == 0 ) break;
        CurBuf = CurBuf->nextbuf;
        if( CurBuf == NULL ) {
            _Zoiks( ZOIKS_006 );
        }
        CurBuf->bufptr = CurBuf->buf;
        CurBuf->bytes_left = IOBUFSIZE;
    }
#endif
}


static  void    SeekStream( handle h, objoffset offset )
/******************************************************/
{
#if 0
    offset = lseek( h, offset, 0 );
    if( offset == -1 ) {
        ObjError( errno );
    }
#else
    struct buf  *pbuf;
    objoffset   n;

    h = h;
    pbuf = BufList;
    n = 0;
    for( ;; ) {
        n += IOBUFSIZE;
        if( n > offset ) break;
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
#endif
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

void    PutObjRec( byte class, byte *buff, uint len )
/***************************************************/
{
#include "pushpck1.h"
    static struct {
        byte            class;
        unsigned_16     len;
        unsigned_8      data[1024+1]; /* +1 for check sum */
    } header;
#include "poppck.h"
    byte        cksum;

    if( NeedSeek ) {
        SeekStream( ObjFile, ObjOffset );
        NeedSeek = FALSE;
    }
    header.class = class;
    header.len = _TargetInt( len + 1 );
    cksum = DoSum( (byte *)&header, 3 );
    cksum += DoSum( buff, len );
    cksum = -cksum;
    if( len <= (sizeof( header.data ) - 1) ) {
        memcpy( header.data, buff, len );
        header.data[len] = cksum;
        PutStream( ObjFile, (byte *)&header, len + 4 );
    } else {
        PutStream( ObjFile, (byte *)&header, 3 );
        PutStream( ObjFile, buff, len );
        PutStream( ObjFile, &cksum, 1 );
    }
    ObjOffset += len + 4;
}


void    PatchObj( objhandle rec, uint roffset, byte *buff, uint len )
/*******************************************************************/
{
    objoffset       recoffset;
    byte            cksum;
    unsigned_16     reclen;
    byte            inbuff[80];

    recoffset = Byte( rec );

    SeekStream( ObjFile, recoffset + 1 );
    GetStream( ObjFile, (byte *)&reclen, 2 );
    reclen = _HostInt( reclen );
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
    struct buf  *pbuf;
    int         retc;

    for( ;; ) {
        pbuf = BufList;
        if( pbuf == NULL ) break;
        retc = write( h, pbuf->buf, pbuf->bytes_written );
        if( (unsigned_16)retc != pbuf->bytes_written ) {
            FatalError( "Error writing object file" );
        }
        BufList = pbuf->nextbuf;
        CGFree( pbuf->buf );
        CGFree( pbuf );
    }
    CurBuf = NULL;
}


void    CloseObj( void )
/**********************/
{
    if( ObjFile != -1 ) {
        FlushBuffers( ObjFile );
        CloseStream( ObjFile );
        if( EraseObj ) {
            EraseStream( ObjName );
        }
        ObjFile = -1;
    }
}


void    ScratchObj( void )
/************************/
{
    EraseObj = TRUE;
    CloseObj();
}
