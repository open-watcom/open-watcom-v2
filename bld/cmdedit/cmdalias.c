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

extern int      WordSep( char ch );
extern void     PutChar( char ch );
extern void     PutNL();
extern void     RestoreLine();
extern char     *EatWhite( char *word );
extern void     PutString( char far * str );
extern void     RestorePrompt( char PASPTR *line );
extern void     SaveLine();
extern void     SavePrompt( char PASPTR *line );
extern char     far *GetEnv( char far *name, int len );
extern int      PutMore();
extern int      Equal( char far * str1, char far * str2, int len );

void ListAliases() {
/******************/

    int i;
    char far *alias;
    char        prompt[ 80 ];

    SaveLine();
    SavePrompt( (char PASPTR *)prompt );
    PutNL();
    i = 0;
    alias = AliasList;
    while( *alias != '\0' ) {
        if( *alias == '\n' ) {
            if( ++i == 23 ) {
                PutChar( '\n' );
                if( !PutMore() ) break;
                ++alias;
                i = 0;
                continue;
            }
        }
        PutChar( *alias );
        ++alias;
    }
    RestorePrompt( (char PASPTR *)prompt );
    RestoreLine();
    Draw = TRUE;
}


char far *InitAlias( char far * inname ) {
/****************************************/

    int hdl,read;
    static char b[80];
    char *bp;
    unsigned long ppos,pos;
    int action;
    char far *endname;
    static char noalias = 0;

    endname = inname;
    while( *endname == ' ' ) ++endname;
    for( ;; ) {
        if( *endname == '\0' ) {
            break;
        }
        if( *endname == ' ' ) {
            *endname = '\0';
            ++endname;
            break;
        }
        ++endname;
    }
    bp = b;
    while( *bp = *inname ) {
        ++bp;
        ++inname;
    }
    action=action;
    if( DosOpen( (char PASPTR *)b, (int PASPTR *)&hdl,
                 (int PASPTR *)&action,
                 0L, 0, 1, 0x10 ,0L ) == 0 ) {
        DosChgFilePtr( hdl, 0L, 2, (unsigned long PASPTR *)&ppos );
        pos = ppos;
        DosChgFilePtr( hdl, 0L, 0, (unsigned long PASPTR *)&ppos );
        #ifdef DOS
        {
            static int alias_seg;
            DosAllocSeg( pos + 1 + ALIAS_SLACK, (int PASPTR *)&alias_seg, 0 );
            *(((int far *)&AliasList)+0) = 0;
            *(((int far *)&AliasList)+1) = alias_seg;
            AliasSize = pos + ALIAS_SLACK;
        }
        #else
            AliasList = (char far *)&AliasArea;
            AliasSize = 2047;
        #endif
        DosRead( hdl, (char far *)AliasList, pos, (int PASPTR *)&read );
        if( pos > 0 && AliasList[ pos-1 ] != '\n' ) {
            AliasList[ pos+0 ] = '\r';
            AliasList[ pos+1 ] = '\n';
            AliasList[ pos+2 ] = '\0';
        } else {
            AliasList[ pos ] = '\0';
        }
        DosClose( hdl );
    } else {
        AliasList = (char far *)&noalias;
    }
    return( endname );
}

char far * FindAlias( char * word, char * endword, char far * far *start ) {
/**************************************************************************/

    char far *alias;
    char            ch;
    int             len;

    if( word == endword ) return( (char *)0 );
    ch = *endword;
    *endword = ' ';
    len = endword - word + 1;

    alias = AliasList;
    for(;;) {
        while( *alias == ' ' ) {
            ++alias;
        }
        if( _null( *alias ) ) break;
        if( Equal( alias, word, len ) ) break;
        while( *alias != '\r' ) {
            ++alias;
        }
        ++alias;
        if( *alias == '\n' ) {
            ++alias;
        }
    }


    *endword = ch;
    if( _null( *alias ) ) return( (char *)0 );
    *start = alias;
    while( !_white( *alias ) ) {
        ++alias;
    }
    while( _white( *alias ) ) {
        ++alias;
    }
    return( alias );
}

char *LocateParm( char *parmlist, int parm ) {
/********************************************/

    for( ;; ) {
        while( _white( *parmlist ) ) {
            ++parmlist;
        }
        if( _null( *parmlist ) ) return( 0 );
        if( parm == 0 ) return( parmlist );
        while( !_white_or_null( *parmlist ) ) {
            ++parmlist;
        }
        --parm;
    }
}

int SubstParms( char far *oldptr, char far *newptr,
                      char *endword, int len ) {
/**********************************************/

    char        ch;
    int         parm;
    char        *parmptr;
    char        *endparm;
    char        far *endptr;
    char        far *environ;
    char        far *overflow;
    char        kill[ 9 ];

    for( parm = 0; parm < 9; ++parm ) {
        kill[ parm ] = FALSE;
    }
    overflow = newptr + len - 1;
    while( *oldptr != '\r' && *oldptr != '\0' ) {
        if( oldptr[ 0 ] != '%' ) {
            if( newptr == overflow ) return( FALSE );
            *newptr++ = *oldptr++;
        } else if( oldptr[ 1 ] == '%' ) {
            if( newptr == overflow ) return( FALSE );
            *newptr++ = '%';
            oldptr += 2;
        } else if( ( ch = oldptr[ 1 ] ) >= '1' && ch <= '9' ) {
            parm = ch - '1';
            parmptr = LocateParm( endword, parm );
            if( parmptr != 0 ) {
                for( ;; ) {
                    ch = *parmptr++;
                    if( _white_or_null( ch ) ) break;
                    if( newptr == overflow ) return( FALSE );
                    *newptr++ = ch;
                }
                kill[ parm ] = TRUE;
            }
            oldptr += 2;
        } else {
            endptr = oldptr;
            for( ;; ) {
                ++endptr;
                if( *endptr == '%' ) {
                    *endptr = '=';
                    environ = GetEnv( oldptr + 1, endptr - oldptr );
                    *endptr = '%';
                    if( environ != 0 ) {
                        while( !_null( *environ ) ) {
                            if( newptr == overflow ) return( FALSE );
                            *newptr++ = *environ++;
                        }
                    }
                    oldptr = endptr + 1;
                    break;
                }
                if( _white_or_null( *endptr ) ) {
                    if( newptr == overflow ) return( FALSE );
                    *newptr++ = *oldptr++;
                    break;
                }
            }
        }
    }
    *newptr = '\r';
    for( parm = 8; parm >= 0; --parm ) {
        if( kill[ parm ] ) {
            parmptr = LocateParm( endword, parm );
            endparm = parmptr;
            do {
                ch = *++endparm;
            } while( !_white_or_null( ch ) );
            ReplaceAlias( (char far *)"", parmptr, endparm );
        }
    }
    return( TRUE );
}


int ReplaceAlias( char far * alias, char * word, char * endword ) {
/******************************************************************/

    char far * endalias;
    int insert,i;
    char newalias[LINE_WIDTH];

    if( !SubstParms( alias, newalias, endword, LINE_WIDTH ) ) {
        PutString( "\r\nCommand too long!\r\n" );
        return( 0 );
    }
    alias = newalias;
    endalias = alias;
    while( *endalias != '\r' && *endalias != '\0' ) {
        ++endalias;
    }
    insert = ( endalias - alias ) - ( endword - word );
    if( ( MaxCursor + insert ) > Overflow ) return( 0 );
    if( insert < 0 ) {
        i = word - Line;
        while( i < MaxCursor ) {
            Line[ i ] = Line[ i - insert ];
            ++i;
        }
    } else if( insert > 0 ) {
        i = MaxCursor;
        while( --i >= More ) {
            if( Line + i < word ) break;
            Line[ i + insert ] = Line[ i ];
        }
    }
    while( alias != endalias ) {
        *word = *alias;
        ++word;
        ++alias;
    }
    MaxCursor += insert;
    Cursor += insert;
    return( insert );
}

char *EndOfWord( char *word ) {
/*****************************/

    while( !WordSep( *word ) ) {
        ++word;
    }
    return( word );
}

void LookForAlias() {
/*******************/

    char *word, *endword, far *alias, far *start;

    for( ;; ) {
        Line[ MaxCursor ] = '\0';
        word = EatWhite( Line + More );
        endword = word;
        while( !_white_or_null( *endword ) && *endword != '/' ) {
            ++endword;
        }
        alias = FindAlias( word, endword, &start );
        if( alias == 0 ) break;
        ReplaceAlias( alias, word, endword );
        *start |= 0x80;
    }
    for( alias = AliasList; !_null( *alias ); ++alias ) {
        *alias &= ~0x80;
    }
}

void PFKey() {
/************/

    static char buff[14];
    int     i;
    char    far *start;

    if( WantAlias ) {
        i = FormName( buff, KbdChar.scan );
        PFChars = FindAlias( buff, (buff + i), &start );
        if( PFChars && *PFChars == '!' ) {
            ++PFChars;
            ImmedCommand = TRUE;
        }
    }
}

int FormName( char * name, char scan ) {
/**************************************/

    name[ 0 ] = '<';
    name[ 1 ] = 'F';
    if( scan <= F9 ) {
        name[ 2 ] = scan - ( F1 - 1 ) + '0';
        name[ 3 ] = '>';
        return( 4 );
    }
    if( scan == F10 ) {
        name[ 2 ] = '1';
        scan = 0;
    } else if( scan >= F40 ) {
        name[ 2 ] = '4';
        scan -= F40;
    } else if( scan >= F30 ) {
        name[ 2 ] = '3';
        scan -= F30;
    } else if( scan >= F20 ) {
        name[ 2 ] = '2';
        scan -= F20;
    } else {
        name[ 2 ] = '1';
        scan -= ( F11 - 1 );
    }
    name[ 3 ] = scan + '0';
    name[ 4 ] = '>';
    return( 5 );
}
