/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
:segment OS2
* Description:  YACC driver for resource script parser, OS/2 grammar variant.
:elsesegment WIN16
* Description:  YACC driver for resource script parser, WIN16 grammar variant.
:elsesegment WIN32
* Description:  YACC driver for resource script parser, WIN32 grammar variant.
:endsegment
*
****************************************************************************/


#include <stdio.h>
#include "global.h"
#include "yydriver.h"
:segment OS2
#include "yydrivr2.gh"
:elsesegment WIN16
#include "yydrivr6.gh"
:elsesegment WIN32
#include "yydrivrw.gh"
:endsegment
#include "semantic.h"
:segment OS2
#include "semantc2.h"
:elsesegment
#include "semantcw.h"
:endsegment
#include "rcmem.h"
#include "rcerrors.h"
#include "errprt.h"
#include "rccore.h"
#include "memfuncs.h"
#include "wresdefn.h"


#define YYCHKTYPE       uint_16
#define YYACTTYPE       uint_16
#define YYPLHSTYPE      uint_16
#define YYPLENTYPE      uint_8

#ifdef _M_I86
    #define YYFAR       __far
#else
    #define YYFAR
#endif

#define STACK_MAX       100

typedef enum {
    P_SHIFT,
    P_ACCEPT,
    P_SYNTAX,
    P_ERROR
} p_action;

/*
 * definitions and tables here
 */

%%YACC%%

typedef union {
    ScanInt                         intinfo;
    unsigned long                   integral;
    IntMask                         maskint;
    ScanString                      string;
    WResID                          *resid;
    AccelEvent                      accevent;
:segment OS2
    FullAccelEntryOS2               accelfullentry;
    FullAccelFlagsOS2               accflags;
    FullAccelTableOS2               *acceltable;
:elsesegment
    FullAccelEntryWIN               accelfullentry;
    FullAccelFlagsWIN               accflags;
:endsegment
    ResLocation                     resloc;
    ResMemFlags                     resmemflags;
    MenuFlags                       menuflags;
:segment OS2
    FullMenuOS2                     *menuptr;
    FullMenuItemOS2                 menufull;
    MenuItemOS2                     menuitem;
:elsesegment
    FullMenuWIN                     *menuptr;
    FullMenuItemWIN                 menufull;
    MenuItemWIN                     menuitem;
:endsegment
    DialogSizeInfo                  sizeinfo;
    FullDialogOptions               diagopts;
    ResNameOrOrdinal                *nameorord;
    FullDialogBoxHeader             *diaghead;
:segment OS2
    FullDialogBoxControlOS2         *diagctrl;
    FullDialogCtrlListOS2           *diagctrllist;
    FullDialogCtrlOptionsOS2        diagctrlopts;
:elsesegment
    FullDialogBoxControlWIN         *diagctrl;
    FullDialogCtrlListWIN           *diagctrllist;
    FullDialogCtrlOptionsWIN        diagctrlopts;
:endsegment
    StringItem                      stritem;
    FullStringTable                 *strtable;
    RawDataItem                     rawitem;
    YYTOKENTYPE                     token;
    DataElemList                    *dataelem;
    uint_16                         residnum;
    uint_16                         ressizenum;
    uint_8                          resbyte;
:segment OS2
    FullHelpTableOS2                *helptable;
    FullHelpEntryOS2                helpfullentry;
    FullHelpSubTableOS2             *helpsubtable;
    PresParamsOS2                   presparams;
    PresParamListOS2                *presparamlist;
    uint_16                         resword;
    uint_16                         codepage;
:elsesegment
    VersionPair                     verpair;
    VerFixedOption                  verfixedoption;
    FullVerValueList                *valuelist;
    VerValueItem                    valueitem;
    FullVerBlockNest                *verblocknest;
    FullVerBlock                    *verblock;
    VerFixedInfo                    *verinforoot;
    WResLangType                    langinfo;
    ToolBar                         *toolbar;
:endsegment
} yystype;
#define YYSTYPE         yystype

/*
 * define value and state stacks for both expressions and declarations
 */
typedef struct {
    YYSTYPE             *vsp;
    YYSTYPE             *vstack;
    YYACTTYPE           *ssp;
    YYACTTYPE           *sstack;
} parse_stack;

static YYSTYPE yylval;
static bool    yysyntaxerror;   /* boolean variable */
#define YYERRORTHRESHOLD    5   /* no. of tokens to accept before restarting */

#ifdef YYDEBUG

static void dump_rule( unsigned rule )
{
    unsigned                i;
    const YYTOKENTYPE YYFAR *yytoken;
    const char YYFAR        *p;

    if( CmdLineParms.DebugParser ) {
        for( p = yytoknames[yyplhstab[rule]]; *p != '\0'; ++p ) {
            RcMsgFprintf( NULL, "%c", *p );
        }
        RcMsgFprintf( NULL, " <-" );
        yytoken = &yyrhstoks[yyrulebase[rule]];
        for( i = yyplentab[rule]; i != 0; --i ) {
            RcMsgFprintf( NULL, " " );
            for( p = yytoknames[*yytoken]; *p != '\0'; ++p ) {
                RcMsgFprintf( NULL, "%c", *p );
            }
            ++yytoken;
        }
        RcMsgFprintf( NULL, "\n" );
    }
}

static void puts_far( const char YYFAR *string )
{
    const char YYFAR            *p;

    if( CmdLineParms.DebugParser ) {
        for( p = string; *p != '\0'; ++p ) {
            RcMsgFprintf( NULL, "%c", *p );
        }
        RcMsgFprintf( NULL, "%c", '\n' );
    }
}

#endif

static YYTOKENTYPE yylex( void )
/******************************/
{
    YYTOKENTYPE curtoken;
    ScanValue   value;

:segment OS2
    curtoken = ScanOS2( &value );
:elsesegment WIN16
    curtoken = ScanW16( &value );
:elsesegment WIN32
    curtoken = ScanWIN( &value );
:endsegment

    while( RcIoIsCOrHFile() ) {
        switch( curtoken ) {
        case Y_NAME:
        case Y_STRING:
        case Y_DOS_FILENAME:
            MemFree( value.string.string );
            break;
        case Y_INTEGER:
            MemFree( value.intinfo.str );
            break;
        case YYEOFTOKEN:
    /**/    goto END_LOOP;
            break;
        case Y_SCAN_ERROR:
            break;          /* ignore unknown chars in a c or h file */
        default:
            break;
        }

:segment OS2
        curtoken = ScanOS2( &value );
:elsesegment WIN16
        curtoken = ScanW16( &value );
:elsesegment WIN32
        curtoken = ScanWIN( &value );
:endsegment
    }
    END_LOOP:

    switch( curtoken ) {
    case Y_INTEGER:
        yylval.intinfo = value.intinfo;
        break;
    case Y_NAME:
    case Y_STRING:
    case Y_DOS_FILENAME:
        yylval.string = value.string;
        break;
    case Y_SCAN_ERROR:
        RcError( ERR_UNKNOWN_CHAR, value.UnknownChar );
        break;
    default:
        break;
    }
    return( curtoken );
}

static YYACTTYPE find_action( YYACTTYPE yyk, YYTOKENTYPE yytoken )
{
    int     yyi;

    while( (yyi = yyk + yytoken) < 0 || yyi >= YYUSED || yychktab[yyi] != yytoken ) {
        if( (yyi = yyk + YYPARTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPARTOKEN ) {
            return( YYNOACTION );
        }
        yyk = yyacttab[yyi];
    }
    return( yyacttab[yyi] );
}

static p_action doAction( YYTOKENTYPE yytoken, parse_stack *state )
{
    YYSTYPE yyval = { 0 };
    YYSTYPE *yyvp;
    YYACTTYPE yyi;
    YYACTTYPE yyaction;
    YYACTTYPE rule;
    YYCHKTYPE yylhs;

    for( ;; ) {
        yyaction = find_action( *(state->ssp), yytoken );
        if( yyaction == YYNOACTION ) {
            yyaction = find_action( *(state->ssp), YYDEFTOKEN );
            if( yyaction == YYNOACTION ) {
                return( P_SYNTAX );
            }
        }
        if( yyaction < YYUSED ) {
            if( yyaction == YYSTOP ) {
                return( P_ACCEPT );
            }
            *++(state->ssp) = yyaction;
            *++(state->vsp) = yylval;
#ifdef YYDEBUG
            puts_far( yytoknames[yytoken] );
#endif
            return( P_SHIFT );
        }
        rule = yyaction - YYUSED;
        yyi = yyplentab[rule];
        state->vsp -= yyi;
        state->ssp -= yyi;
        if( state->ssp < state->sstack ) {
            return( P_ERROR );
        }
        yylhs = yyplhstab[rule];
        yyaction = find_action( *(state->ssp), yylhs );
        if( yyaction == YYNOACTION ) {
            return( P_ERROR );
        }
        *++(state->ssp) = yyaction;
        yyvp = ++(state->vsp);
#ifdef YYDEBUG
        dump_rule( rule );
#endif
        if( !yysyntaxerror ) {
            switch( rule ) {
%%YACC%%
            default:
                yyval = yyvp[0];
            }
        }
        *(state->vsp) = yyval;
    }
}

:segment OS2
void ParseInitOS2( void )
:elsesegment WIN16
void ParseInitW16( void )
:elsesegment WIN32
void ParseInitWIN( void )
:endsegment
/***********************/
{
    SemSetDefLang();
:segment OS2
    ScanInitOS2();
:elsesegment WIN16
    ScanInitW16();
:elsesegment WIN32
    ScanInitWIN();
:endsegment
}

:segment OS2
void ParseFiniOS2( void )
:elsesegment WIN16
void ParseFiniW16( void )
:elsesegment WIN32
void ParseFiniWIN( void )
:endsegment
/***********************/
{
:segment OS2
    ScanFiniOS2();
:elsesegment WIN16
    ScanFiniW16();
:elsesegment WIN32
    ScanFiniWIN();
:endsegment
}

static void initParseStack( parse_stack *stack )
{
    stack->vsp = stack->vstack;
    stack->ssp = stack->sstack;
    /*
     * initialize
     */
    *(stack->ssp) = YYSTART;
}

static void newParseStack( parse_stack *stack )
{
    /*
     * get new stack
     */
    stack->vstack = MemAllocSafe( STACK_MAX * sizeof( YYSTYPE ) );
    stack->sstack = MemAllocSafe( STACK_MAX * sizeof( YYACTTYPE ) );
    initParseStack( stack );
}

static void deleteStack( parse_stack *stack )
{
    if( stack->vstack ) {
        MemFree( stack->vstack );
    }
    if( stack->sstack ) {
        MemFree( stack->sstack );
    }
}

static void handleError( YYTOKENTYPE yytoken, parse_stack *state, bool error_state )
{
    if( !error_state ) {
        switch( yytoken ) {
        case Y_INTEGER:
            RcError( ERR_SYNTAX_STR, yylval.intinfo.str );
            MemFree( yylval.intinfo.str );
            break;
        case Y_NAME:
        case Y_STRING:
        case Y_DOS_FILENAME:
            RcError( ERR_SYNTAX_STR, yylval.string.string );
            MemFree( yylval.string.string );
            break;
        case Y_SCAN_ERROR:
            break;
        default:
:segment OS2
            RcError( ERR_SYNTAX_STR, SemOS2TokenToString( yytoken ) );
:elsesegment
            RcError( ERR_SYNTAX_STR, SemWINTokenToString( yytoken ) );
:endsegment
            break;
        }
    }

    initParseStack( state );
}

static p_action doParse( parse_stack *resource_state )
{
    p_action    what;
    bool        error_state;
    YYTOKENTYPE yytoken;
    int         token_count;

    error_state = false;
    token_count = 0;
    do {
        yytoken = yylex();
        if( error_state ) {
            token_count++;
            if( token_count >= YYERRORTHRESHOLD ) {
                error_state = false;
            }
        }

        what = doAction( yytoken, resource_state );

        if( what == P_SYNTAX ) {
            handleError( yytoken, resource_state, error_state );
            error_state = true;
            yysyntaxerror = true;
            ErrorHasOccured = true;
            token_count = 0;
        } else if( yytoken == Y_INTEGER && yylval.intinfo.str != NULL ) {
            MemFree( yylval.intinfo.str );
            yylval.intinfo.str = NULL;
        }
    } while( what != P_ACCEPT && what != P_ERROR );

    return( what );
}

:segment OS2
bool ParseOS2( void )
:elsesegment WIN16
bool ParseW16( void )
:elsesegment WIN32
bool ParseWIN( void )
:endsegment
/*************************/
{
    parse_stack resource_state;
    p_action    what;

    newParseStack( &resource_state );
    what = doParse( &resource_state );
    if( what == P_ERROR ) {
        RcError( ERR_PARSER_INTERNAL );
        ErrorHasOccured = true;
    }
    deleteStack( &resource_state );
    return( what != P_ACCEPT );
}

:segment OS2
void ParseInitStaticsOS2( void )
:elsesegment WIN16
void ParseInitStaticsW16( void )
:elsesegment WIN32
void ParseInitStaticsWIN( void )
:endsegment
/******************************/
{
    memset( &yylval, 0, sizeof( YYSTYPE ) );
    yysyntaxerror = false;
}
