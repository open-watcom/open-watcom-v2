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
* Description:  Block creation and destruction functions.
*
****************************************************************************/


extern  block           *MakeBlock( label_handle label, block_num edges );
extern  block           *NewBlock( label_handle label, bool label_dies );
extern  void            FreeABlock( block * blk );
extern  void            FreeBlock( void );
extern  void            EnLink( label_handle label, bool label_dies );
extern  void            AddIns( instruction *ins );
extern  void            GenBlock( block_class class, int targets );
extern  block           *ReGenBlock( block *blk, label_handle lbl );
extern  type_class_def  InitCallState( type_def *tipe );
extern  void            AddTarget( label_handle dest, bool dest_label_dies );
extern  block           *FindBlockWithLbl( label_handle label );
extern  void            FixEdges( void );
extern  bool            FixReturns( void );
extern  void            UnFixEdges( void );
extern  void            AddAnIns( block *blk, instruction *ins );
extern  bool            BlkTooBig( void );
extern  void            NewProc( int level );
extern  void            FreeProc( void );
