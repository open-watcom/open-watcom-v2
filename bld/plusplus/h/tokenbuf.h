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


#ifndef _TOKENBUF_H
#define _TOKENBUF_H

#ifndef NDEBUG
#define BUF_SZ  3
#else
#define BUF_SZ  32
#endif

typedef struct token_buffer TOKEN_BUFFER;
struct token_buffer {
    TOKEN_BUFFER        *next;
    unsigned            cur_pos;
    char                h[BUF_SZ];
};

typedef struct buffer_hdr {
    unsigned            no_of_buffers;
    unsigned long       total_size;
    TOKEN_BUFFER        *list;
    TOKEN_BUFFER        *curr;
} BUFFER_HDR;


BUFFER_HDR *TokenBufAddChar(          // TokenBuf: ADD A CHARACTER
    BUFFER_HDR *,                     // - handle to token buffer
    char character )                   // - character to be added
;
BUFFER_HDR *TokenBufAddWhiteSpace(    // TokenBuf: ADD A CHARACTER
    BUFFER_HDR *,                     // - handle to token buffer
    char character )                   // - character to be added
;
BUFFER_HDR *TokenBufAddStr(           // TokenBuf: ADD A STRING
    BUFFER_HDR *,                     // - handle to token buffer
    const char *str )                 // - string to be added
;
BUFFER_HDR *TokenBufInit(             // TokenBuf: INITIALIZE FOR COLLECTION
    BUFFER_HDR * )                    // - handle to token buffer
;
BUFFER_HDR *TokenBufMove(             // TokenBuf: MOVE CONTENTS, RE-INITIALIZE
    BUFFER_HDR *token_hdr,            // - token header
    char *target )                    // - where to save
;
void TokenBufRemoveWhiteSpace(        // TokenBuf: REMOVE WHITE SPACE FROM END
    BUFFER_HDR * )                    // - handle to token buffer
;
size_t TokenBufSize(                  // TokenBuf: RETURN SIZE
    BUFFER_HDR * )                    // - handle to token buffer
;
void TokenBufFini(                    // TokenBuf: FREE TOKEN BUF MEMORY
    BUFFER_HDR *token_hdr )           // - token header
;
BUFFER_HDR *TokenBufNew(              // TokenBuf: ADD A STRING
    BUFFER_HDR *,                     // - handle to token buffer
    const char *str )                 // - string to be added
;
size_t TokenBufTotalSize(             // TokenBuf: RETURN SIZE
    BUFFER_HDR * )
;
#endif
