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


#ifndef __PTRCNV_H__
#define __PTRCNV_H__

// PTRCNV.H -- information related to pointer conversion
//
//

typedef struct                  // PTRCNV -- information related to conversion
{
    TYPE pted_src;              // - source: type of object pointed at
    TYPE pted_tgt;              // - target: type of object pointed at
    type_flag flags_src;        // - source: flags
    type_flag flags_tgt;        // - target: flags
    unsigned converts        :1;// - TRUE ==> implicit conversion ok
    unsigned to_base         :1;// - TRUE ==> conversion to base ptr
    unsigned to_derived      :1;// - TRUE ==> conversion to derived ptr
    unsigned to_void         :1;// - TRUE ==> conversion to void* ptr
    unsigned cv_err_0        :1;// - TRUE ==> CV mismatch on level 0
    unsigned ptr_integral_ext:1;// - TRUE ==> ptr->integer extension
    unsigned reint_cast_ok   :1;// - TRUE ==> reinterpret_cast ok
    unsigned :0;                // alignment only
} PTRCNV;

// PROTOTYPES

TYPE NodeConvertBaseToDerived(  // CONVERT BASE PTR TO DERIVED PTR
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE tgt,                   // - derived type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope )          // - base scope
;
TYPE NodeConvertDerivedToBase(  // CONVERT DERIVED PTR TO NONVIRTUAL BASE PTR
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE tgt,                   // - derived type
    SCOPE derived_scope,        // - derived scope
    SCOPE base_scope )          // - base scope
;
bool PtrCnvInfo(                // FILL IN PTR-CONVERSION INFORMATION
    TYPE ptr_src,               // - source type
    TYPE ptr_tgt,               // - target type
    PTRCNV* info )              // - pointer-conversion information
;

#endif
