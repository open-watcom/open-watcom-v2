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
* Description:  Command matching, file management.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "cmdedit.h"

extern void     PutNL( void );
extern void     PutPad( char far * str, int len );
extern void     RestorePrompt( char PASPTR *line );
extern int      ExpandDirCommand( void );
extern int      ReplaceAlias( char far * alias, char * word, char * endword );
extern void     ZapLower( char far *str );
extern void     SavePrompt( char PASPTR *line );
extern char     far *GetEnv( char far *name, int len );
extern int      Equal( char far * str1, char far * str2, int len );


void SaveLine( void )
/*******************/
{
    LineSaved = TRUE;
    memcpy( OldLine, Line, LINE_WIDTH );
    OldMaxCursor = MaxCursor;
    OldCursor = Cursor;
}


void RestoreLine( void )
/**********************/
{
    memcpy( Line, OldLine, LINE_WIDTH );
    Cursor = OldCursor;
    MaxCursor = OldMaxCursor;
    HaveDirent = FALSE;
}


void MatchACommand( int (*advance)(char *), int (*retreat)(char *) )
/******************************************************************/
{
    int advanced;

    SaveLine();
    advanced = 0;
    for( ;; ) {
        ++advanced;
        MaxCursor = advance( Line );
        if( MaxCursor == 0 ) {
            while( --advanced >= 0 ) {
                MaxCursor = retreat( Line );
            }
            RestoreLine();
            return;
        }
        if( MaxCursor < OldCursor ) continue;
        if( Equal( OldLine, Line, OldCursor ) ) break;
    }
    Draw = TRUE;
    Base = 0;
    Edited = FALSE;
    FirstNextOrPrev = FALSE;
}


void FiniFile( void )
/*******************/
{
    HaveDirent = FALSE;
    NextFileCalls = 0;
    PathCurr = 0;
}


int     NonFileChar( char ch )
/****************************/
{
    switch( ch ) {
    case '<':
    case '>':
    case '|':
    case ' ':
    case '\t':
    case '=':
    case '+':
    case ';':
    case '@':
        return( TRUE );
    }
    return( FALSE );
}


int FileIgnore( DIRINFO *dir, int attr )
/**************************************/
{
    int         ignore_matches;
    char        *p;
    int         len;
    char        far *envname;
    char        *name;

    if( attr != 0 && !( dir->attr & attr ) ) return( 1 );
    name = dir->name;
    if( PathCurr != 0 ) {
#ifdef DOS
        envname = ".COM.EXE.BAT";
#else
        envname = ".COM.EXE.CMD.BAT";
#endif
    } else {
        envname = GetEnv( MATCH_NAME, sizeof( MATCH_NAME ) - 1 );
        if( envname == (char far *)0 ) return( FALSE );
    }
    if( *envname == '~' ) {
        ++envname;
        ignore_matches = TRUE;
    } else {
        ignore_matches = FALSE;
    }
    while( !_null( *name ) && *name != '.' ) ++name;
#ifndef DOS
    if( *name == '.' ) {
        /* consider: a.b.c   .c is the extension */
        name += strlen( name ) - 1;
        while( *name != '.' ) --name;
    }
#endif
    len = 0;
    for( p = name; !_null( *p ); ++p ) ++len;
    for( ;; ) {
        for( ;; ) {
            if( *envname == '.' ) break;
            if( _null( *envname ) ) return( !ignore_matches );
            ++envname;
        }
        if( len == 0 ) {
            if( envname[1] == '.' || _null( envname[1] ) ) {
                return( ignore_matches );
            }
        } else {
            if( Equal( envname, name, len ) ) {
                if( _null( envname[len] ) || envname[len] == '.' ) {
                    return( ignore_matches );
                }
            }
        }
        ++envname;
    }
}


int FindNext( DIRINFO *dir, int attr )
/************************************/
{
    int tmp;
    int cnt;

    tmp = 1;
    do {
        cnt = DosFindNext( 1, (char PASPTR *)dir,
                           sizeof( DIRINFO ), (int PASPTR *)&tmp);
        if( cnt != 0 ) return( cnt );
    } while( FileIgnore( dir, attr ) );
    return( cnt );
}


void NextFile( void )
/*******************/
{
#ifdef DOS
    int                 dot;
#endif
    int                 hdl;
    int                 i;
    int                 path;
    int                 attr;
    int                 cnt;
    int                 searchpath;
    static              int lastrc;
    static char         buff[ MAX_FNAME+2 ];
    char                c0,c1,c2,c3;
    char                *word;
    char                *alias;
    static DIRINFO      dir;
    char                prompt[SCREEN_WIDTH];
    int                 is_directory;
    char                in_quote;
    char                has_blank;

    attr = 0;
    if( ExpandDirCommand() ) {
        attr = _D_SDIR;
    }
    if( Cursor != 0 ) {
        if( !HaveDirent ) {
            if( NextFileCalls == 0 ) SaveLine();
recurse:
            path = 0;
            i = Cursor - 1;
#ifdef DOS
            dot = FALSE;
#endif
            in_quote = FALSE;
            for( ;; ) {
                if( Line[ i ] == '/' || Line[ i ] == '\\' ) path = 1;
#ifdef DOS
                if( path == 0 && Line[ i ] == '.' ) dot = TRUE;
#endif
                if( Line[i] == '"' ) in_quote = !in_quote;
                if( !in_quote && NonFileChar( Line[ i ] ) ) {
                    ++i;
                    break;
                }
                if( i == 0 ) break;
                --i;
            }
            searchpath = FALSE;
            if( i == 0 ) {
                if( PathCurr != 0 ) {
                    searchpath = TRUE;
                } else if( Line[0] != '.' && Line[0] != '\\' &&
                           Line[0] != '/' &&
                           ( MaxCursor == 0 || Line[1] != ':' ) ) {
                    searchpath = TRUE;
                }
            }
            if( searchpath && !FileUnique ) {
                if( NextFileCalls == 0 && PathCurr == 0 ) {
                    PathCurr = GetEnv( PATH, sizeof( PATH ) - 1 );
                } else if( PathCurr == 0 ) {
                    lastrc = ~0;
                    goto done;
                } else if( *PathCurr == '\0' ) {
                    ++NextFileCalls;
                    PathCurr = 0;
                    lastrc = ~0;
                    RestoreLine();
                    goto done;
                } else {
                    RestoreLine();
#ifdef DOS
                    dot = FALSE;
#endif
                    word = PathBuff;
                    for( ;; ) {
                        if( *PathCurr == '\0' ) break;
                        if( *PathCurr == ';' ) {
                            ++PathCurr;
                            break;
                        }
                        *word = *PathCurr;
                        ++word;
                        ++PathCurr;
                    }
                    *word = 0;
                    ReplaceAlias( PathBuff, Line, Line );
                    ReplaceAlias( "\\", Line+(word-PathBuff),
                                        Line+(word-PathBuff) );
                }
            }
            c0 = Line[Cursor+0];
            c1 = Line[Cursor+1];
            c2 = Line[Cursor+2];
            c3 = Line[Cursor+3];
            Line[ Cursor ] = '*';

#ifdef DOS
            if( !dot ) {
                Line[ Cursor + 1 ] = '.';
                Line[ Cursor + 2 ] = '*';
                Line[ Cursor + 3 ] = '\0';
            } else {
                Line[ Cursor + 1 ] = '\0';
            }
#else
            Line[ Cursor + 1 ] = '\0';
#endif
            hdl = 1;
            cnt = 1;
            lastrc = DosFindFirst( (char PASPTR *)Line + i, (int PASPTR *)&hdl,
                                _D_SDIR, (char PASPTR *)&dir, sizeof( DIRINFO ),
                                (int PASPTR *)&cnt, 0 );
            Line[Cursor+0] = c0;
            Line[Cursor+1] = c1;
            Line[Cursor+2] = c2;
            Line[Cursor+3] = c3;
            if( lastrc == 0 ) {
                if( FileIgnore( &dir, attr ) ) lastrc = FindNext( &dir, attr );
                HaveDirent = TRUE;
            }
        }
done:
        ++NextFileCalls;
        if( lastrc == 0 ) {
            is_directory = dir.attr & _D_SDIR;
            word = dir.name;
            alias = buff;
            has_blank = strchr( dir.name, ' ' ) != NULL;
            if( has_blank ) *alias++ = '"';
            while( *alias = *word ) {
                ++alias; ++word;
            }
            if( has_blank ) *alias++ = '"';
            *alias = '\r';
            if( FileUnique ) {
                if( PrintAllFiles ) {
                    SaveLine();
                    NextFileCalls = 0;
                    SavePrompt( (char PASPTR *)prompt );
                    PutNL();
                    i = 1;
                }
                for( ;; ) {
                    if( PrintAllFiles ) {
                        if( --i == 0 ) {
                            PutNL();
                            i = 6;
                        }
                        PutPad( dir.name, 13 );
                    }
                    lastrc = FindNext( &dir, attr );
                    if( lastrc != 0 ) break;
                    alias = buff;
                    word = dir.name;
                    while( tolower( *alias ) == tolower( *word ) && *alias != '\r' ) {
                        ++alias;
                        ++word;
                    }
                    *alias = '\r';
                }
                if( PrintAllFiles ) {
                    PutNL();
                    PutNL();
                    RestorePrompt( (char PASPTR *)prompt );
                    RestoreLine();
                }
                HaveDirent = FALSE;
            } else {
                lastrc = FindNext( &dir, attr );
            }
            i = Cursor - 1;
            if( AppendSlash && NextFileCalls > 1 ) {
                --i;
            }
            in_quote = FALSE;
            for( ;; ) {
                if( Line[ i ] == '"' ) in_quote = !in_quote;
                if( Line[ i ] == '\\' ) break;
                if( Line[ i ] == '/' ) break;
                if( Line[ i ] == ':' ) break;
                if( !in_quote && NonFileChar( Line[ i ] ) ) break;
                --i;
                if( i == -1 ) break;
            }
            ++i;
            if( FileLower ) ZapLower( &buff );
            if( AppendSlash ) {
                alias = buff;
                while( *alias != '\0' && *alias != '\r' ) ++alias;
                *alias++ = is_directory ? '\\' : ' ';
                *alias = 0;
            }
            ReplaceAlias( &buff, Line+i, Line+Cursor );
        } else if( PathCurr != 0 ) {
            RestoreLine();
            HaveDirent = FALSE;
            --NextFileCalls;
            goto recurse;
        } else {
            --NextFileCalls;
        }
    } else if( LineSaved ) {
        RestoreLine();
    }
    Draw = TRUE;
    PrintAllFiles = FALSE;
}


void PrevFile( void )
/*******************/
{
    int         nexts;

    if( NextFileCalls ) {
        RestoreLine();
        nexts = NextFileCalls - 1;
        FiniFile();
        while( NextFileCalls != nexts ) {
            NextFile();
        }
        Draw = TRUE;
    }
}
