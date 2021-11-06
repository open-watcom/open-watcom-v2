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
* Description:  Definitions for preprocessing.
*
****************************************************************************/


#ifndef _PREPROC_H_
#define _PREPROC_H_

#include <stdio.h>

#include "ctokens.h"            // tokens
#include "srcfile.h"            // source files
#include "ftype.h"              // source file types

#include "macro.h"              // macro structures

#include "i64.h"

#include "pragdefn.h"


#define BUF_SIZE_SHIFT          (9)
#define BUF_SIZE                (1<<BUF_SIZE_SHIFT)

#define IS_ID_OR_KEYWORD(t)     (t == T_ID || t >= FIRST_KEYWORD && t <= LAST_KEYWORD)

#define PPOPERATOR_DEFINED          "defined"
#define PPOPERATOR_PRAGMA           "_Pragma"

#define IS_PPOPERATOR_DEFINED(s)    (strcmp(s, PPOPERATOR_DEFINED) == 0)
#define IS_PPOPERATOR_PRAGMA(s,l)   ((CompFlags.extensions_enabled || CompFlags.enable_std0x) \
                                    && l == (sizeof(PPOPERATOR_PRAGMA) - 1) \
                                    && memcmp(s, PPOPERATOR_PRAGMA, sizeof(PPOPERATOR_PRAGMA)) == 0)

enum {
    EL_NEW_LINE = 0x01,
    EL_NULL     = 0
};

typedef void token_source_fn( void );

//typedef target_ulong target_int_const;

typedef enum ppctl_t {
    PPCTL_NO_EXPAND       = 0x01, // don't expand macros
    PPCTL_EOL             = 0x02, // return <end-of-line> as a token
    PPCTL_NO_LEX_ERRORS   = 0x04, // don't diagnose lexical problems
    PPCTL_ASM             = 0x08, // pre-processor is in _asm statement
    PPCTL_NORMAL          = 0x00, // expand macros, treat <eol> as white space
    PPCTL_NULL            = 0x00
} ppctl_t;

#define PPCTL_ENABLE_ASM()            PPControl |= PPCTL_ASM
#define PPCTL_DISABLE_ASM()           PPControl &= ~PPCTL_ASM
#define PPCTL_ENABLE_EOL()            PPControl |= PPCTL_EOL
#define PPCTL_DISABLE_EOL()           PPControl &= ~PPCTL_EOL
#define PPCTL_ENABLE_MACROS()         PPControl &= ~PPCTL_NO_EXPAND
#define PPCTL_DISABLE_MACROS()        PPControl |= PPCTL_NO_EXPAND
#define PPCTL_ENABLE_LEX_ERRORS()     PPControl &= ~PPCTL_NO_LEX_ERRORS
#define PPCTL_DISABLE_LEX_ERRORS()    PPControl |= PPCTL_NO_LEX_ERRORS

// PREPROCESSOR DATA:

global  ppctl_t     PPControl;          // pre-processor control bits
global  TOKEN       CurToken;           // current token
global  MSG_NUM     BadTokenInfo;       // error message that describes why T_BAD_TOKEN is bad
global  size_t      TokenLen;           // length of current token
global  LINE_NO     TokenLine;          // line # of current token
global  COLUMN_NO   TokenColumn;        // column # of current token
global  int         CurrChar;           // current character
global  type_id     ConstType;          // type of constant
global  signed_64   Constant64;         // value of constant: 33-64 bits
global  int         NestLevel;          // pre-processing level of #if
global  int         SkipLevel;          // pre-processing level of #if to skip to
global  NAME        SavedId;            // saved id when doing look ahead
global  TOKEN       LAToken;            // look ahead token
global  macro_flags InitialMacroFlags;  // current value to init macro flags to
global  char        *MacroOffset;       // first free byte in MacroSegment
global  char        __Time[9];          // "HH:MM:SS" for __TIME__ macro
global  char        __Date[12];         // "MMM DD YYYY" for __DATE__ macro
global  FILE        *CppFile;           /* output for preprocessor */
global  char        *ForceInclude;
global  char        *ForcePreInclude;
global  char        *SrcFName;          /* source file name without suffix */
global  char        *WholeFName;        /* whole file name with suffix */
global  char        PreProcChar;        /* preprocessor directive indicator */
global  int         SwitchChar;         // DOS switch character

// token buffer, dynamicaly allocated
global  char        *Buffer;

extern  int         (*NextChar)( void );    // next-character routine (initialized in SRCFILE)

// PROTOTYPES: exposed to C++ project

void MacroStateClear( MACRO_STATE * );
void MacroStateGet( MACRO_STATE * );
bool MacroStateMatchesCurrent( MACRO_STATE * );

// provide a temporary source of tokens
token_source_fn *SetTokenSource( token_source_fn * );

// restore source of tokens
void ResetTokenSource( token_source_fn * );

bool TokenUsesBuffer( TOKEN t );

void GetNextToken(              // GET THE NEXT TOKEN FOR PROCESSOR
    void )
;
unsigned IfDepthInSrcFile(      // COMPUTE #IF DEPTH IN CURRENT SOURCE FILE
    void )
;
void LookPastName(              // SCAN PAST ID (makes a name out of id)
    void )
;
void UndoNextToken(             // make NextToken return the current token
    void )
;
void MacroDefsSysind            // SYSTEM-INDEPENDENT MACRO DEFINITIONS
    ( void )
;
bool MacroDependsDefined        // MACRO DEPENDENCY: DEFINED OR NOT
    ( void )
;
bool MacroExists(               // TEST IF MACRO EXISTS
    const char *macname,        // - macro name
    size_t len )                // - length of macro name
;
void MacroFini(                 // MACRO PROCESSING -- COMPLETION
    void )
;
void MacroInit(                 // MACRO PROCESSING -- INITIALIZATION
    void )
;
void PCHDumpMacroCheck(         // DUMP MACRO CHECK INFO INTO PCHDR
    void )
;
bool PCHVerifyMacroCheck(       // READ AND VERIFY MACRO CHECK INFO FROM PCHDR
    void )
;
void MacroCanBeRedefined(       // SET MACRO SO THAT USE CAN REDEFINE IN SOURCE
    MEPTR mentry )              // - the macro entry
;
MEPTR MacroScan(                // SCAN AND DEFINE A MACRO (#define, -d)
    macro_scanning defn )       // - scanning definition
;
bool OpenSrcFile(               // OPEN A SOURCE FILE
    const char * filename,      // - file name
    src_file_type typ )         // - source file type
;
void PpInit(                    // INITIALIZE PREPROCESSING
    void )
;
void PpOpen(                    // OPEN PREPROCESSOR OUTPUT
    void )
;
void PpParse(                   // PARSE WHEN PREPROCESSING
    void )
;
unsigned PpVerifyWidth(         // VERIFY WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
;
void PpSetWidth(                // SET WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
;
void CppEmitPoundLine(          // EMIT #LINE DIRECTIVE, IF REQ'D
    LINE_NO line_num,           // - line number
    const char *filename,       // - file name
    unsigned control )          // - emit control
;
void PpStartFile(               // INDICATE START OF A FILE
    void )
;
void ScanInit(                  // SCAN INITIALIZATION
    void )
;
bool InitPPScan(                // INIT SCANNER FOR PPNUMBER TOKENS
    void )
;
void FiniPPScan(                // INIT SCANNER FOR PPNUMBER TOKENS
    bool ppscan_mode )          // - mode returned by InitPPScan()
;
TOKEN SpecialMacro(             // EXECUTE A SPECIAL MACRO
    MEPTR mentry )              // - macro entry
;
void DefineAlternativeTokens(   // DEFINE ALTERNATIVE TOKENS
    void )
;
AUX_INFO *PragmaLookup(         // FIND A PRAGMA
    const char *name )          // - name of the pragma
;
AUX_INFO *PragmaLookupMagic(    // FIND A PRAGMA
    magic_words mword )         // - magic index
;
AUX_INFO *PragmaGetIndex( AUX_INFO * );

AUX_INFO *PragmaMapIndex( AUX_INFO * );

// PROTOTYPES: internal to scanner

TOKEN ChkControl(               // CHECK AND PROCESS DIRECTIVES
    bool expanding )
;
bool CppPrinting(               // TEST IF AT LEVEL FOR PREPROC PRINTING
    void )
;
void CPragma(                   // #PRAGMA PROCESSING
    void )
;
void DirectiveFini(             // COMPLETE DIRECTIVE PROCESSING
    void )
;
void DirectiveInit(             // INITIALIZE FOR DIRECTIVE PROCESSING
    void )
;
void DoMacroExpansion(          // EXPAND A MACRO
    MEPTR mentry )              // - macro to expand
;
void Expecting(                 // ISSUE EXPECTING ERROR FOR A TOKEN
    const char *a_token )       // - required token
;
const char *TokenString(        // RETURN A PRINTABLE STRING FOR CURRENT TOK
    void )
;
bool ExpectingToken(            // ISSUE EXPECTING ERROR FOR A TOKEN
    TOKEN token )               // - required token
;
TOKEN GetMacroToken(            // GET NEXT TOKEN
    bool doing_macro_expansion )// - true ==> doing an expansion
;
int GetNextChar(                // GET NEXT CHARACTER FROM A SOURCE FILE
    void )
;
void GetNextCharUndo(           // UNDO PREVIOUS GET NEXT CHARACTER
    int c )                     // - character to undo
;
AUX_INFO *GetTargetHandlerPragma // GET PRAGMA FOR FS HANDLER
    ( void )
;
TOKEN KwLookup(                 // TRANSFORM TO T_ID OR KEYWORD TOKEN
    size_t len )                // - length of id
;
void KwDisable(                 // DISABLE A KEYWORD TOKEN TO T_ID
    TOKEN token )               // - token id
;
void KwEnable(                  // ENABLE A KEYWORD TOKEN FROM T_ID
    TOKEN token )               // - token id
;
MEPTR MacroSpecialAdd(          // ADD A SPECIAL MACRO
    const char *name,           // - macro name
    special_macros value,       // - value for special macro
    macro_flags mflags )        // - macro flags
;
MEPTR MacroDefine(              // DEFINE A NEW MACRO
    MEPTR mentry,               // - scanned macro
    size_t mlen,                // - length of entry
    macro_flags mflags )        // - macro flags
;
MEPTR MacroLookup(              // LOOKUP NAME AS A MACRO
    char const *name,           // - name
    size_t len )                // - length of name
;
void MacroReallocOverflow(      // OVERFLOW SEGMENT IF REQUIRED
    size_t amount_needed,       // - amount for macro
    size_t amount_used )        // - amount used in segment
;
void MacroStorageInit(          // INITIALIZE FOR MACRO STORAGE
    void )
;
void MacroStorageFini(          // RELEASE MACRO STORAGE
    void )
;
void MacroUndefine(             // UNDEFINE CURRENT NAME AS MACRO
    size_t len )                // - length of macro name
;
void MacroCmdLnUndef(           // -U<macro-name>
    char *name,                 // - macro name
    size_t len )                // - length of macro name
;
void MustRecog(                 // REQUIRE A SPECIFIC TOKEN AND SCAN NEXT
    TOKEN token )               // - token to be recognized
;
TOKEN NextToken(                // GET NEXT TOKEN
    void )
;
bool PpConstExpr(               // PREPROCESSOR CONSTANT EXPRESSION
    void )
;
void PrtChar(                   // PRINT PREPROC CHAR IF REQ'D
    int c )                     // - character to be printed
;
void PrtString(                 // PRINT CPP STRING
    const char *str )           // - string to be printed
;
void PrtToken(                  // PRINT PREPROC TOKEN IF REQ'D
    void )
;
void ReScanInit(                // RE-SCAN TOKEN INITIALIZATION
    const char *buf )
;
TOKEN ReScanToken(              // RE-SCAN TOKEN FROM BUFFER
    void )
;
bool DoScanOptionalComment(     // SCAN AN OPTIONAL COMMENT
    void )
;
TOKEN ScanToken(                // SCAN NEXT TOKEN
    bool doing_macro_expansion )// - true ==> expanding
;
void SkipAhead(                 // SKIP AHEAD TO SIGNIFICANT LOCATION
    void )
;
void TimeInit(                  // INITIALIZE TIME
    void )
;
time_t TimeOfCompilation(       // GET TIME OF COMPILATION
    void )
;
extern void         InsertReScanPragmaTokens( const char *pragma, bool internal );
extern void         InsertToken( TOKEN token, const char *str, bool internal );
extern TOKEN        Process_Pragma( bool internal );
#endif
