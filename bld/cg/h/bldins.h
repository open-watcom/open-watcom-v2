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
* Description:  prototypes for bldins.c
*
****************************************************************************/

extern  an      BGVarargsBasePtr( type_def *tipe );
extern  an      BGStackValue( type_def *tipe );
extern  an      BGInteger( signed_32 value, type_def *tipe );
extern  an      BGInt64( signed_64 value, type_def *tipe );
extern  an      BGFloat( char *value, type_def *tipe );
extern  an      BGName( cg_class cl, pointer sym, type_def *tipe );
extern  an      BGTempName( name *temp, type_def *tipe );
extern  bool    FiniLabel( label_handle lbl, block *blk );
extern  void    BGFiniLabel( label_handle lbl );
extern  bool    NeedConvert( type_def *from, type_def *to );
extern  name    *BGNewTemp( type_def *tipe );
extern  temp_name       *BGGlobalTemp( type_def *tipe );
extern  an      Arithmetic( an name, type_def *tipe );
extern  bn      BGCompare( cg_op op, an left, an rite,
                           label_handle entry, type_def *tipe );
extern  bn      Boolean( an node, label_handle entry );
extern  label_handle    BGGetEntry( void );
extern  void    BG3WayControl( an node, label_handle lt,
                               label_handle eq, label_handle gt );
extern  void    BGControl( cg_op op, bn expr, label_handle lbl );
extern  void    BGGenCtrl( cg_op op, bn expr, label_handle lbl, bool gen );
extern  void    BGBigLabel( bck_info *bck );
extern  void    BGBigGoto( label_handle lbl, int level );
extern  unsigned_32 BGUnrollCount( unsigned_32 unroll_count );
extern  unsigned_32 BGUnrollCount( unsigned_32 unroll_count );
extern  an      BGUnary( cg_op op, an left, type_def *tipe );
extern  an      BGBinary( cg_op op, an left,
                          an rite, type_def *tipe, bool fold_addr );
extern  an      BGOpGets( cg_op op, an left, an rite,
                          type_def *tipe, type_def *optipe );
extern  an      BGConvert( an left, type_def *tipe );
extern  bn      BGFlow( cg_op op, bn left, bn rite );
extern  an      BGAssign( an dst, an src, type_def *tipe );
extern  an      BGCopy( an node );
extern  an      BGDuplicate( an node );
extern  void    BGDone( an node );
extern  void    BGTrash( an node );
extern  void    FlowOff( bn name );
extern  void    BGStartBlock( void );

