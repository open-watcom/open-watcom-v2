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
* Description:  Macro preprocessor for wmake (handles !ifdef and the like).
*
****************************************************************************/


#include <string.h>

#include "make.h"
#include "mstream.h"
#include "mlex.h"
#include "macros.h"
#include "mexec.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"

/*
 * This module presents a stream of characters to mlex.c.  Stripped from the
 * stream are comments, and preprocessing directives.  Preprocessing directives
 * are handled invisibly to mlex.c.  Also handles line continuation.
 *
 */

#define MAX_PRE_TOK     8       // chars needed for maximum keyword
#define INCLUDE      "INCLUDE"  // include directory for include file search


typedef enum {                  // must be kept in sync with directives
    D_BLANK = -1,               // a blank line
    #define pick(text,enum) enum,
    #include "mdirectiv.h"
    #undef pick
    D_MAX
} directiveTok;

STATIC const char * const directives[] = {   // table must be lexically sorted.
    #define pick(text,enum) text,
    #include "mdirectiv.h"
    #undef pick
};
#define NUM_DIRECT      D_MAX

#define MAX_DIR_LEN     8       // num chars incl null-terminator

STATIC char     atStartOfLine;  /* EOL at the start of a line...
 * This is a slight optimization for the critical code in PreGetCH().  DJG
 */
STATIC STRM_T   lastChar;
STATIC BOOLEAN  doingPreProc;   // are we doing some preprocessing?


/*
 * MS Compatability extension to add the if (expression) functionality
 */

STATIC void doElIf( BOOLEAN (*logical)(void), directiveTok tok );

// local functions
STATIC void parseExpr ( DATAVALUE *leftVal, char *inString );
STATIC void logorExpr ( DATAVALUE *leftVal );
STATIC void logandExpr( DATAVALUE *leftVal );
STATIC void bitorExpr ( DATAVALUE *leftVal );
STATIC void bitxorExpr( DATAVALUE *leftVal );
STATIC void bitandExpr( DATAVALUE *leftVal );
STATIC void equalExpr ( DATAVALUE *leftVal );
STATIC void relateExpr( DATAVALUE *leftVal );
STATIC void shiftExpr ( DATAVALUE *leftVal );
STATIC void addExpr   ( DATAVALUE *leftVal );
STATIC void multExpr  ( DATAVALUE *leftVal );
STATIC void unaryExpr ( DATAVALUE *leftVal );

STATIC char         *currentPtr;    // Pointer to current start in string
STATIC TOKEN_TYPE   currentToken;   // Contains information for current token


/*
 * struct nestIf is used to keep track of the if-endif constructs
 *
 * These are what the various fields mean:
 *
 * if skip then         we are skipping until an elif, else or endif
 * elif skip2endif then we are skipping everything until matching endif
 * else                 we are not skipping
 * endif
 *
 * if elseFound then    we have already come across the else that matches
 *                      the current if (for error checking)
 *
 * invariant( if( curNest.skip2endif ) then curNest.skip )
 */
struct nestIf {
    BIT skip2endif : 1;
    BIT skip : 1;
    BIT elseFound : 1;
};

#define MAX_NEST    32                  // maximum depth of if nesting

STATIC struct nestIf    nest[MAX_NEST]; // stack for nesting
STATIC nest_level       nestLevel;      // items on stack
STATIC struct nestIf    curNest;        // current skip info


void PreProcInit( void )
/*****************************/
{
    StreamInit();

    atStartOfLine = EOL;
    doingPreProc = FALSE;

    curNest.skip2endif = FALSE;
    curNest.skip = FALSE;
    curNest.elseFound = FALSE;
    nestLevel = 0;
    lastChar = 0;
}


void PreProcFini( void )
/*****************************/
{
    StreamFini();
}


STATIC STRM_T eatWhite( void )
/*****************************
 * pre:
 * post:    0 or more ws characters removed from input
 * returns: the first non-whitespace character is returned
 */
{
    STRM_T  s;

    s = PreGetCH();
    while( isws( s ) ) {
        s = PreGetCH();
    }

    return( s );
}


STATIC STRM_T eatToEOL( void )
/*****************************
 * pre:
 * post:    atStartOfLine == EOL, 0 or more chars removed from input
 * returns: first ( EOL || STRM_END )
 */
{
    STRM_T  s;

    s = PreGetCH();
    while( s != EOL && s != STRM_END ) {
        s = PreGetCH();
    }

    return( s );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC directiveTok getPreTok( void )
/************************************
 * pre:     the '!' has been eaten by caller
 * post:    first character following token is next char of input
 * returns: D_BLANK if no tokens on line, or token unrecognized,
 *          otherwise the D_ number of the token
 */
{
    STRM_T  s;
    char    tok[MAX_PRE_TOK];
    int     pos;
    char    **key;
    char    *tmp;               /* to pass tok buf to bsearch */

    s = eatWhite();

    if( s == EOL ) {
        UnGetCH( s );
        return( D_BLANK );
    }

    pos = 0;
    while( isalpha( s ) && ( pos < MAX_PRE_TOK - 1 ) ) {
        tok[pos++] = s;
        s = PreGetCH();
        // MS Compatability ELSE IFEQ can also be defined as ELSEIFEQ
        // similar for other types of if preprocessor directives
        if( pos == 4 ) {
            tok[pos] = NULLCHAR;
            if( strcmpi( directives[D_ELSE], tok ) == 0 ) {
                break;
            }
        }
    }
    tok[pos] = NULLCHAR;

    UnGetCH( s );
    UnGetCH( eatWhite() );

    tmp = tok;
    key = bsearch( &tmp, directives, NUM_DIRECT, sizeof( char * ),
           (int (*)( const void *, const void * )) KWCompare );

    if( key == NULL ) {
        if( !curNest.skip ) {
            PrtMsg( ERR | LOC | UNK_PREPROC_DIRECTIVE, tok );
        }
        return( D_BLANK );
    }

    assert( ( key - (char **)directives >= 0 ) &&
            ( key - (char **)directives <= D_MAX ) );

    return( (int)( key - (char **)directives ) );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC BOOLEAN ifDef( void )
/***************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro is defined, FALSE otherwise
 */
{
    char    *name;
    char    *value;
    BOOLEAN ret;

    assert( !curNest.skip2endif );

    name = DeMacro( MAC_PUNC );
    (void)eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return( FALSE );
    }

    value = GetMacroValue( name );
    ret = value != NULL;
    if( value != NULL ) {
        FreeSafe( value );
    }
    FreeSafe( name );

    return( ret );
}


STATIC BOOLEAN ifNDef( void )
/***************************/
{
    return( !ifDef() );
}


STATIC void chopTrailWS( char *str )
/***********************************
 * chop trailing whitespace from str
 */
{
    char    *p;

    for( p = str + strlen( str ) - 1; p >= str && isws( *p ); --p ) {
        *p = NULLCHAR;
    }
}


STATIC BOOLEAN ifOp( void )
/**************************
 * MS Compatability  -
 * Allows for NMAKE compatability in binary and string operators
 * process the operands found in !if
 */
{
    char        *test;
    DATAVALUE   temp;

    assert( !curNest.skip2endif );

    test = DeMacro( TOK_EOL );
    (void)eatToEOL();

    parseExpr( &temp, test );

    FreeSafe( test );
    return( (BOOLEAN)temp.data.number );
}


STATIC void ifEqProcess( char const **v1, char **v2 )
/****************************************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: v1 & v2 set to freeable strings
 */
{
    char        *name;
    char        *test;
    char        *value;
    char const  *beg;

    assert( !curNest.skip2endif );

    *v1 = NULL;
    *v2 = NULL;

    name = DeMacro( MAC_PUNC );
    test = DeMacro( TOK_EOL );
    (void)eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        FreeSafe( test );
        return;
    }

    value = WrnGetMacroValue( name );

    FreeSafe( name );               /* don't need name any more */

    if( value == NULL ) {
        FreeSafe( test );
        return;
    }

    UnGetCH( EOL );
    InsString( value, TRUE );
    value = DeMacro( TOK_EOL );
    (void)eatToEOL();

    chopTrailWS( test );            /* chop trailing ws */

    beg = SkipWS( test );           /* find first non-ws */

    *v1 = value;
    *v2 = StrDupSafe( beg );        /* 18-nov-91 */
    FreeSafe( test );
}


STATIC BOOLEAN ifEq( void )
/**************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro equals text, FALSE otherwise
 */
{
    BOOLEAN     ret;
    char const  *v1;
    char        *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = strcmp( v1, v2 ) == 0;

    FreeSafe( (void *)v1 );
    FreeSafe( v2 );

    return( ret );
}


STATIC BOOLEAN ifEqi( void )
/***************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro equals text (case independence), FALSE otherwise
 */
{
    BOOLEAN     ret;
    char const  *v1;
    char        *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = stricmp( v1, v2 ) == 0;

    FreeSafe( (void *)v1 );
    FreeSafe( v2 );

    return( ret );
}


STATIC BOOLEAN ifNEq( void )
/**************************/
{
    return( !ifEq() );
}


STATIC BOOLEAN ifNEqi( void )
/***************************/
{
    return( !ifEqi() );
}


STATIC void bangIf( BOOLEAN (*logical)(void), directiveTok tok )
/********************************************************************
 * pre:
 * post:    nestLevel > old(nestLevel); skip if false logical, or currently
 *          skipping; !elseFound
 * aborts:  if nestLevel >= MAX_NEST
 */
{
    if( nestLevel >= MAX_NEST ) {
        PrtMsg( FTL | LOC | IF_NESTED_TOO_DEEP );
    }

    nest[nestLevel++] = curNest; // save old nesting on the stack

        // remember that curNest still contains info from previous level
    curNest.skip2endif = (curNest.skip || curNest.skip2endif);
    curNest.skip = FALSE;
    curNest.elseFound = FALSE;

    if( !curNest.skip2endif ) { // ok to interpret if arguments?
        curNest.skip = !logical();
    } else {
        // this block is to be skipped, don't interpret args to if
        curNest.skip = TRUE;
        (void)eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, directives[tok] );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, directives[tok] );
    }
}


STATIC void bangEndIf( void )
/****************************
 * pre:
 * post:    atStartOfLine == EOL; curNest < old(curNest)
 * aborts:  if not nested
 */
{
    PrtMsg( DBG | INF | LOC | AT_ENDIF );

    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, directives[D_ENDIF] );
    }
    curNest = nest[--nestLevel];

    (void)eatToEOL();
}


STATIC void doElse( void )
/*************************
 * pre:
 * post:    elseFound; skip if !old( skip ) || skip2endif
 * aborts:  if not nested
 * errors:  if elseFound
 */
{
    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, directives[D_ELSE] );
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN | LOC | SKIPPING_AFTER_ELSE, directives[D_ELSE],
            directives[D_ELSE] );
        // must set these because we may not have been skipping previous block
        curNest.skip2endif = TRUE;
        curNest.skip = TRUE;
        return;
    }
    curNest.elseFound = TRUE;

    if( !curNest.skip2endif ) {
        // check we're not skipping. if !skip then we should skip the else part.
        if( !curNest.skip ) {
            // skip to the end - we've done a block in this nesting
            curNest.skip = TRUE;
            curNest.skip2endif = TRUE;
        } else {
            // we still haven't done block in this nesting, do the else portion
            curNest.skip = FALSE;
        }
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, directives[D_ELSE] );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, directives[D_ELSE] );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC void doElIf( BOOLEAN (*logical)(void), directiveTok tok )
/********************************************************************
 * post:    skip if !logical || skip2endif
 * aborts:  if not nested
 * errors:  if elseFound
 */
{
    char    buf[MAX_DIR_LEN * 2];

    FmtStr( buf, "%s %s", directives[D_ELSE], directives[tok] );

    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, buf );
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN | LOC | SKIPPING_AFTER_ELSE, buf, directives[D_ELSE] );
        // must set these because we may not have been skipping previous block
        curNest.skip2endif = TRUE;
        curNest.skip = TRUE;
        (void)eatToEOL();
        return;
    }

    if( !curNest.skip2endif ) {
        // check we're not skipping. if !skip, we should skip the else if part
        if( !curNest.skip ) {
            // skip to the end - we've done a block in this nesting
            curNest.skip = TRUE;
            curNest.skip2endif = TRUE;
            (void)eatToEOL();
        } else {
            // we still haven't done block in this nesting, try this logical.
            curNest.skip = !logical();
        }
    } else {
        (void)eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, buf );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, buf );
    }
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC void bangElse( void )
/***************************
 * pre:     next character of input is first after 'else' token
 * post:    atStartOfLine == EOL; skip may be modified
 * aborts:  if illegal directive
 */
{
    directiveTok   tok;

    tok = getPreTok();
    switch( tok ) {
    case D_BLANK:
        (void)eatToEOL();
        doElse();
        break;
    case D_IFDEF:   doElIf( ifDef,  D_IFDEF );  break;
    case D_IFEQ:    doElIf( ifEq,   D_IFEQ );   break;
    case D_IFEQI:   doElIf( ifEqi,  D_IFEQI );  break;
    case D_IFNDEF:  doElIf( ifNDef, D_IFNDEF ); break;
    case D_IF:      doElIf( ifOp,   D_IF );     break;
    case D_IFNEQ:   doElIf( ifNEq,  D_IFNEQ );  break;
    case D_IFNEQI:  doElIf( ifNEqi, D_IFNEQI ); break;
    default:
        (void)eatToEOL();
        PrtMsg( FTL | LOC | NOT_ALLOWED_AFTER_ELSE, directives[tok],
            directives[D_ELSE] );
    }
}


STATIC void bangDefine( void )
/*****************************
 * post:    atStartOfLine == EOL; macro may be defined
 * errors:  if !IsMacroName
 */
{
    char    *name;

    assert( !curNest.skip );

    name = DeMacro( MAC_PUNC );    /* decode name */

    if( !IsMacroName( name ) ) {
        (void)eatToEOL();
    } else {
        DefMacro( name );
    }
    FreeSafe( name );
}


static char *skipWhileWS( char *p )
/*********************************/
{
    while( isws( *p ) ) {
        ++p;
    }
    return( p );
}


static char *skipUntilWS( char *p )
/*********************************/
{
    while( *p != '\0' && !isws( *p ) ) {
        ++p;
    }
    return( p );
}


STATIC void bangInject( void )
/*****************************
 * !inject <text> <mac-name1> <mac-name2> ... <mac-nameN>
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char    *text;
    char    *contents;
    char    *end_contents;
    char    *curr;
    char    *mac_name;
    char    *value;

    assert( !curNest.skip );
    text = DeMacro( TOK_EOL );
    (void)eatToEOL();
    contents = skipWhileWS( text );
    if( *contents == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_contents = skipUntilWS( contents );
    if( *end_contents == '\0' ) {
        FreeSafe( text );
        return;
    }
    *end_contents = '\0';
    curr = end_contents + 1;
    for( ;; ) {
        curr = skipWhileWS( curr );
        if( *curr == '\0' ) break;
        mac_name = curr;
        curr = skipUntilWS( curr );
        if( *curr != '\0' ) {
            *curr = '\0';
            ++curr;
        }
        if( !IsMacroName( mac_name ) ) {
            break;
        }
        UnGetCH( EOL );
        InsString( contents, FALSE );
        value = GetMacroValue( mac_name );
        if( value != NULL ) {
            InsString( " ", FALSE );
            InsString( value, TRUE );
        }
        DefMacro( mac_name );
    }
    FreeSafe( text );
}


STATIC void bangLoadDLL( void )
/******************************
 * !loaddll <cmd-name> <dll-name> [<entry-pt>]
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char    *text;
    char    *cmd_name;
    char    *end_cmd_name;
    char    *dll_name;
    char    *end_dll_name;
    char    *ent_name;
    char    *end_ent_name;

    assert( !curNest.skip );
    text = DeMacro( TOK_EOL );
    (void)eatToEOL();
    cmd_name = skipWhileWS( text );
    if( *cmd_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_cmd_name = skipUntilWS( cmd_name );
    if( *end_cmd_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    *end_cmd_name = '\0';
    dll_name = skipWhileWS( end_cmd_name + 1 );
    if( *dll_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_dll_name = skipUntilWS( dll_name );
    if( *end_dll_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    *end_dll_name = '\0';
    ent_name = skipWhileWS( end_dll_name + 1 );
    if( *ent_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    end_ent_name = skipUntilWS( ent_name );
    if( *end_ent_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, ent_name );
        FreeSafe( text );
        return;
    }
    *end_ent_name = '\0';
    OSLoadDLL( cmd_name, dll_name, ent_name );
    FreeSafe( text );
}


STATIC void bangUnDef( void )
/****************************
 * post:    atStartOfLine == EOL; macro may be undefined
 * errors:  if env_variable || !IsMacroName || not-defined
 */
{
    char    *name;
    char    *value;

    assert( !curNest.skip );

    name = DeMacro( MAC_PUNC );
    (void)eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return;
    }

    value = GetMacroValue( name );
    if( value == NULL ) {
        PrtMsg( WRN | LOC | TRYING_UNDEF_UNDEF, directives[D_UNDEF] );
        FreeSafe( name );
        return;
    }

    UnDefMacro( name );
    FreeSafe( name );
    FreeSafe( value );
}


STATIC char *formatLongFileName( char *text )
/*******************************************/
{
    char    *ret;
    char    *currentRet;
    char    *currentTxt;

    assert( text != NULL );
    ret = StrDupSafe( text );
    currentRet = ret;
    currentTxt = text;

    if( currentTxt[0] == DOUBLEQUOTE ) {
        ++currentTxt;
    }
    while( *currentTxt != NULLCHAR && *currentTxt != DOUBLEQUOTE ) {
        if( *currentTxt == '\\' ) {
            if( *(currentTxt + 1) == DOUBLEQUOTE ) {
                ++currentTxt;
            }
        }
        *(currentRet++) = *(currentTxt++);
    }
    *currentRet = NULLCHAR;
    if( *currentTxt == DOUBLEQUOTE ) {
        if( *(currentTxt + 1) != NULLCHAR ) {
            PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
            FreeSafe( ret );
            return( NULL );
        }
    }
    return( ret );
}


STATIC void bangInclude( void )
/******************************
 * post:    atStartOfLine == EOL; maybe( new file in stream )
 * errors:  if unable to InsertFile()
 * for MS-Compatability we add <> angular bracket support to look
 *    for the file in the INCLUDE directory
 */
{
    char    *text;
    char    *temp = NULL;
#ifdef __WATCOMC__
    char    *current;
    char    full_path[_MAX_PATH];
    RET_T   ret;
#endif

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    (void)eatToEOL();

    chopTrailWS( text );    /* get rid of trailing ws */

#ifdef __WATCOMC__
    if( *text == LESSTHAN ) {
        current = text;
        while( *current != GREATERTHAN && *current != NULLCHAR ) {
            ++current;
        }
        if( *current == GREATERTHAN ) {
            *current = NULLCHAR;
            temp = text;
            text = formatLongFileName( temp + 1 );
            if( text == NULL ) {
                FreeSafe( temp );
                return;
            }
            for( ;; ) {
                if( *current == NULLCHAR ) {
                    _searchenv( text, INCLUDE, full_path );
                    ret = RET_ERROR;
                    if( *full_path != NULLCHAR ) {
                        ret = InsFile( full_path, FALSE );
                    }
                    if( ret == RET_ERROR ) {
                        PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
                    }
                    break;
                }
                // check if there are any trailing characters if there are
                // then error
                if( !isws( *current ) ) {
                    PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
                    break;
                }
                ++current;
            }
        } else {
              PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
        }
    } else
#endif
    {
        temp = text;
        text = formatLongFileName( text );
        if( text == NULL ) {
            FreeSafe( temp );
            return;
        }
        if( InsFile( text, FALSE ) != RET_SUCCESS ) {
            PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
        }
    }
    FreeSafe( temp );
    FreeSafe( text );
}


STATIC void bangMessage( void )
/******************************
 * post:    atStartOfLine == EOL
 */
{
    char    *text;

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    (void)eatToEOL();

    chopTrailWS( text );

    PrtMsg( PRNTSTR, text );
    FreeSafe( text );
}


STATIC void bangError( void )
/****************************
 * post:    atStartOfLine == EOL
 */
{
    char    *text;

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    (void)eatToEOL();

    chopTrailWS( text );

    PrtMsg( ERR | LOC | NEOL | USER_ERROR );
    PrtMsg( ERR | PRNTSTR, text );
    FreeSafe( text );
}


STATIC void handleBang( void )
/*****************************
 * post:    atStartOfLine == EOL
 */
{
    directiveTok   tok;

    tok = getPreTok();
    /* these are executed regardless of skip */
    switch( tok ) {
    case D_BLANK:   (void)eatToEOL();           break;
    case D_ELSE:    bangElse();                 break;
    case D_ENDIF:   bangEndIf();                break;
    case D_IF:      bangIf( ifOp,   D_IF );     break;
    case D_IFDEF:   bangIf( ifDef,  D_IFDEF );  break;
    case D_IFEQ:    bangIf( ifEq,   D_IFEQ );   break;
    case D_IFEQI:   bangIf( ifEqi,  D_IFEQI );  break;
    case D_IFNDEF:  bangIf( ifNDef, D_IFNDEF ); break;
    case D_IFNEQ:   bangIf( ifNEq,  D_IFNEQ );  break;
    case D_IFNEQI:  bangIf( ifNEqi, D_IFNEQI ); break;
    default:
        if( !curNest.skip ) {
            /* these are only executed if !skip */
            switch( tok ) {
            case D_DEFINE:  bangDefine();       break;
            case D_ERROR:   bangError();        break;
            case D_MESSAGE: bangMessage();      break;
            case D_INCLUDE: bangInclude();      break;
            case D_INJECT:  bangInject();       break;
            case D_LOADDLL: bangLoadDLL();      break;
            case D_UNDEF:   bangUnDef();        break;
            default:
                break;
            }
        } else {
            (void)eatToEOL(); /* otherwise, we just eat it up */
        }
    }
}


static int PreTestString( const char *str )
/******************************************
 * Test if 'str' is the next sequence of characters in stream.
 * If not, push back any characters read.
 */
{
    const char  *p = str;
    STRM_T      s;
    int         rc = FALSE;

    for( ;; ) {
        s = GetCHR();
        if( s != *p ) {
            UnGetCH( s );
            while( p-- > str ) {
                UnGetCH( *p );
            }
            break;
        }
        ++p;
        if( *p == '\0' ) {
            rc = TRUE;
            break;
        }
    }
    return( rc );
}


STRM_T PreGetCH( void )
/*****************************
 * returns: next character of input that is not a preprocessor directive
 * errors:  if an EOF occurs while nested
 */
{
    STRM_T  s;
    STRM_T  temp;
    BOOLEAN skip;

    s = GetCHR();
    if( !Glob.preproc ) {
        return( s );
    }

    for( ;; ) {
        if( !doingPreProc && (atStartOfLine == EOL || s == STRM_TMP_EOL) ) {
            if( s == STRM_TMP_EOL ) {
                // Throw away the unwanted TMP character
                s = GetCHR();
                if( s != BANG ) {
                    UnGetCH( s );
                    s = STRM_TMP_EOL;
                }
            }
            doingPreProc = TRUE;

            if( Glob.compat_nmake || Glob.compat_posix ) {
                /* Check for NMAKE and UNIX compatible 'include' directive */
                if( s == 'i' && PreTestString( "nclude " ) ) {
                    UnGetCH( eatWhite() );
                    bangInclude();
                    s = GetCHR();
                }
            }
            while( s == BANG ) {
                handleBang();

                assert( atStartOfLine == EOL );

                s = GetCHR();
            }
            doingPreProc = FALSE;
        }

        /* now we have a character of input */

        atStartOfLine = s;
        temp          = s;

        skip = curNest.skip && !doingPreProc;

        if( s == STRM_TMP_EOL ) {
            s = GetCHR();
        }
        if( s == COMMENT && lastChar != DOLLAR && inlineLevel == 0 ) {
            s = GetCHR();
            while( s != EOL && s != STRM_END ) {
                s = GetCHR();
            }
            if( temp == STRM_TMP_EOL ) {
                s = STRM_TMP_EOL;
            }
            /* we already have next char in t */
            continue;
        }

        if( s == STRM_END ) {
            curNest.skip = FALSE;       /* so we don't skip a later file */
            curNest.skip2endif = FALSE;

            atStartOfLine = EOL;   /* reset preprocessor */
            lastChar = STRM_END;
            return( s );
        }

        if( inlineLevel > 0 ) {   // We are currently defining an inline file
            lastChar = s;         // ignore all special characters ie {nl}
            if( skip ) {
                s = GetCHR();
                continue;
            }
            return( s );
        } else {
            if( Glob.compat_nmake && s == MS_LINECONT ) {
                s = GetCHR();
                if( s == EOL ) {
                    lastChar = SPACE;
                    if( skip ) {
                        s = STRM_TMP_EOL;
                        continue;
                    }
                    // place holder for temporary EOL
                    // this is to be able to implement the
                    // bang statements after line continues
                    UnGetCH( STRM_TMP_EOL );
                    return( SPACE );
                } else {
                    lastChar = MS_LINECONT;
                    if( skip ) {
                        s = GetCHR();
                        continue;
                    }
                    UnGetCH( s );
                    return( MS_LINECONT );
                }
            }

            if( s != LINECONT ) {
                if( s != UNIX_LINECONT || !Glob.compat_unix ) {
                    lastChar = s;
                    if( skip ) {
                        s = GetCHR();   /* must get next char */
                        continue;
                    }
                    return( s );
                }
                s = GetCHR();
                if( s != EOL ) {
                    lastChar = UNIX_LINECONT;
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCH( s );
                    return( UNIX_LINECONT );
                } else {
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCH( STRM_TMP_EOL );
                }
            } else {
                s = GetCHR();           /* check if '&' followed by {nl} */
                if( s != EOL || lastChar == '^' ||
                    lastChar == '[' || lastChar == ']' ) {
                           /* nope... restore state */
                    lastChar = LINECONT;
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCH( s );
                    return( LINECONT );
                } else {
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCH( STRM_TMP_EOL );
                }
            }
        }
        s = GetCHR();
    }
}


STATIC void makeToken( TOKEN_O type, TOKEN_TYPE *current, int *index )
/********************************************************************/
{
    switch( type ) {
        case OP_COMPLEMENT:
        case OP_LOG_NEGATION:
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULUS:
        case OP_BIT_AND:
        case OP_BIT_OR:
        case OP_BIT_XOR:
        case OP_LESSTHAN:
        case OP_GREATERTHAN:
        case OP_PAREN_LEFT:
        case OP_PAREN_RIGHT:
        case OP_ERROR:
            (*index)++;
            break;
        case OP_LOG_AND:
        case OP_LOG_OR:
        case OP_SHIFT_LEFT:
        case OP_SHIFT_RIGHT:
        case OP_INEQU:
        case OP_EQUAL:
        case OP_LESSEQU:
        case OP_GREATEREQU:
            (*index) += 2;
            break;
        default:
            current->type = OP_ERROR;
            (*index)++;
    }

    current->type = type;
}


STATIC INT32 makeHexNumber( char *inString, int *stringLength )
/*************************************************************/
{
    INT32   value;
    char    c;
    char    *currentChar;

    value = 0;
    currentChar = inString;
    for( ;; ) {
        c = currentChar[0];
        if( c >= '0' && c <= '9' ) {
            c = c - '0';
        } else if( c >= 'a' && c <= 'f' ) {
            c = c - 'a' + 10;
        } else if( c >= 'A' && c <= 'F' ) {
            c = c - 'A' + 10;
        } else {
            break;
        }
        value = value * 16 + c;
        ++currentChar;
    }
    *stringLength = (int)( currentChar - inString );
    return( value );
}


STATIC void makeNumberToken( char *inString, TOKEN_TYPE *current, int *index )
/****************************************************************************/
{
    INT32   value;
    char    *currentChar;
    char    c;
    int     hexLength;

    currentChar = inString;
    value       = 0;
    *index      = 0;
    c = currentChar[0];
    if( c == '0' ) {                            // octal or hex number
        ++currentChar;
        c = currentChar[0];
        if( c == 'x'  ||  c == 'X' ) {          // hex number
            ++currentChar;
            value = makeHexNumber( currentChar, &hexLength );
            currentChar += hexLength;
        } else {                                // octal number
            while( c >= '0'  &&  c <= '7' ) {
                value = value * 8 + c - '0';
                ++currentChar;
                c = currentChar[0];
            }
        }
    } else {                                    // decimal number
        while( c >= '0'  &&  c <= '9' ) {
            value = value * 10 + c - '0';
            ++currentChar;
            c = currentChar[0];
        }
    }
    current->data.number = value;
    current->type = OP_INTEGER;

    *index = (int)( currentChar - inString );
}


STATIC void makeStringToken( char *inString, TOKEN_TYPE *current, int *index )
/****************************************************************************/
{
    int inIndex;
    int currentIndex;

    inIndex       = 1;   // skip initial DOUBLEQUOTE
    currentIndex  = 0;
    current->type = OP_STRING;
    for( ;; ) {
        if( inString[inIndex] == DOUBLEQUOTE ) {
            // skip the second double quote
            ++inIndex;
            break;
        }
        if( currentIndex >= MAX_STRING ) {
            current->type = OP_ENDOFSTRING;
            break;
        }
        switch( inString[inIndex] ) {
            // error did not find closing quotation
            case NULLCHAR :
            case EOL:
            case COMMENT:
                current->type = OP_ERROR;
                break;
            default:
                current->data.string[currentIndex] = inString[inIndex];
        }

        if( current->type == OP_ERROR ) {
            break;
        }
        ++inIndex;
        ++currentIndex;
    }

    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}

STATIC void makeAlphaToken( char *inString, TOKEN_TYPE *current, int *index )
/***************************************************************************/
{
    char const  *r;
    char        *pwrite;
    char const  *pwritelast;

    r = inString;
    pwrite = current->data.string;
    pwritelast = pwrite + sizeof( current->data.string ) - 1;
    current->type = OP_STRING;

    // Note that in this case we are looking at a string that has no quotations
    // nmake gives expected error with exists(a(b) but also with exists("a(b")
    while( *r != PAREN_RIGHT && *r != PAREN_LEFT && !isws( *r ) ) {
        if( pwrite >= pwritelast ) {
            // VC++ 6 nmake allows 512 or more bytes here. We limit to 255.
            current->type = OP_ENDOFSTRING; // This truncates.
            break;
        }
        *pwrite++ = *r++;
    }

    *pwrite = NULLCHAR;
    *index = (int)( r - inString );
}



STATIC char *probeToken( char *probe )
/*************************************
 * Taking a peek at the next "token"
 */
{
    while( isws( *probe ) ) {
        ++probe;
    }
    return( probe );
}


STATIC BOOLEAN IsMacro( char const *name )
/****************************************/
{
    char    *value;

    // Seemingly redundant but GetMacroValue() needs plausible name
    if( !IsMacroName( name ) ) {
       return( FALSE );
    }
    value = GetMacroValue( name );

    if( value == NULL ) {
        return( FALSE );
    }
    FreeSafe( value );
    return( TRUE );
}


STATIC BOOLEAN name2function( TOKEN_TYPE const *current, char const *criterion,
    BOOLEAN (*action)( const char * ) , BOOLEAN (**pquestion)( const char * ) )
/*****************************************************************************/
{
    if( strcmpi( current->data.string, criterion ) != 0 ) {
        return( FALSE );
    }
    *pquestion = action;
    return( TRUE );
}

STATIC void makeFuncToken( char *inString, TOKEN_TYPE *current, int *index )
/***************************************************************************
 * parses only to get alphanumeric characters for special functions
 * ie. EXIST, defined.  if special characters are needed enclose in quotes
 */
{
    char    *probe;

    makeAlphaToken( inString, current, index );
    // check that the next token is a '(', swallow it, and check we have more.
    probe = probeToken( currentPtr + *index );
    if( *probe != PAREN_LEFT
    || (probe = probeToken( probe + 1), *probe == NULLCHAR) ) {
        current->type = OP_ERROR;
    } else {
        BOOLEAN (*is)( const char * );

        if( name2function( current, DEFINED, IsMacro,   &is )
        ||  name2function( current, EXIST,   existFile, &is )
        ||  name2function( current, EXISTS,  existFile, &is ) ) {
            if( *probe == DOUBLEQUOTE ) {   // Get macro or file name
                makeStringToken( probe, current, index );
            } else {
                makeAlphaToken( probe, current, index );
            }
            probe += *index;
            if( current->type == OP_STRING ) {
                probe = probeToken( probe );
                if( *probe != PAREN_RIGHT ) {
                    current->type = OP_ERROR;
                } else {
                    current->type          = OP_INTEGER;
                    current->data.number   = is( current->data.string );
                    ++probe;    // Swallow OP_PAREN_RIGHT
                    *index = (int)( probe - currentPtr );
                }
            }
        } else {
            current->type = OP_ERROR;
        }
    }
}

STATIC void makeCmdToken( char *inString, TOKEN_TYPE *current, int *index )
/**************************************************************************
 * get a command token enclosed in square brackets; very basic - a right
 * square bracket terminates command regardless of quoting
 */
{
    int     inIndex;
    int     currentIndex;

    inIndex       = 1;   // skip opening bracket
    currentIndex  = 0;
    current->type = OP_SHELLCMD;
    for( ;; ) {
        if( inString[inIndex] == BRACKET_RIGHT ) {
            // skip the closing bracket
            ++inIndex;
            break;
        }
        if( currentIndex >= MAX_STRING ) {
            current->type = OP_ENDOFSTRING;
            break;
        }
        switch( inString[inIndex] ) {
            // error did not find closing quotation
            case NULLCHAR :
            case EOL:
            case COMMENT:
                current->type = OP_ERROR;
                break;
            default:
                current->data.string[currentIndex] = inString[inIndex];
        }

        if( current->type == OP_ERROR ) {
            break;
        }
        ++inIndex;
        ++currentIndex;
    }

    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}

STATIC void ScanToken( char *inString, TOKEN_TYPE *current, int *tokenLength )
/****************************************************************************/
{
    char    *currentString;
    int     index = 0;

    currentString = SkipWS( inString );

    if( currentString[index] != EOL ||
        currentString[index] != NULLCHAR ||
        currentString[index] != COMMENT ) {
        switch( currentString[index] ) {
        case COMPLEMENT:
            makeToken( OP_COMPLEMENT, current, &index );
            break;
        case ADD:
            makeToken( OP_ADD, current, &index );
            break;
        case SUBTRACT:
            makeToken( OP_SUBTRACT, current, &index );
            break;
        case MULTIPLY:
            makeToken( OP_MULTIPLY, current, &index );
            break;
        case DIVIDE:
            makeToken( OP_DIVIDE, current, &index );
            break;
        case MODULUS:
            makeToken( OP_MODULUS, current, &index );
            break;
        case BIT_XOR:
            makeToken( OP_BIT_XOR, current, &index );
            break;
        case PAREN_LEFT:
            makeToken( OP_PAREN_LEFT, current, &index );
            break;
        case PAREN_RIGHT:
            makeToken( OP_PAREN_RIGHT, current, &index );
            break;
        case LOG_NEGATION:
            switch( currentString[index + 1] ) {
            case EQUAL:
                makeToken( OP_INEQU, current, &index );
                break;
            default:
                makeToken( OP_LOG_NEGATION, current, &index );
                break;
            }
            break;
        case BIT_AND:
            switch( currentString[index + 1] ) {
            case BIT_AND:
                makeToken( OP_LOG_AND, current, &index );
                break;
            default:
                makeToken( OP_BIT_AND, current, &index );
                break;
            }
            break;
        case BIT_OR:
            switch( currentString[index + 1] ) {
            case BIT_OR:
                makeToken( OP_LOG_OR, current, &index );
                break;
            default:
                makeToken( OP_BIT_OR, current, &index );
                break;
            }
            break;
        case LESSTHAN:
            switch( currentString[index + 1] ) {
            case LESSTHAN:
                makeToken( OP_SHIFT_LEFT, current, &index );
                break;
            case EQUAL:
                makeToken( OP_LESSEQU, current, &index );
                break;
            default:
                makeToken( OP_LESSTHAN, current, &index );
                break;
            }
            break;
        case GREATERTHAN:
            switch( currentString[index + 1] ) {
            case GREATERTHAN:
                makeToken( OP_SHIFT_RIGHT, current, &index );
                break;
            case EQUAL:
                makeToken( OP_GREATEREQU, current, &index );
                break;
            default:
                makeToken( OP_GREATERTHAN, current, &index );
                break;
            }
            break;
        case EQUAL:
            switch( currentString[index + 1] ) {
            case EQUAL:
                makeToken( OP_EQUAL, current, &index );
                break;
            default:
                makeToken( OP_ERROR, current, &index );
                break;

            }
            break;
        case DOUBLEQUOTE:
            makeStringToken( currentString, current, &index );
            break;
        case BRACKET_LEFT:
            makeCmdToken( currentString, current, &index );
            break;
        default:
            if( currentString[index] >= '0' && currentString[index] <= '9') {
                makeNumberToken( currentString, current, &index );
            } else {
                makeFuncToken( currentString, current, &index );
            }
            break;
        }
    } else {
        makeToken( OP_ENDOFSTRING, current, &index );
    }

    *tokenLength = index + (int)( currentString - inString );
}


STATIC void nextToken( void )
/****************************
 * Get the next token
 */
{
    int tokenLength;

    if( *currentPtr != NULLCHAR ) {
        ScanToken( currentPtr, &currentToken, &tokenLength );
        currentPtr += tokenLength;
        while( isws( *currentPtr ) ) {
            ++currentPtr;
        }
    } else {
        currentToken.type = OP_ERROR;  // no more tokens
    }
}


STATIC void parseExpr( DATAVALUE *leftVal, char *inString )
/**********************************************************
 * Calls functions that parses and evaluates the given expression
 * contained in global variables currentPtr (pointer to the string to be
 * parsed)
 */
{
    assert( inString != NULL && leftVal != NULL );
    currentPtr = inString;
    nextToken();
    logorExpr( leftVal );
    if( leftVal->type != OP_INTEGER ) {
        PrtMsg( ERR | LOC | PARSE_IFEXPR );
    }
}


STATIC void logorExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    logandExpr( leftVal );
    while( currentToken.type == OP_LOG_OR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        logandExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number || rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void logandExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    bitorExpr( leftVal );
    while( currentToken.type == OP_LOG_AND && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitorExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number && rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitorExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    bitxorExpr( leftVal );
    while( currentToken.type == OP_BIT_OR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitxorExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number | rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitxorExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    bitandExpr( leftVal );
    while( currentToken.type == OP_BIT_XOR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitandExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number ^ rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitandExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    equalExpr( leftVal );
    while( currentToken.type == OP_BIT_AND && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        equalExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number & rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void equalExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    relateExpr( leftVal );
    while( leftVal->type == OP_INTEGER || leftVal->type == OP_STRING ) {
        if( currentToken.type == OP_EQUAL ) {
            nextToken();
            relateExpr( &rightVal );
            if( leftVal->type == rightVal.type ) {
                switch( leftVal->type ) {
                case OP_INTEGER:
                    leftVal->data.number =
                        (leftVal->data.number == rightVal.data.number);
                    break;
                case OP_STRING:
                    leftVal->data.number =
                        !strcmp( leftVal->data.string, rightVal.data.string );
                    leftVal->type = OP_INTEGER;
                    break;
                default:
                    // error
                    break;
                }
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_INEQU ) {
            nextToken();
            relateExpr( &rightVal );
            if( leftVal->type == rightVal.type ) {
                switch( leftVal->type ) {
                case OP_INTEGER:
                    leftVal->data.number =
                        (leftVal->data.number != rightVal.data.number);
                    break;
                case OP_STRING:
                    leftVal->data.number =
                        !!strcmp( leftVal->data.string, rightVal.data.string );
                    leftVal->type = OP_INTEGER;
                    break;
                default:
                    // error
                    break;
                }
            } else {
                leftVal->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void relateExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rVal;

    shiftExpr( leftVal );
    while( leftVal->type != OP_ERROR && leftVal->type != OP_STRING ) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_LESSTHAN ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = leftVal->data.number < rVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_LESSEQU ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = leftVal->data.number <= rVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_GREATERTHAN ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = leftVal->data.number > rVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_GREATEREQU ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = leftVal->data.number >= rVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void shiftExpr( DATAVALUE *leftValue )
/*******************************************/
{
    DATAVALUE   rightValue;
    unsigned_32 *leftNumber;

    addExpr( leftValue );
    leftNumber = (void *)&leftValue->data.number;
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING ) {
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_SHIFT_LEFT ) {
            nextToken();
            addExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                *leftNumber <<= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_SHIFT_RIGHT ) {
            nextToken();
            addExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                *leftNumber >>= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void addExpr( DATAVALUE *leftValue )
/*****************************************/
{
    DATAVALUE   rightValue;

    multExpr( leftValue );
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING) {
        if( leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_ADD ) {
            nextToken();
            multExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number += rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if( currentToken.type == OP_SUBTRACT ) {
            nextToken();
            multExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number -= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void multExpr( DATAVALUE *leftValue )
/******************************************/
{
    DATAVALUE   rightValue;

    unaryExpr( leftValue );
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING ) {
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_MULTIPLY ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number *= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if( currentToken.type == OP_DIVIDE ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number /= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_MODULUS ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number %= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


BOOLEAN existFile( char const *inPath )
/*********************************************
 * This function is to determine whether or not a particular
 * filename / directory exists  (for use with EXIST())
 */
{
     if( access( inPath, F_OK ) == 0 ) {
         return( TRUE );
     }
     return( FALSE );
}


STATIC void unaryExpr( DATAVALUE *leftValue )
/********************************************
 * handles the unary expressions, strings and numbers
 */
{
    switch( currentToken.type ) {
    case OP_ADD:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_SUBTRACT:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            leftValue->data.number = - leftValue->data.number;
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_LOG_NEGATION:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            leftValue->data.number = !(leftValue->data.number);
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_COMPLEMENT:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            unsigned_32 *leftNumber;

            leftNumber = (void *)&leftValue->data.number;
            *leftNumber = ~*leftNumber;
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_PAREN_LEFT:
        nextToken();
        logorExpr( leftValue );
        if( currentToken.type == OP_PAREN_RIGHT ) {
            nextToken();
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_STRING:
        leftValue->type = currentToken.type;
        strcpy( leftValue->data.string, currentToken.data.string );
        nextToken();
        break;
    case OP_INTEGER:
        leftValue->type        = currentToken.type;
        leftValue->data.number = currentToken.data.number;
        nextToken();
        break;
    case OP_SHELLCMD:
        leftValue->type        = OP_INTEGER;
        PrtMsg( DBG | INF | EXECING_CMD, currentToken.data.string );
        leftValue->data.number = ExecCommand( currentToken.data.string );
        PrtMsg( DBG | INF | CMD_RETCODE, leftValue->data.number );
        nextToken();
        break;
    default:
        leftValue->type        = OP_ERROR;
        // error
    }
}


nest_level GetNestLevel( void )
/*****************************/
{
    return( nestLevel );
}
