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

extern int      PrevCmd( char * );
extern void     SetCursorType( void );
extern void     FlipScreenCursor( void );
extern int      NextCmd( char * );


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


extern int WordSep( char ch )
/***************************/
{
    static char sep[] = { " \t:;,.<>/?'\"{}[]|\\`~!@$%^&*()-+=" };
    char *psep;

    if( _null( ch ) ) return( 1 );
    for( psep = sep; !_null( *psep ); ++psep ) {
        if( *psep == ch ) return( 1 );
    }
    return( 0 );
}

void Left( void )
/***************/
{
    if( Cursor != 0 ) --Cursor;
}

int LocateLeftWord( void )
/************************/
{
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
    while( Cursor != cursor ) BackSpace();
}

void Right( void )
/****************/
{
    if( Cursor < Overflow ) {
        ++Cursor;
        if( Cursor > MaxCursor ) {
            Line[ MaxCursor ] = ' ';
        }
    }
}

int LocateRightWord( void )
/*************************/
{
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

void RightWord( void )
/********************/
{
    Cursor = LocateRightWord();
}

void DeleteEOW( void )
/********************/
{
    int cursor;

    cursor = LocateRightWord();
    cursor -= Cursor;
    while( cursor ) {
        Delete();
        cursor--;
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
        Cur.start += CursorDiff;
        Insert = FALSE;
    } else {
        Cur.start -= CursorDiff;
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

void OverlayChar( void )
/**********************/
{
    if( Cursor < Overflow ) {
        Edited = TRUE;
        Line[ Cursor ] = KbdChar.ascii;
        ++Cursor;
        Draw = TRUE;
    }
}
