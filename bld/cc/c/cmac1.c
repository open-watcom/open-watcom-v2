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
* Description:  Macro processing, part one.
*
****************************************************************************/


#include "cvars.h"
#include "i64.h"
#include "scan.h"
#include "cfeinfo.h"
#include "dumpapi.h"
#include "cmacadd.h"
#include "ppexpn.h"
#include "cscanbuf.h"

#include "clibext.h"


#define MACRO_END_CHAR          'Z'
#define MACRO_END_STRING        "Z-<end of macro>"
#define MACRO_T_NULL            ""

typedef enum exp_state {
    EXPANDABLE_NO       = 0,    // macro is currently not expandable
    EXPANDABLE_YES      = 1,    // macro is expandable
    EXPANDABLE_WSSKIP   = 2     // we skipped over some white space
} exp_state;

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
    macro_flags     flags;
} special_macro_names;

extern void         DumpMTokens( MACRO_TOKEN *mtok );

static NESTED_MACRO *NestedMacros;
static MACRO_TOKEN  *MTokenList;
static size_t       TokenBufSize;

static MACRO_TOKEN  *ExpandNestedMacros( MACRO_TOKEN *mtok_head, bool rescanning );

static struct special_macro_names  SpcMacros[] = {
    #define pick(s,i,f)     { s, i, f },
    #include "specmac.h"
    #undef pick
};

void InitTokenBuf( size_t size )
/******************************/
{
    TokenBuf = CMemAlloc( size );
    TokenBufSize = size;
}

static void EnlargeTokenBuf( size_t size )
/****************************************/
{
    char    *newBuffer;

//    size += 32;     /* TokenBuf size margin */
    if( size > TokenBufSize ) {
        size = _RoundUp( size, BUF_SIZE );
        newBuffer = CMemAlloc( size );
        memcpy( newBuffer, TokenBuf, TokenBufSize );
        CMemFree( (void *)TokenBuf );
        TokenBuf = newBuffer;
        TokenBufSize = size;
    }
}

static size_t WriteTokenBufToken( size_t i, TOKEN t )
/***************************************************/
{
    EnlargeTokenBuf( MTOKINCR( i ) );
    MTOK( TokenBuf + i ) = t;
    return( MTOKINCR( i ) );
}

static size_t WriteTokenBufChar( size_t i, char c )
/*************************************************/
{
    EnlargeTokenBuf( i + 1 );
    TokenBuf[i++] = c;
    return( i );
}

static size_t WriteTokenBufMem( size_t i, const char *buf, size_t len )
/*********************************************************************/
{
    EnlargeTokenBuf( i + len );
    memcpy( TokenBuf + i, buf, len );
    return( i + len );
}

static size_t TokenBufRemoveWSToken( size_t i )
/*********************************************/
{
    if( i >= MTOKINCR( 0 ) )
        return( MTOKDECR( i ) );
    return( i );
}

static void SpecialMacroAdd( special_macro_names *mac )
/*****************************************************/
{
    MEPTR           mentry;

    mentry = CreateMEntry( mac->name, 0 );
    mentry->parm_count = (mac_parm_count)mac->value;
    MacroDefine( mentry->macro_len, mac->flags );
}

void MacroInit( void )
/********************/
{
    mac_hash_idx    hash;
    int             i;

    MacroCount = 0;
    MacroPtr = NULL;
    CppStackInit();
    InitPPexpn();
    NestedMacros = NULL;
    MTokenList = NULL;
    UndefMacroList = NULL;
    InitialMacroFlags = MFLAG_DEFINED_BEFORE_FIRST_INCLUDE;
    MacHash = PermMemAlloc( MACRO_HASH_SIZE * sizeof( MEPTR ) );
    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        MacHash[hash] = NULL;
    }
    for( i = MACRO_FIRST; i <= MACRO_LAST; i++ ) {
        SpecialMacroAdd( &SpcMacros[i] );
    }
    /*
     * grab time and date for __TIME__ and __DATE__
     */
    TimeInit();
}

void MacroAddComp( void )
/***********************/
{
    int             i;

    for( i = MACRO_COMP_FIRST; i <= MACRO_COMP_LAST; i++ ) {
        SpecialMacroAdd( &SpcMacros[i] );
    }
}

void MacroFini( void )
/********************/
{
    FiniPPexpn();
    CppStackFini();
    MacroPurge();
}


void MacroPurge( void )
/*********************/
{
#if 0
    mac_hash_idx    hash;
    MEPTR           mentry;

    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        for( ; (mentry = MacHash[hash]) != NULL; ) {
            MacHash[hash] = mentry->next_macro;
            CMemFree( mentry );
        }
    }
#endif
}


static void DumpMacroDefn( const char *p )
/****************************************/
{
    unsigned char   c;
    TOKEN           token;

    while( (token = MTOK( p )) != T_NULL ) {
        MTOKINC( p );
        switch( token ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_BAD_TOKEN:
        case T_ID:
        case T_UNEXPANDABLE_ID:
            for( ; (c = *p++) != '\0'; ) {
                fputc( c, CppFile );
            }
            break;
        case T_LSTRING:
            fputc( 'L', CppFile );
            /* fall through */
        case T_STRING:
            fputc( '"', CppFile );
            for( ; (c = *p++) != '\0'; ) {
                fputc( c, CppFile );
            }
            fputc( '"', CppFile );
            break;
        case T_WHITE_SPACE:
            fputc( ' ', CppFile );
            break;
        case T_BAD_CHAR:
            fputc( *p++, CppFile );
            break;
        case T_MACRO_PARM:
        case T_MACRO_VAR_PARM:
            MTOKPARMINC( p );
            break;
        default:
            fprintf( CppFile, "%s", TokenString[token] );
            break;
        }
    }
}


void DumpAllMacros( void )
/************************/
{
    mac_hash_idx    hash;
    MEPTR           mentry;
    const char      *p;

    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        for( mentry = MacHash[hash]; mentry != NULL; mentry = mentry->next_macro ) {
            if( mentry->macro_flags & MFLAG_HIDDEN )
                continue;
            if( MacroIsSpecial( mentry ) )
                continue;
            fprintf( CppFile, "#define %s", mentry->macro_name );
            p = (char *)mentry + mentry->macro_defn;
            if( MTOK( p ) != T_NULL ) {
                fputc( ' ', CppFile );
                DumpMacroDefn( p );
            }
            fputc( '\n', CppFile );
        }
    }
    fflush( CppFile );
}


static void DeleteNestedMacro( void )
/***********************************/
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

TOKEN GetMacroToken( void )
/*************************/
{
    MACRO_TOKEN     *mtok;
    bool            keep_token;
    TOKEN           token;

    token = T_NULL;
    for( ; (mtok = MTokenList) != NULL; ) {
        if( (token = mtok->token) != T_NULL ) {
            break;
        }
        if( mtok->data[0] == MACRO_END_CHAR ) {     // if end of macro
            DeleteNestedMacro();
        }
        MTokenList = mtok->next;
        CMemFree( mtok );
    }
    TokenLen = 0;
    if( mtok == NULL ) {
        MacroPtr = NULL;
        token = T_NULL;
        Buffer[0] = '\0';
    } else {
        /*
         * size of Buffer is OK, token data was processed in Buffer before
         */
        while( (Buffer[TokenLen] = mtok->data[TokenLen]) != '\0' ) {
            TokenLen++;
        }
        keep_token = false;
        switch( token ) {
        case T_SAVED_ID:
            if( CompFlags.doing_macro_expansion ) {
                token = T_ID;
            }
            /* fall through */
        case T_ID:
        case T_UNEXPANDABLE_ID:
            if( !CompFlags.doing_macro_expansion ) {
                if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
                    MTokenList = mtok->next;
                    CMemFree( mtok );
                    token = Process_Pragma();
                    mtok = MTokenList;
                    keep_token = true;
                } else {
                    token = KwLookup( Buffer, TokenLen );
                }
            }
            break;
        case T_BAD_TOKEN:
        case T_CONSTANT:
            ReScanInit( mtok->data );
            token = ReScanToken();
            break;
        case T_PPNUMBER:
            ReScanInit( mtok->data );
            token = ReScanToken();
            if( !CompFlags.rescan_buffer_done ) {   // didn't finish string bad boy
                const char  *tcur;
                char        *tbeg;

                tbeg = mtok->data;
                tcur = ReScanPos();
                /*
                 * ppnumber is quite general so it may absorb multiple tokens
                 * overlapping src & dst so do our own copy;
                 */
                for( ; (*tbeg = *tcur) != '\0'; ) {
                    ++tcur;
                    ++tbeg;
                }
                keep_token = true;
            }
            break;
        case T_LSTRING:
            token = T_STRING;
            CompFlags.wide_char_string = true;
            break;
        case T_STRING:
            CompFlags.wide_char_string = false;
            break;
        }
        if( !keep_token ) {
            MTokenList = mtok->next;
            CMemFree( mtok );
        }
    }
    return( token );
}

static char *expandMacroToken( MACRO_TOKEN *mtok )
/*************************************************
 * returns Dynamically allocated buffer with expanded macro
 */
{
    size_t      len;
    const char  *src;
    char        *dst;
    char        *buf;

    len = 0;
    switch( mtok->token ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:
        return( CMemStrdup( mtok->data ) );
    case T_LSTRING:
        len = 1;
        /* fall through */
    case T_STRING:
        src = mtok->data;
        dst = buf = CMemAlloc( len + strlen( src ) + 3 );
        if( mtok->token == T_LSTRING )
            *dst++ = 'L';
        *dst++ = '"';
        while( (*dst = *src++) != '\0' )
            dst++;
        *dst++ = '"';
        *dst = '\0';
        return( buf );
    default:
        return( CMemStrdup( TokenString[mtok->token] ) );
    }
}

TOKEN SpecialMacro( MEPTR mentry )
/********************************/
{
    const char      *p;

    TokenLen = 0;
    CompFlags.wide_char_string = false;
    switch( (special_macros)mentry->parm_count ) {
    case MACRO_LINE:
        TokenLen = sprintf( Buffer, "%u", TokenLoc.line );
        Set64ValU32( Constant64, TokenLoc.line );
        ConstType = TYP_INT;
        return( T_CONSTANT );
    case MACRO_FILE:
        p = FileIndexToFName( TokenLoc.fno )->name;
        WriteBufferEscStr( &p, false );
        return( T_STRING );
    case MACRO_DATE:
        WriteBufferString( __Date );
        return( T_STRING );
    case MACRO_TIME:
        WriteBufferString( __Time );
        return( T_STRING );
    case MACRO_STDC:
    case MACRO_STDC_HOSTED:
        WriteBufferString( "1" );
        Set64Val1p( Constant64 );
        ConstType = TYP_INT;
        return( T_CONSTANT );
    case MACRO_STDC_VERSION:
        if( CompVars.cstd < STD_C99 ) {
            WriteBufferString( "199409L" );
            Set64ValU32( Constant64, 199409 );
        } else {
            WriteBufferString( "199901L" ); /* C99 */
//            WriteBufferString( "201112L" ); /* C11 */
//            WriteBufferString( "201710L" ); /* C17 */
//            WriteBufferString( "202311L" ); /* C23 */
            Set64ValU32( Constant64, 199901 );  /* C99 */
//            Set64ValU32( Constant64, 201112 );  /* C11 */
//            Set64ValU32( Constant64, 201710 );  /* C17 */
//            Set64ValU32( Constant64, 202311 );  /* C23 */
        }
        ConstType = TYP_LONG;
        return( T_CONSTANT );
    case MACRO_FUNCTION:
    case MACRO_FUNC:
        Buffer[0] = '\0';
        if( CurFunc != NULL && CurFunc->name != NULL ) {
            WriteBufferString( CurFunc->name );
        }
        return( T_STRING );
    default:
        Buffer[0] = '\0';
        return( T_NULL );   // shut up the compiler
    }
}


static TOKEN NextMToken( void )
/*****************************/
{
    TOKEN   token;

    CompFlags.doing_macro_expansion = true;
    token = GetMacroToken();
    if( token == T_NULL ) {
        token = ScanToken();
    }
    CompFlags.doing_macro_expansion = false;
    return( token );
}

static void SaveParm( MEPTR mentry, size_t len, mac_parm_count parmno, MACRO_ARG *macro_parms )
/*********************************************************************************************/
{
    char            *p;

    len = WriteTokenBufToken( len, T_NULL );
    if( parmno < GetMacroParmCount( mentry ) ) {
        p = CMemAlloc( len );
        macro_parms[parmno].arg = p;
        if( p != NULL ) {
            memcpy( p, TokenBuf, len );
        }
    }
}

static MACRO_ARG *CollectParms( MEPTR mentry )
/********************************************/
{
    size_t          len;
    int             bracket;
    TOKEN           token;
    TOKEN           token_prev;
    mac_parm_count  parmno;
    mac_parm_count  parm_count_reqd;
    bool            ppscan_mode;
    MACRO_ARG       *macro_parms;

    macro_parms = NULL;
    if( MacroWithParenthesis( mentry ) ) {     /* if () expected */
        ppscan_mode = InitPPScan();     // enable T_PPNUMBER tokens
        parm_count_reqd = GetMacroParmCount( mentry );
        if( parm_count_reqd > 0 ) {
            macro_parms = (MACRO_ARG *)CMemAlloc( parm_count_reqd * sizeof( MACRO_ARG ) );
        }
        parmno = 0;
        do {
            token = NextMToken();
        } while( token == T_WHITE_SPACE );
        /*
         * token will now be a '('
         */
        bracket = 0;
        len = 0;
        for( ;; ) {
            token_prev = token;
            do {
                token = NextMToken();
            } while( token == T_WHITE_SPACE && len == 0 );
            if( token == T_EOF || token == T_NULL ) {
                CErr2p( ERR_INCOMPLETE_MACRO, mentry->macro_name );
                break;
            }
            if( token == T_BAD_TOKEN && BadTokenInfo == ERR_MISSING_QUOTE ) {
                CErr1( ERR_MISSING_QUOTE );
                token = T_RIGHT_PAREN;
            }
            if( token == T_LEFT_PAREN ) {
                ++bracket;
            } else if( token == T_RIGHT_PAREN ) {
                if( bracket == 0 )
                    break;
                --bracket;
            } else if( token == T_STRING ) {
                if( CompFlags.wide_char_string ) {
                    token = T_LSTRING;
                }
            } else if( token == T_COMMA && bracket == 0
              && ( !MacroHasVarArgs( mentry ) || parmno != parm_count_reqd - 1 ) ) {
                if( token_prev == T_WHITE_SPACE ) {
                    len = TokenBufRemoveWSToken( len );
                }
                if( macro_parms != NULL ) {     // if expecting parms
                    SaveParm( mentry, len, parmno, macro_parms );
                }
                ++parmno;
                len = 0;
                continue;
            }
            len = WriteTokenBufToken( len, token );
            switch( token ) {
            case T_WHITE_SPACE:
                if( token_prev == T_WHITE_SPACE ) {
                    len = TokenBufRemoveWSToken( len );
                }
                break;
            case T_BAD_CHAR:
                len = WriteTokenBufChar( len, Buffer[0] );
                if( Buffer[1] != '\0' ) {
                    len = WriteTokenBufToken( len, T_WHITE_SPACE );
                }
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_LSTRING:
            case T_STRING:
            case T_BAD_TOKEN:
                len = WriteTokenBufMem( len, Buffer, TokenLen + 1 );
                break;
            default:
                break;
            }
        }
        if( token_prev == T_WHITE_SPACE ) {
            len = TokenBufRemoveWSToken( len );
        }
        if( macro_parms != NULL ) {     // if expecting parms
            SaveParm( mentry, len, parmno, macro_parms );
            ++parmno;
        } else if( len != 0 ) {
            ++parmno;                   // will cause "too many parms" error
        }
        if( MacroHasVarArgs( mentry ) && parmno < ( parm_count_reqd - 1 )
          || !MacroHasVarArgs( mentry ) && parmno < parm_count_reqd ) {
            CErr2p( ERR_TOO_FEW_MACRO_PARMS, mentry->macro_name );
        } else if( !MacroHasVarArgs( mentry ) && parmno > parm_count_reqd ) {
            if( parm_count_reqd > 0 ) {
                CWarn2p( ERR_TOO_MANY_MACRO_PARMS, mentry->macro_name  );
            }
        } else if( strcmp( mentry->macro_name, "va_start" ) == 0 ) {
            if( SymLevel != 0 && !VarParm( CurFunc ) ) {
                CErr1( ERR_MUST_BE_VAR_PARM_FUNC );
            }
        }
        FiniPPScan( ppscan_mode );      // disable T_PPNUMBER tokens
    }
    return( macro_parms );
}


#ifdef DEVBUILD

void DumpMDefn( const char *p )
/*****************************/
{
    unsigned char   c;
    TOKEN           token;

    while( (token = MTOK( p )) != T_NULL ) {
        MTOKINC( p );
        switch( token ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            break;
        case T_LSTRING:
            putchar( 'L' );
            /* fall through */
        case T_STRING:
            putchar( '\"' );
            for( ; (c = *p++) != '\0'; ) {
                putchar( c );
            }
            putchar( '\"' );
            break;
        case T_WHITE_SPACE:
            putchar( ' ' );
            break;
        case T_BAD_CHAR:
            putchar( *p++ );
            break;
        case T_MACRO_PARM:
            printf( "<parm#%d>", MTOKPARM( p ) );
            MTOKPARMINC( p );
            break;
        case T_MACRO_VAR_PARM:
            printf( "<varparm#%d>", MTOKPARM( p ) );
            MTOKPARMINC( p );
            break;
        default:
            printf( "%s", TokenString[token] );
            break;
        }
    }
    putchar( '\n' );
    fflush( stdout );
}


void DumpMTokens( MACRO_TOKEN *mtok )
/***********************************/
{
    for( ; mtok != NULL; mtok = mtok->next ) {
        printf( "%s\n", mtok->data );
    }
    fflush( stdout );
}


void DumpNestedMacros( void )
/***************************/
{
    NESTED_MACRO *nested;

    for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
        printf( "%s\n", nested->mentry->macro_name );
    }
    fflush( stdout );
}
#endif


static MACRO_TOKEN *BuildAToken( TOKEN token, const char *data )
/**************************************************************/
{
    MACRO_TOKEN *mtok;

    mtok = (MACRO_TOKEN *)CMemAlloc( sizeof( MACRO_TOKEN ) + strlen( data ) );
    strcpy( mtok->data, data );
    mtok->token = token;
    mtok->next = NULL;
    return( mtok );
}


static MACRO_TOKEN **BuildATokenOnEnd( MACRO_TOKEN **pmtok_tail, TOKEN token, const char *data )
/*****************************************************************************************/
{
    *pmtok_tail = BuildAToken( token, data );
    return( &(*pmtok_tail)->next );
}


static MACRO_TOKEN *AppendToken( MACRO_TOKEN *mtok_head, TOKEN token, const char *data )
/**************************************************************************************/
{
    MACRO_TOKEN *mtok_tail;
    MACRO_TOKEN *mtok;

    mtok = BuildAToken( token, data );
    if( mtok_head == NULL ) {
        mtok_head = mtok;
    } else {
        mtok_tail = mtok_head;
        while( mtok_tail->next != NULL ) {
            mtok_tail = mtok_tail->next;
        }
        mtok_tail->next = mtok;
    }
    return( mtok_head );
}

static bool MacroBeingExpanded( MEPTR mentry )
/********************************************/
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

static exp_state isExpandable( MEPTR mentry, MACRO_TOKEN *mtok, bool macro_parm )
/*******************************************************************************/
{
    int         lparen;

    if( MacroIsSpecial( mentry ) ) {            /* if special macro */
        return( EXPANDABLE_YES );
    }
    if( !MacroWithParenthesis( mentry ) ) {     /* if () not expected */
        if( macro_parm ) {
            if( MacroBeingExpanded( mentry ) ) {
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
            if( MacroDepth == 1 && !macro_parm )
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
        } else if( CompFlags.cpp_mode ) {
            return( EXPANDABLE_WSSKIP );
        }
    }
    return( EXPANDABLE_NO );
}

static char *GlueTokenToBuffer( MACRO_TOKEN *mtok_first, char *gluebuf )
/*****************************************************************/
{
    size_t      gluelen;
    size_t      tokenlen;
    char        *buf;

    buf = NULL;
    if( mtok_first != NULL ) {
        buf = expandMacroToken( mtok_first );
    }
    if( buf == NULL ) {
        buf = gluebuf;
    } else if( gluebuf != NULL ) {
        /*
         * now do a "strcat( gluebuf, buf )"
         */
        gluelen = strlen( gluebuf );
        tokenlen = strlen( buf );
        gluebuf = CMemRealloc( gluebuf, gluelen + tokenlen + 1 );
        strcpy( gluebuf + gluelen, buf );
        CMemFree( buf );
        buf = gluebuf;
    }
    return( buf );
}

static MACRO_TOKEN *ReTokenBuffer( const char *buffer )
/******************************************************
 * retokenize starting at buffer
 */
{
    MACRO_TOKEN *mtok_head;
    MACRO_TOKEN **pmtok_tail;
    bool        ppscan_mode;

    ppscan_mode = InitPPScan();
    ReScanInit( buffer );
    mtok_head = NULL;
    pmtok_tail = &mtok_head;
    for( CompFlags.rescan_buffer_done = false; !CompFlags.rescan_buffer_done; ) {
        pmtok_tail = BuildATokenOnEnd( pmtok_tail, ReScanToken(), Buffer );
    }
    FiniPPScan( ppscan_mode );
    return( mtok_head );
}


static MACRO_TOKEN *GlueTokens( MACRO_TOKEN *mtok_head )
/******************************************************/
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN **pmtok_tail;
    MACRO_TOKEN **pmtok_tail_prev;   // prior pmtok_tail
    MACRO_TOKEN *mtok_next;
    char        *buf;
    char        *gluebuf;

    gluebuf = NULL;
    pmtok_tail_prev = NULL;
    pmtok_tail = &mtok_head;
    mtok = *pmtok_tail;
    buf = Buffer;
    for( ; mtok != NULL; ) {
        if( mtok->token == T_WHITE_SPACE ) {
            pmtok_tail_prev = pmtok_tail;
            pmtok_tail = &mtok->next;
            mtok = *pmtok_tail;
        } else if( mtok->next == NULL ) {
            break;
        } else {
            mtok_next = mtok->next;
            if( mtok_next->token == T_WHITE_SPACE )
                mtok_next = mtok_next->next;
            if( mtok_next == NULL )
                break;
            if( mtok_next->token != T_MACRO_SHARP_SHARP ) {
                pmtok_tail_prev = pmtok_tail;
                pmtok_tail = &mtok->next;
                mtok = *pmtok_tail;
            } else {    // T_MACRO_SHARP_SHARP
                int         pos;
                MACRO_TOKEN *rem;

                mtok_next = mtok_next->next;
                pos = 0;
                /*
                 * glue mtok->token with mtok_next->token to make one token
                 */
                if( ( mtok->token == T_COMMA && mtok_next->token == T_MACRO_EMPTY_VAR_PARM )
                  || ( mtok->token == T_MACRO_EMPTY_VAR_PARM && mtok_next->token == T_COMMA ) ) {
                    /*
                     * delete [mtoken(a comma),##,empty __VA_ARGS__]
                     * delete [empty __VA_ARGS__,##,mtoken(a comma)]
                     */
                    buf[10] = '\0';
                    pos = -1;
                } else {
                    if( mtok->token == T_MACRO_EMPTY_VAR_PARM ) {
                        /*
                         * well should never be in this state if no mtok_next - since ## cannot
                         * appear at the end of a macro
                         */
                        gluebuf = GlueTokenToBuffer( mtok_next, NULL );
                    } else {
                        gluebuf = GlueTokenToBuffer( mtok, NULL );
                        if( mtok_next != NULL && mtok_next->token != T_MACRO_EMPTY_VAR_PARM ) {
                            gluebuf = GlueTokenToBuffer( mtok_next, gluebuf ); //paste in mtok_next
                        }
                    }
                }
                if( mtok_next != NULL ) {
                    rem = mtok_next->next;   // save unseen stuff
                    mtok_next->next = NULL;  // break link;
                } else {
                    rem = NULL;         // happens if no arg after comma
                }
                while( mtok != NULL ) { // free old stuff [mtoken,##,{atok,} mtok_next]
                    mtok_next = mtok->next;
                    CMemFree( mtok );
                    mtok = mtok_next;
                }
                if( pos >= 0 ) {
                    if( gluebuf != NULL ) {
                        mtok = ReTokenBuffer( gluebuf );
                        CMemFree( gluebuf );
                    } else {
                        /*
                         * Both ends of ## were empty
                         */
                        mtok = BuildAToken( T_NULL, "P-<placemarker>" );
                    }
                    *pmtok_tail = mtok;  // link in new mtok to mtok's link
                    while( mtok != NULL && mtok->next != NULL ) {   //position mtok & pmtok_tail to last token
                        pmtok_tail_prev = pmtok_tail;
                        pmtok_tail = &mtok->next;
                        mtok = *pmtok_tail;
                        if( mtok == mtok->next ) {
                            return( mtok_head );
                        }
                    }
                    /*
                     * mtok == last token of retokenizing
                     * pmtok_tail == the pointer which references mtok
                     */
                    mtok->next = rem;
                } else {
                    if( pmtok_tail_prev != NULL ) {
                        *pmtok_tail = rem;
                        pmtok_tail = pmtok_tail_prev;
                        mtok = *pmtok_tail;
                    } else {
                        *pmtok_tail = rem;
                        mtok = mtok_head;
                    }
                }
            }
        }
    }
    return( mtok_head );
}

static MACRO_TOKEN *BuildString( const char *p )
/**********************************************/
{
    MACRO_TOKEN     *mtok_head;
    MACRO_TOKEN     **pmtok_tail;
    char            c;
    TOKEN           tok;

    mtok_head = NULL;
    if( p != NULL ) {
        pmtok_tail = &mtok_head;
        TokenLen = 0;
        while( MTOK( p ) == T_WHITE_SPACE ) {
            MTOKINC( p );   //eat leading white space
        }
        while( (tok = MTOK( p )) != T_NULL ) {
            MTOKINC( p );
            switch( tok ) {
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:
                for( ; (c = *p++) != '\0'; ) {
                    if( c == '\\' )
                        WriteBufferChar( c );
                    WriteBufferChar( c );
                }
                break;
            case T_LSTRING:
                WriteBufferChar( 'L' );
                /* fall through */
            case T_STRING:
                WriteBufferChar( '\\' );
                WriteBufferChar( '"' );
                for( ; (c = *p++) != '\0'; ) {
                    if( c == '\\' || c == '"' )
                        WriteBufferChar( '\\' );
                    WriteBufferChar( c );
                }
                WriteBufferChar( '\\' );
                WriteBufferChar( '"' );
                break;
            case T_WHITE_SPACE:
                while( (tok = MTOK( p )) == T_WHITE_SPACE ) {
                    MTOKINC( p );
                }
                if( tok != T_NULL ) {
                    WriteBufferChar( ' ' );
                }
                break;
            case T_BAD_CHAR:
                if( *p == '\\' && MTOK( p + 1 ) == T_NULL ) {
                    CErr1( ERR_INVALID_STRING_LITERAL );
                }
                WriteBufferChar( *p++ );
                break;
            default:
                WriteBufferString( TokenString[tok] );
                break;
            }
        }
        if( TokenLen > 0 ) {
            WriteBufferNullChar();
            pmtok_tail = BuildATokenOnEnd( pmtok_tail, T_STRING, Buffer );
        }
    }
    return( mtok_head );
}


static MACRO_TOKEN *BuildMTokenList( const char *p, MACRO_ARG *macro_parms )
/**************************************************************************/
{
    MACRO_TOKEN     *mtok;
    MACRO_TOKEN     *mtok_head;
    MACRO_TOKEN     **pmtok_tail;
    NESTED_MACRO    *nested;
    const char      *p2;
    char            buf[2];
    TOKEN           token_prev;
    TOKEN           tok;
    mac_parm_count  parmno;

    mtok_head = NULL;
    pmtok_tail = &mtok_head;
    nested = NestedMacros;
    buf[1] = '\0';
    token_prev = T_NULL;
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
            if( token_prev == T_MACRO_SHARP_SHARP )
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
            /*
             * If no macro arg given, result must be "", not empty
             */
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
                /*
                 * NB: mtok is now NULL if macro arg was empty
                 */
            } else {
                mtok = BuildAToken( T_WHITE_SPACE, "" );
            }
            if( MTOK( p2 ) != T_MACRO_SHARP_SHARP && token_prev != T_MACRO_SHARP_SHARP ) {
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
                    if( token_prev == T_MACRO_SHARP_SHARP || MTOK( p2 ) == T_MACRO_SHARP_SHARP ) {
                        mtok = BuildAToken( T_MACRO_EMPTY_VAR_PARM, "" );
                    } else {
                        mtok = BuildAToken( T_WHITE_SPACE, "" );
                    }
                }
            } else {
                mtok = BuildAToken( T_WHITE_SPACE, "" );
            }
            if( MTOK( p2 ) != T_MACRO_SHARP_SHARP && token_prev != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, false );
                }
            }
            nested->substituting_parms = false;
            break;
        default:
            mtok = BuildAToken( tok, TokenString[tok] );
            break;
        }
        if( mtok != NULL ) {
            if( mtok->token != T_WHITE_SPACE ) {
                token_prev = mtok->token;
            }
            *pmtok_tail = mtok;
            while( *pmtok_tail != NULL ) {
                pmtok_tail = &(*pmtok_tail)->next;
            }
        }
    }
    mtok_head = GlueTokens( mtok_head );
    return( mtok_head );
}

static void markUnexpandableIds( MACRO_TOKEN *mtok_head )
/*******************************************************/
{
    NESTED_MACRO    *nested;
    MACRO_TOKEN     *mtok;

    for( mtok = mtok_head; mtok != NULL; mtok = mtok->next ) {
        if( mtok->token == T_ID ) {
            for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
                if( strcmp( nested->mentry->macro_name, mtok->data ) == 0 ) {
                    if( !nested->substituting_parms ) {
                        /*
                         * change token so it won't be considered a
                         * candidate as a macro
                         */
                        mtok->token = T_UNEXPANDABLE_ID;
                        break;
                    }
                }
            }
        }
    }
}

static MACRO_TOKEN *MacroExpansion( bool rescanning, MEPTR mentry )
/*****************************************************************/
{
    MACRO_ARG       *macro_parms;
    MACRO_TOKEN     *mtok_head;
    NESTED_MACRO    *nested;

    nested = (NESTED_MACRO *)CMemAlloc( sizeof( NESTED_MACRO ) );
    nested->mentry = mentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = false;
    nested->macro_parms = NULL;
    if( MacroIsSpecial( mentry ) ) {    /* if special macro */
        mtok_head = BuildAToken( SpecialMacro( mentry ), Buffer );
        nested->next = NestedMacros;
        NestedMacros = nested;
    } else {
        macro_parms = CollectParms( mentry );
        nested->next = NestedMacros;
        NestedMacros = nested;
        nested->macro_parms = macro_parms;
        mtok_head = BuildMTokenList( (char *)mentry + mentry->macro_defn, macro_parms );
        markUnexpandableIds( mtok_head );
    }
    mtok_head = AppendToken( mtok_head, T_NULL, MACRO_END_STRING );
    return( mtok_head );
}

static MACRO_TOKEN *ExpandNestedMacros( MACRO_TOKEN *mtok_head, bool rescanning )
/*******************************************************************************/
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    MACRO_TOKEN *mtok_save;
    exp_state   i;
    size_t      len;
    char        *buf;
    MEPTR       mentry;

    mtok = mtok_head;
    ++MacroDepth;
    prev_tok = NULL;
    for( ; mtok != NULL; ) {
        toklist = NULL;
        if( mtok->token == T_ID ) {
            /*
             * if macro and not being expanded, then expand it
             * only tokens available for expansion are those in mtok list
             */
            buf = Buffer;
            len = 0;
            while( (buf[len] = mtok->data[len]) != '\0' )
                len++;
            mentry = MacroLookup( buf );
            if( mentry != NULL ) {
                /*
                 * this is a macro
                 */
                if( rescanning ) {
                    if( MacroBeingExpanded( mentry ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = MTokenList;
                        i = isExpandable( mentry, mtok->next, false );
                        switch( i ) {
                        case EXPANDABLE_NO:     // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case EXPANDABLE_YES:    // macro is expandable
                            MTokenList = mtok->next;
                            if( mtok_head == mtok ) {
                                mtok_head = NULL;
                                prev_tok = NULL;
                            }
                            CMemFree( mtok );
                            toklist = MacroExpansion( rescanning, mentry );
                            mtok = MTokenList;
                            MTokenList = NULL;
                            break;
                        case EXPANDABLE_WSSKIP: // we skipped over some white space
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
                    i = isExpandable( mentry, mtok->next, true );
                    switch( i ) {
                    case EXPANDABLE_NO:         // macro is currently not expandable
                        prev_tok = mtok;
                        mtok = mtok->next;      // advance onto next token
                        break;
                    case EXPANDABLE_YES:        // macro is expandable
                    case EXPANDABLE_WSSKIP:     // we skipped over some white space
                        mtok_save = MTokenList;
                        MTokenList = mtok->next;
                        if( mtok_head == mtok ) {
                            mtok_head = NULL;
                            prev_tok = NULL;
                        }
                        CMemFree( mtok );
                        toklist = ExpandNestedMacros( MacroExpansion( false, mentry ), rescanning );
                        mtok = MTokenList;
                        MTokenList = mtok_save;
                        break;
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
                    MTokenList = toklist;
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
                mtok_head = toklist;
            } else {
                prev_tok->next = toklist;
            }
            if( mtok != NULL ) {
                while( toklist->next != NULL )
                    toklist = toklist->next;
                toklist->next = mtok;
            }
            if( prev_tok == NULL ) {
                mtok = mtok_head;
            } else {
                mtok = prev_tok->next;
            }
        } else {
            /*
             * either no change, or tokens were deleted
             */
            if( prev_tok == NULL ) {
                mtok_head = mtok;
            } else {
                prev_tok->next = mtok;
            }
        }
    }
    for( mtok = mtok_head; mtok != NULL; mtok = mtok->next ) {
        /*
         * change a temporarily unexpandable ID into an ID because it
         * could become expandable in a later rescanning phase
         */
        if( mtok->token == T_MACRO ) {
            mtok->token = T_ID;
        }
    }
    --MacroDepth;
    return( mtok_head );
}

void DoMacroExpansion( MEPTR mentry )               // called from cscan
/***********************************/
{
    MacroDepth = 0;
    MTokenList = ExpandNestedMacros( MacroExpansion( false, mentry ), true );
    /*
     * GetMacroToken will feed back tokens from the MTokenList
     * when the MTokenList is exhausted, then revert back to normal scanning
     */
    if( MTokenList == NULL ) {
        MacroPtr = NULL;
    } else {
        MacroPtr = MACRO_T_NULL;
    }
}


void InsertReScanPragmaTokens( const char *pragma )
/*************************************************/
{
    MACRO_TOKEN *toklist;

    toklist = ReTokenBuffer( pragma );
    if( toklist != NULL ) {
        MACRO_TOKEN *mtok_save;

        mtok_save = MTokenList;
        MTokenList = toklist;
        while( toklist->next != NULL ) {
            toklist = toklist->next;
        }
        toklist->next = BuildAToken( T_PRAGMA_END, "" );
        toklist = toklist->next;
        toklist->next = mtok_save;
        MacroPtr = MACRO_T_NULL;
    }
}

void InsertToken( TOKEN token, const char *str )
/**********************************************/
{
    MACRO_TOKEN *toklist;

    toklist = BuildAToken( token, str );
    if( toklist != NULL ) {
        toklist->next = MTokenList;
        MTokenList = toklist;
        MacroPtr = MACRO_T_NULL;
    }
}
