;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  overlay manager shared data structures definition
;*               they are used by WLINK and OVLLDR
;*
;*****************************************************************************

;   !!!!!!!!!   must correspond to declaration in ovltab.h  !!!!!!!!!

OVL_MAJOR_VERSION       equ     3
OVL_MINOR_VERSION       equ     0

OVL_SIGNATURE   equ     2112h

OVLTAB_ENTRY    STRUC
    ove_flags_anc       dw      ?
    ove_relocs          dw      ?
    ove_start_para      dw      ?
    ove_code_handle     dw      ?
    ove_num_paras       dw      ?
    ove_fname           dw      ?
    ove_disk_addr       dd      ?
OVLTAB_ENTRY    ENDS

OVLTAB_PROLOG   STRUC
    ovp_major           db      ?
    ovp_minor           db      ?
    ovp_start           dd      ?
    ovp_delta           dw      ?
    ovp_ovl_size        dw      ?
OVLTAB_PROLOG   ENDS

OVLTAB          STRUC
    ov_prolog           db  size OVLTAB_PROLOG dup(?)
    ov_entries          db  size OVLTAB_ENTRY dup(?)
OVLTAB          ENDS

OVLTAB_TERMINATOR       equ     0ffffh
