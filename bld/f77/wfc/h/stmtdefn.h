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
* Description:  FORTRAN statements constant tables data
*
****************************************************************************/


// Note : If you wish to add a new statement processor there are five files
//        that need changing: PRDEFN.H, KWLIST.C, PROCTBL.C, CFTABLE.C

//      id              keyword                          processor         flags
pick( PR_NULL,         "",                              &BadStmt,        0                          )
pick( PR_ADMIT,        "ADMIT",                         &CpAdmit,        BD|NS|BB                   )
pick( PR_ALLOCATE,     "ALLOCATE",                      &CpAllocate,     SL|DB                      )
pick( PR_ASSIGN,       "ASSIGN",                        &CpAssign,       SL|DB                      )
pick( PR_ATEND,        "ATEND",                         &CpAtEnd,        NS|BB|SL|DB                )
pick( PR_BKSP,         "BACKSPACE",                     &CpBackSp,       SL|DB                      )
pick( PR_BLKDAT,       "BLOCKDATA",                     &CpBlockData,    BD|NS|NE|BB|SP             )
pick( PR_CALL,         "CALL",                          &CpCall,         SL|DB                      )
pick( PR_CASE,         "CASE",                          &CpCase,         BD|NS|BB                   )
pick( PR_CHAR,         "CHARACTER",                     &CpCharVar,      BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_CLOSE,        "CLOSE",                         &CpClose,        SL|DB                      )
pick( PR_COMN,         "COMMON",                        &CpCommon,       BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_COMP,         "COMPLEX",                       &CpCmplxVar,     BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_CONT,         "CONTINUE",                      &CpContinue,     DB                         )
pick( PR_CYCLE,        "CYCLE",                         &CpCycle,        SL|DB                      )
pick( PR_DATA,         "DATA",                          &CpData,         BD|NS|NE|BB|BL|SS          )
pick( PR_DEALLOCATE,   "DEALLOCATE",                    &CpDeAllocate,   SL|DB                      )
pick( PR_DIM,          "DIMENSION",                     &CpDimension,    BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_DO_WHILE,     "DO",                            &CpDoWhile,      BD|NS                      )
pick( PR_DBLE_COMPLEX, "DOUBLECOMPLEX",                 &CpDCmplxVar,    BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_DP,           "DOUBLEPRECISION",               &CpDbleVar,      BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_ELSE,         "ELSE",                          &CpElse,         BD|NS|BB                   )
pick( PR_ELSEIF,       "ELSEIF",                        &CpElseIf,       BD|NS|BB                   )
pick( PR_END,          "END",                           &CpEnd,          BD|NS|BL|DB|SL             )
pick( PR_EATEND,       "ENDATEND",                      &CpEndAtEnd,     BD|NS|BB|IL                )
pick( PR_EBLOCK,       "ENDBLOCK",                      &CpEndBlock,     BD|NS|BB|DB                )
pick( PR_ENDDO,        "ENDDO",                         &CpEndDo,        NS|BB|DB                   )
pick( PR_EFILE,        "ENDFILE",                       &CpEndfile,      SL|DB                      )
pick( PR_EGUESS,       "ENDGUESS",                      &CpEndGuess,     BD|NS|IL                   )
pick( PR_ENDIF,        "ENDIF",                         &CpEndif,        BD|NS|IL                   )
pick( PR_ELOOP,        "ENDLOOP",                       &CpEndLoop,      BD|NS|BB|SL|DB             )
pick( PR_ENDMAP,       "ENDMAP",                        &CpEndMap,       BD|NS|NE|BB|SS|BL          )
pick( PR_ENDSTRUCTURE, "ENDSELECT",                     &CpEndSelect,    BD|NS|BB|IL                )
pick( PR_ENDUNION,     "ENDSTRUCTURE",                  &CpEndStructure, BD|NS|NE|BB|SS|BL|UN|MA    )
pick( PR_ESELECT,      "ENDUNION",                      &CpEndUnion,     BD|NS|NE|BB|SS|BL|MA       )
pick( PR_EWHILE,       "ENDWHILE",                      &CpEndWhile,     BD|NS|BB|DB                )
pick( PR_ENTRY,        "ENTRY",                         &CpEntry,        BD|NS|BB|SS|ST|UN|MA       )
pick( PR_EQUIV,        "EQUIVALENCE",                   &CpEquivalence,  BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_EXEC,         "EXECUTE",                       &CpExecute,      SL|DB                      )
pick( PR_EXIT,         "EXIT",                          &CpExit,         SL|DB                      )
pick( PR_EXT_COMPLEX,  "EXTENDEDCOMPLEX",               &CpXCmplxVar,    BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_EXTENDED,     "EXTENDEDPRECISION",             &CpXDbleVar,     BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_EXT,          "EXTERNAL",                      &CpExternal,     BD|NS|NE|BB|SS|ST|UN|MA    )
pick( PR_FMT,          "FORMAT",                        &CpFormat,       BD|NS|NE|BB                )
pick( PR_FUNC,         "FUNCTION",                      &CpFunction,     BD|NS|NE|BB|SP             )
pick( PR_GOTO,         "GOTO",                          &CpGoto,         BD|SL|DB                   )
pick( PR_GUESS,        "GUESS",                         &CpGuess,        BD|NS|DB                   )
pick( PR_IF,           "IF",                            &CpLogIf,        NS|SL|DB                   )
pick( PR_IMP,          "IMPLICIT",                      &CpImplicit,     BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_INCLUDE,      "INCLUDE",                       &CpInclude,      BD|NS|NE|BB|BL             )
pick( PR_INQ,          "INQUIRE",                       &CpInquire,      SL|DB                      )
pick( PR_INT,          "INTEGER",                       &CpIntVar,       BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_INTRNS,       "INTRINSIC",                     &CpIntrinsic,    BD|NS|NE|BB|SS|ST|UN|MA    )
pick( PR_LOG,          "LOGICAL",                       &CpLogVar,       BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_LOOP,         "LOOP",                          &CpLoop,         BD|NS                      )
pick( PR_MAP,          "MAP",                           &CpMap,          BD|NS|NE|BB|SS|BL|ST|MA    )
pick( PR_NAMELIST,     "NAMELIST",                      &CpNameList,     NS|NE|BB|SS|BL|ST|UN|MA    )
pick( PR_OPEN,         "OPEN",                          &CpOpen,         SL|DB                      )
pick( PR_OTHERWISE,    "OTHERWISE",                     &CpOtherwise,    BD|NS|BB                   )
pick( PR_PARM,         "PARAMETER",                     &CpParameter,    BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_PAUSE,        "PAUSE",                         &CpPause,        SL|DB                      )
pick( PR_PRINT,        "PRINT",                         &CpPrint,        SL|DB                      )
pick( PR_PROG,         "PROGRAM",                       &CpProgram,      BD|NS|NE|BB|SP             )
pick( PR_QUIT,         "QUIT",                          &CpQuit,         SL|DB                      )
pick( PR_READ,         "READ",                          &CpRead,         SL|DB                      )
pick( PR_REAL,         "REAL",                          &CpRealVar,      BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_RECORD,       "RECORD",                        &CpRecord,       BD|NS|NE|BB|SS|SP|BL|UN    )
pick( PR_REMBLK,       "REMOTEBLOCK",                   &CpRemBlock,     BD|NS|NE|BB                )
pick( PR_RET,          "RETURN",                        &CpReturn,       BD|DB|SL                   )
pick( PR_REW,          "REWIND",                        &CpRewind,       SL|DB                      )
pick( PR_SAVE,         "SAVE",                          &CpSave,         BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_SELECT,       "SELECT",                        &CpSelect,       BD|NS|SL|DB                )
pick( PR_STOP,         "STOP",                          &CpStop,         BD|SL|DB                   )
pick( PR_STRUCTURE,    "STRUCTURE",                     &CpStructure,    BD|NS|NE|BB|SS|BL|ST       )
pick( PR_SUB,          "SUBROUTINE",                    &CpSubroutine,   BD|NS|NE|BB|SP             )
pick( PR_UNION,        "UNION",                         &CpUnion,        BD|NS|NE|BB|SS|BL|MA       )
pick( PR_UNTIL,        "UNTIL",                         &CpUntil,        BD|NS|BB|SL|DB             )
pick( PR_VOLATILE,     "VOLATILE",                      &CpVolatile,     BD|NS|NE|BB|SS|BL|ST|UN|MA )
pick( PR_WHILE,        "WHILE",                         &CpWhile,        BD|NS                      )
pick( PR_WRITE,        "WRITE",                         &CpWrite,        SL|DB                      )

// end of keywords that can be looked up in table

pick( PR_DO,           "DO",                            &CpDo,           BD|NS|SL|DB                )

// the final three keyword entries are for error messages

pick( PR_ASNMNT,       "assignment",                    &CpAsgnmt,       SL|DB                      )
pick( PR_ARIF,         "arithmetic if",                 &CpArithIf,      BD|SL|DB                   )
pick( PR_STMTFUNC,     "statement function definition", &CpStmtFunc,     NS|BB|NE|BD                )
