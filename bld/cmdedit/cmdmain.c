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
* Description:  Command input.
*
****************************************************************************/


#include <string.h>
#include "cmdedit.h"

extern int      NonFileChar( char ch );
extern char     far *GetEnv( char far *name, int len );
extern char     far *InitAlias( char far * inname );
extern int      ExpandDirCommand( void );
extern int      LocateLeftWord( void );
extern int      LocateRightWord( void );
extern void     ToLastCmd( void );
extern void     ToFirstCmd( void );
extern void     BOL( void );
extern void     BackSpace( void );
extern void     Delete( void );
extern void     DeleteBOW( void );
extern void     DeleteEOW( void );
extern void     DoDirCommand( void );
extern void     DownScreen( void );
extern void     EOL( void );
extern void     EraseBOL( void );
extern void     EraseEOL( void );
extern void     EraseLine( void );
extern void     FlipInsertMode( void );
extern void     FlipScreenCursor( void );
extern void     InitSave( char far *name );
extern void     InsertChar( void );
extern void     Left( void );
extern void     LeftScreen( void );
extern void     LeftWord( void );
extern void     ListAliases( void );
extern void     ListCommands( void );
extern void     LookForAlias( void );
extern void     MatchACommand( void (*advance)(char *), void (*retreat)(char *) );
extern void     FiniFile( void );
extern void     NextFile( void );
extern void     OverlayChar( void );
extern void     PFKey( void );
extern void     PrevFile( void );
extern void     PutChar( char ch );
extern void     PutNL( void );
extern void     PutString( char far * str );
extern void     ReadScreen( int next_line );
extern void     RestoreLine( void );
extern void     RetrieveACommand( void (* advance)(char *) );
extern void     Right( void );
extern void     RightScreen( void );
extern void     RightWord( void );
extern void     SaveCmd( char *, unsigned );
extern void     SaveLine( void );
extern void     ScreenCursorOff( void );
extern void     SetCursorType( void );
extern void     UpScreen( void );
extern void     NextCmd( char * );
extern void     PrevCmd( char * );
extern void     DelCmd( char * );


void InitRetrieve( char far * inname )
/************************************/
{
    char far *envname;

    AppendSlash = FALSE;
    AlwaysSave = FALSE;
    AlwaysAlias = FALSE;
    HideDirCmds = FALSE;
    FileLower = FALSE;
    StickyInsert = FALSE;
    StartInInsert = FALSE;
    FileUnique = FALSE;
    PrintAllFiles = FALSE;
    CmdSeparator = '!';
    Insert = FALSE;
    envname = GetEnv( ALIAS_FILE, sizeof( ALIAS_FILE ) - 1 );
    if( envname != 0 ) {
        inname = envname;
    }
    VioGetCurType( &Cur, 0 );
    CursorDiff = 2 * ( Cur.cEnd - Cur.yStart );
    for( ;; ) {
        while( *inname == ' ' ) ++inname;
        if( *inname != '-' ) break;
        ++inname;
        switch( *inname++ ) {
        case 'a':
            AlwaysAlias = TRUE;
            break;
        case 'h':
            HideDirCmds = TRUE;
            break;
        case 'l':
            FileLower = TRUE;
            break;
        case 's':
            StickyInsert = TRUE;
            break;
        case 'i':
            StartInInsert = TRUE;
            FlipInsertMode();
            break;
        case 'e':
            AlwaysSave = TRUE;
            break;
        case 'f':
            FileUnique = TRUE;
            break;
        case 'm':
            CmdSeparator = *inname++;
            break;
        case '\\':
            AppendSlash = TRUE;
            break;
        default:
            PutString( "Invalid option '" );
            PutChar( *inname );
            PutNL();
            break;
        }
    }
    inname = InitAlias( inname );
    while( *inname == ' ' ) ++inname;
    InitSave( inname );
#ifndef DOS
    {
        USHORT action;
        DosOpen( "kbd$", &KbdHandle, &action, 0, 0, 1, 0x20, 0 );
    }
#endif
    FiniFile();
    BuffOne = FALSE;
    More = 0;
    LineSaved = FALSE;
#ifdef DOS
    DosFreeEnv();
#endif
}


static int BubbleQuotes( void )
{
    int i;
    int quotes = 0;
    int change = FALSE;
    char tmp;

    for( i = 0; i < MaxCursor; ++i ) {
        if( Line[i] == '"' ) {
            ++quotes;
            if( quotes & 1 ) {
                if( i > 0 && !NonFileChar( Line[i-1] ) && Line[i-1] != '"' ) {
                    tmp = Line[i-1];
                    Line[i-1] = Line[i];
                    Line[i] = tmp;
                    change = TRUE;
                }
            } else {
                if( i+1 < MaxCursor && !NonFileChar( Line[i+1] ) && Line[i+1] != '"' ) {
                    tmp = Line[i+1];
                    Line[i+1] = Line[i];
                    Line[i] = tmp;
                    change = TRUE;
                    ++i;
                }
            }
        }
    }
    return( change );
}

static void ConsolidateQuotes( void )
{
    int i;

    while( BubbleQuotes() );
    for( i = 0; i < MaxCursor-1; ++i ) {
        if( Line[i] == '"' && Line[i+1] == '"' ) {
            memmove( Line+i, Line+i+2, MaxCursor - i - 1 );
            MaxCursor-= 2;
        }
    }
}

void CopyCmd( char far * userbuff, LENGTH far * l, int i )
/********************************************************/
{
    char ch;
    int j;

    j = 0;
    for( ;; ) {
        if( i == MaxCursor ) {
            More = 0;
            break;
        }
        userbuff[j] = ch = Line[i];
        if( ch == CmdSeparator ) {
            if( Line[ i+1 ] == CmdSeparator ) {
                ++i;
            } else {
                More = i + 1;
                break;
            }
        }
        ++i;
        ++j;
    }
    l->output = j;
    userbuff[ j ] = Kbd.chTurnAround;
}


static void DrawLine( int old_len, int old_base )
/***********************************************/
{
    int towrite;
    int oldwrite;
    int left;
    int right;

    if( Draw ) {
        towrite = MaxCursor - Base;
        right = 0;
        if( towrite > SCREEN_WIDTH - 1 - StartCol ) {
            towrite = SCREEN_WIDTH - 1 - StartCol;
            right = 1;
        }
        oldwrite = old_len - old_base;
        if( oldwrite > SCREEN_WIDTH - 1 - StartCol ) {
            oldwrite = SCREEN_WIDTH - 1 - StartCol;
        }
        left = StartDraw - Base;
        VioWrtCharStr( Line + Base + left, towrite - left, Row, StartCol + left, 0 );
        if( Cursor == MaxCursor ) {
            VioWrtNChar( (UCHAR PASPTR *)" ", 1, Row, MaxCursor + StartCol - Base, 0 );
        }
        if( towrite < oldwrite ) {
            VioWrtNChar( (UCHAR PASPTR *)" ", oldwrite - towrite, Row, MaxCursor + StartCol - Base, 0 );
        }
        if( Base != 0 ) {
            VioWrtNChar( (UCHAR PASPTR *)"\021", 1, Row, StartCol, 0 );
        }
        if( right ) {
            VioWrtNChar( (UCHAR PASPTR *)"\020", 1, Row, SCREEN_WIDTH-2, 0 );
        }
    }
}


int StringIn( char far * userbuff, LENGTH far * l, int want_alias, int routine )
/******************************************************************************/
{
    int     old_len,old_base;
    int     first_match;

    routine=routine;
    SetCursorType();
    if( AlwaysAlias ) {
        WantAlias = TRUE;
    } else {
        WantAlias = want_alias;
    }
    if( More ) {
        if( WantAlias ) LookForAlias();
        CopyCmd( userbuff, l, More );
        return( 0 );
    }
    Kbd.cb = sizeof( KBDDESC );
    KbdGetStatus( &Kbd, 0 );
    if( Kbd.fsMask & KBD_BINARY ) return( -1 );
    RowOffset = 0;

    VioGetCurPos( &Row, &Col, 0 );
    StartCol = Col;
    ColOffset = -StartCol;
    Overflow = LINE_WIDTH - StartCol - 1;
    if( Overflow > l->input - 2 ) {
        Overflow = l->input - 2; /* 2 for CR/LF */
    }
    if( Overflow == FALSE ) {
        Overflow = TRUE;
    }

    Cursor = 0;
    MaxCursor = 0;
    OldCursor = 0;
    OldMaxCursor = 0;
    Edited = FALSE;
    Base = 0;
    ImmedCommand = FALSE;
    PFChars = 0;
    FirstNextOrPrev = TRUE;
    first_match = TRUE;

    for( ;; ) { /* for each typed character */
        Draw = FALSE;
        StartDraw = Base;
        old_len = MaxCursor;
        old_base = Base;
        if( PFChars != 0 ) {
            KbdChar.chChar = *PFChars++;
            if( KbdChar.chChar == '\r' ) {
                if( ImmedCommand ) break;
                PFChars = 0;
            }
        }
        if( PFChars == 0 ) {
            if( BuffOne == FALSE ) {
                KbdCharIn( &KbdChar, 0, 0 );
            } else {
                BuffOne = FALSE;
            }
        }
        if( KbdChar.chChar == Kbd.chTurnAround ) break;
        if( ( KbdChar.chChar == 0 || KbdChar.chChar == 0xE0 ) ) {
            if( KbdChar.chScan != BACK_TAB ) {
                FiniFile();
            }
            switch( KbdChar.chScan ) {
            case CTRL_BREAK:
                break;
            case CTRL_INSERT:
                if( LineSaved ) {
                    RestoreLine();
                    Draw = TRUE;
                }
                break;
            case BACK_TAB:
                if( FileUnique ) {
                    PrintAllFiles = TRUE;
                    NextFile();
                } else {
                    PrevFile();
                }
                break;
            case LEFT:
                Left();
                break;
            case CTRL_LEFT:
                LeftWord();
                break;
            case RIGHT:
                Right();
                break;
            case CTRL_RIGHT:
                RightWord();
                break;
            case HOME:
                BOL();
                break;
            case CTRL_HOME:
                EraseBOL();
                break;
            case END:
                EOL();
                break;
            case CTRL_END:
                StartDraw = Cursor;
                EraseEOL();
                break;
            case INSERT:
                FlipInsertMode();
                break;
            case DELETE:
                StartDraw = Cursor;
                Delete();
                break;
            case CTRL_DELETE:
                DeleteEOW();
                break;
            case ALT_R:
            case ALT_RIGHT:
                RightScreen();
                break;
            case ALT_L:
            case ALT_LEFT:
                LeftScreen();
                break;
            case ALT_U:
            case ALT_UP:
                UpScreen();
                break;
            case ALT_D:
            case ALT_DOWN:
                DownScreen();
                break;
            case ALT_0:
            case ALT_HOME:
                if( RowOffset == 0 ) break;
                while( ColOffset != -StartCol ) {
                    LeftScreen();
                }
                break;
            case ALT_2:
            case ALT_END:
                if( RowOffset == 0 ) break;
                ReadScreen( 1 );
                break;
            case ALT_N:
            case CTRL_TAB:
                first_match = FALSE;
                MatchACommand( NextCmd, PrevCmd );
                break;
            case ALT_P:
            case ALT_TAB:
                if( first_match ) {
                    SaveLine();
                    ToLastCmd();
                    RestoreLine();
                    first_match = FALSE;
                }
                MatchACommand( PrevCmd, NextCmd );
                break;
            case ALT_E:
            case CTRL_DOWN:
                ToLastCmd();
                FirstNextOrPrev = FALSE;
            case UP:
                SaveLine();
                ScreenCursorOff();
                if( FirstNextOrPrev ) {
                    NextCmd( Line );
                }
                RetrieveACommand( PrevCmd );
                break;
            case ALT_B:
            case CTRL_UP:
                ToFirstCmd();
            case DOWN:
                SaveLine();
                ScreenCursorOff();
                RetrieveACommand( NextCmd );
                break;
            case PAGEDOWN:
                SaveCmd( Line, MaxCursor );
                break;
            case ALT_X:
                if( MaxCursor != 0 && !FirstNextOrPrev ) {
                    RetrieveACommand( DelCmd );
                }
                break;
            case ALT_C:
                ListCommands();
                break;
            case F1:  case F2:  case F3:  case F4:  case F5:  case F6:
            case F7:  case F8:  case F9:  case F10: case F11: case F12:
            case F13: case F14: case F15: case F16: case F17: case F18:
            case F19: case F20: case F21: case F22: case F23: case F24:
            case F25: case F26: case F27: case F28: case F29: case F30:
            case F31: case F32: case F33: case F34: case F35: case F36:
            case F37: case F38: case F39: case F40:
                PFKey();
                break;
            case ALT_A:
                if( WantAlias ) ListAliases();
                break;
            case ALT_H:
                HideDirCmds = !HideDirCmds;
                break;
            }
        } else if( KbdChar.chChar == TAB ) {
            NextFile();
        } else {
            FiniFile();
            switch( KbdChar.chChar ) {
            case ESCAPE:
                SaveLine();
                EraseLine();
                break;
            case BACKSPACE:
                BackSpace();
                StartDraw = Cursor;
                break;
            case CTRL_BACKSPACE:
                DeleteBOW();
                break;
            default:
                StartDraw = Cursor;
                if( Insert ) {
                    InsertChar();
                } else {
                    OverlayChar();
                }
                break;
            }
        }
        if( Cursor > MaxCursor ) {
            MaxCursor = Cursor;
        }
        if( ( Cursor + StartCol - Base ) > SCREEN_WIDTH - 2 ) {
            Base += Cursor + StartCol - Base - SCREEN_WIDTH + 2;
        }
        if( Cursor < Base ) {
            Base = Cursor;
        }
        if( Cursor == 0 ) {
            Base = 0;
        }
        if( Base != old_base ) {
            Draw = TRUE;
            StartDraw = Base;
        }
        DrawLine( old_len, old_base );
        VioSetCurPos( Row, StartCol + Cursor - Base, 0 );
    } /* for each typed character */

    if( !StickyInsert ) {
        if( StartInInsert && !Insert ) {
            FlipInsertMode();
        } else if( !StartInInsert && Insert ) {
            FlipInsertMode();
        }
    }
    if( RowOffset != 0 ) {
        FlipScreenCursor();
    }
    if( ( Edited || AlwaysSave ) && !ImmedCommand ) {
         SaveCmd( Line, MaxCursor );
    }
    if( WantAlias ) LookForAlias();
    ConsolidateQuotes();
    if( ExpandDirCommand() ) DoDirCommand();
    FiniFile();
    CopyCmd( userbuff, l, 0 );
    {
        static USHORT dummy;
        VioGetCurPos( &Row, &dummy, 0 );
    }
#ifndef __CMDSHELL__
    VioSetCurPos( Row, 0, 0 );
#else
    if( VioSetCurPos( Row+1, 0, 0 ) != 0 ) {
        static char buffer[2];
        static USHORT length;

        length = 2;
        VioReadCellStr( &buffer, &length, Row, StartCol+Cursor-Base, 0 );
        buffer[0] = ' ';
        VioScrollUp( 0, 0, -1, -1, 1, &buffer, 0 );
        VioSetCurPos( Row, 0, 0 );
    }
#endif
    return( 0 );
}
