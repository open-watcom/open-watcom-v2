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
* Description:  FORTRAN compiler global data
*
****************************************************************************/


gbl_defn ftnoption      Options;        // compile options
gbl_defn ftnoption      NewOptions;     // compile options for next statement
gbl_defn unsigned_32    StmtNo;         // current statement number
gbl_defn unsigned_32    NextStmtNo;     // next statement number
gbl_defn int            BrCnt;          // bracket count
gbl_defn itnode         *ITHead;        // head of stmt internal text
gbl_defn itnode         *CITNode;       // current internal text node
gbl_defn itnode         *SPtr1;         // 1st LZ right bracket
gbl_defn itnode         *SPtr2;         // 2nd LZ right bracket
gbl_defn STMT           StmtProc;       // statement processor code
gbl_defn unsigned_16    CtrlFlgs;       // statement control flags
gbl_defn int            SrcRecNum;      // record # of initial line of statement
gbl_defn unsigned_16    BlockNum;       // for controlling unstructured branches
gbl_defn csnode         *CSHead;        // head of control structure list
gbl_defn unsigned_32    IOData;         // bit flags for i/o control lists
gbl_defn itnode         *BkLink;        // ptr to previous itnode in list
gbl_defn unsigned_16    ASType;         // statement flags for expr handling
gbl_defn bool           CpError;        // compile error flag
gbl_defn bool           AError;         // error in expr (downscan, upscan)
gbl_defn sym_id         InitVar;        // S.T. entry being initialized (DATA)
gbl_defn sym_id         SFSymId;        // pointer to symbol table for stmt func
gbl_defn label_id       SFEndLabel;     // end of the current statement function
gbl_defn entry_pt       *ArgList;       // pointer to current entry point list
gbl_defn entry_pt       *Entries;       // pointer to head of entry point lists
gbl_defn act_eq_entry   *EquivSets;     // pointer to equivalent sets
gbl_defn sym_id         SubProgId;      // SymId of subprogram being compiled
gbl_defn impl_entry     ImplicitTab[IMPL_SIZE+1];
gbl_defn byte           IFUsed[24];     // indicates intrinsic functions used
gbl_defn SGMTSW         SgmtSw;         // program segment switches
gbl_defn PSWT           ProgSw;         // program switches
gbl_defn EXTNSW         ExtnSw;         // extension switches
gbl_defn TYPE           ResultType;     // result type of operation in upscan
gbl_defn STMTSW         StmtSw;         // statement switches
gbl_defn rememb         Remember;       // info about the last executable stmt
gbl_defn char           *SrcName;       // name of source file
gbl_defn char           *ObjName;       // name of object output file
gbl_defn source_t       *CurrFile;      // linked list of open FOR77 files
gbl_defn char           *IncludePath;   // path for INCLUDE files

gbl_defn char           TokenBuff[TOKLEN+1];
    // Buffer for collecting tokens. Also used for passing source
    // and listing file names to RunMain.

gbl_defn char           SrcBuff[SRCLEN+1]; // input buffer for source lines
gbl_defn char           *Cursor;        // pointer to current char in SrcBuff
gbl_defn char           *TkCrsr;        // pointer to current char in TokenBuff
gbl_defn lex            Lex;            // information kept by lexical analyser
gbl_defn token_t        LexToken;       // information about token collected
gbl_defn stnumbers      StNumbers;      // struct for statement number info
gbl_defn token_state    State;          // state of token being collected
gbl_defn byte           Line;           // continuation line number
gbl_defn byte           Column;         // current column (as appears to user)
gbl_defn byte           LastColumn;     // last column (default 72)
gbl_defn byte           StmtType;       // type of statement line
gbl_defn byte           ContType;       // type of continuation char

gbl_defn char           *LibMember;     // name of library member
gbl_defn unsigned_16    ISNNumber;      // ISN # only for FORTRAN statements;
gbl_defn char           *CmdPtr;        // pointer to command line
gbl_defn uint           NumExtens;      // number of extens. during compilation
gbl_defn uint           NumWarns;       // number of warnings during compilation
gbl_defn char           *SrcExtn;       // pointer to source file extension
gbl_defn int            RetCode;        // return code for Main()


gbl_defn sym_id         NList;          // start of name list
gbl_defn sym_id         CList;          // start of constant list
gbl_defn sym_id         SList;          // start of statement number list
gbl_defn sym_id         LList;          // start of literal list
gbl_defn sym_id         GList;          // start of global list
gbl_defn sym_id         MList;          // start of magic symbol list
gbl_defn sym_id         BList;          // start of common block symbol list
gbl_defn sym_id         RList;          // start of structure definition list
gbl_defn sym_id         NmList;         // start of NAMELIST list
gbl_defn sym_id         IFList;         // start of intrinsic function list
gbl_defn sym_id         CurrStruct;     // sym_id of structure being defined
gbl_defn itnode         *FieldNode;     // I.T. node of current field

gbl_defn pointer        *ITPool;        // head of free list of I.T. nodes
gbl_defn unsigned_32    CGOpts;         // code generator options
gbl_defn unsigned_32    CPUOpts;        // code generator options (CPU target)
gbl_defn unsigned_32    OZOpts;         // code generator options (optimizations)
gbl_defn char           SymBuff[MAX_SYMLEN+1]; // buffer for symbol names
gbl_defn bool           StmtNoFound;    // statement number found?

gbl_defn int            UnFreeMem;      //  & of unfreed bits of memory
gbl_defn uint           NumErrors;      //  number of errors during compilation
                                        //  and execution
gbl_defn uint           LinesPerPage;   //  number of lines per page in listing
