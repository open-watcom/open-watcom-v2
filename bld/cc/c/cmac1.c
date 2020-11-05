/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "cvars.h"
#include "scan.h"
#include "cfeinfo.h"
#include "dumpapi.h"
#include "cmacsupp.h"

#include "clibext.h"


#define MACRO_END_CHAR          'Z'
#define MACRO_END_STRING        "Z-<end of macro>"

typedef struct tokens {
    struct  tokens  *next;
    size_t          length;
    char            buf[1];
} tokens;

typedef struct macro_token {
    struct macro_token  *next;
    TOKEN               token;
    char                data[1];
} MACRO_TOKEN;

typedef struct macro_arg {
    char    *arg;
} MACRO_ARG;

typedef struct nested_macros {
    struct nested_macros    *next;
    MEPTR                   mentry;
    MACRO_ARG               *macro_parms;
    bool                    rescanning;
    bool                    substituting_parms;
} NESTED_MACRO;

typedef struct special_macro_names {
    const char      *name;
    special_macros  value;
    macro_flags     macro_flags;
} special_macro_names;

extern void         DumpMTokens( MACRO_TOKEN *mtok );

static NESTED_MACRO *NestedMacros;
static MACRO_TOKEN  *TokenList;
static size_t       MTokenLen;              /* macro token length */

static MACRO_TOKEN  *ExpandNestedMacros( MACRO_TOKEN *head, bool rescanning );

static struct special_macro_names  SpcMacros[] = {
    #define pick(s,i,f)     { s, i, f },
    #include "specmac.h"
    #undef pick
};

/* Macro version flag to C standard version */
static const long c_ver[8] = {
    LONG_MAX, C90, C95, C99, C11, C18, LONG_MAX, LONG_MAX
};

static void SpecialMacroAdd( special_macro_names *mac )
{
    MEPTR           mentry;

    mentry = CreateMEntry( mac->name, 0 );
    mentry->parm_count = (mac_parm_count)mac->value;
    MacroDefine( mentry->macro_len, mac->macro_flags );
}

void MacroInit( void )
{
    mac_hash_idx h;

    MTokenLen = 0;
    MacroCount = 0;
    MacroPtr = NULL;
    CppStackInit();
    NestedMacros = NULL;
    TokenList = NULL;
    UndefMacroList = NULL;
    InitialMacroFlags = MFLAG_DEFINED_BEFORE_FIRST_INCLUDE;
    MacHash = PermMemAlloc( MACRO_HASH_SIZE * sizeof( MEPTR ) );
    for( h = 0; h < MACRO_HASH_SIZE; ++h ) {
        MacHash[h] = NULL;
    }
}

void MacroAddSpecial(void)
{
    int i;

    for( i = MACRO_FIRST; i <= MACRO_LAST; i++ ) {
        if (!CompFlags.extensions_enabled
            && stdc_version < c_ver[MacroStdCVersion(SpcMacros + i)])
            continue;
        SpecialMacroAdd( &SpcMacros[i] );
    }

    /* grab time and date for __TIME__ and __DATE__ */
    TimeInit();
}

void MacroAddComp( void )
{
    int             i;

    for( i = MACRO_COMP_FIRST; i <= MACRO_COMP_LAST; i++ ) {
        if (!CompFlags.extensions_enabled
            && stdc_version < c_ver[MacroStdCVersion(SpcMacros + i)])
            continue;
        SpecialMacroAdd( &SpcMacros[i] );
    }
}

void MacroFini( void )
{
    CppStackFini();
    MacroPurge();
}


void MacroPurge( void )
{
#if 0
    mac_hash_idx    h;
    MEPTR           mentry;

    for( h = 0; h < MACRO_HASH_SIZE; ++h ) {
        for( ; mentry = MacHash[h]; ) {
            MacHash[h] = mentry->next_macro;
            CMemFree( mentry );
        }
    }
#endif
}

static void DeleteNestedMacro( void )
{
    MEPTR           mentry;
    NESTED_MACRO    *nested;
    MACRO_ARG       *macro_parms;
    mac_parm_count  parmno;

    nested = NestedMacros;
    if( nested != NULL ) {
        NestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        mentry = nested->mentry;
        CMemFree( nested );
        if( macro_parms != NULL ) {
            for( parmno = GetMacroParmCount( mentry ); parmno-- > 0; ) {
                CMemFree( macro_parms[parmno].arg );
            }
            CMemFree( macro_parms );
        }
    }
}

void GetMacroToken( void )
{
    MACRO_TOKEN     *mtok;
    bool            keep_token;

    Buffer[0] = '\0';
    TokenLen = 0;
    CurToken = T_NULL;
    for( ;; ) {
        if( (mtok = TokenList) == NULL ) {
            MacroPtr = NULL;
            break;
        }
        if( (CurToken = mtok->token) != T_NULL ) {
            while( (Buffer[TokenLen] = mtok->data[TokenLen]) != '\0' ) {
                TokenLen++;
            }
            keep_token = false;
            switch( CurToken ) {
            case T_SAVED_ID:
                if( CompFlags.doing_macro_expansion ) {
                    CurToken = T_ID;
                }
                /* fall through */
            case T_ID:
            case T_UNEXPANDABLE_ID:
                CalcHash( Buffer, TokenLen );
                if( !CompFlags.doing_macro_expansion ) {
                    if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
                        TokenList = mtok->next;
                        CMemFree( mtok );
                        CurToken = Process_Pragma();
                        mtok = TokenList;
                        keep_token = true;
                    } else {
                        CurToken = KwLookup( Buffer, TokenLen );
                    }
                }
                break;
            case T_BAD_TOKEN:
            case T_CONSTANT:
                ReScanInit( mtok->data );
                ReScanToken();
                break;
            case T_PPNUMBER:
                ReScanInit( mtok->data );
                if( ReScanToken() ) {   // didn't finish string bad boy
                    const char  *tcur;
                    char        *tbeg;

                    tbeg = mtok->data;
                    tcur = ReScanPos();
                    // ppnumber is quite general so it may absorb multiple tokens
                    // overlapping src & dst so do our own copy;
                    for( ; (*tbeg = *tcur) != '\0'; ) {
                        ++tcur;
                        ++tbeg;
                    }
                    keep_token = true;
                }
                break;
            case T_LSTRING:
                CurToken = T_STRING;
                CompFlags.wide_char_string = true;
                break;
            case T_STRING:
                CompFlags.wide_char_string = false;
                break;
            }
            if( !keep_token ) {
                TokenList = mtok->next;
                CMemFree( mtok );
            }
            break;
        }
        if( mtok->data[0] == MACRO_END_CHAR ) {     // if end of macro
            DeleteNestedMacro();
        }
        TokenList = mtok->next;
        CMemFree( mtok );
    }
}

/* returns Dynamically allocated buffer with expanded macro */
static char *ExpandMacroToken( void )
{
    size_t      i;
    size_t      len;
    const char  *p;
    char        *buf;
    TOKEN       tok;

    tok = MTOK( MacroPtr );
    if( tok == T_NULL )
        return( NULL );
    MTOKINC( MacroPtr );
    p = NULL;
    len = 0;
    switch( tok ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:
        p = MacroPtr;
        len = strlen( p );
        MacroPtr += len;
        break;
    case T_LSTRING:
        len = 1;
        /* fall through */
    case T_STRING:
        len += strlen( MacroPtr ) + 3;
        buf = CMemAlloc( len );
        i = 0;
        if( tok == T_LSTRING )
            buf[i++] = 'L';
        buf[i++] = '"';
        while( (buf[i] = *MacroPtr++) != '\0' )
            ++i;
        buf[i++] = '"';
        buf[i] = '\0';
        return( buf );
    default:
        p = Tokens[tok];
        len = strlen( p );
        break;
    }
    buf = NULL;
    if( len > 0 ) {
        len++;              /* account for terminating null character */
        buf = CMemAlloc( len );
        memcpy( buf, p, len );
    }
    return( buf );
}


TOKEN SpecialMacro( MEPTR mentry )
{
    char            *p;
    char            *bufp;

    CompFlags.wide_char_string = false;
    switch( (special_macros)mentry->parm_count ) {
    case MACRO_LINE:
        sprintf( Buffer, "%u", TokenLoc.line );
        Constant = TokenLoc.line;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_FILE:
        bufp = Buffer;
        for( p = FileIndexToFName( TokenLoc.fno )->name; (*bufp++ = *p) != '\0'; ++p ) {
            if( *p == '\\' ) {
                *bufp++ = '\\';
            }
        }
        break;
    case MACRO_DATE:
        strcpy( Buffer, __Date );
        break;
    case MACRO_TIME:
        strcpy( Buffer, __Time );
        break;
    case MACRO_STDC_HOSTED:
    case MACRO_STDC:
        Buffer[0] = '1';
        Buffer[1] = '\0';
        Constant = 1;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_STDC_VERSION:
        *Buffer   = '\0';
        ConstType = TYPE_LONG;
        Constant  = stdc_version;

        switch (stdc_version) {
        default:  CPYLIT(Buffer, "199409L"); break;
        case C99: CPYLIT(Buffer, "199901L"); break;
        case C11: CPYLIT(Buffer, "201112L"); break;
        case C18: CPYLIT(Buffer, "201710L"); break;
        }
        return T_CONSTANT;
    case MACRO_FUNCTION:
    case MACRO_FUNC:
        Buffer[0] = '\0';
        if(CurFunc && CurFunc->name)
            strcpy( Buffer, CurFunc->name );
        break;
    default:
        Buffer[0] = '\0';
        return( T_NULL );   // shut up the compiler
    }
    TokenLen = strlen( Buffer );
    return( T_STRING );
}


static TOKEN NextMToken( void )
{
    CompFlags.doing_macro_expansion = true;
    GetMacroToken();
    if( CurToken == T_NULL ) {
        CurToken = ScanToken();
    }
    CompFlags.doing_macro_expansion = false;
    return( CurToken );
}

void EnlargeBuffer( size_t size )
{
    char       *newBuffer;

    newBuffer = CMemAlloc( size );
    memcpy( newBuffer, Buffer, BufSize );
    CMemFree( (void *)Buffer );
    Buffer = newBuffer;
    newBuffer = CMemAlloc( size );
    memcpy( newBuffer, TokenBuf, BufSize );
    CMemFree( TokenBuf );
    TokenBuf = newBuffer;
    BufSize = size;
}

static void SaveParm( MEPTR mentry, size_t size, mac_parm_count parmno,
                     MACRO_ARG *macro_parms, tokens *token_list )
{
    tokens          *token;
    char            *p;
    size_t          total;

    MTOK( TokenBuf + size ) = T_NULL;
    MTOKINC( size );

    if( parmno < GetMacroParmCount( mentry ) ) {
        p = CMemAlloc( size );
        macro_parms[parmno].arg = p;
        if( p != NULL ) {
            total = 0;
            while( (token = token_list) != NULL ) {
                token_list = token->next;
                memcpy( &p[total], token->buf, token->length );
                total += token->length;
                CMemFree( token );
            }
            memcpy( &p[total], TokenBuf, size );
        }
    }
}

static MACRO_ARG *CollectParms( MEPTR mentry )
{
    size_t          len;
    int             bracket;
    TOKEN           tok;
    TOKEN           prev_tok;
    mac_parm_count  parmno;
    mac_parm_count  parm_count_reqd;
    bool            ppscan_mode;
    MACRO_ARG       *macro_parms;
    tokens          *token_head;

    macro_parms = NULL;
    if( MacroWithParenthesis( mentry ) ) {     /* if () expected */
        ppscan_mode = InitPPScan();     // enable T_PPNUMBER tokens
        parm_count_reqd = GetMacroParmCount( mentry );
        if( parm_count_reqd > 0 ) {
            macro_parms = (MACRO_ARG *)CMemAlloc( parm_count_reqd * sizeof( MACRO_ARG ) );
        }
        parmno = 0;
        do {
            tok = NextMToken();
        } while( tok == T_WHITE_SPACE );
        /* tok will now be a '(' */
        bracket = 0;
        token_head = NULL;
        MTokenLen = 0;
        for( ;; ) {
            prev_tok = tok;
            do {
                tok = NextMToken();
            } while( tok == T_WHITE_SPACE && MTokenLen == 0 );
            if( tok == T_EOF || tok == T_NULL ) {
                CErr2p( ERR_INCOMPLETE_MACRO, mentry->macro_name );
                break;
            }
            if( tok == T_BAD_TOKEN && BadTokenInfo == ERR_MISSING_QUOTE ) {
                CErr1( ERR_MISSING_QUOTE );
                tok = T_RIGHT_PAREN;
            }
            if( tok == T_LEFT_PAREN ) {
                ++bracket;
            } else if( tok == T_RIGHT_PAREN ) {
                if( bracket == 0 )
                    break;
                --bracket;
            } else if( tok == T_COMMA && bracket == 0
              && ( !MacroHasVarArgs( mentry ) || parmno != parm_count_reqd - 1 ) ) {
                if( prev_tok == T_WHITE_SPACE ) {
                    MTOKDEC( MTokenLen );
                }
                if( macro_parms != NULL ) {     // if expecting parms
                    SaveParm( mentry, MTokenLen, parmno, macro_parms, token_head );
                }
                ++parmno;
                token_head = NULL;
                MTokenLen = 0;
                continue;
            }
            /* determine size of current token */
            len = sizeof( TOKEN );
            switch( tok ) {
            case T_WHITE_SPACE:
                if( prev_tok == T_WHITE_SPACE )
                    len = 0;
                break;
            case T_STRING:
                if( CompFlags.wide_char_string ) {
                    tok = T_LSTRING;
                }
                /* fall through */
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:
                len += TokenLen + 1;
                break;
            default:
                break;
            }
            if( MTokenLen + len >= BufSize ) { /* if not enough space */
                EnlargeBuffer( ( MTokenLen + len ) * 2 );
            }
            MTOK( TokenBuf + MTokenLen ) = tok;
            MTOKINC( MTokenLen );
            switch( tok ) {
            case T_WHITE_SPACE:
                if( prev_tok == T_WHITE_SPACE ) {
                    MTOKDEC( MTokenLen );
                }
                break;
            case T_BAD_CHAR:
                TokenBuf[MTokenLen++] = Buffer[0];
                if( Buffer[1] != '\0' ) {
                     MTOK( TokenBuf + MTokenLen ) = T_WHITE_SPACE;
                     MTOKINC( MTokenLen );
                }
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_LSTRING:
            case T_STRING:
            case T_BAD_TOKEN:
                memcpy( TokenBuf + MTokenLen, Buffer, TokenLen + 1 );
                MTokenLen += TokenLen + 1;
                break;
            default:
                break;
            }
        }
        if( prev_tok == T_WHITE_SPACE ) {
            MTOKDEC( MTokenLen );
        }
        if( macro_parms != NULL ) {     // if expecting parms
            SaveParm( mentry, MTokenLen, parmno, macro_parms, token_head );
            ++parmno;
        } else if( MTokenLen != 0 ) {
            ++parmno;                   // will cause "too many parms" error
        }
        if( MacroHasVarArgs( mentry ) && parmno < ( parm_count_reqd - 1 )
          || !MacroHasVarArgs( mentry ) && parmno < parm_count_reqd ) {
            CErr2p( ERR_TOO_FEW_MACRO_PARMS, mentry->macro_name );
        } else if( !MacroHasVarArgs( mentry ) && parmno > parm_count_reqd ) {
            if( parm_count_reqd > 0 ) {
                CWarn2p( WARN_PARM_COUNT_MISMATCH, ERR_TOO_MANY_MACRO_PARMS, mentry->macro_name  );
            }
        } else if( CMPLIT( mentry->macro_name, "va_start" ) == 0 ) {
            if( SymLevel != 0 && !VarParm( CurFunc ) ) {
                CErr1( ERR_MUST_BE_VAR_PARM_FUNC );
            }
        }
        FiniPPScan( ppscan_mode );      // disable T_PPNUMBER tokens
    }
    return( macro_parms );
}


#ifndef NDEBUG

void DumpMDefn( const char *p )
{
    unsigned char   c;
    TOKEN           tok;

    while( (tok = MTOK( p )) != T_NULL ) {
        MTOKINC( p );
        switch( tok ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            continue;
        case T_LSTRING:
            putchar( 'L' );
            /* fall through */
        case T_STRING:
            putchar( '\"' );
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            putchar( '\"' );
            continue;
        case T_WHITE_SPACE:
            putchar( ' ' );
            continue;
        case T_BAD_CHAR:
            putchar( *p++ );
            continue;
        case T_MACRO_PARM:
            printf( "<parm#%d>", MTOKPARM( p ) );
            MTOKPARMINC( p );
            continue;
        case T_MACRO_VAR_PARM:
            printf( "<varparm#%d>", MTOKPARM( p ) );
            MTOKPARMINC( p );
            continue;
        default:
            printf( "%s", Tokens[tok] );
            continue;
        }
    }
    putchar( '\n' );
    fflush( stdout );
}


void DumpMTokens( MACRO_TOKEN *mtok )
{
    for( ; mtok != NULL; mtok = mtok->next ) {
        printf( "%s\n", mtok->data );
    }
    fflush( stdout );
}


void DumpNestedMacros( void )
{
    NESTED_MACRO *nested;

    for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
        printf( "%s\n", nested->mentry->macro_name );
    }
    fflush( stdout );
}
#endif


static MACRO_TOKEN *BuildAToken( TOKEN token, const char *data )
{
    size_t      len;
    MACRO_TOKEN *mtok;

    len = strlen( data ) + 1;
    mtok = (MACRO_TOKEN *)CMemAlloc( sizeof( MACRO_TOKEN ) - 1 + len );
    memcpy( mtok->data, data, len );
    mtok->token = token;
    mtok->next = NULL;
    return( mtok );
}


static MACRO_TOKEN **BuildATokenOnEnd( MACRO_TOKEN **ptail, TOKEN token, const char *data )
{
    *ptail = BuildAToken( token, data );
    return( &(*ptail)->next );
}


static MACRO_TOKEN *AppendToken( MACRO_TOKEN *head, TOKEN token, const char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new;

    new = BuildAToken( token, data );
    if( head == NULL ) {
        head = new;
    } else {
        tail = head;
        while( tail->next != NULL ) {
            tail = tail->next;
        }
        tail->next = new;
    }
    return( head );
}

static bool MacroBeingExpanded( MEPTR mentry )
{
    NESTED_MACRO    *nested;

    for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
        if( nested->mentry == mentry )
            return( true );
        if( !nested->rescanning ) {
            break;
        }
    }
    return( false );
}

static int Expandable( MACRO_TOKEN *mtok, bool macro_parm, MEPTR mentry )
{
    int         lparen;

    if( MacroIsSpecial( mentry ) ) {            /* if special macro */
        return( 1 );
    }
    if( !MacroWithParenthesis( mentry ) ) {     /* if () not expected */
        if( macro_parm ) {
            if( MacroBeingExpanded( mentry ) ) {
                return( 0 );
            }
        }
        return( 1 );
    }
    for( ; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token != T_WHITE_SPACE && mtok->token != T_NULL ) {
            break;
        }
    }
    if( mtok != NULL ) {
        if( mtok->token == T_LEFT_PAREN ) {
            if( MacroDepth == 1 && !macro_parm )
                return( 1 );
            lparen = 0;
            for( ; (mtok = mtok->next) != NULL; ) {
                if( mtok->token == T_LEFT_PAREN ) {
                    ++lparen;
                } else if( mtok->token == T_RIGHT_PAREN ) {
                    if( lparen == 0 )
                        return( 1 );
                    --lparen;
                }
            }
        }
    } else if( !macro_parm ) {
        SkipAhead();
        if( CurrChar == '(' ) {
            return( 1 );
        } else if( CompFlags.cpp_mode ) {
            return( 2 );
        }
    }
    return( 0 );
}

static char *GlueTokenToBuffer( MACRO_TOKEN *first, char *gluebuf )
{
    size_t      gluelen;
    size_t      tokenlen;
    char        *buf;

    buf = NULL;
    if( first != NULL ) {
        MacroPtr = (char *)&first->token;
        buf = ExpandMacroToken();
    }
    if( buf == NULL ) {
        buf = gluebuf;
    } else if( gluebuf != NULL ) {
        /* now do a "strcat( gluebuf, buf )" */
        gluelen = strlen( gluebuf );
        tokenlen = strlen( buf );
        gluebuf = CMemRealloc( gluebuf, gluelen + tokenlen + 1 );
        memcpy( gluebuf + gluelen, buf, tokenlen + 1 );
        CMemFree( buf );
        buf = gluebuf;
    }
    return( buf );
}

static MACRO_TOKEN *ReTokenBuffer( const char *buffer )
// retokenize starting at buffer
{
    MACRO_TOKEN *head;
    MACRO_TOKEN **ptail;
    bool        ppscan_mode;

    ppscan_mode = InitPPScan();
    ReScanInit( buffer );
    head = NULL;
    ptail = &head;
    for( ;; ) {
        Buffer[0] = '\0';
        ReScanToken();
        ptail = BuildATokenOnEnd( ptail, CurToken, Buffer );
        if( CompFlags.rescan_buffer_done ) {
            break;
        }
    }
    FiniPPScan( ppscan_mode );
    return( head );
}


static MACRO_TOKEN *GlueTokens( MACRO_TOKEN *head )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN **ptail;
    MACRO_TOKEN **prev_ptail;   // prior ptail
    MACRO_TOKEN *next;
    char        *buf;
    char        *gluebuf;

    gluebuf = NULL;
    prev_ptail = NULL;
    ptail = &head;
    mtok = *ptail;
    buf = Buffer;
    for( ; mtok != NULL; ) {
        if( mtok->token != T_WHITE_SPACE ) {
            next = mtok->next;
            if( next == NULL )
                break;
            if( next->token == T_WHITE_SPACE )
                next = next->next;
            if( next == NULL )
                break;
            if( next->token == T_MACRO_SHARP_SHARP ) {  // let's paste
                int         pos;
                MACRO_TOKEN *rem;

                next = next->next;
                pos = 0;
                // glue mtok->token with next->token to make one token
                if( ( mtok->token == T_COMMA && next->token == T_MACRO_EMPTY_VAR_PARM ) ||
                    ( mtok->token == T_MACRO_EMPTY_VAR_PARM && next->token == T_COMMA ) )
                {
                    // delete [mtoken(a comma),##,empty __VA_ARGS__]
                    // delete [empty __VA_ARGS__,##,mtoken(a comma)]
                    buf[10] = '\0';
                    pos = -1;
                } else {
                    if( mtok->token == T_MACRO_EMPTY_VAR_PARM ) {
                        // well should never be in this state if no next - since ## cannot
                        // appear at the end of a macro
                        gluebuf = GlueTokenToBuffer( next, NULL );
                    } else {
                        gluebuf = GlueTokenToBuffer( mtok, NULL );
                        if( next != NULL && next->token != T_MACRO_EMPTY_VAR_PARM ) {
                            gluebuf = GlueTokenToBuffer( next, gluebuf ); //paste in next
                        }
                    }
                }
                if( next != NULL ) {
                    rem = next->next;   // save unseen stuff
                    next->next = NULL;  // break link;
                } else {
                    rem = NULL;         // happens if no arg after comma
                }
                while( mtok != NULL ) { // free old stuff [mtoken,##,{atok,} next]
                    next = mtok->next;
                    CMemFree( mtok );
                    mtok = next;
                }
                if( pos >= 0 ) {
                    if( gluebuf != NULL ) {
                        mtok = ReTokenBuffer( gluebuf );
                        CMemFree( gluebuf );
                    } else {
                        /* Both ends of ## were empty */
                        mtok = BuildAToken( T_NULL, "P-<placemarker>" );
                    }
                    *ptail = mtok;  // link in new mtok to mtok's link
                    while( mtok != NULL && mtok->next != NULL ) {   //position mtok & ptail to last token
                        prev_ptail = ptail;
                        ptail = &mtok->next;
                        mtok = *ptail;
                        if( mtok == mtok->next ) {
                            return( head );
                        }
                    }
                    // mtok == last token of retokenizing
                    // ptail == the pointer which references mtok
                    mtok->next = rem;
                } else {
                    if( prev_ptail != NULL ) {
                        *ptail = rem;
                        ptail = prev_ptail;
                        mtok = *ptail;
                    } else {
                        *ptail = rem;
                        mtok = head;
                    }
                }
                continue;          //ready to go
            }
        }
        prev_ptail = ptail;
        ptail = &mtok->next;
        mtok = *ptail;
    }
    return( head );
}

static MACRO_TOKEN *BuildString( const char *p )
{
    MACRO_TOKEN     *head;
    MACRO_TOKEN     **ptail;
    size_t          i;
    char            c;
    const char      *tokenstr;
    size_t          len;
    char            *buf;
    size_t          bufsize;
    TOKEN           tok;

    head = NULL;
    ptail = &head;

    len = 0;
    if( p != NULL ) {
        bufsize = BUF_SIZE;
        buf = CMemAlloc( bufsize );
        while( MTOK( p ) == T_WHITE_SPACE ) {
            MTOKINC( p );   //eat leading wspace
        }
        while( (tok = MTOK( p )) != T_NULL ) {
            MTOKINC( p );
            if( len >= ( bufsize - 8 ) ) {
                buf = CMemRealloc( buf, 2 * len );
            }
            switch( tok ) {
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:
                for( ; (c = *p++) != '\0'; ) {
                    if( c == '\\' )
                        buf[len++] = c;
                    buf[len++] = c;
                    if( len >= ( bufsize - 8 ) ) {
                        buf = CMemRealloc( buf, 2 * len );
                    }
                }
                break;
            case T_LSTRING:
                buf[len++] = 'L';
                /* fall through */
            case T_STRING:
                buf[len++] = '\\';
                buf[len++] = '"';
                for( ; (c = *p++) != '\0'; ) {
                    if( c == '\\' || c == '"' )
                        buf[len++] = '\\';
                    buf[len++] = c;
                    if( len >= ( bufsize - 8 ) ) {
                        buf = CMemRealloc( buf, 2 * len );
                    }
                }
                buf[len++] = '\\';
                buf[len++] = '"';
                break;
            case T_WHITE_SPACE:
                while( (tok = MTOK( p )) == T_WHITE_SPACE ) {
                    MTOKINC( p );
                }
                if( tok != T_NULL ) {
                    buf[len++] = ' ';
                }
                break;
            case T_BAD_CHAR:
                if( *p == '\\' && MTOK( p + 1 ) == T_NULL ) {
                    CErr1( ERR_INVALID_STRING_LITERAL );
                }
                buf[len++] = *p++;
                break;
            default:
                tokenstr = Tokens[tok];
                i = strlen( tokenstr );
                if( len >= ( bufsize - i ) )
                    buf = CMemRealloc( buf, 2 * len );
                memcpy( &buf[len], tokenstr, i );
                len += i;
                break;
            }
        }
        if( len > 0 ) {
            buf[len] = '\0';
            ptail = BuildATokenOnEnd( ptail, T_STRING, buf );
        }
        CMemFree( buf );
    }
    return( head );
}


static MACRO_TOKEN *BuildMTokenList( const char *p, MACRO_ARG *macro_parms )
{
    MACRO_TOKEN     *mtok;
    MACRO_TOKEN     *head;
    MACRO_TOKEN     **ptail;
    NESTED_MACRO    *nested;
    const char      *p2;
    char            buf[2];
    TOKEN           prev_token;
    TOKEN           tok;
    mac_parm_count  parmno;

    head = NULL;
    ptail = &head;
    nested = NestedMacros;
    buf[1] = '\0';
    prev_token = T_NULL;
    if( p == NULL )
        return( NULL );
    while( (tok = MTOK( p )) != T_NULL ) {
        MTOKINC( p );
        switch( tok ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
        case T_LSTRING:
        case T_STRING:
            mtok = BuildAToken( tok, p );
            while( *p++ != '\0' )
                ;
            break;
        case T_WHITE_SPACE:
            if( prev_token == T_MACRO_SHARP_SHARP )
                continue;
            mtok = BuildAToken( T_WHITE_SPACE, " " );
            break;
        case T_BAD_CHAR:
            buf[0] = *p++;
            mtok = BuildAToken( T_BAD_CHAR, buf );
            break;
        case T_MACRO_SHARP:
            while( MTOK( p ) == T_WHITE_SPACE ) {
                MTOKINC( p );
            }
            MTOKINC( p );   // skip over T_MACRO_PARM
            // If no macro arg given, result must be "", not empty
            parmno = MTOKPARM( p );
            MTOKPARMINC( p );
            if( macro_parms != NULL && macro_parms[parmno].arg != NULL && macro_parms[parmno].arg[0] != '\0' ) {
                mtok = BuildString( macro_parms[parmno].arg );
            } else {
                mtok = BuildAToken( T_STRING, "" );
            }
            break;
        case T_MACRO_PARM:
            parmno = MTOKPARM( p );
            MTOKPARMINC( p );
            p2 = p;
            while( MTOK( p2 ) == T_WHITE_SPACE ) {
                MTOKINC( p2 );
            }
            nested->substituting_parms = true;
            if( macro_parms != NULL ) {
                mtok = BuildMTokenList( macro_parms[parmno].arg, NULL );
                /* NB: mtok is now NULL if macro arg was empty */
            } else {
                mtok = BuildAToken( T_WHITE_SPACE, "" );
            }
            if( MTOK( p2 ) != T_MACRO_SHARP_SHARP && prev_token != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, false );
                }
            } else if( mtok == NULL ) {
                mtok = BuildAToken( T_NULL, "P-<placemarker>" );
            }
            nested->substituting_parms = false;
            break;
        case T_MACRO_VAR_PARM:
            parmno = MTOKPARM( p );
            MTOKPARMINC( p );
            p2 = p;
            while( MTOK( p2 ) == T_WHITE_SPACE )
                MTOKINC( p2 );
            nested->substituting_parms = true;
            if( macro_parms != NULL ) {
                if( macro_parms[parmno].arg != NULL ) {
                    mtok = BuildMTokenList( macro_parms[parmno].arg, NULL );
                } else {
                    if( prev_token == T_MACRO_SHARP_SHARP || MTOK( p2 ) == T_MACRO_SHARP_SHARP ) {
                        mtok = BuildAToken( T_MACRO_EMPTY_VAR_PARM, "" );
                    } else {
                        mtok = BuildAToken( T_WHITE_SPACE, "" );
                    }
                }
            } else {
                mtok = BuildAToken( T_WHITE_SPACE, "" );
            }
            if( MTOK( p2 ) != T_MACRO_SHARP_SHARP && prev_token != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, false );
                }
            }
            nested->substituting_parms = false;
            break;
        default:
            mtok = BuildAToken( tok, Tokens[tok] );
            break;
        }
        if( mtok != NULL ) {
            if( mtok->token != T_WHITE_SPACE ) {
                prev_token = mtok->token;
            }
            *ptail = mtok;
            while( *ptail != NULL ) {
                ptail = &(*ptail)->next;
            }
        }
    }
    head = GlueTokens( head );
    return( head );
}

static void markUnexpandableIds( MACRO_TOKEN *head )
{
    NESTED_MACRO    *nested;
    MACRO_TOKEN     *mtok;
    size_t          len;

    for( mtok = head; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token == T_ID ) {
            len = strlen( mtok->data ) + 1;
            for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
                if( memcmp( nested->mentry->macro_name, mtok->data, len ) == 0 ) {
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

static MACRO_TOKEN *MacroExpansion( bool rescanning, MEPTR mentry )
{
    MACRO_ARG       *macro_parms;
    MACRO_TOKEN     *head;
    NESTED_MACRO    *nested;
    TOKEN           tok;

    nested = (NESTED_MACRO *)CMemAlloc( sizeof( NESTED_MACRO ) );
    nested->mentry = mentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = false;
    nested->macro_parms = NULL;
    if( MacroIsSpecial( mentry ) ) {    /* if special macro */
        tok = SpecialMacro( mentry );
        head = BuildAToken( tok, Buffer );
        nested->next = NestedMacros;
        NestedMacros = nested;
    } else {
        macro_parms = CollectParms( mentry );
        nested->next = NestedMacros;
        NestedMacros = nested;
        nested->macro_parms = macro_parms;
        head = BuildMTokenList( (char *)mentry + mentry->macro_defn, macro_parms );
        markUnexpandableIds( head );
    }
    head = AppendToken( head, T_NULL, MACRO_END_STRING );
    return( head );
}

static MACRO_TOKEN *ExpandNestedMacros( MACRO_TOKEN *head, bool rescanning )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    MACRO_TOKEN *old_tokenlist;
    int         i;
    size_t      len;
    char        *buf;
    MEPTR       mentry;

    mtok = head;
    ++MacroDepth;
    prev_tok = NULL;
    for( ; mtok != NULL; ) {
        toklist = NULL;
        if( mtok->token == T_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            buf = Buffer;
            len = 0;
            while( (buf[len] = mtok->data[len]) != '\0' )
                len++;
            CalcHash( buf, len );
            mentry = MacroLookup( buf );
            if( mentry != NULL ) {
                /* this is a macro */
                if( rescanning ) {
                    if( MacroBeingExpanded( mentry ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = TokenList;
                        i = Expandable( mtok->next, false, mentry );
                        switch( i ) {
                        case 0:                 // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case 1:                 // macro is expandable
                            TokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                prev_tok = NULL;
                            }
                            CMemFree( mtok );
                            toklist = MacroExpansion( rescanning, mentry );
                            mtok = TokenList;
                            TokenList = NULL;
                            break;
                        case 2:                 // we skipped over some white space
                            mtok->token = T_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            toklist = BuildAToken( T_WHITE_SPACE, " " );
                            toklist->next = mtok->next;
                            mtok->next = toklist;
                            toklist = NULL;
                            break;
                        }
                    }
                } else {                        // expanding a macro parm
                    if( Expandable( mtok->next, true, mentry ) ) {
                        old_tokenlist = TokenList;
                        TokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            prev_tok = NULL;
                        }
                        CMemFree( mtok );
                        toklist = ExpandNestedMacros( MacroExpansion( false, mentry ), rescanning );
                        mtok = TokenList;
                        TokenList = old_tokenlist;
                    } else {
                        prev_tok = mtok;
                        mtok = mtok->next;      // advance onto next token
                    }
                }
            } else {
                mtok->token = T_SAVED_ID;       // avoid rechecking this ID
                prev_tok = mtok;
                mtok = mtok->next;              // advance onto next token
            }
        } else if( mtok->token == T_NULL ) {
            toklist = mtok->next;
            if( mtok->data[0] == MACRO_END_CHAR ) { // end of a macro
                rescanning = NestedMacros->rescanning;
                DeleteNestedMacro();
                CMemFree( mtok );
                mtok = toklist;
            } else {                            // end of a macro parm
                if( toklist != NULL ) {
                    TokenList = toklist;
                }
                CMemFree( mtok );
                mtok = NULL;                    // will cause us to exit
            }
            toklist = NULL;
        } else {                                // advance onto next token
            prev_tok = mtok;
            mtok = mtok->next;
        }
        if( toklist != NULL ) {                 // new tokens to insert
            if( prev_tok == NULL ) {
                head = toklist;
            } else {
                prev_tok->next = toklist;
            }
            if( mtok != NULL ) {
                while( toklist->next != NULL )
                    toklist = toklist->next;
                toklist->next = mtok;
            }
            if( prev_tok == NULL ) {
                mtok = head;
            } else {
                mtok = prev_tok->next;
            }
        } else {
            // either no change, or tokens were deleted
            if( prev_tok == NULL ) {
                head = mtok;
            } else {
                prev_tok->next = mtok;
            }
        }
    }
    for( mtok = head; mtok != NULL; mtok = mtok->next ) {
        // change a temporarily unexpandable ID into an ID because it
        // could become expandable in a later rescanning phase
        if( mtok->token == T_MACRO ) {
            mtok->token = T_ID;
        }
    }
    --MacroDepth;
    return( head );
}

void DoMacroExpansion( MEPTR mentry )               // called from cscan
{
    MacroDepth = 0;
    TokenList = ExpandNestedMacros( MacroExpansion( false, mentry ), true );
    // GetMacroToken will feed back tokens from the TokenList
    // when the TokenList is exhausted, then revert back to normal scanning
    if( TokenList == NULL ) {
        MacroPtr = NULL;
    } else {
        MacroPtr = "";
    }
}


void InsertReScanPragmaTokens( const char *pragma )
{
    MACRO_TOKEN *toklist;

    toklist = ReTokenBuffer( pragma );
    if( toklist != NULL ) {
        MACRO_TOKEN *old_list;

        old_list = TokenList;
        TokenList = toklist;
        while( toklist->next != NULL ) {
            toklist = toklist->next;
        }
        toklist->next = BuildAToken( T_PRAGMA_END, "" );
        toklist = toklist->next;
        toklist->next = old_list;
        MacroPtr = "";
    }
}

void InsertToken( TOKEN token, const char *str )
{
    MACRO_TOKEN *toklist;

    toklist = BuildAToken( token, str );
    if( toklist != NULL ) {
        toklist->next = TokenList;
        TokenList = toklist;
        MacroPtr = "";
    }
}
