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


#include "cmdedit.h"

static char Lower( char c ) {
/***************************/

    if( ( c >= 'A' ) && ( c <= 'Z' ) ) return( c | ' ' );
    return( c );
}

int Equal( char far * str1, char far * str2, int len ) {
/******************************************************/

    while( --len >= 0 ) {
        if( *str1 == ' ' && *str2 == '\t' ) {
            /**/
        } else if( *str2 == ' ' && *str1 == '\t' ) {
            /**/
        } else {
            if( Lower( *str1 ) != Lower( *str2 ) ) return( FALSE );
        }
        ++str1;
        ++str2;
    }
    return( TRUE );
}

void ZapLower( char far *str ) {
/**************************/

    while( *str != '\0' && *str != '\r' ) {
        *str = Lower( *str );
        ++str;
    }
}


void PutChar( char ch ) {
/***********************/

    USHORT written;
    char c;

    c = ch;
    DosWrite( 1, &c, 1, &written );
}


void PutPad( char far * str, int len ) {
/**************************************/

    while( *str ) {
        PutChar( *str );
        --len;
        ++str;
    }
    while( len > 0 ) {
        PutChar( ' ' );
        --len;
    }
}


void PutString( char far * str ) {
/********************************/

    PutPad( str, -1 );
}


void PutNL() {
/************/

    PutString( "\r\n" );
}


int PutMore() {
/**************/

    PutString( "\r... More [y|n]\r" );
    KbdCharIn( &KbdChar, 0, 0 );
    PutString( "\r              \r" );
    return( KbdChar.chChar != 'n' && KbdChar.chChar != 'N' );
}


char *EatWhite( char *word ) {
/****************************/

    while( *word == ' ' ) {
        ++word;
    }
    return( word );
}

void SavePrompt( char PASPTR *line ) {
/*******************************/

    USHORT      len;

    len = StartCol;
    VioReadCharStr( line, &len, Row, 0, 0 );
}


void RestorePrompt( char PASPTR *line ) {
/**********************************/

    USHORT      col;

    VioGetCurPos( &Row, &col, 0 );
    VioWrtCharStr( line, StartCol, Row, 0, 0 );
}


void SetCursorType() {
/****************/

    VioSetCurType( &Cur, 0 );
}


char far *GetEnv( char far *name, int len ) {
/*******************************************/

    char far *environ;
    USHORT envoff, envseg;

    if( DosGetEnv( &envseg, &envoff ) != 0 ) return( 0 );
    environ = MK_FP( envseg, 0 );
    while( !_null( *environ ) ) {
        if( Equal( environ, name, len ) ) return( environ + len );
        while( !_null( *environ ) ) ++environ;
        ++environ;
    }
    return( 0 );
}
