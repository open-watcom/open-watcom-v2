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


#define NC '\0'

hw_reg_set DefaultParms[] = {
#if _CPU == 8086
        HW_AX+HW_BX+HW_CX+HW_DX+HW_ST1+HW_ST2+HW_ST3+HW_ST4,
#elif _CPU == 386
        HW_D_4( HW_EAX,HW_EBX,HW_ECX,HW_EDX ) /*+HW_ST1+HW_ST2+HW_ST3+HW_ST4*/,
#endif
        0 };

#ifndef WCPP
        char Registers[] = {            /* table for TableLookup*/
                'e','s',NC,             /* should agree with RegBits*/
                'd','s',NC,
                'c','s',NC,
                's','s',NC,
                'a','x',NC,
                'b','x',NC,
                'c','x',NC,
                'd','x',NC,
                'a','l',NC,
                'b','l',NC,
                'c','l',NC,
                'd','l',NC,
                'a','h',NC,
                'b','h',NC,
                'c','h',NC,
                'd','h',NC,
                's','i',NC,
                'd','i',NC,
                'b','p',NC,
                's','p',NC,
#if _CPU == 386
                'f','s',NC,
                'g','s',NC,
                'e','a','x',NC,
                'e','b','x',NC,
                'e','c','x',NC,
                'e','d','x',NC,
                'e','s','i',NC,
                'e','d','i',NC,
                'e','b','p',NC,
                'e','s','p',NC,
#endif
                NC };

        hw_reg_set RegBits[] ={ HW_D( HW_ES ),
                                HW_D( HW_DS ),
                                HW_D( HW_CS ),
                                HW_D( HW_SS ),
                                HW_D( HW_AX ),
                                HW_D( HW_BX ),
                                HW_D( HW_CX ),
                                HW_D( HW_DX ),
                                HW_D( HW_AL ),
                                HW_D( HW_BL ),
                                HW_D( HW_CL ),
                                HW_D( HW_DL ),
                                HW_D( HW_AH ),
                                HW_D( HW_BH ),
                                HW_D( HW_CH ),
                                HW_D( HW_DH ),
                                HW_D( HW_SI ),
                                HW_D( HW_DI ),
                                HW_D( HW_BP ),
                                HW_D( HW_SP ),
#if _CPU == 386
                                HW_D( HW_FS ),
                                HW_D( HW_GS ),
                                HW_D( HW_EAX ),
                                HW_D( HW_EBX ),
                                HW_D( HW_ECX ),
                                HW_D( HW_EDX ),
                                HW_D( HW_ESI ),
                                HW_D( HW_EDI ),
                                HW_D( HW_BP ),
                                HW_D( HW_SP ),
#endif
 };

hw_reg_set DefaultVarParms[] = {
        0 };

/*      these are the registers that Microsoft saves and restores */

hw_reg_set MSC_Save = { HW_D_3( HW_ESI, HW_EDI, HW_BP ) };

/*      INLINE FUNCTIONS */

struct  aux_info        InlineInfo;

enum    asm_codes {
        push_es = 0x06,
        pop_es  = 0x07,
        sbb_rr  = 0x19,
        push_ds = 0x1e,
        pop_ds  = 0x1f,
        sub_rr  = 0x29,
        xor_rr  = 0x31,
        cmp_al  = 0x3c,
        inc_cx  = 0x41,
        inc_di  = 0x47,
        dec_cx  = 0x49,
        dec_si  = 0x4e,
        dec_di  = 0x4f,
        push_ax = 0x50,
        push_cx = 0x51,
        push_dx = 0x52,
        push_si = 0x56,
        push_di = 0x57,
        pop_ax  = 0x58,
        pop_cx  = 0x59,
        pop_dx  = 0x5a,
        pop_si  = 0x5e,
        pop_di  = 0x5f,
        size_prefix = 0x66,
        je      = 0x74,
        jne     = 0x75,
        jge     = 0x7d,
        xchg_cx_ax = 0x91,
        xchg_si_ax = 0x96,
        cbw     = 0x98,
        cwde    = 0x98,
        cwd     = 0x99,
        fwait   = 0x9b,
        movsb   = 0xa4,
        movsd   = 0xa5,
        cmpsb   = 0xa6,
        stosb   = 0xaa,
        stosd   = 0xab,
        lodsb   = 0xac,
        lodsd   = 0xad,
        scasb   = 0xae,
        jcxz    = 0xe3,
        in_al_dx= 0xec,
        in_ax_dx= 0xed,
        out_dx_al=0xee,
        out_dx_ax=0xef,
        rep     = 0xf2,
        repnz   = 0xf2,
        repz    = 0xf3,
        cli     = 0xfa,
        sti     = 0xfb,
        cld     = 0xfc,
        ax_ax   = 0xc0,
        di_cx   = 0xcf,
        ax_dx   = 0xd0,
        cx_cx   = 0xc9,
        si_si   = 0xf6,
        hide1   = 0xa8,
        cmc     = 0xf5,
};
#define hide2           size_prefix,0xa9
#define movsw           size_prefix,movsd
#define stosw           size_prefix,stosd
#define lodsw           size_prefix,lodsd
#define sbb_ax_ffff     0x83,0xd8,0xff
#define sbb_cx_ffff     0x83,0xd9,0xff
#define mov_cx_ax       0x89,0xc1
#define mov_dx_ax       0x89,0xc2
#define mov_di_ax       0x89,0xc7
#define mov_ax_cx       0x89,0xc8
#define mov_ax_dx       0x89,0xd0
#define mov_ax_bx       0x89,0xd8
#define mov_si_bx       0x89,0xde
#define mov_dx_si       0x89,0xf2
#define mov_ax_di       0x89,0xf8
#define mov_cl_al       0x8a,0xc8
#define mov_ax_es       0x8c,0xc0
#define mov_cx_es       0x8c,0xc1
#define mov_dx_es       0x8c,0xc2
#define mov_ax_ds       0x8c,0xd8
#define mov_dx_ds       0x8c,0xda
#define mov_ds_ax       0x8e,0xd8
#define mov_es_ax       0x8e,0xc0
#define mov_es_cx       0x8e,0xc1
#define mov_es_dx       0x8e,0xc2
#define mov_ds_cx       0x8e,0xd9
#define mov_ds_dx       0x8e,0xda
#define mov_ds_si       0x8e,0xde
#define mov_ah_atdi     0x8a,0x27
#define mov_ah_atesdi   0x26,0x8a,0x27
#define not_cx          0xf7,0xd1
#define or_cx_cx        0x09,0xc9
#define or_dx_dx        0x0b,0xd2
#define neg_ax          0xf7,0xd8
#define adc_dx_0        0x83,0xd2,0x00
#define neg_dx          0xf7,0xda
#define sub_al_ah       0x28,0xe0
#define sub_ah_ah       0x28,0xe4
#define shr_cx_1        0xd1,0xe9
#define shr_cx_2        0xc1,0xe9,0x02
#define adc_cx_cx       0x11,0xc9
#define xchg_si_di      0x87,0xfe
#define mov_cx_ffff     0x29,0xc9,0x49  /* sub cx,cx; dec cx */
#define test_al_al      0x84,0xc0
#define test_ah_ah      0x84,0xe4
#define test_al_aa      0xa8,0xaa
#define fabs            0xd9,0xe1
#define and_cl          0x80,0xe1
#define and_ah_7f       0x80,0xe4,0x7f
#define mov_ah_al       0x88,0xc4
#define mov_di_cx       0x89,0xcf
#define cmp_al_dl       0x38,0xd0
#define cmp_ah_dl       0x38,0xd4
#define cmp_al_cl       0x38,0xc8
#define cmp_ah_cl       0x38,0xcc
#define cmp_al_ah       0x38,0xe0
#define mov_dx_si       0x89,0xf2
#define cmp_atdi_0      0x80,0x3f,0x00
#define cmp_atesdi_0    0x26,0x80,0x3f,0x00
#define idiv_cx         0xf7,0xf9
#define shl_edx_1       0xd1,0xe2
#define shr_edx_1       0xd1,0xea
#define sub_ax_dx       0x29,0xd0
#define sbb_cx_cx       0x19,0xc9
#define and_ax_cx       0x21,0xc8
#define add_ax_dx       0x01,0xd0
#define sbb_ah_ah       0x18,0xe4

/*
  here come the code bursts ...
*/
#include "code386.gh"

/* dx:ax  div( ax, cx ) */
#define C_div_ret       HW_D( HW_EAX )
#define C_div_parms     P_AX_CX
#define C_div_saves      HW_NotD_2( HW_EAX, HW_EDX )
static byte_seq C_div = {
                8,
                cwd,
                idiv_cx,
                0x89,0x06,      /* mov [esi],eax   store quotient */
                0x89,0x56,0x04  /* mov 4[esi],edx  store remainder */
        };


/* di memset( di, al, cx ) zaps es,cx */
#define S_memset_ret    HW_D( HW_EDI )
#define S_memset_parms  P_DI_AL_CX
#define S_memset_saves   HW_NotD( HW_ECX )
static byte_seq S_memset = {
                8,
                push_es,
                push_di,
                push_ds,
                pop_es,
                rep,
                stosb,
                pop_di,
                pop_es
        };
static byte_seq FS_memset = {
                4,
                push_di,
                rep,
                stosb,
                pop_di,
        };


/* di memset( di, al, cx ) zaps es,ah,cx */
#define C_memset_ret    HW_D( HW_EDI )
#define C_memset_parms  P_DI_AL_CX
#define C_memset_saves   HW_NotD_2( HW_AH, HW_ECX )
#define C_memset_bodysize 13
#define C_memset_body \
                push_di,\
                mov_ah_al,\
                shr_cx_1,\
                rep,\
                stosw,\
                adc_cx_cx,\
                rep,\
                stosb,\
                pop_di
static byte_seq C_memset = {
                C_memset_bodysize+4,
                push_es,
                push_ds,
                pop_es,
                C_memset_body,
                pop_es
        };
static byte_seq FC_memset = {
                C_memset_bodysize,
                C_memset_body,
        };


/* es:di memset( es:di, al, cx ) zaps cx */
#define Z_memset_ret    HW_D_2( HW_ES, HW_EDI )
#define Z_memset_parms  P_ESDI_AL_CX
#define Z_memset_saves   HW_NotD( HW_ECX )
static byte_seq Z_memset = {
                4,
                push_di,
                rep,
                stosb,
                pop_di
        };


/* es:di memset( es:di, al, cx ) zaps ah,cx */
#define BD_memset_ret   HW_D_2( HW_ES, HW_EDI )
#define BD_memset_parms P_ESDI_AL_CX
#define BD_memset_saves  HW_NotD_2( HW_AH, HW_ECX )
static byte_seq BD_memset = {
                13,
                push_di,
                mov_ah_al,
                shr_cx_1,
                rep,
                stosw,
                adc_cx_cx,
                rep,
                stosb,
                pop_di
        };

/* dx:edi memset( dx:edi, al, ecx ) zaps ah,ecx */
#define DP_memset_ret   HW_D_2( HW_DX, HW_EDI )
#define DP_memset_parms P_DXEDI_AL_ECX
#define DP_memset_saves  HW_NotD_2( HW_AH, HW_ECX )
static byte_seq DP_memset = {
                17,
                push_es,
                push_di,
                mov_es_dx,
                mov_ah_al,
                shr_cx_1,
                rep,
                stosw,
                adc_cx_cx,
                rep,
                stosb,
                pop_di,
                pop_es
        };
/****************************************************************/

/* edi stosw( es:edi, ax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be even */
#define C_stosw_ret    HW_D( HW_EDI )
#define C_stosw_parms  P_DXEDI_AX_CX
#define C_stosw_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq C_stosw = {
                7,
                push_es,
                mov_es_dx,
                rep,
                stosw,
                pop_es
        };


/* edi stosw( edi, ax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be even */
#define F_stosw_ret    HW_D( HW_EDI )
#define F_stosw_parms  P_DI_AX_CX
#define F_stosw_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq F_stosw = {
                3,
                rep,
                stosw,
        };

/* es:edi stosw( es:edi, ax, ecx ) zaps edi,ecx */
#define BD_stosw_ret   HW_D( HW_EDI )
#define BD_stosw_parms P_ESDI_AX_CX
#define BD_stosw_saves  HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq BD_stosw = {
                3,
                rep,
                stosw,
        };


/* edi stosd( es:edi, eax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be multiple of 4 */
#define C_stosd_ret    HW_D( HW_EDI )
#define C_stosd_parms  P_DXEDI_EAX_ECX
#define C_stosd_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq C_stosd = {
                6,
                push_es,
                mov_es_dx,
                rep,
                stosd,
                pop_es
        };


/* edi stosd( edi, eax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be multiple of 4 */
#define F_stosd_ret    HW_D( HW_EDI )
#define F_stosd_parms  P_EDI_EAX_ECX
#define F_stosd_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq F_stosd = {
                2,
                rep,
                stosd,
        };

/* es:edi stosd( es:edi, eax, ecx ) zaps edi,ecx */
#define BD_stosd_ret   HW_D( HW_EDI )
#define BD_stosd_parms P_ESEDI_EAX_ECX
#define BD_stosd_saves  HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq BD_stosd = {
                2,
                rep,
                stosd,
        };


/* edi stoswb( es:edi, ax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be odd */
#define C_stoswb_ret    HW_D( HW_EDI )
#define C_stoswb_parms  P_DXEDI_AX_CX
#define C_stoswb_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq C_stoswb = {
                8,
                push_es,
                mov_es_dx,
                rep,
                stosw,
                stosb,
                pop_es
        };


/* edi stoswb( edi, ax, ecx ) zaps edi,ecx */
/* used when value of ECX is known to be odd */
#define F_stoswb_ret    HW_D( HW_EDI )
#define F_stoswb_parms  P_DI_AX_CX
#define F_stoswb_saves   HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq F_stoswb = {
                4,
                rep,
                stosw,
                stosb,
        };

/* es:edi stoswb( es:edi, al, ecx ) zaps edi,ecx */
#define BD_stoswb_ret   HW_D( HW_EDI )
#define BD_stoswb_parms P_ESDI_AX_CX
#define BD_stoswb_saves  HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq BD_stoswb = {
                4,
                rep,
                stosw,
                stosb,
        };


/* di memcpy( di, si, cx ) zaps es,si,cx */
#define S_memcpy_ret    HW_D( HW_EDI )
#define S_memcpy_parms  P_DI_SI_CX
#define S_memcpy_saves   HW_NotD_2( HW_ESI, HW_ECX )
static byte_seq S_memcpy = {
                8,
                push_es,
                push_di,
                push_ds,
                pop_es,
                rep,
                movsb,
                pop_di,
                pop_es
        };
static byte_seq FS_memcpy = {
                4,
                push_di,
                rep,
                movsb,
                pop_di,
        };


/* di memcpy( di, si, cx ) zaps ax,es,si,cx */
#define C_memcpy_ret    HW_D( HW_EDI )
#define C_memcpy_parms  P_DI_SI_CX
#define C_memcpy_saves   HW_NotD_3( HW_EAX, HW_ESI, HW_ECX )
#define C_memcpy_bodysize 16
#define C_memcpy_body \
                push_di,\
                mov_ax_cx,\
                shr_cx_2,\
                rep,\
                movsd,\
                mov_cl_al,\
                and_cl,3,\
                rep,\
                movsb,\
                pop_di


static byte_seq C_memcpy = {
                C_memcpy_bodysize+6,
                push_es,
                mov_ax_ds,
                mov_es_ax,
                C_memcpy_body,
                pop_es
        };
static byte_seq FC_memcpy = {
                C_memcpy_bodysize,
                C_memcpy_body
        };


/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */
#define Z_memcpy_ret    HW_D_2( HW_ES, HW_EDI )
#define Z_memcpy_parms  P_ESDI_DSSI_CX
#define Z_memcpy_saves   HW_NotD_2( HW_ESI, HW_ECX )
static byte_seq Z_memcpy = {
                4,
                push_di,
                rep,
                movsb,
                pop_di
        };


/* es:di memcpy( es:di, ds:si, cx ) zaps cx,si */
#define BD_memcpy_ret   HW_D_2( HW_ES, HW_EDI )
#define BD_memcpy_parms P_ESDI_DSSI_CX
#define BD_memcpy_saves  C_memcpy_saves
static byte_seq BD_memcpy = {
                C_memcpy_bodysize,
                C_memcpy_body
        };

/* dx:eax memcpy( dx:edi, cx:esi, eax ) zaps ecx,esi,edi,eax */
#define DP_memcpy_ret   HW_D_2( HW_DX, HW_EAX )
#define DP_memcpy_parms P_DXEDI_CXESI_EAX
#define DP_memcpy_saves  HW_NotD_4( HW_ESI, HW_ECX, HW_EDI, HW_EAX )
static byte_seq DP_memcpy = {
                25,
                push_ds,
                push_es,
                push_di,
                xchg_cx_ax,
                mov_ds_ax,
                mov_es_dx,
                mov_ax_cx,
                shr_cx_2,
                rep,
                movsd,
                mov_cl_al,
                and_cl,3,
                rep,
                movsb,
                pop_ax,
                pop_es,
                pop_ds
        };

/****************************************************************/

/* ax memcmp( si, di, cx ) zaps ax,es,si,di,cx */
#define C_memcmp_ret    HW_D( HW_EAX )
#define C_memcmp_parms  P_SI_DI_CX
#define C_memcmp_saves   HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )
#define C_memcmp_bodysize 11
#define C_memcmp_body \
                xor_rr, ax_ax,\
                repz,\
                cmpsb,\
                je,  5,\
                sbb_rr, ax_ax,\
                sbb_ax_ffff

static byte_seq C_memcmp = {
                C_memcmp_bodysize+6,
                push_es,
                mov_ax_ds,
                mov_es_ax,
                C_memcmp_body,
                pop_es
        };
static byte_seq FC_memcmp = {
                C_memcmp_bodysize,
                C_memcmp_body
        };


/* cx memcmp( ds:si, es:di, cx ) zaps si,di,cx */
#define BD_memcmp_ret   HW_D( HW_ECX )
#define BD_memcmp_parms P_DSSI_ESDI_CX
#define BD_memcmp_saves  HW_NotD_3( HW_ESI, HW_EDI, HW_ECX )
static byte_seq BD_memcmp = {
                11,
                or_cx_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff     /* 3 bytes */
        };

/* ecx memcmp( dx:esi, cx:edi, eax ) zaps esi,edi,ecx,eax */
#define DP_memcmp_ret   HW_D( HW_ECX )
#define DP_memcmp_parms P_DXESI_CXEDI_EAX
#define DP_memcmp_saves  HW_NotD_4( HW_ESI, HW_EDI, HW_ECX, HW_EAX )
static byte_seq DP_memcmp = {
                20,
                push_ds,
                push_es,
                xchg_cx_ax,
                mov_ds_dx,
                mov_es_ax,
                or_cx_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff,     /* 3 bytes */
                pop_es,
                pop_ds
        };

/****************************************************************/


/* di memchr( di, al, cx ) zaps dx,es,di,cx */
#define C_memchr_ret    HW_D( HW_EDI )
#define C_memchr_parms  P_DI_AL_CX
#define C_memchr_saves   HW_NotD_3( HW_EDX, HW_EDI, HW_ECX )
static byte_seq C_memchr = {
                17,
                push_es,
                jcxz, 11,
                mov_dx_ds,
                mov_es_dx,
                repnz,
                scasb,
                jne, 3,
                dec_di,
                hide2,
                mov_di_cx,
                pop_es
        };
static byte_seq FC_memchr = {
                11,
                jcxz, 7,
                repnz,
                scasb,
                jne, 3,
                dec_di,
                hide2,
                mov_di_cx,
        };


/* cx:di memchr( es:di, al, cx ) zaps di,cx */
#define BD_memchr_ret   HW_D_2( HW_EDI, HW_CX )
#define BD_memchr_parms P_ESDI_AL_CX
#define BD_memchr_saves  HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq BD_memchr = {
                13,
                jcxz, 9,
                repnz,
                scasb,
                jne, 5,
                dec_di,
                mov_cx_es,
                hide2,
                mov_di_cx
        };

/* cx:edi memchr( dx:edi, al, ecx ) zaps edi,ecx */
#define DP_memchr_ret   HW_D_2( HW_CX, HW_EDI )
#define DP_memchr_parms P_DXEDI_AL_ECX
#define DP_memchr_saves  HW_NotD_2( HW_EDI, HW_ECX )
static byte_seq DP_memchr = {
                17,
                push_es,
                jcxz, 11,               /* 27-jul-90, was 10 */
                mov_es_dx,
                repnz,
                scasb,
                jne, 5,                 /* 27-jul-90, was 4 */
                dec_di,
                mov_cx_es,
                hide2,
                mov_di_cx,
                pop_es
        };

/****************************************************************/


/* ax strcmp( si, di ) zaps ax,si,di */
#define S_strcmp_ret    HW_D( HW_EAX )
#define S_strcmp_parms  P_SI_DI
#define S_strcmp_saves   HW_NotD_3( HW_EAX, HW_ESI, HW_EDI )
static byte_seq S_strcmp = {
                16,
                lodsb,
                mov_ah_atdi,
                sub_al_ah,
                jne, 5,
                inc_di,
                cmp_al_ah,
                jne, -12,
                size_prefix,
                sbb_ah_ah,
                cwde
        };

/* ax strcmp( si, di ) zaps ax,es,si,di,cx */
#define C_strcmp_ret    HW_D( HW_EAX )
#define C_strcmp_parms  P_SI_DI
#define C_strcmp_saves   HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )
static byte_seq C_strcmp = {
                31,
                push_es,
                mov_ax_ds,
                mov_es_ax,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx_ffff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                sbb_ax_ffff,
                pop_es
        };


/* ax strcmp( si, di ) zaps ax,si,di,cx */
#define F_strcmp_ret    HW_D( HW_EAX )
#define F_strcmp_parms  P_SI_DI
#define F_strcmp_saves   HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )
static byte_seq F_strcmp = {
                25,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx_ffff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                sbb_ax_ffff
        };


/* ax strcmp( ds:si, es:di ) zaps ax,si,di */
#define Z_strcmp_ret    HW_D( HW_EAX )
#define Z_strcmp_parms  P_DSSI_ESDI
#define Z_strcmp_saves   HW_NotD_3( HW_EAX, HW_ESI, HW_EDI )
static byte_seq Z_strcmp = {
                17,
                lodsb,
                mov_ah_atesdi,
                sub_al_ah,
                jne, 5,
                inc_di,
                cmp_al_ah,
                jne, -13,
                size_prefix,
                sbb_ah_ah,
                cwde
        };


/* cx strcmp( ds:si, es:di ) zaps ax,si,di,cx */
#define BD_strcmp_ret   HW_D( HW_ECX )
#define BD_strcmp_parms P_DSSI_ESDI
#define BD_strcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )
static byte_seq BD_strcmp = {
                25,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx_ffff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff     /* 3 bytes */
        };

/* ecx strcmp( si:eax, cx:edi ) zaps eax,esi,edi,ecx */
#define DP_strcmp_ret   HW_D( HW_ECX )
#define DP_strcmp_parms P_SIEAX_CXEDI
#define DP_strcmp_saves  HW_NotD_4( HW_EAX, HW_ESI, HW_EDI, HW_ECX )
static byte_seq DP_strcmp = {
                34,
                push_ds,
                push_es,
                xchg_si_ax,
                mov_ds_ax,
                mov_es_cx,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                xor_rr, ax_ax,
                mov_cx_ax,
                dec_cx,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff,     /* 3 bytes */
                pop_es,
                pop_ds
        };

/****************************************************************/


/* cx strlen( di ) zaps ax,es,cx,di */
#define C_strlen_ret    HW_D( HW_ECX )
#define C_strlen_parms  P_DI
#define C_strlen_saves   HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )
#define C_strlen_bodysize 10
#define C_strlen_body \
                mov_cx_ffff,\
                xor_rr, ax_ax,\
                repnz,\
                scasb,\
                not_cx,\
                dec_cx

static byte_seq C_strlen = {
                C_strlen_bodysize+6,
                push_es,
                mov_ax_ds,
                mov_es_ax,
                C_strlen_body,
                pop_es
        };
static byte_seq FC_strlen = {
                C_strlen_bodysize,
                C_strlen_body
        };


/* cx strlen( es:di ) zaps ax,cx,di */
#define BD_strlen_ret   HW_D( HW_ECX )
#define BD_strlen_parms P_ESDI
#define BD_strlen_saves  HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )
static byte_seq BD_strlen = {
                10,
                mov_cx_ffff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                dec_cx
        };

/* ecx strlen( cx:edi ) zaps eax,ecx,edi */
#define DP_strlen_ret   HW_D( HW_ECX )
#define DP_strlen_parms P_CXEDI
#define DP_strlen_saves  HW_NotD_3( HW_EAX, HW_ECX, HW_EDI )
static byte_seq DP_strlen = {
                14,
                push_es,
                mov_es_cx,
                xor_rr, ax_ax,
                mov_cx_ax,
                dec_cx,
                repnz,
                scasb,
                not_cx,
                dec_cx,
                pop_es
        };

/****************************************************************/

#define C_abs_ret    HW_D( HW_EAX )
#define C_abs_parms  P_AX
#define C_abs_saves   HW_NotD_2( HW_EAX, HW_EDX )
static byte_seq C_abs = {
                5,
                cwd,
                xor_rr, ax_dx,
                sub_rr, ax_dx
        };


#define C_labs_ret    HW_D( HW_EAX )
#define C_labs_parms  P_AX
#define C_labs_saves   HW_NotD_2( HW_EAX, HW_EDX )
static byte_seq C_labs = {
                5,
                cwd,
                xor_rr, ax_dx,
                sub_rr, ax_dx
        };

#define C_inp_ret    HW_D( HW_EAX )
#define C_inp_parms  P_DX
#define C_inp_saves   HW_NotD( HW_EAX )
static byte_seq C_inp = {
                3,
                sub_rr,ax_ax,
                in_al_dx
        };


#define C_inpw_ret    HW_D( HW_EAX )
#define C_inpw_parms  P_DX
#define C_inpw_saves   HW_NotD( HW_EAX )
static byte_seq C_inpw = {
                4,
                sub_rr,ax_ax,
                size_prefix,
                in_ax_dx
        };


#define C_inpd_ret    HW_D( HW_EAX )
#define C_inpd_parms  P_DX
#define C_inpd_saves   HW_NotD( HW_EAX )
static byte_seq C_inpd = {
                1,
                in_ax_dx
        };


#define C_outp_ret    HW_D( HW_EAX )
#define C_outp_parms  P_DX_AL
#define C_outp_saves   HW_NotD( HW_EMPTY )
static byte_seq C_outp = {
                1,
                out_dx_al
        };


#define C_outpw_ret    HW_D( HW_EAX )
#define C_outpw_parms  P_DX_AX
#define C_outpw_saves   HW_NotD( HW_EMPTY )
static byte_seq C_outpw = {
                2,
                size_prefix,
                out_dx_ax
        };


#define C_outpd_ret    HW_D( HW_EAX )
#define C_outpd_parms  P_DX_AX
#define C_outpd_saves   HW_NotD( HW_EMPTY )
static byte_seq C_outpd = {
                1,
                out_dx_ax
        };


#define C_movedata_ret    HW_D( HW_EMPTY )
#define C_movedata_parms  P_AX_SI_DX_DI_CX
#define C_movedata_saves   HW_NotD_4( HW_EAX, HW_ECX, HW_ESI, HW_EDI )
static byte_seq C_movedata = {
                22,
                push_ds,
                push_es,
                mov_ds_ax,
                mov_es_dx,
                mov_ax_cx,
                shr_cx_2,
                rep,
                movsd,
                mov_cl_al,
                and_cl,3,
                rep,
                movsb,
                pop_es,
                pop_ds
        };

#define C_enable_ret    HW_D( HW_EMPTY )
#define C_enable_parms  P_AX
#define C_enable_saves   HW_NotD( HW_EMPTY )
static byte_seq C_enable = {
                2,
                sti,
                cld
        };

#define C_disable_ret    HW_D( HW_EMPTY )
#define C_disable_parms  P_AX
#define C_disable_saves   HW_NotD( HW_EMPTY )
static byte_seq C_disable = {
                1,
                cli
        };

#define C_rotl_ret    HW_D( HW_EAX )
#define C_rotl_parms  P_AX_CX
#define C_rotl_saves   HW_NotD( HW_EAX )
static byte_seq C_rotl = {
                2,
                0xd3,0xc0      /* rol ax,cl */
        };

#define C_rotr_ret    HW_D( HW_EAX )
#define C_rotr_parms  P_AX_CX
#define C_rotr_saves   HW_NotD( HW_EAX )
static byte_seq C_rotr = {
                2,
                0xd3,0xc8      /* ror ax,cl */
        };

#define C_8087_fabs_ret    HW_D( HW_FLTS )
#define C_8087_fabs_parms  P_8087
#define C_8087_fabs_saves   HW_NotD( HW_EMPTY )
static byte_seq _8087_fabs = {
                2,
                fabs
        };

#define C_fabs_ret    HW_D_2( HW_EDX, HW_EAX )
#define C_fabs_parms  P_EDXEAX
#define C_fabs_saves   HW_NotD( HW_EDX )
static byte_seq C_fabs = {
                4,
                shl_edx_1,
                shr_edx_1
        };


#define C_min_ret       HW_D( HW_EAX )
#define C_min_parms     P_AX_DX
#define C_min_saves      HW_NotD_2( HW_ECX, HW_EAX )
static byte_seq C_min = {
                8,
                sub_ax_dx,
                sbb_cx_cx,
                and_ax_cx,
                add_ax_dx
        };

#define C_max_ret       HW_D( HW_EAX )
#define C_max_parms     P_AX_DX
#define C_max_saves      HW_NotD_2( HW_ECX, HW_EAX )
static byte_seq C_max = {
                9,
                sub_ax_dx,
                cmc,
                sbb_cx_cx,
                and_ax_cx,
                add_ax_dx
        };



static hw_reg_set P_AX_CX[]        = {
    HW_D( HW_EAX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY ) };

static hw_reg_set P_DI_SI[]        = {
    HW_D( HW_EDI ),
    HW_D( HW_ESI ),
    HW_D( HW_EMPTY ) };

static hw_reg_set P_DI_SI_CX[]     = {
    HW_D( HW_EDI ),
    HW_D( HW_ESI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_SI_DI_CX[]     = {
    HW_D( HW_ESI ),
    HW_D( HW_EDI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DI_AX_CX[]     = {
    HW_D( HW_EDI ),
    HW_D( HW_AX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_EDI_EAX_ECX[]     = {
    HW_D( HW_EDI ),
    HW_D( HW_EAX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DI_AL_CX[]     = {
    HW_D( HW_EDI ),
    HW_D( HW_AL ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESDI_DSSI[]    = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D_2( HW_DS, HW_ESI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXEDI_CXESI_EAX[]= {
    HW_D_2( HW_DX, HW_EDI ),
    HW_D_2( HW_CX, HW_ESI ),
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXESI_CXEDI_EAX[]= {
    HW_D_2( HW_DX, HW_ESI ),
    HW_D_2( HW_CX, HW_EDI ),
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXEAX_CXEBX[]  = {
    HW_D_2( HW_DX, HW_EAX ),
    HW_D_2( HW_CX, HW_EBX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESDI_DSSI_CX[] = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D_2( HW_DS, HW_ESI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESDI_AL_CX[]   = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_AL ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXEDI_AL_ECX[] = {
    HW_D_2( HW_DX, HW_EDI ),
    HW_D( HW_AL ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESDI_AX_CX[]   = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_AX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESEDI_EAX_ECX[]   = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_EAX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXEDI_AX_CX[] = {
    HW_D_2( HW_DX, HW_EDI ),
    HW_D( HW_AX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DXEDI_EAX_ECX[] = {
    HW_D_2( HW_DX, HW_EDI ),
    HW_D( HW_EAX ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_SI_DI[]        = {
    HW_D( HW_ESI ),
    HW_D( HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DSSI_ESDI[]    = {
    HW_D_2( HW_DS, HW_ESI ),
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_SIEAX_CXEDI[]  = {
    HW_D_2( HW_SI, HW_EAX ),
    HW_D_2( HW_CX, HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DSSI_ESDI_CX[] = {
    HW_D_2( HW_DS, HW_ESI ),
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DSSI_CL[]      = {
    HW_D_2( HW_DS, HW_ESI ),
    HW_D( HW_CL ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_SI_DL[]        = {
    HW_D( HW_ESI ),
    HW_D( HW_DL ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DI[]           = {
    HW_D( HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_ESDI[]         = {
    HW_D_2( HW_ES, HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_CXEDI[]        = {
    HW_D_2( HW_CX, HW_EDI ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_AX[]           = {
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DX[]           = {
    HW_D( HW_EDX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DX_AL[]        = {
    HW_D( HW_EDX ),
    HW_D( HW_AL ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_DX_AX[]        = {
    HW_D( HW_EDX ),
    HW_D( HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_AX_DX[]        = {
    HW_D( HW_EAX ),
    HW_D( HW_EDX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_AX_SI_DX_DI_CX[]= {
    HW_D( HW_EAX ),
    HW_D( HW_ESI ),
    HW_D( HW_EDX ),
    HW_D( HW_EDI ),
    HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_EDXEAX[]       = {
    HW_D_2( HW_EDX, HW_EAX ),
    HW_D( HW_EMPTY )
};

static hw_reg_set P_8087[]         = {
    HW_D( HW_FLTS ),
    HW_D( HW_EMPTY )
};


byte_seq *FlatAlternates[] = {
        &S_memset,   &FS_memset,
        &C_memset,   &FC_memset,
        &S_memcpy,   &FS_memcpy,
        &C_memcpy,   &FC_memcpy,
        &C_memcmp,   &FC_memcmp,
        &C_memchr,   &FC_memchr,
        &C_strlen,   &FC_strlen,
        &C_strcat,   &FC_strcat,
        NULL,       NULL
};

/* these must also work in FLAT model */
struct  inline_funcs SInline_Functions[] = {
    { "strcpy", &S_strcpy, S_strcpy_parms, S_strcpy_ret, S_strcpy_saves },
    { "strcmp", &S_strcmp, S_strcmp_parms, S_strcmp_ret, S_strcmp_saves },
    { "strcat", &S_strcat, S_strcat_parms, S_strcat_ret, S_strcat_saves },
    { "strchr", &S_strchr, S_strchr_parms, S_strchr_ret, S_strchr_saves },
    { "memset", &S_memset, S_memset_parms, S_memset_ret, S_memset_saves },
    { "memcpy", &S_memcpy, S_memcpy_parms, S_memcpy_ret, S_memcpy_saves },
    { NULL }
};

/* these must also work in FLAT model */
struct  inline_funcs Inline_Functions[] = {
    { "strlen", &C_strlen, C_strlen_parms, C_strlen_ret, C_strlen_saves },
    { "strcpy", &C_strcpy, C_strcpy_parms, C_strcpy_ret, C_strcpy_saves },
    { "strcmp", &C_strcmp, C_strcmp_parms, C_strcmp_ret, C_strcmp_saves },
    { "strcat", &C_strcat, C_strcat_parms, C_strcat_ret, C_strcat_saves },
    { "strchr", &C_strchr, C_strchr_parms, C_strchr_ret, C_strchr_saves },
    { "memcpy", &C_memcpy, C_memcpy_parms, C_memcpy_ret, C_memcpy_saves },
    { "memcmp", &C_memcmp, C_memcmp_parms, C_memcmp_ret, C_memcmp_saves },
    { "memset", &C_memset, C_memset_parms, C_memset_ret, C_memset_saves },
    { "memchr", &C_memchr, C_memchr_parms, C_memchr_ret, C_memchr_saves },
    { ".stosd", &C_stosd,  C_stosd_parms,  C_stosd_ret,  C_stosd_saves  },
    { ".stosw", &C_stosw,  C_stosw_parms,  C_stosw_ret,  C_stosw_saves  },
    { ".stoswb",&C_stoswb, C_stoswb_parms, C_stoswb_ret, C_stoswb_saves },
    { NULL }
};

struct  inline_funcs SBigData_Functions[] = {
    { "strcpy", &Z_strcpy, Z_strcpy_parms, Z_strcpy_ret, Z_strcpy_saves },
    { "strcmp", &Z_strcmp, Z_strcmp_parms, Z_strcmp_ret, Z_strcmp_saves },
    { "strcat", &Z_strcat, Z_strcat_parms, Z_strcat_ret, Z_strcat_saves },
    { "memset", &Z_memset, Z_memset_parms, Z_memset_ret, Z_memset_saves },
    { "memcpy", &Z_memcpy, Z_memcpy_parms, Z_memcpy_ret, Z_memcpy_saves },
    { NULL }
};

struct  inline_funcs Flat_Functions[] = {
    { "strcmp", &F_strcmp, F_strcmp_parms, F_strcmp_ret, F_strcmp_saves },
    { ".stosd", &F_stosd,  F_stosd_parms,  F_stosd_ret,  F_stosd_saves  },
    { ".stosw", &F_stosw,  F_stosw_parms,  F_stosw_ret,  F_stosw_saves  },
    { ".stoswb",&F_stoswb, F_stoswb_parms, F_stoswb_ret, F_stoswb_saves },
    { NULL }
};

struct  inline_funcs BigData_Functions[] = {
    { "strlen", &BD_strlen, BD_strlen_parms, BD_strlen_ret, BD_strlen_saves },
    { "strcpy", &BD_strcpy, BD_strcpy_parms, BD_strcpy_ret, BD_strcpy_saves },
    { "strcmp", &BD_strcmp, BD_strcmp_parms, BD_strcmp_ret, BD_strcmp_saves },
    { "strcat", &BD_strcat, BD_strcat_parms, BD_strcat_ret, BD_strcat_saves },
    { "strchr", &BD_strchr, BD_strchr_parms, BD_strchr_ret, BD_strchr_saves },
    { "memcpy", &BD_memcpy, BD_memcpy_parms, BD_memcpy_ret, BD_memcpy_saves },
    { "memcmp", &BD_memcmp, BD_memcmp_parms, BD_memcmp_ret, BD_memcmp_saves },
    { "memset", &BD_memset, BD_memset_parms, BD_memset_ret, BD_memset_saves },
    { "memchr", &BD_memchr, BD_memchr_parms, BD_memchr_ret, BD_memchr_saves },
    { ".stosd", &BD_stosd,  BD_stosd_parms,  BD_stosd_ret,  BD_stosd_saves  },
    { ".stosw", &BD_stosw,  BD_stosw_parms,  BD_stosw_ret,  BD_stosw_saves  },
    { ".stoswb",&BD_stoswb, BD_stoswb_parms, BD_stoswb_ret, BD_stoswb_saves },
    { NULL }
 };

/* these must also work in FLAT model */
struct  inline_funcs _8087_Functions[] = {
 { "fabs",  &_8087_fabs, C_8087_fabs_parms, C_8087_fabs_ret, C_8087_fabs_saves  },
 { NULL }
 };


/* memory model independent functions */

/* these must also work in FLAT model except _fmem & _fstr functions */
struct  inline_funcs Common_Functions[] = {
  { "abs",     &C_abs,    C_abs_parms,     C_abs_ret,     C_abs_saves      },
  { "labs",    &C_labs,   C_labs_parms,    C_labs_ret,    C_labs_saves     },
  { "fabs",    &C_fabs,   C_fabs_parms,    C_fabs_ret,    C_fabs_saves     },
  { "inp",     &C_inp,    C_inp_parms,     C_inp_ret,     C_inp_saves      },
  { "inpw",    &C_inpw,   C_inpw_parms,    C_inpw_ret,    C_inpw_saves     },
  { "inpd",    &C_inpd,   C_inpd_parms,    C_inpd_ret,    C_inpd_saves     },
  { "outp",    &C_outp,   C_outp_parms,    C_outp_ret,    C_outp_saves     },
  { "outpw",   &C_outpw,  C_outpw_parms,   C_outpw_ret,   C_outpw_saves    },
  { "outpd",   &C_outpd,  C_outpd_parms,   C_outpd_ret,   C_outpd_saves    },
  { "movedata",&C_movedata,C_movedata_parms,C_movedata_ret,C_movedata_saves },
  { "_enable", &C_enable, C_enable_parms,  C_enable_ret,  C_enable_saves   },
  { "_disable",&C_disable,C_disable_parms, C_disable_ret, C_disable_saves  },
  { "_rotl",   &C_rotl,   C_rotl_parms,    C_rotl_ret,    C_rotl_saves     },
  { "_rotr",   &C_rotr,   C_rotr_parms,    C_rotr_ret,    C_rotr_saves     },
  { "_lrotl",  &C_rotl,   C_rotl_parms,    C_rotl_ret,    C_rotl_saves     },
  { "_lrotr",  &C_rotr,   C_rotr_parms,    C_rotr_ret,    C_rotr_saves     },
  { "div",     &C_div,    C_div_parms,     C_div_ret,     C_div_saves      },
  { "ldiv",    &C_div,    C_div_parms,     C_div_ret,     C_div_saves      },
  { "_fmemcpy",&DP_memcpy,DP_memcpy_parms, DP_memcpy_ret, DP_memcpy_saves  },
  { "_fmemcmp",&DP_memcmp,DP_memcmp_parms, DP_memcmp_ret, DP_memcmp_saves  },
  { "_fmemset",&DP_memset,DP_memset_parms, DP_memset_ret, DP_memset_saves  },
  { "_fmemchr",&DP_memchr,DP_memchr_parms, DP_memchr_ret, DP_memchr_saves  },
  { "_fstrcpy",&DP_strcpy,DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves  },
  { "_fstrcat",&DP_strcat,DP_strcat_parms, DP_strcat_ret, DP_strcat_saves  },
  { "_fstrcmp",&DP_strcmp,DP_strcmp_parms, DP_strcmp_ret, DP_strcmp_saves  },
  { "_fstrlen",&DP_strlen,DP_strlen_parms, DP_strlen_ret, DP_strlen_saves  },
  { ".min",    &C_min,    C_min_parms,     C_min_ret,     C_min_saves      },
  { ".max",    &C_max,    C_max_parms,     C_max_ret,     C_max_saves      },
  { NULL }
 };

#endif
