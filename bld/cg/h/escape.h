/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


/*
 * Grammar of Escapes :
 *
 * Sequence                                                 Meaning
 * ========                                                 ========
 *
 * ESC, ESC                                                 actual ESC byte
 * ESC, IMP <LDOF|OFST|BASE|SELF>, cg_sym_handle <,offset>  import reference
 * ESC, REL <LDOF|OFST|BASE>, segid                         relocate, with seg-id
 * ESC, SYM <LDOF|OFST|SELF>, cg_sym_handle <,offset>       unknown sym ref
 * ESC, LBL <LDOF|OFST|SELF>, segid, lbl_handle, <,offset>  ptr reference
 * ESC, ABS objhandle, len, offset                          object patch
 * ESC, FUN byte                                            for 87 instructions
 *
 *       OFST bit on means offset follows
 *       LDOF means loader resolved offset
 *       BASE means use F_BASE relocation
 */

/*  Beginning of any escape sequence */

#define ESC     0x6b    /*  NOT the same as the 8086/8 ESCape instruction */

typedef enum {
    ESC_SYM,
    ESC_IMP,
    ESC_REL,
    ESC_LBL,
    ESC_ABS,
    ESC_FUN,
    ESC_NOC,
    /*  bit flags */
    ESCA_OFST = 0x10,
    ESCA_BASE = 0x20,
    ESCA_LDOF = 0x40,
    ESCA_SELF = 0x80
} escape_class;

#define ESCA_MASK    (ESCA_OFST|ESCA_BASE|ESCA_LDOF|ESCA_SELF)


typedef enum {
    FE_FIX_BASE,
    FE_FIX_OFF,
    FE_FIX_SELF
} fe_fixup_types;
