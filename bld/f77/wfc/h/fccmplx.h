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
* Description:  F-Code processor for complex arithmetic.
*
****************************************************************************/


extern cg_name ImagPtr( cg_name dest, cg_type typ );
extern cg_type CmplxBaseType( cg_type typ );
extern void    SplitCmplx( cg_name cmplx_addr, cg_type typ );
extern void    XPopCmplx( cg_cmplx *z, cg_type typ );
extern void    PushComplex( sym_id sym );
extern void    PushCmplxConst( sym_id sym );
extern void    CmplxAssign( sym_id sym, cg_type dst_typ, cg_type src_typ );
extern cg_name CmplxAddr( cg_name real, cg_name imag );
extern void    Cmplx2Scalar( void );
