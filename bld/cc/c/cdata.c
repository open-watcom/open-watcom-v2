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
* Description:  InitGlobalVars() initialises all global variables from cfe;
*               Variables are, unfortunately, declared all over the joint.
*
****************************************************************************/


#define global
#include "cvars.h"


void InitGlobalVars( void )
{
    PCH_Start               = NULL; // start of precompiled memory block
    PCH_End                 = NULL; // end of precompiled memory block
    PCH_Macros              = NULL; // macros loaded from pre-compiled header
    PCH_FileName            = NULL; // name to use for pre-compiled header
    IncFileList             = NULL; // list of primary include files for PCH
    PCH_SymArray            = NULL; // array of symbol table pointers from PCH
    PCH_MaxSymHandle        = 0;    // number of symbols in PCH_SymArray
    DebugFlag               = 0;
    CurToken                = T_NULL;
    BadTokenInfo            = 0;
    TokenLen                = 0;
    TokenLoc.line           = 0;
    TokenLoc.fno            = 0;
    SrcFileLoc.line         = 0;
    SrcFileLoc.fno          = 0;
    CommentLoc.line         = 0;
    CommentLoc.fno          = 0;
    CurrChar                = 0;
    ConstType               = 0;
    Constant                = 0;
    MainSrcFile             = NULL; /* primary source file being compiled */
    SrcFile                 = NULL;
    SrcFName                = NULL; /* source file name without suffix */
    DefFName                = NULL; /* .def file name (prototypes) */
    WholeFName              = NULL; /* whole file name with suffix */
    ForceInclude            = NULL;
#if _CPU == 370
    AuxName                 = NULL;
#endif
    FNames                  = NULL; /* list of file names processed */
    ErrFile                 = NULL; /* error file */
    DefFile                 = NULL; /* output for func prototypes */
    CppFile                 = NULL; /* output for preprocessor */
    CppStack                = NULL; /* #if structure control stack */
    IncPathList             = NULL; /* list of path names to try for include files */
    SrcLoc.line             = 0;
    SrcLoc.fno              = 0;
    SrcLineCount            = 0;    /* # of lines in primary source file */
    IncLineCount            = 0;    /* # of lines in all included files  */
    ErrCount                = 0;    /* total # of errors encountered     */
    WngCount                = 0;    /* total # of warnings encountered   */
    WngLevel                = 0;    /* warning severity level */
    TypeCount               = 0;    /* total # of type nodes allocated   */
    GblSymCount             = 0;    /* total # of global symbols */
    LclSymCount             = 0;    /* total # of local and temporary symbols */
    FuncCount               = 0;    /* total # of functions defined in module */
    SizeOfCount             = 0;    /* # of nested sizeof() expressions  */
    SymLevel                = 0;    /* current lex level (# of nested {) */
    HashValue               = 0;    /* hash value for identifier */
    MacHashValue            = 0;    /* hash value for macro name */
    SavedId                 = NULL; /* saved id when doing look ahead */
    SavedHash               = 0;    /* hash value for saved id */
    SavedTokenLoc.line      = 0;    /* value of TokenLine when id saved */
    SavedTokenLoc.fno       = 0;    /* value of TokenFno when id saved */
    LAToken                 = 0;    /* look ahead token */
    LabelHead               = NULL; /* list of all labels defined in function */
    TagHead                 = NULL; /* list of all struct, union, enum tags */
    DeadTags                = NULL; /* list of all tags that are out of scope */
    CurFunc                 = NULL; /* current function being worked on */
    ParmList                = NULL; /* list of parms for function */
    GlobalSym               = 0;    /* global symbol table list head */
#if _CPU == 386
    SymSTOD                 = 0;    /* builtin symbol for 'rep stosd' */
    SymSTOSB                = 0;    /* builtin symbol for '__STOSB' */
    SymSTOSD                = 0;    /* builtin symbol for '__STOSD' */
#endif
#ifdef __SEH__
    SymTryInit              = 0;    /* builtin symbol for '__TryInit' */
    SymTryFini              = 0;    /* builtin symbol for '__TryFini' */
    SymExcept               = 0;    /* builtin symbol for '__Except' */
    SymFinally              = 0;    /* builtin symbol for '__Finally' */
    SymTryUnwind            = 0;    /* builtin symbol for '__TryUnwind' */
    TrySymHandle            = 0;    /* builtin symbol for local try block */
    TryScope                = 0;    /* current scope of _try blocks */
    TryCount                = 0;    /* current number of _try blocks */
#endif
    SymSTOW                 = 0;    /* builtin symbol for 'rep stosw' */
    SymSTOWB                = 0;    /* builtin symbol for 'rep stosw, stosb' */
    SymMIN                  = 0;    /* builtin symbol for 'min(a,b)' */
    SymMAX                  = 0;    /* builtin symbol for 'max(a,b)' */
    SymMEMCMP               = 0;    /* builtin symbol for 'memcmp' func */
    SpecialSyms             = 0;    /* builtin symbols (thread linked) */
    CharSymHandle           = 0;    /* sym handle for "char" typedef */
    Sym_CS                  = 0;    /* sym handle for __segname("_CODE") ie. CS */
    Sym_SS                  = 0;    /* sym handle for __segname("_STACK")ie. SS */
    SymCover                = 0;    /* sym handle for '__COVERAGE' */
    SymDFAbbr               = 0;    /* sym handle for '__DFABBREV' */
    SymChipBug              = 0;    /* sym handle for '__chipbug' */
    ErrSym                  = NULL;

#if _CPU == 386
    FunctionProfileBlock    = NULL; /* handle for profiling data block */
    FunctionProfileSegment  = SEG_UNKNOWN; /* segment for profiling data block */
#endif

    MacroDepth              = 0;
    NextMacro               = NULL;
    HashTab                 = NULL;

    GenSwitches             = 0;    /* target independant switches for code generator */
    TargetSwitches          = 0;    /* target specific code generator switches */

    ProcRevision            = 0;    /* processor revision for c.g. */
    GenCodeGroup            = NULL; /* pointer to code group name */
    ProEpiDataSize          = 0;    /* data to be alloc'd for pro/epi hook */
    Toggles                 = 0;    /* global toggle flags */
    ErrLimit                = 0;

    DataThreshold           = 0;    /* sizeof(obj) > this ==> separate segment */
    Inline_Threshold        = 0;    /* -oe=num for function inlining */

    DataPtrSize             = 0;
    CodePtrSize             = 0;
    DeadCode                = 0;    /* non-zero => next stmt is unreachable */
    TmpSymCount             = 0;
    LitCount                = 0;
    LitPoolSize             = 0;
    MacroSize               = 0;
    SegmentNum              = 0;    /* next PRIVATE segment number to use */
    FarStringSegment        = SEG_UNKNOWN;

    Environment             = NULL; /* var for Suicide() */


    /* The following 3 arrays are also used by CGEN for saving _try block info */
    ExprLevel               = 0;

    SegListHead             = NULL;
    SegImport               = SEG_UNKNOWN; /* next segment # for import sym */
    SegData                 = SEG_UNKNOWN; /* data seg # for -nd option */

    ScopeStruct             = 0;
    ScopeUnion              = 0;
    ScopeEnum               = 0;

    B_Int_1                 = 0;
    B_UInt_1                = 0;
    B_Short                 = 0;
    B_UShort                = 0;
    B_Int                   = 0;
    B_UInt                  = 0;
    B_Int32                 = 0;
    B_UInt32                = 0;
    B_Int64                 = 0;
    B_UInt64                = 0;
    B_Bool                  = 0;

    OptSize                 = 0;    /* 100 => make pgm small as possible */
    MsgFlags                = NULL; /* Bit mask of disabled messages */
    MacSegList              = NULL; /* pointer to list of macro segments */
    LoopDepth               = 0;    /* current nesting of loop constructs */
    HeadLibs                = 0;    /* list of library search records */
    AliasHead               = 0;    /* list of symbol alias records */
    CurFuncHandle           = 0;    /* sym_handle for current function */
    LastFuncOutOfMem        = 0;    /* cinfo: */
    HashFreeList            = NULL; /* list of available hash entries */

    SymBufSegNum            = 0;    /* segment # containing buffer */

    StringType              = NULL; /* "unsigned char *" for use by literals */
    ConstCharType           = NULL; /* "const char" type */
    StringArrayType         = NULL; /* "unsigned char []" used by literals */
    NestedParms             = NULL;

    TextSegName             = NULL; /* name of the text segment */
    DataSegName             = NULL; /* name of the data segment */
    CodeClassName           = NULL; /* name of the code class */
    ModuleName              = NULL; /* name of module */
    ObjectFileName          = NULL; /* name of object file */
    DependFileName          = NULL; /* Name of make style auto depend file */
    DependHeaderPath        = NULL; /* If no path is part of an included file, use this*/
    DependForceSlash        = 0;
    PackAmount              = 0;    /* current packing alignment */
    GblPackAmount           = 0;    /* packing alignment given on command line */
    Column                  = 0;    /* skip to Column when reading */
    Trunc                   = 0;    /* stop at Trunc when reading  */
    PrevProtoType           = NULL; /* prev func prototype */

    TargSys                 = TS_OTHER;
    DefDataSegment          = SEG_UNKNOWN; /* #pragma data_seg("segname","class") */
    DefCodeSegment          = NULL; /* #pragma code_seg("seg","c") */

    UnrollCount             = 0;    /* #pragma unroll(#); */
    InitialMacroFlag        = MFLAG_NONE;
    Stack87                 = 0;
    ErrorFileName           = NULL;

    UndefNames              = NULL;

    memset( &CompFlags, 0, sizeof( CompFlags ) );
    InitStmt();
    InitErrLoc();
}
