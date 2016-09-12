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
* Description:  X86 registers structures and procedures
*               for input/output/expressions.
*
****************************************************************************/


#include <ctype.h>
#include "x86.h"
#include "x86types.h"
#include "madregs.h"

#define NUM_ELTS( name ) (sizeof( name ) / sizeof( name[0] ) )

typedef enum {
    RF_NONE,
    RF_GPREG  = 0x1,
    RF_FPREG  = 0x2,
    RF_MMREG  = 0x4,
    RF_XMMREG = 0x8,
} register_flags;

#define REG_BIT_OFF(basereg)    (offsetof( mad_registers, x86.basereg ) * 8)
#define STK_REG_BIT_OFF(num)    ((offsetof( mad_registers, x86.u.fpu.reg ) + sizeof( xreal ) * num) * 8)
#define MMX_REG_BIT_OFF(num)    ((offsetof( mad_registers, x86.u.mmx.mm ) + sizeof( mmx_reg ) * num) * 8)
#define XMM_REG_BIT_OFF(num)    ((offsetof( mad_registers, x86.xmm.xmm ) + sizeof( xmm_reg ) * num) * 8)

#define GET_STK_REG_IDX(off)    ((off - STK_REG_BIT_OFF( 0 )) / (sizeof( xreal ) * 8))

#define REG( p, name, type, start, size, flags, sublist, cpulevel, fpulevel ) \
    const x86_reg_info p##_##name = { \
        { #name, X86T_##type, start, size, RF_##flags##REG }, \
        sublist, cpulevel, fpulevel \
    };

#define CPU( name, type, basereg, startbit, size, sublist, cpulevel ) \
    REG( CPU, name, type, REG_BIT_OFF( cpu.basereg ) + startbit, size, GP, sublist, cpulevel, LX )

#define CPU_FLAG( name, cpulevel ) \
    CPU( name, BYTE, efl, SHIFT_##name, LEN_##name, NULL, cpulevel )

#define FPU( name, type, basereg, startbit, size, sublist, fpulevel ) \
    REG( FPU, name, type, REG_BIT_OFF( fpu.basereg ) + startbit, size, FP, sublist, LX, fpulevel )

#define FPU1( name, type, basereg, startbit, size, sublist, fpulevel ) \
    REG( FPU, name, type, REG_BIT_OFF( u.fpu.basereg ) + startbit, size, FP, sublist, LX, fpulevel )

#define FPU_STK( num ) \
    REG( FPU, st##num, EXTENDED, STK_REG_BIT_OFF( num ), 80, FP, NULL, LX, L1 )

#define FPU_SW( name, fpulevel ) \
    FPU1( name, BYTE, sw, SHIFT_##name, LEN_##name, NULL, fpulevel )

#define FPU_CW( name, fpulevel ) \
    FPU1( name, BYTE, cw, SHIFT_##name, LEN_##name, NULL, fpulevel )

#define MMX( num ) \
    REG( MMX, mm##num, U64, MMX_REG_BIT_OFF( num ), 64, MM, MMXSubList##num, L5, LX )

#define XMM( num ) \
    REG( XMM, xmm##num, U128, XMM_REG_BIT_OFF( num ), 128, XMM, XMMSubList##num, L6, LX )

#define MXCSR( name ) \
    REG( XMM_mxcsr, name, BYTE, REG_BIT_OFF( xmm.mxcsr ) + SHIFT_mxcsr_##name, LEN_mxcsr_##name, XMM, NULL, L6, LX )

CPU_FLAG( vm,   L3 );
CPU_FLAG( rf,   L3 );
CPU_FLAG( nt,   L2 );
CPU_FLAG( iopl, L2 );
CPU_FLAG( o,    L1 );
CPU_FLAG( d,    L1 );
CPU_FLAG( i,    L1 );
CPU_FLAG( s,    L1 );
CPU_FLAG( z,    L1 );
CPU_FLAG( a,    L1 );
CPU_FLAG( p,    L1 );
CPU_FLAG( c,    L1 );

static const x86_reg_info * const EFLRegList[] = {
    &CPU_vm, &CPU_rf, &CPU_nt, &CPU_iopl,
    &CPU_o, &CPU_d, &CPU_i, &CPU_s, &CPU_z, &CPU_a, &CPU_p, &CPU_c,
NULL };

/* register definitions */
CPU( eax, DWORD, eax, 0, 32, NULL, L3 );
CPU( ebx, DWORD, ebx, 0, 32, NULL, L3 );
CPU( ecx, DWORD, ecx, 0, 32, NULL, L3 );
CPU( edx, DWORD, edx, 0, 32, NULL, L3 );
CPU( esi, DWORD, esi, 0, 32, NULL, L3 );
CPU( edi, DWORD, edi, 0, 32, NULL, L3 );
CPU( ebp, DWORD, ebp, 0, 32, NULL, L3 );
CPU( esp, DWORD, esp, 0, 32, NULL, L3 );
CPU( eip, DWORD, eip, 0, 32, NULL, L3 );
CPU( efl, DWORD, efl, 0, 32, &EFLRegList[0], L3 );

CPU(  ax,  WORD, eax, 0, 16, NULL, L1 );
CPU(  bx,  WORD, ebx, 0, 16, NULL, L1 );
CPU(  cx,  WORD, ecx, 0, 16, NULL, L1 );
CPU(  dx,  WORD, edx, 0, 16, NULL, L1 );
CPU(  si,  WORD, esi, 0, 16, NULL, L1 );
CPU(  di,  WORD, edi, 0, 16, NULL, L1 );
CPU(  bp,  WORD, ebp, 0, 16, NULL, L1 );
CPU(  sp,  WORD, esp, 0, 16, NULL, L1 );
CPU(  ip,  WORD, eip, 0, 16, NULL, L1 );
CPU(  fl,  WORD, efl, 0, 16, &EFLRegList[2], L1 );

CPU(  al,  BYTE, eax, 0,  8, NULL, L1 );
CPU(  bl,  BYTE, ebx, 0,  8, NULL, L1 );
CPU(  cl,  BYTE, ecx, 0,  8, NULL, L1 );
CPU(  dl,  BYTE, edx, 0,  8, NULL, L1 );
CPU(  ah,  BYTE, eax, 8,  8, NULL, L1 );
CPU(  bh,  BYTE, ebx, 8,  8, NULL, L1 );
CPU(  ch,  BYTE, ecx, 8,  8, NULL, L1 );
CPU(  dh,  BYTE, edx, 8,  8, NULL, L1 );

CPU(  cs,  WORD,  cs, 0, 16, NULL, L1 );
CPU(  ds,  WORD,  ds, 0, 16, NULL, L1 );
CPU(  es,  WORD,  es, 0, 16, NULL, L1 );
CPU(  ss,  WORD,  ss, 0, 16, NULL, L1 );
CPU(  fs,  WORD,  fs, 0, 16, NULL, L3 );
CPU(  gs,  WORD,  gs, 0, 16, NULL, L3 );

/* register list definitions */
static const x86_reg_info * const CPURegList[] = {
    &CPU_eax, &CPU_ebx, &CPU_ecx, &CPU_edx, &CPU_eip,
    &CPU_esi, &CPU_edi, &CPU_esp, &CPU_ebp, &CPU_efl,

    &CPU_ax,  &CPU_bx,  &CPU_cx,  &CPU_dx,  &CPU_ip,
    &CPU_si,  &CPU_di,  &CPU_sp,  &CPU_bp,  &CPU_fl,

    &CPU_al,  &CPU_bl,  &CPU_cl,  &CPU_dl,
    &CPU_ah,  &CPU_bh,  &CPU_ch,  &CPU_dh,

    &CPU_cs,  &CPU_ds,  &CPU_es,  &CPU_ss,  &CPU_fs,  &CPU_gs,
NULL };

FPU_SW( ie, L1 );
FPU_SW( de, L1 );
FPU_SW( ze, L1 );
FPU_SW( oe, L1 );
FPU_SW( ue, L1 );
FPU_SW( pe, L1 );
FPU_SW( sf, L3 );
FPU_SW( c0, L1 );
FPU_SW( es, L1 );
FPU_SW( c1, L1 );
FPU_SW( c2, L1 );
FPU_SW( st, L1 );
FPU_SW( c3, L1 );
FPU_SW( b,  L1 );

static const x86_reg_info * const SWRegList[] = {
    &FPU_ie, &FPU_de, &FPU_ze, &FPU_oe, &FPU_ue, &FPU_pe,
    &FPU_sf, &FPU_c0, &FPU_es, &FPU_c1, &FPU_c2, &FPU_st, &FPU_c3, &FPU_b,
NULL };

FPU_CW( im,  L1 );
FPU_CW( dm,  L1 );
FPU_CW( zm,  L1 );
FPU_CW( om,  L1 );
FPU_CW( um,  L1 );
FPU_CW( pm,  L1 );
FPU_CW( iem, L1 );
FPU_CW( pc,  L1 );
FPU_CW( rc,  L1 );
FPU_CW( ic,  L1 );

static const x86_reg_info * const CWRegList[] = {
    &FPU_im, &FPU_dm, &FPU_zm, &FPU_om, &FPU_um, &FPU_pm,
    &FPU_iem, &FPU_pc, &FPU_rc, &FPU_ic,
NULL };

FPU1( sw,  WORD, sw,  0, 16, SWRegList, L1 );
FPU1( cw,  WORD, cw,  0, 16, CWRegList, L1 );
FPU1( tag, WORD, tag, 0, 16, NULL, L1 );
FPU1( tag0, BIT, tag, 0,  2, NULL, L1 );
FPU1( tag1, BIT, tag, 2,  2, NULL, L1 );
FPU1( tag2, BIT, tag, 4,  2, NULL, L1 );
FPU1( tag3, BIT, tag, 6,  2, NULL, L1 );
FPU1( tag4, BIT, tag, 8,  2, NULL, L1 );
FPU1( tag5, BIT, tag,10,  2, NULL, L1 );
FPU1( tag6, BIT, tag,12,  2, NULL, L1 );
FPU1( tag7, BIT, tag,14,  2, NULL, L1 );
FPU1( iptr, F32_PTR, ip_err, 0, 64, NULL, L1 );
FPU1( optr, F32_PTR, op_err, 0, 64, NULL, L1 );

FPU_STK( 0 );
FPU_STK( 1 );
FPU_STK( 2 );
FPU_STK( 3 );
FPU_STK( 4 );
FPU_STK( 5 );
FPU_STK( 6 );
FPU_STK( 7 );

static const x86_reg_info * const FPURegList[] = {
    &FPU_st0, &FPU_st1, &FPU_st2, &FPU_st3,
    &FPU_st4, &FPU_st5, &FPU_st6, &FPU_st7,
    &FPU_sw,  &FPU_cw,  &FPU_tag,
NULL };

#define X86T_b  X86T_BYTE
#define X86T_w  X86T_WORD
#define X86T_d  X86T_DWORD
#define X86T_q  X86T_QWORD
#define MMXS_b  8
#define MMXS_w  16
#define MMXS_d  32
#define MMXS_q  64
#define XMMS_b  8
#define XMMS_w  16
#define XMMS_d  32
#define XMMS_q  64

#define MMX_SUBREG( s, i, reg ) \
    REG( MMX##reg, s##i, s, MMX_REG_BIT_OFF( reg ) + MMXS_##s * i, MMXS_##s, MM, NULL, L5, LX )

#define XMM_SUBREG( s, i, reg ) \
    REG( XMM##reg, s##i, s, XMM_REG_BIT_OFF( reg ) + XMMS_##s * i, XMMS_##s, XMM, NULL, L6, LX )

#define MMX_SUBLIST( n )        \
        MMX_SUBREG( b, 0, n )   \
        MMX_SUBREG( b, 1, n )   \
        MMX_SUBREG( b, 2, n )   \
        MMX_SUBREG( b, 3, n )   \
        MMX_SUBREG( b, 4, n )   \
        MMX_SUBREG( b, 5, n )   \
        MMX_SUBREG( b, 6, n )   \
        MMX_SUBREG( b, 7, n )   \
        MMX_SUBREG( w, 0, n )   \
        MMX_SUBREG( w, 1, n )   \
        MMX_SUBREG( w, 2, n )   \
        MMX_SUBREG( w, 3, n )   \
        MMX_SUBREG( d, 0, n )   \
        MMX_SUBREG( d, 1, n )   \
        MMX_SUBREG( q, 0, n )   \
                                \
        static const x86_reg_info * const MMXSubList##n[] = {           \
            &MMX##n##_b0, &MMX##n##_b1, &MMX##n##_b2, &MMX##n##_b3,     \
            &MMX##n##_b4, &MMX##n##_b5, &MMX##n##_b6, &MMX##n##_b7,     \
            &MMX##n##_w0, &MMX##n##_w1, &MMX##n##_w2, &MMX##n##_w3,     \
            &MMX##n##_d0, &MMX##n##_d1, &MMX##n##_q0, NULL              \
        };

MMX_SUBLIST( 0 )
MMX_SUBLIST( 1 )
MMX_SUBLIST( 2 )
MMX_SUBLIST( 3 )
MMX_SUBLIST( 4 )
MMX_SUBLIST( 5 )
MMX_SUBLIST( 6 )
MMX_SUBLIST( 7 )

MMX( 0 );
MMX( 1 );
MMX( 2 );
MMX( 3 );
MMX( 4 );
MMX( 5 );
MMX( 6 );
MMX( 7 );

static const x86_reg_info * const MMXRegList[] = {
    &MMX_mm0, &MMX_mm1, &MMX_mm2, &MMX_mm3,
    &MMX_mm4, &MMX_mm5, &MMX_mm6, &MMX_mm7,
NULL };


#define XMM_SUBLIST( n )        \
        XMM_SUBREG( b, 0, n )   \
        XMM_SUBREG( b, 1, n )   \
        XMM_SUBREG( b, 2, n )   \
        XMM_SUBREG( b, 3, n )   \
        XMM_SUBREG( b, 4, n )   \
        XMM_SUBREG( b, 5, n )   \
        XMM_SUBREG( b, 6, n )   \
        XMM_SUBREG( b, 7, n )   \
        XMM_SUBREG( b, 8, n )   \
        XMM_SUBREG( b, 9, n )   \
        XMM_SUBREG( b, 10, n )  \
        XMM_SUBREG( b, 11, n )  \
        XMM_SUBREG( b, 12, n )  \
        XMM_SUBREG( b, 13, n )  \
        XMM_SUBREG( b, 14, n )  \
        XMM_SUBREG( b, 15, n )  \
        XMM_SUBREG( w, 0, n )   \
        XMM_SUBREG( w, 1, n )   \
        XMM_SUBREG( w, 2, n )   \
        XMM_SUBREG( w, 3, n )   \
        XMM_SUBREG( w, 4, n )   \
        XMM_SUBREG( w, 5, n )   \
        XMM_SUBREG( w, 6, n )   \
        XMM_SUBREG( w, 7, n )   \
        XMM_SUBREG( d, 0, n )   \
        XMM_SUBREG( d, 1, n )   \
        XMM_SUBREG( d, 2, n )   \
        XMM_SUBREG( d, 3, n )   \
        XMM_SUBREG( q, 0, n )   \
        XMM_SUBREG( q, 1, n )   \
                                \
        static const x86_reg_info * const XMMSubList##n[] = {           \
            &XMM##n##_b0, &XMM##n##_b1, &XMM##n##_b2, &XMM##n##_b3,     \
            &XMM##n##_b4, &XMM##n##_b5, &XMM##n##_b6, &XMM##n##_b7,     \
            &XMM##n##_b8, &XMM##n##_b9, &XMM##n##_b10,&XMM##n##_b11,    \
            &XMM##n##_b12,&XMM##n##_b13,&XMM##n##_b14,&XMM##n##_b15,    \
            &XMM##n##_w0, &XMM##n##_w1, &XMM##n##_w2, &XMM##n##_w3,     \
            &XMM##n##_w4, &XMM##n##_w5, &XMM##n##_w6, &XMM##n##_w7,     \
            &XMM##n##_d0, &XMM##n##_d1, &XMM##n##_d2, &XMM##n##_d3,     \
            &XMM##n##_q0, &XMM##n##_q1, NULL };

XMM_SUBLIST( 0 )
XMM_SUBLIST( 1 )
XMM_SUBLIST( 2 )
XMM_SUBLIST( 3 )
XMM_SUBLIST( 4 )
XMM_SUBLIST( 5 )
XMM_SUBLIST( 6 )
XMM_SUBLIST( 7 )

XMM( 0 );
XMM( 1 );
XMM( 2 );
XMM( 3 );
XMM( 4 );
XMM( 5 );
XMM( 6 );
XMM( 7 );

MXCSR( ie );
MXCSR( de );
MXCSR( ze );
MXCSR( oe );
MXCSR( ue );
MXCSR( pe );
MXCSR( daz );
MXCSR( im );
MXCSR( dm );
MXCSR( zm );
MXCSR( om );
MXCSR( um );
MXCSR( pm );
MXCSR( rc );
MXCSR( fz );

static const x86_reg_info * const MXCSRRegList[] = {
    &XMM_mxcsr_ie, &XMM_mxcsr_de, &XMM_mxcsr_ze, &XMM_mxcsr_oe, &XMM_mxcsr_ue, &XMM_mxcsr_pe,
    &XMM_mxcsr_daz,
    &XMM_mxcsr_im, &XMM_mxcsr_dm, &XMM_mxcsr_zm, &XMM_mxcsr_om, &XMM_mxcsr_um, &XMM_mxcsr_pm,
    &XMM_mxcsr_rc, &XMM_mxcsr_fz,
NULL };

/* MXCSR register */
REG( XMM, mxcsr, DWORD, REG_BIT_OFF( xmm.mxcsr ), 32, XMM, MXCSRRegList, L6, LX )

static const x86_reg_info * const XMMRegList[] = {
    &XMM_xmm0, &XMM_xmm1, &XMM_xmm2, &XMM_xmm3,
    &XMM_xmm4, &XMM_xmm5, &XMM_xmm6, &XMM_xmm7,
    &XMM_mxcsr,
NULL };


struct mad_reg_set_data {
    mad_status (*get_piece)( const mad_registers *mr, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value );
    const mad_toggle_strings    *togglelist;
    mad_string                  name;
    const x86_reg_info          * const *reglist;
    unsigned_8                  grouping;
};

static const mad_toggle_strings CPUToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_MEXTENDED,MAD_MSTR_REG_EXTENDED,MAD_MSTR_REG_NORMAL},
    {MAD_MSTR_MOS,MAD_MSTR_REG_OS,MAD_MSTR_REG_USER},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};
static const mad_toggle_strings FPUToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};
static const mad_toggle_strings MMXToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_SIGNED,MAD_MSTR_SIGNED,MAD_MSTR_UNSIGNED},
    {MAD_MSTR_BYTE,MAD_MSTR_BYTE,MAD_MSTR_NIL},
    {MAD_MSTR_WORD,MAD_MSTR_WORD,MAD_MSTR_NIL},
    {MAD_MSTR_DWORD,MAD_MSTR_DWORD,MAD_MSTR_NIL},
    {MAD_MSTR_QWORD,MAD_MSTR_QWORD,MAD_MSTR_NIL},
    {MAD_MSTR_FLOAT,MAD_MSTR_FLOAT,MAD_MSTR_NIL},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};
static const mad_toggle_strings XMMToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_SIGNED,MAD_MSTR_SIGNED,MAD_MSTR_UNSIGNED},
    {MAD_MSTR_BYTE,MAD_MSTR_BYTE,MAD_MSTR_NIL},
    {MAD_MSTR_WORD,MAD_MSTR_WORD,MAD_MSTR_NIL},
    {MAD_MSTR_DWORD,MAD_MSTR_DWORD,MAD_MSTR_NIL},
    {MAD_MSTR_QWORD,MAD_MSTR_QWORD,MAD_MSTR_NIL},
    {MAD_MSTR_FLOAT,MAD_MSTR_FLOAT,MAD_MSTR_NIL},
    {MAD_MSTR_DOUBLE,MAD_MSTR_DOUBLE,MAD_MSTR_NIL},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};

#define FPU_GROUPING    6

static mad_status CPUGetPiece( const mad_registers *, unsigned piece, const char **, size_t *, const mad_reg_info **, mad_type_handle *, size_t * );
static mad_status FPUGetPiece( const mad_registers *, unsigned piece, const char **, size_t *, const mad_reg_info **, mad_type_handle *, size_t * );
static mad_status MMXGetPiece( const mad_registers *, unsigned piece, const char **, size_t *, const mad_reg_info **, mad_type_handle *, size_t * );
static mad_status XMMGetPiece( const mad_registers *, unsigned piece, const char **, size_t *, const mad_reg_info **, mad_type_handle *, size_t * );

static const mad_reg_set_data RegSet[] = {
    { CPUGetPiece, CPUToggleList, MAD_MSTR_CPU, CPURegList, 0 },
    { FPUGetPiece, FPUToggleList, MAD_MSTR_FPU, FPURegList, FPU_GROUPING },
    { MMXGetPiece, MMXToggleList, MAD_MSTR_MMX, MMXRegList, 1 },
    { XMMGetPiece, XMMToggleList, MAD_MSTR_XMM, XMMRegList, 1 } };

unsigned MADIMPENTRY( RegistersSize )( void )
{
    return( sizeof( struct x86_mad_registers ) );
}


#define EXTRACT_ST( mr ) (((unsigned_16)(mr)->x86.u.fpu.sw >> SHIFT_st) & ((1<<LEN_st)-1))

mad_status MADIMPENTRY( RegistersHost )( mad_registers *mr )
{
    unsigned    st;
    unsigned_16 tag;

    /* normalize the tag bits to tag[0..1] refer to ST(0), etc */
    st = EXTRACT_ST( mr ) * 2;
    tag = (unsigned_16)mr->x86.u.fpu.tag;
    tag = (tag >> (st)) | (tag << (16 - st));
    mr->x86.u.fpu.tag &= ~0xffffUL;
    mr->x86.u.fpu.tag |= tag;
    return( MS_MODIFIED );
}

mad_status MADIMPENTRY( RegistersTarget )( mad_registers *mr )
{
    unsigned    st;
    unsigned_16 tag;

    /* put the tag bits back the stupid way the x87 wants them */
    st = EXTRACT_ST( mr ) * 2;
    tag = (unsigned_16)mr->x86.u.fpu.tag;
    tag = (tag << (st)) | (tag >> (16 - st));
    mr->x86.u.fpu.tag &= ~0xffffUL;
    mr->x86.u.fpu.tag |= tag;
    return( MS_MODIFIED );
}

walk_result MADIMPENTRY( RegSetWalk )( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    walk_result wr;

    if( tk & (MTK_INTEGER|MTK_ADDRESS) ) {
        wr = wk( &RegSet[CPU_REG_SET], d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    if( tk & MTK_FLOAT ) {
        wr = wk( &RegSet[FPU_REG_SET], d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    if( (tk & MTK_CUSTOM) && (MCSystemConfig()->cpu & X86_MMX) ) {
        wr = wk( &RegSet[MMX_REG_SET], d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    if( (tk & MTK_XMM) && (MCSystemConfig()->cpu & X86_XMM) ) {
        wr = wk( &RegSet[XMM_REG_SET], d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    return( WR_CONTINUE );
}

mad_string MADIMPENTRY( RegSetName )( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

size_t MADIMPENTRY( RegSetLevel )( const mad_reg_set_data *rsd, char *buff, size_t buff_size )
{
    char        str[80];
    size_t      len;

    switch( rsd - RegSet ) {
    case CPU_REG_SET:
        switch( MCSystemConfig()->cpu & X86_CPU_MASK ) {
        case X86_86:
            strcpy( str, "8086" );
            break;
        case X86_586:
            strcpy( str, "Pentium" );
            break;
        case X86_686:
            strcpy( str, "Pentium Pro" );
            break;
        case X86_P4:
            strcpy( str, "Pentium 4/Xeon" );
            break;
        default:
            str[0] = MCSystemConfig()->cpu + '0';
            strcpy( &str[1], "86" );
            break;
        }
        break;
    case FPU_REG_SET:
        switch( MCSystemConfig()->fpu ) {
        case X86_NO:
            MCString( MAD_MSTR_NONE, str, sizeof( str ) );
            break;
        case X86_EMU:
            MCString( MAD_MSTR_EMULATOR, str, sizeof( str ) );
            break;
        case X86_87:
            strcpy( str, "8087" );
            break;
        case X86_587:
            strcpy( str, "Pentium" );
            break;
        case X86_687:
            strcpy( str, "Pentium Pro" );
            break;
        case X86_P47:
            strcpy( str, "Pentium 4/Xeon" );
            break;
        default:
            str[0] = MCSystemConfig()->fpu + '0';
            strcpy( &str[1], "87" );
            break;
        }
        break;
    default:
        str[0] = '\0';
        break;
    }
    len = strlen( str );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, str, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

unsigned MADIMPENTRY( RegSetDisplayGrouping )( const mad_reg_set_data *rsd )
{
    switch( rsd - RegSet ) {
    case MMX_REG_SET:
        if( MADState->reg_state[MMX_REG_SET] & MT_BYTE ) {
            return( 8 );
        } else if( MADState->reg_state[MMX_REG_SET] & MT_WORD ) {
            return( 4 );
        } else if( MADState->reg_state[MMX_REG_SET] & MT_DWORD ) {
            return( 2 );
        } else if( MADState->reg_state[MMX_REG_SET] & MT_FLOAT ) {
            return( 2 );
        } else {
            return( 1 );
        }
    case XMM_REG_SET:
        if( MADState->reg_state[XMM_REG_SET] & XT_BYTE ) {
            return( 16 + 2 );
        } else if( MADState->reg_state[XMM_REG_SET] & XT_WORD ) {
            return( 8 + 2 );
        } else if( MADState->reg_state[XMM_REG_SET] & XT_DWORD ) {
            return( 4 + 2 );
        } else if( MADState->reg_state[XMM_REG_SET] & XT_QWORD ) {
            return( 2 + 2 );
        } else if( MADState->reg_state[XMM_REG_SET] & XT_FLOAT ) {
            return( 4 + 2 );
        } else {  // double
            return( 2 + 2 );
        }
    default:
        return( rsd->grouping );
    }
}

static char     DescriptBuff[40];

static mad_status CPUGetPiece(
    const mad_registers *mr,
    unsigned piece,
    const char **descript_p,
    size_t *max_descript_p,
    const mad_reg_info **reg_p,
    mad_type_handle *disp_type_p,
    size_t *max_value_p )
{
    static const x86_reg_info *list16[] = {
        &CPU_ax, &CPU_bx, &CPU_cx, &CPU_dx,
        &CPU_si, &CPU_di, &CPU_bp, &CPU_sp, &CPU_ip,
        &CPU_ds, &CPU_es, &CPU_ss, &CPU_cs,
        &CPU_fl, &CPU_c, &CPU_p, &CPU_a, &CPU_z, &CPU_s, &CPU_i, &CPU_d, &CPU_o,
        };
    static const x86_reg_info *list32[] = {
        &CPU_eax, &CPU_ebx, &CPU_ecx, &CPU_edx,
        &CPU_esi, &CPU_edi, &CPU_ebp, &CPU_esp, &CPU_eip,
        &CPU_efl, &CPU_c, &CPU_p, &CPU_a, &CPU_z, &CPU_s, &CPU_i, &CPU_d, &CPU_o,
        &CPU_ds, &CPU_es, &CPU_fs, &CPU_gs, &CPU_ss, &CPU_cs,
        };
    static const x86_reg_info *listOS[] = {
        &CPU_iopl, &CPU_nt, &CPU_rf, &CPU_vm,
        };
    const x86_reg_info          *curr;
    const x86_reg_info          **list;
    unsigned                    list_num;
    char                        *p;
    static const x86_reg_info   **last_list;

    if( (MADState->reg_state[CPU_REG_SET] & CT_EXTENDED) || BIG_SEG( GetRegIP( mr ) ) ) {
        list = list32;
        list_num = NUM_ELTS( list32 );
    } else {
        list = list16;
        list_num = NUM_ELTS( list16 );
    }
    if( last_list == NULL )
        last_list = list;
    if( last_list != list ) {
        last_list = list;
        MCNotify( MNT_REDRAW_REG, (void *)&RegSet[CPU_REG_SET] );
    }
    if( MADState->reg_state[CPU_REG_SET] & CT_OS ) {
        *max_descript_p = 4;
        if( piece >= list_num ) {
            piece -= list_num;
            if( piece >= NUM_ELTS( listOS ) )
                return( MS_FAIL );
            list = listOS;
        }
    } else {
        *max_descript_p = 3;
        if( piece >= list_num ) {
            return( MS_FAIL );
        }
    }
    curr = list[ piece ];
    *max_value_p = 0;
    *reg_p = &curr->info;
    *descript_p = DescriptBuff;
    strcpy( DescriptBuff, curr->info.name );
    p = DescriptBuff;
    while( *p != '\0' ) {
        *p = (char)toupper( *(unsigned char *)p );
        ++p;
    }
    *p = '\0';
    if( curr->info.bit_size <= 3 ) {
        *disp_type_p = X86T_BIT;
    } else if( curr->info.bit_size <= 16 ) {
        if( MADState->reg_state[CPU_REG_SET] & CT_HEX ) {
            *disp_type_p = X86T_WORD;
        } else {
            *disp_type_p = X86T_USHORT;
        }
    } else {
        if( MADState->reg_state[CPU_REG_SET] & CT_HEX ) {
            *disp_type_p = X86T_DWORD;
        } else {
            *disp_type_p = X86T_ULONG;
        }
    }
    return( MS_OK );
}

static const byte zero  = 0;
static const byte one   = 1;
static const byte two   = 2;
static const byte three = 3;
static const byte four  = 4;
static const byte five  = 5;
static const byte six   = 6;
static const byte seven = 7;

static const mad_modify_list ModBit[] = {
    { &zero, X86T_BYTE, MAD_MSTR_NIL },
    { &one,  X86T_BYTE, MAD_MSTR_NIL },
};
static const mad_modify_list ModByte[] = {
    { NULL, X86T_BYTE, MAD_MSTR_NIL },
};
static const mad_modify_list ModWord[] = {
    { NULL, X86T_WORD, MAD_MSTR_NIL },
};
static const mad_modify_list ModDWord[] = {
    { NULL, X86T_DWORD, MAD_MSTR_NIL },
};
static const mad_modify_list ModQWord[] = {
    { NULL, X86T_QWORD, MAD_MSTR_NIL },
};
static const mad_modify_list ModFPUTag[] = {
    { &zero,  X86T_BYTE, MAD_MSTR_VALID },
    { &one,   X86T_BYTE, MAD_MSTR_ZERO },
    { &two,   X86T_BYTE, MAD_MSTR_SPECIAL },
    { &three, X86T_BYTE, MAD_MSTR_EMPTY },
};
static const mad_modify_list ModFPUStack[] = {
    { NULL, X86T_EXTENDED, MAD_MSTR_NIL },
};
static const mad_modify_list ModFPUSt[] = {
    { &zero, X86T_BIT, MAD_MSTR_NIL },
    { &one,  X86T_BIT, MAD_MSTR_NIL },
    { &two,  X86T_BIT, MAD_MSTR_NIL },
    { &three,X86T_BIT, MAD_MSTR_NIL },
    { &four, X86T_BIT, MAD_MSTR_NIL },
    { &five, X86T_BIT, MAD_MSTR_NIL },
    { &six,  X86T_BIT, MAD_MSTR_NIL },
    { &seven,X86T_BIT, MAD_MSTR_NIL },
};
static const mad_modify_list ModFPUPc[] = {
    { &zero,  X86T_BYTE, MAD_MSTR_PCSINGLE },
    { &one,   X86T_BYTE, MAD_MSTR_NIL },
    { &two,   X86T_BYTE, MAD_MSTR_PCDOUBLE },
    { &three, X86T_BYTE, MAD_MSTR_PCEXTENDED },
};
static const mad_modify_list ModFPURc[] = {
    { &zero,  X86T_BYTE, MAD_MSTR_NEAREST },
    { &one,   X86T_BYTE, MAD_MSTR_DOWN },
    { &two,   X86T_BYTE, MAD_MSTR_UP },
    { &three, X86T_BYTE, MAD_MSTR_CHOP },
};
static const mad_modify_list ModFPUIc[] = {
    { &zero,  X86T_BYTE, MAD_MSTR_PROJECTIVE },
    { &one,   X86T_BYTE, MAD_MSTR_AFFINE },
};

#define MODLEN( name ) MaxModLen( name, NUM_ELTS( name ) )

static size_t MaxModLen( const mad_modify_list *list, unsigned num )
{
    size_t      max;
    size_t      len;

    max = 0;
    while( num-- > 0 ) {
        len = MCString( list[num].name, NULL, 0 );
        if( len > max ) {
            max = len;
        }
    }
    return( max );
}

#define LIST( num, r1, r2, r3, r4, r5, r6, r7, r8 )     \
    static const x86_reg_info * const list##num[]       \
    = { &FPU_##r1, &FPU_##r2, &FPU_##r3, &FPU_##r4,     \
        &FPU_##r5, &FPU_##r6, &FPU_##r7, &FPU_##r8 }

static x86_reg_info     XXX_dummy;

static unsigned GetTag( const mad_registers *mr, int st )
{
    int shft;

    shft = ( st & 0x07 ) << 1;
    return(( mr->x86.u.fpu.tag >> shft ) & 0x03 );
}

static void SetTag( mad_registers *mr, int st, int val )
{
    int shft;

    shft = ( st & 0x07 ) << 1;
    mr->x86.u.fpu.tag &= ~( 3 << shft );
    mr->x86.u.fpu.tag |= ( val & 0x03 ) << shft;
}

static mad_status FPUGetPiece(
    const mad_registers *mr,
    unsigned piece,
    const char **descript_p,
    size_t *max_descript_p,
    const mad_reg_info **reg_p,
    mad_type_handle *disp_type_p,
    size_t *max_value_p )
{
    const static x86_reg_info   FPU_nil = { 0 };

    LIST( 0, st0,  st1,  st2,  st3,  st4,  st5,  st6,  st7 );
    LIST( 1, tag0, tag1, tag2, tag3, tag4, tag5, tag6, tag7 );
    LIST( 2, ie,   de,   ze,   oe,   ue,   pe,   sf,   es  );
    LIST( 3, st,   c0,   c1,   c2,   c3,   nil,  nil,  nil );
    LIST( 4, im,   dm,   zm,   om,   um,   pm,   iem,  nil );
    LIST( 5, sw,   cw,   pc,   rc,   ic,   iptr, optr, nil );

    unsigned            row;
    unsigned            column;
    char                *p;
    unsigned            tag;

    /* in case we haven't got an FPU */
    *reg_p = &XXX_dummy.info;
    *disp_type_p = X86T_UNKNOWN;

    DescriptBuff[0] = '\0';
    p = DescriptBuff;
    *descript_p = p;
    *max_value_p = 0;
    row = piece / FPU_GROUPING;
    if( row > 7 )
        return( MS_FAIL );
    column = piece % FPU_GROUPING;
    switch( column ) {
    case 0: /* stack registers */
        *max_value_p = 28;
        *p++ = 'S';
        *p++ = 'T';
        *p++ = '(';
        *p++ = '0' + (char)row;
        *p++ = ')';
        *p++ = '\0';
        if( MCSystemConfig()->fpu == X86_NO )
            break;
        *reg_p = &list0[row]->info;
        if( MADState->reg_state[FPU_REG_SET] & FT_HEX ) {
            *disp_type_p = X86T_HEXTENDED;
        } else {
            tag = GetTag( mr, row );
            switch( tag ) {
            case 0: /* valid */
            case 1: /* zero */
            case 2: /* special */
                *disp_type_p = X86T_EXTENDED;
                break;
            case 3: /* empty */
                *disp_type_p = X86T_F10EMPTY;
                break;
            }
        }
        break;
    case 1: /* tag registers */
        if( MCSystemConfig()->fpu == X86_NO )
            break;
        *p++ = 'T';
        *p++ = 'A';
        *p++ = 'G';
        *p++ = '(';
        *p++ = '0' + (char)row;
        *p++ = ')';
        *p++ = '\0';
        *reg_p = &list1[row]->info;
        *disp_type_p = X86T_TAG;
        *max_value_p = MODLEN( ModFPUTag );
        break;
    case 2: /* status word bits 1 */
    case 3: /* status word bits 2 */
    case 4: /* control word bits */
        if( column == 2 ) {
            *reg_p = &list2[row]->info;
        } else if( column == 3 ) {
            *reg_p = &list3[row]->info;
        } else {
            *reg_p = &list4[row]->info;
        }
        if( (*reg_p)->name == NULL ) {
            *reg_p = NULL;
            break;
        }
        strcpy( p, (*reg_p)->name );
        *max_value_p = 1;
        if( MCSystemConfig()->fpu == X86_NO )
            break;
        *disp_type_p = X86T_BIT;
        break;
    case 5: /* misc */
        *reg_p = &list5[row]->info;
        switch( row ) {
        case 0:
            strcpy( p, "status" );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            *max_value_p = 0;
            *disp_type_p = X86T_WORD;
            break;
        case 1:
            strcpy( p, "control" );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            *max_value_p = 0;
            *disp_type_p = X86T_WORD;
            break;
        case 2:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPUPc );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            *disp_type_p = X86T_PC;
            break;
        case 3:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPURc );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            *disp_type_p = X86T_RC;
            break;
        case 4:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPUIc );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            *disp_type_p = X86T_IC;
            break;
        case 5: /* iptr */
        case 6: /* optr */
            strcpy( p, (*reg_p)->name );
            if( MCSystemConfig()->fpu == X86_NO )
                break;
            switch( AddrCharacteristics( GetRegIP( mr ) ) ) {
            case X86AC_REAL:
                *disp_type_p = X86T_FPPTR_REAL;
                break;
            case X86AC_BIG:
                *disp_type_p = X86T_FPPTR_32;
                break;
            case 0: /* 16-bit protect mode */
                *disp_type_p = X86T_FPPTR_16;
                break;
            }
            break;
        case 7:
            *reg_p = NULL;
            break;
        }
        break;
    }
    *max_descript_p = strlen( DescriptBuff );
    return( MS_OK );
}

static mad_status MMXGetPiece(
    const mad_registers *mr,
    unsigned piece,
    const char **descript_p,
    size_t *max_descript_p,
    const mad_reg_info **reg_p,
    mad_type_handle *disp_type_p,
    size_t *max_value_p )
{
    static const x86_reg_info *list_byte[] = {
        &MMX0_b7,&MMX0_b6,&MMX0_b5,&MMX0_b4,&MMX0_b3,&MMX0_b2,&MMX0_b1,&MMX0_b0,
        &MMX1_b7,&MMX1_b6,&MMX1_b5,&MMX1_b4,&MMX1_b3,&MMX1_b2,&MMX1_b1,&MMX1_b0,
        &MMX2_b7,&MMX2_b6,&MMX2_b5,&MMX2_b4,&MMX2_b3,&MMX2_b2,&MMX2_b1,&MMX2_b0,
        &MMX3_b7,&MMX3_b6,&MMX3_b5,&MMX3_b4,&MMX3_b3,&MMX3_b2,&MMX3_b1,&MMX3_b0,
        &MMX4_b7,&MMX4_b6,&MMX4_b5,&MMX4_b4,&MMX4_b3,&MMX4_b2,&MMX4_b1,&MMX4_b0,
        &MMX5_b7,&MMX5_b6,&MMX5_b5,&MMX5_b4,&MMX5_b3,&MMX5_b2,&MMX5_b1,&MMX5_b0,
        &MMX6_b7,&MMX6_b6,&MMX6_b5,&MMX6_b4,&MMX6_b3,&MMX6_b2,&MMX6_b1,&MMX6_b0,
        &MMX7_b7,&MMX7_b6,&MMX7_b5,&MMX7_b4,&MMX7_b3,&MMX7_b2,&MMX7_b1,&MMX7_b0,
    };
    static const x86_reg_info *list_word[] = {
        &MMX0_w3,&MMX0_w2,&MMX0_w1,&MMX0_w0,
        &MMX1_w3,&MMX1_w2,&MMX1_w1,&MMX1_w0,
        &MMX2_w3,&MMX2_w2,&MMX2_w1,&MMX2_w0,
        &MMX3_w3,&MMX3_w2,&MMX3_w1,&MMX3_w0,
        &MMX4_w3,&MMX4_w2,&MMX4_w1,&MMX4_w0,
        &MMX5_w3,&MMX5_w2,&MMX5_w1,&MMX5_w0,
        &MMX6_w3,&MMX6_w2,&MMX6_w1,&MMX6_w0,
        &MMX7_w3,&MMX7_w2,&MMX7_w1,&MMX7_w0,
    };
    static const x86_reg_info *list_dword[] = {
        &MMX0_d1,&MMX0_d0,
        &MMX1_d1,&MMX1_d0,
        &MMX2_d1,&MMX2_d0,
        &MMX3_d1,&MMX3_d0,
        &MMX4_d1,&MMX4_d0,
        &MMX5_d1,&MMX5_d0,
        &MMX6_d1,&MMX6_d0,
        &MMX7_d1,&MMX7_d0,
    };
    static const x86_reg_info *list_qword[] = {
        &MMX0_q0,
        &MMX1_q0,
        &MMX2_q0,
        &MMX3_q0,
        &MMX4_q0,
        &MMX5_q0,
        &MMX6_q0,
        &MMX7_q0,
    };
    #define T(t)        X86T_##t
    static const mad_type_handle type_select[2][2][5] = {
        { { T(UCHAR),T(USHORT),T(ULONG), T(U64),   T(FLOAT) },   /* decimal, unsigned */
          { T(CHAR), T(SHORT), T(LONG),  T(I64),   T(FLOAT) } }, /* decimal, signed */
        { { T(BYTE), T(WORD),  T(DWORD), T(QWORD), T(FLOAT) },   /* hex, unsigned */
          { T(SBYTE),T(SWORD), T(SDWORD),T(SQWORD),T(FLOAT) } }, /* hex, signed */
    };
    #undef T

    unsigned            group;
    unsigned            list_num;
    unsigned            type;
    const x86_reg_info  **list;

    mr = mr;
    /* in case we haven't got an MMX */
    *reg_p = NULL;
    *disp_type_p = X86T_UNKNOWN;

    *max_descript_p = 0;
    DescriptBuff[0] = '\0';
    *descript_p = DescriptBuff;
    *max_value_p = 0;
    if( ( MCSystemConfig()->cpu & X86_MMX ) == 0 )
        return( MS_FAIL );

    if( MADState->reg_state[MMX_REG_SET] & MT_BYTE ) {
        list = list_byte;
        list_num = NUM_ELTS( list_byte );
        group = 8;
        type = 0;
    } else if( MADState->reg_state[MMX_REG_SET] & MT_WORD ) {
        list = list_word;
        list_num = NUM_ELTS( list_word );
        group = 4;
        type = 1;
    } else if( MADState->reg_state[MMX_REG_SET] & MT_DWORD ) {
        list = list_dword;
        list_num = NUM_ELTS( list_dword );
        group = 2;
        type = 2;
    } else if( MADState->reg_state[MMX_REG_SET] & MT_FLOAT ) {
        list = list_dword;
        list_num = NUM_ELTS( list_dword );
        group = 2;
        type = 4;
    } else {
        list = list_qword;
        list_num = NUM_ELTS( list_qword );
        group = 1;
        type = 3;
    }
    *disp_type_p = type_select[( MADState->reg_state[MMX_REG_SET] & MT_HEX ) ? 1 : 0]
                              [( MADState->reg_state[MMX_REG_SET] & MT_SIGNED ) ? 1 : 0]
                              [type];

    if( piece < group ) {
        *max_value_p = 2;
        *reg_p = &XXX_dummy.info;
        *disp_type_p = (mad_type_handle)( X86T_MMX_TITLE0 - 1 + group - piece );
        return( MS_OK );
    }
    piece -= group;

    if( piece >= list_num )
        return( MS_FAIL );
    if( (piece % group) == 0 ) {
        /* first column */
        DescriptBuff[0] = 'M';
        DescriptBuff[1] = 'M';
        DescriptBuff[2] = (char)( piece / group ) + '0';
        DescriptBuff[3] = '\0';
        *max_descript_p = 3;
    }
    *reg_p = &list[piece]->info;
    return( MS_OK );
}

static mad_status XMMGetPiece(
    const mad_registers *mr,
    unsigned piece,
    const char **descript_p,
    size_t *max_descript_p,
    const mad_reg_info **reg_p,
    mad_type_handle *disp_type_p,
    size_t *max_value_p )
{
    static const x86_reg_info *list_byte[] = {
        &XMM0_b15,&XMM0_b14,&XMM0_b13,&XMM0_b12,&XMM0_b11,&XMM0_b10,&XMM0_b9,&XMM0_b8,
        &XMM0_b7, &XMM0_b6, &XMM0_b5, &XMM0_b4, &XMM0_b3, &XMM0_b2, &XMM0_b1,&XMM0_b0,
        &XMM1_b15,&XMM1_b14,&XMM1_b13,&XMM1_b12,&XMM1_b11,&XMM1_b10,&XMM1_b9,&XMM1_b8,
        &XMM1_b7, &XMM1_b6, &XMM1_b5, &XMM1_b4, &XMM1_b3, &XMM1_b2, &XMM1_b1,&XMM1_b0,
        &XMM2_b15,&XMM2_b14,&XMM2_b13,&XMM2_b12,&XMM2_b11,&XMM2_b10,&XMM2_b9,&XMM2_b8,
        &XMM2_b7, &XMM2_b6, &XMM2_b5, &XMM2_b4, &XMM2_b3, &XMM2_b2, &XMM2_b1,&XMM2_b0,
        &XMM3_b15,&XMM3_b14,&XMM3_b13,&XMM3_b12,&XMM3_b11,&XMM3_b10,&XMM3_b9,&XMM3_b8,
        &XMM3_b7, &XMM3_b6, &XMM3_b5, &XMM3_b4, &XMM3_b3, &XMM3_b2, &XMM3_b1,&XMM3_b0,
        &XMM4_b15,&XMM4_b14,&XMM4_b13,&XMM4_b12,&XMM4_b11,&XMM4_b10,&XMM4_b9,&XMM4_b8,
        &XMM4_b7, &XMM4_b6, &XMM4_b5, &XMM4_b4, &XMM4_b3, &XMM4_b2, &XMM4_b1,&XMM4_b0,
        &XMM5_b15,&XMM5_b14,&XMM5_b13,&XMM5_b12,&XMM5_b11,&XMM5_b10,&XMM5_b9,&XMM5_b8,
        &XMM5_b7, &XMM5_b6, &XMM5_b5, &XMM5_b4, &XMM5_b3, &XMM5_b2, &XMM5_b1,&XMM5_b0,
        &XMM6_b15,&XMM6_b14,&XMM6_b13,&XMM6_b12,&XMM6_b11,&XMM6_b10,&XMM6_b9,&XMM6_b8,
        &XMM6_b7, &XMM6_b6, &XMM6_b5, &XMM6_b4, &XMM6_b3, &XMM6_b2, &XMM6_b1,&XMM6_b0,
        &XMM7_b15,&XMM7_b14,&XMM7_b13,&XMM7_b12,&XMM7_b11,&XMM7_b10,&XMM7_b9,&XMM7_b8,
        &XMM7_b7, &XMM7_b6, &XMM7_b5, &XMM7_b4, &XMM7_b3, &XMM7_b2, &XMM7_b1,&XMM7_b0,
    };
    static const x86_reg_info *list_word[] = {
        &XMM0_w7,&XMM0_w6,&XMM0_w5,&XMM0_w4,&XMM0_w3,&XMM0_w2,&XMM0_w1,&XMM0_w0,
        &XMM1_w7,&XMM1_w6,&XMM1_w5,&XMM1_w4,&XMM1_w3,&XMM1_w2,&XMM1_w1,&XMM1_w0,
        &XMM2_w7,&XMM2_w6,&XMM2_w5,&XMM2_w4,&XMM2_w3,&XMM2_w2,&XMM2_w1,&XMM2_w0,
        &XMM3_w7,&XMM3_w6,&XMM3_w5,&XMM3_w4,&XMM3_w3,&XMM3_w2,&XMM3_w1,&XMM3_w0,
        &XMM4_w7,&XMM4_w6,&XMM4_w5,&XMM4_w4,&XMM4_w3,&XMM4_w2,&XMM4_w1,&XMM4_w0,
        &XMM5_w7,&XMM5_w6,&XMM5_w5,&XMM5_w4,&XMM5_w3,&XMM5_w2,&XMM5_w1,&XMM5_w0,
        &XMM6_w7,&XMM6_w6,&XMM6_w5,&XMM6_w4,&XMM6_w3,&XMM6_w2,&XMM6_w1,&XMM6_w0,
        &XMM7_w7,&XMM7_w6,&XMM7_w5,&XMM7_w4,&XMM7_w3,&XMM7_w2,&XMM7_w1,&XMM7_w0,
    };
    static const x86_reg_info *list_dword[] = {
        &XMM0_d3,&XMM0_d2,&XMM0_d1,&XMM0_d0,
        &XMM1_d3,&XMM1_d2,&XMM1_d1,&XMM1_d0,
        &XMM2_d3,&XMM2_d2,&XMM2_d1,&XMM2_d0,
        &XMM3_d3,&XMM3_d2,&XMM3_d1,&XMM3_d0,
        &XMM4_d3,&XMM4_d2,&XMM4_d1,&XMM4_d0,
        &XMM5_d3,&XMM5_d2,&XMM5_d1,&XMM5_d0,
        &XMM6_d3,&XMM6_d2,&XMM6_d1,&XMM6_d0,
        &XMM7_d3,&XMM7_d2,&XMM7_d1,&XMM7_d0,
    };
    static const x86_reg_info *list_qword[] = {
        &XMM0_q1,&XMM0_q0,
        &XMM1_q1,&XMM1_q0,
        &XMM2_q1,&XMM2_q0,
        &XMM3_q1,&XMM3_q0,
        &XMM4_q1,&XMM4_q0,
        &XMM5_q1,&XMM5_q0,
        &XMM6_q1,&XMM6_q0,
        &XMM7_q1,&XMM7_q0,
    };
    static const x86_reg_info *list1_mxcsr[] = {
        &XMM_mxcsr_ie,
        &XMM_mxcsr_de,
        &XMM_mxcsr_ze,
        &XMM_mxcsr_oe,
        &XMM_mxcsr_ue,
        &XMM_mxcsr_pe,
        &XMM_mxcsr_fz,
        &XMM_mxcsr_daz
    };
    static const x86_reg_info *list2_mxcsr[] = {
        &XMM_mxcsr_im,
        &XMM_mxcsr_dm,
        &XMM_mxcsr_zm,
        &XMM_mxcsr_om,
        &XMM_mxcsr_um,
        &XMM_mxcsr_pm,
        &XMM_mxcsr_rc,
        &XMM_mxcsr
    };
    #define T(t)        X86T_##t
    static const mad_type_handle type_select[2][2][6] = {
        { { T(UCHAR),T(USHORT),T(ULONG), T(U64),   T(FLOAT),T(DOUBLE) },   /* decimal, unsigned */
          { T(CHAR), T(SHORT), T(LONG),  T(I64),   T(FLOAT),T(DOUBLE) } }, /* decimal, signed */
        { { T(BYTE), T(WORD),  T(DWORD), T(QWORD), T(FLOAT),T(DOUBLE) },   /* hex, unsigned */
          { T(SBYTE),T(SWORD), T(SDWORD),T(SQWORD),T(FLOAT),T(DOUBLE) } }, /* hex, signed */
    };
    #undef T

    unsigned            row;
    unsigned            column;
    unsigned            group = 0;
//    unsigned            list_num = 0;
    unsigned            type = 0;
    const x86_reg_info  **list = NULL;


    mr = mr;
    /* in case we haven't got an XMM */
    *reg_p = NULL;
    *disp_type_p = X86T_UNKNOWN;

    *max_descript_p = 0;
    DescriptBuff[0] = '\0';
    *descript_p = DescriptBuff;
    *max_value_p = 0;
    if( ( MCSystemConfig()->cpu & X86_XMM ) == 0 )
        return( MS_FAIL );

    if( MADState->reg_state[XMM_REG_SET] & XT_BYTE ) {
        list = list_byte;
//        list_num = NUM_ELTS( list_byte );
        group = 16;
        type = 0;
    } else if( MADState->reg_state[XMM_REG_SET] & XT_WORD ) {
        list = list_word;
//        list_num = NUM_ELTS( list_word );
        group = 8;
        type = 1;
    } else if( MADState->reg_state[XMM_REG_SET] & XT_DWORD ) {
        list = list_dword;
//        list_num = NUM_ELTS( list_dword );
        group = 4;
        type = 2;
    } else if( MADState->reg_state[XMM_REG_SET] & XT_QWORD ) {
        list = list_qword;
//        list_num = NUM_ELTS( list_qword );
        group = 2;
        type = 3;
    } else if( MADState->reg_state[XMM_REG_SET] & XT_FLOAT ) {
        list = list_dword;
//        list_num = NUM_ELTS( list_dword );
        group = 4;
        type = 4;
    } else if( MADState->reg_state[XMM_REG_SET] & XT_DOUBLE ) {
        list = list_qword;
//        list_num = NUM_ELTS( list_qword );
        group = 2;
        type = 5;
    }
    row = piece / ( group + 2 );
    column = piece % ( group + 2 );
    if( row == 0 ) {
        if( column < group ) {
            *max_value_p = 2;
            *reg_p = &XXX_dummy.info;
            *disp_type_p = (mad_type_handle)( X86T_XMM_TITLE0 - 1 + group - piece );
        } else {
            *max_value_p = 0;
            *reg_p = NULL;
            *disp_type_p = 0;
        }
        return( MS_OK );
    }
    row--;
    if( row > 7 )
        return( MS_FAIL );
    if( column == 0 ) {
        /* first column */
        DescriptBuff[0] = 'X';
        DescriptBuff[1] = 'M';
        DescriptBuff[2] = 'M';
        DescriptBuff[3] = (char)row + '0';
        DescriptBuff[4] = '\0';
        *max_descript_p = 4;
    }
    if( ( column - group ) == 0 )  {
        *reg_p = &list1_mxcsr[ row ]->info;
        *descript_p = (*reg_p)->name;
        *disp_type_p = X86T_BIT;
        *max_value_p = 1;
    } else if( (column - group ) == 1 )  {
        *reg_p = &list2_mxcsr[ row ]->info;
        *descript_p = (*reg_p)->name;
        if( row == 6 ) {
            *disp_type_p = X86T_RC;
            *max_value_p = MODLEN( ModFPURc );
        } else if( row == 7 ) {
            *disp_type_p = X86T_DWORD;
        } else {
            *disp_type_p = X86T_BIT;
            *max_value_p = 1;
        }
    } else {
        *reg_p = &list[ row * group + column ]->info;
        *disp_type_p = type_select[( MADState->reg_state[XMM_REG_SET] & XT_HEX ) ? 1 : 0]
                              [( MADState->reg_state[XMM_REG_SET] & XT_SIGNED ) ? 1 : 0]
                              [type];

    }
    return( MS_OK );
}

mad_status MADIMPENTRY( RegSetDisplayGetPiece )(
    const mad_reg_set_data *rsd,
    const mad_registers *mr,
    unsigned piece,
    const char **descript_p,
    size_t *max_descript_p,
    const mad_reg_info **reg,
    mad_type_handle *disp_type,
    size_t *max_value )
{
    return( rsd->get_piece( mr, piece, descript_p, max_descript_p, reg, disp_type, max_value ) );
}

mad_status MADIMPENTRY( RegSetDisplayModify )(
    const mad_reg_set_data *rsd,
    const mad_reg_info *ri,
    const mad_modify_list **possible_p,
    int *num_possible_p )
{
    rsd = rsd;
    if( ri == &XXX_dummy.info )
        return( MS_FAIL );
    if( ri == &FPU_iptr.info )
        return( MS_FAIL );
    if( ri == &FPU_optr.info )
        return( MS_FAIL );

    if( ri == &FPU_pc.info ) {
        *possible_p = ModFPUPc;
        *num_possible_p = NUM_ELTS( ModFPUPc );
    } else if( ri == &FPU_rc.info ) {
        *possible_p = ModFPURc;
        *num_possible_p = NUM_ELTS( ModFPURc );
    } else if( ri == &XMM_mxcsr_rc.info ) {
        *possible_p = ModFPURc;
        *num_possible_p = NUM_ELTS( ModFPURc );
    } else if( ri == &FPU_ic.info ) {
        *possible_p = ModFPUIc;
        *num_possible_p = NUM_ELTS( ModFPUIc );
    } else if( ri == &FPU_st.info ) {
        *possible_p = ModFPUSt;
        *num_possible_p = NUM_ELTS( ModFPUSt );
    } else if( ri->type == X86T_EXTENDED ) {
        *possible_p = ModFPUStack;
        *num_possible_p = NUM_ELTS( ModFPUStack );
    } else if( ri->bit_start >= REG_BIT_OFF( u.fpu.tag )
            && ri->bit_start <  REG_BIT_OFF( u.fpu.tag ) + 16
            && ri->bit_size == 2 ) {
        *possible_p = ModFPUTag;
        *num_possible_p = NUM_ELTS( ModFPUTag );
    } else if( ri->bit_size == 1 ) {
        *possible_p = ModBit;
        *num_possible_p = NUM_ELTS( ModBit );
    } else if( ri->bit_size <= 8 ) {
        *possible_p = ModByte;
        *num_possible_p = NUM_ELTS( ModByte );
    } else if( ri->bit_size <= 16 ) {
        *possible_p = ModWord;
        *num_possible_p = NUM_ELTS( ModWord );
    } else if( ri->bit_size <= 32 ) {
        *possible_p = ModDWord;
        *num_possible_p = NUM_ELTS( ModDWord );
    } else {
        *possible_p = ModQWord;
        *num_possible_p = NUM_ELTS( ModQWord );
    }
    return( MS_OK );
}


static size_t FmtPtr( addr_seg seg, addr_off off, unsigned off_digits, char *buff, size_t buff_size )
{
    char        buff1[20];
    char        *p;
    size_t      len;

    p = buff1 + MCRadixPrefix( 16, buff1, sizeof( buff1 ) );
    p = CnvRadix( seg, 16, 'A', p, 4 );
    *p++ = ':';
    p += MCRadixPrefix( 16, p, sizeof( buff1 ) - (p - buff1) );
    p = CnvRadix( off, 16, 'A', p, off_digits );
    len = p - buff1;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, buff1, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

size_t RegDispType( mad_type_handle th, const void *d, char *buff, size_t buff_size )
{
    const mad_modify_list   *p = NULL;
    const fpu_ptr           *fp;
    char                    title[3];

    switch( th ) {
    case X86T_PC:       p = ModFPUPc; break;
    case X86T_RC:       p = ModFPURc; break;
    case X86T_IC:       p = ModFPUIc; break;
    case X86T_TAG:      p = ModFPUTag; break;
    case X86T_BIT:
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > 1 )
                buff_size = 1;
            if( buff_size > 0 )
                buff[0] = *(unsigned_8 *)d + '0';
            buff[buff_size] = '\0';
        }
        return( 1 );
    case X86T_FPPTR_REAL:
        fp = d;
        return( FmtPtr( (fp->r.low >> 4) + (fp->r.hi << 12), fp->r.low & 0xf, 4, buff, buff_size ) );
    case X86T_FPPTR_32:
        fp = d;
        return( FmtPtr( (addr_seg)fp->p.segment, fp->p.offset, 8, buff, buff_size ) );
    case X86T_FPPTR_16:
        fp = d;
        return( FmtPtr( (addr_seg)fp->p.segment, fp->p.offset & 0xffff, 4, buff, buff_size ) );
    case X86T_F10SPECIAL:
        return( MCString( MAD_MSTR_SPECIAL, buff, buff_size ) );
    case X86T_MMX_TITLE0:
    case X86T_MMX_TITLE1:
    case X86T_MMX_TITLE2:
    case X86T_MMX_TITLE3:
    case X86T_MMX_TITLE4:
    case X86T_MMX_TITLE5:
    case X86T_MMX_TITLE6:
    case X86T_MMX_TITLE7:
        if( MADState->reg_state[MMX_REG_SET] & MT_BYTE ) {
            title[0] = 'b';
        } else if( MADState->reg_state[MMX_REG_SET] & MT_WORD ) {
            title[0] = 'w';
        } else if( MADState->reg_state[MMX_REG_SET] & MT_DWORD ) {
            title[0] = 'd';
        } else {
            title[0] = 'q';
        }
        title[1] = (char)( th - X86T_MMX_TITLE0 ) + '0';
        title[2] = '\0';
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > 2 )
                buff_size = 2;
            memcpy( buff, title, buff_size );
            buff[buff_size] = '\0';
        }
        return( 2 );
    case X86T_XMM_TITLE0:
    case X86T_XMM_TITLE1:
    case X86T_XMM_TITLE2:
    case X86T_XMM_TITLE3:
    case X86T_XMM_TITLE4:
    case X86T_XMM_TITLE5:
    case X86T_XMM_TITLE6:
    case X86T_XMM_TITLE7:
    case X86T_XMM_TITLE8:
    case X86T_XMM_TITLE9:
    case X86T_XMM_TITLE10:
    case X86T_XMM_TITLE11:
    case X86T_XMM_TITLE12:
    case X86T_XMM_TITLE13:
    case X86T_XMM_TITLE14:
    case X86T_XMM_TITLE15:
        if( MADState->reg_state[XMM_REG_SET] & XT_BYTE ) {
            title[0] = 'b';
        } else if( MADState->reg_state[XMM_REG_SET] & XT_WORD ) {
            title[0] = 'w';
        } else if( MADState->reg_state[XMM_REG_SET] & XT_DWORD ) {
            title[0] = 'd';
        } else if( MADState->reg_state[XMM_REG_SET] & XT_QWORD ) {
            title[0] = 'q';
        } else if( MADState->reg_state[XMM_REG_SET] & XT_FLOAT ) {
            title[0] = 'd';
        } else if( MADState->reg_state[XMM_REG_SET] & XT_DOUBLE ) {
            title[0] = 'q';
        }
        if( th - X86T_XMM_TITLE0 > 9 ) {
            title[1] = (char)( ( th - X86T_XMM_TITLE0 ) / 10 ) + '0';
            title[2] = (char)( ( th - X86T_XMM_TITLE0 ) % 10 ) + '0';
        } else {
            title[1] = (char)( th - X86T_XMM_TITLE0 ) + '0';
            title[2] = ' ';
        }
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > 3 )
                buff_size = 3;
            memcpy( buff, title, buff_size );
            buff[buff_size] = '\0';
        }
        return( 3 );
    }
    return( MCString( p[*(unsigned_8 *)d].name, buff, buff_size ) );
}

mad_status MADIMPENTRY( RegModified )(
    const mad_reg_set_data *rsd,
    const mad_reg_info *ri,
    const mad_registers *old,
    const mad_registers *cur )
{
    unsigned        old_start;
    unsigned        cur_start;
    unsigned        size;
    int             rotation;
    address         addr;
    unsigned        mask;
    unsigned_8      *p_old;
    unsigned_8      *p_cur;
    int             st;
    unsigned        old_tag;
    unsigned        cur_tag;
    mad_status      unchanged;
    mad_disasm_data dd;

    rsd = rsd;
    if( old == cur )
        return( MS_OK );
    cur_start = ri->bit_start;
    if( cur_start >= REG_BIT_OFF( cpu.eip ) && cur_start < REG_BIT_OFF( cpu.eip ) + 32 ) {
        /* dealing with [E]IP - check for control transfer */
        addr = GetRegIP( old );
        DecodeIns( &addr, &dd, BIG_SEG( addr ) );
        if( addr.mach.offset != cur->x86.cpu.eip )
            return( MS_MODIFIED_SIGNIFICANTLY );
        if( old->x86.cpu.eip != cur->x86.cpu.eip )
            return( MS_MODIFIED );
        return( MS_OK );
    }
    unchanged = MS_OK;
    old_start = cur_start;
    if( rsd - RegSet == FPU_REG_SET ) {
        if( cur_start >= STK_REG_BIT_OFF( 0 ) && cur_start <  STK_REG_BIT_OFF( 8 ) ) {
            /* dealing with a 87 stack register - handle stack rotation */
            st = GET_STK_REG_IDX( cur_start );
            rotation = EXTRACT_ST( old ) - EXTRACT_ST( cur );
            old_tag = GetTag( old, st - rotation );
            cur_tag = GetTag( cur, st );
            if( old_tag == cur_tag ) {
                if( rotation != 0 ) {
                    old_start -= rotation * sizeof( xreal ) * 8;
                    if( old_start < STK_REG_BIT_OFF( 0 ) ) {
                        old_start += sizeof( xreal ) * 8 * 8;
                    } else if( old_start >= STK_REG_BIT_OFF( 8 ) ) {
                        old_start -= sizeof( xreal ) * 8 * 8;
                    }
                }
                if( cur_tag != TAG_EMPTY ) {
                    unchanged = MS_MODIFIED;
                }
            } else if( cur_tag == TAG_EMPTY ) {
                return( MS_MODIFIED );
            } else {
                return( MS_MODIFIED_SIGNIFICANTLY );
            }
        } else if( cur_start >= REG_BIT_OFF( u.fpu.tag ) && cur_start < REG_BIT_OFF( u.fpu.tag ) + 16 ) {
            /* dealing with the 87 tag register - handle stack rotation */
            rotation = EXTRACT_ST( old ) - EXTRACT_ST( cur );
            if( rotation != 0 ) {
                old_start -= rotation * 2;
                if( old_start < REG_BIT_OFF( u.fpu.tag ) ) {
                    old_start += 16;
                } else if( old_start >= REG_BIT_OFF( u.fpu.tag ) + 16 ) {
                    old_start -= 16;
                }
                st = ( cur_start - REG_BIT_OFF( u.fpu.tag ) ) / 2;
                old_tag = GetTag( old, st );
                cur_tag = GetTag( cur, st );
                if( old_tag != cur_tag ) {
                    unchanged = MS_MODIFIED;
                }
            }
        }
    }
    p_old = (unsigned_8 *)old + (old_start / 8);
    p_cur = (unsigned_8 *)cur + (cur_start / 8);
    size = ri->bit_size;
    if( size >= 8 ) {
        /* it's going to be byte aligned */
        return( memcmp( p_old, p_cur, size / 8 ) != 0 ? MS_MODIFIED_SIGNIFICANTLY : unchanged );
    } else {
        mask = (1 << size) - 1;
        #define GET_VAL( w ) (((*p_##w >> (w##_start % 8))) & mask)
        return( GET_VAL( old ) != GET_VAL( cur ) ? MS_MODIFIED_SIGNIFICANTLY : unchanged );
    }
}

mad_status MADIMPENTRY( RegInspectAddr )( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    unsigned    bit_start;
    unsigned_32 *p;

    bit_start = ri->bit_start;
    if( bit_start >= REG_BIT_OFF( u.fpu ) ) {
        return( MS_FAIL );
    }
    if( bit_start >= REG_BIT_OFF( cpu.efl ) && bit_start < REG_BIT_OFF( cpu.efl ) + 32 ) {
        return( MS_FAIL );
    }
    if( bit_start == REG_BIT_OFF( cpu.eip ) ) {
        *a = GetRegIP( mr );
        return( MS_OK );
    }
    if( bit_start == REG_BIT_OFF( cpu.esp ) ) {
        *a = GetRegSP( mr );
        return( MS_OK );
    }
    if( bit_start == REG_BIT_OFF( cpu.ebp ) ) {
        *a = GetRegFP( mr );
        return( MS_OK );
    }
    memset( a, 0, sizeof( *a ) );
    if( bit_start == REG_BIT_OFF( cpu.cs ) ) {
        a->mach.segment = mr->x86.cpu.cs;
    } else if( bit_start == REG_BIT_OFF( cpu.ds ) ) {
        a->mach.segment = mr->x86.cpu.ds;
    } else if( bit_start == REG_BIT_OFF( cpu.es ) ) {
        a->mach.segment = mr->x86.cpu.es;
    } else if( bit_start == REG_BIT_OFF( cpu.es ) ) {
        a->mach.segment = mr->x86.cpu.es;
    } else if( bit_start == REG_BIT_OFF( cpu.ss ) ) {
        a->mach.segment = mr->x86.cpu.ss;
    } else if( bit_start == REG_BIT_OFF( cpu.fs ) ) {
        a->mach.segment = mr->x86.cpu.fs;
    } else if( bit_start == REG_BIT_OFF( cpu.gs ) ) {
        a->mach.segment = mr->x86.cpu.gs;
    } else {
        p = (unsigned_32 *)((unsigned_8 *)mr + BYTEIDX( bit_start ));
        a->mach.offset = *p;
        a->mach.segment = mr->x86.cpu.ds;
        if( ri->bit_size == 16 ) {
            a->mach.offset &= 0xffff;
        }
    }
    MCAddrSection( a );
    return( MS_OK );
}

const mad_toggle_strings *MADIMPENTRY( RegSetDisplayToggleList )( const mad_reg_set_data *rsd )
{
    return( rsd->togglelist );
}

unsigned MADIMPENTRY( RegSetDisplayToggle )( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    unsigned    toggle;
    unsigned    save;
    unsigned    save_on;
    unsigned    save_off;
    unsigned    mask;
    int         index;

    index = (unsigned)( rsd - RegSet );
    toggle = on & off;
    save_on = 0;
    switch( index ) {
    case MMX_REG_SET:
        mask = MT_HEX | MT_SIGNED;
        break;
    case XMM_REG_SET:
        mask = XT_HEX | XT_SIGNED;
        break;
    default:
        mask = 0;
        break;
    }
    if( mask ) {
        save_on = on & ~mask;
        on &= mask;
        off &= mask;
        toggle &= mask;
    }
    MADState->reg_state[index] ^= toggle;
    MADState->reg_state[index] |= on & ~toggle;
    MADState->reg_state[index] &= ~off | toggle;
    if( save_on ) {
        /* only allow one bit to turn on */
        save_on &= ~(save_on & (save_on-1));
        save_off = ~save_on & ~mask;
        toggle = save_on & save_off;
        save = MADState->reg_state[index];
        MADState->reg_state[index] ^= toggle;
        MADState->reg_state[index] |= save_on & ~toggle;
        MADState->reg_state[index] &= ~save_off | toggle;
        if( (MADState->reg_state[index] & ~mask) == 0 ) {
            MADState->reg_state[index] = save;
        }
    }
    return( MADState->reg_state[index] );
}

walk_result MADIMPENTRY( RegWalk )(
    const mad_reg_set_data *rsd,
    const mad_reg_info *ri,
    MI_REG_WALKER *wk,
    void *d )
{
    const x86_reg_info  *const *list;
    const x86_reg_info  *reg;
    processor_level     cpulevel;
    processor_level     fpulevel;
    walk_result         wr;
    unsigned            level;

    list = ( ri == NULL ) ? rsd->reglist : ((const x86_reg_info *)ri)->sublist;
    if( list == NULL )
        return( WR_CONTINUE );
    level = MCSystemConfig()->cpu & X86_CPU_MASK;
    if( level >= X86_686 ) {
        cpulevel = L6;
    } else if( level >= X86_586 ) {
        cpulevel = L5;
    } else if( level >= X86_386 ) {
        cpulevel = L3;
    } else if( level >= X86_286 ) {
        cpulevel = L2;
    } else {
        cpulevel = L1;
    }
    level = MCSystemConfig()->fpu;
    if( level >= X86_387 ) {
        fpulevel = L3;
    } else if( level >= X86_287 ) {
        fpulevel = L2;
    } else if( level != X86_NO ) {
        fpulevel = L1;
    } else {
        fpulevel = LN;
    }
    for( ; (reg = *list) != NULL; ++list ) {
        if( (processor_level)reg->cpulevel <= cpulevel || (processor_level)reg->fpulevel <= fpulevel ) {
            wr = wk( &reg->info, reg->sublist != NULL, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
    }
    return( WR_CONTINUE );
}

address GetRegSP( const mad_registers *mr )
{
    address     a;

    a.mach.segment = mr->x86.cpu.ss;
    a.mach.offset  = mr->x86.cpu.esp;
    MCAddrSection( &a );
    return( a );
}

address GetRegFP( const mad_registers *mr )
{
    address     a;

    a.mach.segment = mr->x86.cpu.ss;
    a.mach.offset  = mr->x86.cpu.ebp;
    MCAddrSection( &a );
    return( a );
}

address GetRegIP( const mad_registers *mr )
{
    address     a;

    a.mach.segment = mr->x86.cpu.cs;
    a.mach.offset  = mr->x86.cpu.eip;
    MCAddrSection( &a );
    return( a );
}

void MADIMPENTRY( RegSpecialGet )( mad_special_reg sr, const mad_registers *mr, addr_ptr *ma )
{
    switch( sr ) {
    case MSR_IP:
        ma->segment = mr->x86.cpu.cs;
        ma->offset  = mr->x86.cpu.eip;
        break;
    case MSR_SP:
        ma->segment = mr->x86.cpu.ss;
        ma->offset  = mr->x86.cpu.esp;
        break;
    case MSR_FP:
        ma->segment = mr->x86.cpu.ss;
        ma->offset  = mr->x86.cpu.ebp;
        break;
    }
}

void MADIMPENTRY( RegSpecialSet )( mad_special_reg sr, mad_registers *mr, const addr_ptr *ma )
{
    switch( sr ) {
    case MSR_IP:
        mr->x86.cpu.cs  = ma->segment;
        mr->x86.cpu.eip = ma->offset;
        break;
    case MSR_SP:
        mr->x86.cpu.ss  = ma->segment;
        mr->x86.cpu.esp = ma->offset;
        break;
    case MSR_FP:
        mr->x86.cpu.ss  = ma->segment;
        mr->x86.cpu.ebp = ma->offset;
        break;
    }
}

size_t MADIMPENTRY( RegSpecialName )(
    mad_special_reg sr,
    const mad_registers *mr,
    mad_address_format af,
    char *buff,
    size_t buff_size )
{
    const char  *seg;
    const char  *offset;
    size_t      len;
    size_t      left;
    size_t      amount;

    switch( sr ) {
    case MSR_IP:
        seg = CPU_cs.info.name;
        offset = BIG_SEG( GetRegIP( mr ) ) ? CPU_eip.info.name : CPU_ip.info.name;
        break;
    case MSR_SP:
        seg = CPU_ss.info.name;
        offset = BIG_SEG( GetRegSP( mr ) ) ? CPU_esp.info.name : CPU_sp.info.name;
        break;
    case MSR_FP:
        seg = CPU_ss.info.name;
        offset = BIG_SEG( GetRegIP( mr ) ) ? CPU_ebp.info.name : CPU_bp.info.name;
        break;
    default:
        seg = NULL;
        offset = NULL;
    }
    len = 0;
    left = buff_size;
    if( left > 0 )
        --left;
    if( af == MAF_FULL ) {
        len = strlen( seg );
        amount = len;
        if( amount > left )
            amount = left;
        memcpy( buff, seg, amount );
        buff += amount;
        left -= amount;
        if( left > 0 ) {
            *buff++ = ':';
            --left;
            ++len;
        }
    }
    amount = strlen( offset );
    len += amount;
    if( amount > left )
        amount = left;
    memcpy( buff, offset, amount );
    buff += amount;
    left -= amount;
    if( buff_size > 0 )
        *buff = '\0';
    return( len );
}


const mad_reg_info *MADIMPENTRY( RegFromContextItem )( context_item ci )
{
    static const mad_reg_info *list[] = {
        &CPU_eax.info,
        &CPU_ebx.info,
        &CPU_ecx.info,
        &CPU_edx.info,
        &CPU_esi.info,
        &CPU_edi.info,
        &CPU_ebp.info,
        &CPU_esp.info,
        &CPU_efl.info,
        &CPU_eip.info,
        &CPU_ds.info,
        &CPU_es.info,
        &CPU_ss.info,
        &CPU_cs.info,
        &CPU_fs.info,
        &CPU_gs.info,
        &FPU_st0.info,
        &FPU_st1.info,
        &FPU_st2.info,
        &FPU_st3.info,
        &FPU_st4.info,
        &FPU_st5.info,
        &FPU_st6.info,
        &FPU_st7.info,
        &FPU_sw.info,
        &FPU_cw.info
    };

    return( list[ci-CI_EAX] );
}

void MADIMPENTRY( RegUpdateStart )( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    mr = mr;
    flags = flags;
    bit_start = bit_start;
    bit_size = bit_size;
}

void MADIMPENTRY( RegUpdateEnd )( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    bit_size = bit_size;
    if( flags & RF_GPREG )
        MCNotify( MNT_MODIFY_REG, (void *)&RegSet[CPU_REG_SET] );
    if( flags & (RF_FPREG|RF_MMREG) ) {
        MCNotify( MNT_MODIFY_REG, (void *)&RegSet[FPU_REG_SET] );
        MCNotify( MNT_MODIFY_REG, (void *)&RegSet[MMX_REG_SET] );
    }
    if( flags & RF_XMMREG )
        MCNotify( MNT_MODIFY_REG, (void *)&RegSet[XMM_REG_SET] );
    switch( bit_start ) {
    case REG_BIT_OFF( cpu.eip ):
    case REG_BIT_OFF( cpu.cs ):
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case REG_BIT_OFF( cpu.esp ):
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case REG_BIT_OFF( cpu.ebp ):
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    case REG_BIT_OFF( cpu.ss ):
        MCNotify( MNT_MODIFY_SP, NULL );
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    case REG_BIT_OFF( u.fpu.tag ) + 0:
    case REG_BIT_OFF( u.fpu.tag ) + 2:
    case REG_BIT_OFF( u.fpu.tag ) + 4:
    case REG_BIT_OFF( u.fpu.tag ) + 6:
    case REG_BIT_OFF( u.fpu.tag ) + 8:
    case REG_BIT_OFF( u.fpu.tag ) + 10:
    case REG_BIT_OFF( u.fpu.tag ) + 12:
    case REG_BIT_OFF( u.fpu.tag ) + 14:
        MCNotify( MNT_REDRAW_REG, (void *)&RegSet[FPU_REG_SET] );
        break;
    case STK_REG_BIT_OFF( 0 ):
    case STK_REG_BIT_OFF( 1 ):
    case STK_REG_BIT_OFF( 2 ):
    case STK_REG_BIT_OFF( 3 ):
    case STK_REG_BIT_OFF( 4 ):
    case STK_REG_BIT_OFF( 5 ):
    case STK_REG_BIT_OFF( 6 ):
    case STK_REG_BIT_OFF( 7 ):
        if( flags & RF_FPREG ) {
            unsigned    index;

            index = GET_STK_REG_IDX( bit_start );
            SetTag( mr, index, TAG_VALID );
        }
        break;
    }
}

mad_status RegInit( void )
{
    return( MS_OK );
}

void RegFini( void )
{
}
