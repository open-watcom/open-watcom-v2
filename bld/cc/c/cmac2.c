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


#include "cvars.h"
#include "pragmas.h"
#include "scan.h"
#include <stddef.h>


void    CSkip(void);
void    CSkipIf(void);
void    CDefine(void);
void    CInclude(void);
void    CIfDef(void);
void    CIfNDef(void);
void    CIf(void);
void    CElif(void);
void    CElse(void);
void    CEndif(void);
void    CUndef(void);
void    CLine(void);
void    CError(void);
void    CPragma(void);
void    CIdent(void);
void    CUnknown(void);

struct preproc {
    char  *directive;
    void  (*samelevel)(void);   /* func to call when NestLevel == SkipLevel */
    void  (*skipfunc)(void);    /* func to call when NestLevel != SkipLevel */
};

static unsigned char PreProcWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  2, 0, 0,11, 1, 4, 0, 0, 5, 0, 0,12, 0, 0, 0,13, 0,14, 0, 9,15, 0, 0, 0, 0,0
};
static struct preproc PreProcTable[] = {
    { "",       NULL,           NULL },  // 0
    { "line",   CLine,          CSkip }, // 4 +12 + 1 = 17 mod 16 = 1
    { "define", CDefine,        CSkip }, // 6 +11 + 1 = 18 mod 16 = 2
    { "ident",  CIdent,         CSkip }, // 5 + 5 + 9 = 19 mod 16 = 3
    { "error",  CError,         CSkip }, // 5 + 1 +14 = 20 mod 16 = 4
    { "pragma", CPragma,        CSkip }, // 6 +13 + 2 = 21 mod 16 = 5
    { "else",   CElse,          CElse }, // 4 + 1 + 1 = 6
    { "",       NULL,           NULL },  // 7
    { "undef",  CUndef,         CSkip }, // 5 +15 + 4 = 24 mod 16 = 8
    { "elif",   CElif,          CElif }, // 4 + 1 + 4 = 9
    { "endif",  CEndif,         CEndif },// 5 + 1 + 4 = 10
    { "if",     CIf,            CSkipIf },//2 + 5 + 4 = 11
    { "",       NULL,           NULL },  // 12
    { "include",CInclude,       CSkip }, // 7 + 5 + 1 = 13
    { "ifdef",  CIfDef,         CSkipIf },//5 + 5 + 4 = 14
    { "ifndef", CIfNDef,        CSkipIf },//6 + 5 + 4 = 15
};

enum    pp_types {
        PRE_IF,
        PRE_ELIF,
        PRE_ELSE
};

struct  cpp_info {              /* C Pre-processor information */
        struct cpp_info *prev_cpp;
        char            *file_name;
        unsigned        line_num;
        enum pp_types   cpp_type;
        int             processing;
};

void PreProcStmt(void)
{
    struct preproc      *pp;
    int                 hash;

    NextChar();                 /* skip over '#' */
    PPNextToken();
    if( CurToken == T_ID ) {
        hash = (TokenLen + PreProcWeights[ Buffer[0] - 'a' ]
                 + PreProcWeights[ Buffer[TokenLen-1] - 'a' ]) & 15;
        pp = &PreProcTable[hash];
        if( strcmp( pp->directive, Buffer ) == 0 ) {
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
    Flush2EOL();
}

extern bool PrintWhiteSpace;  //ppc printing
int ChkControl(void)
{
    int lines_skipped;


    if( ! CompFlags.doing_macro_expansion ) {
        if( CompFlags.cpp_output ){
            PrintWhiteSpace = FALSE;
        }
    }
    while( CurrChar == '\n' ) {
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = 0;
        for(;;) {
            if( CompFlags.cpp_output )  PrtChar( '\n' );
            NextChar();
            if( CurrChar != PreProcChar ) {
                SkipAhead();
            }
            if( CurrChar == EOF_CHAR ) break;
            CompFlags.pre_processing = 2;
            if( CurrChar == PreProcChar ) { /* start of comp control line */
                PreProcStmt();
            } else if( NestLevel != SkipLevel ) {
                PPNextToken();          /* get into token mode */
                Flush2EOL();
            }
            if( NestLevel == SkipLevel ) break;
            if( CurrChar == '\n' ) {
                lines_skipped = 1;
            }
            if( CurrChar == EOF_CHAR ) break;           /* 16-may-89 */
        }
        CompFlags.pre_processing = 0;
        if( CompFlags.cpp_output ) {
            if( lines_skipped ) {
                if( SrcFile != NULL ) {                 /* 14-may-92 */
                    EmitLine( SrcFile->src_line, SrcFile->src_name );
                }
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    CurToken = T_WHITE_SPACE;
//  CurToken = ScanToken();
    return( T_WHITE_SPACE );
}

int ScanCharacters()
{
    int         c;
    char        *scanptr;

    scanptr = ScanCharPtr;
    for(;;) {
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
        c = *scanptr++;
        if( CharSet[c] & C_EX ) break;
    }
    ScanCharPtr = scanptr;
    return( c );
}

void Flush2EOL()
{
#if 1
    while( CurToken != T_NULL ) {
        if( CurToken == T_EOF ) break;          /* 07-may-89 */
        PPNextToken();
    }
#else
    int         c;

    // mark some special characters so that we only have to do one test
    // for each character inside the main loop
    c = CurrChar;
    for(;;) {
        CharSet['/']  |= C_EX;          // make '/' special character
        if( (CharSet[c] & C_EX) == 0 ) {
            c = ScanCharacters();
        }
        if( c == '\n' ) {               // we are at end of line
            SrcFile->src_line++;
            SrcFileLineNum = SrcFile->src_line;
            break;
        }
        if( c == '/' ) {                // check for comment
            ScanSlash();
            c = CurrChar;
        } else {                // one of the other special characters
            c = GetCharCheck( c );
            if( c == EOF_CHAR ) break;
        }
    }
    CharSet['/']  &= ~C_EX;             // undo '/' special character
#endif
}

local void CSkip(void)
{
}

local void CSkipIf(void)
{
    IncLevel( 0 );
}

local void CUnknown()
{
    if( NestLevel == SkipLevel ) {
        CErr2p( ERR_UNKNOWN_DIRECTIVE, Buffer );
    }
}


local void CIdent()
{
    if( !CompFlags.extensions_enabled ) CUnknown();
    Flush2EOL();
}


void CInclude()
{
    char        in_macro;
    auto char   buf[82];

    if( PCH_FileName != NULL  &&  CompFlags.make_precompiled_header == 0 ) {
        if( CompFlags.ok_to_use_precompiled_hdr ) {     /* 27-jun-94 */
            CompFlags.use_precompiled_header = 1;
        }
    }
    if( CompFlags.make_precompiled_header ){
        InitBuildPreCompiledHeader();
    }
    InitialMacroFlag = 0;
    in_macro = 0;
    if( MacroPtr != NULL ) in_macro = 1;
    CompFlags.pre_processing = 1;
    PPNextToken();
    if( CurToken == T_STRING ) {
        OpenSrcFile( Buffer, 0 );
        #if _CPU == 370
            if( ! CompFlags.use_precompiled_header ) {
                SrcFile->colum = Column;   /* do trunc and col on  */
                SrcFile->trunc = Trunc;  /* on user source files */
            }
        #endif
    } else if( CurToken == T_LT ) {
        if( ! in_macro )  CompFlags.pre_processing = 2; /* 28-may-89 */
        buf[0] = '\0';
        for(;;) {
            PPNextToken();
            if( CurToken == T_GT ) {
                OpenSrcFile( buf, '<' );
                break;
            }
            strncat( buf, Buffer, 80 );
            if( ( in_macro != 0  &&  MacroPtr == NULL)
                ||      CurToken == T_NULL
                ||      CurToken == T_EOF ) {
                CErr1( ERR_INVALID_INCLUDE );
                break;
            }
        }
    } else {
        CErr1( ERR_INVALID_INCLUDE );
    }
    if( CurToken != T_EOF )  PPNextToken();
    CompFlags.use_precompiled_header = 0;
}


local void CDefine()
{
    struct macro_parm *mp, *prev_mp, *formal_parms;
    int         parm_cnt;
    int         i, j;
    int         ppscan_mode;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return;
    }
    if( strcmp( Buffer, "defined" ) == 0 ) {
        CErr1( ERR_CANT_DEFINE_DEFINED );
        return;
    }
    i = 0;
    while( TokenBuf[i] = Buffer[i] ) ++i;
    ++i;
    formal_parms = NULL;
    parm_cnt = -1;              /* -1 ==> no () following */
    if( CurrChar == '(' ) {         /* parms present */
        PPNextToken();          /* grab the '(' */
        PPNextToken();
        parm_cnt = 0;           /* 0 ==> () following */
        prev_mp = NULL;
        for( ;; ) {
            if( CurToken == T_RIGHT_PAREN ) break;
            if( CurToken != T_ID ) {
                ExpectIdentifier();
                return;
            }
            ++parm_cnt;
            mp = (struct macro_parm *)
                            CMemAlloc( sizeof( struct macro_parm ) );
            if( formal_parms == NULL ) {
                formal_parms = mp;
            } else {
                if( FormalParm( formal_parms ) ) {
                    CErr2p( ERR_DUPLICATE_MACRO_PARM, Buffer );
                }
                prev_mp->next_macro_parm = mp;
            }
            mp->parm = CStrSave( Buffer );
            prev_mp = mp;
            j = 0;
            while( TokenBuf[i++] = Buffer[j++] );
            PPNextToken();
            if( CurToken == T_RIGHT_PAREN ) break;
            if( CurToken == T_NULL ) {
                CErr1( ERR_INVALID_MACRO_DEFN );
                break;
            }
            MustRecog( T_COMMA );
            if( CurToken != T_ID ) {            /* 16-nov-94 */
                ExpectIdentifier();
                return;
            }
        }
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    GrabTokens( parm_cnt + 1, formal_parms, i );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens
    for(; mp = formal_parms; ) {
        formal_parms = mp->next_macro_parm;
        CMemFree( mp->parm );
        CMemFree( mp );
    }
}


local void GrabTokens( parm_cnt, formal_parms, defn_offset )
        int     parm_cnt;
        struct macro_parm *formal_parms;
        int     defn_offset;
    {
        MEPTR   mentry;
        int     i;
        int     j;
        int     prev_token;
        int     prev_non_ws_token;
        unsigned mlen;

        i = defn_offset;
        j = strlen( TokenBuf );
        mentry = (MEPTR) CMemAlloc( sizeof(MEDEFN) + j );
        mentry->parm_count = parm_cnt;
        strcpy( mentry->macro_name, TokenBuf );
        mlen = offsetof(MEDEFN,macro_name) + i;
        mentry->macro_defn = mlen;
        MacroOverflow( mlen, 0 );
        MacroCopy( (char *)mentry, MacroOffset, offsetof(MEDEFN,macro_name) );
        MacroCopy( TokenBuf, MacroOffset + offsetof(MEDEFN,macro_name), i );
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
        for(;;) {
            i = 0;
            if( CurToken == T_STRING  &&  CompFlags.wide_char_string ) {
                CurToken = T_LSTRING;                   /* 15-may-92 */
            }
            TokenBuf[i++] = CurToken;
            if( CurToken == T_NULL ) break;
            if( CurToken == T_EOF )  break;
            switch( CurToken ) {
            case T_SHARP:
                /* if it is a function-like macro definition */
                if( parm_cnt != 0 ) {
                    TokenBuf[i-1] = T_MACRO_SHARP;
                    CurToken = T_MACRO_SHARP;           /* 26-mar-91 */
                }
                break;
            case T_SHARP_SHARP:
                TokenBuf[i-1] = T_MACRO_SHARP_SHARP;
                break;
            case T_WHITE_SPACE:
                if( prev_token == T_WHITE_SPACE ) --i;
                break;
            case T_ID:
                j = FormalParm( formal_parms );
                if( j != 0 ) {
                    CurToken = T_MACRO_PARM;
                    TokenBuf[i-1] = T_MACRO_PARM;
                    TokenBuf[i] = j - 1;
                    ++i;
                } else {
                    j = 0;
                    while( TokenBuf[i++] = Buffer[j++] ) ;
                }
                break;
            case T_BAD_CHAR:
                TokenBuf[i++] = Buffer[0];
                if( Buffer[1] != '\0' ) TokenBuf[i++] = T_WHITE_SPACE;
                break;
            case T_CONSTANT:
            case T_STRING:
            case T_LSTRING:
            case T_BAD_TOKEN:
            case T_PPNUMBER:
                j = 0;
                while( TokenBuf[i++] = Buffer[j++] ) ;
                break;
            }
            if( CurToken != T_WHITE_SPACE ) {
                if( prev_non_ws_token == T_MACRO_SHARP &&       /* 26-mar-91 */
                    CurToken != T_MACRO_PARM ) {
                    CErr1( ERR_MUST_BE_MACRO_PARM );
                    prev_token = TokenBuf[0];
                    TokenBuf[0] = T_SHARP;              /* 17-jul-92 */
                    MacroCopy( TokenBuf, MacroOffset + mlen - 1, 1 );
                    TokenBuf[0] = prev_token;
                }
                prev_non_ws_token = CurToken;
            }
            prev_token = CurToken;
            CurToken = ScanToken();
            MacroOverflow( mlen + i, mlen );
            MacroCopy( TokenBuf, MacroOffset + mlen, i );
            mlen += i;
        }
        if( prev_non_ws_token == T_MACRO_SHARP ) {      /* 16-nov-94 */
            CErr1( ERR_MUST_BE_MACRO_PARM );
        }
        if( prev_token == T_WHITE_SPACE ) {
            --mlen;
        }
        MacroOverflow( mlen + 1, mlen );                /* 27-apr-94 */
        *(char *)(MacroOffset + mlen) = T_NULL;
        ++mlen;
        if( prev_non_ws_token == T_SHARP_SHARP ) {
            CErr1( ERR_MISPLACED_SHARP_SHARP );
        }
        mentry->macro_len = mlen;
        MacLkAdd( mentry, mlen, MACRO_USER_DEFINED );   /* 15-apr-94 */
        CMemFree( mentry );                     /* 22-aug-88, FWC */
        MacroSize += mlen;
    }


local int FormalParm( struct macro_parm *formal_parms )
{
    int i;

    i = 1;
    while( formal_parms != NULL ) {
        if( strcmp( formal_parms->parm, Buffer ) == 0 ) return( i );
        ++i;
        formal_parms = formal_parms->next_macro_parm;
    }
    return( 0 );
}


local void CIfDef()
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( 0 );
        return;
    }
    mentry = MacroLookup();
    if( mentry != NULL ) {
        mentry->macro_flags |= MACRO_REFERENCED;
        IncLevel( 1 );
    } else {
        IncLevel( 0 );
    }
    PPNextToken();
    ChkEOL();
}


local void CIfNDef()
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( 0 );
        return;
    }
    mentry = MacroLookup();
    if( mentry != NULL ) {
        mentry->macro_flags |= MACRO_REFERENCED;
        IncLevel( 0 );
    } else {
        IncLevel( 1 );
    }
    PPNextToken();
    ChkEOL();
}

local int GetConstExpr()                                /* 13-nov-91 */
{
    int         value;
    int         useful_side_effect;
    int         meaningless_stmt;

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

local void CIf()
{
    int value;

    CompFlags.pre_processing = 1;
    PPNextToken();
    value = GetConstExpr();
    IncLevel( value );
    ChkEOL();
}


local void CElif()
{
    int value;

    CompFlags.pre_processing = 1;
    PPNextToken();
    if( NestLevel == 0  ||  CppStack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = 0;
            CppStack->cpp_type = PRE_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( CppStack->cpp_type == PRE_IF ) {        /* 30-jun-88 */
                value = GetConstExpr();         /* 21-jun-88 */
                ChkEOL();
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    CppStack->processing = 1;
                    CppStack->cpp_type = PRE_ELIF;
                }
            }
        }
    }
}


local void IncLevel( int value )
{
    struct cpp_info *cpp;

    cpp = (struct cpp_info *)CMemAlloc( sizeof( struct cpp_info ) );
    cpp->prev_cpp = CppStack;
    cpp->file_name = ErrFName;
    cpp->line_num = TokenLine;
    cpp->cpp_type = PRE_IF;
    cpp->processing = 0;
    CppStack = cpp;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = 1;
        }
    }
    ++NestLevel;
}


local void WantEOL()
{
    if( CompFlags.extensions_enabled ) {
        if( CurToken != T_NULL  &&  CurToken != T_EOF ) {
            if( NestLevel == SkipLevel ) {
                CWarn1( WARN_JUNK_FOLLOWS_DIRECTIVE, ERR_JUNK_FOLLOWS_DIRECTIVE );
            }
            Flush2EOL();
        }
    } else {
        ChkEOL();
    }
}


local void CElse()
{
    if( NestLevel == 0  ||      CppStack->cpp_type == PRE_ELSE ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = 0;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* cpp_type will be PRE_ELIF if an elif was true */
            if( CppStack->cpp_type == PRE_IF ) {        /* 19-sep-88 */
                SkipLevel = NestLevel;  /* start including else part */
                CppStack->processing = 1;
            }
        }
        CppStack->cpp_type = PRE_ELSE;
    }
    PPNextToken();
    WantEOL();
}


local void CEndif()
{

    if( NestLevel == 0 ) {
        CErr1( ERR_MISPLACED_ENDIF );
    } else {
        struct cpp_info *cpp;

        --NestLevel;
        cpp = CppStack;
        if( cpp->file_name != ErrFName ) {
             CWarn( WARN_LEVEL_1, ERR_WEIRD_ENDIF_ENCOUNTER, cpp->file_name  );
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

extern bool MacroDel( char *name ){
/*********************************/
    MEPTR       mentry;
    MEPTR       prev_entry;
    int         len;
    bool        ret;

    ret = FALSE;
    if( strcmp( name, "defined" ) == 0 ) {
        CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name  );
        return( ret );
    }
    prev_entry = NULL;
    len = strlen( name ) + 1;
    mentry = MacHash[ MacHashValue ];
    while( mentry != NULL ) {
        if( far_strcmp( mentry->macro_name, name, len ) == 0 ) break;
        prev_entry = mentry;
        mentry = mentry->next_macro;
    }
    if( mentry != NULL ) {
        if( mentry->macro_defn == 0 ) {
            CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name );
        } else {
            if( prev_entry != NULL ) {
                prev_entry->next_macro = mentry->next_macro;
            } else {
                MacHash[ MacHashValue ] = mentry->next_macro;
            }
            if(( InitialMacroFlag & MACRO_DEFINED_BEFORE_FIRST_INCLUDE ) == 0 ) {
                /* remember macros that were defined before first include */
                if( mentry->macro_flags & MACRO_DEFINED_BEFORE_FIRST_INCLUDE ) {
                    mentry->next_macro = UndefMacroList;
                    UndefMacroList = mentry;
                }
            }
            ret = TRUE;
        }
    }
    return( ret );
}


local void CUndef()
{

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return;
    }
    MacroDel( Buffer );
    PPNextToken();
    ChkEOL();
}


local void ChkEOL()
{
    if( CurToken != T_NULL  &&  CurToken != T_EOF ) { /* 15-dec-91 */
        ExpectEndOfLine();
    }
}


local void CLine()
{
    FNAMEPTR    flist;
    unsigned long src_line;

    CompFlags.pre_processing = 1;
    PPNextToken();
    if( CurToken != T_CONSTANT ) {
        ExpectConstant();
        return;
    }
    src_line = Constant; // stash in case of side effects
    SrcFile->src_line = src_line - 1; /* don't count this line */
    PPNextToken();
    if( CurToken != T_NULL ) {
        if( CurToken != T_STRING  ||  CompFlags.wide_char_string ) {
            /* wide char string not allowed, 26-mar-91 */
            ExpectString();
            return;
        }
//      RemoveEscapes( Buffer );                /* 04-apr-91 */
        flist = AddFlist( Buffer );
        flist->rwflag = FALSE;  // not a real file so no autodep
        SrcFile->src_name = flist->name;
        SrcFile->src_fno  = flist->index;
        TokenFno = flist->index;
        SrcFile->src_flist = flist;             /* 21-dec-93 */
        ErrFName = SrcFile->src_name;
        if( CompFlags.cpp_output ) {            /* 30-may-95 */
            EmitLine( src_line, SrcFile->src_name );
        }
        PPNextToken();
        ChkEOL();
    } else {
        if( CompFlags.cpp_output ) {            /* 30-may-95 */
            EmitLine( src_line, SrcFile->src_name );
        }
    }
}


local void CError()
{
    int i;
    int save;

    i = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != EOF_CHAR ) {
        if( i != 0 || CurrChar != ' ' ) {
            Buffer[ i ] = CurrChar;
            ++i;
        }
        NextChar();
    }
    Buffer[ i ] = '\0';
    /* Force #error output to be reported, even with preprocessor */
    save = CompFlags.cpp_output;
    CompFlags.cpp_output = 0;
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
    CompFlags.cpp_output = save;
}

void CppStackInit( void )
{
    NestLevel = 0;
    SkipLevel = 0;
    CppStack = NULL;
}

void CppStackFini(void)
{
    struct cpp_info *cpp;

    while( cpp = CppStack ) {
        SetErrLoc( cpp->file_name, cpp->line_num );
        CErr1( ERR_MISSING_CENDIF );
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    CppStack = NULL;
}

