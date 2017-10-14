/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Prototypes for cvsyms.c
*
****************************************************************************/


extern  void    CVInitDbgInfo( void );
extern  void    CVFiniDbgInfo( void );
extern  void    CVObjInitDbgInfo( void );
extern  void    CVObjFiniDbgInfo( void );
extern  void    CVGenStatic( cg_sym_handle sym, dbg_loc loc, bool mem );
extern  void    CVTypedef( const char *nm, dbg_type tipe );
extern  void    CVSetBase( void );
extern  void    CVRtnBeg( dbg_rtn *rtn, offset lc );
extern  void    CVProEnd( dbg_rtn *rtn, offset lc );
extern  void    CVBlkBeg( dbg_block *blk, offset lc );
extern  void    CVBlkEnd( dbg_block *blk, offset lc );
extern  void    CVEpiBeg( dbg_rtn *rtn, offset lc );
extern  void    CVRtnEnd( dbg_rtn *rtn, offset lc );
extern  void    CVDefSegs( void );
extern  void    CVOutSymICon( cv_out *out, const char *nm, int val, dbg_type tipe );
extern  void    CVOutSym( cv_out *out, cg_sym_handle sym );
extern  void    CVOutBck( cv_out *out, back_handle bck, offset add,  dbg_type tipe );
extern  void    CVOutLocal( cv_out *out, name *t, int disp,  dbg_type tipe );
extern  void    CVSymIConst( const char *nm, int val, dbg_type tipe );
extern  void    CVSymIConst64( const char *nm, signed_64 val, dbg_type tipe );
