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


#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>      /* for SEEK_SET, SEEK_CUR, SEEK_END */
#include <string.h>
#include "womp.h"
#include "genutil.h"
#include "memutil.h"
#include "myassert.h"
#include "objio.h"
#include "objrec.h"


#if _WOMP_OPT & _WOMP_WATFOR
extern  void            GObjError(void);
#define Fatal(__x,__y)  GObjError()
#elif _WOMP_OPT & _WOMP_WASM
extern  void            ObjWriteError( void );
#define Fatal(__x,__y)  ObjWriteError()
#endif

STATIC void safeSeek( int fh, long offset, int mode ) {

    if( lseek( fh, offset, mode ) == -1 ) {
        Fatal( MSG_DISK_ERROR, "lseek" );
    }
}

#if _WOMP_OPT & _WOMP_READ

/*
    Routines for buffered reading of an object file
*/

OBJ_RFILE *ObjReadOpen( const char *filename ) {
/********************************************/
    int         fh;
    OBJ_RFILE   *new;

    fh = open( filename, O_RDONLY | O_BINARY );
    if( fh < 0 ) {
        return( NULL );
    }
    new = MemAlloc( sizeof( *new ) );
    new->fh = fh;
    new->bufs = NULL;
    new->cur_off = 0;
    return( new );
}

void ObjReadClose( OBJ_RFILE *obj ) {
/*********************************/
    objread_buf *cur;
    objread_buf *next;

/**/myassert( obj != NULL );
    close( obj->fh );
    cur = obj->bufs;
    while( cur != NULL ) {
        next = cur->next;
        MemFree( cur );
        cur = next;
    }
    MemFree( obj );
}

STATIC uint_8 *readData( OBJ_RFILE *obj, size_t len_reqd ) {

    objread_buf *cur;
    objread_buf *buf;
    size_t      len_read;
    size_t      cur_off;
    size_t      partial_len;

/**/myassert( obj != NULL );
/**/myassert( len_reqd <= OBJ_BUFFER_SIZE );
    cur = obj->bufs;
    if( cur != NULL ) {
        cur_off = obj->cur_off;
        partial_len = cur->in_buf - cur_off;
        if( len_reqd <= partial_len ) {
            obj->cur_off += len_reqd;
            return( cur->data + cur_off );
        }
        buf = MemAlloc( sizeof( *buf ) + partial_len + OBJ_BUFFER_SIZE );
        memcpy( buf->data, cur->data + cur_off, partial_len );
        cur->in_buf = cur_off;
        buf->next = MemRealloc( cur, sizeof( *cur ) + cur_off );
/**/    myassert( cur == buf->next );   /* FIXME is this reqd by ANSI? */
    } else {
        buf = MemAlloc( sizeof( *buf ) + OBJ_BUFFER_SIZE );
        buf->next = NULL;
        partial_len = 0;
    }
    obj->cur_off = len_reqd;
    len_read = read( obj->fh, buf->data + partial_len, OBJ_BUFFER_SIZE );
    if( len_read == 0 ) {
        Fatal( MSG_PREMATURE_EOF );
    }
    buf = MemRealloc( buf, sizeof( *buf ) + partial_len + len_read );
    buf->in_buf = partial_len + len_read;
    obj->bufs = buf;
    return( buf->data );
}

obj_rec *ObjReadRec( OBJ_RFILE *obj ) {
/***********************************/
    obj_rec *new;
    uint_8  command;
    uint_16 length;
    uint_8  *data;

/**/myassert( obj != NULL );

    data = readData( obj, 3 );
    command = data[0];
    length = ReadU16( data + 1 );
    --length;               /* we don't care about checksum */
    if( length > OBJ_MAX_REC ) {
        Fatal( MSG_MAX_REC_EXCEEDED, OBJ_MAX_REC );
    }
    data = readData( obj, length );
    new = ObjNewRec( command );
    ObjAttachData( new, data, length );
    readData( obj, 1 );     /* advance over checksum byte */
    return( new );
}

int ObjRMoreData( OBJ_RFILE *obj ) {
/**********************************/
    objread_buf *buf;

/**/myassert( obj != NULL );
    buf = obj->bufs;
    if( buf == NULL ) {
        return( !eof( obj->fh ) );
    }
    return( obj->cur_off < buf->in_buf || !eof( obj->fh ) );
}

void ObjRSkipPage( OBJ_RFILE *obj, size_t page_len ) {
/****************************************************/
    objread_buf *buf;
    long        offset;
    long        seek_ofs;

/**/myassert( obj != NULL );
    if( page_len == 0 ) {
        return;
    }
    buf = obj->bufs;
    if( buf == NULL ) {
        return;
    }
    offset = tell( obj->fh );
    if( offset == -1 ) {
        Fatal( MSG_DISK_ERROR, "tell" );
    }
    offset = page_len - ( offset - buf->in_buf + obj->cur_off ) % page_len;
    if( offset != page_len ) {
        seek_ofs = offset + obj->cur_off - buf->in_buf;
        if( seek_ofs < 0 ) {
            obj->cur_off += offset;
        } else {
            obj->cur_off = buf->in_buf;
            safeSeek( obj->fh, seek_ofs, SEEK_CUR );
        }
    }
}

#endif


#if _WOMP_OPT & _WOMP_WRITE

#ifdef __QNX__
#define OP_MODE         (O_RDWR | O_CREAT | O_TRUNC)
#define OP_PERM         (0666)
#else
#define OP_MODE         (O_RDWR | O_CREAT | O_TRUNC | O_BINARY)
#define OP_PERM         (S_IREAD | S_IWRITE)
#endif

/*
    Routines for buffered writing of an object file
*/

STATIC void safeWrite( int fh, const char *buf, size_t len ) {

    if( write( fh, buf, len ) != len ) {
        Fatal( MSG_DISK_ERROR, "write" );
    }
}

OBJ_WFILE *ObjWriteOpen( const char *filename ) {
/*********************************************/
    int         fh;
    OBJ_WFILE    *new;

    fh = open( filename, OP_MODE, OP_PERM );
    if( fh < 0 ) {
        return( NULL );
    }
    new = MemAlloc( sizeof( *new ) + OBJ_BUFFER_SIZE );
    new->fh = fh;
    new->in_buf = 0;
    new->in_rec = 0;

    return( new );
}

void ObjWriteClose( OBJ_WFILE *obj ) {
/**********************************/
/**/myassert( obj != NULL );

    if( obj->in_rec ) {
        ObjWEndRec( obj );
    }
    close( obj->fh );
    MemFree( obj );
}

void ObjWBegRec( OBJ_WFILE *obj, uint_8 command ) {
/***********************************************/
    char    buf[3];

/**/myassert( obj != NULL && !obj->in_rec );

    buf[0] = command;
    buf[1] = 0;
    buf[2] = 0;
    safeWrite( obj->fh, buf, 3 );
    obj->in_rec = 1;
    obj->checksum = command;
    obj->in_buf = 0;
    obj->length = 0;
}

static void objWFlushBuffer( OBJ_WFILE *obj ) {
/*******************************************/
    size_t  len_to_write;
    uint_8  checksum;
    char    *p;

/**/myassert( obj != NULL );

    len_to_write = obj->in_buf;
    if( len_to_write == 0 )  return;
    checksum = obj->checksum;
    for( p = obj->buffer; p < obj->buffer + len_to_write; ++p ) {
        checksum += *p;
    }
    obj->checksum = checksum;
    obj->length += len_to_write;
    safeWrite( obj->fh, obj->buffer, len_to_write );
    obj->in_buf = 0;
}

void ObjWEndRec( OBJ_WFILE *obj ) {
/*******************************/
    char    buf[2];
    uint_8  checksum;

/**/myassert( obj != NULL && obj->in_rec );

    if( obj->in_buf > 0 ) {
        objWFlushBuffer( obj );
    }
    ++obj->length;                  /* add 1 for checksum byte */
    WriteU16( buf, obj->length );
    checksum = obj->checksum + buf[0] + buf[1];
    checksum = -checksum;
    safeWrite( obj->fh, &checksum, 1 );
        /* back up to length */
    safeSeek( obj->fh, - (int_32)obj->length - 2, SEEK_CUR );
    safeWrite( obj->fh, buf, 2 );                   /* write the length */
    safeSeek( obj->fh, 0L, SEEK_END );       /* move to end of file again */
    obj->in_rec = 0;
}

void ObjWrite8( OBJ_WFILE *obj, uint_8 byte ) {
/*******************************************/
/**/myassert( obj != NULL && obj->in_rec );

    if( obj->in_buf == OBJ_BUFFER_SIZE ) {
        objWFlushBuffer( obj );
    }
    obj->buffer[ obj->in_buf++ ] = byte;
}

void ObjWrite16( OBJ_WFILE *obj, uint_16 word ) {
/*********************************************/
/**/myassert( obj != NULL && obj->in_rec );

    if( obj->in_buf >= OBJ_BUFFER_SIZE - 1 ) {
        objWFlushBuffer( obj );
    }
    WriteU16( obj->buffer + obj->in_buf, word );
    obj->in_buf += 2;
}

void ObjWrite32( OBJ_WFILE *obj, uint_32 dword ) {
/**********************************************/
/**/myassert( obj != NULL && obj->in_rec );

    if( obj->in_buf >= OBJ_BUFFER_SIZE - 3 ) {
        objWFlushBuffer( obj );
    }
    WriteU32( obj->buffer + obj->in_buf, dword );
    obj->in_buf += 4;
}

void ObjWriteIndex( OBJ_WFILE *obj, uint_16 index ) {
/*************************************************/
    if( index > 0x7f ) {
        ObjWrite8( obj, 0x80 | ( index >> 8 ) );
    }
    ObjWrite8( obj, index & 0xff );
}

void ObjWrite( OBJ_WFILE *obj, const char *buf, size_t length ) {
/*************************************************************/
    const char *write;
    size_t amt;

/**/myassert( obj != NULL && buf != NULL );

    write = buf;
    for(;;) {
        amt = OBJ_BUFFER_SIZE - obj->in_buf;
        if( amt >= length ) {
            memcpy( &obj->buffer[ obj->in_buf ], write, length );
            obj->in_buf += length;
            break;
        } else if( amt > 0 ) {
            memcpy( &obj->buffer[ obj->in_buf ], write, amt );
            obj->in_buf += amt;
            write += amt;
            length -= amt;
        }
        objWFlushBuffer( obj );
    }
}

STATIC uint_8 checkSum( const uint_8 *buf, uint_16 length ) {
/***********************************************************/
    uint_8 checksum;

    checksum = 0;
    while( length ) {
        checksum += *buf;
        ++buf;
        --length;
    }
    return( checksum );
}

void ObjWriteRec( OBJ_WFILE *obj, uint_8 command, uint_16 length,
    const char *contents ) {
/***************************************************************/
/*
    Contents and length don't include checksum
*/
    char buf[3];
    uint_8 checksum;

/**/myassert( obj != NULL && !obj->in_rec );

    checksum  = buf[0] = command;
    checksum += buf[1] = ( length + 1 ) & 0xff;
    checksum += buf[2] = ( length + 1 ) >> 8;
    safeWrite( obj->fh, buf, 3 );
    checksum += checkSum( contents, length );
    safeWrite( obj->fh, contents, length );
    checksum = -checksum;
    safeWrite( obj->fh, &checksum, 1 );
}

#if _WOMP_OPT & _WOMP_NASM

obj_offset ObjWSkip32( OBJ_WFILE *obj ) {
/*************************************/
    obj_offset off;

/**/myassert( obj != NULL && obj->in_rec );
    off.rec_begin = tell( obj->fh ) - obj->length - 2;
    off.offset = obj->length + obj->in_buf;
    ObjWrite32( obj, 0 );
    return( off );
}

void ObjWRedo32( OBJ_WFILE *obj, obj_offset off, uint_32 dword ) {
/*************************************************************/
#if !LITTLE_ENDIAN
    unsigned char    buf[4];
#endif
    uint_16 rec_length;
    char    checksum;
    int i;

/**/myassert( obj != NULL );
    safeSeek( obj->fh, off.rec_begin, SEEK_SET );
    #if LITTLE_ENDIAN
        safeRead( obj->fh, (char *)&rec_length, 2 );
    #else
        safeRead( obj->fh, buf, 2 );
        rec_length = ReadU16( buf );
    #endif
    safeSeek( obj->fh, off.offset, SEEK_CUR );
    #if LITTLE_ENDIAN
        safeWrite( obj->fh, (char *)&dword, 4 );
    #else
        for (i = 0; i < 4; i++) {
            buf[ i ] = ((char*)&dword)[3-i];
        }
        safeWrite( obj->fh, buf, 4 );
    #endif
    safeSeek( obj->fh, (int_32)rec_length - off.offset - 5, SEEK_CUR );
    safeRead( obj->fh, &checksum, 1 );
    checksum = -checksum;
    for (i = 0; i < 4; i++) {
        checksum += ((char*)&dword)[i];
    }
    checksum = -checksum;
    safeSeek( obj->fh, -1L, SEEK_CUR );
    safeWrite( obj->fh, &checksum, 1 );
    safeSeek( obj->fh, 0L, SEEK_END );
}

obj_offset ObjWSkip16( OBJ_WFILE *obj ) {
/*************************************/
    obj_offset off;

/**/myassert( obj != NULL && obj->in_rec );
    off.rec_begin = tell( obj->fh ) - obj->length - 2;
    off.offset = obj->length + obj->in_buf;
    ObjWrite16( obj, 0 );
    return( off );
}

STATIC void safeRead( int fh, char *buf, size_t len ) {

    if( read( fh, buf, len ) != len ) {
        Fatal( MSG_PREMATURE_EOF, "read");
    }
}

void ObjWRedo16( OBJ_WFILE *obj, obj_offset off, uint_16 word ) {
/*************************************************************/
#if !LITTLE_ENDIAN
    char    buf[2];
#endif
    uint_16 rec_length;
    char    checksum;

/**/myassert( obj != NULL );
    safeSeek( obj->fh, off.rec_begin, SEEK_SET );
    #if LITTLE_ENDIAN
        safeRead( obj->fh, (char *)&rec_length, 2 );
    #else
        safeRead( obj->fh, buf, 2 );
        rec_length = ReadU16( buf );
    #endif
    safeSeek( obj->fh, off.offset, SEEK_CUR );
    #if LITTLE_ENDIAN
        safeWrite( obj->fh, (char *)&word, 2 );
    #else
        buf[ 0 ] = word & 0xff;
        buf[ 1 ] = word >> 8;
        safeWrite( obj->fh, buf, 2 );
    #endif
    safeSeek( obj->fh, (int_32)rec_length - off.offset - 3, SEEK_CUR );
    safeRead( obj->fh, &checksum, 1 );
    checksum = -checksum;
    checksum += word & 0xff;
    checksum += word >> 8;
    checksum = -checksum;
    safeSeek( obj->fh, -1L, SEEK_CUR );
    safeWrite( obj->fh, &checksum, 1 );
    safeSeek( obj->fh, 0L, SEEK_END );
}

#endif

#endif
