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


// VBUF.H -- DECLARATION FOR VBUF FUNCTIONS

#ifndef _VBUF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vbuf{            // VBUF structure
    char *buf;                  // - buffer pointer
    size_t len;                 // - buffer size
    size_t used;                // - amount used in buffer
} VBUF;

// PROTOTYPES:

void VbufConcVector             // CONCATENATE A VECTOR
    ( VBUF *vbuf                // - VBUF structure
    , size_t size               // - size of vector
    , char const * vect )       // - vector
;
void VbufFree(                  // FREE BUFFER
    VBUF *vbuf )                // - VBUF structure
;
void VbufInit(                  // INITIALIZE BUFFER STRUCTURE
    VBUF *vbuf )                // - VBUF structure
;
void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
;
void VbufUsed(                  // ENSURE BUFFER IS USED FOR A CERTAIN SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t used )               // - used size
;
void VStrConcChr(               // CONCATENATE CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
;
void VStrConcDecimal(           // CONCATENATE DECIMAL # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    unsigned value )            // - value to be concatenated
;
void VStrConcInteger(           // CONCATENATE INTEGER # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    int value )                 // - value to be concatenated
;
#if __WATCOMC__ >= 1100
void VStrConcI64(               // CONCATENATE I64 # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    __int64 value )             // - value to be concatenated
;
#endif
void VStrConcStr(               // CONCATENATE STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
;
void VStrConcStrRev(            // CONCATENATE STRING BACKWARDS TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated backwards
;
void VStrTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM vbuf->buf
    VBUF *vbuf )                // - VBUF structure
;
void VStrPrepChr(               // PREPEND CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
;
void VStrPrepStr(               // PREPEND STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
;
unsigned VStrLen(               // RETURN STRLEN OF BUFFER
    VBUF *vbuf )                // - VBUF structure
;
void VStrNull(                  // CLEAR BUFFER
    VBUF *vbuf )                // - VBUF structure
;
#define VbufOffset( v )         ((v)->used-1)
#define VbufRemoveOffset(v,n)   ((v)->used-=(n))

#ifdef __cplusplus
};
#endif

#define _VBUF_H
#endif
