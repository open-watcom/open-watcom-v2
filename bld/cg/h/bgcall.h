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
* Description:  Prototypes for i86call.c
*
****************************************************************************/


extern  an      BGCall( cn call, bool use_return, bool in_line );
extern  void    BGProcDecl( cg_sym_handle sym, type_def *tipe );
extern  name    *StReturn( an retval, type_def *tipe, instruction **pins );
extern  reg_set_index   CallIPossible( instruction *ins );
extern  void    InitTargProc( void );
extern  void    SaveToTargProc( void );
extern  void    RestoreFromTargProc( void );
extern  void    PushInSameBlock( instruction *ins );
extern  instruction     *PushOneParm( instruction *ins, name *curr,
                                      type_class_def class, type_length offset,
                                      call_state *state );
extern  void    PreCall( cn call );
extern  void    PostCall( cn call );
extern  type_def        *PassParmType( cg_sym_handle func, type_def *tipe, call_class class );
