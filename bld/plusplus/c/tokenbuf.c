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


#include "plusplus.h"

#include "preproc.h"
#include "tokenbuf.h"
#include "memmgr.h"
#include "ring.h"



BUFFER_HDR *TokenBufInit(               // TokenBuf: INITIALIZE FOR COLLECTION
    BUFFER_HDR *tokbuf_hdr )
{
    TOKEN_BUFFER    *tokbuf;


    /*
     *  Allocate tokenBuffer header structure
     */
    if( tokbuf_hdr == NULL ) {
        tokbuf_hdr = (BUFFER_HDR *)CMemAlloc( sizeof( BUFFER_HDR ) );
        tokbuf_hdr->no_of_buffers = 0;
        tokbuf_hdr->total_size = 0;
        tokbuf_hdr->list = NULL;
        tokbuf_hdr->curr = NULL;
    }


    /*
     *  Allocate tokenBuffer list structure
     */
    tokbuf = (TOKEN_BUFFER *)CMemAlloc( sizeof( TOKEN_BUFFER ) );
    tokbuf->cur_pos = 0;
    tokbuf->h[0] = '\0';


    /*
     *  Add tokenBuffer entry to the end of the token list
     *  in tokbuf_hdr structure.
     */
    RingAppend( &tokbuf_hdr->list, tokbuf );
    tokbuf_hdr->curr = tokbuf;


    ++(tokbuf_hdr->no_of_buffers);
    return( tokbuf_hdr );
}


BUFFER_HDR *TokenBufAddChar(            // TokenBuf: ADD A CHARACTER
    BUFFER_HDR *tokbuf_hdr,             // - handle to token buffer
    char character )                    // - character to be added
{
    TOKEN_BUFFER *tokbuf = tokbuf_hdr->curr;


    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tokbuf->cur_pos + 1) > BUF_SZ ) {
        tokbuf_hdr = TokenBufInit( tokbuf_hdr );
        tokbuf = tokbuf_hdr->curr;
    }

    tokbuf->h[tokbuf->cur_pos++] = character;
    ++tokbuf_hdr->total_size;

    return( tokbuf_hdr );
}

BUFFER_HDR *TokenBufAddToken(           // TokenBuf: ADD A TOKEN
    BUFFER_HDR *tokbuf_hdr,             // - handle to token buffer
    TOKEN token )                       // - token to be added
{
    TOKEN_BUFFER *tokbuf = tokbuf_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tokbuf->cur_pos + SIZE_MTOKEN ) > BUF_SZ ) {
        tokbuf_hdr = TokenBufInit( tokbuf_hdr );
        tokbuf = tokbuf_hdr->curr;
    }

    SET_MTOKEN( tokbuf->h + tokbuf->cur_pos, token );
    tokbuf->cur_pos += SIZE_MTOKEN;
    tokbuf_hdr->total_size += SIZE_MTOKEN;

    return( tokbuf_hdr );
}


BUFFER_HDR *TokenBufAddStr(             // TokenBuf: ADD A STRING
    BUFFER_HDR *tokbuf_hdr,             // - handle to token buffer
    const char *str )                   // - string to be added
{
    unsigned len = strlen( str );
    TOKEN_BUFFER *tokbuf = tokbuf_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate as many
     *  buffers as needed to store string.
     */
    if( ( tokbuf->cur_pos + len + 1) > BUF_SZ ) {
        tokbuf_hdr = TokenBufNew( tokbuf_hdr, str );
        tokbuf = tokbuf_hdr->curr;
    } else {
        memcpy( &(tokbuf->h[tokbuf->cur_pos]), str, len );
        tokbuf->cur_pos += len;
        tokbuf_hdr->total_size += len;
    }
    tokbuf_hdr = TokenBufAddChar( tokbuf_hdr, '\0' );
    return( tokbuf_hdr );
}


size_t TokenBufSize(                    // TokenBuf: RETURN SIZE
    BUFFER_HDR *tokbuf_hdr )            // - handle to token buffer
{
    return( tokbuf_hdr->curr->cur_pos );
}


void TokenBufRemoveWhiteSpace(          // TokenBuf: REMOVE WHITE SPACE FROM END
    BUFFER_HDR *tokbuf_hdr )            // - handle to token buffer
{
    TOKEN_BUFFER *tokbuf = tokbuf_hdr->curr;

    while( tokbuf->cur_pos >= SIZE_MTOKEN ) {
        tokbuf->cur_pos -= SIZE_MTOKEN;
        if( GET_MTOKEN( &tokbuf->h[tokbuf->cur_pos] ) != T_WHITE_SPACE ) {
            tokbuf->cur_pos += SIZE_MTOKEN;
            break;
        }
    }
}


BUFFER_HDR *TokenBufMove(               // TokenBuf: MOVE CONTENTS, RE-INITIALIZE
    BUFFER_HDR *tokbuf_hdr,             // - token header
    char *target )                      // - where to save
{
    TOKEN_BUFFER  *tokbuf;

    RingIterBeg( tokbuf_hdr->list, tokbuf ) {
        memcpy( target, tokbuf->h, tokbuf->cur_pos );
        target += tokbuf->cur_pos;
    } RingIterEnd( tokbuf )
    *target = '\0';

    TokenBufFini( tokbuf_hdr );
    tokbuf_hdr = TokenBufInit( NULL );
    return( tokbuf_hdr );
}


void TokenBufFini(                      // TokenBuf: FREE MEMORY USED BY TOKEN BUFFER
    BUFFER_HDR *tokbuf_hdr )
{
    RingFree( &tokbuf_hdr->list );
    CMemFreePtr( &tokbuf_hdr );
}

BUFFER_HDR *TokenBufNew(                // TokenBuf: ADD A STRING
    BUFFER_HDR *tokbuf_hdr,             // - handle to token buffer
    const char *str )                   // - string to be added
{
    TOKEN_BUFFER *tokbuf = tokbuf_hdr->curr;
    int   len, req_len;


    req_len = strlen( str );
    len = BUF_SZ - tokbuf->cur_pos;
    memcpy( &(tokbuf->h[tokbuf->cur_pos]), str, len );
    tokbuf->cur_pos = BUF_SZ;
    tokbuf_hdr->total_size += len;
    str += len;
    req_len -= len;

    while( req_len > 0 ) {
        tokbuf_hdr = TokenBufInit( tokbuf_hdr );
        tokbuf = tokbuf_hdr->curr;
        if( req_len > BUF_SZ ) {
            memcpy( tokbuf->h, str, BUF_SZ );
            tokbuf->cur_pos = BUF_SZ;
            tokbuf_hdr->total_size += BUF_SZ;
            str += BUF_SZ;
            req_len -= BUF_SZ;
        } else {
            memcpy( tokbuf->h, str, req_len );
            tokbuf->cur_pos += req_len;
            tokbuf_hdr->total_size += req_len;
            req_len = 0;
        }
    }

    return( tokbuf_hdr );
}

size_t TokenBufTotalSize(               // TokenBuf: RETURN SIZE OF ALL BUFFERS
    BUFFER_HDR *tokbuf_hdr )
{
   return( tokbuf_hdr->total_size );
}
