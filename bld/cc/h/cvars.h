/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C compiler global variables.
*
****************************************************************************/


#define BY_C_FRONT_END
#ifndef global
    #define global  extern
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "target.h"
#include "cconst.h"
#include "dw.h"
#include "ctypes.h"
#include "macro.h"
#include "cfcb.h"
#include "csegid.h"
#include "ctokens.h"
#include "cerrs.h"
#include "cmsg.h"
#include "pragdefn.h"

global char         *PCH_Start;         // start of precompiled memory block
global char         *PCH_End;           // end of precompiled memory block
global char         *PCH_Macros;        // macros loaded from pre-compiled header
global char         *PCH_FileName;      // name to use for pre-compiled header
global INCFILE      *IncFileList;       // list of primary include files for PCH
global SYMPTR       *PCH_SymArray;      // array of symbol table pointers from PCH
global unsigned     PCH_MaxSymHandle;   // number of symbols in PCH_SymArray
global int          DebugFlag;
global TOKEN        CurToken;
global msg_codes    BadTokenInfo;
global size_t       TokenLen;
global source_loc   TokenLoc;
global source_loc   SrcFileLoc;         /* duplicate of SrcFile->src_line */
global source_loc   CommentLoc;
global int          CurrChar;
global DATA_TYPE    ConstType;
global unsigned     Constant;
global uint64       Constant64;
global FCB          *MainSrcFile;       /* primary source file being compiled */
global FCB          *SrcFile;
global char         *SrcFName;          /* source file name without suffix */
global char         *DefFName;          /* .def file name (prototypes) */
global char         *WholeFName;        /* whole file name with suffix */
global char         *ForceInclude;
global char         *ForcePreInclude;
#if _CPU == 370
global char         *AuxName;
#endif
global FNAMEPTR     FNames;             /* list of file names processed */
global RDIRPTR      RDirNames;          /* list of read only directorys */
global IALIASPTR    IAliasNames;        /* list of include aliases */
global FILE         *ErrFile;           /* error file */
global FILE         *DefFile;           /* output for func prototypes */
global FILE         *CppFile;           /* output for preprocessor */
global FILE         *DepFile;           /* make style auto depend file */
global struct cpp_info *CppStack;       /* #if structure control stack */
global char         *IncPathList;       /* list of path names to try for include files */
global source_loc   SrcLoc;
global unsigned     SrcLineCount;       /* # of lines in primary source file */
global unsigned     IncLineCount;       /* # of lines in all included files  */
global unsigned     ErrCount;           /* total # of errors encountered     */
global unsigned     WngCount;           /* total # of warnings encountered   */
global int          WngLevel;           /* warning severity level */
global int          TypeCount;          /* total # of type nodes allocated   */
global int          GblSymCount;        /* total # of global symbols */
global int          LclSymCount;        /* total # of local and temporary symbols */
global int          FuncCount;          /* total # of functions defined in module */
global int          MacroCount;         /* total # of macros defined */
global int          TagCount;           /* total # of tag entries */
global int          FieldCount;         /* total # of struct field names */
global int          EnumCount;          /* total # of enumerated symbols */
global int          SizeOfCount;        /* # of nested sizeof() expressions  */
global int          NestLevel;          /* pre-processing level of #if */
global int          SkipLevel;          /* pre-processing level of #if to skip to */
global id_level_stype SymLevel;         /* current lex level (# of nested {) */
global bool         Check_global_prototype;
#define ChkEqSymLevel(p)  ((p)->level == (id_level_type)SymLevel)
#define ChkLtSymLevel(p)  ((p)->level < (id_level_type)SymLevel)
global id_hash_idx  HashValue;          /* hash value for identifier */
global mac_hash_idx MacHashValue;       /* hash value for macro name */
global char         *SavedId;           /* saved id when doing look ahead */
global id_hash_idx  SavedHash;          /* hash value for saved id */
global source_loc   SavedTokenLoc;      /* value of TokenLine when id saved */
global TOKEN        LAToken;            /* look ahead token */
global LABELPTR     LabelHead;          /* list of all labels defined in function */
global TAGPTR       TagHead;            /* list of all struct, union, enum tags */
global TAGPTR       DeadTags;           /* list of all tags that are out of scope */
global TYPEPTR      TypeHead;           /* list of all type nodes allocated */
global TYPEPTR      FuncTypeHead[MAX_PARM_LIST_HASH_SIZE + 1];
global TYPEPTR      VoidParmList[2];    /* function with void parm */
global PARMPTR      ParmList;           /* list of parms for function */
global SYM_HANDLE   GlobalSym;          /* global symbol table list head */
#if _CPU == 386
global SYM_HANDLE   SymSTOD;            /* builtin symbol for 'rep stosd' */
global SYM_HANDLE   SymSTOSB;           /* builtin symbol for '__STOSB' */
global SYM_HANDLE   SymSTOSD;           /* builtin symbol for '__STOSD' */
#endif
#ifdef __SEH__
global SYM_HANDLE   SymTryInit;         /* builtin symbol for '__TryInit' */
global SYM_HANDLE   SymTryFini;         /* builtin symbol for '__TryFini' */
global SYM_HANDLE   SymExcept;          /* builtin symbol for '__Except' */
global SYM_HANDLE   SymFinally;         /* builtin symbol for '__Finally' */
global SYM_HANDLE   SymTryUnwind;       /* builtin symbol for '__TryUnwind' */
global SYM_HANDLE   TrySymHandle;       /* builtin symbol for local try block */
global tryindex_t   TryScope;           /* current scope of _try blocks */
global tryindex_t   TryCount;           /* current number of _try blocks */
#endif
global SYM_HANDLE   SymSTOW;            /* builtin symbol for 'rep stosw' */
global SYM_HANDLE   SymSTOWB;           /* builtin symbol for 'rep stosw, stosb' */
global SYM_HANDLE   SymMIN;             /* builtin symbol for 'min(a,b)' */
global SYM_HANDLE   SymMAX;             /* builtin symbol for 'max(a,b)' */
global SYM_HANDLE   SymMEMCMP;          /* builtin symbol for 'memcmp' func */
global SYM_HANDLE   SpecialSyms;        /* builtin symbols (thread linked) */
global SYM_HANDLE   CharSymHandle;      /* sym handle for "char" typedef */
global SYM_HANDLE   Sym_CS;             /* sym handle for __segname("_CODE") ie. CS */
global SYM_HANDLE   Sym_SS;             /* sym handle for __segname("_STACK")ie. SS */
global SYM_HANDLE   SymCover;           /* sym handle for '__COVERAGE' */
global SYM_HANDLE   SymDFAbbr;          /* sym handle for '__DFABBREV' */
global SYM_HANDLE   SymChipBug;         /* sym handle for '__chipbug' */
global FIELDPTR     ErrSym;

#if _CPU == 386
global BACK_HANDLE  FunctionProfileBlock;   /* handle for profiling data block */
global segment_id   FunctionProfileSegId;   /* segment for profiling data block */
#endif

global int          MacroDepth;
global char         *MacroPtr;
global MEPTR        UndefMacroList;
global MEPTR        *MacHash;           /* [MACRO_HASH_SIZE] */
global ENUMPTR      EnumTable[ID_HASH_SIZE];
global SYM_HASHPTR  *HashTab;
global TYPEPTR      BaseTypes[TYP_LAST_ENTRY];
global unsigned     CTypeCounts[TYP_LAST_ENTRY];

global char         *Buffer;
global char         *TokenBuf;

global unsigned     GenSwitches;        /* target independant switches for code generator */
global unsigned     TargetSwitches;     /* target specific code generator switches */

global unsigned     ProcRevision;       /* processor revision for c.g. */
global char         *GenCodeGroup;      /* pointer to code group name */
global unsigned     ProEpiDataSize;     /* data to be alloc'd for pro/epi hook */

global unsigned     ErrLimit;
#define ERRLIMIT_NOMAX  ((unsigned)-1)

global target_size  DataThreshold;      /* sizeof(obj) > this ==> separate segment */
global unsigned     Inline_Threshold;   /* -oe=num for function inlining */

global unsigned     DataPtrSize;
global unsigned     CodePtrSize;
global int          DeadCode;           /* non-zero => next stmt is unreachable */
global int          TmpSymCount;
global int          LitCount;
global target_size  LitPoolSize;
global size_t       MacroSize;
global ppctl_t      PPControl;
global comp_flags   CompFlags;
global global_comp_flags GlobalCompFlags;
global segment_id   SegmentNum;         /* next PRIVATE segment number to use */
global segment_id   FarStringSegId;

global jmp_buf      *Environment;       /* var for Suicide() */

#define MAX_LEVEL   1024

/* The ValueStack array is also used by CGEN for saving _try block info */
global TREEPTR      ValueStack[MAX_LEVEL];
global TOKEN        Token[MAX_LEVEL];
global token_class  Class[MAX_LEVEL];
global expr_level_type ExprLevel;

global segment_list *SegListHead;

global TREEPTR      FirstStmt;          /* root of expression tree */

global dbug_type    ScopeStruct;
global dbug_type    ScopeUnion;
global dbug_type    ScopeEnum;

global dbug_type    B_Int_1;
global dbug_type    B_UInt_1;
global dbug_type    B_Short;
global dbug_type    B_UShort;
global dbug_type    B_Int;
global dbug_type    B_UInt;
global dbug_type    B_Int32;
global dbug_type    B_UInt32;
global dbug_type    B_Int64;
global dbug_type    B_UInt64;
global dbug_type    B_Bool;

global int          OptSize;            /* 100 => make pgm small as possible */
global char         __Time[9];          /* "HH:MM:SS" for __TIME__ macro */
global char         __Date[12];         /* "MMM DD YYYY" for __DATE__ macro */

global int          SwitchChar;         /* DOS switch character */
global int          LoopDepth;          /* current nesting of loop constructs */
global char         CLIB_Name[10];      /* "1CLIBMT3x" */
global char         MATHLIB_Name[10];   /* "5MATHx" or "8MATH87x" */
global char         *EmuLib_Name;       /* "9emu87" for -fpi, "9noemu87" for -fpi */

#define USER_LIB_PRIO '9'

typedef struct library_list {
    struct  library_list    *next;      // used by precompiled header
    char                    libname[2]; // first char is priority '1'-'9' followed by library name
} library_list;

global library_list *HeadLibs;

typedef struct alias_list {
    struct  alias_list      *next;
    const char              *name;      /* one of 'name', 'a_sym' is valid */
    SYM_HANDLE              a_sym;
    const char              *subst;
    SYM_HANDLE              s_sym;      /* one of 'subst', 's_sym' is valid */
    char                    names[1];
} alias_list;

global alias_list   *AliasHead;

global SYMPTR       CurFunc;            /* current function being worked on */
global SYM_ENTRY    CurFuncSym;         /* for contents of current function symbol */
global SYM_HANDLE   CurFuncHandle;      /* sym_handle for current function */
global CGPOINTER    LastFuncOutOfMem;   /* cinfo: */
global SYM_HASHPTR  HashFreeList;       /* list of available hash entries */

global unsigned     SymBufNum;          /* current buffer in memory */
global unsigned     SymBufSegNum;       /* segment # containing buffer */
global unsigned     LastSymBuf;         /* # of last symbol table buffer */
global unsigned     SymBufDirty;        /* 1 => buffer has been changed */

global TYPEPTR      StringType;         /* "unsigned char *" for use by literals */
global TYPEPTR      ConstCharType;      /* "const char" type */
global TYPEPTR      StringArrayType;    /* "unsigned char []" used by literals */

typedef struct nested_parm_lists {
    struct nested_parm_lists    *prev_list;
    TYPEPTR                     *next_parm_type;
} nested_parm_lists;

global nested_parm_lists    *NestedParms;

global STR_HANDLE   StringHash[STRING_HASH_SIZE]; /* string literals */
global char         *TextSegName;       /* name of the text segment */
global char         *DataSegName;       /* name of the data segment */
global char         *CodeClassName;     /* name of the code class */
global char         *ModuleName;        /* name of module */
global char         *ObjectFileName;    /* name of object file */
global char         *DependFileName;    /* name of make style auto depend file */
global char         *DependHeaderPath;  /* path to prepend if included header has none */
global char         *DependTarget;      /* name of target in make style autodep */
global char         *SrcDepName;        /* name of first depend (sourcefile)*/
global char         DependForceSlash;   /* type of slash to force to in depend output */
global align_type   PackAmount;         /* current packing alignment */
global align_type   GblPackAmount;      /* packing alignment given on command line */
global textsegment  *TextSegList;       /* list of #pragma alloc_text segs*/

typedef enum {
    SEGTYPE_CODE       = 1,             /* #pragma code_seg("segname","class") */
    SEGTYPE_DATA       = 2,             /* #pragma data_seg("segname","class") */
    SEGTYPE_BASED      = 3,             /* __based(__segname("segname")) */
    SEGTYPE_INITFINI   = 4,             /* "XI" or "YI" segment */
    SEGTYPE_INITFINITR = 5,             /* thread data */
} seg_type;

typedef struct extref_info {
    struct extref_info  *next;
    SYM_HANDLE          symbol;
    char                name[1];
} extref_info;

global extref_info  *ExtrefInfo;

global int          (*NextChar)( void );
global void         (*UnGetChar)( int c );
global int          (*GetCharCheck)( int );

global struct debug_fwd_types *DebugNameList;

global unsigned     Column;             /* skip to Column when reading */
global unsigned     Trunc;              /* stop at Trunc when reading  */
global size_t       EnumRecSize;        /* size of largest enum entry */
global int          PreProcChar;
global TYPEPTR      PrevProtoType;      /* prev func prototype */

global align_type   SegAlignment[FIRST_PRIVATE_SEGMENT];
global unsigned     TargSys;
global segment_id   DefDataSegment;     /* #pragma data_seg("segname","class") */
global textsegment  *DefCodeSegment;    /* #pragma code_seg("seg","c") */

global unroll_type  UnrollCount;        /* #pragma unroll(#); */
global macro_flags  InitialMacroFlags;
global unsigned char Stack87;
global char         *ErrorFileName;

typedef struct undef_names {
    struct undef_names  *next;
    char                *name;
} undef_names;

global undef_names  *UndefNames;

global TREEPTR      CurFuncNode;

//================= Function Prototypes ========================

/* casian.c */
extern void         SetDBChar(int);

/* ccheck.c */
extern bool         ChkCompatibleFunction( TYPEPTR typ1, TYPEPTR typ2, bool topLevelCheck );
extern bool         ChkCompatibleLanguage( type_modifiers typ1, type_modifiers typ2 );
extern void         TernChk( TYPEPTR typ1, TYPEPTR typ2 );
extern void         ChkCallParms(void);
extern void         ChkRetType(TREEPTR);
extern void         ChkConst(TREEPTR);
extern void         CompatiblePtrType(TYPEPTR,TYPEPTR,TOKEN);
extern bool         IdenticalType(TYPEPTR,TYPEPTR);
extern bool         VerifyType(TYPEPTR,TYPEPTR,SYMPTR);
extern TYPEPTR      SkipTypeFluff( TYPEPTR typ );
extern bool         AssRangeChk( TYPEPTR typ1, TREEPTR opnd2 );
extern void         ParmAsgnCheck( TYPEPTR typ1, TREEPTR opnd2, int parmno, bool asgn_check );

/* ccmain.c */
extern void         FreeRDir( void );
extern void         FrontEndInit( bool reuse );
extern bool         FrontEnd(char **);
extern void         FrontEndFini( void );
extern void         CppComment(int);
extern bool         CppPrinting(void);
extern void         CppPuts(const char *);
extern void         CppPutsQuoted(const char *);
extern void         SetCppWidth(unsigned);
extern void         CppPrtChar(int);
extern void         CppPrtToken(TOKEN);
extern bool         OpenSrcFile(const char *, src_file_type);
extern void         CloseSrcFile(FCB *);
extern void         OpenDefFile(void);
extern FILE         *OpenBrowseFile(void);
extern void         CloseFiles(void);
extern void         CClose( FILE *fp );
extern void         FreeFNames(void);
extern char         *ErrFileName(void);
extern char         *DepFileName(void);
extern char         *ObjFileName(void);
extern char         *CppFileName(void);
extern char         *ForceSlash(char *, char );
extern char         *GetSourceDepName( void );
extern FNAMEPTR     NextDependency( FNAMEPTR );
extern void         PrtfFilenameErr( const char *filename, src_file_type typ, bool print_error );

extern FNAMEPTR     AddFlist(char const *);
extern FNAMEPTR     FileIndexToFName(unsigned);
extern char         *FNameFullPath( FNAMEPTR flist );
extern char         *FileIndexToCorrectName( unsigned file_index );
extern void         SrcFileReadOnlyDir( char const *dirs );
extern void         SrcFileReadOnlyFile( char const *file );
extern bool         SrcFileInRDir( FNAMEPTR flist );
extern void         SrcFileIncludeAlias( const char *alias_name, const char *real_name, bool is_lib );
extern int          SrcFileTime(char const *,time_t *);
extern void         SetSrcFNameOnce( void );
extern TOKEN        GetNextToken(void);
extern void         CppEmitPoundLine(unsigned,const char *,bool);

extern void         AddIncFile( INCFILE * );
extern void         AddIncFileList( const char *filename );
extern void         FreeIncFileList( void );

/* cdata.c */
extern void         InitGlobalVars( void );

/* cdebug.c */
extern dbug_type    DBType(TYPEPTR);
extern void         EmitDBType(void);

extern void         ParsePgm(void);
extern bool         CheckFuncMain( const char *name );
extern void         AdjParmType(SYMPTR sym);
extern void         Chk_Struct_Union_Enum(TYPEPTR);
extern void         Declarator( SYMPTR sym, type_modifiers mod, TYPEPTR typ, decl_state state );
extern bool         DeclList(SYM_HANDLE *);
extern FIELDPTR     FieldDecl( TYPEPTR typ, type_modifiers mod, decl_state state );
extern TYPEPTR      TypeName(void);

/* cdecl2.c */
extern void         InvDecl( void );
extern parm_list    *NewParm( TYPEPTR, parm_list * );
extern TYPEPTR      *MakeParmList( parm_list *, bool );
extern FIELDPTR     FieldCreate( const char *name );
extern bool         LoopDecl( SYM_HANDLE *sym_head );

/* cdinit */
extern void         InitDataQuads(void);
extern void         FreeDataQuads(void);
extern bool         DataQuadsAvailable(void);
extern void         *StartDataQuadAccess( void );
extern void         EndDataQuadAccess( void * );
extern DATA_QUAD    *NextDataQuad(void);
extern void         InitSymData(TYPEPTR,TYPEPTR,int);
extern void         StaticInit(SYMPTR,SYM_HANDLE);
extern void         VarDeclEquals(SYMPTR,SYM_HANDLE);

/* cdump */
extern void         DumpFuncDefn(void);
extern void         SymDump(void);
extern char         *DiagGetTypeName(TYPEPTR typ);

/* cenum */
extern TYPEPTR      EnumDecl(type_modifiers);
extern ENUMPTR      EnumLookup(id_hash_idx,const char *);
extern void         EnumInit(void);
extern void         FreeEnums(void);

/* cerror.c */
extern void         CErr1(msg_codes msgnum);
extern void         CErr2(msg_codes msgnum,int);
extern void         CErr2p(msg_codes msgnum,const char *);
extern void         CErr3p(msg_codes msgnum,const char *,const char *);
extern void         CErr4p(msg_codes msgnum,const char *,const char *,const char *);
extern void         CErrP1(int parmno,msg_codes msgnum);
extern void         SetErrLoc(source_loc *);
extern void         InitErrLoc(void);
extern void         CWarn1(int level,msg_codes msgnum);
extern void         CWarn2(int level,msg_codes msgnum,int);
extern void         CWarn2p(int level,msg_codes msgnum,const char *);
extern void         CWarnP1(int parmno,int level,msg_codes msgnum);
extern void         PCHNote( msg_codes msgnum, ... );
extern void         CInfoMsg(msg_codes msgnum,...);
extern void         CSuicide(void);
extern void         OpenErrFile(void);
extern void         FmtCMsg( char *buff, cmsg_info *info );
extern void         SetDiagSymbol(SYMPTR sym, SYM_HANDLE handle);
extern void         SetDiagEnum(ENUMPTR);
extern void         SetDiagMacro(MEPTR);
extern void         SetDiagType1(TYPEPTR typ_source);
extern void         SetDiagType2(TYPEPTR typ_target, TYPEPTR typ_source);
extern void         SetDiagType3(TYPEPTR typ_first, TYPEPTR typ_second, TOKEN opr);
extern void         SetDiagPop(void);

/* cexpr.c */
extern void         ExprInit(void);
extern void         ChkCallNode( TREEPTR tree );
extern TREEPTR      Expr(void);
extern TREEPTR      AddrExpr(void);
extern TREEPTR      BoolExpr(TREEPTR);
extern TREEPTR      CommaExpr(void);
extern int          ConstExpr(void);
typedef struct {
    int64     value;
    DATA_TYPE type;
} const_val;
extern bool         ConstExprAndType(const_val *);
extern TREEPTR      SingleExpr(void);
extern TREEPTR      IntLeaf(target_int);
extern TREEPTR      RValue(TREEPTR);
extern TREEPTR      LongLeaf(target_long);
extern TREEPTR      UIntLeaf(target_uint);
extern TREEPTR      VarLeaf(SYMPTR,SYM_HANDLE);
extern TREEPTR      BasedPtrNode(TYPEPTR,TREEPTR);
extern bool         IsLValue(TREEPTR);
extern op_flags     OpFlags( type_modifiers  flags );
extern type_modifiers FlagOps( op_flags ops );
extern FIELDPTR     SearchFields( TYPEPTR *class_typ, target_size *field_offset, const char *name );

/* cfold.c */
extern int64        LongValue64( TREEPTR leaf );
extern void         CastFloatValue( TREEPTR leaf, DATA_TYPE newtype );
extern void         CastConstValue(TREEPTR,DATA_TYPE);
extern void         CastConstNode( TREEPTR leaf, TYPEPTR newtyp );
extern void         DoConstFold(TREEPTR);
extern void         FoldExprTree(TREEPTR);

/* cgen.c */
extern void         DoCompile(void);
extern void         EmitInit(void);
extern void         EmitAbort(void);
extern target_size  EmitBytes(STR_HANDLE);
extern void         GenInLineFunc( SYM_HANDLE sym_handle );
extern bool         IsInLineFunc( SYM_HANDLE sym_handle );

/* cgendata */
extern void         EmitDataQuads(void);
extern void         EmitZeros(target_size);
extern void         AlignIt(TYPEPTR);

/* cgetch */
extern int          GetNextChar(void);
extern void         GetNextCharUndo(int);
extern int          GetCharCheckFile(int);
extern int          getCharAfterBackSlash(void);
extern bool         OpenFCB( FILE *fp, const char *filename, src_file_type typ );
extern void         CloseFCB( FCB * );
extern void         SrcPurge( void );
extern void         InitIncFile( void );

/* cinfo.c */
extern void         SegInit(void);
extern void         SegFini(void);
extern segment_id   AddSegName(const char *,const char *,int);
extern segment_id   DefThreadSeg( void );
extern void         EmitSegLabels(void);
extern void         FiniSegLabels(void);
extern void         FiniSegBacks(void);
extern void         SetSegs(void);
extern SYM_HANDLE   SegSymHandle(segment_id);
extern void         SetFuncSegment(SYMPTR,segment_id);
extern void         SetFarHuge(SYMPTR,bool);
extern char         *SegClassName(segment_id);
#if _INTEL_CPU
extern hw_reg_set   *SegPeggedReg(segment_id);
#endif
extern void         SetSegment(SYMPTR);
extern void         SetSegAlign(SYMPTR);
extern void         AssignSeg(SYMPTR);
extern segment_id   SymSegId( SYMPTR sym );
extern void         SetSegSymHandle( SYM_HANDLE sym_handle, segment_id segid );
extern void         ImportNearSegIdInit( void );
extern void         ImportSegIdInit( void );

/* cintmain.c */
extern void         ConsErrMsg( cmsg_info  *info );
extern void         ConsErrMsgVerbatim( char const  *line );
extern void         BannerMsg( char const  *line );
extern void         DebugMsg( char const  *line );
extern void         NoteMsg( char const  *line );
extern void         ConBlip( void );
extern void         MyExit( int ret );

/* cmac1.c */
extern void         InitTokenBuf( size_t );
extern void         MacroInit(void);
extern void         MacroAddComp(void);
extern void         MacroFini(void);
extern void         MacroPurge(void);
extern TOKEN        GetMacroToken(void);
extern TOKEN        SpecialMacro( MEPTR );
extern void         DoMacroExpansion( MEPTR );
extern void         InsertReScanPragmaTokens( const char *pragma );
extern void         InsertToken( TOKEN token, const char *str );

/* cmac2.c */
extern TOKEN        ChkControl(void);
extern bool         MacroDel( const char *name );
extern void         CppStackInit( void );
extern void         CppStackFini(void);
extern MEPTR        MacroScan( void );
extern TOKEN        Process_Pragma( void );

/* cmath.c */
extern TREEPTR      AddOp(TREEPTR,TOKEN,TREEPTR);
extern TREEPTR      InitAsgn( TYPEPTR,TREEPTR );
extern TREEPTR      AsgnOp(TREEPTR,TOKEN,TREEPTR);
extern TREEPTR      BinOp(TREEPTR,TOKEN,TREEPTR);
extern bool         IsPtrConvSafe( TREEPTR, TYPEPTR, TYPEPTR );
extern TREEPTR      CnvOp(TREEPTR,TYPEPTR,bool);
extern TREEPTR      FlowOp(TREEPTR,opr_code,TREEPTR);
extern TREEPTR      IntOp(TREEPTR,TOKEN,TREEPTR);
extern TREEPTR      RelOp(TREEPTR,TOKEN,TREEPTR);
extern TREEPTR      ShiftOp(TREEPTR,TOKEN,TREEPTR);
extern TYPEPTR      TernType(TREEPTR,TREEPTR);
extern TYPEPTR      TypeOf(TREEPTR);
extern TREEPTR      UComplement(TREEPTR);
extern TREEPTR      UMinus(TREEPTR);
extern DATA_TYPE    BinExprType(TYPEPTR,TYPEPTR);
extern DATA_TYPE    DataTypeOf(TYPEPTR);
extern bool         IsFuncPtr(TYPEPTR);
extern TREEPTR      FixupAss( TREEPTR opnd, TYPEPTR newtyp );
extern pointer_class ExprTypeClass( TYPEPTR typ );
extern TREEPTR      LCastAdj(  TREEPTR tree );

/* cmodel */
extern char         *BadCmdLine(int,const char *);
extern char         *Define_Macro(const char *);
extern char         *Define_UserMacro(const char *);
extern void         PreDefine_Macro(const char *);
extern char         *AddUndefName(const char *);
extern void         InitModInfo(void);
extern void         MiscMacroDefs(void);

/* cmsg.c */
extern char const   *CGetMsgStr( msg_codes msgnum );
extern void         CGetMsg( char *msgbuf, msg_codes msgnum );
extern void         InitMsg( void );
extern void         FiniMsg( void );
extern char const   *UsageText(void);   // GET INTERNATIONAL USAGE TEXT
//extern msg_type     CGetMsgType( msg_codes msgnum );
extern char const   *CGetMsgPrefix( msg_codes msgnum );
extern int          GetMsgIndex( msg_codes msgnum );

/* cname */
extern int          NameCmp(const void *,const void *,size_t);

/* coptions */
extern bool         EqualChar(int);
extern void         GenCOptions(char **);
extern void         MergeInclude(void);

/* cpragma */
extern void         CPragmaInit( void );
extern void         CPragmaFini( void );
extern void         SetToggleFlag( char const *name, int func, bool push );
extern void         CPragma(void);
extern textsegment  *LkSegName(const char *,const char *);
extern textsegment  *NewTextSeg(const char *,const char *,const char *);
extern void         PragmaInit(void);
extern void         PragmaFini(void);
extern void         PragmaAuxEnding(void);
extern void         PragObjNameInfo(char **);
extern bool         PragRecogId(const char *);
extern bool         PragRecogName(const char *);
extern hw_reg_set   PragRegList(void);
extern int          PragRegIndex(const char *,const char *,size_t,bool);
extern int          PragRegNumIndex( const char *name, size_t len, int max_reg );
extern void         PragRegNameErr( const char *regname, size_t regnamelen );
extern hw_reg_set   *PragManyRegSets(void);
extern TOKEN        PragRegSet(void);
extern void         ChkPragmas(void);
extern void         CreateAux(const char *);
extern void         SetCurrInfo(const char *);
extern void         XferPragInfo(const char*,const char*);
extern void         WarnEnableDisable(bool enabled,msg_codes msgnum);
extern void         AddLibraryName( const char *, const char );
extern void         AddExtRefN( const char * );
extern void         AddExtRefS( SYM_HANDLE );
extern void         SetPackAmount( unsigned amount );
extern bool         GetPragmaAuxAliasInfo( void );
extern aux_info     *PragmaAuxAlias( const char *name );
extern bool         GetPragmaAuxAlias( void );
extern const char   *SkipUnderscorePrefix( const char *str, size_t *len, bool iso_compliant_names );

/* cprag??? */
extern void         AsmStmt(void);
extern void         PragAux(void);
extern hw_reg_set   PragRegName(const char *, size_t);
extern hw_reg_set   PragReg( void );

/* cpurge */
extern void         InitPurge(void);
extern void         PurgeMemory(void);

/* cscan */
extern void         NewLineStartPos( FCB *srcfile );
extern void         ScanInit( void );
extern bool         InitPPScan( void );
extern void         FiniPPScan( bool );
extern id_hash_idx  CalcHash( const char *, size_t );
extern unsigned     hashpjw( const char * );
extern void         SkipAhead( void );
extern TOKEN        ScanToken( void );
extern void         ReScanInit( const char * );
extern TOKEN        ReScanToken( void );
extern const char   *ReScanPos( void );
extern TOKEN        KwLookup( const char *, size_t );
extern TOKEN        NextToken( void );
extern TOKEN        PPNextToken( void );

/* csizeof */
extern target_size  SizeOfArg(TYPEPTR);

/* cstats */
extern void         InitStats(void);
extern void         PrintStats(void);

/* cstmt */
extern void         LookAhead(void);
extern void         Statement(void);
extern void         AddStmt(TREEPTR);
extern void         GenFunctionNode(SYM_HANDLE);
extern LABEL_INDEX  NextLabel(void);
extern void         StmtInit( void );

/* cstring */
extern void         FreeLiteral(STR_HANDLE);
extern STR_HANDLE   GetLiteral(void);
extern void         LoadUnicodeTable(unsigned);
extern void         StringInit(void);
extern TREEPTR      StringLeaf(string_flags);

/* csym */
extern void         SymInit(void);
extern void         SpcSymInit(void);
extern void         SymFini(void);
extern void         SymCreate(SYMPTR,const char *);
extern SYM_HANDLE   SegSymbol(const char *,segment_id);
extern SYM_HANDLE   SpcSymbol(const char *,TYPEPTR,stg_classes);
extern SYM_HANDLE   SymAdd(id_hash_idx,SYMPTR);
extern SYM_HANDLE   SymAddL0(id_hash_idx,SYMPTR);
extern SYM_HANDLE   SymLook(id_hash_idx,const char *);
extern SYM_HANDLE   Sym0Look(id_hash_idx,const char *);
extern SYM_HANDLE   SymLookTypedef(id_hash_idx,const char *,SYMPTR);
extern void         SymGet(SYMPTR,SYM_HANDLE);
extern SYMPTR       SymGetPtr(SYM_HANDLE);
extern void         SymReplace(SYMPTR,SYM_HANDLE);
extern void         EndBlock(void);
extern SYM_HANDLE   MakeFunction(const char *,TYPEPTR);
extern SYM_HANDLE   MakeNewSym(SYMPTR,char,TYPEPTR,stg_classes);
extern LABELPTR     LkLabel(const char *);
extern void         FreeLabels(void);
extern XREFPTR      NewXref(XREFPTR);
extern void         FreeXrefs(XREFPTR);
extern unsigned     SymGetNumSyms( void );
extern unsigned     SymGetNumSpecialSyms( void );
extern SYM_HANDLE   SymGetFirst( void );
extern SYM_HANDLE   SymGetNext( SYM_HANDLE sym_handle );
extern void         SymsPurge( void );
extern void         SetNextSymHandle( unsigned val );

/* ctimepc */
extern void         TimeInit(void);

/* ctype.c */
extern void         CTypeInit(void);
extern void         InitTypeHashTables(void);
extern void         SetSignedChar(void);
extern TYPEPTR      GetType(DATA_TYPE);
extern TYPEPTR      ArrayNode(TYPEPTR);
extern TYPEPTR      FuncNode(TYPEPTR, type_modifiers, TYPEPTR *);
extern TYPEPTR      TypeDefault(void);
extern TYPEPTR      PtrNode(TYPEPTR,type_modifiers,segment_id);
extern TYPEPTR      BPtrNode(TYPEPTR,type_modifiers,segment_id,SYM_HANDLE, BASED_KIND);
extern TYPEPTR      TypeNode(DATA_TYPE,TYPEPTR);
extern type_modifiers TypeQualifier(void);
extern void         TypeSpecifier( decl_info *info );
extern void         FullDeclSpecifier( decl_info *info );
extern TAGPTR       NullTag(void);
extern TAGPTR       TagLookup(void);
extern void         FreeTags(void);
extern target_size  TypeSize(TYPEPTR);
extern target_size  TypeSizeEx( TYPEPTR, bitfield_width *pFieldWidth );
extern TYPEPTR      GetIntTypeBySize( target_size size, bool sign, bool exact );
extern TAGPTR       VfyNewTag( TAGPTR, DATA_TYPE );
extern void         VfyNewSym( id_hash_idx, const char * );
extern align_type   GetTypeAlignment( TYPEPTR );
extern void         TypesPurge( void );
extern void         AddTypeHash( TYPEPTR );
extern void         AddPtrTypeHash( TYPEPTR );
extern void         AdjModsTypeNode( TYPEPTR *ptyp, type_modifiers decl_mod, SYMPTR sym );

/* cusage */
extern void         CUsage( void );

/* cutil */
extern void         CErrSymName( int, SYMPTR,SYM_HANDLE );
extern void         Expecting( const char * );
extern bool         ExpectingToken( TOKEN );
extern void         ExpectingAfter( TOKEN, TOKEN );
extern bool         ExpectingConstant( void );
extern void         ExpectEndOfLine( void );
extern void         ExpectIdentifier( void );
extern void         ExpectString( void );
extern void         ExpectStructUnionTag( void );
extern void         MustRecog( TOKEN );
extern SYM_NAMEPTR  SymName( SYMPTR, SYM_HANDLE );

/* dwarf */
extern void         DwarfBrowseEmit( void );

/* ftoa */
extern char         *ftoa( FLOATVAL * );

/* jis2unic */
extern unsigned     JIS2Unicode( unsigned );

/* pchdr.c */
extern bool         UsePreCompiledHeader( const char * );
extern void         InitBuildPreCompiledHeader( void );
extern void         BuildPreCompiledHeader( const char * );
extern void         FreePreCompiledHeader( void );

/* watcom */
extern void         CBanner( void );

/* cstmt.c */
extern SYM_HANDLE   GetBlockSymList( void );
extern void         InitStmt( void );
extern void         SwitchPurge( void );

/* Macros to skip all typedefs and arrive at the underlying type */
#define SKIP_TYPEDEFS( typeptr )                    \
    while( typeptr->decl_type == TYP_TYPEDEF ) {    \
        typeptr = typeptr->object;                  \
    }
#define SKIP_DUMMY_TYPEDEFS( typeptr )              \
    while( typeptr->decl_type == TYP_TYPEDEF && (typeptr->type_flags & TF2_DUMMY_TYPEDEF) ) { \
        typeptr = typeptr->object;                  \
    }
#define SKIP_ENUM( typeptr )                        \
    if( typeptr->decl_type == TYP_ENUM ) {          \
        typeptr = typeptr->object;                  \
    }
#define SKIP_ARRAYS( typeptr )                      \
    while( typeptr->decl_type == TYP_ARRAY ) {      \
        typeptr = typeptr->object;                  \
    }
