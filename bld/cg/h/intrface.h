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


struct  cg_interface {
void            (*_CGProcDecl)(cg_sym_handle,cg_type);
void            (*_CGParmDecl)(cg_sym_handle,cg_type);
label_handle    (*_CGLastParm)(void);
void            (*_CGAutoDecl)(cg_sym_handle,cg_type);
cg_name         (*_CGInteger)(signed_32,cg_type);
cg_name         (*_CGFloat)(char*,cg_type);
cg_name         (*_CGFEName)(cg_sym_handle,cg_type);
cg_name         (*_CGBackName)(back_handle,cg_type);
cg_name         (*_CGAssign)(cg_name,cg_name,cg_type);
cg_name         (*_CGLVAssign)(cg_name,cg_name,cg_type);
cg_name         (*_CGPostGets)(cg_op,cg_name,cg_name,cg_type);
cg_name         (*_CGPreGets)(cg_op,cg_name,cg_name,cg_type);
cg_name         (*_CGLVPreGets)(cg_op,cg_name,cg_name,cg_type);
cg_name         (*_CGBinary)(cg_op,cg_name,cg_name,cg_type);
cg_name         (*_CGUnary)(cg_op,cg_name,cg_type);
cg_name         (*_CGIndex)(cg_name,cg_name,cg_type,cg_type);
call_handle     (*_CGInitCall)(cg_name,cg_type,cg_sym_handle);
void            (*_CGAddParm)(call_handle,cg_name,cg_type);
cg_name         (*_CGCall)(call_handle);
cg_name         (*_CGCompare)(cg_op,cg_name,cg_name,cg_type);
cg_name         (*_CGFlow)(cg_op,cg_name,cg_name);
cg_name         (*_CGChoose)(cg_name,cg_name,cg_name,cg_type);
cg_name         (*_CGWarp)(cg_name,label_handle,cg_name);
void            (*_CGControl)(cg_op,cg_name,label_handle);
void            (*_CG3WayControl)(cg_name,label_handle,label_handle,label_handle);
void            (*_CGBigLabel)(back_handle);
void            (*_CGBigGoto)(label_handle,int);
sel_handle      (*_CGSelInit)(void);
void            (*_CGSelCase)(sel_handle,label_handle,signed_32);
void            (*_CGSelRange)(sel_handle,signed_32,signed_32,label_handle);
void            (*_CGSelOther)(sel_handle,label_handle);
void            (*_CGSelect)(sel_handle,cg_name);
void            (*_CGReturn)(cg_name,cg_type);
cg_name         (*_CGEval)(cg_name);
void            (*_CGTrash)(cg_name);
void            (*_CGDone)(cg_name);
cg_type         (*_CGType)(cg_name);
cg_name         (*_CGBitMask)(cg_name,byte,byte,cg_type);
cg_name         (*_CGVolatile)( cg_name );
cg_name         (*_CGAttr)( cg_name, cg_sym_attr );
cg_name         (*_CGAlign)( cg_name, uint );
cg_name         *(*_CGDuplicate)(cg_name);
temp_handle     (*_CGTemp)(cg_type);
cg_name         (*_CGTempName)(temp_handle,cg_type);
cg_name         (*_CGCallback)( cg_callback, callback_handle );
cg_name         (*_CGPatchNode)( patch_handle,cg_type );
patch_handle    (*_BEPatch)(void);
void            (*_BEPatchInteger)( patch_handle, signed_32 );
void            (*_BEFiniPatch)( patch_handle );
label_handle    (*_BENewLabel)(void);
void            (*_BEFiniLabel)(label_handle);
void            (*_DBSrcCue)( uint , uint, uint );
void            (*_DBBegBlock)(void);
void            (*_DBEndBlock)(void);
};
extern  struct cg_interface *CGFuncTable;
