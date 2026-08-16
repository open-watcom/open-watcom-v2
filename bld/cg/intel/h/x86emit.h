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

#ifndef X86_EMIT_INCLUDED
#define X86_EMIT_INCLUDED

#include "x86objd.h"
#include "fppatch.h"
#include "rtrtn.h"

/*
 * Format-neutral object writer used by the Intel backends.  The existing
 * implementation is the OMF provider.
 */
typedef struct x86_obj_emitter {
    void            (*init_seg_defs)( void );
    bool            (*free_obj_cache)( void );
    segment_id      (*ask_op)( void );
    segment_id      (*change_op)( segment_id );
    void            (*set_op)( segment_id );
    void            (*def_segment)( segment_id, seg_attr, const char *, uint, bool );
    void            (*empty_queue)( void );
    char            (*get_mem_model)( void );
    segment_id      (*dbg_seg_def)( const char *, const char *, int );
    void            (*obj_init)( void );
    offset          (*ask_location)( void );
    void            (*check_dbg_size)( offset, bool );
    bool            (*use_import_form)( fe_attr );
    bool            (*seg_is_near)( segment_id );
    bool            (*seg_is_blank)( segment_id );
    bool            (*seg_is_private)( segment_id );
    bool            (*seg_is_rom)( segment_id );
    segment_id      (*ask_back_seg)( void );
    segment_id      (*ask_code_seg)( void );
    bool            (*have_code_seg)( void );
    segment_id      (*ask_alt_code_seg)( void );
    segment_id      (*ask_code16_seg)( void );
    void            (*out_select)( bool );
    void            (*set_up_obj)( bool );
    void            (*flush_op)( segment_id );
    void            (*obj_fini)( void );
    void            (*out_dll_export)( uint, cg_sym_handle );
    void            (*out_label)( label_handle );
    void            (*abs_patch)( abspatch_handle, offset );
    void            *(*init_patches)( void );
    void            (*set_big_location)( long_offset );
    void            (*inc_location)( offset );
    void            (*set_location)( offset );
    void            (*out_fp_patch)( fp_patches );
    void            (*out_patch)( label_handle, patch_attr );
    abs_patch       *(*new_abs_patch)( void );
    void            (*out_data_byte)( byte );
    void            (*out_data_short)( uint_16 );
    void            (*out_data_long)( uint_32 );
    void            (*out_abs_patch)( abs_patch *, patch_attr );
    void            (*out_reloc)( segment_id, fix_class, bool );
    void            (*out_special_common)( import_handle, fix_class, bool );
    void            (*out_import)( cg_sym_handle, fix_class, bool );
    void            (*out_rt_import_rel)( rt_class, fix_class, bool );
    void            (*out_rt_import)( rt_class, fix_class );
    void            (*out_bck_export)( const char *, bool );
    void            (*out_bck_import)( const char *, back_handle, fix_class );
    void            (*out_line_num)( cg_linenum, bool );
    unsigned        (*save_pending_line)( unsigned );
    void            (*out_bytes)( unsigned, const byte * );
    void            (*out_iterated)( byte, offset );
    bool            (*need_base_set)( void );
    offset          (*ask_max_size)( void );
    long_offset     (*ask_big_location)( void );
    long_offset     (*ask_big_max_size)( void );
    void            (*tell_new_label)( cg_sym_handle );
    void            (*tell_new_proc)( cg_sym_handle );
    void            (*tell_virt_func_ref)( void * );
    segment_id      (*ask_seg_id)( pointer, cg_class );
    bool            (*name_is_code)( pointer, cg_class );
    bool            (*name_is_rom)( pointer, cg_class );
    bool            (*sym_is_exported)( cg_sym_handle );
} x86_obj_emitter;

extern const x86_obj_emitter X86OmfEmitter;

#endif
