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

/*
 * =========================================================================
 * UTILITY
 * =========================================================================
 */

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

char Lower( char c ) {
/********************/

    if( ( c >= 'A' ) && ( c <= 'Z' ) ) return( c | ' ' );
    return( c );
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

    int written;
    char c;

    c = ch;
    DosWrite( 1, (char far *)&c, 1, (int PASPTR *)&written );
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
    KbdCharIn( (KBDCHAR PASPTR *)&KbdChar, 0, 0 );
    PutString( "\r              \r" );
    return( KbdChar.ascii != 'n' && KbdChar.ascii != 'N' );
}


char *EatWhite( char *word ) {
/****************************/

    while( *word == ' ' ) {
        ++word;
    }
    return( word );
}

SavePrompt( char PASPTR *line ) {
/*******************************/

    int         len;

    len = StartCol;
    VioReadCharStr( line, (int PASPTR *)&len, Row, 0, 0 );
}


RestorePrompt( char PASPTR *line ) {
/**********************************/

    int         col;

    VioGetCurPos( (int PASPTR *)&Row, (int PASPTR *)&col, 0 );
    VioWrtCharStr( line, StartCol, Row, 0, 0 );
}


void SetCursorType() {
/****************/

    VioSetCurType( (CURSOR PASPTR *)&Cur, 0 );
}


char far *GetEnv( char far *name, int len ) {
/*******************************************/

    char far *environ;
    unsigned envoff, envseg;

    if( DosGetEnv( &envseg, &envoff ) != 0 ) return( 0 );
    environ = MK_FP( envseg, 0 );
    while( !_null( *environ ) ) {
        if( Equal( environ, name, len ) ) return( environ + len );
        while( !_null( *environ ) ) ++environ;
        ++environ;
    }
    return( 0 );
}
