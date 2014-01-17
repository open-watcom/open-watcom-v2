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
* Description:  Prototypes for cvtypes.c
*
****************************************************************************/


/* CV interface */
extern  dbg_type        CVFtnType( const char *name, dbg_ftn_type tipe );
extern  dbg_type        CVScalar( const char *name, cg_type tipe );
extern  dbg_type        CVScope( const char *name );
extern  void            CVDumpName( dbg_name name, dbg_type tipe );
extern  void            CVBackRefType( dbg_name name, dbg_type tipe );
extern  dbg_type        CVCharBlock( unsigned_32 len );
extern  dbg_type        CVIndCharBlock( back_handle len, cg_type len_type, int off );
extern  dbg_type        CVLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        CVFtnArray( back_handle dims, cg_type lo_bound_tipe, cg_type num_elts_tipe, int off, dbg_type base );
extern  dbg_type        CVArray( dbg_type idx, dbg_type base );
extern  dbg_type        CVIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        CVEndArray( dbg_array ar );
extern  dbg_type        CVArraySize( offset size, unsigned_32 hi, dbg_type base );
extern  dbg_type        CVSubRange( signed_32 lo, signed_32 hi, dbg_type base );
extern  dbg_type        CVDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        CVPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        CVBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc loc_segment );
extern  dbg_type        CVEndStruct( dbg_struct st );
extern  dbg_type        CVEndEnum( dbg_enum en );
extern  dbg_type        CVEndProc( dbg_proc pr );
