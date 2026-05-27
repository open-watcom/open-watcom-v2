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
    BUFFER_HDR *tokenbuf_hdr )
{
    TOKEN_BUFFER    *tokenbuf;


    /*
     *  Allocate tokenBuffer header structure
     */
    if( tokenbuf_hdr == NULL ) {
        tokenbuf_hdr = CMemAlloc( sizeof( *tokenbuf_hdr ));
        tokenbuf_hdr->no_of_buffers = 0;
        tokenbuf_hdr->total_size = 0;
        tokenbuf_hdr->list = NULL;
        tokenbuf_hdr->curr = NULL;
    }


    /*
     *  Allocate tokenBuffer list structure
     */
    tokenbuf = CMemAlloc( sizeof( *tokenbuf ));
    tokenbuf->cur_pos = 0;
    tokenbuf->h[0] = '\0';


    /*
     *  Add tokenBuffer entry to the end of the token list
     *  in tokenbuf_hdr structure.
     */
    RingAppend( &tokenbuf_hdr->list, tokenbuf );
    tokenbuf_hdr->curr = tokenbuf;


    ++(tokenbuf_hdr->no_of_buffers);
    return( tokenbuf_hdr );
}


BUFFER_HDR *TokenBufAddChar(            // TokenBuf: ADD A CHARACTER
    BUFFER_HDR *tokenbuf_hdr,           // - handle to token buffer
    char character )                    // - character to be added
{
    TOKEN_BUFFER *tokenbuf = tokenbuf_hdr->curr;


    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tokenbuf->cur_pos + 1) > BUF_SZ ) {
        tokenbuf_hdr = TokenBufInit( tokenbuf_hdr );
        tokenbuf = tokenbuf_hdr->curr;
    }

    tokenbuf->h[tokenbuf->cur_pos++] = character;
    ++tokenbuf_hdr->total_size;

    return( tokenbuf_hdr );
}

BUFFER_HDR *TokenBufAddToken(           // TokenBuf: ADD A TOKEN
    BUFFER_HDR *tokenbuf_hdr,           // - handle to token buffer
    TOKEN token )                       // - token to be added
{
    TOKEN_BUFFER *tokenbuf = tokenbuf_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate new one.
     */
    if( ( tokenbuf->cur_pos + SIZE_MTOKEN ) > BUF_SZ ) {
        tokenbuf_hdr = TokenBufInit( tokenbuf_hdr );
        tokenbuf = tokenbuf_hdr->curr;
    }

    SET_MTOKEN( tokenbuf->h + tokenbuf->cur_pos, token );
    tokenbuf->cur_pos += SIZE_MTOKEN;
    tokenbuf_hdr->total_size += SIZE_MTOKEN;

    return( tokenbuf_hdr );
}


BUFFER_HDR *TokenBufAddStr(             // TokenBuf: ADD A STRING
    BUFFER_HDR *tokenbuf_hdr,           // - handle to token buffer
    const char *str )                   // - string to be added
{
    unsigned len = strlen( str );
    TOKEN_BUFFER *tokenbuf = tokenbuf_hdr->curr;

    /*
     *  If not enough space on current buffer, allocate as many
     *  buffers as needed to store string.
     */
    if( ( tokenbuf->cur_pos + len + 1) > BUF_SZ ) {
        tokenbuf_hdr = TokenBufNew( tokenbuf_hdr, str );
        tokenbuf = tokenbuf_hdr->curr;
    } else {
        memcpy( &(tokenbuf->h[tokenbuf->cur_pos]), str, len );
        tokenbuf->cur_pos += len;
        tokenbuf_hdr->total_size += len;
    }
    tokenbuf_hdr = TokenBufAddChar( tokenbuf_hdr, '\0' );
    return( tokenbuf_hdr );
}


size_t TokenBufSize(                    // TokenBuf: RETURN SIZE
    BUFFER_HDR *tokenbuf_hdr )          // - handle to token buffer
{
    return( tokenbuf_hdr->curr->cur_pos );
}


void TokenBufRemoveWhiteSpace(          // TokenBuf: REMOVE WHITE SPACE FROM END
    BUFFER_HDR *tokenbuf_hdr )          // - handle to token buffer
{
    TOKEN_BUFFER *tokenbuf = tokenbuf_hdr->curr;

    while( tokenbuf->cur_pos >= SIZE_MTOKEN ) {
        tokenbuf->cur_pos -= SIZE_MTOKEN;
        if( GET_MTOKEN( &tokenbuf->h[tokenbuf->cur_pos] ) != T_WHITE_SPACE ) {
            tokenbuf->cur_pos += SIZE_MTOKEN;
            break;
        }
    }
}


BUFFER_HDR *TokenBufMove(               // TokenBuf: MOVE CONTENTS, RE-INITIALIZE
    BUFFER_HDR *tokenbuf_hdr,           // - token header
    char *target )                      // - where to save
{
    TOKEN_BUFFER  *tokenbuf;

    RingIterBeg( tokenbuf_hdr->list, tokenbuf ) {
        memcpy( target, tokenbuf->h, tokenbuf->cur_pos );
        target += tokenbuf->cur_pos;
    } RingIterEnd( tokenbuf )
    *target = '\0';

    TokenBufFini( tokenbuf_hdr );
    tokenbuf_hdr = TokenBufInit( NULL );
    return( tokenbuf_hdr );
}


void TokenBufFini(                      // TokenBuf: FREE MEMORY USED BY TOKEN BUFFER
    BUFFER_HDR *tokenbuf_hdr )
{
    RingFree( &tokenbuf_hdr->list );
    CMemFreePtr( &tokenbuf_hdr );
}

BUFFER_HDR *TokenBufNew(                // TokenBuf: ADD A STRING
    BUFFER_HDR *tokenbuf_hdr,           // - handle to token buffer
    const char *str )                   // - string to be added
{
    TOKEN_BUFFER *tokenbuf = tokenbuf_hdr->curr;
    int   len, req_len;


    req_len = strlen( str );
    len = BUF_SZ - tokenbuf->cur_pos;
    memcpy( &(tokenbuf->h[tokenbuf->cur_pos]), str, len );
    tokenbuf->cur_pos = BUF_SZ;
    tokenbuf_hdr->total_size += len;
    str += len;
    req_len -= len;

    while( req_len > 0 ) {
        tokenbuf_hdr = TokenBufInit( tokenbuf_hdr );
        tokenbuf = tokenbuf_hdr->curr;
        if( req_len > BUF_SZ ) {
            memcpy( tokenbuf->h, str, BUF_SZ );
            tokenbuf->cur_pos = BUF_SZ;
            tokenbuf_hdr->total_size += BUF_SZ;
            str += BUF_SZ;
            req_len -= BUF_SZ;
        } else {
            memcpy( tokenbuf->h, str, req_len );
            tokenbuf->cur_pos += req_len;
            tokenbuf_hdr->total_size += req_len;
            req_len = 0;
        }
    }

    return( tokenbuf_hdr );
}

size_t TokenBufTotalSize(               // TokenBuf: RETURN SIZE OF ALL BUFFERS
    BUFFER_HDR *tokenbuf_hdr )
{
   return( tokenbuf_hdr->total_size );
}
