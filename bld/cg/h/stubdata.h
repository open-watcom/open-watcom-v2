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


typedef struct {
        int             hdl;
        char            *name;
        int             exists;
} FILE_CTL;

extern  FILE_CTL        Files[MAX_SEG+1];
extern  int             FilesOpen;

extern  bool            SegOk[MAX_SEG+1];
extern  unsigned_32     Locs[MAX_SEG+1];
extern  int             SymDebug;
extern  int             TypDebug;
extern  int             Actions;
extern  segment_id      CurSeg;
extern  int             Out;
extern  char            UBuff[UBUFF_LEN+1];
extern  a               *AutoList;
extern  n               *NodeList;
extern  l               *LblList;
extern  b               *BackList;
extern  t               *TempList;
extern  s               *StaticList;
extern  ic              *Inlines;
extern  int             LabelId;
extern  int             NodeId;
extern  int             BackId;
extern  int             SelId;
extern  segdef          *SegDefs;
extern  cg_state        CGState;
extern  segment_id      CodeSeg;
extern  sym_handle      InProc;
extern  cg_type         CGIntTypes[];
extern  cg_type         DGIntTypes[];
extern  cg_type         FloatTypes[];
extern  cg_op           ControlOps[];
extern  cg_op           CompareOps[];
extern  cg_op           FlowOps[];
extern  cg_op           UnaryOps[];
extern  cg_op           FunkyOps[];
extern  cg_op           BinaryOps[];
extern  char            * Classes[];
extern  char            * Ops[];
extern  char            * Tipes[];
