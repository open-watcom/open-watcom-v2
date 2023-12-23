/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2022 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  C lexical analyzer token buffer handling.
*
****************************************************************************/


#include "cvars.h"
#include "cscanbuf.h"


static size_t   BufferSize;

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

void WriteBufferString( const char *s )
/*************************************/
{
    char    c;

    EnlargeBuffer( TokenLen + strlen( s ) + 1 );
    while( (c = *s++) != '\0' ) {
        Buffer[TokenLen++] = c;
    }
    Buffer[TokenLen] = '\0';
}

void WriteBufferEscStr( const char **src, bool quote )
/****************************************************/
{
    const char  *p;
    char        c;

    p = *src;
    while( (c = *p++) != '\0' ) {
        EnlargeBuffer( TokenLen + 1 );
        if( c == '\\' || quote && c == '"' ) {
            Buffer[TokenLen++] = '\\';
            EnlargeBuffer( TokenLen + 1 );
        }
        Buffer[TokenLen++] = c;
    }
    *src = p;
    EnlargeBuffer( TokenLen + 1 );
    Buffer[TokenLen] = '\0';
}

void InitBuffer( size_t size )
/****************************/
{
    Buffer = CMemAlloc( size );
    BufferSize = size;
}

void FiniBuffer( void )
/*********************/
{
    CMemFree( Buffer );
    Buffer = NULL;
    BufferSize = 0;
}
