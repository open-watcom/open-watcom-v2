/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include "initdefs.h"
#include "preproc.h"
#include "cscanbuf.h"


static size_t       BufferSize;

static void EnlargeBuffer( size_t size )
/**************************************/
{
    char    *newBuffer;

//    size += 32;     /* Buffer size margin */
    if( size > BufferSize ) {
        size = _RoundUp( size, BUF_SIZE );
        newBuffer = CMemAlloc( size );
        memcpy( newBuffer, Buffer, BufferSize );
        CMemFree( (void *)Buffer );
        Buffer = newBuffer;
        BufferSize = size;
    }
}

void WriteBufferChar( int c )
/***************************/
{
    EnlargeBuffer( TokenLen + 1 );
    Buffer[TokenLen++] = (char)c;
}

int WriteBufferCharNextChar( int c )
/**********************************/
{
    EnlargeBuffer( TokenLen + 1 );
    Buffer[TokenLen++] = (char)c;
    return( NextChar() );
}

void WriteBufferNullChar( void )
/******************************/
{
    EnlargeBuffer( TokenLen + 1 );
    Buffer[TokenLen] = '\0';
}

void WriteBufferStr( const char *src )
/************************************/
{
    size_t  len;

    len = strlen( src );
    EnlargeBuffer( TokenLen + len + 1 );
    memcpy( Buffer + TokenLen, src, len + 1 );
    TokenLen += len;
}


void WriteBufferMem( const char *src, size_t len )
/************************************************/
{
    EnlargeBuffer( TokenLen + len );
    memcpy( Buffer + TokenLen, src, len );
    TokenLen += len;
}

size_t WriteBufferPosChar( size_t pos, int c )
/********************************************/
{
    EnlargeBuffer( pos + 1 );
    Buffer[pos++] = c;
    return( pos );
}

void WriteBufferPosNullChar( size_t pos )
/***************************************/
{
    EnlargeBuffer( pos + 1 );
    Buffer[pos] = '\0';
}

size_t WriteBufferPosStr( size_t pos, const char *src )
/*****************************************************/
{
    size_t  len;

    len = strlen( src );
    EnlargeBuffer( pos + len + 1 );
    memcpy( Buffer + pos, src, len + 1 );
    return( pos + len );
}

size_t WriteBufferPosEscStr( size_t pos, const char **src, bool quote )
/*********************************************************************/
{
    const char  *p;
    char        c;

    p = *src;
    while( (c = *p++) != '\0' ) {
        EnlargeBuffer( pos + 1 );
        if( c == '\\' || quote && c == '"' ) {
            Buffer[pos++] = '\\';
            EnlargeBuffer( pos + 1 );
        }
        Buffer[pos++] = c;
    }
    *src = p;
    EnlargeBuffer( pos + 1 );
    Buffer[pos] = '\0';
    return( pos );
}

static void bufInit(            // INITIALIZATION
    INITFINI *defn )            // - definition
{
    /* unused parameters */ (void)defn;

    BufferSize = _RoundUp( BUF_SIZE + 16 + sizeof( uint_32 ), BUF_SIZE );
    Buffer = CMemAlloc( BufferSize );
}

static void bufFini(            // COMPLETION
    INITFINI *defn )            // - definition
{
    /* unused parameters */ (void)defn;

    CMemFree( Buffer );
    Buffer = NULL;
    BufferSize = 0;
}

INITDEFN( membuf, bufInit, bufFini )

