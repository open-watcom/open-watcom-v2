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
* Description:  Prototypes for bldcall.c
*
****************************************************************************/
extern  type_class_def  AddCallBlock( cg_sym_handle sym, type_def *tipe );
extern  void    BGFiniCall( cn call );
extern  cn      BGInitCall( an node, type_def *tipe, aux_handle aux );
extern  void    BGAddParm( cn call, an parm );
extern  void    BGAutoDecl( cg_sym_handle sym, type_def *tipe );
extern  name    *DoParmDecl( cg_sym_handle sym, type_def *tipe, hw_reg_set reg );
extern  void    BGParmDecl( cg_sym_handle sym, type_def *tipe );
extern  void    AddCallIns( instruction *ins, cn call );
extern  void    ReverseParmNodeList( pn *owner );
extern  void    PushParms( pn parm, call_state *state );
extern  void    ReserveStack( call_state *state, instruction *prev, type_length len );
extern  void    ParmIns( pn parm, call_state *state );
extern  void    BGZapBase( name *base, type_def *tipe );
extern  void    BGReturn( an retval, type_def *tipe );
extern  bool    AssgnParms( cn call, bool in_line );
