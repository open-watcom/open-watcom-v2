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
* Description:  Macro processing, part 2.
*
****************************************************************************/


#include "cvars.h"
#include "scan.h"
#include <stddef.h>
#include "cgmisc.h"

extern bool PrintWhiteSpace;  //ppc printing   (from ccmain.c)

static void    CSkip( void );
static void    CSkipIf( void );
static void    CDefine( void );
static void    CInclude( void );
static void    CIfDef( void );
static void    CIfNDef( void );
static void    CIf( void );
static void    CElif( void );
static void    CElse( void );
static void    CEndif( void );
static void    CUndef( void );
static void    CLine( void );
static void    CError( void );
static void    CIdent( void );

local void GrabTokens( mac_parm_count parm_count, macro_flags mflags, MPPTR formal_parms, const char *mac_name, source_loc *src_loc );
local mac_parm_count FormalParm( MPPTR formal_parms );

struct preproc {
    char  *directive;
    void  (*samelevel)( void ); /* func to call when NestLevel == SkipLevel */
    void  (*skipfunc)( void );  /* func to call when NestLevel != SkipLevel */
};

static unsigned char PreProcWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  2, 0, 0,11, 1, 4, 0, 0, 5, 0, 0,12, 0, 0, 0,13, 0,14, 0, 9,15, 0, 0, 0, 0,0
};

static struct preproc PreProcTable[] = {
    { "",       NULL,           NULL },     // 0
    { "line",   CLine,          CSkip },    // 4 +12 + 1 = 17 mod 16 = 1
    { "define", CDefine,        CSkip },    // 6 +11 + 1 = 18 mod 16 = 2
    { "ident",  CIdent,         CSkip },    // 5 + 5 + 9 = 19 mod 16 = 3
    { "error",  CError,         CSkip },    // 5 + 1 +14 = 20 mod 16 = 4
    { "pragma", CPragma,        CSkip },    // 6 +13 + 2 = 21 mod 16 = 5
    { "else",   CElse,          CElse },    // 4 + 1 + 1 = 6
    { "",       NULL,           NULL },     // 7
    { "undef",  CUndef,         CSkip },    // 5 +15 + 4 = 24 mod 16 = 8
    { "elif",   CElif,          CElif },    // 4 + 1 + 4 = 9
    { "endif",  CEndif,         CEndif },   // 5 + 1 + 4 = 10
    { "if",     CIf,            CSkipIf },  // 2 + 5 + 4 = 11
    { "",       NULL,           NULL },     // 12
    { "include",CInclude,       CSkip },    // 7 + 5 + 1 = 13
    { "ifdef",  CIfDef,         CSkipIf },  // 5 + 5 + 4 = 14
    { "ifndef", CIfNDef,        CSkipIf },  // 6 + 5 + 4 = 15
};

enum    pp_types {
    PRE_IF,
    PRE_ELIF,
    PRE_ELSE
};

struct  cpp_info {              /* C Pre-processor information */
    struct cpp_info *prev_cpp;
    source_loc      src_loc;
    FNAMEPTR        flist;
    enum pp_types   cpp_type;
    bool            processing;
};


static void PPFlush2EOL( void )
{
    while( CurToken != T_NULL && CurToken != T_EOF ) {
        PPNextToken();
    }
}


local void ChkEOL( void )
{
    if( CurToken != T_EOF ) {
        ExpectingToken( T_NULL );
    }
}


local void WantEOL( void )
{
    if( CompFlags.extensions_enabled ) {
        if( CurToken != T_NULL && CurToken != T_EOF ) {
            if( NestLevel == SkipLevel ) {
                CWarn1( WARN_JUNK_FOLLOWS_DIRECTIVE, ERR_JUNK_FOLLOWS_DIRECTIVE );
            }
        }
    } else {
        ChkEOL();
    }
}


local void IncLevel( bool value )
{
    struct cpp_info *cpp;

    cpp = (struct cpp_info *)CMemAlloc( sizeof( struct cpp_info ) );
    cpp->prev_cpp = CppStack;
    cpp->src_loc = TokenLoc;
    cpp->flist = SrcFile->src_flist;
    cpp->cpp_type = PRE_IF;
    cpp->processing = FALSE;
    CppStack = cpp;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = TRUE;
        }
    }
    ++NestLevel;
}


local void CUnknown( void )
{
    if( NestLevel == SkipLevel ) {
        CErr2p( ERR_UNKNOWN_DIRECTIVE, Buffer );
    }
}


static void PreProcStmt( void )
{
    struct preproc      *pp;
    unsigned            hash;

    NextChar();                 /* skip over '#' */
    PPNextToken();
    if( CurToken == T_ID ) {
        hash = (TokenLen + PreProcWeights[Buffer[0] - 'a']
                 + PreProcWeights[Buffer[TokenLen - 1] - 'a']) & 15;
        pp = &PreProcTable[hash];
        if( memcmp( pp->directive, Buffer, TokenLen + 1 ) == 0 ) {
            if( NestLevel == SkipLevel ) {
                pp->samelevel();
            } else {
                pp->skipfunc();
            }
        } else {
            CUnknown();
        }
    } else if( CurToken != T_NULL ) {
        CUnknown();
    }
}

TOKEN ChkControl( void )
{
    bool        lines_skipped;
    ppctl_t     old_ppctl;

    if( !CompFlags.doing_macro_expansion ) {
        if( CompFlags.cpp_output ) {
            PrintWhiteSpace = FALSE;
        }
    }
    while( CurrChar == '\n' ) {
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = FALSE;
        old_ppctl = CompFlags.pre_processing;
        for( ; CurrChar != EOF_CHAR; ) {
            if( CompFlags.cpp_output ) {
                CppPrtChar( '\n' );
            }
            NextChar();
            if( CurrChar != PreProcChar ) {
                SkipAhead();
            }
            if( CurrChar == EOF_CHAR )
                break;
            if( CurrChar == PreProcChar ) { /* start of comp control line */
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PreProcStmt();
                PPFlush2EOL();
                CompFlags.pre_processing = old_ppctl;
            } else if( NestLevel != SkipLevel ) {
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PPNextToken();              /* get into token mode */
                PPFlush2EOL();
                CompFlags.pre_processing = old_ppctl;
            }
            if( NestLevel == SkipLevel )
                break;
            if( CurrChar == '\n' ) {
                lines_skipped = TRUE;
            }
        }
        if( CompFlags.cpp_output ) {
            if( lines_skipped ) {
                if( SrcFile != NULL ) {
                    EmitLine( SrcFile->src_loc.line, SrcFile->src_name );
                }
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    CurToken = T_WHITE_SPACE;
//  CurToken = ScanToken();
    return( T_WHITE_SPACE );
}


local void CSkip( void )
{
}


local void CSkipIf( void )
{
    IncLevel( FALSE );
}


local void CIdent( void )
{
    if( !CompFlags.extensions_enabled ) {
        CUnknown();
    }
}


void CInclude( void )
{
    bool        in_macro;
    auto char   buf[82];

    if( PCH_FileName != NULL && CompFlags.make_precompiled_header == 0 ) {
        if( CompFlags.ok_to_use_precompiled_hdr ) {
            CompFlags.use_precompiled_header = TRUE;
        }
    }
    if( CompFlags.use_precompiled_header ) {
        InitBuildPreCompiledHeader();
    }
    InitialMacroFlag = MFLAG_NONE;
    in_macro = ( MacroPtr != NULL );
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    PPCTL_DISABLE_MACROS();
    if( CurToken == T_STRING ) {
        OpenSrcFile( Buffer, FALSE );
#if _CPU == 370
        if( !CompFlags.use_precompiled_header ) {
            SrcFile->colum = Column;    /* do trunc and col on  */
            SrcFile->trunc = Trunc;     /* on user source files */
        }
#endif
    } else if( CurToken == T_LT ) {
        if( in_macro ) {
            PPCTL_ENABLE_MACROS();
        }
        buf[0] = '\0';
        for( ;; ) {
            PPNextToken();
            if( CurToken == T_GT ) {
                OpenSrcFile( buf, TRUE );
                break;
            }
            strncat( buf, Buffer, sizeof( buf ) - 2 );
            if( in_macro && MacroPtr == NULL || CurToken == T_NULL || CurToken == T_EOF ) {
                CErr1( ERR_INVALID_INCLUDE );
                break;
            }
        }
        if( in_macro ) {
            PPCTL_DISABLE_MACROS();
        }
    } else {
        CErr1( ERR_INVALID_INCLUDE );
    }
    if( CurToken != T_EOF ) {
        PPNextToken();
    }
    CompFlags.use_precompiled_header = FALSE;
}


local void CDefine( void )
{
    MPPTR           mp;
    MPPTR           prev_mp;
    MPPTR           formal_parms;
    mac_parm_count  parm_count;
    macro_flags     mflags;
    bool            ppscan_mode;
    char            *token_buf;
    source_loc      macro_loc;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return;
    }
    if( CMPLIT( Buffer, "defined" ) == 0 ) {
        CErr1( ERR_CANT_DEFINE_DEFINED );
        return;
    }
    token_buf = CStrSave( Buffer );
    formal_parms = NULL;
    macro_loc = SrcFileLoc;
    parm_count = 0;             /* 0 ==> no () following */
    mflags = MFLAG_USER_DEFINED;
    if( CurrChar == '(' ) {     /* parms present */
        PPNextToken();          /* grab the '(' */
        PPNextToken();
        parm_count = 1;         /* 1 ==> () following */
        prev_mp = NULL;
        for( ; CurToken != T_RIGHT_PAREN; ) {
            if( mflags & MFLAG_VAR_ARGS ) {
                ExpectingAfter( T_RIGHT_PAREN, T_DOT_DOT_DOT );
                return;
            }
            if( CurToken != T_DOT_DOT_DOT && !ExpectingToken( T_ID ) ) {
                return;
            }
            ++parm_count;
            if( CurToken == T_DOT_DOT_DOT ) {
                mflags |= MFLAG_VAR_ARGS;   /* can have no more parms after this. */
            }
            mp = (MPPTR)CMemAlloc( sizeof( MPDEFN ) );
            if( formal_parms == NULL ) {
                formal_parms = mp;
            } else {
                if( FormalParm( formal_parms ) ) {
                    CErr2p( ERR_DUPLICATE_MACRO_PARM, Buffer );
                }
                prev_mp->next = mp;
            }
            if( CurToken == T_DOT_DOT_DOT ) {
                mp->parm = CStrSave( "__VA_ARGS__" );
            } else {
                mp->parm = CStrSave( Buffer );
            }
            prev_mp = mp;
            PPNextToken();
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( CurToken == T_NULL ) {
                CErr1( ERR_INVALID_MACRO_DEFN );
                break;
            }
            if( mflags & MFLAG_VAR_ARGS ) {
                ExpectingAfter( T_RIGHT_PAREN, T_DOT_DOT_DOT );
                return;
            }
            MustRecog( T_COMMA );
            if( CurToken != T_DOT_DOT_DOT && !ExpectingToken( T_ID ) ) {
                return;
            }
        }
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    GrabTokens( parm_count, mflags, formal_parms, token_buf, &macro_loc );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens
    for( ; (mp = formal_parms) != NULL; ) {
        formal_parms = mp->next;
        CMemFree( mp->parm );
        CMemFree( mp );
    }
    CMemFree( token_buf );
}


local void GrabTokens( mac_parm_count parm_count, macro_flags mflags, MPPTR formal_parms, const char *mac_name, source_loc *loc )
{
    MEPTR           mentry;
    size_t          len;
    size_t          i;
    TOKEN           prev_token;
    TOKEN           prev_non_ws_token;
    size_t          mlen;
    mac_parm_count  parmno;

    mentry = CreateMEntry( mac_name, strlen( mac_name ) );
    mentry->parm_count = parm_count;
    mentry->src_loc.fno = loc->fno;
    mentry->src_loc.line = loc->line;
    mlen = mentry->macro_len;
    mentry->macro_defn = mlen;
    MacroOverflow( mlen, 0 );
    MacroCopy( mentry, MacroOffset, mlen );
    prev_token = T_NULL;
    prev_non_ws_token = T_NULL;
    if( CurToken != T_NULL ) {
        do {
            CurToken = ScanToken();
        } while( CurToken == T_WHITE_SPACE );
        if( CurToken == T_SHARP_SHARP ) {
            CErr1( ERR_MISPLACED_SHARP_SHARP );
            PPNextToken();
        }
    }
    for( ; CurToken != T_NULL && CurToken != T_EOF ; ) {
        MTOK( TokenBuf ) = CurToken;
        len = sizeof( TOKEN );
        switch( CurToken ) {
        case T_SHARP:
            /* if it is a function-like macro definition */
            if( parm_count != 0 ) {
                CurToken = T_MACRO_SHARP;
                MTOK( TokenBuf ) = CurToken;
            }
            break;
        case T_SHARP_SHARP:
            MTOK( TokenBuf ) = T_MACRO_SHARP_SHARP;
            break;
        case T_WHITE_SPACE:
            if( prev_token == T_WHITE_SPACE )
                MTOKDEC( len );
            break;
        case T_ID:
            parmno = FormalParm( formal_parms );
            if( parmno != 0 ) {
                if( (mflags & MFLAG_VAR_ARGS) && (parmno == parm_count - 1) ) {
                    CurToken = T_MACRO_VAR_PARM;
                } else {
                    CurToken = T_MACRO_PARM;
                }
                MTOK( TokenBuf ) = CurToken;
                MTOKPARM( TokenBuf + len ) = parmno - 1;
                MTOKPARMINC( len );
            } else {
                i = 0;
                while( (TokenBuf[len++] = Buffer[i++]) != '\0' ) {
                    ;   /*empty*/
                }
            }
            break;
        case T_BAD_CHAR:
            TokenBuf[len++] = Buffer[0];
            if( Buffer[1] != '\0' ) {
                MTOK( TokenBuf + len ) = T_WHITE_SPACE;
                MTOKINC( len );
            }
            break;
        case T_STRING:
            if( CompFlags.wide_char_string ) {
                CurToken = T_LSTRING;
                MTOK( TokenBuf ) = CurToken;
            }
            /* fall through */
        case T_CONSTANT:
        case T_LSTRING:
        case T_BAD_TOKEN:
        case T_PPNUMBER:
            i = 0;
            while( (TokenBuf[len++] = Buffer[i++]) != '\0' )
                ;   /* empty */
            break;
        default:
            break;
        }
        if( CurToken != T_WHITE_SPACE ) {
            if( prev_non_ws_token == T_MACRO_SHARP
              && CurToken != T_MACRO_PARM && CurToken != T_MACRO_VAR_PARM ) {
                CErr1( ERR_MUST_BE_MACRO_PARM );
                MTOK( MacroOffset + mlen - sizeof( TOKEN ) ) = T_SHARP;
            }
            prev_non_ws_token = CurToken;
        }
        prev_token = CurToken;
        CurToken = ScanToken();
        MacroOverflow( mlen + len, mlen );
        MacroCopy( TokenBuf, MacroOffset + mlen, len );
        mlen += len;
    }
    if( prev_non_ws_token == T_MACRO_SHARP ) {
        CErr1( ERR_MUST_BE_MACRO_PARM );
    }
    if( prev_token == T_WHITE_SPACE ) {
        MTOKDEC( mlen );
    }
    MacroOverflow( mlen + sizeof( TOKEN ), mlen );
    MTOK( MacroOffset + mlen ) = T_NULL;
    MTOKINC( mlen );
    if( prev_non_ws_token == T_SHARP_SHARP ) {
        CErr1( ERR_MISPLACED_SHARP_SHARP );
    }
    mentry->macro_len = mlen;
    MacLkAdd( mentry, mlen, mflags );
    FreeMEntry( mentry );
    MacroSize += mlen;
}


local mac_parm_count FormalParm( MPPTR formal_parms )
{
    mac_parm_count i;

    i = 1;
    for( ; formal_parms != NULL; formal_parms = formal_parms->next ) {
        if( memcmp( formal_parms->parm, Buffer, TokenLen + 1 ) == 0 ) {
            return( i );
        }
        ++i;
    }
    return( 0 );
}


local void CIfDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( FALSE );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( TRUE );
    } else {
        IncLevel( FALSE );
    }
    PPNextToken();
    ChkEOL();
}


local void CIfNDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( !ExpectingToken( T_ID ) ) {
        IncLevel( FALSE );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( FALSE );
    } else {
        IncLevel( TRUE );
    }
    PPNextToken();
    ChkEOL();
}


local bool GetConstExpr( void )
{
    bool        value;
    bool        useful_side_effect;
    bool        meaningless_stmt;

/* This solves the following weird condition.   */
/*      while( f() == 1 )                       */
/* The expression for the #if destroys the flags saved for the while expr */
/*   #if 1                                      */
/*              ;                               */
/*   #endif                                     */

    useful_side_effect = CompFlags.useful_side_effect;
    meaningless_stmt   = CompFlags.meaningless_stmt;
    value = BoolConstExpr();
    CompFlags.useful_side_effect = useful_side_effect;
    CompFlags.meaningless_stmt   = meaningless_stmt;
    return( value );
}

local void CIf( void )
{
    bool    value;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    value = GetConstExpr();
    IncLevel( value );
    ChkEOL();
    PPCTL_DISABLE_MACROS();
}


local void CElif( void )
{
    bool    value;

    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = FALSE;
            CppStack->cpp_type = PRE_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( CppStack->cpp_type == PRE_IF ) {
                value = GetConstExpr();
                ChkEOL();
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    CppStack->processing = TRUE;
                    CppStack->cpp_type = PRE_ELIF;
                }
            }
        }
    }
    PPCTL_DISABLE_MACROS();
}


local void CElse( void )
{
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = FALSE;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* cpp_type will be PRE_ELIF if an elif was true */
            if( CppStack->cpp_type == PRE_IF ) {
                SkipLevel = NestLevel;  /* start including else part */
                CppStack->processing = TRUE;
            }
        }
        CppStack->cpp_type = PRE_ELSE;
    }
    PPNextToken();
    WantEOL();
}


local void CEndif( void )
{
    if( NestLevel == 0 ) {
        CErr1( ERR_MISPLACED_ENDIF );
    } else {
        struct cpp_info *cpp;

        --NestLevel;
        cpp = CppStack;
        if( cpp->flist != SrcFile->src_flist ) {
             CWarn2p( WARN_LEVEL_1, ERR_WEIRD_ENDIF_ENCOUNTER, FileIndexToCorrectName( cpp->src_loc.fno ) );
        }
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    if( NestLevel < SkipLevel ) {
        SkipLevel = NestLevel;
    }
    PPNextToken();
    WantEOL();
}

bool MacroDel( const char *name )
/*******************************/
{
    MEPTR       mentry;
    MEPTR       prev_entry;
    size_t      len;
    bool        ret;

    ret = FALSE;
    if( CMPLIT( name, "defined" ) == 0 ) {
        CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name  );
        return( ret );
    }
    prev_entry = NULL;
    len = strlen( name ) + 1;
    for( mentry = MacHash[MacHashValue]; mentry != NULL; mentry = mentry->next_macro ) {
        if( memcmp( mentry->macro_name, name, len ) == 0 )
            break;
        prev_entry = mentry;
    }
    if( mentry != NULL ) {
        if( mentry->macro_defn == 0 ) {
            CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name );
        } else {
            if( prev_entry != NULL ) {
                prev_entry->next_macro = mentry->next_macro;
            } else {
                MacHash[MacHashValue] = mentry->next_macro;
            }
            if( (InitialMacroFlag & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE) == 0 ) {
                /* remember macros that were defined before first include */
                if( mentry->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                    mentry->next_macro = UndefMacroList;
                    UndefMacroList = mentry;
                }
            }
            ret = TRUE;
        }
    }
    return( ret );
}


local void CUndef( void )
{

    PPNextToken();
    if( ExpectingToken( T_ID ) ) {
        MacroDel( Buffer );
        PPNextToken();
        ChkEOL();
    }
}


local void CLine( void )
{
    FNAMEPTR        flist;
    unsigned        src_line;

    src_line = 0;
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingConstant() ) {
        if( CompFlags.cpp_ignore_line == 0 ) {
            src_line = Constant; // stash in case of side effects
            SrcFile->src_loc.line = src_line - 1; /* don't count this line */
        }
        PPNextToken();
        if( CurToken == T_NULL ) {
            if( CompFlags.cpp_ignore_line == 0 ) {
                if( CompFlags.cpp_output ) {
                    EmitLine( src_line, SrcFile->src_name );
                }
            }
        } else {
            if( ExpectingToken( T_STRING ) ) {
                if( CompFlags.wide_char_string ) {
                    /* wide char string not allowed */
                    ExpectString();
                } else {
                    if( CompFlags.cpp_ignore_line == 0 ) {
                        // RemoveEscapes( Buffer );
                        flist = AddFlist( Buffer );
                        flist->rwflag = FALSE;  // not a real file so no autodep
                        SrcFile->src_name = flist->name;
                        SrcFile->src_loc.fno  = flist->index;
                        if( CompFlags.cpp_output ) {
                            EmitLine( src_line, SrcFile->src_name );
                        }
                    }
                }
            }
            PPNextToken();
            ChkEOL();
        }
    }
    PPCTL_DISABLE_MACROS();
}


local void CError( void )
{
    size_t      len;
    bool        save;

    len = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != EOF_CHAR ) {
        if( len != 0 || CurrChar != ' ' ) {
            Buffer[len++] = CurrChar;
        }
        NextChar();
    }
    Buffer[len] = '\0';
    /* Force #error output to be reported, even with preprocessor */
    save = CompFlags.cpp_output;
    CompFlags.cpp_output = FALSE;
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
    CompFlags.cpp_output = save;
}


void CppStackInit( void )
{
    NestLevel = 0;
    SkipLevel = 0;
    CppStack = NULL;
}


void CppStackFini( void )
{
    struct cpp_info *cpp;

    while( (cpp = CppStack) != NULL ) {
        SetErrLoc( &cpp->src_loc );
        CErr1( ERR_MISSING_CENDIF );
        InitErrLoc();
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    CppStack = NULL;
}
