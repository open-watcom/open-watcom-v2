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
* Description:  Macro processing, part one.
*
****************************************************************************/


#include "plusplus.h"
#include "preproc.h"
#include "tokenbuf.h"
#include "memmgr.h"
#include "macro.h"
#include "ring.h"
#include "fmtsym.h"
#include "initdefs.h"
#include "yydriver.h"
#include "carve.h"
#include "dumpapi.h"
#include "cscanbuf.h"
#ifndef NDEBUG
    #include "dbg.h"
#endif


#define FUNCTION_not_expandable (CompFlags.cpp_output)

#define MACRO_END_CHAR          'Z'
#define MACRO_END_STRING        "Z-<end of macro>"

typedef enum exp_state {
    EXPANDABLE_NO       = 0,    // macro is currently not expandable
    EXPANDABLE_YES      = 1,    // macro is expandable
    EXPANDABLE_WSSKIP   = 2     // we skipped over some white space
} exp_state;

typedef struct _tokens TOKEN_LIST;
struct _tokens {
    TOKEN_LIST  *next;
    unsigned    length;
    char        buf[1];
};

typedef struct macro_token MACRO_TOKEN;
struct macro_token {
    MACRO_TOKEN *next;
    TOKEN       token;
    char        data[1];
};

typedef struct macro_arg {
    char        *arg;
} MACRO_ARG;

typedef struct nested_macros NESTED_MACRO;
struct nested_macros {
    NESTED_MACRO    *next;
    MEPTR           mentry;
    MACRO_ARG       *macro_parms;
    unsigned        rescanning          : 1;
    unsigned        substituting_parms  : 1;
};

static carve_t      carveNESTED_MACRO;
static NESTED_MACRO *nestedMacros;
static MACRO_TOKEN  *scannerTokenList;
static MACRO_TOKEN  *internalTokenList;
static unsigned     macroDepth;

static MACRO_TOKEN  *macroExpansion( MEPTR, bool );
static MACRO_TOKEN  *nestedMacroExpansion( MEPTR, bool );

static struct special_macro_name {
    char            *name;
    special_macros  value;
    macro_flags     flags;
} SpcMacros[] = {
    #define pick( s, i, f )    { s, i, f },
    #include "specmac.h"
    #undef pick
};

static void macroDiagNesting(   // DIAGNOSE NESTING
    void )
{
    NESTED_MACRO* curr;         // - current entry

    for( curr = nestedMacros ; NULL != curr ; curr = curr->next ) {
        MEPTR mentry = curr->mentry;
        CErr( INF_MACRO_EXPANSION, mentry->macro_name, &mentry->defn );
    }
}


static void macroInit(          // MACRO PROCESSING -- INITIALIZATION
    INITFINI* defn )            // - definition
{
    int i;

    /* unused parameters */ (void)defn;

    DirectiveInit();
    nestedMacros = NULL;
    scannerTokenList = NULL;
    InitialMacroFlags = MFLAG_DEFINED_BEFORE_FIRST_INCLUDE;
    MacroStorageInit();
    for( i = MACRO_FIRST; i <= MACRO_LAST; ++i ) {
        MacroSpecialAdd( SpcMacros[i].name, SpcMacros[i].value, SpcMacros[i].flags );
    }
    TimeInit();     /* grab time and date for __TIME__ and __DATE__ */
    carveNESTED_MACRO = CarveCreate( sizeof( NESTED_MACRO ), 16 );
}


static void macroFini(          // MACRO PROCESSING -- COMPLETION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    DirectiveFini();
    MacroStorageFini();
    DbgStmt( CarveVerifyAllGone( carveNESTED_MACRO, "NESTED_MACRO" ) );
    CarveDestroy( carveNESTED_MACRO );
}


INITDEFN( macros, macroInit, macroFini )


static void deleteNestedMacro( void )
{
    MEPTR mentry;
    NESTED_MACRO *nested;
    MACRO_ARG *macro_parms;
    mac_parm_count parmno;

    nested = nestedMacros;
    if( nested != NULL ) {
        nestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        mentry = nested->mentry;
        CarveFree( carveNESTED_MACRO, nested );
        if( macro_parms != NULL ) {
            for( parmno = 0; parmno < GetMacroParmCount( mentry ); ++parmno ) {
                CMemFree( macro_parms[parmno].arg );
            }
            CMemFree( macro_parms );
        }
    }
}

static TOKEN doGetMacroToken(       // GET NEXT TOKEN
    bool internal,                  // - list of tokens
    bool doing_macro_expansion )    // - true ==> doing an expansion
{
    char        *token_end;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN **mlist;
    bool        keep_token;
    TOKEN       token;

    if( internal ) {
        mlist = &internalTokenList;
    } else {
        mlist = &scannerTokenList;
    }
    Buffer[0] = '\0';
    TokenLen = 0;
    token = T_NULL;
    for( ;; ) {
        if( (mtok = *mlist) == NULL ) {
            CompFlags.use_macro_tokens = false;
            break;
        }
        if( (token = mtok->token) != T_NULL ) {
            keep_token = false;
            WriteBufferStr( mtok->data );
            switch( token ) {
            case T_SAVED_ID:
                if( doing_macro_expansion ) {
                    token = T_ID;
                }
                /* fall through */
            case T_ID:
            case T_UNEXPANDABLE_ID:
                if( !doing_macro_expansion ) {
                    if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
                        *mlist = mtok->next;
                        CMemFree( mtok );
                        token = Process_Pragma( internal );
                        mtok = *mlist;
                        keep_token = true;
                    } else {
                        token = KwLookup( TokenLen );
                    }
                }
                break;
            case T_BAD_TOKEN:
            case T_CONSTANT:
                ReScanInit( Buffer );
                token = ReScanToken();
                DbgAssert( mtok->data[TokenLen] == '\0' || token == T_BAD_TOKEN );
                break;
            case T_PPNUMBER:
                ReScanInit( Buffer );
                token = ReScanToken();
                DbgAssert( token != T_STRING && token != T_LSTRING );
                token_end = &(mtok->data[TokenLen]);
                if( *token_end != '\0' ) {
                    // ppnumber is quite general so it may absorb multiple tokens
                    strcpy( mtok->data, token_end );
                    keep_token = true;
                }
                break;
            case T_STRING:
            case T_LSTRING:
                break;
            }
            if( !keep_token ) {
                *mlist = mtok->next;
                CMemFree( mtok );
            }
            break;
        }
        if( mtok->data[0] == MACRO_END_CHAR ) {     // if end of macro
            deleteNestedMacro();
        }
        *mlist = mtok->next;
        CMemFree( mtok );
    }
#ifndef NDEBUG
    DumpMacToken();
#endif
    return( token );
}

TOKEN GetMacroToken(                // GET NEXT TOKEN
    bool doing_macro_expansion )    // - true ==> doing an expansion
{
    return( doGetMacroToken( false, doing_macro_expansion ) );
}

static size_t expandMacroToken( size_t i, MACRO_TOKEN *m )
{
    switch( m->token ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:
        i = WriteBufferPosStr( i, m->data );
        break;
    case T_LSTRING:
        i = WriteBufferPosChar( i, 'L' );
        /* fall through */
    case T_STRING:
        i = WriteBufferPosChar( i, '"' );
        i = WriteBufferPosStr( i, m->data );
        i = WriteBufferPosChar( i, '"' );
        WriteBufferPosNullChar( i );
        break;
    default:
        i = WriteBufferPosStr( i, Tokens[m->token] );
    }
    return( i );
}

static size_t genFUNCTION(
    void )
{
    SYMBOL  sym;
    size_t  len;
    VBUF    buff;

    sym = ParseCurrFunction();
    if( sym != NULL ) {
        FormatSymWithTypedefs( sym, &buff );
        len = WriteBufferPosStr( 0, VbufString( &buff ) );
        VbufFree( &buff );
    } else {
        Buffer[0] = '?';
        Buffer[1] = '\0';
        len = 1;
    }
    return( len );
}

TOKEN SpecialMacro(             // EXECUTE A SPECIAL MACRO
    MEPTR mentry )              // - macro entry
{
    const char *p;

    switch( mentry->parm_count ) {
    case MACRO_LINE:
        TokenLen = sprintf( Buffer, "%u", TokenLine );
        U32ToU64( TokenLine, &Constant64 );
        ConstType = TYP_SINT;
        return( T_CONSTANT );
    case MACRO_FILE:
        p = SrcFileNameCurrent();
        TokenLen = WriteBufferPosEscStr( 0, &p, false );
        return( T_STRING );
    case MACRO_DATE:
        strcpy( Buffer, __Date );
        TokenLen = strlen( Buffer );
        return( T_STRING );
    case MACRO_TIME:
        strcpy( Buffer, __Time );
        TokenLen = strlen( Buffer );
        return( T_STRING );
    case MACRO_FUNCTION:
    case MACRO_FUNC:
        if( FUNCTION_not_expandable ) {
            TokenLen = WriteBufferPosStr( 0, SpcMacros[mentry->parm_count].name );
            return( T_ID );
        }
        TokenLen = genFUNCTION();
        return( T_STRING );
    case MACRO_CPLUSPLUS:
        Buffer[0] = '1';
        Buffer[1] = '\0';
        TokenLen = 1;
        U32ToU64( 1, &Constant64 );
        ConstType = TYP_SINT;
        return( T_CONSTANT );
    case MACRO_ALT_AND:
        return( T_ALT_AND_AND );
    case MACRO_ALT_BITAND:
        return( T_ALT_AND );
    case MACRO_ALT_AND_EQ:
        return( T_ALT_AND_EQUAL );
    case MACRO_ALT_OR:
        return( T_ALT_OR_OR );
    case MACRO_ALT_BITOR:
        return( T_ALT_OR );
    case MACRO_ALT_OR_EQ:
        return( T_ALT_OR_EQUAL );
    case MACRO_ALT_XOR:
        return( T_ALT_XOR );
    case MACRO_ALT_XOR_EQ:
        return( T_ALT_XOR_EQUAL );
    case MACRO_ALT_NOT:
        return( T_ALT_EXCLAMATION );
    case MACRO_ALT_NOT_EQ:
        return( T_ALT_NE );
    case MACRO_ALT_COMPL:
        return( T_ALT_TILDE );
    }
    return( T_EOF );
}


static TOKEN nextMToken( TOKEN prev_token )
{
    ppctl_t old_ppctl;
    TOKEN token;

    token = doGetMacroToken( true, true );
    if( token == T_NULL ) {
        if( DoScanOptionalComment() ) {
            token = T_WHITE_SPACE;
        } else {
            if( prev_token != T_WHITE_SPACE ) {
                // at EOL, ChkControl skips white-space for you which
                // is not appropriate at this point (this white-space
                // may be significant)
                // [the prev_token check is req'd because the scanner
                //  will not advance past T_NULL]
                old_ppctl = PPControl;
                PPCTL_ENABLE_EOL();
                token = ScanToken( true );
                PPControl = old_ppctl;
                if( token == T_NULL ) {
                    token = T_WHITE_SPACE;
                }
            } else {
                token = ScanToken( true );
            }
        }
    }
    return( token );
}

static void saveParm(
    MEPTR               mentry,
    mac_parm_count      parmno,
    MACRO_ARG           *macro_parms,
    TOKEN_LIST          *token_list,
    int                 total,
    BUFFER_HDR          **h )
{
    TOKEN_LIST *last_token;
    char *p;

    *h = TokenBufAddToken( *h, T_NULL );
    if( parmno < GetMacroParmCount( mentry ) ) {
        p = CMemAlloc( total + TokenBufTotalSize( *h ) + 1 );
        macro_parms[parmno].arg = p;
        if( token_list != NULL ) {
            last_token = token_list;
            do {
                token_list = token_list->next;
                p = stxvcpy( p, token_list->buf, token_list->length );
            } while( token_list != last_token );
            RingFree( &token_list );
        }
        *h = TokenBufMove( *h, p );
    }
}

static MACRO_ARG *collectParms( MEPTR mentry )
{
    int             bracket;
    TOKEN           token;
    TOKEN           prev_token;
    mac_parm_count  parm_count_reqd;
    mac_parm_count  parmno;
    int             total;
    bool            ppscan_mode;
    MACRO_ARG       *macro_parms;
    TOKEN_LIST      *token_head;
    BUFFER_HDR      *htokenbuf;

    macro_parms = NULL;
    if( MacroWithParenthesis( mentry ) ) { /* if () expected */
        // () = 1, (a) = 2, (a,b) = 3
        parm_count_reqd = GetMacroParmCount( mentry );
        ppscan_mode = InitPPScan();             // enable T_PPNUMBER tokens
        htokenbuf = TokenBufInit( NULL );
        if( parm_count_reqd > 0 ) {
            macro_parms = CMemAlloc( parm_count_reqd * sizeof( *macro_parms ) );
            if( MacroHasVarArgs( mentry ) ) {
                macro_parms[parm_count_reqd - 1].arg = NULL;
            }
        }
        parmno = 0;
        token = T_NULL;
        do {
            token = nextMToken( token );
        } while( token == T_WHITE_SPACE );
        /* token will now be a '(' */
        bracket = 0;
        token_head = NULL;
        total = 0;
        for( ;; ) {
            prev_token = token;
            do {
                token = nextMToken( token );
                if( token != T_WHITE_SPACE ) {
                    break;
                }
            } while( TokenBufSize( htokenbuf ) == 0 );
            if( token == T_EOF || token == T_NULL ) {
                CErr( ERR_INCOMPLETE_MACRO, mentry->macro_name );
                InfMacroDecl( mentry );
                macroDiagNesting();
                break;
            }
            if( token == T_BAD_TOKEN && BadTokenInfo == ERR_MISSING_QUOTE ) {
                CErr1( ERR_MISSING_QUOTE );
                InfMacroDecl( mentry );
                macroDiagNesting();
                token = T_RIGHT_PAREN;
            }
            if( token == T_LEFT_PAREN ) {
                ++bracket;
            } else if( token == T_RIGHT_PAREN ) {
                if( bracket == 0 )
                    break;
                --bracket;
            } else if( token == T_COMMA && bracket == 0 &&
                  !( MacroHasVarArgs( mentry ) && parmno == ( parm_count_reqd - 1 ) ) ) {
                TokenBufRemoveWhiteSpace( htokenbuf );
                if( macro_parms != NULL ) {     // if expecting parms
                    saveParm( mentry, parmno, macro_parms, token_head, total, &htokenbuf );
                }
                ++parmno;
                token_head = NULL;
                total = 0;
                continue;
            }
            switch( token ) {
            case T_WHITE_SPACE:
                if( prev_token != T_WHITE_SPACE ) {
                    htokenbuf = TokenBufAddToken( htokenbuf, token );
                }
                break;
            case T_BAD_CHAR:
                htokenbuf = TokenBufAddToken( htokenbuf, token );
                htokenbuf = TokenBufAddChar( htokenbuf, Buffer[0] );
                if( Buffer[1] != '\0' ) {
                    htokenbuf = TokenBufAddToken( htokenbuf, T_WHITE_SPACE );
                }
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_STRING:
            case T_LSTRING:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:
                htokenbuf = TokenBufAddToken( htokenbuf, token );
                htokenbuf = TokenBufAddStr( htokenbuf, Buffer );
                break;
            default :
                htokenbuf = TokenBufAddToken( htokenbuf, token );
                break;
            }
        }
        TokenBufRemoveWhiteSpace( htokenbuf );
        if( macro_parms != NULL ) {     // if expecting parms
            saveParm( mentry, parmno, macro_parms, token_head, total, &htokenbuf );
            ++parmno;
        } else if( TokenBufSize( htokenbuf ) + total != 0 ) {
            ++parmno;
        }
        if( ( MacroHasVarArgs( mentry ) && ( parmno < parm_count_reqd - 1 ) )
            || ( !MacroHasVarArgs( mentry ) && ( parmno < parm_count_reqd ) ) ) {
            CErr( ERR_TOO_FEW_MACRO_PARMS, mentry->macro_name );
            InfMacroDecl( mentry );
            macroDiagNesting();
            do {
                htokenbuf = TokenBufAddToken( htokenbuf, T_WHITE_SPACE );
                saveParm( mentry, parmno, macro_parms, NULL, 1, &htokenbuf );
                ++parmno;
            } while( parmno < parm_count_reqd );
        } else if( !MacroHasVarArgs( mentry ) && ( parmno > parm_count_reqd ) ) {
            CErr( ANSI_TOO_MANY_MACRO_PARMS, mentry->macro_name );
            InfMacroDecl( mentry );
            macroDiagNesting();
        } else if( strcmp( mentry->macro_name, "va_start" ) == 0 ) {
            if( ScopeFunctionInProgress() != NULL ) {
                if( ! CurrFunctionHasEllipsis() ) {
                    CErr1( ERR_MUST_BE_VAR_PARM_FUNC );
                    macroDiagNesting();
                }
            }
        }
        FiniPPScan( ppscan_mode );      // disable T_PPNUMBER tokens
        TokenBufFini( htokenbuf );
    }
    return( macro_parms );
}


#ifndef NDEBUG
#if 0
void DumpMTokens( MACRO_TOKEN *mtok )
{
    puts( "---" );
    for( ; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token == T_MACRO_PARM ) {
            printf( "%p: <macro-parm>\n", mtok );
        } else {
            printf( "%p: '%s'\n", mtok, mtok->data );
        }
    }
    puts( "---" );
}
#endif
void DumpNestedMacros( void )
{
    NESTED_MACRO *nested;

    puts( "---" );
    for( nested = nestedMacros; nested != NULL; nested = nested->next ) {
        printf( "%p: %s\n", nested->mentry, nested->mentry->macro_name );
    }
    puts( "---" );
}
#endif


static MACRO_TOKEN *buildAToken( TOKEN token, const char *p )
{
    size_t      nbytes;
    MACRO_TOKEN *mtok;

    nbytes = strlen( p ) + 1;
    mtok = CMemAlloc( offsetof( MACRO_TOKEN, data ) + nbytes );
    mtok->next = NULL;
    mtok->token = token;
    memcpy( mtok->data, p, nbytes );
    return( mtok );
}

static MACRO_TOKEN **buildTokenOnEnd( MACRO_TOKEN **ptail, TOKEN token, const char *str )
{
    MACRO_TOKEN *mtok;

    mtok = buildAToken( token, str );
    mtok->next = *ptail;
    *ptail = mtok;
    return( &(mtok->next) );
}

static void buildTokenAfter( MACRO_TOKEN *ptail, TOKEN token, const char *str )
{
    MACRO_TOKEN *mtok;

    mtok = buildAToken( token, str );
    mtok->next = ptail->next;
    ptail->next = mtok;
}

static MACRO_TOKEN *appendToken( MACRO_TOKEN *head, TOKEN token, const char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new_tok;

    new_tok = buildAToken( token, data );
    if( head == NULL ) {
        head = new_tok;
    } else {
        tail = head;
        while( tail->next != NULL )
            tail = tail->next;
        tail->next = new_tok;
    }
    return( head );
}

static bool macroBeingExpanded( MEPTR mentry )
{
    NESTED_MACRO *nested;

    for( nested = nestedMacros; nested != NULL; nested = nested->next ) {
        if( nested->mentry == mentry )
            return( true );
        if( ! nested->rescanning ) {
            break;
        }
    }
    return( false );
}

static exp_state isExpandable( MEPTR mentry, MACRO_TOKEN *mtok, bool macro_parm )
{
    int lparen;

    if( MacroIsSpecial( mentry ) ) {  /* if special macro */
        if( ( mentry->parm_count == MACRO_FUNCTION ) || ( mentry->parm_count == MACRO_FUNC ) ) {
            if( FUNCTION_not_expandable ) {
                return( EXPANDABLE_NO );
            }
        }
        return( EXPANDABLE_YES );
    }
    if( !MacroWithParenthesis( mentry ) ) { /* if () not expected */
        if( macro_parm ) {
            if( macroBeingExpanded( mentry ) ) {
                return( EXPANDABLE_NO );
            }
        }
        return( EXPANDABLE_YES );
    }
    for( ; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token != T_WHITE_SPACE && mtok->token != T_NULL ) {
            break;
        }
    }
    if( mtok != NULL ) {
        if( mtok->token == T_LEFT_PAREN ) {
            if( macroDepth == 1 && !macro_parm )
                return( EXPANDABLE_YES );
            lparen = 0;
            for( ; (mtok = mtok->next) != NULL; ) {
                if( mtok->token == T_LEFT_PAREN ) {
                    ++lparen;
                } else if( mtok->token == T_RIGHT_PAREN ) {
                    if( lparen == 0 )
                        return( EXPANDABLE_YES );
                    --lparen;
                }
            }
        }
    } else if( !macro_parm ) {
        SkipAhead();
        if( CurrChar == '(' ) {
            return( EXPANDABLE_YES );
        } else if( CompFlags.cpp_output ) {
            return( EXPANDABLE_WSSKIP );
        }
    }
    return( EXPANDABLE_NO );
}

static MEPTR isAMacro( MACRO_TOKEN *mtok )
{
    const char  *s;
    size_t      len;
    MEPTR       mentry;

    DbgStmt( ( Buffer[0] = '?' , Buffer[1] = '\0' ) );
    s = mtok->data;
    len = strlen( s );
    mentry = MacroLookup( s, len );
    return( mentry );
}

static MACRO_TOKEN *expandNestedMacros( MACRO_TOKEN *head, bool rescanning )
{
    MEPTR mentry;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *old_tokenlist;
    MACRO_TOKEN **ptail;
    exp_state i;

    ptail = &head;
    ++macroDepth;
#ifndef NDEBUG
    if( macroDepth > 100 ) {
  #if defined( __WATCOMC__ )
        __trap();
  #endif
    }
#endif
    for( mtok = head; mtok != NULL; ) {
        toklist = NULL;
        if( mtok->token == T_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            mentry = isAMacro( mtok );
            if( mentry != NULL ) {
                if( rescanning ) {
                    if( macroBeingExpanded( mentry ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL )
                            toklist = toklist->next;
                        toklist->next = internalTokenList;
                        i = isExpandable( mentry, mtok->next, false );
                        switch( i ) {
                        case EXPANDABLE_NO:         // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case EXPANDABLE_YES:        // macro is expandable
                            internalTokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                ptail = &head;
                            }
                            CMemFree( mtok );
                            toklist = macroExpansion( mentry, rescanning );
                            mtok = internalTokenList;
                            internalTokenList = NULL;
                            break;
                        case EXPANDABLE_WSSKIP:     // we skipped over some white space
                            mtok->token = T_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            buildTokenAfter( mtok, T_WHITE_SPACE, " " );
                            toklist = NULL;
                            break;
                        }
                    }
                } else {        // expanding a macro parm
                    i = isExpandable( mentry, mtok->next, true );
                    switch( i ) {
                    case EXPANDABLE_NO:         // macro is currently not expandable
                        ptail = &(mtok->next);
                        mtok = mtok->next;      // advance onto next token
                        break;
                    case EXPANDABLE_YES:        // macro is expandable
                    case EXPANDABLE_WSSKIP:     // we skipped over some white space
                        old_tokenlist = internalTokenList;
                        internalTokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            ptail = &head;
                        }
                        CMemFree( mtok );
                        toklist = nestedMacroExpansion( mentry, rescanning );
                        mtok = internalTokenList;
                        internalTokenList = old_tokenlist;
                        break;
                    }
                }
            } else {
                mtok->token = T_SAVED_ID;       // avoid rechecking this ID
                ptail = &(mtok->next);
                mtok = mtok->next;      // advance onto next token
            }
        } else if( mtok->token == T_NULL ) {
            toklist = mtok->next;
            if( mtok->data[0] == MACRO_END_CHAR ) {        // end of a macro
                rescanning = nestedMacros->rescanning;
                deleteNestedMacro();
                CMemFree( mtok );
                mtok = toklist;
            } else {                            // end of a macro parm
                if( toklist != NULL ) {
                    internalTokenList = toklist;
                }
                CMemFreePtr( &mtok );           // will cause us to exit
            }
            toklist = NULL;
        } else {                        // advance onto next token
            ptail = &(mtok->next);
            mtok = mtok->next;
        }
        if( toklist != NULL ) {         // new tokens to insert
            *ptail = toklist;
            if( mtok != NULL ) {
                while( toklist->next != NULL )
                    toklist = toklist->next;
                toklist->next = mtok;
            }
            mtok = *ptail;
        } else {
            // either no change, or tokens were deleted
            *ptail = mtok;
        }
    }
    for( mtok = head; mtok != NULL; mtok = mtok->next ) {
        // change a temporarily unexpandable ID into an ID because it
        // could become expandable in a later rescanning phase
        if( mtok->token == T_MACRO ) {
            mtok->token = T_ID;
        }
    }
    --macroDepth;
    return( head );
}

static MACRO_TOKEN *reTokenBuffer( const char *buffer )
{
    MACRO_TOKEN *head;
    MACRO_TOKEN **ptail;
    bool        ppscan_mode;

    ppscan_mode = InitPPScan();
    ReScanInit( buffer );
    head = NULL;
    ptail = &head;
    for( CompFlags.rescan_buffer_done = false; !CompFlags.rescan_buffer_done; ) {
        ptail = buildTokenOnEnd( ptail, ReScanToken(), Buffer );
    }
    FiniPPScan( ppscan_mode );
    return( head );
}

static MACRO_TOKEN *glue2Tokens( MACRO_TOKEN *first, MACRO_TOKEN *second )
{
    size_t      i;

    #define INPOUT_SHIFT    10

    i = INPOUT_SHIFT;
    Buffer[i] = '\0';
    if( first != NULL ) {
        i = expandMacroToken( i, first );
    }
    if( second != NULL ) {
        i = expandMacroToken( i, second );
    }
    return( reTokenBuffer( Buffer + INPOUT_SHIFT ) );
}

static MACRO_TOKEN *glueTokens( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *next;
    MACRO_TOKEN *new_mtok;
    MACRO_TOKEN **ptail;
    MACRO_TOKEN **prev_ptail;

    prev_ptail = NULL;
    ptail = &head;
    for( mtok = head; mtok != NULL; ) {
        if( mtok->token != T_WHITE_SPACE ) {
            next = mtok->next;
            if( next == NULL )
                break;
            if( next->token == T_WHITE_SPACE )
                next = next->next;
            if( next == NULL )
                break;
            if( next->token == T_MACRO_SHARP_SHARP ) {
                next = next->next;
                if( next == NULL )
                    break;

                if( next->token == T_WHITE_SPACE )
                    next = next->next;

                if( next == NULL )
                    break;

                if( ( next->token == T_MACRO_EMPTY_VAR_PARM ) || ( mtok->token == T_MACRO_EMPTY_VAR_PARM ) ) {
                    MACRO_TOKEN *rem;

                    if( next->token != T_COMMA && mtok->token == T_MACRO_EMPTY_VAR_PARM ) {
                        //  EMPTY##
                        //keep the next token...
                        rem = next;
                        while( mtok->next != rem ) { //free old stuff [mtoken,##,{atok,} next]
                            next = mtok->next;
                            CMemFree( mtok );
                            mtok = next;
                        }
                    } else {
                        if( next->token == T_COMMA || mtok->token == T_COMMA ) {
                            // EMPTY##, or ,##EMPTY
                            // delete whole expression
                            rem = next->next;  // save unseen stuff
                            next->next = NULL; // break link;
                        } else {
                            // ##EMPTY
                            // keep the thing after the ##
                            rem = next->next;
                            next->next = NULL;
                            prev_ptail = ptail;
                            ptail = &(mtok->next);
                            mtok = mtok->next; // skip first...
                        }
                        while( mtok != NULL ) { //free old stuff [mtoken,##,{atok,} next]
                            next = mtok->next;
                            CMemFree( mtok );
                            mtok = next;
                        }
                    }
                    *ptail = rem;
                    if( prev_ptail != NULL ) {
                        // skip back one param..
                        ptail = prev_ptail;
                        mtok = *ptail;
                    } else {
                        *ptail = rem;
                        mtok = head;
                    }
                } else {
                    MACRO_TOKEN *last;

                    // glue mtok->token with next->token to make one token
                    // create new token
                    *ptail = last = new_mtok = glue2Tokens( mtok, next );
                    while( last->next != NULL ) {
                        last = last->next;
                    }
                    if( next != NULL ) {
                        last->next = next->next;
                    }
                    do {
                        next = mtok->next;
                        CMemFree( mtok );
                        mtok = next;
                    } while( mtok != last->next );
                    if( prev_ptail == NULL )
                        head = new_mtok;
                    mtok = new_mtok;
                }
                continue;       /* to catch consecutive ##'s */
            }
        }
        prev_ptail = ptail;
        ptail = &(mtok->next);
        mtok = mtok->next;
    }
    return( head );
}

static MACRO_TOKEN **buildString( MACRO_TOKEN **ptail, const char *p )
{
    size_t      i;
    size_t      last_non_ws;
    TOKEN       tok;

    i = 0;
    last_non_ws = 0;
    // skip leading whitespace
    while( *(TOKEN *)p == T_WHITE_SPACE ) {
        p += sizeof( TOKEN );
    }
    while( (tok = *(TOKEN *)p) != T_NULL ) {
        p += sizeof( TOKEN );
        switch( tok ) {
        case T_WHITE_SPACE:
            while( *(TOKEN *)p == T_WHITE_SPACE ) {
                p += sizeof( TOKEN );
            }
            i = WriteBufferPosChar( i, ' ' );
            break;
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
            i = WriteBufferPosEscStr( i, &p, false );
            last_non_ws = i;
            break;
        case T_LSTRING:
            i = WriteBufferPosChar( i, 'L' );
            /* fall through */
        case T_STRING:
            i = WriteBufferPosChar( i, '\\' );
            i = WriteBufferPosChar( i, '"' );
            i = WriteBufferPosEscStr( i, &p, true );
            i = WriteBufferPosChar( i, '\\' );
            i = WriteBufferPosChar( i, '"' );
            last_non_ws = i;
            break;
        case T_BAD_CHAR:
            i = WriteBufferPosChar( i, *p++ );
            last_non_ws = i;
            break;
        default:
            i = WriteBufferPosStr( i, Tokens[tok] );
            last_non_ws = i;
            break;
        }
    }
    WriteBufferPosNullChar( last_non_ws );
    ptail = buildTokenOnEnd( ptail, T_STRING, Buffer );
    return( ptail );
}

static MACRO_TOKEN **buildMTokenList( MACRO_TOKEN **ptail, const char *p, MACRO_ARG *macro_parms )
{
    TOKEN prev_token;
    TOKEN curr_token;
    char buf[2];

    buf[1] = '\0';
    prev_token = T_NULL;
    for( ; (curr_token = *(TOKEN *)p) != T_NULL; ) {
        p += sizeof( TOKEN );
        switch( curr_token ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
        case T_LSTRING:
        case T_STRING:
            ptail = buildTokenOnEnd( ptail, curr_token, p );
            while( *p++ != '\0' )
                {;}
            prev_token = curr_token;
            break;
        case T_WHITE_SPACE:
            if( prev_token != T_MACRO_SHARP_SHARP ) {
                buf[0] = ' ';
                ptail = buildTokenOnEnd( ptail, curr_token, buf );
            }
            break;
        case T_BAD_CHAR:
        case T_MACRO_PARM:
        case T_MACRO_VAR_PARM:
            buf[0] = *p++;
            ptail = buildTokenOnEnd( ptail, curr_token, buf );
            prev_token = curr_token;
            break;
        case T_MACRO_SHARP:
            while( *(TOKEN *)p == T_WHITE_SPACE )
                p += sizeof( TOKEN );
            if( *(TOKEN *)p != T_MACRO_PARM && *(TOKEN *)p != T_MACRO_VAR_PARM ) {
                // we had an error before; handle as T_BAD_CHAR
                buf[0] = '#';
                ptail = buildTokenOnEnd( ptail, T_BAD_CHAR, buf );
                prev_token = T_BAD_CHAR;
                break;
            }
            p += sizeof( TOKEN );               // skip over T_MACRO_PARM
            if( macro_parms != NULL && macro_parms[(unsigned char)*p].arg != NULL ) {
                ptail = buildString( ptail, macro_parms[(unsigned char)*p].arg );
            } else {
                buf[0] = '\0';
                ptail = buildTokenOnEnd( ptail, T_STRING, buf );
            }
            ++p;
            prev_token = T_STRING;
            break;
        default:
            ptail = buildTokenOnEnd( ptail, curr_token, Tokens[curr_token] );
            prev_token = curr_token;
            break;
        }
    }
    return( ptail );
}

static bool SharpSharp( MACRO_TOKEN *mtok )
{
    for( ; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token == T_MACRO_SHARP_SHARP )
            return( true );
        if( mtok->token != T_WHITE_SPACE ) {
            break;
        }
    }
    return( false );
}

static MACRO_TOKEN *substituteParms( MACRO_TOKEN *head, MACRO_ARG *macro_parms )
{
    TOKEN       prev_non_ws;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *list;
    MACRO_TOKEN *dummy_list;
    MACRO_TOKEN **ptail;
    char        empty_var[sizeof( TOKEN ) + 1 + sizeof( TOKEN )];
    const char  *p;

    ptail = &head;
    prev_non_ws = T_NULL;
    for( mtok = head; mtok != NULL; mtok = mtok->next ) {
        list = NULL;
        if( mtok->token == T_MACRO_PARM || mtok->token == T_MACRO_VAR_PARM ) {
            // replace this ID with a copy of the tokens from
            // macro_parms[mtok->data[0]].arg
            dummy_list = NULL;
            p = macro_parms[(unsigned char)mtok->data[0]].arg;
            if( mtok->token == T_MACRO_VAR_PARM || p != NULL ) {
                if( p == NULL ) {
                    *(TOKEN *)empty_var = T_MACRO_EMPTY_VAR_PARM;
                    empty_var[sizeof( TOKEN )] = 0;
                    *(TOKEN *)( empty_var + sizeof( TOKEN ) + 1 ) = T_NULL;
                    p = empty_var;
                }
                buildMTokenList( &dummy_list, p, NULL );
            }
            list = dummy_list;
            if( prev_non_ws != T_MACRO_SHARP_SHARP && !SharpSharp( mtok->next ) ) {
                list = expandNestedMacros( list, false );
            }
            if( list == NULL ) {
                list = mtok;
                mtok = mtok->next;
                CMemFreePtr( &list );
                *ptail = mtok;
            }
        }
        if( list != NULL ) {
            *ptail = list;
            while( list->next != NULL ) {
                list = list->next;
            }
            list->next = mtok->next;
            CMemFree( mtok );
            mtok = list;
        }
        if( mtok == NULL )
            break;
        ptail = &(mtok->next);
        if( mtok->token != T_WHITE_SPACE ) {
            prev_non_ws = mtok->token;
        }
    }
    return( head );
}

static void markUnexpandableIds( MACRO_TOKEN *head )
{
    NESTED_MACRO    *nested;
    MACRO_TOKEN     *mtok;

    for( mtok = head; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token == T_ID ) {
            for( nested = nestedMacros; nested != NULL; nested = nested->next ) {
                if( strcmp( nested->mentry->macro_name, mtok->data ) == 0 ) {
                    if( !nested->substituting_parms ) {
                        // change token so it won't be considered a
                        // candidate as a macro
                        mtok->token = T_UNEXPANDABLE_ID;
                        break;
                    }
                }
            }
        }
    }
}

static MACRO_TOKEN *macroExpansion( MEPTR mentry, bool rescanning )
{
    MACRO_ARG       *macro_parms;
    MACRO_TOKEN     *head;
    MACRO_TOKEN     **ptail;
    NESTED_MACRO    *nested;
    const char      *tokens;

    nested = CarveAlloc( carveNESTED_MACRO );
    nested->mentry = mentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = false;
    nested->macro_parms = NULL;
    head = NULL;
    ptail = &head;
    if( MacroIsSpecial( mentry ) ) {    /* if special macro */
        ptail = buildTokenOnEnd( ptail, SpecialMacro( mentry ), Buffer );
        nested->next = nestedMacros;
        nestedMacros = nested;
    } else {
        macro_parms = collectParms( mentry );
        nested->next = nestedMacros;
        nestedMacros = nested;
        nested->macro_parms = macro_parms;
        tokens = (char *)mentry + mentry->macro_defn;
        ptail = buildMTokenList( ptail, tokens, macro_parms );
        if( macro_parms != NULL ) {
            nested->substituting_parms = true;
            head = substituteParms( head, macro_parms );
            nested->substituting_parms = false;
        }
        head = glueTokens( head );
        markUnexpandableIds( head );
    }
    head = appendToken( head, T_NULL, MACRO_END_STRING );
    return( head );
}

static MACRO_TOKEN *nestedMacroExpansion( MEPTR mentry, bool rescanning )
{
    return( expandNestedMacros( macroExpansion( mentry, false ), rescanning ) );
}

void DoMacroExpansion(          // EXPAND A MACRO
    MEPTR mentry )             // - macro to expand
{
    DbgAssert( scannerTokenList == NULL );
    macroDepth = 0;
    scannerTokenList = nestedMacroExpansion( mentry, true );
    // GetMacroToken will feed back tokens from the tokenList
    // when the tokenList is exhausted, then revert back to normal scanning
    CompFlags.use_macro_tokens = ( scannerTokenList != NULL );
}

void DefineAlternativeTokens(   // DEFINE ALTERNATIVE TOKENS
    void )
{
    int i;

    for( i = MACRO_ALT_FIRST; i <= MACRO_ALT_LAST; ++i ) {
        MacroSpecialAdd( SpcMacros[i].name, SpcMacros[i].value, SpcMacros[i].flags );
    }
}


void InsertReScanPragmaTokens( const char *pragma, bool internal )
{
    MACRO_TOKEN *toklist;

    toklist = reTokenBuffer( pragma );
    if( toklist != NULL ) {
        MACRO_TOKEN *old_list;
        if( internal ) {
            old_list = internalTokenList;
            internalTokenList = toklist;
        } else {
            old_list = scannerTokenList;
            scannerTokenList = toklist;
        }
        while( toklist->next != NULL ) {
            toklist = toklist->next;
        }
        toklist->next = buildAToken( T_PRAGMA_END, "" );
        toklist = toklist->next;
        toklist->next = old_list;
        CompFlags.use_macro_tokens = true;
    }
}

void InsertToken( TOKEN token, const char *str, bool internal )
{
    MACRO_TOKEN *toklist;

    toklist = buildAToken( token, str );
    if( toklist != NULL ) {
        if( internal ) {
            toklist->next = internalTokenList;
            internalTokenList = toklist;
        } else {
            toklist->next = scannerTokenList;
            scannerTokenList = toklist;
        }
        CompFlags.use_macro_tokens = true;
    }
}
