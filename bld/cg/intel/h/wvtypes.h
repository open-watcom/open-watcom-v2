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
* Description:  Prototypes for wvtypes.c
*
****************************************************************************/


#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
extern  dbg_type        WVFtnType( const char *name, dbg_ftn_type tipe );
extern  dbg_type        WVScalar( const char *name, cg_type tipe );
extern  dbg_type        WVScope( const char *name );
extern  void            WVDumpName( dbg_name name, dbg_type tipe );
extern  void            WVBackRefType( dbg_name name, dbg_type tipe );
extern  dbg_type        WVCharBlock( unsigned_32 len );
extern  dbg_type        WVIndCharBlock( back_handle len, cg_type len_type, int off );
extern  dbg_type        WVLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        WVFtnArray( back_handle dims, cg_type lo_bound_tipe, cg_type num_elts_tipe, int off, dbg_type base );
extern  dbg_type        WVArray( dbg_type idx, dbg_type base );
extern  dbg_type        WVIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        WVEndArray( dbg_array ar );
extern  dbg_type        WVSubRange( signed_32 lo, signed_32 hi, dbg_type base );
extern  dbg_type        WVDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        WVPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        WVBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc loc_segment );
extern  dbg_type        WVEndStruct( dbg_struct st );
extern  dbg_type        WVEndEnum( dbg_enum en );
extern  dbg_type        WVEndProc( dbg_proc pr );
extern  void            WVTypesEof( void );
extern  void            WVDmpCueInfo( long_offset where );
#endif
