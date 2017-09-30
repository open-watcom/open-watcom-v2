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
* Description:  Command line editing.
*
****************************************************************************/


#include "cmdedit.h"


void ToFirstCmd( void )
/*********************/
{
    while( MaxCursor = PrevCmd( Line ) );
}


void ToLastCmd( void )
/********************/
{
    while( MaxCursor = NextCmd( Line ) );
}


int WordSep( char ch )
/********************/
{
    static char sep[] = { " \t:;,.<>/?'\"{}[]|\\`~!@$%^&*()-+=" };
    char *psep;

    if( _null( ch ) )
        return( 1 );
    for( psep = sep; !_null( *psep ); ++psep ) {
        if( *psep == ch ) {
            return( 1 );
        }
    }
    return( 0 );
}

void Left( void )
/***************/
{
    if( Cursor != 0 ) {
        --Cursor;
    }
}

int LocateLeftWord( void )
/************************/
{
    int cursor;

    if( Cursor == 0 )
        return( 0 );
    for( cursor = Cursor - 1; cursor > 0; cursor-- ) {
        if( !WordSep( Line[cursor] ) ) {
            break;
        }
    }
    for( ; cursor > 0; cursor-- ) {
        if( WordSep( Line[cursor] ) ) {
            ++cursor;
            break;
        }
    }
    return( cursor );
}

void LeftWord( void )
/*******************/
{
    Cursor = LocateLeftWord();
}

void Delete( void )
/*****************/
{
    int i;

    if ( MaxCursor != 0 ) {
        if( Cursor != MaxCursor )
            Edited = TRUE;
        Line[MaxCursor] = ' ';
        for( i = Cursor; i < MaxCursor; i++ ) {
            Line[i] = Line[i + 1];
        }
        --MaxCursor;
    }
    Draw = TRUE;
}

void BackSpace( void )
/********************/
{
    if( Cursor != 0 ) {
        Left();
        Delete();
    }
}

void DeleteBOW( void )
/********************/
{
    int cursor;

    cursor = LocateLeftWord();
    while( Cursor != cursor ) {
        BackSpace();
    }
}

void Right( void )
/****************/
{
    if( Cursor < Overflow ) {
        ++Cursor;
        if( Cursor > MaxCursor ) {
            Line[MaxCursor] = ' ';
        }
    }
}

int LocateRightWord( void )
/*************************/
{
    int cursor;

    for( cursor = Cursor; cursor < MaxCursor; cursor++ ) {
        if( WordSep( Line[cursor] ) ) {
            break;
        }
    }
    for( ; cursor < MaxCursor; cursor++ ) {
        if( !WordSep( Line[cursor] ) ) {
            break;
        }
    }
    return( cursor );
}

void RightWord( void )
/********************/
{
    Cursor = LocateRightWord();
}

void DeleteEOW( void )
/********************/
{
    int cursor;

    for( cursor = LocateRightWord() - Cursor; cursor > 0; cursor-- ) {
        Delete();
    }
}

void BOL( void )
/**************/
{
    Cursor = 0;
}

void EraseBOL( void )
/*******************/
{
    while( Cursor != 0 ) {
        Left();
        Delete();
    }
    BOL();
}

void EOL( void )
/**************/
{
    Cursor = MaxCursor;
}

void EraseEOL( void )
/*******************/
{
    while( Cursor != MaxCursor ) {
        Delete();
    }
}

void FlipInsertMode( void )
/*************************/
{
    if( Insert ) {
        Cur.yStart += CursorDiff;
        Insert = FALSE;
    } else {
        Cur.yStart -= CursorDiff;
        Insert = TRUE;
    }
    SetCursorType();
}

void ScreenCursorOff( void )
/**************************/
{
    if( RowOffset != 0 ) {
        FlipScreenCursor();
        RowOffset = 0;
    }
}

void EraseLine( void )
/********************/
{
    ToLastCmd();
    Cursor = 0;
    if( Insert & !StickyInsert ) {
        FlipInsertMode();
    }
    ScreenCursorOff();
    Draw = TRUE;
}

void InsertChar( void )
/*********************/
{
    int i;

    if( MaxCursor < Overflow ) {
        Edited = TRUE;
        for( i = MaxCursor; i > Cursor; i-- ) {
            Line[i] = Line[i - 1];
        }
        ++MaxCursor;
        Line[Cursor] = KbdChar.chChar;
        ++Cursor;
        Draw = TRUE;
    }
}

void OverlayChar( void )
/**********************/
{
    if( Cursor < Overflow ) {
        Edited = TRUE;
        Line[Cursor] = KbdChar.chChar;
        ++Cursor;
        Draw = TRUE;
    }
}
