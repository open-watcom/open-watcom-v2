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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "global.h"
#include "mem.h"
#include "error.h"
#include "keywords.h"
#include "hashtab.h"
#include "types.h"
#include "sruinter.h"
#include "ytab.gh"
#include "filelist.h"

extern void SetComment(void);

extern YYSTYPE yylval;

FILE    *yyin;

enum {
    CH_RET,     /* must be 0, single character return   */
    CH_ID,      /* identifier state                     */
    CH_INT,     /* integer state                        */
    CH_LIT,     /* literal string state                 */
    CH_COM,     /* comment state                        */
    CH_WS       /* white space                          */
};

/* cell definition for the fsm */
typedef struct fsm_cell {
    char        new_state;      /* new state on the transition  */
    char        class;          /* new class after transition   */
} fsm_cell;

/* commonly use constants */
#define LINE_INC                ( 8 * 1024 )
#define MAX_STATES              10
#define MAX_CHAR                256
#define CASE_SHIFT              'a' - 'A'

#define LS_INIT                 0
#define LS_NORMAL               1
#define LS_END_OF_FILE          -1

/* fsm table */
static fsm_cell fsmTable[MAX_STATES][MAX_CHAR];

/* macro for setting up the table */
#define _SetTable( state, ch, new, cls ) \
        (fsmTable[state][ch].new_state = new, fsmTable[state][ch].class = cls)

static int      lexStatus = LS_INIT;    /* initial lex status           */
static int      yyTextSize = 0;         /* size of current token buffer */
static char     *yyText = NULL;         /* token buffer                 */
static char     *yyLine = NULL;         /* Current line buffer          */
static int      yyLineCnt = 0;          /* Current line size            */
static char     idBuffer[LINE_INC];     /* delayed token destroy buffer */
static int      idPtr;                  /* pointer to current token     */
static hash_tab keyTable;               /* hash table of keywords       */
static BOOL     yyLineFini;             /* have we finished a line      */

static void     destroyLex( void );
static void     initLex( void );
static void     putBackChar( int c );
static int      nextChar( void );
static int      checkKeyWord( long hash, const char *id, int id_len, int *kval);
static char     *add2IdBuffer( char *src );

/*
 * main lexxer routine
 */
int yylex() {
/***********/

    int         c;
    int         state = 0;
    int         class = CH_RET;
    int         new_state;
    int         new_class;
    int         ccnt = 0;
    int         hash = 0;

    yyLineFini = FALSE;

    /* do initilization and eof checking */
    if( lexStatus == LS_INIT ) {
        initLex();
        lexStatus = LS_NORMAL;
    } else if( lexStatus == LS_END_OF_FILE ) {
        return( 0 );
    };

    /* Run the fsm for next token */
    for( ;; ) {
        c = nextChar();
        if( c == EOF ) {
            lexStatus = LS_END_OF_FILE;
            return( FI_EOF );
        }

        new_state = fsmTable[state][c].new_state;
        new_class = fsmTable[state][c].class;
        if( ( new_state == 0 ) && ( new_class == CH_RET ) ) {
            break;
        }

        hash = HashByte( hash, tolower( c ) );
        yyText[ ccnt++ ] = tolower( c );  // power builder is case insensitive
        if( ccnt > ( yyTextSize - 16 ) ) { // 16 byte safety zone
            yyTextSize += LINE_INC;
            yyText = MemRealloc( yyText, yyTextSize );
        }
        class = new_class;
        state = new_state;
    }

    /* process token */
    yyLine[ yyLineCnt ] = 0;
    yyText[ ccnt ] = 0;
    switch( class ) {
    case( CH_INT ):                             /* integer */
        putBackChar( c );
        yylval.o_value = atoi( yyText );
        return( CT_INTEGER );
    case( CH_ID ):                              /* identifier or keyword */
        putBackChar( c );
        switch( checkKeyWord( hash, yyText, ccnt, &c ) ) {
        case( ID_IDENTIFIER ):
            yylval.o_string = add2IdBuffer( yyText );
            return( ID_IDENTIFIER );
        default:
            /* if its not an identifier, it is a keyword */
            yylval.o_type = c;
            return( c );
        }
    case( CH_LIT ):                             /* string literal */
        yyText[ ccnt++ ] = c;
        yyText[ ccnt ] = 0;
        yylval.o_string = add2IdBuffer( yyText );
        return( CT_STRING );
    case( CH_COM ):                             /* comment */
        SetComment();
        if( c == '\n' ) {
            FinishLine();
            yyLineFini = TRUE;
            return( 0 ); /* execute parse */
        }
        /* else c == '/' */
        return( yylex() );
    case( CH_WS ):                              /* white space */
        putBackChar( c );
        return( yylex() );
    default:                                    /* single character */
        if( ( c == '\n' ) || ( c == ';' ) ) {
            FinishLine();
            return( 0 );
        }
        return( c );
    }
}


char *GetParsedLine( void ) {
/***************************/

    return( yyLine );
}


BOOL LineFinished( void ) {
/*************************/

    return( yyLineFini );
}


void    FinishLine( void ) {
/**************************/

    /* if necessary terminate line */
    if( yyLineCnt ) {
        yyLine[ yyLineCnt ] = 0;
        yyLineCnt = 0;
    }
}


void    GetToEOS(void) {
/**********************/

    int         c;

    /* process to end of statement, used for error handling */

    do {
        c = nextChar();
    } while( ( c != ';' ) && ( c != '\n' ) && ( c != EOF ) );

    if( c != EOF ) {
        putBackChar( c );
        yyLineCnt++;
    } else {
        yyLineCnt--;
    }

    yyLine[ yyLineCnt ] = 0;
}


static void initLex( void ) {
/****************************/

    int         x = 0;
    int         xs = 0;

    memset( fsmTable, 0, sizeof( fsm_cell ) * MAX_STATES * MAX_CHAR );

    /* Set white space transitions */
    _SetTable( 0, ' ', 9, CH_WS );
    _SetTable( 0, '\t', 9, CH_WS );
    _SetTable( 0, '\r', 9, CH_WS );
    _SetTable( 9, ' ', 9, CH_WS );
    _SetTable( 9, '\t', 9, CH_WS );
    _SetTable( 9, '\r', 9, CH_WS );

    /* set integer transitions */
    for( x = '0'; x <= '9'; x++ ) {
        _SetTable( 0, x, 1, CH_INT );
        _SetTable( 1, x, 1, CH_INT );
        _SetTable( 2, x, 2, CH_ID );
    }

    /* set identifier transitions */
    for( xs = CASE_SHIFT + 'A' , x = 'A'; x <= 'Z'; x++, xs++ ) {
        _SetTable( 0, x, 2, CH_ID );
        _SetTable( 1, x, 2, CH_ID );
        _SetTable( 2, x, 2, CH_ID );

        _SetTable( 0, xs, 2, CH_ID );
        _SetTable( 1, xs, 2, CH_ID );
        _SetTable( 2, xs, 2, CH_ID );
    }
    _SetTable( 0, '$', 2, CH_ID );
    _SetTable( 0, '#', 2, CH_ID );
    _SetTable( 0, '_', 2, CH_ID );
    _SetTable( 0, '-', 2, CH_ID );
    _SetTable( 0, '%', 2, CH_ID );

    _SetTable( 1, '$', 2, CH_ID );
    _SetTable( 1, '#', 2, CH_ID );
    _SetTable( 1, '_', 2, CH_ID );
    _SetTable( 1, '-', 2, CH_ID );
    _SetTable( 1, '%', 2, CH_ID );

    _SetTable( 2, '$', 2, CH_ID );
    _SetTable( 2, '#', 2, CH_ID );
    _SetTable( 2, '_', 2, CH_ID );
    _SetTable( 2, '-', 2, CH_ID );
    _SetTable( 2, '%', 2, CH_ID );

    /* set comment transitions */
    _SetTable( 0, '/', 3, CH_RET );
    _SetTable( 3, '/', 4, CH_COM );
    _SetTable( 3, '*', 5, CH_COM );
    for( x = 0; x < MAX_CHAR; x++ ) {
        _SetTable( 4, x, 4, CH_COM );
        _SetTable( 5, x, 5, CH_COM );
        _SetTable( 6, x, 5, CH_COM );
    }
    _SetTable( 4, '\n', 0, CH_RET );
    _SetTable( 5, '*', 6, CH_COM );
    _SetTable( 6, '/', 0, CH_RET );

    /* literal transitions */
    _SetTable( 0, '"', 7, CH_LIT );
    for( x = 0; x < MAX_CHAR; x++ ) {
        _SetTable( 7, x, 7, CH_LIT );
        _SetTable( 8, x, 7, CH_LIT );
    }
    _SetTable( 7, '~', 8, CH_LIT );
    _SetTable( 7, '"', 0, CH_RET );

    /* load keywords into hash table */
    keyTable = NewHashTable( HASH_PRIME );
    x = 0;
    while( Statements[x].key != NULL ) {
        InsertHashValue( keyTable, Statements[x].key, strlen(Statements[x].key),
                         &Statements[x] );
        x++;
    }

    yyTextSize = LINE_INC;
    yyText = MemMalloc( LINE_INC );
    yyLine = MemMalloc( LINE_INC );
    idPtr = 0;
    idBuffer[ 0 ] = 0;
//    atexit( destroyLex );
}


static void     putBackChar( int c ) {
/***********************************/

    assert( yyin );

    ungetc( c, yyin );
    yyLineCnt--;
}


static int      nextChar( void ) {
/********************************/

    int         c;

    assert( yyin );

    errno = 0;
    c = fgetc( yyin );
    if( ( c == EOF ) && errno ) {
        WigCloseFile( yyin );
        Error( FILE_READ_ERR );
    }
    yyLine[ yyLineCnt++ ] = c;
    return( c );
}


static void     destroyLex( void ) {
/**********************************/

        MemFree( yyText );
        MemFree( yyLine );
        DestroyHashTable( keyTable );
}


static int      checkKeyWord( long hash, const char *id, int id_len,
                              int *kval ) {
/*****************************************/

    keyword             *tmp;

    /* check if identifier is a keyword */

    assert( id );
    assert( kval );

    tmp = FindHashEntry( keyTable, hash, id, id_len );
    if( !tmp ) {
        return( ID_IDENTIFIER );
    } else {
        *kval = tmp->id;
        return( tmp->id );
    }
}


static char *add2IdBuffer( char *src ) {
/**************************************/

    int         len;

    assert( src );

    /* add token to identifier buffer for delayed destruction */

    len = strlen( src ) + 1;
    if( idPtr + len >= LINE_INC ) {
        idPtr = 0;
    }
    strcpy( idBuffer + idPtr, src );
    idPtr += len;
    return( idBuffer + idPtr - len );
}
