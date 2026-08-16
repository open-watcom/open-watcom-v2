/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
****************************************************************************/

#include "_cgstd.h"
#include "coderep.h"
#include "model.h"
#include "x86objd.h"
#include "objout.h"
#include "x86obj.h"
#include "x86emit.h"

static const x86_obj_emitter *Emitter = &X86OmfEmitter;

static void selectObjEmitter( void )
{
#if _TARGET & _TARG_80386
    if( _IsModel( CGSW_GEN_OBJ_OWL ) ) {
        Emitter = &X86OwlEmitter;
    } else
#endif
    {
        Emitter = &X86OmfEmitter;
    }
}

void InitSegDefs( void ) { selectObjEmitter(); Emitter->init_seg_defs(); }
bool FreeObjCache( void ) { return( Emitter->free_obj_cache() ); }
segment_id AskOP( void ) { return( Emitter->ask_op() ); }
segment_id ChangeOP( segment_id s ) { return( Emitter->change_op( s ) ); }
void SetOP( segment_id s ) { Emitter->set_op( s ); }
void DefSegment( segment_id s, seg_attr a, const char *n, uint l, bool u ) { Emitter->def_segment( s, a, n, l, u ); }
void DoEmptyQueue( void ) { Emitter->empty_queue(); }
char GetMemModel( void ) { return( Emitter->get_mem_model() ); }
segment_id DbgSegDef( const char *n, const char *c, int m ) { return( Emitter->dbg_seg_def( n, c, m ) ); }
void ObjInit( void ) { Emitter->obj_init(); }
offset AskLocation( void ) { return( Emitter->ask_location() ); }
void ChkDbgSegSize( offset m, bool t ) { Emitter->check_dbg_size( m, t ); }
bool UseImportForm( fe_attr a ) { return( Emitter->use_import_form( a ) ); }
bool AskSegIsNear( segment_id s ) { return( Emitter->seg_is_near( s ) ); }
bool AskSegIsBlank( segment_id s ) { return( Emitter->seg_is_blank( s ) ); }
bool AskSegIsPrivate( segment_id s ) { return( Emitter->seg_is_private( s ) ); }
bool AskSegIsROM( segment_id s ) { return( Emitter->seg_is_rom( s ) ); }
segment_id AskBackSeg( void ) { return( Emitter->ask_back_seg() ); }
segment_id AskCodeSeg( void ) { return( Emitter->ask_code_seg() ); }
bool HaveCodeSeg( void ) { return( Emitter->have_code_seg() ); }
segment_id AskAltCodeSeg( void ) { return( Emitter->ask_alt_code_seg() ); }
segment_id AskCode16Seg( void ) { return( Emitter->ask_code16_seg() ); }
void OutSelect( bool s ) { Emitter->out_select( s ); }
void SetUpObj( bool d ) { Emitter->set_up_obj( d ); }
void FlushOP( segment_id s ) { Emitter->flush_op( s ); }
void ObjFini( void ) { Emitter->obj_fini(); }
void OutDLLExport( uint w, cg_sym_handle s ) { Emitter->out_dll_export( w, s ); }
void OutLabel( label_handle l ) { Emitter->out_label( l ); }
void AbsPatch( abspatch_handle p, offset l ) { Emitter->abs_patch( p, l ); }
void *InitPatches( void ) { return( Emitter->init_patches() ); }
void SetBigLocation( long_offset l ) { Emitter->set_big_location( l ); }
void IncLocation( offset l ) { Emitter->inc_location( l ); }
void SetLocation( offset l ) { Emitter->set_location( l ); }
void OutFPPatch( fp_patches p ) { Emitter->out_fp_patch( p ); }
void OutPatch( label_handle l, patch_attr a ) { Emitter->out_patch( l, a ); }
abs_patch *NewAbsPatch( void ) { return( Emitter->new_abs_patch() ); }
void OutDataByte( byte v ) { Emitter->out_data_byte( v ); }
void OutDataShort( uint_16 v ) { Emitter->out_data_short( v ); }
void OutDataLong( uint_32 v ) { Emitter->out_data_long( v ); }
void OutAbsPatch( abs_patch *p, patch_attr a ) { Emitter->out_abs_patch( p, a ); }
void OutReloc( segment_id s, fix_class c, bool r ) { Emitter->out_reloc( s, c, r ); }
void OutSpecialCommon( import_handle h, fix_class c, bool r ) { Emitter->out_special_common( h, c, r ); }
void OutImport( cg_sym_handle s, fix_class c, bool r ) { Emitter->out_import( s, c, r ); }
void OutRTImportRel( rt_class i, fix_class c, bool r ) { Emitter->out_rt_import_rel( i, c, r ); }
void OutRTImport( rt_class i, fix_class c ) { Emitter->out_rt_import( i, c ); }
void OutBckExport( const char *n, bool e ) { Emitter->out_bck_export( n, e ); }
void OutBckImport( const char *n, back_handle b, fix_class c ) { Emitter->out_bck_import( n, b, c ); }
void OutLineNum( cg_linenum l, bool b ) { Emitter->out_line_num( l, b ); }
unsigned SavePendingLine( unsigned l ) { return( Emitter->save_pending_line( l ) ); }
void OutDBytes( unsigned l, const byte *s ) { Emitter->out_bytes( l, s ); }
void OutIBytes( byte p, offset l ) { Emitter->out_iterated( p, l ); }
bool NeedBaseSet( void ) { return( Emitter->need_base_set() ); }
offset AskMaxSize( void ) { return( Emitter->ask_max_size() ); }
long_offset AskBigLocation( void ) { return( Emitter->ask_big_location() ); }
long_offset AskBigMaxSize( void ) { return( Emitter->ask_big_max_size() ); }
void TellObjNewLabel( cg_sym_handle l ) { Emitter->tell_new_label( l ); }
void TellObjNewProc( cg_sym_handle p ) { Emitter->tell_new_proc( p ); }
void TellObjVirtFuncRef( void *c ) { Emitter->tell_virt_func_ref( c ); }
segment_id AskSegID( pointer h, cg_class c ) { return( Emitter->ask_seg_id( h, c ) ); }
bool AskNameIsCode( pointer h, cg_class c ) { return( Emitter->name_is_code( h, c ) ); }
bool AskNameIsROM( pointer h, cg_class c ) { return( Emitter->name_is_rom( h, c ) ); }
bool SymIsExported( cg_sym_handle s ) { return( Emitter->sym_is_exported( s ) ); }
