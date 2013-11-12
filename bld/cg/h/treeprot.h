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
* Description:  Prototypes for tree.c
*
****************************************************************************/
extern  tn      TGLeaf( an addr );
extern  tn      TGBitMask( tn left, byte start, byte len, type_def *tipe );
extern  tn      TGNode( tn_class class, cg_op op, tn left, tn rite,
                        type_def *tipe );
extern  tn      TGWarp( tn before, label_handle label, tn after );
extern  tn      TGHandle( void );
extern  tn      TGCallback( cg_callback rtn, callback_handle ptr );
extern  tn      TGCompare( cg_op op, tn left, tn rite, type_def *tipe );
extern  unsigned_32 TGMask32( tn node );
extern  tn      TGConvert( tn name, type_def *tipe );
extern  tn      TGBinary( cg_op op, tn left, tn rite, type_def *tipe );
extern  tn      TGUnary( cg_op op, tn left, type_def *tipe );
extern  tn      TGInitCall( tn left, type_def *tipe, cg_sym_handle sym );
extern  tn      TGAddParm( tn to, tn parm, type_def *tipe );
extern  tn      TGCall( tn what );
extern  tn      TGIndex( tn left, tn rite, type_def *tipe, type_def *ptipe );
extern  tn      DoTGAssign( tn dst, tn src, type_def *tipe, tn_class class );
extern  tn      TGAssign( tn dst, tn src, type_def *tipe );
extern  tn      TGLVAssign( tn dst, tn src, type_def *tipe );
extern  name    *TGetName( tn node );
extern  tn      TGReLeaf( an addr );
extern  tn      TGTmpLeaf( an addr );
extern  tn      TGConst( float_handle cons, type_def *tipe );
extern  tn      DoTGPreGets( cg_op op, tn left, tn rite, type_def *tipe,
                             tn_class class, tn_class assn_class );
extern  tn      TGPreGets( cg_op op, tn left, tn rite, type_def *tipe );
extern  tn      TGLVPreGets( cg_op op, tn left, tn rite, type_def *tipe );
extern  tn      TGPostGets( cg_op op, tn left, tn rite, type_def *tipe );
extern  cg_type TGType( tn node );
extern  tn      TGPatch( patch_handle hdl, type_def *tipe );
extern  tn      TGFlow( cg_op op, tn left, tn rite );
extern  tn      TGTrash( tn node );
extern  tn      TGAttr( tn node, cg_sym_attr attr );
extern  tn      TGAlign( tn node, uint align );
extern  tn      TGVolatile( tn node );
extern  void    TG3WayControl( tn node, label_handle lt, label_handle eq, label_handle gt );
extern  void    TGControl( cg_op op, tn node, label_handle lbl );
extern  an      TGen( tn node, type_def *tipe );
extern  an      TGReturn( tn node, type_def *tipe );
extern  an      TNFlow( tn node );
extern  an      TNPostGets( tn node );
extern  an      TNPreGets( tn node );
extern  an      TNAssign( tn node );
extern  an      TNCompare( tn node );
extern  an      TNUnary( tn node );
extern  an      TNBinary( tn node );
extern  void    TNZapParms( void );
extern  bool    TGIsAddress( void );
extern  tn      TGQuestion( tn sel, tn left, tn rite, type_def *tipe );
extern  void    BurnTree( tn node );
extern  void    TInit( void );
extern  bool    TreeFrlFree( void );
extern  void    TFini( void );
extern  an      TreeGen( tn node );
extern  tn      TGDuplicate( tn node );
extern  bool    TGCanDuplicate( tn node );
