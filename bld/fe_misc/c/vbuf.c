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


#include <string.h>
#include <ctype.h>
#include header
#include "vbuf.h"

#ifdef fe_cfg
#    include "fe_cfg.h"
#endif

#define MIN_VBUF_INC 32     // minimum increment allocated (power of 2)


// ************************************
// Vbuf functions that manage size only
// ************************************

void VbufInit(                  // INITIALIZE BUFFER STRUCTURE
    VBUF *vbuf )                // - VBUF structure
{
    vbuf->buf = NULL;
    vbuf->len = 0;
    vbuf->used = 0;
}


void VbufFree(                  // FREE BUFFER
    VBUF *vbuf )                // - VBUF structure
{
    _MemoryFree( vbuf->buf );
    VbufInit( vbuf );
}


void VbufUsed(                  // ENSURE BUFFER IS USED FOR A CERTAIN SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t used )               // - used size
{
    vbuf->used = used;
}


void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
{
    char *old_buffer;           // - old buffer

    if( reqd > vbuf->len ) {
        reqd = ( reqd + ( MIN_VBUF_INC - 1 ) ) & ( - MIN_VBUF_INC );
        old_buffer = vbuf->buf;
        vbuf->buf = _MemoryAllocate( reqd );
        vbuf->len = reqd;
        if( old_buffer != NULL ) {
            memcpy( vbuf->buf, old_buffer, vbuf->used );
            _MemoryFree( old_buffer );
        }
    }
}


void VbufConcVector             // CONCATENATE A VECTOR
    ( VBUF *vbuf                // - VBUF structure
    , size_t size               // - size of vector
    , char const * vect )       // - vector
{
    VbufReqd( vbuf, size );
    memcpy( &vbuf->buf[ vbuf->used ], vect, size );
    vbuf->used += size;
}


// *************************************************
// Vbuf functions which treat the buffer as a string
// *************************************************

void VStrNull(                  // CLEAR BUFFER
    VBUF *vbuf )                // - VBUF structure
{
    VbufReqd( vbuf, 1 );
    vbuf->buf[0] = '\0';
    vbuf->used = 1;
}


void VStrConcStr(               // CONCATENATE STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
{
    size_t new_size;            // - size of concatenated strings
    size_t conc_size;           // - size of concatenation string

    conc_size = strlen( string );
    new_size = vbuf->used + conc_size;
    VbufReqd( vbuf, new_size );
    stvcpy( vbuf->buf + vbuf->used - 1, string, conc_size );
    vbuf->used = new_size;
}


void VStrConcDecimal(           // CONCATENATE DECIMAL # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    unsigned value )            // - value to be concatenated
{
    char buffer[16];            // - temp buffer

    stdcpy( buffer, value );
    VStrConcStr( vbuf, buffer );
}


void VStrConcInteger(           // CONCATENATE INTEGER # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    int value )                 // - value to be concatenated
{
    char buffer[16];            // - temp buffer

    sticpy( buffer, value );
    VStrConcStr( vbuf, buffer );
}

#ifdef FE_I64_MEANINGLESS
void VStrConcI64(               // CONCATENATE I64 # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    __int64 value )             // - value to be concatenated
{
    char buffer[16];            // - temp buffer

    sti64cpy( buffer, value );
    VStrConcStr( vbuf, buffer );
}
#endif

void VStrConcChr(               // CONCATENATE CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
{
#if 0
    char buffer[ 2 ];           // buffer passed to VStrConcStr

    buffer[ 0 ] = chr;
    buffer[ 1 ] = '\0';
    VStrConcStr( vbuf, buffer );
#else
    char* tgt;                  // - target location
    size_t reqd;                // - size afterwards

    reqd = vbuf->used + 1;
    if( reqd > vbuf->len ) {
        VbufReqd( vbuf, reqd );
        VStrConcChr( vbuf, chr );
    } else {
        vbuf->used = reqd;
        tgt = vbuf->buf + reqd - 2;
        tgt[0] = chr;
        tgt[1] = '\0';
    }
#endif
}


void VStrConcStrRev(            // CONCATENATE STRING BACKWARDS TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated backwards
{
    size_t new_size;            // - size of concatenated strings
    size_t conc_size;           // - size of concatenation string

    conc_size = strlen( string );
    new_size = vbuf->used + conc_size;
    VbufReqd( vbuf, new_size );
    stvcpyr( vbuf->buf + vbuf->used - 1, string, conc_size );
    vbuf->used = new_size;
}


unsigned VStrLen(               // RETURN STRLEN OF BUFFER
    VBUF *vbuf )                // - VBUF structure
{
    return vbuf->used - 1;
}


void VStrTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM vbuf->buf
    VBUF *vbuf )                // - VBUF structure
{
    char    *ptr;
    size_t size;

    size = vbuf->used;
    if( size > 1 ) {
        ptr = vbuf->buf + size - 2;
        while( isspace( *ptr ) ) {
            *ptr-- = '\0';
            vbuf->used--;
        }
    }
}

void VStrPrepStr(               // PREPEND STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
{
    size_t prep_size;
    size_t new_size;
    VBUF   temp;

    prep_size = strlen( string );
    new_size = prep_size + vbuf->used;
    VbufInit( &temp );
    VbufReqd( &temp, new_size );
    stvcpy( temp.buf, string, prep_size );
    stvcpy( temp.buf + prep_size, vbuf->buf, vbuf->used-1 );
    temp.used = new_size;
    VbufFree( vbuf );
    *vbuf = temp;
}


void VStrPrepChr(               // PREPEND CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
{
    char buffer[ 2 ];           // buffer passed to VStrPrepStr

    buffer[ 0 ] = chr;
    buffer[ 1 ] = '\0';
    VStrPrepStr( vbuf, buffer );
}
