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


/* docode.c */
extern instruction      CurrIns;

/* fpptab.c */
extern ins_name const    FppTab1[];
extern ins_name const    FppTab2[];
extern ins_name const    FppTab3[];
extern ins_name const    FppTab4[];
extern ins_name const    FppTab5[];
extern ins_name const    FppTab6[];
extern ins_name const    FppTab7[];

/* insname.c */
extern char const             * const InsName[];

/* instab.c */
extern op_desc const          InsTab[];
extern op_desc const          S4xInsTab[];
extern op_desc const          S6xInsTab[];
extern op_desc const          S7xInsTab[];
extern op_desc const          SDxInsTab[];
extern op_desc const          SExInsTab[];
extern op_desc const          SFxInsTab[];

/* o2adata.c */
extern FILE             *ObjFile;
extern FILE             *Output;
extern FILE             *Source;
extern int              ErrCount;
extern int_16           LastNum;        /* last line num record dumped */
extern int              Repeats;
extern char             *SrcName;
extern char             *CommentName;
extern char             *CodeName;
extern struct line_num  *SrcLines;
extern struct line_num  *SrcLinesRover;
extern form_option      Options;
extern char             DataBytes;
extern char             DataLen;
extern char             Pass;
extern bool             ExpDump;
extern bool             ImpDump;
extern bool             RetFarUsed;
extern char             DataString[ MAX_INSTR_LEN ];
extern char             PrevString[ MAX_INSTR_LEN ];
extern bool             IsPharLap;
extern bool             Is32Record;
extern bool             Is32BitObj;
extern char             IntLblStart;
extern bool             UnMangle;

/* objdasm.c */
extern struct segment   *Segment;
extern struct group     *Group;
extern struct module    *Mod;

/* pcoio.c */
extern uint_8           RecType;
extern bool             EndOfRecord;
extern unsigned         RecNumber;
extern uint_32          Offset;
extern unsigned         RecLen;

/* pcoutil.c */
extern void             **NameTab;
extern uint_16          NameIndex;
extern void             **SegTab;
extern uint_16          SegIndex;
extern void             **GrpTab;
extern uint_16          GrpIndex;
extern void             **ExtTab;
extern uint_16          ExtIndex;
extern void             **ThrdTab;

/* regname.c */
extern char const       * const ModifierTab[];
extern char const       * const X86RegisterName[];
extern ins_name const        ImmedTable[];
extern ins_name const        ShiftTable[];
extern ins_name const        Group1Table[];
extern ins_name const        Group2Table[];
extern ins_name const        Group3Table[];
extern ins_name const        S00Ops[];
extern ins_name const        S01Ops[];
extern ins_name const        S2xOpType[];
extern ins_name const        S8xOps[];
extern ins_name const        S9xOps[];
extern ins_name const        SAxOps[];
extern ins_name const        SAxOpType[];
extern ins_name const        SBAOps[];
extern ins_name const        SBxOps[];
extern ins_name const        SBxOpType[];
extern ins_name const        SCxOps[];
extern ins_name const        SCxOpType[];
extern prefix   const        PrefixTab[];
extern prefix   const        SegPrefixTab[];

/* rwdata.c */
extern uint_32          InsAddr;
extern char             *TxtBuff;
extern char             *NameBuff;
extern uint_8           Opcode;
extern uint_8           SecondByte;

/* wcode.c */
extern uint_32          WtkAddr;
extern char             WTLBASEStr[];
extern char             WTLSEGStr[];
extern bool             WtlsegPresent;

/* objorl.c */
extern bool             UseORL;
extern char             *ModNameORL;
