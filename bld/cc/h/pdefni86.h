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

hw_reg_set DefaultVarParms[] = {
        0 };


hw_reg_set DefaultParms[] = {
#ifndef WCPP
        HW_D_4( HW_AX, HW_BX, HW_CX, HW_DX ) /*+HW_ST1+HW_ST2+HW_ST3+HW_ST4*/,
#endif
        0 };

#if _CPU == 8086 || _CPU == 386
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
                'f','s',NC,
                'g','s',NC,
#if _CPU == 386
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

        hw_reg_set RegBits[] ={
                                HW_D( HW_ES ),
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
                                HW_D( HW_FS ),
                                HW_D( HW_GS ),
#if _CPU == 386
                                HW_D( HW_EAX ),
                                HW_D( HW_EBX ),
                                HW_D( HW_ECX ),
                                HW_D( HW_EDX ),
                                HW_D( HW_ESI ),
                                HW_D( HW_EDI ),
                                HW_D( HW_EBP ),
                                HW_ESP,
#endif
 };

/*      these are the registers that Microsoft saves and restores */

hw_reg_set MSC_Save = { HW_D_3( HW_SI, HW_DI, HW_BP ) };

/*      INLINE FUNCTIONS */

struct  aux_info        InlineInfo;

enum    asm_codes {
        push_es = 0x06,
        pop_es  = 0x07,
        sbb_rr  = 0x19,
        sbb_ax_i= 0x1d,
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
        je      = 0x74,
        jne     = 0x75,
        jge     = 0x7d,
        xchg_cx_ax = 0x91,
        xchg_si_ax = 0x96,
        cbw     = 0x98,
        cwd     = 0x99,
        fwait   = 0x9b,
        movsb   = 0xa4,
        movsw   = 0xa5,
        cmpsb   = 0xa6,
        stosb   = 0xaa,
        stosw   = 0xab,
        lodsb   = 0xac,
        lodsw   = 0xad,
        scasb   = 0xae,
        mov_cx  = 0xb9,
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
        hide2   = 0xa9,
        cmc     = 0xf5,
};
#define sbb_cx_ffff     0x83,0xd9,0xff
#define mov_dx_ax       0x89,0xc2
#define mov_ax_dx       0x89,0xd0
#define mov_ax_bx       0x89,0xd8
#define mov_dx_si       0x89,0xf2
#define mov_ax_di       0x89,0xf8
#define mov_ax_es       0x8c,0xc0
#define mov_cx_es       0x8c,0xc1
#define mov_dx_es       0x8c,0xc2
#define mov_ax_ds       0x8c,0xd8
#define mov_dx_ds       0x8c,0xda
#define mov_ds_ax       0x8e,0xd8
#define mov_es_ax       0x8e,0xc0
#define mov_es_dx       0x8e,0xc2
#define mov_ds_dx       0x8e,0xda
#define mov_ds_si       0x8e,0xde
#define mov_ah_atdi     0x8a,0x25
#define mov_ah_atesdi   0x26,0x8a,0x25
#define not_cx          0xf7,0xd1
#define or_cx_cx        0x09,0xc9
#define or_dx_dx        0x0b,0xd2
#define neg_ax          0xf7,0xd8
#define adc_dx_0        0x83,0xd2,0x00
#define neg_dx          0xf7,0xda
#define sub_al_ah       0x28,0xe0
#define sub_ah_ah       0x28,0xe4
#define shr_cx_1        0xd1,0xe9
#define adc_cx_cx       0x11,0xc9
#define xchg_si_di      0x87,0xfe
#define mov_ax_0        0xb8,0x00,0x00
#define mov_cx_0        0xb9,0x00,0x00
#define test_al_al      0x84,0xc0
#define test_ah_ah      0x84,0xe4
#define test_al_aa      0xa8,0xaa
#define fabs            0xd9,0xe1
#define and_ah_7f       0x80,0xe4,0x7f
#define mov_ah_al       0x88,0xc4
#define mov_di_cx       0x89,0xcf
#define cmp_al_dl       0x38,0xd0
#define cmp_ah_dl       0x38,0xd4
#define cmp_al_cl       0x38,0xc8
#define cmp_ah_cl       0x38,0xcc
#define cmp_al_ah       0x38,0xe0
#define mov_dx_si       0x89,0xf2
#define cmp_atdi_0      0x80,0x3d,0x00
#define cmp_atesdi_0    0x26,0x80,0x3d,0x00
#define idiv_cx         0xf7,0xf9
#define sub_ax_dx       0x29,0xd0
#define sbb_cx_cx       0x19,0xc9
#define and_ax_cx       0x21,0xc8
#define add_ax_dx       0x01,0xd0
#define sbb_ah_ah       0x18,0xe4

/*
  here come the code bursts ...
*/
/****************************************************************/
#include "codei86.gh"

/* dx:ax  div( ax, cx ) */
#define C_div_ret       HW_D_2( HW_AX, HW_DX )
#define C_div_parms     P_AX_CX
#define C_div_saves      HW_NotD_2( HW_AX, HW_DX )
static byte_seq C_div = {
                3,
                cwd,
                idiv_cx
        };

/****************************************************************/

/* di stosw( di, si, cx ) zaps ax,es,si,cx */
/* used when value of CX is known to be even */

/* es:di stosw( es:di, ds:si, cx ) zaps si,cx */
#define DF_stosw_ret   HW_D( HW_DI )
#define DF_stosw_parms P_ESDI_AX_CX
#define DF_stosw_saves  HW_NotD_2( HW_DI, HW_CX )
static byte_seq DF_stosw = {
                2,
                rep,
                stosw,
        };


#define DP_stosw_ret   DF_stosw_ret
#define DP_stosw_parms DF_stosw_parms
#define DP_stosw_saves  DF_stosw_saves
#define DP_stosw       DF_stosw

#define C_stosw_ret   DF_stosw_ret
#define C_stosw_parms DF_stosw_parms
#define C_stosw_saves  DF_stosw_saves
#define C_stosw       DF_stosw
/****************************************************************/

/* di stoswb( di, si, cx ) zaps ax,es,si,cx */
/* used when value of CX is known to be even */

/* es:di stoswb( es:di, ds:si, cx ) zaps si,cx */
#define DF_stoswb_ret   HW_D( HW_DI )
#define DF_stoswb_parms P_ESDI_AX_CX
#define DF_stoswb_saves  HW_NotD_2( HW_DI, HW_CX )
static byte_seq DF_stoswb = {
                3,
                rep,
                stosw,
                stosb,
        };


#define C_stoswb_ret   DF_stoswb_ret
#define C_stoswb_parms DF_stoswb_parms
#define C_stoswb_saves  DF_stoswb_saves
#define C_stoswb       DF_stoswb

#define DP_stoswb_ret   DF_stoswb_ret
#define DP_stoswb_parms DF_stoswb_parms
#define DP_stoswb_saves  DF_stoswb_saves
#define DP_stoswb       DF_stoswb
/****************************************************************/

/* ax memcmp( si, di, cx ) zaps ax,es,si,di,cx */
#define C_memcmp_ret    HW_D( HW_AX )
#define C_memcmp_parms  P_SI_DI_CX
#define C_memcmp_saves   HW_NotD_5( HW_AX, HW_ES, HW_SI, HW_DI, HW_CX )
static byte_seq C_memcmp = {
                15,
                mov_ax_ds,
                mov_es_ax,
                xor_rr, ax_ax,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                sbb_ax_i, 0xff, 0xff
        };


/* cx memcmp( ds:si, es:di, cx ) zaps si,di,cx */
#define DF_memcmp_ret   HW_D( HW_CX )
#define DF_memcmp_parms P_DSSI_ESDI_CX
#define DF_memcmp_saves  HW_NotD_3( HW_SI, HW_DI, HW_CX )
static byte_seq DF_memcmp = {
                11,
                or_cx_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff     /* 3 bytes */
        };


/* cx memcmp( cx:si, es:di, ax ) zaps si,di,cx,ax */
#define DP_memcmp_ret   HW_D( HW_CX )
#define DP_memcmp_parms P_CXSI_ESDI_AX
#define DP_memcmp_saves  HW_NotD_4( HW_SI, HW_DI, HW_CX, HW_AX )
static byte_seq DP_memcmp = {
                16,
                push_ds,
                xchg_cx_ax,
                mov_ds_ax,
                or_cx_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff,     /* 3 bytes */
                pop_ds
        };

/****************************************************************/

/* di memchr( di, al, cx ) zaps dx,es,di,cx */
#define C_memchr_ret    HW_D( HW_DI )
#define C_memchr_parms  P_DI_AL_CX
#define C_memchr_saves   HW_NotD_4( HW_DX, HW_ES, HW_DI, HW_CX )
static byte_seq C_memchr = {
                14,
                jcxz, 10,
                mov_dx_ds,
                mov_es_dx,
                repnz,
                scasb,
                jne, 2,
                dec_di,
                hide2,
                mov_di_cx
        };


/* cx:di memchr( es:di, al, cx ) zaps di,cx */
#define DF_memchr_ret   HW_D_2( HW_DI, HW_CX )
#define DF_memchr_parms P_ESDI_AL_CX
#define DF_memchr_saves  HW_NotD_2( HW_DI, HW_CX )
static byte_seq DF_memchr = {
                12,
                jcxz, 8,
                repnz,
                scasb,
                jne, 4,
                dec_di,
                mov_cx_es,
                hide2,
                mov_di_cx
        };


/* cx:di memchr( es:di, al, cx ) zaps di,cx */
#define DP_memchr_ret   HW_D_2( HW_DI, HW_CX )
#define DP_memchr_parms P_ESDI_AL_CX
#define DP_memchr_saves  HW_NotD_2( HW_DI, HW_CX )
static byte_seq DP_memchr = {
                12,
                jcxz, 8,
                repnz,
                scasb,
                jne, 4,
                dec_di,
                mov_cx_es,
                hide2,
                mov_di_cx
        };

/****************************************************************/

/* ax strcmp( si, di ) zaps ax,si,di */
#define S_strcmp_ret    HW_D( HW_AX )
#define S_strcmp_parms  P_SI_DI
#define S_strcmp_saves   HW_NotD_3( HW_AX, HW_SI, HW_DI )
static byte_seq S_strcmp = {
                14,
                lodsb,
                mov_ah_atdi,
                sub_al_ah,
                jne, 5,
                inc_di,
                cmp_al_ah,
                jne, -12,
                sbb_ah_ah
        };

/* ax strcmp( si, di ) zaps ax,es,si,di,cx */
#define C_strcmp_ret    HW_D( HW_AX )
#define C_strcmp_parms  P_SI_DI
#define C_strcmp_saves   HW_NotD_5( HW_AX, HW_ES, HW_SI, HW_DI, HW_CX )
static byte_seq C_strcmp = {
                29,
                mov_ax_ds,
                mov_es_ax,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx, 0xff, 0xff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, ax_ax,
                sbb_ax_i, 0xff, 0xff
        };

/* ax strcmp( ds:si, es:di ) zaps ax,si,di */
#define ZF_strcmp_ret    HW_D( HW_AX )
#define ZF_strcmp_parms  P_DSSI_ESDI
#define ZF_strcmp_saves   HW_NotD_3( HW_AX, HW_SI, HW_DI )
static byte_seq ZF_strcmp = {
                15,
                lodsb,
                mov_ah_atesdi,
                sub_al_ah,
                jne, 5,
                inc_di,
                cmp_al_ah,
                jne, -13,
                sbb_ah_ah
        };

/* ax strcmp( si:ax, es:di ) zaps ax,si,di */
#define ZP_strcmp_ret    HW_D( HW_AX )
#define ZP_strcmp_parms  P_SIAX_ESDI
#define ZP_strcmp_saves   HW_NotD_3( HW_AX, HW_SI, HW_DI )
static byte_seq ZP_strcmp = {
                20,
                push_ds,
                xchg_si_ax,
                mov_ds_ax,
                lodsb,
                mov_ah_atesdi,
                sub_al_ah,
                jne, 5,
                inc_di,
                cmp_al_ah,
                jne, -13,
                sbb_ah_ah,
                pop_ds
        };


/* cx strcmp( ds:si, es:di ) zaps ax,si,di,cx */
#define DF_strcmp_ret   HW_D( HW_CX )
#define DF_strcmp_parms P_DSSI_ESDI
#define DF_strcmp_saves  HW_NotD_4( HW_AX, HW_SI, HW_DI, HW_CX )
static byte_seq DF_strcmp = {
                25,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx, 0xff, 0xff,
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


/* cx strcmp( si:ax, es:di ) zaps ax,si,di,cx */
#define DP_strcmp_ret   HW_D( HW_CX )
#define DP_strcmp_parms P_SIAX_ESDI
#define DP_strcmp_saves  HW_NotD_4( HW_AX, HW_SI, HW_DI, HW_CX )
static byte_seq DP_strcmp = {
                30,
                push_ds,
                xchg_si_ax,
                mov_ds_ax,
                cmpsb,
                jne, 17,
                dec_di,
                dec_si,
                mov_cx, 0xff, 0xff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                sub_rr, di_cx,
                repz,
                cmpsb,
                je,  5,
                sbb_rr, cx_cx,
                sbb_cx_ffff,     /* 3 bytes */
                pop_ds
        };

/****************************************************************/

/* cx strlen( di ) zaps ax,es,cx,di */
#define C_strlen_ret    HW_D( HW_CX )
#define C_strlen_parms  P_DI
#define C_strlen_saves   HW_NotD_4( HW_AX, HW_ES, HW_CX, HW_DI )
static byte_seq C_strlen = {
                14,
                mov_ax_ds,
                mov_es_ax,
                mov_cx, 0xff, 0xff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                dec_cx
        };


/* cx strlen( es:di ) zaps ax,cx,di */
#define DF_strlen_ret   HW_D( HW_CX )
#define DF_strlen_parms P_ESDI
#define DF_strlen_saves  HW_NotD_3( HW_AX, HW_CX, HW_DI )
static byte_seq DF_strlen = {
                10,
                mov_cx, 0xff, 0xff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                dec_cx
        };

/* cx strlen( es:di ) zaps ax,cx,di */
#define DP_strlen_ret   HW_D( HW_CX )
#define DP_strlen_parms P_ESDI
#define DP_strlen_saves  HW_NotD_3( HW_AX, HW_CX, HW_DI )
static byte_seq DP_strlen = {
                10,
                mov_cx, 0xff, 0xff,
                xor_rr, ax_ax,
                repnz,
                scasb,
                not_cx,
                dec_cx
        };

/****************************************************************/

#define C_abs_ret    HW_D( HW_AX )
#define C_abs_parms  P_AX
#define C_abs_saves   HW_NotD_2( HW_AX, HW_DX )
static byte_seq C_abs = {
                5,
                cwd,
                xor_rr, ax_dx,
                sub_rr, ax_dx
        };

/****************************************************************/

#define C_labs_ret    HW_D_2( HW_AX, HW_DX )
#define C_labs_parms  P_AXDX
#define C_labs_saves   HW_NotD_2( HW_AX, HW_DX )
static byte_seq C_labs = {
                11,
                or_dx_dx,
                jge, 7,
                neg_ax,
                adc_dx_0,
                neg_dx
        };

/****************************************************************/

#define C_inp_ret    HW_D( HW_AX )
#define C_inp_parms  P_DX
#define C_inp_saves   HW_NotD( HW_AX )
static byte_seq C_inp = {
                3,
                in_al_dx,
                sub_ah_ah
        };

/****************************************************************/

#define C_inpw_ret    HW_D( HW_AX )
#define C_inpw_parms  P_DX
#define C_inpw_saves   HW_NotD( HW_AX )
static byte_seq C_inpw = {
                1,
                in_ax_dx
        };

/****************************************************************/

#define C_outp_ret    HW_D( HW_AX )
#define C_outp_parms  P_DX_AL
#define C_outp_saves   HW_NotD( HW_EMPTY )
static byte_seq C_outp = {
                1,
                out_dx_al
        };

/****************************************************************/

#define C_outpw_ret    HW_D( HW_AX )
#define C_outpw_parms  P_DX_AX
#define C_outpw_saves   HW_NotD( HW_EMPTY )
static byte_seq C_outpw = {
                1,
                out_dx_ax
        };

/****************************************************************/

#define C_movedata_ret    HW_D( HW_EMPTY )
#define C_movedata_parms  P_AX_SI_ES_DI_CX
#define C_movedata_saves   HW_NotD_3( HW_CX, HW_SI, HW_DI )
static byte_seq C_movedata = {
                12,
                push_ds,
                mov_ds_ax,
                shr_cx_1,
                rep,
                movsw,
                adc_cx_cx,
                rep,
                movsb,
                pop_ds
        };
/****************************************************************/

#define C_enable_ret    HW_D( HW_EMPTY )
#define C_enable_parms  P_AX
#define C_enable_saves   HW_NotD( HW_EMPTY )
static byte_seq C_enable = {
                2,
                sti,
                cld
        };
/****************************************************************/

#define C_disable_ret    HW_D( HW_EMPTY )
#define C_disable_parms  P_AX
#define C_disable_saves   HW_NotD( HW_EMPTY )
static byte_seq C_disable = {
                1,
                cli
        };
/****************************************************************/

#define C_rotl_ret    HW_D( HW_AX )
#define C_rotl_parms  P_AX_CX
#define C_rotl_saves   HW_NotD( HW_AX )
static byte_seq C_rotl = {
                2,
                0xd3,0xc0      /* rol ax,cl */
        };
/****************************************************************/

#define C_rotr_ret    HW_D( HW_AX )
#define C_rotr_parms  P_AX_CX
#define C_rotr_saves   HW_NotD( HW_AX )
static byte_seq C_rotr = {
                2,
                0xd3,0xc8      /* ror ax,cl */
        };
/****************************************************************/

#define C_8087_fabs_ret    HW_D( HW_EMPTY )
#define C_8087_fabs_parms  P_8087
#define C_8087_fabs_saves   HW_NotD( HW_EMPTY )
static byte_seq _8087_fabs = {
                0x80000000 + 4,
                0xff,                   /* 8087 fixup indicator */
                fwait,
                fabs
        };

#define C_fabs_ret    HW_D( HW_ABCD )
#define C_fabs_parms  P_AXBXCXDX
#define C_fabs_saves   HW_NotD( HW_ABCD )
static byte_seq C_fabs = {
                3,
                and_ah_7f
        };

/**************************************************************/

#define C_min_ret       HW_D( HW_AX )
#define C_min_parms     P_AX_DX
#define C_min_saves      HW_NotD_2( HW_CX, HW_AX )
static byte_seq C_min = {
                8,
                sub_ax_dx,
                sbb_cx_cx,
                and_ax_cx,
                add_ax_dx
        };

#define C_max_ret       HW_D( HW_AX )
#define C_max_parms     P_AX_DX
#define C_max_saves      HW_NotD_2( HW_CX, HW_AX )
static byte_seq C_max = {
                9,
                sub_ax_dx,
                cmc,
                sbb_cx_cx,
                and_ax_cx,
                add_ax_dx
        };



/****************************************************************/

static hw_reg_set P_AX_CX[]        = {
    HW_D( HW_AX ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX_DX[]        = {
    HW_D( HW_AX ),
    HW_D( HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_SI[]        = {
    HW_D( HW_DI ),
    HW_D( HW_SI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_SI_CX[]     = {
    HW_D( HW_DI ),
    HW_D( HW_SI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_AX_CX[]   = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AX ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DI_CX[]     = {
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI_AL_CX[]     = {
    HW_D( HW_DI ),
    HW_D( HW_AL ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_SIAX[]    = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_SI, HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_DSSI[]    = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_CXSI_AX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_CX, HW_SI ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_DSSI_CX[] = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI_AL_CX[]   = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AL ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DI[]        = {
    HW_D( HW_SI ),
    HW_D( HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SIAX_ESDI[]    = {
    HW_D_2( HW_SI, HW_AX ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_ESDI[]    = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_CXSI_ESDI_AX[] = {
    HW_D_2( HW_CX, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_ESDI_CX[] = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DXSI_CL[]      = {
    HW_D_2( HW_DX, HW_SI ),
    HW_D( HW_CL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DSSI_CL[]      = {
    HW_D_2( HW_DS, HW_SI ),
    HW_D( HW_CL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_SI_DL[]        = {
    HW_D( HW_SI ),
    HW_D( HW_DL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DI[]           = {
    HW_D( HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_ESDI[]         = {
    HW_D_2( HW_ES, HW_DI ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX[]           = {
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX[]           = {
    HW_D( HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX_AL[]        = {
    HW_D( HW_DX ),
    HW_D( HW_AL ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_DX_AX[]        = {
    HW_D( HW_DX ),
    HW_D( HW_AX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AXDX[]         = {
    HW_D_2( HW_AX, HW_DX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AX_SI_ES_DI_CX[]={
    HW_D( HW_AX ),
    HW_D( HW_SI ),
    HW_D( HW_ES ),
    HW_D( HW_DI ),
    HW_D( HW_CX ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_AXBXCXDX[]     = {
    HW_D( HW_ABCD ),
    HW_D( HW_EMPTY )
};
static hw_reg_set P_8087[]         = {
    HW_D( HW_FLTS ),
    HW_D( HW_EMPTY )
};


struct  inline_funcs SInline_Functions[] = {
    { "strcpy", &S_strcpy, S_strcpy_parms, S_strcpy_ret, S_strcpy_saves },
    { "strcmp", &S_strcmp, S_strcmp_parms, S_strcmp_ret, S_strcmp_saves },
    { "strcat", &S_strcat, S_strcat_parms, S_strcat_ret, S_strcat_saves },
    { "memset", &S_memset, S_memset_parms, S_memset_ret, S_memset_saves },
    { "memcpy", &S_memcpy, S_memcpy_parms, S_memcpy_ret, S_memcpy_saves },
    {"_fstrcpy",&ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves},
    { NULL }
};

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
    { ".stosw", &C_stosw,  C_stosw_parms,  C_stosw_ret, C_stosw_saves   },
    { ".stoswb",&C_stoswb, C_stoswb_parms, C_stoswb_ret, C_stoswb_saves },
    {"_fstrcpy",&DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves},
    { NULL }
};

struct  inline_funcs ZF_Data_Functions[] = {
    { "strcpy", &ZF_strcpy, ZF_strcpy_parms, ZF_strcpy_ret, ZF_strcpy_saves },
    { "strcmp", &ZF_strcmp, ZF_strcmp_parms, ZF_strcmp_ret, ZF_strcmp_saves },
    { "strcat", &ZF_strcat, ZF_strcat_parms, ZF_strcat_ret, ZF_strcat_saves },
    { "memset", &ZF_memset, ZF_memset_parms, ZF_memset_ret, ZF_memset_saves },
    { "memcpy", &ZF_memcpy, ZF_memcpy_parms, ZF_memcpy_ret, ZF_memcpy_saves },
    { NULL }
};


struct  inline_funcs ZP_Data_Functions[] = {
    { "strcpy", &ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves },
    { "strcmp", &ZP_strcmp, ZP_strcmp_parms, ZP_strcmp_ret, ZP_strcmp_saves },
    { "strcat", &ZP_strcat, ZP_strcat_parms, ZP_strcat_ret, ZP_strcat_saves },
    { "memset", &ZP_memset, ZP_memset_parms, ZP_memset_ret, ZP_memset_saves },
    { "memcpy", &ZP_memcpy, ZP_memcpy_parms, ZP_memcpy_ret, ZP_memcpy_saves },
    {"_fstrcpy",&ZP_strcpy, ZP_strcpy_parms, ZP_strcpy_ret, ZP_strcpy_saves },
    { NULL }
};

struct  inline_funcs DF_Data_Functions[] = {
    { "strlen", &DF_strlen, DF_strlen_parms, DF_strlen_ret, DF_strlen_saves },
    { "strcpy", &DF_strcpy, DF_strcpy_parms, DF_strcpy_ret, DF_strcpy_saves },
    { "strcmp", &DF_strcmp, DF_strcmp_parms, DF_strcmp_ret, DF_strcmp_saves },
    { "strcat", &DF_strcat, DF_strcat_parms, DF_strcat_ret, DF_strcat_saves },
    { "strchr", &DF_strchr, DF_strchr_parms, DF_strchr_ret, DF_strchr_saves },
    { "memcpy", &DF_memcpy, DF_memcpy_parms, DF_memcpy_ret, DF_memcpy_saves },
    { "memcmp", &DF_memcmp, DF_memcmp_parms, DF_memcmp_ret, DF_memcmp_saves },
    { "memset", &DF_memset, DF_memset_parms, DF_memset_ret, DF_memset_saves },
    { "memchr", &DF_memchr, DF_memchr_parms, DF_memchr_ret, DF_memchr_saves },
    { ".stosw", &DF_stosw,  DF_stosw_parms,  DF_stosw_ret,  DF_stosw_saves  },
    { ".stoswb",&DF_stoswb, DF_stoswb_parms, DF_stoswb_ret, DF_stoswb_saves },
    { NULL }
 };

struct  inline_funcs DP_Data_Functions[] = {
    { "strlen", &DP_strlen, DP_strlen_parms, DP_strlen_ret, DP_strlen_saves },
    { "strcpy", &DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves },
    { "strcmp", &DP_strcmp, DP_strcmp_parms, DP_strcmp_ret, DP_strcmp_saves },
    { "strcat", &DP_strcat, DP_strcat_parms, DP_strcat_ret, DP_strcat_saves },
    { "strchr", &DP_strchr, DP_strchr_parms, DP_strchr_ret, DP_strchr_saves },
    { "memcpy", &DP_memcpy, DP_memcpy_parms, DP_memcpy_ret, DP_memcpy_saves },
    { "memcmp", &DP_memcmp, DP_memcmp_parms, DP_memcmp_ret, DP_memcmp_saves },
    { "memset", &DP_memset, DP_memset_parms, DP_memset_ret, DP_memset_saves },
    { "memchr", &DP_memchr, DP_memchr_parms, DP_memchr_ret, DP_memchr_saves },
    { ".stosw", &DP_stosw,  DP_stosw_parms,  DP_stosw_ret,  DP_stosw_saves  },
    { ".stoswb",&DP_stoswb, DP_stoswb_parms, DP_stoswb_ret, DP_stoswb_saves },
    { NULL }
 };

struct  inline_funcs _8087_Functions[] = {
 { "fabs", &_8087_fabs, C_8087_fabs_parms, C_8087_fabs_ret, C_8087_fabs_saves  },
 { NULL }
 };


/* memory model independent functions */

struct  inline_funcs Common_Functions[] = {
  { "abs",     &C_abs,     C_abs_parms,     C_abs_ret,     C_abs_saves     },
  { "labs",    &C_labs,    C_labs_parms,    C_labs_ret,    C_labs_saves    },
  { "fabs",    &C_fabs,    C_fabs_parms,    C_fabs_ret,    C_fabs_saves    },
  { "inp",     &C_inp,     C_inp_parms,     C_inp_ret,     C_inp_saves     },
  { "inpw",    &C_inpw,    C_inpw_parms,    C_inpw_ret,    C_inpw_saves    },
  { "outp",    &C_outp,    C_outp_parms,    C_outp_ret,    C_outp_saves    },
  { "outpw",   &C_outpw,   C_outpw_parms,   C_outpw_ret,   C_outpw_saves   },
  { "movedata",&C_movedata,C_movedata_parms,C_movedata_ret,C_movedata_saves},
  { "_enable", &C_enable,  C_enable_parms,  C_enable_ret,  C_enable_saves  },
  { "_disable",&C_disable, C_disable_parms, C_disable_ret, C_disable_saves },
  { "_rotl",   &C_rotl,    C_rotl_parms,    C_rotl_ret,    C_rotl_saves    },
  { "_rotr",   &C_rotr,    C_rotr_parms,    C_rotr_ret,    C_rotr_saves    },
  { "div",     &C_div,     C_div_parms,     C_div_ret,     C_div_saves     },
  { "_fmemcpy",&DP_memcpy, DP_memcpy_parms, DP_memcpy_ret, DP_memcpy_saves },
  { "_fmemcmp",&DP_memcmp, DP_memcmp_parms, DP_memcmp_ret, DP_memcmp_saves },
  { "_fmemset",&DP_memset, DP_memset_parms, DP_memset_ret, DP_memset_saves },
  { "_fmemchr",&DP_memchr, DP_memchr_parms, DP_memchr_ret, DP_memchr_saves },
  { "_fstrcpy",&DP_strcpy, DP_strcpy_parms, DP_strcpy_ret, DP_strcpy_saves },
  { "_fstrcat",&DP_strcat, DP_strcat_parms, DP_strcat_ret, DP_strcat_saves },
  { "_fstrcmp",&DP_strcmp, DP_strcmp_parms, DP_strcmp_ret, DP_strcmp_saves },
  { "_fstrlen",&DP_strlen, DP_strlen_parms, DP_strlen_ret, DP_strlen_saves },
  { ".min",    &C_min,     C_min_parms,     C_min_ret,     C_min_saves     },
  { ".max",    &C_max,     C_max_parms,     C_max_ret,     C_max_saves     },
  { NULL }
 };

#endif

#endif
