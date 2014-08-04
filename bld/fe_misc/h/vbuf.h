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
* Description:  Declaration for VBUF functions.
*
****************************************************************************/


#ifndef _VBUF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vbuf{            // VBUF structure
    char           *buf;        // - buffer pointer
    size_t         len;         // - buffer size
    size_t         used;        // - amount used in buffer
} VBUF;

// PROTOTYPES:

void VbufConcVbuf               // CONCATENATE A VBUF
    ( VBUF *vbuf1               // - VBUF structure
    , VBUF *vbuf2 )             // - VBUF structure
;
void VbufConcVbufRev            // CONCATENATE A VBUF2 BACKWARDS TO VBUF1
    ( VBUF *vbuf1               // - VBUF structure
    , VBUF *vbuf2 )             // - VBUF structure to be concatenated backwards
;
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
void VbufPrepVbuf               // PREPEND A VBUF TO VBUF
    ( VBUF *vbuf1               // - VBUF structure
    , VBUF *vbuf2 )             // - VBUF structure to be prepended
;
void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
;
void VbufSetLen(                // SET BUFFER LENGTH
    VBUF *vbuf,                 // - VBUF structure
    size_t size )               // - new length
;
void VbufConcChr(               // CONCATENATE CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
;
void VbufConcDecimal(           // CONCATENATE DECIMAL # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    unsigned value )            // - value to be concatenated
;
void VbufConcInteger(           // CONCATENATE INTEGER # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    int value )                 // - value to be concatenated
;
void VbufConcI64(               // CONCATENATE I64 # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    signed_64 value )           // - value to be concatenated
;
void VbufConcStr(               // CONCATENATE STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
;
void VbufConcStrRev(            // CONCATENATE STRING BACKWARDS TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated backwards
;
void VbufTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM vbuf->buf
    VBUF *vbuf )                // - VBUF structure
;
void VbufPrepChr(               // PREPEND CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
;
void VbufPrepStr(               // PREPEND STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
;
#define VbufLen(v)              ((v)->used)                 // RETURN LENGTH OF BUFFER
;
#define VbufRewind(v)           VbufSetLen(v,0)             // CLEAN BUFFER
;
#define VbufSetPosBack(v,n)     VbufSetLen(v,(v)->used-n)   // SHORTEN BUFFER LENGTH
;
#define VbufBuffer(v)           ((unsigned char *)(v)->buf) // RETURN POINTER OF BUFFER
;
#define VbufString(v)           ((v)->buf)                  // RETURN POINTER OF BUFFER
;
#define VbufSize(v)             ((v)->len)                  // RETURN SIZE OF BUFFER
;

#ifdef __cplusplus
};
#endif

#define _VBUF_H
#endif
