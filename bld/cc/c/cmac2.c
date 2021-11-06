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
* Description:  Macro processing, part 2.
*
****************************************************************************/


#include "cvars.h"
#include "scan.h"
#include <stddef.h>
#include "cgmisc.h"
#include "cmacadd.h"
#include "ppexpn.h"


#define HasVarArgs(m)      (((m) & MFLAG_HAS_VAR_ARGS) != 0)

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

static MEPTR GrabTokens( mac_parm_count parm_count, macro_flags mflags, MPPTR formal_parms, const char *mac_name, source_loc *src_loc );
static mac_parm_count FormalParm( MPPTR formal_parms );

struct preproc {
    char  *directive;
    void  (*samelevel)( void ); /* func to call when SkipLevel == NestLevel */
    void  (*skipfunc)( void );  /* func to call when SkipLevel != NestLevel */
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
        if( CurToken == T_BAD_TOKEN && BadTokenInfo == ERR_MISSING_QUOTE ) {
            CErr1( BadTokenInfo );
        }
    }
}


static void ChkEOL( void )
{
    if( CurToken != T_EOF ) {
        ExpectingToken( T_NULL );
    }
}


static void WantEOL( void )
{
    if( CompFlags.extensions_enabled ) {
        if( CurToken != T_NULL && CurToken != T_EOF ) {
            if( SkipLevel == NestLevel ) {
                CWarn1( WARN_JUNK_FOLLOWS_DIRECTIVE, ERR_JUNK_FOLLOWS_DIRECTIVE );
            }
        }
    } else {
        ChkEOL();
    }
}


static void IncLevel( bool value )
{
    struct cpp_info *cpp;

    cpp = (struct cpp_info *)CMemAlloc( sizeof( struct cpp_info ) );
    cpp->prev_cpp = CppStack;
    cpp->src_loc = TokenLoc;
    cpp->flist = SrcFile->src_flist;
    cpp->cpp_type = PRE_IF;
    cpp->processing = false;
    CppStack = cpp;
    if( SkipLevel == NestLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = true;
        }
    }
    ++NestLevel;
}


static void CUnknown( void )
{
    if( SkipLevel == NestLevel ) {
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
            if( SkipLevel == NestLevel ) {
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
    TOKEN       token;

    if( !CompFlags.doing_macro_expansion ) {
        if( CompFlags.cpp_mode ) {
            PrintWhiteSpace = false;
        }
    }
    while( CurrChar == '\n' ) {
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = false;
        old_ppctl = PPControl;
        for( ; CurrChar != LCHR_EOF; ) {
            if( CompFlags.cpp_mode ) {
                CppPrtChar( '\n' );
            }
            NextChar();
            if( CurrChar != PreProcChar ) {
                SkipAhead();
            }
            if( CurrChar == LCHR_EOF )
                break;
            if( CurrChar == PreProcChar ) { /* start of comp control line */
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PPCTL_DISABLE_LEX_ERRORS();
                PreProcStmt();
                PPFlush2EOL();
                PPControl = old_ppctl;
            } else if( SkipLevel != NestLevel ) {
                PPCTL_ENABLE_EOL();
                PPCTL_DISABLE_MACROS();
                PPCTL_DISABLE_LEX_ERRORS();
                PPNextToken();              /* get into token mode */
                PPFlush2EOL();
                PPControl = old_ppctl;
            }
            if( SkipLevel == NestLevel )
                break;
            if( CurrChar == '\n' ) {
                lines_skipped = true;
            }
        }
        if( CompFlags.cpp_mode ) {
            if( lines_skipped ) {
                if( SrcFile != NULL ) {
                    CppEmitPoundLine( SrcFile->src_loc.line, SrcFile->src_name, false );
                }
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    token = T_WHITE_SPACE;
//  token = ScanToken();
    return( token );
}


static void CSkip( void )
{
}


static void CSkipIf( void )
{
    IncLevel( false );
}


static void CIdent( void )
{
    if( !CompFlags.extensions_enabled ) {
        CUnknown();
    }
}


void CInclude( void )
{
    bool        in_macro;
    char        buf[82];

    if( PCH_FileName != NULL && !CompFlags.make_precompiled_header ) {
        if( CompFlags.ok_to_use_precompiled_hdr ) {
            CompFlags.use_precompiled_header = true;
        }
    }
    if( CompFlags.use_precompiled_header ) {
        InitBuildPreCompiledHeader();
    }
    InitialMacroFlags = MFLAG_NONE;
    in_macro = ( MacroPtr != NULL );
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    PPCTL_DISABLE_MACROS();
    if( CurToken == T_STRING ) {
        if( !OpenSrcFile( Buffer, FT_HEADER ) ) {
            PrtfFilenameErr( Buffer, FT_HEADER, true );
        }
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
                if( !OpenSrcFile( buf, FT_LIBRARY ) ) {
                    PrtfFilenameErr( buf, FT_LIBRARY, true );
                }
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
    CompFlags.use_precompiled_header = false;
}


MEPTR MacroScan( void )
{
    MPPTR           mp;
    MPPTR           prev_mp;
    MPPTR           formal_parms;
    mac_parm_count  parm_count;
    macro_flags     mflags;
    bool            ppscan_mode;
    char            *token_buf;
    source_loc      macro_loc;
    MEPTR           mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return( NULL );
    }
    if( IS_PPOPERATOR_DEFINED( Buffer ) ) {
        CErr1( ERR_CANT_DEFINE_DEFINED );
        return( NULL );
    }
    token_buf = CStrSave( Buffer );
    formal_parms = NULL;
    macro_loc = SrcFileLoc;
    parm_count = 0;             /* 0 ==> no () following */
    mflags = MFLAG_NONE;
    if( CurrChar == '(' ) {     /* parms present */
        PPNextToken();          /* grab the '(' */
        PPNextToken();
        parm_count = 1;         /* 1 ==> () following */
        prev_mp = NULL;
        for( ; CurToken != T_RIGHT_PAREN; ) {
            if( HasVarArgs( mflags ) ) {
                ExpectingAfter( T_RIGHT_PAREN, T_DOT_DOT_DOT );
                return( NULL );
            }
            if( CurToken != T_DOT_DOT_DOT && !ExpectingToken( T_ID ) ) {
                return( NULL );
            }
            ++parm_count;
            if( CurToken == T_DOT_DOT_DOT ) {
                mflags |= MFLAG_HAS_VAR_ARGS;   /* can have no more parms after this. */
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
            if( HasVarArgs( mflags ) ) {
                ExpectingAfter( T_RIGHT_PAREN, T_DOT_DOT_DOT );
                return( NULL );
            }
            MustRecog( T_COMMA );
            if( CurToken != T_DOT_DOT_DOT && !ExpectingToken( T_ID ) ) {
                return( NULL );
            }
        }
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    mentry = GrabTokens( parm_count, mflags, formal_parms, token_buf, &macro_loc );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens
    for( ; (mp = formal_parms) != NULL; ) {
        formal_parms = mp->next;
        CMemFree( mp->parm );
        CMemFree( mp );
    }
    CMemFree( token_buf );
    return( mentry );
}

static void CDefine( void )
{
    MEPTR           mentry;

    mentry = MacroScan();
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_USER_DEFINED;
    }
}

static MEPTR GrabTokens( mac_parm_count parm_count, macro_flags mflags, MPPTR formal_parms, const char *mac_name, source_loc *loc )
{
    MEPTR           mentry;
    MEPTR           new_mentry;
    TOKEN           prev_token;
    TOKEN           prev_non_ws_token;
    size_t          mlen;
    mac_parm_count  parmno;

    mentry = CreateMEntry( mac_name, 0 );
    mentry->parm_count = parm_count;
    mentry->src_loc = *loc;
    mlen = mentry->macro_len;
    mentry->macro_defn = mlen;
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
        switch( CurToken ) {
        case T_SHARP:
            /* if it is a function-like macro definition */
            if( parm_count != 0 ) {
                CurToken = T_MACRO_SHARP;
            }
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        case T_SHARP_SHARP:
            CurToken = T_MACRO_SHARP_SHARP;
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        case T_WHITE_SPACE:
            if( prev_token != T_WHITE_SPACE ) {
                MacroSegmentAddToken( &mlen, CurToken );
            }
            break;
        case T_ID:
            parmno = FormalParm( formal_parms );
            if( parmno != 0 ) {
                if( HasVarArgs( mflags ) && (parmno == parm_count - 1) ) {
                    CurToken = T_MACRO_VAR_PARM;
                } else {
                    CurToken = T_MACRO_PARM;
                }
            }
            MacroSegmentAddToken( &mlen, CurToken );
            if( parmno != 0 ) {
                MacroSegmentAddChar( &mlen, parmno - 1 );
            } else {
                MacroSegmentAddMem( &mlen, Buffer, TokenLen + 1 );
            }
            break;
        case T_BAD_CHAR:
            MacroSegmentAddToken( &mlen, CurToken );
            MacroSegmentAddChar( &mlen, Buffer[0] );
            if( Buffer[1] != '\0' ) {
                CurToken = T_WHITE_SPACE;
                MacroSegmentAddToken( &mlen, CurToken );
            }
            break;
        case T_STRING:
            if( CompFlags.wide_char_string ) {
                CurToken = T_LSTRING;
            }
            /* fall through */
        case T_LSTRING:
        case T_CONSTANT:
        case T_BAD_TOKEN:
        case T_PPNUMBER:
            MacroSegmentAddToken( &mlen, CurToken );
            MacroSegmentAddMem( &mlen, Buffer, TokenLen + 1 );
            break;
        default:
            MacroSegmentAddToken( &mlen, CurToken );
            break;
        }
        if( CurToken != T_WHITE_SPACE ) {
            if( prev_non_ws_token == T_MACRO_SHARP && CurToken != T_MACRO_PARM && CurToken != T_MACRO_VAR_PARM ) {
                CErr1( ERR_MUST_BE_MACRO_PARM );
//                MTOK( MacroOffset + mlen - sizeof( TOKEN ) ) = T_SHARP;
            }
            prev_non_ws_token = CurToken;
        }
        prev_token = CurToken;
        CurToken = ScanToken();
    }
    if( prev_non_ws_token == T_MACRO_SHARP ) {
        CErr1( ERR_MUST_BE_MACRO_PARM );
    }
    if( prev_token == T_WHITE_SPACE ) {
        MTOKDEC( mlen );
    }
    MacroSegmentAddToken( &mlen, T_NULL );
    if( prev_non_ws_token == T_MACRO_SHARP_SHARP ) {
        CErr1( ERR_MISPLACED_SHARP_SHARP );
    }
    new_mentry = MacroDefine( mlen, mflags );
    if( new_mentry != NULL ) {
        MacroSize += mlen;
    }
    return( new_mentry );
}


static mac_parm_count FormalParm( MPPTR formal_parms )
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


static void CIfDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( false );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( true );
    } else {
        IncLevel( false );
    }
    PPNextToken();
    ChkEOL();
}


static void CIfNDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( !ExpectingToken( T_ID ) ) {
        IncLevel( false );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( false );
    } else {
        IncLevel( true );
    }
    PPNextToken();
    ChkEOL();
}


static void CIf( void )
{
    bool    value;

    PPCTL_ENABLE_LEX_ERRORS();
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    value = PpConstExpr();
    IncLevel( value );
    ChkEOL();
    PPCTL_DISABLE_MACROS();
    PPCTL_DISABLE_LEX_ERRORS();
}


static void CElif( void )
{
    bool    value;

    PPCTL_ENABLE_LEX_ERRORS();
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( SkipLevel == NestLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = false;
            CppStack->cpp_type = PRE_ELIF;
        } else if( SkipLevel + 1 == NestLevel ) {
            /* only evaluate the expression when required */
            if( CppStack->cpp_type == PRE_IF ) {
                value = PpConstExpr();
                ChkEOL();
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    CppStack->processing = true;
                    CppStack->cpp_type = PRE_ELIF;
                }
            }
        }
    }
    PPCTL_DISABLE_MACROS();
    PPCTL_DISABLE_LEX_ERRORS();
}


static void CElse( void )
{
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( SkipLevel == NestLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = false;
        } else if( SkipLevel + 1 == NestLevel ) {
            /* cpp_type will be PRE_ELIF if an elif was true */
            if( CppStack->cpp_type == PRE_IF ) {
                SkipLevel = NestLevel;  /* start including else part */
                CppStack->processing = true;
            }
        }
        CppStack->cpp_type = PRE_ELSE;
    }
    PPNextToken();
    WantEOL();
}


static void CEndif( void )
{
    if( NestLevel == 0 ) {
        CErr1( ERR_MISPLACED_ENDIF );
    } else {
        struct cpp_info *cpp;

        --NestLevel;
        cpp = CppStack;
        if( SrcFile != NULL && cpp->flist != SrcFile->src_flist ) {
             CWarn2p( WARN_LEVEL_1, ERR_WEIRD_ENDIF_ENCOUNTER, FileIndexToCorrectName( cpp->src_loc.fno ) );
        }
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    if( SkipLevel > NestLevel ) {
        SkipLevel = NestLevel;
    }
    PPNextToken();
    WantEOL();
}

bool MacroDel( const char *name )
/*******************************/
{
    MEPTR       mentry;
    MEPTR       prev_mentry;
    size_t      len;
    bool        ret;

    ret = false;
    if( IS_PPOPERATOR_DEFINED( name ) ) {
        CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name  );
        return( ret );
    }
    prev_mentry = NULL;
    len = strlen( name ) + 1;
    for( mentry = MacHash[MacHashValue]; mentry != NULL; mentry = mentry->next_macro ) {
        if( memcmp( mentry->macro_name, name, len ) == 0 )
            break;
        prev_mentry = mentry;
    }
    if( mentry != NULL ) {
        if( MacroIsSpecial( mentry ) ) {
            CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name );
        } else {
            if( prev_mentry != NULL ) {
                prev_mentry->next_macro = mentry->next_macro;
            } else {
                MacHash[MacHashValue] = mentry->next_macro;
            }
            if( (InitialMacroFlags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE) == 0 ) {
                /* remember macros that were defined before first include */
                if( mentry->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                    mentry->next_macro = UndefMacroList;
                    UndefMacroList = mentry;
                }
            }
            ret = true;
        }
    }
    return( ret );
}


static void CUndef( void )
{

    PPNextToken();
    if( ExpectingToken( T_ID ) ) {
        MacroDel( Buffer );
        PPNextToken();
        ChkEOL();
    }
}


static void CLine( void )
{
    FNAMEPTR        flist;
    unsigned        src_line;

    src_line = 0;
    PPCTL_ENABLE_MACROS();
    PPNextToken();
    if( ExpectingConstant() ) {
        if( !CompFlags.cpp_ignore_line ) {
            src_line = Constant; // stash in case of side effects
            SrcFile->src_loc.line = src_line - 1; /* don't count this line */
        }
        PPNextToken();
        if( CurToken == T_NULL ) {
            if( !CompFlags.cpp_ignore_line ) {
                if( CompFlags.cpp_mode ) {
                    CppEmitPoundLine( src_line, SrcFile->src_name, false );
                }
            }
        } else {
            if( ExpectingToken( T_STRING ) ) {
                if( CompFlags.wide_char_string ) {
                    /* wide char string not allowed */
                    ExpectString();
                } else {
                    if( !CompFlags.cpp_ignore_line ) {
                        // RemoveEscapes( Buffer );
                        flist = AddFlist( Buffer );
                        flist->rwflag = false;  // not a real file so no autodep
                        SrcFile->src_name = flist->name;
                        SrcFile->src_loc.fno = flist->index;
                        if( CompFlags.cpp_mode ) {
                            CppEmitPoundLine( src_line, SrcFile->src_name, false );
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


static void CError( void )
{
    size_t      len;

    len = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != LCHR_EOF ) {
        if( len != 0 || CurrChar != ' ' ) {
            Buffer[len++] = CurrChar;
        }
        NextChar();
    }
    Buffer[len] = '\0';
    /* Force #error output to be reported, even with preprocessor */
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
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

static void stringize( char *s )
{
    char    *d;

    d = s;
    while( *s != '\0' ) {
        if( s[0] == '\\' ) {
            if( s[1] == '\\' || s[1] == '\"' ) {
                s++;
            }
        }
        *d++ = *s++;
    }
    *d = '\0';
}

TOKEN Process_Pragma( void )
{
    PPNextToken();
    if( CurToken == T_LEFT_PAREN ) {
        PPNextToken();
        if( CurToken == T_STRING ) {
            char        *token_buf;

            token_buf = CStrSave( Buffer );
            PPNextToken();
            if( CurToken == T_RIGHT_PAREN ) {
                ppctl_t old_ppctl;

                stringize( token_buf );
                InsertReScanPragmaTokens( token_buf );
                // call CPragma()
                old_ppctl = PPControl;
                PPCTL_ENABLE_EOL();
                CPragma();
                PPControl = old_ppctl;
            } else {
                /* error, incorrect syntax of the operator _Pragma() */
            }
            CMemFree( token_buf );
            PPNextToken();
        } else {
            /* error, incorrect syntax of the operator _Pragma() */
        }
    } else {
        InsertToken( CurToken, Buffer );
        CPYLIT( Buffer, PPOPERATOR_PRAGMA );
        TokenLen = LENLIT( PPOPERATOR_PRAGMA );
        CurToken = T_ID;
    }
    return( CurToken );
}
