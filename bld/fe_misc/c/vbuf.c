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

#define BUFFER_SIZE(x)   ((x+1+(MIN_VBUF_INC-1))&(-MIN_VBUF_INC))
#define FREE_BUFFER(x)   {if(x->len>1)_MemoryFree(x->buf);}

// ************************************
// Vbuf functions that manage size only
// ************************************

void VbufInit(                  // INITIALIZE BUFFER STRUCTURE
    VBUF *vbuf )                // - VBUF structure
{
    vbuf->buf = "";
    vbuf->len = 1;
    vbuf->used = 0;
}


void VbufFree(                  // FREE BUFFER
    VBUF *vbuf )                // - VBUF structure
{
    FREE_BUFFER( vbuf );
    VbufInit( vbuf );
}


void VbufReqd(                  // ENSURE BUFFER IS OF SUFFICIENT SIZE
    VBUF *vbuf,                 // - VBUF structure
    size_t reqd )               // - required size
{
    char    *new_buffer;        // - old buffer

    if( reqd >= vbuf->len ) {
        reqd = BUFFER_SIZE( reqd );
        new_buffer = _MemoryAllocate( reqd );
        stvcpy( new_buffer, vbuf->buf, vbuf->used );
        FREE_BUFFER( vbuf );
        vbuf->buf = new_buffer;
        vbuf->len = reqd;
    }
}


void VbufSetLen(                // SET BUFFER LENGTH
    VBUF *vbuf,                 // - VBUF structure
    size_t size )               // - new length
{
    if( vbuf->len > 1 ) {
        vbuf->used = size;
        vbuf->buf[size] = '\0';
    }
}


// ****************************************************
// Vbuf functions which manipulate with data in buffer
// ****************************************************

void VbufConcVector             // CONCATENATE A VECTOR
    ( VBUF *vbuf                // - VBUF structure
    , size_t size               // - size of vector
    , char const * vect )       // - vector
{
    if( size > 0 ) {
        VbufReqd( vbuf, vbuf->used + size );
        stvcpy( vbuf->buf + vbuf->used, vect, size );
        vbuf->used += size;
    }
}


void VbufConcVbuf               // CONCATENATE A VBUF
    ( VBUF *vbuf1               // - VBUF structure
    , VBUF *vbuf2 )             // - VBUF structure
{
    if( vbuf2->used > 0 ) {
        VbufReqd( vbuf1, vbuf1->used + vbuf2->used );
        stvcpy( vbuf1->buf + vbuf1->used, vbuf2->buf, vbuf2->used );
        vbuf1->used += vbuf2->used;
    }
}


void VbufConcVbufRev(           // CONCATENATE STRING BACKWARDS TO vbuf->buf
    VBUF *vbuf1,                // - VBUF structure
    VBUF *vbuf2 )               // - VBUF structure to be concatenated backwards
{
    if( vbuf2->used > 0 ) {
        VbufReqd( vbuf1, vbuf1->used + vbuf2->used );
        stvcpyr( vbuf1->buf + vbuf1->used, vbuf2->buf, vbuf2->used );
        vbuf1->used += vbuf2->used;
    }
}


void VbufPrepVbuf               // PREPEND A VBUF TO VBUF
    ( VBUF *vbuf1               // - VBUF structure
    , VBUF *vbuf2 )             // - VBUF structure to be prepended
{
    VBUF    temp;

    if( vbuf2->used > 0 ) {
        VbufInit( &temp );
        VbufReqd( &temp, vbuf1->used + vbuf2->used );
        stvcpy( temp.buf, vbuf2->buf, vbuf2->used );
        stvcpy( temp.buf + vbuf2->used, vbuf1->buf, vbuf1->used );
        temp.used = vbuf1->used + vbuf2->used;
        VbufFree( vbuf1 );
        *vbuf1 = temp;
    }
}


void VbufConcStr(               // CONCATENATE STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated
{
    VbufConcVector( vbuf, strlen( string ), string );
}


void VbufConcDecimal(           // CONCATENATE DECIMAL # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    unsigned long value )       // - value to be concatenated
{
    char    buffer[16];         // - temp buffer

    stdcpy( buffer, value );
    VbufConcStr( vbuf, buffer );
}


void VbufConcInteger(           // CONCATENATE INTEGER # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    long value )                // - value to be concatenated
{
    char    buffer[16];         // - temp buffer

    sticpy( buffer, value );
    VbufConcStr( vbuf, buffer );
}

void VbufConcI64(               // CONCATENATE I64 # TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    signed_64 value )           // - value to be concatenated
{
    char    buffer[25];         // - temp buffer

    sti64cpy( buffer, value );
    VbufConcStr( vbuf, buffer );
}

void VbufConcChr(               // CONCATENATE CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be concatenated
{
    VbufReqd( vbuf, vbuf->used + 1 );
    vbuf->buf[ vbuf->used++ ] = chr;
    vbuf->buf[ vbuf->used ] = '\0';
}


void VbufConcStrRev(            // CONCATENATE STRING BACKWARDS TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be concatenated backwards
{
    size_t  conc_size;          // - size of concatenation string

    conc_size = strlen( string );
    if( conc_size > 0 ) {
        VbufReqd( vbuf, vbuf->used + conc_size );
        stvcpyr( vbuf->buf + vbuf->used, string, conc_size );
        vbuf->used += conc_size;
    }
}


void VbufTruncWhite(            // TRUNCATE TRAILING WHITESPACE FROM vbuf->buf
    VBUF *vbuf )                // - VBUF structure
{
    char    *ptr;

    if( vbuf->used > 0 ) {
        ptr = vbuf->buf + vbuf->used - 1;
        while( isspace( *ptr ) ) {
            *ptr-- = '\0';
            vbuf->used--;
        }
    }
}

void VbufPrepStr(               // PREPEND STRING TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char const *string )        // - string to be prepended
{
    size_t  prep_size;
    VBUF    temp;

    prep_size = strlen( string );
    if( prep_size > 0 ) {
        VbufInit( &temp );
        VbufReqd( &temp, prep_size + vbuf->used );
        stvcpy( temp.buf, string, prep_size );
        stvcpy( temp.buf + prep_size, vbuf->buf, vbuf->used );
        temp.used = prep_size + vbuf->used;
        VbufFree( vbuf );
        *vbuf = temp;
    }
}


void VbufPrepChr(               // PREPEND CHAR TO vbuf->buf
    VBUF *vbuf,                 // - VBUF structure
    char chr )                  // - char to be prepended
{
    char    buffer[ 2 ];        // buffer passed to VbufPrepStr

    buffer[ 0 ] = chr;
    buffer[ 1 ] = '\0';
    VbufPrepStr( vbuf, buffer );
}
