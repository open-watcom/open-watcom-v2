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


#include "symtypes.h"
#include "itdefn.h"
#include "fcodes.h"

// emit.c
extern  void            EmitOp(FCODE);
// gtypes.c
extern  void            GenType(itnode *);
extern  void            GenTypes(itnode *,itnode *);
extern  void            DumpType(TYPE,uint);
extern  void            DumpTypes(TYPE,uint,TYPE,uint);
// setopn.c
extern  void            SetOpn(itnode *,USOPN);
// stkops.c
extern  void            PushOpn(itnode *);
extern  void            PushSym(sym_id sym);
extern  void            SymRef(itnode *itptr);
extern  void            PushConst(intstar4 val);
extern  void            GParenExpr(void);
// objutil.c
extern  void            InitObj(void);
extern  void            FiniObj(void);
extern  obj_ptr         ObjTell(void);
extern  obj_ptr         ObjSeek(obj_ptr new_obj);
extern  unsigned_16     ObjOffset(obj_ptr prev_obj);
extern  void            AlignEven(void);
extern  void            OutPtr(pointer val);
extern  void            OutU16(unsigned_16 val);
extern  void            OutInt(inttarg val);
extern  void            OutConst32(signed_32 val);
extern  void            OutObjPtr(obj_ptr val);
extern  void            OutByte(byte val);
extern  void            InitFCode(void);
extern  void            *GetPtr(void);
extern  unsigned_16     GetU16(void);
extern  signed_32       GetConst32(void);
extern  inttarg         GetInt(void);
extern  obj_ptr         GetObjPtr(void);
extern  byte            GetByte(void);
extern  obj_ptr         FCodeSeek(obj_ptr new_obj);
extern  obj_ptr         FCodeTell(int offset);
extern  void            FCSeek(void);
extern  FCODE           GetFCode(void);
