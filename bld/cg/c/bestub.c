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


#pragma off(unreferenced);
#include "cgstd.h"
#include "cgswitch.h"
#include "cfloat.h"
#include "dbbit.h"
#include "coderep.h"
#include "cgdefs.h"
#include "symdbg.h"

typedef union hdl_type {
        bck_info        *b;
        label_handle    l;
        pointer         p;
} hdl_type;

extern  cg_init_info    BEInit(cg_switches switches, cg_target_switches tg_switches,
                               uint size,proc_revision proc) {
    cg_init_info        info;
    info.version.is_large = TRUE;
    info.version.revision = II_REVISION;
#if _TARGET & _TARG_IAPX86
    info.version.target = II_TARG_8086;
#else
    info.version.target = II_TARG_80386;
#endif
    return( info );
}
extern void BEStart() {}
extern void BEStop() {}
extern void BEAbort() {}
extern void BEFini() {}
extern bool BEMoreMem(void) {return(0);}
extern segment_id BESetSeg(segment_id _1) {return(0);}
extern void BEDefSeg(segment_id _1,seg_attr _2,char * _3,uint _4) {}
extern void BEFlushSeg(segment_id _1 ) {}
extern label_handle BENewLabel(){return(0);}
extern void BEFiniLabel(label_handle _4) {}
extern bck_info *BENewBack(sym_handle _4){return(0);}
extern void BEFiniBack(bck_info *_4) {}
extern void BEFreeBack(bck_info *_4) {}
extern void BEDefType(cg_type _1,uint _2,unsigned_32 _4) {}
extern void BEAliasType(cg_type _1,cg_type _4) {}
extern unsigned_32 BETypeLength(cg_type _4){return(0);}
extern uint BETypeAlign(cg_type _4){return(0);}
extern void CGProcDecl(pointer _1,cg_type _4) {}
extern void CGParmDecl(pointer _1,cg_type _4) {}
extern label_handle CGLastParm(){return(0);}
extern void CGAutoDecl(pointer _1,cg_type _4) {}
extern cg_name CGInteger(signed_32 _1,cg_type _4){return(0);}
extern cg_name CGFloat(char *_1,cg_type _4){return(0);}
extern cg_name CGFEName(sym_handle _1,cg_type _4){return(0);}
extern cg_name CGBackName(bck_info *_1,cg_type _4){return(0);}
extern cg_name CGTempName(temp_handle _1,cg_type _4){return(0);}
extern temp_handle CGTemp(cg_type _4){return(0);}
extern cg_name CGAssign(cg_name _1,cg_name _2,cg_type _4){return(0);}
extern cg_name CGLVAssign(cg_name _1,cg_name _2,cg_type _4){return(0);}
extern cg_name CGPostGets(cg_op _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGPreGets(cg_op _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGLVPreGets(cg_op _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGBinary(cg_op _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGUnary(cg_op _1,cg_name _2,cg_type _4){return(0);}
extern cg_name CGIndex(cg_name _1,cg_name _2,cg_type _3,cg_type _4){return(0);}
extern call_handle CGInitCall(cg_name _1,cg_type _2,sym_handle _4){return(0);}
extern void CGAddParm(call_handle _1,cg_name _2,cg_type _4) {}
extern cg_name CGCall(call_handle _4){return(0);}
extern cg_name CGCompare(cg_op _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGFlow(cg_op _1,cg_name _2,cg_name _4){return(0);}
extern cg_name CGChoose(cg_name _1,cg_name _2,cg_name _3,cg_type _4){return(0);}
extern cg_name CGWarp(cg_name _1,label_handle _2,cg_name _4){return(0);}
extern void CG3WayControl(cg_name _1,label_handle _2,label_handle _3,label_handle _4) {}
extern void CGControl(cg_op _1,cg_name _2,label_handle _4) {}
extern void CGBigLabel(back_handle _4) {}
extern void CGBigGoto(label_handle _1,int _4) {}
extern sel_handle CGSelInit(){return(0);}
extern void CGSelCase(sel_handle _1,label_handle _2,signed_32 _4) {}
extern void CGSelRange(sel_handle _1,signed_32 _2,signed_32 _3,label_handle _4) {}
extern void CGSelOther(sel_handle _1,label_handle _4) {}
extern void CGSelectRestricted(sel_handle _1,cg_name _4,cg_switch_type _2 );
extern void CGSelect(sel_handle _1,cg_name _4) {}
extern void CGReturn(cg_name _1,cg_type _4) {}
extern cg_name CGEval(cg_name _4){return(0);}
extern void CGTrash(cg_name _4) {}
extern void CGDone(cg_name _4) {}
extern cg_type CGType(cg_name _4){return(0);}
extern cg_name CGBitMask(cg_name _1,byte _2,byte _3,cg_type _4){return(0);}
extern cg_name CGVolatile(cg_name _4){return(0);}
extern cg_name CGAttr(cg_name _4, cg_sym_attr _1 ){return(0);}
static cg_name TwoZeroes[] = { 0,0 };
extern cg_name *CGDuplicate(cg_name _1) { return( TwoZeroes ); }
extern void DGLabel(bck_info *_4) {}
extern void DGBackPtr(bck_info * _1,segment_id _2,signed_32 _3,cg_type _4) {}
extern void DGFEPtr(sym_handle _1,cg_type _2,signed_32 _4) {}
extern void DGInteger(unsigned_32 _1,cg_type _4) {}
extern void DGFloat(char *_1,cg_type _4) {}
extern void DGChar(char _4) {}
extern void DGString(char *_1,uint _4) {}
extern void DGBytes(unsigned_32 _1,byte *_4) {}
extern void DGIBytes(unsigned_32 _1,byte _4) {}
extern void DGUBytes(unsigned_32 _4) {}
extern void DGAlign(uint _4) {}
extern unsigned_32 DGSeek(unsigned_32 _4){return(0);}
extern unsigned_32 DGTell(){return(0);}
extern unsigned_32 DGBackTell(bck_info *_4){return(0);}
extern void DGCFloat(pointer _1,cg_type _4) {}

extern void DBLineNum(uint _4){}
extern void DBModSym(sym_handle _1,cg_type _4){}
extern void DBLocalSym(sym_handle _1,cg_type _4){}
extern void DBBegBlock(void){}
extern void DBEndBlock(void){}
extern dbg_type DBFtnType(char *_5,dbg_ftn_type _4){return(0);}
extern dbg_type DBScalar(char *_5,cg_type _4){return(0);}
extern dbg_type DBScope(char *_6){return(0);}
extern name_entry *DBBegName(const char *_5,dbg_type _4){return(0);}
extern dbg_type DBForward(name_entry *_6){return(0);}
extern dbg_type DBEndName(name_entry *_5,dbg_type _4){return(0);}
extern void DumpName(name_entry *_5,dbg_type _4){}
extern dbg_type DBCharBlock(unsigned_32 _4){return(0);}
extern dbg_type DBIndCharBlock(back_handle _1,cg_type _2, int _4){return(0);}
extern dbg_type DBFtnArray(back_handle _1,cg_type _2, cg_type _3, int _4,dbg_type _5){return(0);}
extern dbg_type DBArray(dbg_type _1,dbg_type _4){return(0);}
extern dbg_type DBIntArray(unsigned_32 _1,dbg_type _4){return(0);}
extern dbg_type DBSubRange(signed_32 _1,signed_32 _2,dbg_type _4){return(0);}
extern dbg_type DBDereference(cg_type _1,dbg_type _4){return(0);}
extern dbg_type DBPtr(cg_type _1,dbg_type _4){return(0);}
extern struct_list *DBBegStruct(cg_type _1,bool _2){return(0);}
extern void DBAddField(struct_list *_5,unsigned_32 _1,char *_2,dbg_type _4){}
extern void DBAddBitField(struct_list *_5,unsigned_32 _1,byte _2,byte _3,char *_6,dbg_type _4){}
extern dbg_type DBEndStruct(struct_list *_6){return(0);}
extern enum_list *DBBegEnum(cg_type _4){return(0);}
extern void DBAddConst(enum_list *_5,const char *_6,signed_32 _4){}
extern dbg_type DBEndEnum(enum_list *_6){return(0);}
extern proc_list *DBBegProc(cg_type _1,dbg_type _4){return(0);}
extern void DBAddParm(proc_list *_5,dbg_type _4){}
extern dbg_type DBEndProc(proc_list *_6){return(0);}
extern void DBGenSym(cg_sym_handle _1,dbg_loc _2,int _3){}
extern dbg_type DBBasedPtr(cg_type _1,dbg_type _2,dbg_loc _3){return 0;}
extern void DBAddLocField(dbg_struct _1,dbg_loc _2,uint _3,byte _4,byte _5,char * _6,dbg_type _7){}
extern void DBAddInheritance(dbg_struct _1,dbg_type _2,dbg_loc _3){}
extern dbg_loc DBLocInit(void){return 0;}
extern dbg_loc DBLocSym(dbg_loc _1,cg_sym_handle _2){return 0;}
extern dbg_loc DBLocConst(dbg_loc _1,unsigned_32 _2){return 0;}
extern dbg_loc DBLopOp(dbg_loc _1,dbg_loc_op _2,unsigned _3){return 0;}
extern void DBLocFini(dbg_loc _1){}
