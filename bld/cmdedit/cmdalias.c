/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Process command aliases.
*
****************************************************************************/


#include <stdlib.h>
#include "cmdedit.h"


void ListAliases( void )
/**********************/
{
    int     i;
    char    __far *alias;
    char    prompt[80];

    SaveLine();
    SavePrompt( prompt );
    PutNL();
    i = 0;
    for( alias = AliasList; *alias != '\0'; alias++ ) {
        if( *alias == '\n' ) {
            if( ++i == 23 ) {
                PutChar( '\n' );
                if( !PutMore() )
                    break;
                i = 0;
                continue;
            }
        }
        PutChar( *alias );
    }
    RestorePrompt( prompt );
    RestoreLine();
    Draw = TRUE;
}


char __far *InitAlias( char __far * inname )
/******************************************/
{
    USHORT          hdl,read;
    static char     b[80];
    char            *bp;
    unsigned long   ppos,pos;
    USHORT          action;
    char            __far *endname;
    static char     noalias = 0;
#ifdef __OS2__
    static char     AliasArea[2048]; /* The DLL seems to need static memory */
#endif

    endname = inname;
    while( *endname == ' ' )
        ++endname;
    for( ; *endname != '\0'; endname++ ) {
        if( *endname == ' ' ) {
            *endname++ = '\0';
            break;
        }
    }
    for( bp = b; (*bp = *inname) != '\0'; bp++ ) {
        ++inname;
    }
    action=action;
    if( DosOpen( b, &hdl, &action, 0,
            FILE_NORMAL,
            OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READONLY,
            0 ) == 0 ) {
        DosChgFilePtr( hdl, 0L, 2, &ppos );
        pos = ppos;
        DosChgFilePtr( hdl, 0L, 0, &ppos );
#ifdef __OS2__
        AliasList = AliasArea;
        AliasSize = 2047;
#else
        {
            static int alias_seg;

            DosAllocSeg( pos + 1 + ALIAS_SLACK, &alias_seg, 0 );
            AliasList = (char __far *)_MK_FP( alias_seg, 0 );
            AliasSize = pos + ALIAS_SLACK;
        }
#endif
        DosRead( hdl, AliasList, pos, &read );
        if( pos > 0 && AliasList[pos - 1] != '\n' ) {
            AliasList[pos + 0] = '\r';
            AliasList[pos + 1] = '\n';
            AliasList[pos + 2] = '\0';
        } else {
            AliasList[pos] = '\0';
        }
        DosClose( hdl );
    } else {
        AliasList = &noalias;
    }
    return( endname );
}

static char __far *FindAlias( char *word, char *endword, char __far * __far *start )
/**********************************************************************************/
{
    char    __far *alias;
    char    ch;
    int     len;

    if( word == endword )
        return( NULL );
    ch = *endword;
    *endword = ' ';
    len = endword - word + 1;

    alias = AliasList;
    for( ;; ) {
        while( *alias == ' ' ) {
            ++alias;
        }
        if( _null( *alias ) )
            break;
        if( Equal( alias, word, len ) )
            break;
        while( *alias != '\r' ) {
            ++alias;
        }
        ++alias;
        if( *alias == '\n' ) {
            ++alias;
        }
    }


    *endword = ch;
    if( _null( *alias ) )
        return( NULL );
    *start = alias;
    while( !_white( *alias ) ) {
        ++alias;
    }
    while( _white( *alias ) ) {
        ++alias;
    }
    return( alias );
}

static char *LocateParm( char *parmlist, int parm )
/*************************************************/
{
    for( ;; ) {
        while( _white( *parmlist ) ) {
            ++parmlist;
        }
        if( _null( *parmlist ) )
            return( NULL );
        if( parm == 0 )
            return( parmlist );
        while( !_white_or_null( *parmlist ) ) {
            ++parmlist;
        }
        --parm;
    }
}

static int SubstParms( char __far *oldptr, char __far *newptr,
                      char *endword, int len )
/********************************************************/
{
    char        ch;
    int         parm;
    char        *parmptr;
    char        *endparm;
    char        __far *endptr;
    char        __far *environ;
    char        __far *overflow;
    char        kill[9];

    for( parm = 0; parm < 9; ++parm ) {
        kill[parm] = FALSE;
    }
    overflow = newptr + len - 1;
    while( *oldptr != '\r' && *oldptr != '\0' ) {
        if( oldptr[0] != '%' ) {
            if( newptr == overflow )
                return( FALSE );
            *newptr++ = *oldptr++;
        } else if( oldptr[1] == '%' ) {
            if( newptr == overflow )
                return( FALSE );
            *newptr++ = '%';
            oldptr += 2;
        } else if( ( ch = oldptr[1] ) >= '1' && ch <= '9' ) {
            parm = ch - '1';
            parmptr = LocateParm( endword, parm );
            if( parmptr != NULL ) {
                for( ;; ) {
                    ch = *parmptr++;
                    if( _white_or_null( ch ) )
                        break;
                    if( newptr == overflow )
                        return( FALSE );
                    *newptr++ = ch;
                }
                kill[parm] = TRUE;
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
                    if( environ != NULL ) {
                        while( !_null( *environ ) ) {
                            if( newptr == overflow )
                                return( FALSE );
                            *newptr++ = *environ++;
                        }
                    }
                    oldptr = endptr + 1;
                    break;
                }
                if( _white_or_null( *endptr ) ) {
                    if( newptr == overflow )
                        return( FALSE );
                    *newptr++ = *oldptr++;
                    break;
                }
            }
        }
    }
    *newptr = '\r';
    for( parm = 8; parm >= 0; --parm ) {
        if( kill[parm] ) {
            parmptr = LocateParm( endword, parm );
            endparm = parmptr;
            do {
                ch = *++endparm;
            } while( !_white_or_null( ch ) );
            ReplaceAlias( "", parmptr, endparm );
        }
    }
    return( TRUE );
}


int ReplaceAlias( char __far * alias, char * word, char * endword )
/*****************************************************************/
{
    char    __far *endalias;
    int     insert,i;
    char    newalias[LINE_WIDTH];

    if( !SubstParms( alias, newalias, endword, LINE_WIDTH ) ) {
        PutString( "\r\nCommand too long!\r\n" );
        return( 0 );
    }
    alias = newalias;
    for( endalias = alias; *endalias != '\0'; endalias++ ) {
        if( *endalias == '\r' ) {
            break;
        }
    }
    insert = ( endalias - alias ) - ( endword - word );
    if( ( MaxCursor + insert ) > Overflow )
        return( 0 );
    if( insert < 0 ) {
        for( i = word - Line; i < MaxCursor; i++ ) {
            Line[i] = Line[i - insert];
        }
    } else if( insert > 0 ) {
        for( i = MaxCursor; i-- > More; ) {
            if( Line + i < word )
                break;
            Line[i + insert] = Line[i];
        }
    }
    while( alias != endalias ) {
        *word++ = *alias++;
    }
    MaxCursor += insert;
    Cursor += insert;
    return( insert );
}

#if 0
static char *EndOfWord( char *word )
/**********************************/
{
    while( !WordSep( *word ) ) {
        ++word;
    }
    return( word );
}
#endif

void LookForAlias( void )
/***********************/
{
    char    *word;
    char    *endword;
    char    __far *start;
    char    __far *alias;

    for( ;; ) {
        Line[MaxCursor] = '\0';
        word = EatWhite( Line + More );
        endword = word;
        while( !_white_or_null( *endword ) && *endword != '/' ) {
            ++endword;
        }
        alias = FindAlias( word, endword, &start );
        if( alias == NULL )
            break;
        ReplaceAlias( alias, word, endword );
        *start |= 0x80;
    }
    for( alias = AliasList; !_null( *alias ); ++alias ) {
        *alias &= ~0x80;
    }
}

static int FormName( char * name, char scan )
/*******************************************/
{
    name[0] = '<';
    name[1] = 'F';
    if( scan <= F9 ) {
        name[2] = scan - ( F1 - 1 ) + '0';
        name[3] = '>';
        return( 4 );
    }
    if( scan == F10 ) {
        name[2] = '1';
        scan = 0;
    } else if( scan >= F40 ) {
        name[2] = '4';
        scan -= F40;
    } else if( scan >= F30 ) {
        name[2] = '3';
        scan -= F30;
    } else if( scan >= F20 ) {
        name[2] = '2';
        scan -= F20;
    } else {
        name[2] = '1';
        scan -= ( F11 - 1 );
    }
    name[3] = scan + '0';
    name[4] = '>';
    return( 5 );
}

void PFKey( void )
/****************/
{
    static char buff[14];
    int         i;
    char        __far *start;

    if( WantAlias ) {
        i = FormName( buff, KbdChar.chScan );
        PFChars = FindAlias( buff, ( buff + i ), &start );
        if( PFChars && *PFChars == '!' ) {
            ++PFChars;
            ImmedCommand = TRUE;
        }
    }
}
