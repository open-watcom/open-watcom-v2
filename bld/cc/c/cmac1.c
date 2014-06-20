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


#include "cvars.h"
#include "scan.h"
#include "pragdefn.h"
#include "cfeinfo.h"


#define T_UNEXPANDABLE_ID       T_LAST_TOKEN

struct  tokens {
    struct  tokens  *next;
    int             length;
    char            buf[1];
};

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
    char                    rescanning;
    char                    substituting_parms;
} NESTED_MACRO;

static NESTED_MACRO *NestedMacros;
static MACRO_TOKEN  *TokenList;
static int          MTokenLen;              /* macro token length */
static MACRO_TOKEN  *MacroExpansion( int );
static MACRO_TOKEN  *NestedMacroExpansion( int );

static int MacroBeingExpanded( MEPTR mentry );

local void SaveParm( MEPTR      mentry,
                     size_t     size,
                     int        parm_cnt,
                     MACRO_ARG  *macro_parms,
                     struct tokens *token_list );

struct special_macro_names {
    char    *name;
    int     value;
 };
static struct special_macro_names  SpcMacros[] = {
    { "__DATE__",           MACRO_DATE          },
    { "__FILE__",           MACRO_FILE          },
    { "__LINE__",           MACRO_LINE          },
    { "__STDC__",           MACRO_STDC          },
    { "__STDC_HOSTED__",    MACRO_STDC_HOSTED   },
    { "__STDC_LIB_EXT1__",  MACRO_STDC_LIB_EXT1 },
    { "__STDC_VERSION__",   MACRO_STDC_VERSION  },
    { "__TIME__",           MACRO_TIME          },
    { NULL,                 0                   }
 };


void MacroInit( void )
{
    mac_hash_idx    h;
    struct special_macro_names *mac;
    MEPTR           mentry;

    MTokenLen = 0;
    MacroCount = 0;
    MacroPtr = NULL;
    CppStackInit();
    NestedMacros = NULL;
    TokenList = NULL;
    UndefMacroList = NULL;                              /* 26-may-94 */
    InitialMacroFlag = MFLAG_DEFINED_BEFORE_FIRST_INCLUDE;
    AllocMacroSegment( 0x1000 );
    MacHash = (MEPTR *)MacroSegment;
    MacroOffset = MacroSegment + MACRO_HASH_SIZE * sizeof(MEPTR);
    for( h = 0; h < MACRO_HASH_SIZE; ++h ) {
        MacHash[h] = NULL;
    }
    for( mac = SpcMacros; mac->name != NULL; ++mac ) {
        mentry = CreateMEntry( mac->name );
        mentry->parm_count = mac->value;
        MacroAdd( mentry, NULL, 0, MFLAG_NONE );
        FreeMEntry( mentry );
    }
    TimeInit(); /* grab time and date for __TIME__ and __DATE__ */
}

static struct special_macro_names  SpcMacroCompOnly[] = {
    { "__FUNCTION__",   MACRO_FUNC },
    { "__func__",       MACRO_FUNC },
    { NULL,             0 }
};

void MacroAddComp( void )
{
    struct special_macro_names *mac;
    MEPTR       mentry;

    for( mac = SpcMacroCompOnly; mac->name != NULL; ++mac ) {
        mentry = CreateMEntry( mac->name );
        mentry->parm_count = mac->value;
        MacroAdd( mentry, NULL, 0, MFLAG_NONE );
        FreeMEntry( mentry );
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
    unsigned char   argno;

    nested = NestedMacros;
    if( nested != NULL ) {
        NestedMacros = nested->next;
        macro_parms =  nested->macro_parms;
        mentry = nested->mentry;
        CMemFree( nested );
        argno = mentry->parm_count - 1;
        if( argno > 0  &&  macro_parms != NULL ) {
            do {
                --argno;
                CMemFree( macro_parms[argno].arg );
            } while( argno != 0 );
            CMemFree( macro_parms );
        }
    }
}

void GetMacroToken( void )
{
    MACRO_TOKEN     *mtok;
    unsigned        len;
    char            *buf;
    struct {
        unsigned keep_token : 1;
        unsigned next_token : 1;
    } flag;

    buf = Buffer;
    CurToken = T_NULL;
    for( ;; ) {
        flag.keep_token = FALSE;
        flag.next_token = FALSE;
        mtok = TokenList;
        if( mtok == NULL ) {
            MacroPtr = NULL;
            break;
        }
        CurToken = mtok->token;
        len = 0;
        while( (buf[len] = mtok->data[len]) != '\0' ) {
            len++;
        }
        switch( CurToken ) {
        case T_UNEXPANDABLE_ID:
            CalcHash( buf, len );
            if( !CompFlags.doing_macro_expansion ) {
                CurToken = KwLookup( buf, len );
            }
            break;
        case T_ID:
        case T_SAVED_ID:
            CalcHash( buf, len );
            if( CompFlags.doing_macro_expansion ) {
                CurToken = T_ID;
            } else {
                CurToken = KwLookup( buf, len );
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
                char    *tcur;
                char    *tbeg;

                tbeg = mtok->data;
                tcur = ReScanPos();
                // ppnumber is quite general so it may absorb multiple tokens
                // overlapping src & dst so do our own copy;
                for( ;; ) {
                    *tbeg = *tcur;
                    if( *tcur == '\0' )
                        break;
                    ++tcur;
                    ++tbeg;
                }
                flag.keep_token = TRUE;
            }
            break;
        case T_LSTRING:
            CurToken = T_STRING;
            CompFlags.wide_char_string = 1;
            CLitLength = len + 1;
            break;
        case T_STRING:
            CompFlags.wide_char_string = 0;
            CLitLength = len + 1;
            break;
        case T_BAD_CHAR:
            break;
        case T_NULL:
            if( mtok->data[0] == 'Z' ) {    // if end of macro
                DeleteNestedMacro();
            }
            flag.next_token = TRUE;
            break;
        default:
            break;
        }
        if( !flag.keep_token ) {
            TokenList = mtok->next;
            CMemFree( mtok );
        }
        if( !flag.next_token ) {
            break;
        }
    }
}

/* returns Dynamically allocated buffer with expanded macro */
local char *ExpandMacroToken( void )
{
    size_t      i, len;
    char        *p;
    char        *buf;
    TOKEN       tok;

    tok = *(TOKEN *)MacroPtr;
    if( tok == T_NULL )
        return( NULL );
    MacroPtr += sizeof( TOKEN );
    p = NULL;
    len = 0;
    switch( tok ) {
    case T_CONSTANT:
    case T_PPNUMBER:
    case T_ID:
    case T_UNEXPANDABLE_ID:
    case T_SAVED_ID:
    case T_BAD_TOKEN:                                   /* 07-apr-91 */
        p = MacroPtr;
        len = strlen( p );
        MacroPtr += len;
        break;
    case T_LSTRING:                                     /* 15-may-92 */
        len = 1;
    case T_STRING:                                      /* 15-dec-91 */
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
    default:                                            /* 28-mar-90 */
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


int SpecialMacro( MEPTR mentry )
{
    char        *p;
    char        *bufp;

    CompFlags.wide_char_string = 0;                     /* 16-dec-91 */
    switch( mentry->parm_count ) {
    case MACRO_LINE:
        ultoa( TokenLoc.line, Buffer, 10 );
        Constant = TokenLoc.line;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_FILE:
        p = FileIndexToFName( TokenLoc.fno )->name;
        bufp = Buffer;
        for( ;; ) {
            *bufp++ = *p;
            if( *p == '\0' )
                break;
            if( *p == '\\' )
                *bufp++ = '\\';
            ++p;
        }
        CLitLength = bufp - Buffer;
        return( T_STRING );
    case MACRO_DATE:
        CLitLength = 12;
        strcpy( Buffer, __Date );
        return( T_STRING );
    case MACRO_TIME:
        CLitLength = 9;
        strcpy( Buffer, __Time );
        return( T_STRING );
    case MACRO_STDC:
        Buffer[0] = '1';
        Buffer[1] = '\0';
        Constant = 1;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_STDC_HOSTED:
        Buffer[0] = '1';
        Buffer[1] = '\0';
        Constant = 1;
        ConstType = TYPE_INT;
        return( T_CONSTANT );
    case MACRO_STDC_LIB_EXT1:
        strcpy( Buffer, "200509L" );
        Constant = 200509;
        ConstType = TYPE_LONG;
        return( T_CONSTANT );
    case MACRO_STDC_VERSION:
        if( CompFlags.c99_extensions ) {
            strcpy( Buffer, "199901L" );
            Constant = 199901;
        } else {
            strcpy( Buffer, "199409L" );
            Constant = 199409;
        }
        ConstType = TYPE_LONG;
        return( T_CONSTANT );
    case MACRO_FUNC:
        p = "";
        if( CurFunc != NULL ) {
            if( CurFunc->name != NULL ) {
                p = CurFunc->name;
            }
        }
        CLitLength = strlen( p ) + 1;
        strcpy( Buffer, p );
        return( T_STRING );
    default:
        return( 0 ); // shut up the compiler
    }
}


local TOKEN NextMToken( void )
{
    CompFlags.doing_macro_expansion = 1;
    GetMacroToken();
    if( CurToken == T_NULL ) {
        CurToken = ScanToken();
    }
    CompFlags.doing_macro_expansion = 0;
    return( CurToken );
}

void EnlargeBuffer( size_t size )
{
    char       *newBuffer;

    newBuffer = CMemAlloc( size );
    memcpy( newBuffer, Buffer, BufSize );
    CMemFree( Buffer );
    Buffer = newBuffer;
    newBuffer = CMemAlloc( size );
    memcpy( newBuffer, TokenBuf, BufSize );
    CMemFree( TokenBuf );
    TokenBuf = newBuffer;
    BufSize = size;
}

static MACRO_ARG *CollectParms(void)
{
    MEPTR           mentry;
    size_t          len;
    int             bracket;
    TOKEN           tok;
    TOKEN           prev_tok;
    TOKEN           *p_token;
    int             parm_cnt;
    int             ppscan_mode;
    MACRO_ARG       *macro_parms;
//    struct tokens   **token_tail;
    struct tokens   *token_head;

    macro_parms = NULL;
    mentry = NextMacro;
    if( mentry->parm_count != 0 ) {     /* if() expected */
        ppscan_mode = InitPPScan();     // enable T_PPNUMBER tokens
        if( mentry->parm_count > 1 ) {
            macro_parms = (MACRO_ARG *)CMemAlloc( (mentry->parm_count - 1) *
                                    sizeof( MACRO_ARG ) );
        }
        parm_cnt = 0;
        do {
            tok = NextMToken();
        } while( tok == T_WHITE_SPACE );
        /* tok will now be a '(' */
        bracket = 0;
//        token_tail = &token_head;
        token_head = NULL;
        MTokenLen = 0;
        for( ;; ) {
            prev_tok = tok;
            do {
                tok = NextMToken();
                if( tok != T_WHITE_SPACE ) {
                    break;
                }
            } while( MTokenLen == 0 );
            if( tok == T_EOF || tok == T_NULL ) {       /* 08-dec-94 */
                CErr2p( ERR_INCOMPLETE_MACRO, mentry->macro_name );
                break;
            }
            if( tok == T_BAD_TOKEN &&                   /* 18-feb-90 */
                BadTokenInfo == ERR_MISSING_QUOTE ) {
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
              && !( (mentry->macro_flags & MFLAG_VAR_ARGS ) && parm_cnt == mentry->parm_count - 2 ) ) {
                if( prev_tok == T_WHITE_SPACE ) {
                    MTokenLen -= sizeof( TOKEN );
                }
                if( macro_parms != NULL ) {     // if expecting parms
                    SaveParm( mentry, MTokenLen, parm_cnt, macro_parms, token_head );
                }
                ++parm_cnt;
//                token_tail = &token_head;
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
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_STRING:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                len += strlen( Buffer );
                len++;
                break;
            default:
                break;
            }
            if( MTokenLen + len >= BufSize ) { /* if not enough space */
                EnlargeBuffer( ( MTokenLen + len ) * 2 );
            }
            if( tok == T_STRING  &&  CompFlags.wide_char_string ) {
                tok = T_LSTRING;
            }
            p_token = (TOKEN *)&TokenBuf[MTokenLen];
            *p_token = tok;
            MTokenLen += sizeof( TOKEN );
            switch( tok ) {
            case T_WHITE_SPACE:
                if( prev_tok == T_WHITE_SPACE ) {
                    MTokenLen -= sizeof( TOKEN );
                }
                break;
            case T_BAD_CHAR:
                TokenBuf[MTokenLen++] = Buffer[0];
                if( Buffer[1] != '\0' ) {
                     *(TOKEN *)&TokenBuf[MTokenLen] = T_WHITE_SPACE;
                     MTokenLen += sizeof( TOKEN );
                }
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_LSTRING:
            case T_STRING:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                len = 0;
                while( (TokenBuf[MTokenLen++] = Buffer[len++]) != '\0' )
                    /* empty */;
                break;
            default:
                break;
            }
        }
        if( prev_tok == T_WHITE_SPACE ) {
            MTokenLen -= sizeof( TOKEN );
        }
        if( macro_parms != NULL ) {     // if expecting parms
            SaveParm( mentry, MTokenLen, parm_cnt, macro_parms, token_head );
            ++parm_cnt;
        } else if( MTokenLen != 0 ) {
            ++parm_cnt;                 // will cause "too many parms" error
        }
        if( ( (mentry->macro_flags & MFLAG_VAR_ARGS) &&
              (parm_cnt < mentry->parm_count - 2) )
           || ( !(mentry->macro_flags & MFLAG_VAR_ARGS) &&
                 (parm_cnt < mentry->parm_count - 1) ) ) {
            CErr2p( ERR_TOO_FEW_MACRO_PARMS, mentry->macro_name );
        } else if( !(mentry->macro_flags & MFLAG_VAR_ARGS) &&
                   (parm_cnt > mentry->parm_count - 1) ) {
            if( mentry->parm_count - 1 != 0 ) {
                CWarn2p( WARN_PARM_COUNT_MISMATCH, ERR_TOO_MANY_MACRO_PARMS, mentry->macro_name  );
            }
        } else if( memcmp( mentry->macro_name, "va_start", 9 ) == 0 ) {
            if( SymLevel != 0  &&  ! VarParm( CurFunc ) ) {
                CErr1( ERR_MUST_BE_VAR_PARM_FUNC );
            }
        }
        FiniPPScan( ppscan_mode );      // disable T_PPNUMBER tokens
    }
    return( macro_parms );
}


local void SaveParm( MEPTR      mentry,
                     size_t     size,
                     int        parm_cnt,
                     MACRO_ARG  *macro_parms,
                     struct tokens *token_list )
{
    struct tokens   *next_tokens;
    char            *p;
    TOKEN           *p_token;
    size_t          total;

    p_token = (TOKEN *)&TokenBuf[size];
    *p_token = T_NULL;
    size += sizeof( TOKEN );

    if( parm_cnt < mentry->parm_count - 1 ) {
        p = CMemAlloc( size );
        macro_parms[parm_cnt].arg = p;
        if( p != NULL ) {
            total = 0;
            while( token_list != NULL ) {
                memcpy( &p[total], token_list->buf, token_list->length );
                total += token_list->length;
                next_tokens = token_list->next;
                CMemFree( token_list );
                token_list = next_tokens;
            }
            memcpy( &p[total], TokenBuf, size );
        }
    }
}

#ifndef NDEBUG

void DumpMDefn( unsigned char *p )
{
    int         c;
    TOKEN       tok;

    while( (tok = *(TOKEN *)p) != T_NULL ) {
        p += sizeof( TOKEN );
        switch( tok ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
            for( ;; ) {
                c = *p++;
                if( c == '\0' )
                    break;
                putchar( c );
            }
            continue;
        case T_LSTRING:
            putchar( 'L' );
            /* fall through */
        case T_STRING:
            putchar( '\"' );
            for( ;; ) {
                c = *p++;
                if( c == '\0' )
                    break;
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
            printf( "<parm#%c>", '1' + *p++ );
            continue;
        case T_MACRO_VAR_PARM:
            printf( "<varparm#%c>", '1' + *p++ );
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
    while( mtok ) {
        printf( "%s\n", mtok->data );
        mtok = mtok->next;
    }
    fflush( stdout );
}


void DumpNestedMacros( void )
{
    NESTED_MACRO *nested;

    nested = NestedMacros;
    while( nested ) {
        printf( "%s\n", nested->mentry->macro_name );
        nested = nested->next;
    }
    fflush( stdout );
}
#endif


static MACRO_TOKEN *BuildAToken( char *p )
{
    size_t      len;
    MACRO_TOKEN *mtok;

    len = strlen( p ) + 1;
    mtok = (MACRO_TOKEN *)CMemAlloc( sizeof( MACRO_TOKEN ) - 1 + len );
    memcpy( mtok->data, p, len );
    mtok->next = NULL;
    return( mtok );
}


static MACRO_TOKEN *AppendToken( MACRO_TOKEN *head, TOKEN token, char *data )
{
    MACRO_TOKEN *tail;
    MACRO_TOKEN *new;

    new = BuildAToken( data );
    new->token = token;
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

static int Expandable( MACRO_TOKEN *mtok, int macro_parm )
{
    int         lparen;

    if( NextMacro->macro_defn == 0 ) {  /* if special macro */
        return( 1 );
    }
    if( NextMacro->parm_count == 0 ) { /* if() not expected */
        if( macro_parm ) {
            if( MacroBeingExpanded( NextMacro ) ) {
                return( 0 );
            }
        }
        return( 1 );
    }
    for( ;; ) {
        if( mtok == NULL )
            break;
        if( mtok->token != T_WHITE_SPACE  &&  mtok->token != T_NULL )
            break;
        mtok = mtok->next;
    }
    if( mtok != NULL ) {
        if( mtok->token == T_LEFT_PAREN ) {
            if( MacroDepth == 1 && !macro_parm )
                return( 1 );
            lparen = 0;
            for( ;; ) {
                mtok = mtok->next;
                if( mtok == NULL )
                    break;
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
        } else if( CompFlags.cpp_output ) {
            return( 2 );
        }
    }
    return( 0 );
}

static int MacroBeingExpanded( MEPTR mentry )
{
    NESTED_MACRO    *nested;

    for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
        if( nested->mentry == mentry )
            return( 1 );
        if( !nested->rescanning ) {
            break;
        }
    }
    return( 0 );
}

static MACRO_TOKEN *ExpandNestedMacros( MACRO_TOKEN *head, int rescanning )
{
    MACRO_TOKEN *mtok;
    MACRO_TOKEN *toklist;
    MACRO_TOKEN *prev_tok;
    MACRO_TOKEN *old_tokenlist;
    int         i;
    size_t      len;
    char        *buf;

    mtok = head;
    ++MacroDepth;
    prev_tok = NULL;
    for( ;; ) {
        if( mtok == NULL )
            break;
        toklist = NULL;
        if( mtok->token == T_ID ) {
            // if macro and not being expanded, then expand it
            // only tokens available for expansion are those in mtok list
            buf = Buffer;
            len = 0;
            while( (buf[len] = mtok->data[len]) != '\0' )
                len++;
            CalcHash( buf, len );
            if( IdLookup( buf, len ) == T_MACRO ) {
                if( rescanning ) {
                    if( MacroBeingExpanded( NextMacro ) ) {
                        mtok->token = T_UNEXPANDABLE_ID;
                    } else {
                        toklist = mtok;
                        while( toklist->next != NULL ) {
                            toklist = toklist->next;
                        }
                        toklist->next = TokenList;
                        i = Expandable( mtok->next, 0 );
                        switch( i ) {
                        case 0:         // macro is currently not expandable
                            mtok->token = T_MACRO;
                            toklist->next = NULL;
                            toklist = NULL;
                            break;
                        case 1:         // macro is expandable
                            TokenList = mtok->next;
                            if( head == mtok ) {
                                head = NULL;
                                prev_tok = NULL;
                            }
                            CMemFree( mtok );
                            toklist = MacroExpansion( rescanning );
                            mtok = TokenList;
                            TokenList = NULL;
                            break;
                        case 2:         // we skipped over some white space
                            mtok->token = T_UNEXPANDABLE_ID;
                            toklist->next = NULL;
                            toklist = BuildAToken( " " );
                            toklist->token = T_WHITE_SPACE;
                            toklist->next = mtok->next;
                            mtok->next = toklist;
                            toklist = NULL;
                            break;
                        }
                    }
                } else {        // expanding a macro parm
                    if( Expandable( mtok->next, 1 ) ) {
                        old_tokenlist = TokenList;
                        TokenList = mtok->next;
                        if( head == mtok ) {
                            head = NULL;
                            prev_tok = NULL;
                        }
                        CMemFree( mtok );
                        toklist = NestedMacroExpansion( rescanning );
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
                mtok = mtok->next;      // advance onto next token
            }
        } else if( mtok->token == T_NULL ) {
            toklist = mtok->next;
            if( mtok->data[0] == 'Z' ) {        // end of a macro
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
        } else {                        // advance onto next token
            prev_tok = mtok;
            mtok = mtok->next;
        }
        if( toklist != NULL ) {         // new tokens to insert
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

static char *GlueTokenToBuffer( MACRO_TOKEN *first, char *gluebuf )
{
    size_t      gluelen;
    size_t      tokenlen;
    char        *buf;

    buf = NULL;
    if( first != NULL ) {                               /* 19-apr-93 */
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

static MACRO_TOKEN *ReTokenGlueBuffer( char *gluebuf )
{ // retokenize starting at gluebuf
    MACRO_TOKEN *head;
    MACRO_TOKEN **lnk;
    MACRO_TOKEN *new;
    int         ppscan_mode;

    ppscan_mode = InitPPScan();
    if( gluebuf == NULL )
        gluebuf = "";
    ReScanInit( gluebuf );
    head = NULL;
    lnk = &head;
    for( ;; ) {
        ReScanToken();
        new = BuildAToken( Buffer );
        new->token = CurToken;
        *lnk = new;
        lnk = &new->next;
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
    MACRO_TOKEN **lnk,**_lnk;// prior lnk
    MACRO_TOKEN *next;
    char        *buf;
    char        *gluebuf;

    gluebuf = NULL;
    _lnk = NULL;
    lnk  = &head;
    mtok = *lnk;
    buf = Buffer;
    for( ;; ) {
        if( mtok == NULL )
            break;
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
                        if( next != NULL &&
                            next->token != T_MACRO_EMPTY_VAR_PARM ) {
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
                        mtok = ReTokenGlueBuffer( gluebuf );
                        CMemFree( gluebuf );
                    } else {
                        /* Both ends of ## were empty */
                        mtok = BuildAToken( "P-<placemarker>" );
                        mtok->token = T_NULL;
                    }
                    *lnk = mtok;  // link in new mtok to mtok's link
                    while( mtok && mtok->next != NULL ) {   //position mtok & lnk to last token
                        _lnk = lnk;
                        lnk = &mtok->next;
                        mtok = *lnk;
                        if( mtok == mtok->next ) {
                            return head;
                        }
                    }
                    // mtok == last token of retokenizing
                    // lnk == the pointer which references mtok
                    mtok->next = rem;
                } else {
                    if( _lnk ) {
                        *lnk = rem;
                        lnk = _lnk;
                        mtok = *lnk;
                    } else {
                        *lnk = rem;
                        mtok = head;
                    }
                }
                continue;          //ready to go
            }
        }
        _lnk = lnk;
        lnk = &mtok->next;
        mtok = *lnk;
    }
    return( head );
}

static MACRO_TOKEN **NextString( MACRO_TOKEN **lnk, char *buf, unsigned i )
{
    MACRO_TOKEN *mtok;

    buf[i] = '\0';
    CLitLength = i + 1;
    CurToken = T_STRING;
    mtok = BuildAToken( buf );
    mtok->token = T_STRING;
    *lnk = mtok;
    lnk = &mtok->next;
    return( lnk );
}


local MACRO_TOKEN *BuildString( char *p )
{
    MACRO_TOKEN     *head;
    MACRO_TOKEN     **lnk;
    size_t          i;
    int             c;
    char            *tokenstr;
    size_t          len;
    char            *buf;
    size_t          bufsize;
    TOKEN           tok;

    head = NULL;
    lnk = &head;

    len = 0;
    if( p != NULL ) {
        bufsize = BUF_SIZE;
        buf = CMemAlloc( bufsize );
        while( *(TOKEN *)p == T_WHITE_SPACE ) {
            p += sizeof( TOKEN );   //eat leading wspace
        }
        while( (tok = *(TOKEN *)p) != T_NULL ) {
            p += sizeof( TOKEN );
            if( len >= ( bufsize - 8 ) ) {
                buf = CMemRealloc( buf, 2 * len );
            }
            switch( tok ) {
            case T_CONSTANT:
            case T_PPNUMBER:
            case T_ID:
            case T_UNEXPANDABLE_ID:
            case T_BAD_TOKEN:                           /* 07-apr-91 */
                for( ;; ) {
                    c = *p++;
                    if( c == '\0' )
                        break;
                    if( c == '\\' )
                        buf[len++] = c; /* 15-mar-88 */
                    buf[len++] = c;
                    if( len >= ( bufsize - 8 ) ) {
                        buf = CMemRealloc( buf, 2 * len );
                    }
                }
                break;
            case T_LSTRING:
                buf[len++] = 'L';
            case T_STRING:
                buf[len++] = '\\';
                buf[len++] = '"';
                for( ;; ) {
                    c = *p++;
                    if( c == '\0' )
                        break;
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
                while( (tok = *(TOKEN *)p) == T_WHITE_SPACE ) {
                    p += sizeof( TOKEN );
                }
                if( tok != T_NULL ) {
                    buf[len++] = ' ';
                }
                break;
            case T_BAD_CHAR:
                if( *p == '\\' && *(TOKEN *)( p + 1 ) == T_NULL ) {
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
            lnk = NextString( lnk, buf, len );
            len = 0;
        }
        CMemFree( buf );
    }
    return( head );
}


static MACRO_TOKEN *BuildMTokenList( char *ptr, MACRO_ARG *macro_parms )
{
    MACRO_TOKEN     *mtok;
    MACRO_TOKEN     *head;
    MACRO_TOKEN     **lnk;
    NESTED_MACRO    *nested;
    byte            *p;
    byte            *p2;
    byte            buf[2];
    TOKEN           prev_token;
    TOKEN           tok;
    unsigned char   argno;

    p = (byte *)ptr;
    head = NULL;
    lnk = &head;
    nested = NestedMacros;
    buf[1] = '\0';
    prev_token = T_NULL;
    if( p == NULL )
        return( NULL );
    while( (tok = *(TOKEN *)p) != T_NULL ) {
        p += sizeof( TOKEN );
        switch( tok ) {
        case T_CONSTANT:
        case T_PPNUMBER:
        case T_ID:
        case T_UNEXPANDABLE_ID:
        case T_BAD_TOKEN:
        case T_LSTRING:
        case T_STRING:
            mtok = BuildAToken( (char *)p );
            mtok->token = tok;
            while( *p++ )
                ;
            break;
        case T_WHITE_SPACE:
            if( prev_token == T_MACRO_SHARP_SHARP )
                continue;
            mtok = BuildAToken( " " );
            mtok->token = T_WHITE_SPACE;
            break;
        case T_BAD_CHAR:
            buf[0] = *p++;
            mtok = BuildAToken( (char *)buf );
            mtok->token = T_BAD_CHAR;
            break;
        case T_MACRO_SHARP:
            while( *(TOKEN *)p == T_WHITE_SPACE ) {
                p += sizeof( TOKEN );
            }
            p += sizeof( TOKEN );   // skip over T_MACRO_PARM
            // If no macro arg given, result must be "", not empty
            argno = *p++;
            if( macro_parms && macro_parms[argno].arg && !(*(macro_parms[argno].arg) == '\0') ) {
                mtok = BuildString( macro_parms[argno].arg );
            } else {
                mtok = BuildAToken( "" );
                mtok->token = T_STRING;
            }
            break;
        case T_MACRO_PARM:
            argno = *p++;
            p2 = p;
            while( *(TOKEN *)p2 == T_WHITE_SPACE ) {
                p2 += sizeof( TOKEN );
            }
            nested->substituting_parms = 1;             /* 09-nov-93 */
            if( macro_parms ) {
                mtok = BuildMTokenList( macro_parms[argno].arg, NULL );
                /* NB: mtok is now NULL if macro arg was empty */
            } else {
                mtok = BuildAToken( "" );
                mtok->token = T_WHITE_SPACE;
            }
            if( *(TOKEN *)p2 != T_MACRO_SHARP_SHARP  &&
                prev_token != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, 0 );
                }
            } else if( mtok == NULL ) {
                mtok = BuildAToken( "P-<placemarker>" );
                mtok->token = T_NULL;
            }
            nested->substituting_parms = 0;
            break;
        case T_MACRO_VAR_PARM:
            argno = *p++;
            p2 = p;
            while( *(TOKEN *)p2 == T_WHITE_SPACE )
                p2 += sizeof( TOKEN );
            nested->substituting_parms = 1;             /* 09-nov-93 */
            if( macro_parms ) {
                if( macro_parms[argno].arg ) {
                    mtok = BuildMTokenList( macro_parms[argno].arg, NULL );
                } else {
                    if( prev_token == T_MACRO_SHARP_SHARP
                    || *(TOKEN *)p2 == T_MACRO_SHARP_SHARP ) {
                        mtok = BuildAToken( "" );
                        mtok->token = T_MACRO_EMPTY_VAR_PARM;
                    } else {
                        mtok = BuildAToken( "" );
                        mtok->token = T_WHITE_SPACE;
                    }
                }
            } else {
                mtok = BuildAToken( "" );
                mtok->token = T_WHITE_SPACE;
            }
            if( *(TOKEN *)p2 != T_MACRO_SHARP_SHARP  &&
                prev_token != T_MACRO_SHARP_SHARP ) {
                if( mtok != NULL ) {
                    mtok = AppendToken( mtok, T_NULL, "P-<end of parm>" );
                    mtok = ExpandNestedMacros( mtok, 0 );
                }
            }
            nested->substituting_parms = 0;
            break;
        default:
            mtok = BuildAToken( Tokens[tok] );
            mtok->token = tok;
            break;
        }
        if( mtok != NULL ) {
            if( mtok->token != T_WHITE_SPACE ) {
                prev_token = mtok->token;
            }
            *lnk = mtok;
            while( *lnk != NULL ) lnk = &(*lnk)->next;
        }
    }
    head = GlueTokens( head );
    return( head );
}

MACRO_TOKEN *MacroExpansion( int rescanning )
{
    MEPTR           mentry;
    MACRO_ARG       *macro_parms;
    MACRO_TOKEN     *head;
    MACRO_TOKEN     *mtok;
    NESTED_MACRO    *nested;
    char            *tokens;
    size_t          len;

    mentry = NextMacro;
    nested = (NESTED_MACRO *)CMemAlloc( sizeof( NESTED_MACRO ) );
    nested->mentry = mentry;
    nested->rescanning = rescanning;
    nested->substituting_parms = 0;
    nested->macro_parms = NULL;
    if( mentry->macro_defn == 0 ) {     /* if special macro */
        CurToken = SpecialMacro( mentry );
        head = BuildAToken( Buffer );
        head->token = CurToken;
        nested->next = NestedMacros;
        NestedMacros = nested;
    } else {
        macro_parms = CollectParms();
        nested->next = NestedMacros;
        NestedMacros = nested;
        nested->macro_parms = macro_parms;
        tokens = (char *)mentry + mentry->macro_defn;
        head = BuildMTokenList( tokens, macro_parms );
        for( mtok = head; mtok; mtok = mtok->next ) {   /* 26-oct-93 */
            if( mtok->token == T_ID ) {
                len = strlen( mtok->data ) + 1;
                for( nested = NestedMacros; nested != NULL; nested = nested->next ) {
                    if( memcmp( nested->mentry->macro_name, mtok->data, len ) == 0 ) {
                        if( nested->substituting_parms == 0 ) {
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
    head = AppendToken( head, T_NULL, "Z-<end of macro>" );
    return( head );
}

MACRO_TOKEN *NestedMacroExpansion( int rescanning )
{
    return( ExpandNestedMacros( MacroExpansion( 0 ), rescanning ) );
}

void DoMacroExpansion( void )               // called from cscan
{
    MacroDepth = 0;
    TokenList = NestedMacroExpansion( 1 );
    // GetMacroToken will feed back tokens from the TokenList
    // when the TokenList is exhausted, then revert back to normal scanning
    if( TokenList == NULL ) {
        MacroPtr = NULL;
    } else {
        MacroPtr = "";
    }
}
