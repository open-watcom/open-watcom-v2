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
* Description:  Global code generator data (defined in data.c).
*
****************************************************************************/


#include "typedef.h"

#include "model.h"
#include "addrname.h"
#include "regset.h"
#include "rttable.h"
#include "conflict.h"
#include "procdef.h"
#include "optmac.h"

extern  block                   *HeadBlock;
extern  block                   *BlockList;
extern  block                   *CurrBlock;
extern  int                     InsId;
extern  int                     TempId;
extern  conflict_node           *ConfList;
extern  rt_class                RoutineNum;
extern  name                    *Names[N_INDEXED+1];
extern  name                    *LastTemp;
extern  name                    *DummyIndex;
extern  source_line_number      SrcLine;
extern  global_bit_set          MemoryBits;
extern  type_class_def          ClassPointer;
extern  bool                    BlockByBlock;
extern  type_length             MaxStack;
extern  type_def                *TypeBoolean;
extern  type_def                *TypeInteger;
extern  type_def                *TypeUnsigned;
extern  type_def                *TypePtr;
extern  type_def                *TypeNone;
extern  type_def                *TypeProcParm;
extern  type_def                *TypeNearInteger;
extern  type_def                *TypeLongInteger;
extern  type_def                *TypeLongLongInteger;
extern  type_def                *TypeHugeInteger;
extern  hw_reg_set              GivenRegisters;
extern  bool                    BlocksUnTrimmed;
extern  an                      AddrList;
extern  segment_id              DbgLocals;
extern  segment_id              DbgTypes;
extern  unsigned_16             TypeIdx;      // uint ???
extern  byte                    OptForSize;
extern  bool                    HaveLiveInfo;
extern  bool                    HaveDominatorInfo;
extern  pointer_int             FrlSize;
extern  bool                    HaveCurrBlock;
extern  proc_def                *CurrProc;
extern  type_length             TypeClassSize[];
