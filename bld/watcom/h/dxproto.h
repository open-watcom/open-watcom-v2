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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "metaconv.h"

#pragma aux (metaware) _dx_reset_data;
#pragma aux (metaware) _dx_pmiv_get;
#pragma aux (metaware) _dx_rmiv_get;
#pragma aux (metaware) _dx_pmiv_set;
#pragma aux (metaware) _dx_rmiv_set;
#pragma aux (metaware) _dx_apmiv_set;
#pragma aux (metaware) _dx_rpmiv_set;
#pragma aux (metaware) _dx_tolinear;
#pragma aux (metaware) _dx_tophys;
#pragma aux (metaware) _dx_map_phys;
#pragma aux (metaware) _dx_hwiv_get;
#pragma aux (metaware) _dx_rmlink_get;
#pragma aux (metaware) _dx_real_call;
#pragma aux (metaware) _dx_toreal;
#pragma aux (metaware) _dx_real_regcall;
#pragma aux (metaware) _dx_real_int;
#pragma aux (metaware) _dx_dbg_load;
#pragma aux (metaware) _dx_seg_alias;
#pragma aux (metaware) _dx_segattr_set;
#pragma aux (metaware) _dx_segattr_get;
#pragma aux (metaware) _dx_ldtmem_free;
#pragma aux (metaware) _dx_dosbuf_get;
#pragma aux (metaware) _dx_segmove_hand;
#pragma aux (metaware) _dx_memerr_get;
#pragma aux (metaware) _dx_page_lock;
#pragma aux (metaware) _dx_page_lockl;
#pragma aux (metaware) _dx_page_unlock;
#pragma aux (metaware) _dx_page_unlockl;
#pragma aux (metaware) _dx_page_free;
#pragma aux (metaware) _dx_page_freel;
#pragma aux (metaware) _dx_pte_read;
#pragma aux (metaware) _dx_pte_readl;
#pragma aux (metaware) _dx_pte_write;
#pragma aux (metaware) _dx_pte_writel;
#pragma aux (metaware) _dx_pte_xchg;
#pragma aux (metaware) _dx_pte_xchgl;
#pragma aux (metaware) _dx_vm_stats;
#pragma aux (metaware) _dx_xmem_limit;
#pragma aux (metaware) _dx_pgflt_hand;
#pragma aux (metaware) _dx_oswsp_hand;
#pragma aux (metaware) _dx_pgrepl_hand;
#pragma aux (metaware) _dx_cmem_limit;
#pragma aux (metaware) _dx_config_inf;
#pragma aux (metaware) _dx_dbg_regsav;
#pragma aux (metaware) _dx_cc_rdregs;
#pragma aux (metaware) _dx_ld_flat;
#pragma aux (metaware) _dx_dbg_ld;
#pragma aux (metaware) _dx_umap_pgs;
#pragma aux (metaware) _dx_umap_pgsl;
#pragma aux (metaware) _dx_alloc_pgs;
#pragma aux (metaware) _dx_alloc_pgsl;
#pragma aux (metaware) _dx_map_pgs;
#pragma aux (metaware) _dx_map_pgsl;
#pragma aux (metaware) _dx_mfile_pgs;
#pragma aux (metaware) _dx_mfile_pgsl;
#pragma aux (metaware) _dx_gtyp_pgs;
#pragma aux (metaware) _dx_gtyp_pgsl;
#pragma aux (metaware) _dx_lock_pgs;
#pragma aux (metaware) _dx_lock_pgsl;
#pragma aux (metaware) _dx_ulock_pgs;
#pragma aux (metaware) _dx_ulock_pgsl;
#pragma aux (metaware) _dx_fphys_pgs;
#pragma aux (metaware) _dx_fphys_pgsl;
#pragma aux (metaware) _dx_add_umapg;
#pragma aux (metaware) _dx_vmm_close;
#pragma aux (metaware) _dx_vmm_pget;
#pragma aux (metaware) _dx_vmm_pset;
#pragma aux (metaware) _dx_vmm_wlog;
#pragma aux (metaware) _dx_dosbuf_set;
#pragma aux (metaware) _dx_ldt_rd;
#pragma aux (metaware) _dx_ldt_wr;
#pragma aux (metaware) _dx_excep_get;
#pragma aux (metaware) _dx_excep_set;
#pragma aux (metaware) _dx_if_get;
#pragma aux (metaware) _dx_sysregs_get;
#pragma aux (metaware) _dx_sysregs_set;
#pragma aux (metaware) _dx_cmem_usage;
#pragma aux (metaware) _dx_ext_usage;
#pragma aux (metaware) _dx_real_above;
#pragma aux (metaware) _dx_swi_rdregs;
#pragma aux (metaware) _dx_hdroff_get;
#pragma aux (metaware) _dx_msegfail_hnd;
#pragma aux (metaware) _dx_jmp_toreal;
#pragma aux (metaware) _dx_shrink_swap;
#pragma aux (metaware) _dx_idt_rd;
#pragma aux (metaware) _dx_idt_wr;
#pragma aux (metaware) _dx_real_alloc;
#pragma aux (metaware) _dx_real_free;
#pragma aux (metaware) _dx_real_resize;
#pragma aux (metaware) _dx_exec_spec;
#pragma aux (metaware) _dos_char_ine;
#pragma aux (metaware) _dos_char_out;
#pragma aux (metaware) _dos_str_out;
#pragma aux (metaware) _dos_buff_in;
#pragma aux (metaware) _dos_ver;
#pragma aux (metaware) _dos_create;
#pragma aux (metaware) _dos_open;
#pragma aux (metaware) _dos_close;
#pragma aux (metaware) _dos_read;
#pragma aux (metaware) _dos_write;
#pragma aux (metaware) _dos_delete;
#pragma aux (metaware) _dos_seek;
#pragma aux (metaware) _dos_force_hnd;
#pragma aux (metaware) _dos_seg_alloc;
#pragma aux (metaware) _dos_seg_free;
#pragma aux (metaware) _dos_seg_resize;
#pragma aux (metaware) _dos_alloc_sset;
#pragma aux (metaware) _dos_alloc_sget;
