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
    BUFFER_HDR *token_hdr )
{
    TOKEN_BUFFER    *token_buf;


    /*
     *  Allocate tokenBuffer header structure
     */
    if( !token_hdr ) {
        token_hdr = (BUFFER_HDR *)CMemAlloc( sizeof( BUFFER_HDR ));
        token_hdr->no_of_buffers = 0;
        token_hdr->total_size = 0;
        token_hdr->list = NULL;
        token_hdr->curr = NULL;
    }


    /*
     *  Allocate tokenBuffer list structure
     */
    token_buf = (TOKEN_BUFFER *)CMemAlloc( sizeof(TOKEN_BUFFER));
    token_buf->cur_pos = 0;
    token_buf->h[0] = '\0';


    /*
     *  Add tokenBuffer entry to the end of the token list
     *  in token_hdr structure.
     */
    RingAppend( &token_hdr->list, token_buf );
    token_hdr->curr = token_buf;


    ++(token_hdr->no_of_buffers);
    return token_hdr;
}


BUFFER_HDR *TokenBufAddChar(            // TokenBuf: ADD A CHARACTER
    BUFFER_HDR *token_hdr,              // - handle to token buffer
    char character )                    // - character to be added
{
    TOKEN_BUFFER *tb = token_hdr->curr;


    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tb->cur_pos + 1) > BUF_SZ ) {
        token_hdr = TokenBufInit( token_hdr );
        tb = token_hdr->curr;
    }

    tb->h[tb->cur_pos++] = character;
    ++token_hdr->total_size;

    return token_hdr;
}

BUFFER_HDR *TokenBufAddToken(           // TokenBuf: ADD A TOKEN
    BUFFER_HDR *token_hdr,              // - handle to token buffer
    TOKEN token )                       // - token to be added
{
    TOKEN_BUFFER *tb = token_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tb->cur_pos + sizeof( TOKEN ) ) > BUF_SZ ) {
        token_hdr = TokenBufInit( token_hdr );
        tb = token_hdr->curr;
    }

    *(TOKEN *)( tb->h + tb->cur_pos ) = token;
    tb->cur_pos += sizeof( TOKEN );
    token_hdr->total_size += sizeof( TOKEN );

    return token_hdr;
}


BUFFER_HDR *TokenBufAddStr(             // TokenBuf: ADD A STRING
    BUFFER_HDR *token_hdr,              // - handle to token buffer
    const char *str )                   // - string to be added
{
    unsigned len = strlen( str );
    TOKEN_BUFFER *tb = token_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate as many
     *  buffers as needed to store string.
     */
    if( ( tb->cur_pos + len + 1) > BUF_SZ ) {
        token_hdr = TokenBufNew( token_hdr, str );
        tb = token_hdr->curr;
    } else {
        memcpy( &(tb->h[tb->cur_pos]), str, len );
        tb->cur_pos += len;
        token_hdr->total_size += len;
    }
    token_hdr = TokenBufAddChar( token_hdr, '\0' );
    return token_hdr;
}


size_t TokenBufSize(                    // TokenBuf: RETURN SIZE
    BUFFER_HDR *token_hdr )             // - handle to token buffer
{
    return token_hdr->curr->cur_pos;
}


void TokenBufRemoveWhiteSpace(          // TokenBuf: REMOVE WHITE SPACE FROM END
    BUFFER_HDR *token_hdr )             // - handle to token buffer
{
    TOKEN_BUFFER *tb = token_hdr->curr;

    while( tb->cur_pos >= sizeof( TOKEN ) ) {
        tb->cur_pos -= sizeof( TOKEN );
        if( *(TOKEN *)&tb->h[tb->cur_pos] != T_WHITE_SPACE ) {
            tb->cur_pos += sizeof( TOKEN );
            break;
        }
    }
}


BUFFER_HDR *TokenBufMove(               // TokenBuf: MOVE CONTENTS, RE-INITIALIZE
    BUFFER_HDR *token_hdr,              // - token header
    char *target )                      // - where to save
{
    TOKEN_BUFFER  *tb;

    RingIterBeg( token_hdr->list, tb ) {
        memcpy( target, tb->h, tb->cur_pos );
        target += tb->cur_pos;
    } RingIterEnd( tb )
    *target = '\0';

    TokenBufFini( token_hdr );
    token_hdr = TokenBufInit( NULL );
    return token_hdr;
}


void TokenBufFini(                      // TokenBuf: FREE MEMORY USED BY TOKEN BUFFER
    BUFFER_HDR *token_hdr )
{
    RingFree( &token_hdr->list );
    CMemFreePtr( &token_hdr );
}

BUFFER_HDR *TokenBufNew(                // TokenBuf: ADD A STRING
    BUFFER_HDR *token_hdr,              // - handle to token buffer
    const char *str )                   // - string to be added
{
    TOKEN_BUFFER *tb = token_hdr->curr;
    int   len, req_len;


    req_len = strlen( str );
    len = BUF_SZ - tb->cur_pos;
    memcpy( &(tb->h[tb->cur_pos]), str, len );
    tb->cur_pos = BUF_SZ;
    token_hdr->total_size += len;
    str += len;
    req_len -= len;

    while( req_len > 0 ) {
        token_hdr = TokenBufInit( token_hdr );
        tb = token_hdr->curr;
        if( req_len > BUF_SZ ) {
            memcpy( tb->h, str, BUF_SZ );
            tb->cur_pos = BUF_SZ;
            token_hdr->total_size += BUF_SZ;
            str += BUF_SZ;
            req_len -= BUF_SZ;
        } else {
            memcpy( tb->h, str, req_len );
            tb->cur_pos += req_len;
            token_hdr->total_size += req_len;
            req_len = 0;
        }
    }

    return token_hdr;
}

size_t TokenBufTotalSize(               // TokenBuf: RETURN SIZE OF ALL BUFFERS
    BUFFER_HDR *token_hdr )
{
   return( token_hdr->total_size );
}
