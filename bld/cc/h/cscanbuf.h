/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  C lexical analyzer token buffer handling.
*
****************************************************************************/


extern void     WriteBufferChar( int c );
extern void     WriteBufferNullChar( void );
extern int      WriteBufferCharNextChar( int c );
extern size_t   WriteBufferPosEscStr( size_t pos, const char **src, bool quote );
extern void     InitBuffer( size_t size );
extern void     FiniBuffer( void );
