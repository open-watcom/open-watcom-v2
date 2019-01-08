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
* Description:  dynamic string (VBUF) related declarations
*
****************************************************************************/


typedef struct vbuf{            // VBUF structure
    char           *buf;        // - buffer pointer
    size_t         len;         // - buffer size
    size_t         used;        // - amount used in buffer
} VBUF;

// PROTOTYPES:

void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
;
void VbufSetLen(                // SET BUFFER LENGTH
    VBUF *vbuf,                 // - VBUF structure
    size_t size )               // - new length
;
void VbufInit(                  // INITIALIZE BUFFER STRUCTURE
    VBUF *vbuf )                // - VBUF structure
;
void VbufFree(                  // FREE BUFFER
    VBUF *vbuf )                // - VBUF structure
;
int VbufComp(                   // COMPARE A VBUF
    VBUF *vbuf1,                // - VBUF structure
    VBUF *vbuf2,                // - VBUF structure
    bool igncase )              // - bool ignore case
;
void VbufConcVbuf(              // CONCATENATE A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    VBUF *vbuf2 )               // - VBUF structure
;
void VbufPrepVbuf(              // PREPEND A VBUF TO VBUF
    VBUF *vbuf1,                // - VBUF structure
    VBUF *vbuf2 )               // - VBUF structure to be prepended
;
void VbufSetVbuf(               // SET A VBUF
    VBUF *vbuf1,                // - VBUF structure
    VBUF *vbuf2 )               // - VBUF structure
;
void VbufConcBuffer(            // CONCATENATE A BUFFER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    size_t size,                // - size of buffer
    char const *buffer )        // - buffer
;
void VbufPrepBuffer(            // PREPEND A BUFFER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    size_t size,                // - size of buffer
    char const *buffer )        // - buffer
;
void VbufSetBuffer(             // SET A BUFFER
    VBUF *vbuf,                 // - VBUF structure
    size_t size,                // - size of buffer
    char const *buffer )        // - buffer
;
void VbufConcStr(               // CONCATENATE A STRING TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
;
void VbufPrepStr(               // PREPEND STRING TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
;
void VbufSetStr(                // SET STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
;
void VbufConcChr(               // CONCATENATE A CHAR TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
;
void VbufPrepChr(               // PREPEND CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
;
void VbufSetChr(                // CONCATENATE CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
;
#if 0
void VbufConcDecimal(           // CONCATENATE A DECIMAL TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    unsigned value )            // - value to be concatenated
;
void VbufConcInteger(           // CONCATENATE A INTEGER TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    int value )                 // - value to be concatenated
;
void VbufConcI64(               // CONCATENATE A I64 TO VBUF
    VBUF *vbuf,                 // - VBUF structure
    signed_64 value )           // - value to be concatenated
;
#endif
void VbufTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM vbuf->buf
    VBUF *vbuf )                // - VBUF structure
;
void VbufAddDirSep(             // TERMINATE A VBUF AS PATH BY DIR_SEP
    VBUF *vbuf )                // - VBUF structure
;
void VbufRemDirSep(             // REMOVE DIR_SEP FROM A VBUF END
    VBUF *vbuf )                // - VBUF structure
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
