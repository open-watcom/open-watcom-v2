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
* Description:  Prototypes for dfsyms.c
*
****************************************************************************/


extern  void    DFInitDbgInfo( void );
extern  void    DFFiniDbgInfo( void );
extern  void    DFObjInitDbgInfo( void );
extern  void    DFObjFiniDbgInfo( offset );
extern  void    DFObjLineInitDbgInfo( void );
extern  void    DFObjLineFiniDbgInfo( void );
extern  void    DFGenStatic( cg_sym_handle sym, dbg_loc loc );
extern  void    DFTypedef( const char *nm, dbg_type tipe );
extern  void    DFProEnd( dbg_rtn *rtn, offset lc );
extern  void    DFBlkBeg( dbg_block *blk, offset lc );
extern  void    DFBlkEnd( dbg_block *blk, offset lc );
extern  void    DFEpiBeg( dbg_rtn *rtn, offset lc );
extern  void    DFRtnEnd( dbg_rtn *rtn, offset lc );
extern  void    DFBegCCU( segment_id code, dw_sym_handle dbg_pch );
extern  void    DFDefSegs( void );
extern  void    DFLineNum( cue_state *, offset );
extern  void    DFSegRange( void );
extern  void    DFSymRange( cg_sym_handle, offset );
