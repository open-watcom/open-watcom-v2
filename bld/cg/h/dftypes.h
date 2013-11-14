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
* Description:  Prototypes for dftypes.c
*
****************************************************************************/


extern  dbg_type        DFFtnType( const char *name, dbg_ftn_type tipe );
extern  dbg_type        DFScalar( const char *name, cg_type tipe );
extern  dbg_type        DFScope( const char *name );
extern  void            DFDumpName( dbg_name name, dbg_type tipe );
extern  void            DFBackRefType( dbg_name name, dbg_type tipe );
extern  dbg_type        DFCharBlock( unsigned_32 len );
extern  dbg_type        DFCharBlockNamed( const char *name, unsigned_32 len );
extern  dbg_type        DFIndCharBlock( back_handle len, cg_type len_type, int off );
extern  dbg_type        DFLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        DFFtnArray( back_handle dims, cg_type lo_bound_tipe, cg_type num_elts_tipe, int off, dbg_type base );
extern  dbg_type        DFArray( dbg_type idx, dbg_type base );
extern  dbg_type        DFIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        DFEndArray( dbg_array ar );
extern  dbg_type        DFSubRange( signed_32 lo, signed_32 hi, dbg_type base );
extern  dbg_type        DFDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        DFPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        DFBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc loc_segment );
extern  void            DFBegStruct( dbg_struct st );
extern  dbg_type        DFEndStruct( dbg_struct st );
extern  dbg_type        DFEndEnum( dbg_enum en );
extern  dbg_type        DFEndProc( dbg_proc pr );
