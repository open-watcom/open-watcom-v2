/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS LFN specific constant definitions.
*
****************************************************************************/


#ifndef __DOSLFN_H_INCLUDED
#define __DOSLFN_H_INCLUDED

#if defined( __WATCOM_LFN__ )

#define _LFN_SIGN           0x004e464cUL    // "LFN"

#define IS_LFN(x)           (_RWD_uselfn && DTALFN_SIGN_OF(x) == _LFN_SIGN && DTALFN_HANDLE_OF(x))

#define LFN_ERROR(x)        ((x) < 0 && (x)!= 0xFFFF7100)
#define LFN_OK(x)           ((x) >= 0)
#define LFN_INFO(x)         ((unsigned short)(x))

#define LFN_DPMI_ERROR(x)   (((x).flags & 1) || (x).ax == 0x7100)
#define LFN_RET_ERROR(e)    ((e) | ~0xFFFF)

#define EX_LFN_OPEN         0x01
#define EX_LFN_CREATE       0x12
#define EX_LFN_CREATE_NEW   0x10

typedef long        lfn_ret_t;

#include "pushpck1.h"
/* The find block for the LFN find */
typedef struct {
    long            attributes;
    unsigned short  creattime;
    unsigned short  creatdate;
    long            cr_pad;
    unsigned short  accesstime;
    unsigned short  accessdate;
    long            ac_pad;
    unsigned short  wrtime;
    unsigned short  wrdate;
    long            wr_pad;
    long            hfilesize;
    long            lfilesize;
    long            reserved[2];
    char            lfn[NAME_MAX + 1];
    char            sfn[14];
} lfnfind_t;

typedef struct {
    long        attributes;
    long long   creattimestamp;
    long long   accesstimestamp;
    long long   writetimestamp;
    long        volid;
    long        hfilesize;
    long        lfilesize;
    long        linksno;
    long        hfileid;
    long        lfileid;
} lfninfo_t;
#include "poppck.h"

#ifdef _M_I86

extern lfn_ret_t __lfnerror_0( void );
extern lfn_ret_t __lfnerror_ax( void );

#else  /* !_M_I86 */

extern char                 * const __lfn_rm_tb_linear;
extern unsigned short       const __lfn_rm_tb_segment;

#define RM_TB_PARM1_SIZE    ( MB_CUR_MAX * _MAX_PATH )
#define RM_TB_PARM2_SIZE    ( MB_CUR_MAX * _MAX_PATH )
#define RM_TB_PARM1_SEGM    __lfn_rm_tb_segment
#define RM_TB_PARM1_OFFS    0
#define RM_TB_PARM1_LINEAR  __lfn_rm_tb_linear
#define RM_TB_PARM2_SEGM    __lfn_rm_tb_segment
#define RM_TB_PARM2_OFFS    RM_TB_PARM1_SIZE
#define RM_TB_PARM2_LINEAR  (__lfn_rm_tb_linear + RM_TB_PARM1_SIZE)

#define __DPMI_DOS_CALL \
        "push   es"         \
        "mov    eax,ds"     \
        "mov    es,eax"     \
        "xor    ecx,ecx"    \
        "mov    bx,21h"     \
        "mov    ax,300h"    \
        "int 31h"           \
        "pop    es"

#define __DPMI_DOS_CALL_INFO \
    __parm __caller     [__edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __ecx]

extern int __dpmi_dos_call_lfn( call_struct *cs );
#pragma aux __dpmi_dos_call_lfn = \
        "or     byte ptr [edi+20H],1" /* cs struct flags carry */ \
        __DPMI_DOS_CALL     \
        "sbb    eax,eax"    \
        "jnz short L2"      \
        "mov    ax,word ptr [edi+1cH]" /* cs struct reg AX */ \
        "test   byte ptr [edi+20H],1" /* cs struct flags carry */ \
        "jne short L1"      \
        "cmp    ax, 7100h"  \
        "je short L1"       \
        "xor    eax,eax"    \
        "jmp short L2"      \
    "L1: or     eax,0ffff0000h" \
    "L2:"                   \
    __DPMI_DOS_CALL_INFO

extern int __dpmi_dos_call_lfn_ax( call_struct *cs );
#pragma aux __dpmi_dos_call_lfn_ax = \
        "or     byte ptr [edi+20H],1" /* cs struct flags carry */ \
        __DPMI_DOS_CALL     \
        "sbb    eax,eax"    \
        "jnz short L2"      \
        "mov    ax,word ptr [edi+1cH]" /* cs struct reg AX */ \
        "test   byte ptr [edi+20H],1" /* cs struct flags carry */ \
        "jne short L1"      \
        "cmp    ax, 7100h"  \
        "jne short L2"      \
    "L1: or     eax,0ffff0000h" \
    "L2:"                   \
    __DPMI_DOS_CALL_INFO

#endif  /* !_M_I86 */

extern lfn_ret_t _dos_create_open_ex_lfn( const char *path, unsigned mode, unsigned attrib, unsigned action );

#endif  /* __WATCOM_LFN__ */

#endif
