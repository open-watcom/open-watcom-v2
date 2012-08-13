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
* Description:  Macro processing, part one.
*
****************************************************************************/


// FIXME: This code is seriously broken. It assumes that the TOKEN enum is
// interchangeable with char. This assumption may break if plain char is
// signed and will break if the TOKEN enum is not stored as 8-bit quantity.

#include "plusplus.h"
#include "preproc.h"
#include "tokenbuf.h"
#include "errdefns.h"
#include "memmgr.h"
#include "macro.h"
#include "ring.h"
#include "dbg.h"
#include "fmtsym.h"
#include "initdefs.h"
#include "yydriver.h"
#include "carve.h"

#define T_UNEXPANDABLE_ID       T_LAST_TOKEN

typedef struct _tokens TOKEN_LIST;
struct _tokens {
    TOKEN_LIST  *next;
    unsigned    length;
    char        buf[1];
};

typedef struct macro_token MACRO_TOKEN;
struct macro_token {
    MACRO_TOKEN *next;
    uint_8      token;
    char        data[1];
};

typedef struct macro_arg {
    char        *arg;
} MACRO_ARG;

typedef struct nested_macros NESTED_MACRO;
struct nested_macros {
    NESTED_MACRO    *next;
    MEPTR           fmentry;
    MACRO_ARG       *macro_parms;
    unsigned        rescanning : 1;
    unsigned        substituting_parms : 1;
};

static carve_t carveNESTED_MACRO;
static NESTED_MACRO *nestedMacros;
static MACRO_TOKEN *scannerTokenList;
static MACRO_TOKEN *internalTokenList;
static unsigned macroDepth;

static MACRO_TOKEN  *macroExpansion( MEPTR, int );
static MACRO_TOKEN  *nestedMacroExpansion( MEPTR, boolean );

typedef struct special_macro_name SPECIAL_MACRO_NAME;
static struct special_macro_name {
    char        *name;
    int         value;
    macro_flags flags;
} SpcMacros[] = {
    #define pick( s, i, f )    { s, i, f },
    #include "specmac.h"
    #undef pick
    { NULL, 0, MFLAG_NONE }
};


static void macroDiagNesting(   // DIAGNOSE NESTING
    void )
{
    NESTED_MACRO* curr;         // - current entry

    for( curr = nestedMacros ; NULL != curr ; curr = curr->next ) {
        MEPTR mdef = curr->fmentry;
        CErr( INF_MACRO_EXPANSION, mdef->macro_name, &mdef->defn );
    }
}


static void macroInit(          // MACRO PROCESSING -- INITIALIZATION
    INITFINI* defn )            // - definition
{
    SPECIAL_MACRO_NAME *mac;

    defn = defn;
    DirectiveInit();
    nestedMacros = NULL;
    scannerTokenList = NULL;
    InitialMacroFlag = MFLAG_DEFINED_BEFORE_FIRST_INCLUDE;
    MacroStorageInit();
    for( mac = SpcMacros; mac->name != NULL; ++mac ) {
        MacroSpecialAdd( mac->name, mac->value, mac->flags );
    }
    TimeInit();     /* grab time and date for __TIME__ and __DATE__ */
    carveNESTED_MACRO = CarveCreate( sizeof( NESTED_MACRO ), 16 );
}


static void macroFini(          // MACRO PROCESSING -- COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    DirectiveFini();
    MacroStorageFini();
#ifndef NDEBUG
    CarveVerifyAllGone( carveNESTED_MACRO, "NESTED_MACRO" );
#endif
    CarveDestroy( carveNESTED_MACRO );
}


INITDEFN( macros, macroInit, macroFini )


static void deleteNestedMacro( void )
{
    MEPTR fmentry;
    NESTED_MACRO *nested;
    MACRO_ARG *macro_parms;
    int i;

    nested = nestedMacros;
    if( nested != NULL ) {
        nestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        fmentry = nested->fmentry;
        CarveFree( carveNESTED_MACRO, nested );
        i = fmentry->parm_count - 1;
        if( i > 0  &&  macro_parms != NULL ) {
            do {
                --i;
                CMemFree( macro_parms[i].arg );
            } while( i != 0 );
            CMemFree( macro_parms );
        }
    }
}

static unsigned copyMTokToBuffer( MACRO_TOKEN *mtok )
{
    char *s;
    unsigned len;

    s = mtok->data;
    len = strlen( s );
    memcpy( Buffer, s, len + 1 );
    return( len );
}


static MACRO_TOKEN *doGetMacroToken(    // GET NEXT TOKEN
    MACRO_TOKEN *list,          // - list of tokens
    boolean doing_macro_expansion ) // - TRUE ==> doing an expansion
{
    char *token_end;
    MACRO_TOKEN *mtok;
    unsigned i;
    struct {
        unsigned keep_token : 1;
        unsigned next_token : 1;
    } flag;

    CurToken = T_NULL;
    for(;;) {
        flag.keep_token = FALSE;
        flag.next_token = FALSE;
        mtok = list;
        if( mtok == NULL ) {
            CompFlags.use_macro_tokens = FALSE;
            break;
        }
        CurToken = mtok->token;
        i = copyMTokToBuffer( mtok );
        switch( CurToken ) {
        case T_UNEXPANDABLE_ID:
            if( ! doing_macro_expansion ) {
                CurToken = KwLookup( i );
                TokenLen = i;
            }
            break;
        case T_ID:
        case T_SAVED_ID:
            if( doing_macro_expansion ) {
                CurToken = T_ID;
            } else {
                CurToken = KwLookup( i );
            }
            TokenLen = i;
            break;
        case T_BAD_TOKEN:
        case T_CONSTANT:
            ReScanInit( Buffer );
            ReScanToken();
            DbgAssert( mtok->data[ TokenLen ] == '\0' || CurToken == T_BAD_TOKEN );
            break;
        case T_PPNUMBER:
            ReScanInit( Buffer );
            ReScanToken();
            DbgAssert( CurToken != T_STRING && CurToken != T_LSTRING );
            token_end = &(mtok->data[ TokenLen ]);
            if( *token_end != '\0' ) {
                // ppnumber is quite general so it may absorb multiple tokens
                strcpy( mtok->data, token_end );
                flag.keep_token = TRUE;
            }
            break;
        case T_STRING:
        case T_LSTRING:
            TokenLen = i + 1;
            break;
        case T_BAD_CHAR:
            break;
        case T_NULL:
            if( Buffer[0] == 'Z' ) {    // if end of macro
                deleteNestedMacro();
            }
            flag.next_token = TRUE;
            break;
        }
        if( ! flag.keep_token ) {
            list = mtok->next;
            CMemFree( mtok );
        }
        if( ! flag.next_token ) {
            break;
        }
    }
    DumpMacToken();
    return( list );
}

void GetMacroToken(             // GET NEXT TOKEN
    boolean doing_macro_expansion ) // - TRUE ==> doing an expansion
{
    scannerTokenList = doGetMacroToken( scannerTokenList, doing_macro_expansion );
}

static unsigned copySafe( unsigned i, char *m )
{
    for(;;) {
        if( i >= (BUF_SIZE-4) ) {
            CErr1( ERR_TOKEN_TRUNCATED );
            macroDiagNesting();
            Buffer[i] = '\0';
            break;
        }
        if(( Buffer[i] = *m++ ) == 0 ) break;
        ++i;
        if(( Buffer[i] = *m++ ) == 0 ) break;
        ++i;
        if(( Buffer[i] = *m++ ) == 0 ) break;
        ++i;
        if(( Buffer[i] = *m++ ) == 0 ) break;
        ++i;
    }
    return( i );
}

static unsigned expandMacroToken( unsigned i, uint_8 *m )
{
    char *p;

    switch( *m ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:
        ++m;
        i = copySafe( i, (char *)m );
        break;
    case T_LSTRING:
        Buffer[i++] = 'L';
    case T_STRING:
        Buffer[i++] = '"';
        ++m;
        i = copySafe( i, (char *)m );
        Buffer[i++] = '"';
        Buffer[i] = '\0';
        break;
    default:
        p = Tokens[ *m ];
        ++m;
        i = copySafe( i, (char *)p );
    }
    return( i );
}


static int file_name_copy(       // COPY STRING, ESCAPING ANY BACKSLASHES
    char *dst,                  // - destination
    char const *src )           // - source
{
    char *org = dst;
    while( *src ) {
        if( *src == '\\' ) {
            *dst++ = '\\';
        }
        *dst++ = *src++;
    }
    *dst++ = *src++;
    return( dst - org );
}

#define _FUNCTION_expandable    (CompFlags.cpp_output == 0)

static int genFUNCTION(
    int spec_macro )
{
    SYMBOL sym;
    size_t len;
    VBUF buff;
    char *name;

    DbgAssert( ( spec_macro == MACRO_FUNCTION )
            || ( spec_macro == MACRO_FUNC ) );

    if( ! _FUNCTION_expandable ) {
        name = SpcMacros[ spec_macro ].name;
        len = strlen( name );
        memcpy( Buffer, name, len + 1 );
        TokenLen = len;
        return( T_ID );
    }
    sym = ParseCurrFunction();
    if( sym != NULL ) {
        FormatSymWithTypedefs( sym, &buff );
        len = VbufLen( &buff );
        if( len >= BUF_SIZE ) {
            len = BUF_SIZE;
        }
        memcpy( Buffer, VbufString( &buff ), len );
        VbufFree( &buff );
    } else {
        Buffer[0] = '?';
        len = 1;
    }
    Buffer[len] = '\0';
    TokenLen = len + 1;
    return( T_STRING );
}

int SpecialMacro(               // EXECUTE A SPECIAL MACRO
    MEPTR fmentry )             // - macro entry
{
    switch( fmentry->parm_count ) {
    case MACRO_LINE:
        ultoa( TokenLine, Buffer, 10 );
        U32ToU64( TokenLine, &Constant64 );
        ConstType = TYP_SINT;
        return( T_CONSTANT );
    case MACRO_FILE:
        TokenLen = file_name_copy( Buffer, SrcFileNameCurrent() );
        return( T_STRING );
    case MACRO_DATE:
        TokenLen = stpcpy( Buffer, __Date ) - Buffer + 1;
        return( T_STRING );
    case MACRO_TIME:
        TokenLen = stpcpy( Buffer, __Time ) - Buffer + 1;
        return( T_STRING );
    case MACRO_FUNCTION:
    case MACRO_FUNC:
        return( genFUNCTION( fmentry->parm_count ) );
    case MACRO_CPLUSPLUS:
        Buffer[0] = '1';
        Buffer[1] = '\0';
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


static int nextMToken( unsigned prev_token )
{
    ppstate_t save_ppstate;

    internalTokenList = doGetMacroToken( internalTokenList, TRUE );
    if( CurToken == T_NULL ) {
        if( ScanOptionalComment() ) {
            CurToken = T_WHITE_SPACE;
        } else {
            if( prev_token != T_WHITE_SPACE ) {
                // at EOL, ChkControl skips white-space for you which
                // is not appropriate at this point (this white-space
                // may be significant)
                // [the prev_token check is req'd because the scanner
                //  will not advance past T_NULL]
                save_ppstate = PPState;
                PPState |= PPS_EOL;
                CurToken = ScanToken( 1 );
                PPState = save_ppstate;
                if( CurToken == T_NULL ) {
                    CurToken = T_WHITE_SPACE;
                }
            } else {
                CurToken = ScanToken( 1 );
            }
        }
    }
    return( CurToken );
}

static void saveParm(
    MEPTR               fmentry,
    int                 parm_cnt,
    MACRO_ARG           *macro_parms,
    TOKEN_LIST          *token_list,
    int                 total,
    BUFFER_HDR          **h )
{
    TOKEN_LIST *last_token;
    char *p;

    *h = TokenBufAddChar( *h, T_NULL );
    if( parm_cnt < fmentry->parm_count - 1 ) {
        p = CMemAlloc( total + TokenBufTotalSize( *h ) + 1 );
        macro_parms[ parm_cnt ].arg = p;
        if( token_list != NULL ) {
            last_token = token_list;
            do {
                token_list = token_list->next;
                p = stvcpy( p, token_list->buf, token_list->length );
            } while( token_list != last_token );
            RingFree( &token_list );
        }
        *h = TokenBufMove( *h, p );
    }
}

static MACRO_ARG *collectParms( MEPTR fmentry )
{
    int         bracket;
    TOKEN       tok;
    TOKEN       prev_tok;
    unsigned    parm_cnt_plus_one;
    unsigned    parm_cnt_reqd;
    unsigned    curr_cnt;
    int         total;
    int         ppscan_mode;
    MACRO_ARG   *macro_parms;
    TOKEN_LIST  *token_head;
    BUFFER_HDR  *htokenbuf;

    macro_parms = NULL;
    parm_cnt_plus_one = fmentry->parm_count;
    if( parm_cnt_plus_one != 0 ) { /* if () expected */
        // () = 1, (a) = 2, (a,b) = 3
        parm_cnt_reqd = parm_cnt_plus_one - 1;
        ppscan_mode = InitPPScan();             // enable T_PPNUMBER tokens
        htokenbuf = TokenBufInit( NULL );
        if( parm_cnt_reqd > 0 ) {
            macro_parms = CMemAlloc( parm_cnt_reqd * sizeof( MACRO_ARG ) );
            if( fmentry->macro_flags & MFLAG_HAS_VAR_ARGS )
                macro_parms[parm_cnt_reqd-1].arg = NULL;
        }
        curr_cnt = 0;
        tok = T_NULL;
        do {
            tok = nextMToken( tok );
        } while( tok == T_WHITE_SPACE );
        /* tok will now be a '(' */
        bracket = 0;
        token_head = NULL;
        total = 0;
        for( ;; ) {
            prev_tok = tok;
            do {
                tok = nextMToken( tok );
                if( tok != T_WHITE_SPACE ) break;
            } while( TokenBufSize( htokenbuf ) == 0 );
            if( tok == T_EOF || tok == T_NULL ) {
                CErr( ERR_INCOMPLETE_MACRO, fmentry->macro_name );
                InfMacroDecl( fmentry );
                macroDiagNesting();
                break;
            }
            if( tok == T_BAD_TOKEN && BadTokenInfo == ERR_MISSING_QUOTE ) {
                CErr1( ERR_MISSING_QUOTE );
                InfMacroDecl( fmentry );
                macroDiagNesting();
                tok = T_RIGHT_PAREN;
            }
            if( tok == T_LEFT_PAREN ) {
                ++bracket;
            } else if( tok == T_RIGHT_PAREN ) {
                if( bracket == 0 ) break;
                --bracket;
            } else if( tok == T_COMMA && bracket == 0 &&
                  ( !( ( fmentry->macro_flags & MFLAG_HAS_VAR_ARGS ) &&
                      curr_cnt == fmentry->parm_count - 2 ) ) ) {
                TokenBufRemoveWhiteSpace( htokenbuf );
                if( macro_parms != NULL ) {     // if expecting parms
                    saveParm( fmentry, curr_cnt, macro_parms, token_head,
                              total, &htokenbuf );
                }
                ++curr_cnt;
                token_head = NULL;
                total = 0;
                continue;
            }
            switch( tok ) {
              case T_WHITE_SPACE:
                if( prev_tok != T_WHITE_SPACE ) {
                    htokenbuf = TokenBufAddWhiteSpace( htokenbuf, tok );
                }
                break;
              case T_BAD_CHAR:
                htokenbuf = TokenBufAddChar( htokenbuf, tok );
                htokenbuf = TokenBufAddChar( htokenbuf, Buffer[0] );
                if( Buffer[1] != '\0' ) {
                    htokenbuf = TokenBufAddWhiteSpace( htokenbuf, T_WHITE_SPACE );
                }
                break;
              case T_CONSTANT:
              case T_PPNUMBER:
              case T_STRING:
              case T_LSTRING:
              case T_ID:
              case T_UNEXPANDABLE_ID:
              case T_BAD_TOKEN:
                htokenbuf = TokenBufAddChar( htokenbuf, tok );
                htokenbuf = TokenBufAddStr( htokenbuf, Buffer );
                break;
              default :
                htokenbuf = TokenBufAddChar( htokenbuf, tok );
                break;
            }
        }
        TokenBufRemoveWhiteSpace( htokenbuf );
        if( macro_parms != NULL ) {     // if expecting parms
            saveParm( fmentry, curr_cnt, macro_parms,
                      token_head, total, &htokenbuf );
            ++curr_cnt;
        } else if( TokenBufSize( htokenbuf ) + total != 0 ) {
            ++curr_cnt;
        }
        if( ( ( fmentry->macro_flags & MFLAG_HAS_VAR_ARGS ) &&
              ( curr_cnt < parm_cnt_reqd - 1 ) )
            ||( !(fmentry->macro_flags & MFLAG_HAS_VAR_ARGS ) &&
              ( curr_cnt < parm_cnt_reqd ) ) ) {
            CErr( ERR_TOO_FEW_MACRO_PARMS, fmentry->macro_name );
            InfMacroDecl( fmentry );
            macroDiagNesting();
            do {
                htokenbuf = TokenBufAddWhiteSpace( htokenbuf, T_WHITE_SPACE );
                saveParm( fmentry, curr_cnt, macro_parms, NULL, 1, &htokenbuf );
                ++curr_cnt;
            } while( curr_cnt < parm_cnt_reqd );
        } else if( !( fmentry->macro_flags & MFLAG_HAS_VAR_ARGS ) &&
                   ( curr_cnt > parm_cnt_reqd ) ) {
            CErr( ANSI_TOO_MANY_MACRO_PARMS, fmentry->macro_name );
            InfMacroDecl( fmentry );
            macroDiagNesting();
        } else if( strcmp( fmentry->macro_name, "va_start" ) == 0 ) {
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
void DumpMTokens( MACRO_TOKEN *mtok )
{
    puts( "---" );
    while( mtok ) {
        if( mtok->token == T_MACRO_PARM ) {
            printf( "%p: <macro-parm>\n", mtok );
        } else {
            printf( "%p: '%s'\n", mtok, mtok->data );
        }
        mtok = mtok->next;
    }
    puts( "---" );
}
void DumpNestedMacros( void )
{
    NESTED_MACRO *nested;

    nested = nestedMacros;
    puts( "---" );
    while( nested ) {
        printf( "%p: %s\n", nested->fmentry, nested->fmentry->macro_name );
        nested = nested->next;
    }
    puts( "---" );
}
#endif


static MACRO_TOKEN *buildAToken( unsigned token, char *p )
{
    size_t nbytes;
    MACRO_TOKEN *mtok;

    nbytes = strlen( p ) + 1;
    mtok = CMemAlloc( offsetof( MACRO_TOKEN, data ) + nbytes );
    mtok->next = NULL;
    mtok->token = token;
    memcpy( mtok->data, p, nbytes );
    return( mtok );
}

static MACRO_TOKEN **buildTokenOnEnd( MACRO_TOKEN **ptail, unsigned token, char *str )
{
    MACRO_TOKEN *mtok;

    mtok = buildAToken( token, str );
    mtok->next = *ptail;
    *ptail = mtok;
    return( &(mtok->next) );
}

static void buildTokenAfter( MACRO_TOKEN *ptail, unsigned token, char *str )
{
    MACRO_TOKEN *mtok;

    mtok = buildAToken( token, str );
    mtok->next = ptail->next;
    ptail->next = mtok;
}

static MACRO_TOKEN *appendToken( MACRO_TOKEN *head, int token, char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new_tok;

    new_tok = buildAToken( token, data );
    if( head == NULL ) {
        head = new_tok;
    } else {
        tail = head;
        while( tail->next != NULL ) tail = tail->next;
        tail->next = new_tok;
    }
    return( head );
}

static int macroBeingExpanded( MEPTR fmentry )
{
    NESTED_MACRO *nested;

    nested = nestedMacros;
    while( nested ) {
        if( nested->fmentry == fmentry )  return( 1 );
        if( ! nested->rescanning )  break;
        nested = nested->next;
    }
    return( 0 );
}

static int isExpandable( MEPTR curr_mac, MACRO_TOKEN *mtok, int macro_parm )
{
    int lparen;

    if( curr_mac->macro_defn == 0 ) {  /* if special macro */
        if( ( curr_mac->parm_count == MACRO_FUNCTION )
         || ( curr_mac->parm_count == MACRO_FUNC ) ) {
            if( ! _FUNCTION_expandable ) {
                return( 0 );
            }
        }
        return( 1 );
    }
    if( curr_mac->parm_count == 0 ) { /* if () not expected */
        if( macro_parm ) {
            if( macroBeingExpanded( curr_mac ) )  return( 0 );
        }
        return( 1 );
    }
    for(;;) {
        if( mtok == NULL ) break;
        if( mtok->token != T_WHITE_SPACE  &&  mtok->token != T_NULL ) break;
        mtok = mtok->next;
    }
    if( mtok != NULL ) {
        if( mtok->token == T_LEFT_PAREN ) {
            if( macroDepth == 1  &&  !macro_parm )  return( 1 );
            lparen = 0;
            for(;;) {
                mtok = mtok->next;
                if( mtok == NULL ) break;
                if( mtok->token == T_LEFT_PAREN ) {
                    ++lparen;
                } else if( mtok->token == T_RIGHT_PAREN ) {
                    if( lparen == 0 )  return( 1 );
                    --lparen;
                }
            }
        }
    } else if( ! macro_parm ) {
        SkipAhead();
        if( CurrChar == '(' ) {
            return( 1 );
        } else if( CompFlags.cpp_output ) {
            return( 2 );
        }
    }
    return( 0 );
}

static MEPTR isAMacro( MACRO_TOKEN *mtok )
{
    const char *s;
    unsigned len;
    MEPTR fmentry;

    DbgStmt( ( Buffer[0] = '?' , Buffer[1] = '\0' ) );
    s = mtok->data;
    len = strlen( s );
    fmentry = MacroLookup( s, len );
    return( fmentry );
}

static MACRO_TOKEN *expandNestedMacros( MACRO_TOKEN *head, int rescanning )
{
    MEPTR fmentry;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *old_tokenlist;
    MACRO_TOKEN **ptail;
    int i;

    mtok = head;
    ptail = &head;
    ++macroDepth;
#ifndef NDEBUG
    if( macroDepth > 100 ) {
        __trap();
    }
#endif
    for(;;) {
        if( mtok == NULL ) break;
        toklist = NULL;
        if( mtok->token == T_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            fmentry = isAMacro( mtok );
            if( fmentry != NULL ) {
                if( rescanning ) {
                    if( macroBeingExpanded( fmentry ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = internalTokenList;
                        i = isExpandable( fmentry, mtok->next, 0 );
                        switch( i ) {
                        case 0:         // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case 1:         // macro is expandable
                            internalTokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                ptail = &head;
                            }
                            CMemFree( mtok );
                            toklist = macroExpansion( fmentry, rescanning );
                            mtok = internalTokenList;
                            internalTokenList = NULL;
                            break;
                        case 2:         // we skipped over some white space
                            mtok->token = T_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            buildTokenAfter( mtok, T_WHITE_SPACE, " " );
                            toklist = NULL;
                            break;
                        }
                    }
                } else {        // expanding a macro parm
                    if( isExpandable( fmentry, mtok->next, 1 ) ) {
                        old_tokenlist = internalTokenList;
                        internalTokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            ptail = &head;
                        }
                        CMemFree( mtok );
                        toklist = nestedMacroExpansion( fmentry, rescanning );
                        mtok = internalTokenList;
                        internalTokenList = old_tokenlist;
                    } else {
                        ptail = &(mtok->next);
                        mtok = mtok->next;      // advance onto next token
                    }
                }
            } else {
                mtok->token = T_SAVED_ID;       // avoid rechecking this ID
                ptail = &(mtok->next);
                mtok = mtok->next;      // advance onto next token
            }
        } else if( mtok->token == T_NULL ) {
            toklist = mtok->next;
            if( mtok->data[0] == 'Z' ) {        // end of a macro
                rescanning = nestedMacros->rescanning;
                deleteNestedMacro();
                CMemFree( mtok );
                mtok = toklist;
            } else {                            // end of a macro parm
                if( toklist != NULL ) {
                    internalTokenList = toklist;
                }
                CMemFree( mtok );
                mtok = NULL;                    // will cause us to exit
            }
            toklist = NULL;
        } else {                        // advance onto next token
            ptail = &(mtok->next);
            mtok = mtok->next;
        }
        if( toklist != NULL ) {         // new tokens to insert
            *ptail = toklist;
            if( mtok != NULL ) {
                while( toklist->next != NULL )  toklist = toklist->next;
                toklist->next = mtok;
            }
            mtok = *ptail;
        } else {
            // either no change, or tokens were deleted
            *ptail = mtok;
        }
    }
    for( mtok = head; mtok; mtok = mtok->next ) {
        // change a temporarily unexpandable ID into an ID because it
        // could become expandable in a later rescanning phase
        if( mtok->token == T_MACRO )  mtok->token = T_ID;
    }
    --macroDepth;
    return( head );
}

static MACRO_TOKEN *glue2Tokens( MACRO_TOKEN *first, MACRO_TOKEN *second )
{
    MACRO_TOKEN *head;
    MACRO_TOKEN **ptail;
    unsigned i;
    int ppscan_mode;

    i = 10;
    Buffer[i] = '\0';
    if( first != NULL ) {
        i = expandMacroToken( i, &first->token );
    }
    if( second != NULL ) {
        i = expandMacroToken( i, &second->token );
    }
    ppscan_mode = InitPPScan();
    ReScanInit( &Buffer[10] );
    head = NULL;
    ptail = &head;
    for(;;) {
        ReScanToken();
        ptail = buildTokenOnEnd( ptail, CurToken, Buffer );
        if( CompFlags.rescan_buffer_done ) break;
    }
    FiniPPScan( ppscan_mode );
    return( head );
}

static MACRO_TOKEN *glueTokens( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *next;
    MACRO_TOKEN *new_mtok;
    MACRO_TOKEN **ptail, **_ptail;

    mtok = head;
    _ptail = NULL;
    ptail = &head;
    for(;;) {
        if( mtok == NULL ) break;
        if( mtok->token != T_WHITE_SPACE ) {
            next = mtok->next;
            if( next == NULL ) break;
            if( next->token == T_WHITE_SPACE )  next = next->next;
            if( next == NULL ) break;
            if( next->token == T_MACRO_SHARP_SHARP ) {
                next = next->next;
                if( next->token == T_WHITE_SPACE )  next = next->next;
                // glue mtok->token with next->token to make one token
                // create new token

                if(next == NULL) break;

                if( ( next->token == T_MACRO_EMPTY_VAR_PARM ) ||
                    ( mtok->token == T_MACRO_EMPTY_VAR_PARM ) )
                {
                    MACRO_TOKEN *rem;
                    if( next->token != T_COMMA &&
                        mtok->token == T_MACRO_EMPTY_VAR_PARM ) //  EMPTY##
                    {
                        //keep the next token...
                        rem = next;
                        while( mtok->next != rem ){ //free old stuff [mtoken,##,{atok,} next]
                            next = mtok->next;
                            CMemFree( mtok );
                            mtok = next;
                        }
                    }
                    else
                    {
                        if( next->token == T_COMMA ||
                            mtok->token == T_COMMA ) // EMPTY##, or ,##EMPTY
                        {
                            // delete whole expression
                            rem = next->next;  // save unseen stuff
                            next->next = NULL; // break link;
                        }
                        else // ##EMPTY
                        {
                            // keep the thing after the ##
                            rem = next->next;
                            next->next = NULL;
                            _ptail = ptail;
                            ptail = &(mtok->next);
                            mtok = mtok->next; // skip first...
                        }
                        while( mtok != NULL ){ //free old stuff [mtoken,##,{atok,} next]
                            next = mtok->next;
                            CMemFree( mtok );
                            mtok = next;
                        }
                    }
                    *ptail = rem;
                    if( _ptail )
                    {
                        // skip back one param..
                        ptail = _ptail;
                        mtok = *ptail;
                    }
                    else
                    {
                        *ptail = rem;
                        mtok = head;
                    }
                }
                else
                {
                    MACRO_TOKEN *last;
                    last = new_mtok = glue2Tokens( mtok, next );
                    *ptail = new_mtok;
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
                    if( !_ptail )
                        head = new_mtok;
                    mtok = new_mtok;
                }
                continue;       /* to catch consecutive ##'s */
            }
        }
        _ptail = ptail;
        ptail = &(mtok->next);
        mtok = mtok->next;
    }
    return( head );
}

static MACRO_TOKEN **snapString( MACRO_TOKEN **ptail, unsigned i )
{
    Buffer[i] = '\0';
    TokenLen = i + 1;
    CurToken = T_STRING;
    ptail = buildTokenOnEnd( ptail, T_STRING, Buffer );
    return( ptail );
}

static MACRO_TOKEN **buildString( MACRO_TOKEN **ptail, void *str )
{
    MACRO_TOKEN **old_ptail;
    unsigned char *p = str;
    char *token_str;
    unsigned len;
    int i;
    int last_non_ws;
    int c;

    old_ptail = ptail;
    i = 0;
    last_non_ws = 0;
    if( p != NULL ) {
        // skip leading whitespace
        while( *p == T_WHITE_SPACE ) {
            ++p;
        }
        while( *p != T_NULL ) {
            if( i >= (BUF_SIZE-8) ) {
                DbgAssert( p[0] != T_WHITE_SPACE && p[1] != T_NULL );
                ptail = snapString( ptail, i );
                i = 0;
                last_non_ws = 0;
            }
            switch( *p ) {
            case T_WHITE_SPACE:
                ++p;
                while( *p == T_WHITE_SPACE ) {
                    ++p;
                }
                Buffer[i++] = ' ';
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:
                ++p;
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    if( c == '\\' ) {
                        Buffer[i++] = c;
                    }
                    Buffer[i++] = c;
                    if( i >= (BUF_SIZE-8) ) {
                        ptail = snapString( ptail, i );
                        i = 0;
                    }
                }
                last_non_ws = i;
                break;
            case T_LSTRING:
                Buffer[i++] = 'L';
            case T_STRING:
                ++p;
                Buffer[i++] = '\\';
                Buffer[i++] = '"';
                for(;;) {
                    c = *p++;
                    if( c == '\0' ) break;
                    if( c == '\\' || c == '"' ) {
                        Buffer[i++] = '\\';
                    }
                    Buffer[i++] = c;
                    if( i >= (BUF_SIZE-8) ) {
                        ptail = snapString( ptail, i );
                        i = 0;
                    }
                }
                Buffer[i++] = '\\';
                Buffer[i++] = '"';
                last_non_ws = i;
                break;
            case T_BAD_CHAR:
                ++p;
                Buffer[i++] = *p++;
                last_non_ws = i;
                break;
            default:
                token_str = Tokens[ *p ];
                ++p;
                len = strlen( token_str );
                if( i >= (BUF_SIZE-len) ) {
                    ptail = snapString( ptail, i );
                    i = 0;
                }
                memcpy( &Buffer[i], token_str, len );
                i += len;
                last_non_ws = i;
            }
        }
    }
    if( last_non_ws != 0 || old_ptail == ptail ) {
        ptail = snapString( ptail, last_non_ws );
    }
    return( ptail );
}

static MACRO_TOKEN **buildMTokenList( MACRO_TOKEN **ptail, void *ptr, MACRO_ARG *macro_parms )
{
    unsigned char *p = ptr;
    unsigned prev_token;
    unsigned curr_token;
    auto char buf[2];

    buf[1] = '\0';
    prev_token = T_NULL;
    if( p == NULL ) {
        return( ptail );
    }
    for(;;) {
        if( *p == 0 ) break;
        curr_token = *p;
        switch( curr_token ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
        case T_LSTRING:
        case T_STRING:
            ptail = buildTokenOnEnd( ptail, curr_token, (char *)p + 1 );
            ++p;
            while( *p++ ) {;}
            prev_token = curr_token;
            break;
        case T_WHITE_SPACE:
            ++p;
            if( prev_token == T_MACRO_SHARP_SHARP ) continue;
            ptail = buildTokenOnEnd( ptail, T_WHITE_SPACE, " " );
            break;
        case T_BAD_CHAR:
            ++p;
            buf[0] = *p++;
            ptail = buildTokenOnEnd( ptail, T_BAD_CHAR, buf );
            prev_token = T_BAD_CHAR;
            break;
        case T_MACRO_SHARP:
            ++p;                // skip over T_MACRO_SHARP
            while( *p == T_WHITE_SPACE ) ++p;
            if( *p != T_MACRO_PARM &&
                *p != T_MACRO_VAR_PARM ) {
                // we had an error before; handle as T_BAD_CHAR
                buf[0] = '#';
                ptail = buildTokenOnEnd( ptail, T_BAD_CHAR, buf );
                prev_token = T_BAD_CHAR;
                break;
            }
            ++p;                // skip over T_MACRO_PARM
            if( macro_parms ) {
                ptail = buildString( ptail, macro_parms[*p].arg );
            } else {
                Buffer[0] = '\0';
                ptail = buildTokenOnEnd( ptail, T_STRING, Buffer );
            }
            ++p;
            prev_token = T_STRING;
            break;
        case T_MACRO_PARM:
            ++p;
            buf[0] = *p++;
            ptail = buildTokenOnEnd( ptail, T_MACRO_PARM, buf );
            prev_token = T_MACRO_PARM;
            break;
        case T_MACRO_VAR_PARM:
            ++p;
            buf[0] = *p++;
            ptail = buildTokenOnEnd( ptail, T_MACRO_VAR_PARM, buf );
            prev_token = T_MACRO_VAR_PARM;
            break;
        default:
            ptail = buildTokenOnEnd( ptail, curr_token, Tokens[*p] );
            ++p;
            prev_token = curr_token;
        }
    }
    return( ptail );
}

static int SharpSharp( MACRO_TOKEN *mtok )
{
    for(;;) {
        if( mtok == NULL ) break;
        if( mtok->token == T_MACRO_SHARP_SHARP )  return( 1 );
        if( mtok->token != T_WHITE_SPACE )  break;
        mtok = mtok->next;
    }
    return( 0 );
}

static MACRO_TOKEN *substituteParms( MACRO_TOKEN *head, MACRO_ARG *macro_parms )
{
    unsigned prev_non_ws;
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *list;
    MACRO_TOKEN *dummy_list;
    MACRO_TOKEN **ptail;

    mtok = head;
    ptail = &head;
    prev_non_ws = T_NULL;
    for(;;) {
        if( mtok == NULL ) break;
        list = NULL;
        if( mtok->token == T_MACRO_PARM ||
            mtok->token == T_MACRO_VAR_PARM ) {
            // replace this ID with a copy of the tokens from
            // macro_parms[mtok->data[0]].arg
            dummy_list = NULL;
            if( mtok->token != T_MACRO_VAR_PARM ||
                macro_parms[(unsigned char)mtok->data[0]].arg )
            {
                buildMTokenList( &dummy_list, macro_parms[(unsigned char)mtok->data[0]].arg, NULL );
            }
            else
            {
                unsigned char p[2] = { T_MACRO_EMPTY_VAR_PARM, 0 };
                buildMTokenList( &dummy_list, p, NULL );
            }
            list = dummy_list;
            if( prev_non_ws != T_MACRO_SHARP_SHARP && !SharpSharp(mtok->next) ) {
                list = expandNestedMacros( list, FALSE );
            }
            if( list == NULL ) {
                list = mtok;
                mtok = mtok->next;
                CMemFree( list );
                list = NULL;
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
        if( mtok == NULL ) break;
        ptail = &(mtok->next);
        if( mtok->token != T_WHITE_SPACE ) {
            prev_non_ws = mtok->token;
        }
        mtok = mtok->next;
    }
    return( head );
}

static void markUnexpandableIds( MACRO_TOKEN *head )
{
    NESTED_MACRO *nested;
    MACRO_TOKEN *mtok;

    for( mtok = head; mtok; mtok = mtok->next ) {
        if( mtok->token == T_ID ) {
            for( nested = nestedMacros; nested; nested = nested->next ) {
                if( strcmp( nested->fmentry->macro_name, mtok->data ) == 0 ) {
                    if( ! nested->substituting_parms ) {
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

static MACRO_TOKEN *macroExpansion( MEPTR fmentry, int rescanning )
{
    MACRO_ARG *macro_parms;
    MACRO_TOKEN *head;
    MACRO_TOKEN **ptail;
    NESTED_MACRO *nested;
    char *tokens;

    nested = CarveAlloc( carveNESTED_MACRO );
    nested->fmentry = fmentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = FALSE;
    nested->macro_parms = NULL;
    head = NULL;
    ptail = &head;
    if( fmentry->macro_defn == 0 ) {    /* if special macro */
        CurToken = SpecialMacro( fmentry );
        ptail = buildTokenOnEnd( ptail, CurToken, Buffer );
        nested->next = nestedMacros;
        nestedMacros = nested;
    } else {
        macro_parms = collectParms( fmentry );
        nested->next = nestedMacros;
        nestedMacros = nested;
        nested->macro_parms = macro_parms;
        tokens = ((char*) fmentry) + fmentry->macro_defn;
        ptail = buildMTokenList( ptail, tokens, macro_parms );
        if( macro_parms != NULL ) {
            nested->substituting_parms = TRUE;
            head = substituteParms( head, macro_parms );
            nested->substituting_parms = FALSE;
        }
        head = glueTokens( head );
        markUnexpandableIds( head );
    }
    head = appendToken( head, T_NULL, "Z-<end of macro>" );
    return( head );
}

static MACRO_TOKEN *nestedMacroExpansion( MEPTR fmentry, boolean rescanning )
{
    return( expandNestedMacros( macroExpansion( fmentry, FALSE ), rescanning ) );
}

void DoMacroExpansion(          // EXPAND A MACRO
    MEPTR fmentry )             // - macro to expand
{
    DbgAssert( scannerTokenList == NULL );
    macroDepth = 0;
    scannerTokenList = nestedMacroExpansion( fmentry, TRUE );
    // GetMacroToken will feed back tokens from the tokenList
    // when the tokenList is exhausted, then revert back to normal scanning
    if( scannerTokenList == NULL ) {
        CompFlags.use_macro_tokens = FALSE;
    } else {
        CompFlags.use_macro_tokens = TRUE;
    }
}

void DefineAlternativeTokens(   // DEFINE ALTERNATIVE TOKENS
    void )
{
    SPECIAL_MACRO_NAME *mac;

    for( mac = SpcMacros + MACRO_ALT_MARKER + 1; mac->name != NULL; ++mac ) {
        MacroSpecialAdd( mac->name, mac->value, mac->flags );
    }
}
