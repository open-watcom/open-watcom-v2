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

extern int      PrevCmd( char * );
extern void     SetCursorType();
extern void     FlipScreenCursor();
extern int      NextCmd( char * );

void ToFirstCmd() {
/************************/

    while( MaxCursor = PrevCmd( Line ) );
}


void ToLastCmd() {
/***********************/

    while( MaxCursor = NextCmd( Line ) );
}


extern int WordSep( char ch ) {
/*****************************/

    static char sep[] = { " \t:;,.<>/?'\"{}[]|\\`~!@$%^&*()-+=" };
    char *psep;

    if( _null( ch ) ) return( 1 );
    for( psep = sep; !_null( *psep ); ++psep ) {
        if( *psep == ch ) return( 1 );
    }
    return( 0 );
}

void Left() {
/***********/

    if( Cursor != 0 ) --Cursor;
}

int LocateLeftWord() {
/********************/

    int cursor;

    if( Cursor == 0 ) return( 0 );
    cursor = Cursor;
    --cursor;
    for(;;) {
        if( cursor == 0 ) break;
        if( !WordSep( Line[ cursor ] ) ) break;
        --cursor;
    }
    for(;;) {
        if( cursor == 0 ) break;
        if( WordSep( Line[ cursor ] ) ) {
            ++cursor;
            break;
        }
        --cursor;
    }
    return( cursor );
}

void LeftWord() {
/***************/

    Cursor = LocateLeftWord();
}

void DeleteBOW() {
/****************/

    int cursor;

    cursor = LocateLeftWord();
    while( Cursor != cursor ) BackSpace();
}

void Right() {
/************/

    if( Cursor < Overflow ) {
        ++Cursor;
        if( Cursor > MaxCursor ) {
            Line[ MaxCursor ] = ' ';
        }
    }
}

int LocateRightWord() {
/*********************/

    int cursor;

    cursor = Cursor;
    for(;;) {
        if( cursor == MaxCursor ) break;
        if( WordSep( Line[ cursor ] ) ) break;
        ++cursor;
    }
    for(;;) {
        if( cursor == MaxCursor ) break;
        if( !WordSep( Line[ cursor ] ) ) break;
        ++cursor;
    }
    return( cursor );
}

void RightWord() {
/****************/

    Cursor = LocateRightWord();
}

void DeleteEOW() {
/****************/

    int cursor;

    cursor = LocateRightWord();
    cursor -= Cursor;
    while( cursor ) {
        Delete();
        cursor--;
    }
}

void BOL() {
/**********/

    Cursor = 0;
}

void EraseBOL() {
/***************/

    while( Cursor != 0 ) {
        Left();
        Delete();
    }
    BOL();
}

void EOL() {
/**********/

    Cursor = MaxCursor;
}

void EraseEOL() {
/***************/

    while( Cursor != MaxCursor ) {
        Delete();
    }
}

void FlipInsertMode() {
/*********************/

    if( Insert ) {
        Cur.start += CursorDiff;
        Insert = FALSE;
    } else {
        Cur.start -= CursorDiff;
        Insert = TRUE;
    }
    SetCursorType();
}

void BackSpace() {
/****************/

    if( Cursor != 0 ) {
        Left();
        Delete();
    }
}

void ScreenCursorOff() {
/**********************/

    if( RowOffset != 0 ) {
        FlipScreenCursor();
        RowOffset = 0;
    }
}

void EraseLine() {
/****************/

    ToLastCmd();
    Cursor = 0;
    if( Insert & !StickyInsert ) {
        FlipInsertMode();
    }
    ScreenCursorOff();
    Draw = TRUE;
}


void Delete() {
/*************/

    int i;

    if ( MaxCursor != 0 ) {
        if( Cursor != MaxCursor ) Edited = TRUE;
        i = Cursor;
        Line[ MaxCursor ] = ' ';
        while( i < MaxCursor ) {
            Line[ i ] = Line[ i + 1 ];
            ++i;
        }
        --MaxCursor;
    }
    Draw = TRUE;
}

void InsertChar() {
/*****************/

    int i;

    if( MaxCursor < Overflow ) {
        Edited = TRUE;
        i = MaxCursor;
        while( i > Cursor ) {
            Line[ i ] = Line[ i - 1 ];
            --i;
        }
        ++MaxCursor;
        Line[ Cursor ] = KbdChar.ascii;
        ++Cursor;
        Draw = TRUE;
    }
}

void OverlayChar() {
/******************/


    if( Cursor < Overflow ) {
        Edited = TRUE;
        Line[ Cursor ] = KbdChar.ascii;
        ++Cursor;
        Draw = TRUE;
    }
}
