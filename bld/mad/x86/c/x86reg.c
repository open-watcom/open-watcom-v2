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


#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "x86.h"
#include "x86types.h"
#include "madregs.h"

#define NUM_ELTS( name ) (sizeof( name ) / sizeof( name[0] ) )

typedef enum {
    RF_NONE,
    RF_GPREG = 0x1,
    RF_FPREG = 0x2,
    RF_MMREG = 0x4,
} register_flags;

#define REG( p, name, type, start, size, flags, sublist, cpulevel, fpulevel ) \
    const x86_reg_info p##_##name = { #name, X86T_##type, start, size,    \
                                    RF_##flags##REG, sublist, cpulevel, fpulevel };

#define CPU( name, type, basereg, startbit, size, sublist, cpulevel )   \
    REG( CPU, name, type, offsetof( mad_registers, x86.cpu.basereg )*8+startbit,        \
        size, GP, sublist, cpulevel, LX )

#define CPU_FLAG( name, cpulevel )      \
    CPU( name, BYTE, efl, SHIFT_##name, LEN_##name, NULL, cpulevel )

#define FPU( name, type, basereg, startbit, size, sublist, fpulevel )   \
    REG( FPU, name, type, offsetof( mad_registers, x86.fpu.basereg )*8+startbit,        \
        size, FP, sublist, LX, fpulevel )

#define FPU_STK( num )  \
    FPU( st##num, EXTENDED, reg[num], 0, 80, NULL, L1 )

#define FPU_SW( name, fpulevel )        \
    FPU( name, BYTE, sw, SHIFT_##name, LEN_##name, NULL, fpulevel )

#define FPU_CW( name, fpulevel )        \
    FPU( name, BYTE, cw, SHIFT_##name, LEN_##name, NULL, fpulevel )

#define MMX( num )      \
    REG( MMX, mm##num, U64, offsetof( mad_registers, x86.mmx.mm[num] )*8,\
    64, MM, MMXSubList##num, L5, LX )


/* forward definitions */
extern const x86_reg_info       * const EFLRegList[];
extern const x86_reg_info       * const SWRegList[];
extern const x86_reg_info       * const CWRegList[];

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

FPU_STK( 0 );
FPU_STK( 1 );
FPU_STK( 2 );
FPU_STK( 3 );
FPU_STK( 4 );
FPU_STK( 5 );
FPU_STK( 6 );
FPU_STK( 7 );

FPU( sw,  WORD, sw,  0, 16, SWRegList, L1 );
FPU( cw,  WORD, cw,  0, 16, CWRegList, L1 );
FPU( tag, WORD, tag, 0, 16, NULL, L1 );
FPU( tag0, BIT, tag, 0,  2, NULL, L1 );
FPU( tag1, BIT, tag, 2,  2, NULL, L1 );
FPU( tag2, BIT, tag, 4,  2, NULL, L1 );
FPU( tag3, BIT, tag, 6,  2, NULL, L1 );
FPU( tag4, BIT, tag, 8,  2, NULL, L1 );
FPU( tag5, BIT, tag,10,  2, NULL, L1 );
FPU( tag6, BIT, tag,12,  2, NULL, L1 );
FPU( tag7, BIT, tag,14,  2, NULL, L1 );
FPU( iptr, F32_PTR, ip_err, 0, 64, NULL, L1 );
FPU( optr, F32_PTR, op_err, 0, 64, NULL, L1 );

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

static const x86_reg_info * const EFLRegList[] = {
    &CPU_vm, &CPU_rf, &CPU_nt, &CPU_iopl,
    &CPU_o, &CPU_d, &CPU_i, &CPU_s, &CPU_z, &CPU_a, &CPU_p, &CPU_c,
NULL };

static const x86_reg_info * const FPURegList[] = {
    &FPU_st0, &FPU_st1, &FPU_st2, &FPU_st3,
    &FPU_st4, &FPU_st5, &FPU_st6, &FPU_st7,
    &FPU_sw,  &FPU_cw,  &FPU_tag,
NULL };

static const x86_reg_info * const SWRegList[] = {
    &FPU_ie, &FPU_de, &FPU_ze, &FPU_oe, &FPU_ue, &FPU_pe,
    &FPU_sf, &FPU_c0, &FPU_es, &FPU_c1, &FPU_c2, &FPU_st, &FPU_c3, &FPU_b,
NULL };

static const x86_reg_info * const CWRegList[] = {
    &FPU_im, &FPU_dm, &FPU_zm, &FPU_om, &FPU_um, &FPU_pm,
    &FPU_iem, &FPU_pc, &FPU_rc, &FPU_ic,
NULL };

#define X86T_b  X86T_BYTE
#define X86T_w  X86T_WORD
#define X86T_d  X86T_DWORD
#define MMXS_b  8
#define MMXS_w  16
#define MMXS_d  32

#define MMX_SUBREG( s, i, reg ) \
    REG( MMX##reg, s##i, s,     \
    offsetof( mad_registers, x86.mmx.mm[reg] )*8 + MMXS_##s * i,\
    MMXS_##s, MM, NULL, L5, LX )

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
                                \
        static const x86_reg_info * const MMXSubList##n[] = {           \
            &MMX##n##_b0, &MMX##n##_b1, &MMX##n##_b2, &MMX##n##_b3,     \
            &MMX##n##_b4, &MMX##n##_b5, &MMX##n##_b6, &MMX##n##_b7,     \
            &MMX##n##_w0, &MMX##n##_w1, &MMX##n##_w2, &MMX##n##_w3,     \
            &MMX##n##_d0, &MMX##n##_d1, NULL };

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


struct mad_reg_set_data {
    mad_string          name;
    const x86_reg_info  * const *reglist;
    const mad_toggle_strings    *togglelist;
    unsigned_8          grouping;
};

static const mad_toggle_strings CPUToggleList[] =
{
    {MSTR_MHEX,MSTR_HEX,MSTR_DECIMAL},
    {MSTR_MEXTENDED,MSTR_REG_EXTENDED,MSTR_REG_NORMAL},
    {MSTR_MOS,MSTR_REG_OS,MSTR_REG_USER},
    {MSTR_NIL,MSTR_NIL,MSTR_NIL}
};
static const mad_toggle_strings FPUToggleList[] =
{
    {MSTR_MHEX,MSTR_HEX,MSTR_DECIMAL},
    {MSTR_NIL,MSTR_NIL,MSTR_NIL}
};
static const mad_toggle_strings MMXToggleList[] =
{
    {MSTR_MHEX,MSTR_HEX,MSTR_DECIMAL},
    {MSTR_SIGNED,MSTR_SIGNED,MSTR_UNSIGNED},
    {MSTR_BYTE,MSTR_BYTE,MSTR_NIL},
    {MSTR_WORD,MSTR_WORD,MSTR_NIL},
    {MSTR_DWORD,MSTR_DWORD,MSTR_NIL},
    {MSTR_NIL,MSTR_NIL,MSTR_NIL}
};

#define FPU_GROUPING    6

static const mad_reg_set_data CPURegSet =
    { MSTR_CPU, CPURegList, CPUToggleList, 0 };
static const mad_reg_set_data FPURegSet =
    { MSTR_FPU, FPURegList, FPUToggleList, FPU_GROUPING };
static const mad_reg_set_data MMXRegSet =
    { MSTR_MMX, MMXRegList, MMXToggleList, 1 };

unsigned        DIGENTRY MIRegistersSize( void )
{
    return( sizeof( struct x86_mad_registers ) );
}


#define EXTRACT_ST( mr ) (((unsigned_16)(mr)->x86.fpu.sw >> SHIFT_st) & ((1<<LEN_st)-1))

mad_status      DIGENTRY MIRegistersHost( mad_registers *mr )
{
    unsigned    st;
    unsigned_16 tag;

    /* normalize the tag bits to tag[0..1] refer to ST(0), etc */
    st = EXTRACT_ST(mr)*2;
    tag = mr->x86.fpu.tag;
    tag = (tag >> (st)) | (tag << (16-st));
    mr->x86.fpu.tag &= ~0xffffUL;
    mr->x86.fpu.tag |= tag;
    return( MS_MODIFIED );
}

mad_status      DIGENTRY MIRegistersTarget( mad_registers *mr )
{
    unsigned    st;
    unsigned_16 tag;

    /* put the tag bits back the stupid way the x87 wants them */
    st = EXTRACT_ST(mr)*2;
    tag = mr->x86.fpu.tag;
    tag = (tag << (st)) | (tag >> (16-st));
    mr->x86.fpu.tag &= ~0xffffUL;
    mr->x86.fpu.tag |= tag;
    return( MS_MODIFIED );
}

walk_result     DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    walk_result wr;

    if( tk & (MTK_INTEGER|MTK_ADDRESS) ) {
        wr = wk( &CPURegSet, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    if( tk & MTK_FLOAT ) {
        wr = wk( &FPURegSet, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    if( (tk & MTK_CUSTOM) && (MCSystemConfig()->cpu >= X86_586) ) {
        wr = wk( &MMXRegSet, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    return( WR_CONTINUE );
}

mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

unsigned        DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, unsigned max, char *buff )
{
    char        str[80];
    unsigned    len;

    if( rsd == &CPURegSet ) {
        switch( MCSystemConfig()->cpu ) {
        case X86_86:
            strcpy( str, "8086" );
            break;
        case X86_586:
            strcpy( str, "Pentium" );
            break;
        case X86_686:
            strcpy( str, "Pentium Pro" );
            break;
        default:
            str[0] = MCSystemConfig()->cpu + '0';
            strcpy( &str[1], "86" );
            break;
        }
    } else if( rsd == &FPURegSet ) {
        switch( MCSystemConfig()->fpu ) {
        case X86_NO:
            MCString( MSTR_NONE, sizeof( str ), str );
            break;
        case X86_EMU:
            MCString( MSTR_EMULATOR, sizeof( str ), str );
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
        default:
            str[0] = MCSystemConfig()->fpu + '0';
            strcpy( &str[1], "87" );
            break;
        }
    } else {
        str[0] = '\0';
    }
    len = strlen( str );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, str, max );
        buff[max] = '\0';
    }
    return( len );
}

unsigned        DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    if( rsd != &MMXRegSet ) {
        return( rsd->grouping );
    } else if( MADState->mmx_toggles & MT_BYTE ) {
        return( 8 );
    } else if( MADState->mmx_toggles & MT_WORD ) {
        return( 4 );
    } else {
        return( 2 );
    }
}

static char     DescriptBuff[40];

static mad_status CPUGetPiece( const mad_registers *mr,
                                unsigned piece,
                                char **descript_p,
                                unsigned *max_descript_p,
                                const mad_reg_info **reg_p,
                                mad_type_handle *disp_type_p,
                                unsigned *max_value_p )
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
    const x86_reg_info  *curr;
    const x86_reg_info  **list;
    unsigned            list_num;
    char                *p;
    static const void   **last_list;

    if( (MADState->cpu_toggles & CT_EXTENDED) || BIG_SEG( GetRegIP( mr ) ) ) {
        list = list32;
        list_num = NUM_ELTS( list32 );
    } else {
        list = list16;
        list_num = NUM_ELTS( list16 );
    }
    if( last_list == NULL ) last_list = list;
    if( last_list != list ) {
        last_list = list;
        MCNotify( MNT_REDRAW_REG, (void *)&CPURegSet );
    }
    if( MADState->cpu_toggles & CT_OS ) {
        *max_descript_p = 4;
        if( piece >= list_num ) {
            piece -= list_num;
            if( piece >= NUM_ELTS( listOS ) ) return( MS_FAIL );
            list = listOS;
        }
    } else {
        *max_descript_p = 3;
        if( piece >= list_num ) return( MS_FAIL );
    }
    curr = list[ piece ];
    *max_value_p = 0;
    *reg_p = &curr->info;
    *descript_p = DescriptBuff;
    strcpy( DescriptBuff, curr->info.name );
    p = DescriptBuff;
    while( *p != '\0' ) {
        *p = toupper( *p );
        ++p;
    }
    *p = '\0';
    if( curr->info.bit_size <= 3 ) {
        *disp_type_p = X86T_BIT;
    } else if( curr->info.bit_size <= 16 ) {
        if( MADState->cpu_toggles & CT_HEX ) {
            *disp_type_p = X86T_WORD;
        } else {
            *disp_type_p = X86T_USHORT;
        }
    } else {
        if( MADState->cpu_toggles & CT_HEX ) {
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
    { &zero, X86T_BYTE, MSTR_NIL },
    { &one,  X86T_BYTE, MSTR_NIL },
};
static const mad_modify_list ModWord[] = {
    { NULL, X86T_WORD, MSTR_NIL },
};
static const mad_modify_list ModDWord[] = {
    { NULL, X86T_DWORD, MSTR_NIL },
};
static const mad_modify_list ModFPUTag[] = {
    { &zero,  X86T_BYTE, MSTR_VALID },
    { &one,   X86T_BYTE, MSTR_ZERO },
    { &two,   X86T_BYTE, MSTR_NAN },
    { &three, X86T_BYTE, MSTR_EMPTY },
};
static const mad_modify_list ModFPUStack[] = {
    { NULL, X86T_EXTENDED, MSTR_NIL },
};
static const mad_modify_list ModFPUSt[] = {
    { &zero, X86T_BIT, MSTR_NIL },
    { &one,  X86T_BIT, MSTR_NIL },
    { &two,  X86T_BIT, MSTR_NIL },
    { &three,X86T_BIT, MSTR_NIL },
    { &four, X86T_BIT, MSTR_NIL },
    { &five, X86T_BIT, MSTR_NIL },
    { &six,  X86T_BIT, MSTR_NIL },
    { &seven,X86T_BIT, MSTR_NIL },
};
static const mad_modify_list ModFPUPc[] = {
    { &zero,  X86T_BYTE, MSTR_PCSINGLE },
    { &one,   X86T_BYTE, MSTR_NIL },
    { &two,   X86T_BYTE, MSTR_PCDOUBLE },
    { &three, X86T_BYTE, MSTR_PCEXTENDED },
};
static const mad_modify_list ModFPURc[] = {
    { &zero,  X86T_BYTE, MSTR_NEAREST },
    { &one,   X86T_BYTE, MSTR_DOWN },
    { &two,   X86T_BYTE, MSTR_UP },
    { &three, X86T_BYTE, MSTR_CHOP },
};
static const mad_modify_list ModFPUIc[] = {
    { &zero,  X86T_BYTE, MSTR_PROJECTIVE },
    { &one,   X86T_BYTE, MSTR_AFFINE },
};

#define MODLEN( name ) MaxModLen( name, NUM_ELTS( name ) )

static unsigned MaxModLen( const mad_modify_list *list, unsigned num )
{
    unsigned    max;
    unsigned    len;

    max = 0;
    while( num != 0 ) {
        --num;
        len = MCString( list[num].name, 0, NULL );
        if( len > max ) max = len;
    }
    return( max );
}

#define LIST( num, r1, r2, r3, r4, r5, r6, r7, r8 )     \
    static const x86_reg_info * const list##num[]               \
    = { &FPU_##r1, &FPU_##r2, &FPU_##r3, &FPU_##r4,     \
        &FPU_##r5, &FPU_##r6, &FPU_##r7, &FPU_##r8 }

static x86_reg_info     XXX_dummy;

static mad_status FPUGetPiece( const mad_registers *mr,
                                unsigned piece,
                                char **descript_p,
                                unsigned *max_descript_p,
                                const mad_reg_info **reg_p,
                                mad_type_handle *disp_type_p,
                                unsigned *max_value_p )
{
    const static x86_reg_info   FPU_nil;
    LIST( 0, st0,  st1,  st2,  st3,  st4,  st5,  st6,  st7 );
    LIST( 1, tag0, tag1, tag2, tag3, tag4, tag5, tag6, tag7 );
    LIST( 2, ie,   de,   ze,   oe,   ue,   pe,   sf,   nil );
    LIST( 3, st,   c0,   c1,   c2,   c3,   nil,  nil,  nil );
    LIST( 4, im,   dm,   zm,   om,   um,   pm,   iem,  es  );
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
    if( row > 7 ) return( MS_FAIL );
    column = piece % FPU_GROUPING;
    switch( column ) {
    case 0: /* stack registers */
        *max_value_p = 28;
        *p++ = 'S';
        *p++ = 'T';
        *p++ = '(';
        *p++ = '0' + row;
        *p++ = ')';
        *p++ = '\0';
        if( MCSystemConfig()->fpu == X86_NO ) break;
        *reg_p = &list0[row]->info;
        if( MADState->fpu_toggles & FT_HEX ) {
            *disp_type_p = X86T_HEXTENDED;
        } else {
            tag = ((unsigned)mr->x86.fpu.tag >> (row*2)) & 0x3;
            switch( tag ) {
            case 0: /* valid */
            case 1: /* zero */
                *disp_type_p = X86T_EXTENDED;
                break;
            case 2: /* nan */
                *disp_type_p = X86T_F10NAN;
                break;
            case 3: /* empty */
                *disp_type_p = X86T_F10EMPTY;
                break;
            }
        }
        break;
    case 1: /* tag registers */
        if( MCSystemConfig()->fpu == X86_NO ) break;
        *p++ = 'T';
        *p++ = 'A';
        *p++ = 'G';
        *p++ = '(';
        *p++ = '0' + row;
        *p++ = ')';
        *p++ = '\0';
        *reg_p = &list1[row]->info;
        *disp_type_p = X86T_TAG;
        *max_value_p = MODLEN( ModFPUTag );
        break;
    case 2: /* status word bits 1 */
    case 3: /* status word bits 2 */
        *reg_p = (column==2) ? &list2[row]->info : &list3[row]->info;
        if( (*reg_p)->name == NULL ) {
            *reg_p = NULL;
            break;
        }
        strcpy( p, (*reg_p)->name );
        *max_value_p = 1;
        if( MCSystemConfig()->fpu == X86_NO ) break;
        *disp_type_p = X86T_BIT;
        break;
    case 4: /* control word bits */
        *reg_p = &list4[row]->info;
        strcpy( p, (*reg_p)->name );
        *max_value_p = 1;
        if( MCSystemConfig()->fpu == X86_NO ) break;
        *disp_type_p = X86T_BIT;
        break;
    case 5: /* misc */
        *reg_p = &list5[row]->info;
        switch( row ) {
        case 0:
            strcpy( p, "status" );
            if( MCSystemConfig()->fpu == X86_NO ) break;
            *max_value_p = 0;
            *disp_type_p = X86T_WORD;
            break;
        case 1:
            strcpy( p, "control" );
            if( MCSystemConfig()->fpu == X86_NO ) break;
            *max_value_p = 0;
            *disp_type_p = X86T_WORD;
            break;
        case 2:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPUPc );
            if( MCSystemConfig()->fpu == X86_NO ) break;
            *disp_type_p = X86T_PC;
            break;
        case 3:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPURc );
            if( MCSystemConfig()->fpu == X86_NO ) break;
            *disp_type_p = X86T_RC;
            break;
        case 4:
            strcpy( p, (*reg_p)->name );
            *max_value_p = MODLEN( ModFPUIc );
            if( MCSystemConfig()->fpu == X86_NO ) break;
            *disp_type_p = X86T_IC;
            break;
        case 5: /* iptr */
        case 6: /* optr */
            strcpy( p, (*reg_p)->name );
            if( MCSystemConfig()->fpu == X86_NO ) break;
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

static mad_status MMXGetPiece( const mad_registers *mr,
                                unsigned piece,
                                char **descript_p,
                                unsigned *max_descript_p,
                                const mad_reg_info **reg_p,
                                mad_type_handle *disp_type_p,
                                unsigned *max_value_p )
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
    #define T(t)        X86T_##t
    static const mad_type_handle type_select[2][2][3] = {
        /* decimal, unsigned */         /* decimal, signed */
        { {T(UCHAR),T(USHORT),T(ULONG)},{T(CHAR),T(SHORT),T(LONG)} },
        /* hex, unsigned */             /* hex, signed */
        { {T(BYTE),T(WORD),T(DWORD)},   {T(SBYTE),T(SWORD),T(SDWORD)} },
    };
    #undef T

    unsigned            group;
    unsigned            list_num;
    unsigned            type;
    const x86_reg_info  **list;


    /* in case we haven't got an MMX */
    *reg_p = NULL;
    *disp_type_p = X86T_UNKNOWN;

    *max_descript_p = 0;
    DescriptBuff[0] = '\0';
    *descript_p = DescriptBuff;
    *max_value_p = 0;
    if( MCSystemConfig()->cpu < X86_586 ) return( MS_FAIL );

    if( MADState->mmx_toggles & MT_BYTE ) {
        list = list_byte;
        list_num = NUM_ELTS( list_byte );
        group = 8;
        type = 0;
    } else if( MADState->mmx_toggles & MT_WORD ) {
        list = list_word;
        list_num = NUM_ELTS( list_word );
        group = 4;
        type = 1;
    } else {
        list = list_dword;
        list_num = NUM_ELTS( list_dword );
        group = 2;
        type = 2;
    }
    *disp_type_p = type_select[(MADState->mmx_toggles&MT_HEX) ? 1 : 0]
                                [(MADState->mmx_toggles&MT_SIGNED) ? 1 : 0]
                                [type];

    if( piece < group ) {
        *max_value_p = 2;
        *reg_p = &XXX_dummy.info;
        *disp_type_p = X86T_MMX_TITLE0 - 1 + group - piece;
        return( MS_OK );
    }
    piece -= group;

    if( piece >= list_num ) return( MS_FAIL );
    if( (piece % group) == 0 ) {
        /* first column */
        DescriptBuff[0] = 'M';
        DescriptBuff[1] = 'M';
        DescriptBuff[2] = (piece / group) + '0';
        DescriptBuff[3] = '\0';
        *max_descript_p = 3;
    }
    *reg_p = &list[piece]->info;
    return( MS_OK );
}

mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                const mad_registers *mr,
                                unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    if( rsd == &CPURegSet ) {
        return( CPUGetPiece( mr, piece, descript, max_descript, reg, disp_type, max_value ) );
    } else if( rsd == &FPURegSet ) {
        return( FPUGetPiece( mr, piece, descript, max_descript, reg, disp_type, max_value ) );
    } else {
        return( MMXGetPiece( mr, piece, descript, max_descript, reg, disp_type, max_value ) );
    }
}

mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, unsigned *num_possible_p )
{
    rsd = rsd;
    if( ri == &XXX_dummy.info ) return( MS_FAIL );
    if( ri == &FPU_iptr.info ) return( MS_FAIL );
    if( ri == &FPU_optr.info ) return( MS_FAIL );

    if( ri == &FPU_pc.info ) {
        *possible_p = ModFPUPc;
        *num_possible_p = NUM_ELTS( ModFPUPc );
    } else if( ri == &FPU_rc.info ) {
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
    } else if( ri->bit_start >= offsetof( mad_registers, x86.fpu.tag )*8
            && ri->bit_start <  offsetof( mad_registers, x86.fpu.tag )*8+16 ) {
        *possible_p = ModFPUTag;
        *num_possible_p = NUM_ELTS( ModFPUTag );
    } else if( ri->bit_size == 1 ) {
        *possible_p = ModBit;
        *num_possible_p = NUM_ELTS( ModBit );
    } else if( ri->bit_size <= 16 ) {
        *possible_p = ModWord;
        *num_possible_p = NUM_ELTS( ModWord );
    } else {
        *possible_p = ModDWord;
        *num_possible_p = NUM_ELTS( ModDWord );
    }
    return( MS_OK );
}


static unsigned FmtPtr( addr_seg seg, addr_off off, unsigned off_digits,
                        unsigned max, char *dst )
{
    char        buff[20];
    char        *p;
    unsigned    len;

    p = &buff[ MCRadixPrefix( 16, sizeof( buff ), buff ) ];
    p = CnvRadix( seg, 16, 'A', p, 4 );
    *p++ = ':';
    p += MCRadixPrefix( 16, sizeof( buff ) - (p - buff), p );
    p = CnvRadix( off, 16, 'A', p, off_digits );
    len = p - buff;
    if( max > 0 ) {
        --max;
        if( len > max ) max = len;
        memcpy( dst, buff, max );
        dst[max] = '\0';
    }
    return( len );
}

unsigned RegDispType( mad_type_handle th, const void *d, unsigned max, char *buff )
{
    const mad_modify_list       *p;
    const fpu_ptr       *fp;
    char                title[2];

    switch( th ) {
    case X86T_PC:       p = ModFPUPc; break;
    case X86T_RC:       p = ModFPURc; break;
    case X86T_IC:       p = ModFPUIc; break;
    case X86T_TAG:      p = ModFPUTag; break;
    case X86T_BIT:
        if( max > 0 ) {
            buff[0] = *(unsigned_8 *)d + '0';
            if( max > 1 ) max = 1;
            buff[max] = '\0';
        }
        return( 1 );
    case X86T_FPPTR_REAL:
        fp = d;
        return( FmtPtr( (fp->r.low >> 4) + (fp->r.hi << 12), fp->r.low & 0xf,
                        4, max, buff ) );
    case X86T_FPPTR_32:
        fp = d;
        return( FmtPtr( fp->p.segment, fp->p.offset, 8, max, buff ) );
    case X86T_FPPTR_16:
        fp = d;
        return( FmtPtr( fp->p.segment, fp->p.offset & 0xffff, 4, max, buff ) );
    case X86T_F10NAN:
        return( MCString( MSTR_NAN, max, buff ) );
    case X86T_MMX_TITLE0:
    case X86T_MMX_TITLE1:
    case X86T_MMX_TITLE2:
    case X86T_MMX_TITLE3:
    case X86T_MMX_TITLE4:
    case X86T_MMX_TITLE5:
    case X86T_MMX_TITLE6:
    case X86T_MMX_TITLE7:
        if( MADState->mmx_toggles & MT_BYTE ) {
            title[0] = 'b';
        } else if( MADState->mmx_toggles & MT_WORD ) {
            title[0] = 'w';
        } else {
            title[0] = 'd';
        }
        title[1] = th - X86T_MMX_TITLE0 + '0';
        if( max > 0 ) {
            --max;
            if( max > 2 ) max = 2;
            memcpy( buff, title, max );
            buff[max] = '\0';
        }
        return( 2 );
    }
    return( MCString( p[*(unsigned_8 *)d].name, max, buff ) );
}

static unsigned GetTag( const mad_registers *mr, int st )
{
    if( st >= 8 ) st -= 8;
    if( st <  0 ) st += 8;
    return( (mr->x86.fpu.tag >> (st*2)) & 0x03 );
}

mad_status DIGENTRY MIRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *cur )
{
    unsigned    old_start;
    unsigned    cur_start;
    unsigned    size;
    int         rotation;
    address     addr;
    unsigned    mask;
    unsigned_8  *p_old;
    unsigned_8  *p_cur;
    instruction ins;
    int         st;
    unsigned    old_tag;
    unsigned    cur_tag;
    mad_status  unchanged;

    rsd = rsd;
    if( old == cur ) return( MS_OK );
    cur_start = ri->bit_start;
    if( cur_start >= offsetof( mad_registers, x86.cpu.eip )*8
     && cur_start <  offsetof( mad_registers, x86.cpu.eip )*8 + 32 ) {
        /* dealing with [E]IP - check for control transfer */
        addr = GetRegIP( old );
        DecodeIns( &addr, &ins, BIG_SEG( addr ) );
        if( addr.mach.offset != cur->x86.cpu.eip ) return( MS_MODIFIED_SIGNIFICANTLY );
        if( old->x86.cpu.eip != cur->x86.cpu.eip ) return( MS_MODIFIED );
        return( MS_OK );
    }
    unchanged = MS_OK;
    old_start = cur_start;
    if( rsd == &FPURegSet ) {
        if( cur_start >= offsetof( mad_registers, x86.fpu.reg[0] )*8
         && cur_start <  offsetof( mad_registers, x86.fpu.reg[8] )*8 ) {
            /* dealing with a 87 stack register - handle stack rotation */
            st = (cur_start - offsetof( mad_registers, x86.fpu.reg[0])*8)
                    / (sizeof( cur->x86.fpu.reg[0]) * 8);
            rotation = EXTRACT_ST( old ) - EXTRACT_ST( cur );
            old_tag = GetTag( old, st - rotation );
            cur_tag = GetTag( cur, st );
            if( old_tag == cur_tag ) {
                if( rotation != 0 ) {
                    old_start -= rotation * sizeof( xreal )*8;
                    if( old_start < offsetof( mad_registers, x86.fpu.reg[0] )*8 ) {
                        old_start += sizeof( xreal )*8*8;
                    } else if( old_start >= (offsetof( mad_registers, x86.fpu.reg[7] ) + sizeof( xreal ))*8 ) {
                        old_start -= sizeof( xreal )*8*8;
                    }
                }
                if( cur_tag != TAG_EMPTY ) unchanged = MS_MODIFIED;
            } else if( cur_tag == TAG_EMPTY ) {
                return( MS_MODIFIED );
            } else {
                return( MS_MODIFIED_SIGNIFICANTLY );
            }
        } else if( cur_start >= offsetof( mad_registers, x86.fpu.tag )*8
                && cur_start <  offsetof( mad_registers, x86.fpu.tag )*8 + 16 ) {
            /* dealing with the 87 tag register - handle stack rotation */
            rotation = EXTRACT_ST( old ) - EXTRACT_ST( cur );
            if( rotation != 0 ) {
                old_start -= rotation * 2;
                if( old_start < offsetof( mad_registers, x86.fpu.tag )*8 ) {
                    old_start += 16;
                } else if( old_start >= offsetof( mad_registers, x86.fpu.tag )*8 + 16 ) {
                    old_start -= 16;
                }
                st = (cur_start - offsetof( mad_registers, x86.fpu.tag)*8) / 2;
                old_tag = GetTag( old, st );
                cur_tag = GetTag( cur, st );
                if( old_tag != cur_tag ) unchanged = MS_MODIFIED;
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

mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    unsigned    bit_start;
    unsigned_32 *p;

    bit_start = ri->bit_start;
    if( bit_start >= offsetof( mad_registers, x86.fpu ) * 8 ) {
        return( MS_FAIL );
    }
    if( bit_start >= offsetof( mad_registers, x86.cpu.efl )*8
     && bit_start <  offsetof( mad_registers, x86.cpu.efl )*8 + 32 ) {
        return( MS_FAIL );
    }
    if( bit_start == offsetof( mad_registers, x86.cpu.eip )*8 ) {
        *a = GetRegIP( mr );
        return( MS_OK );
    }
    if( bit_start == offsetof( mad_registers, x86.cpu.esp )*8 ) {
        *a = GetRegSP( mr );
        return( MS_OK );
    }
    if( bit_start == offsetof( mad_registers, x86.cpu.ebp )*8 ) {
        *a = GetRegFP( mr );
        return( MS_OK );
    }
    memset( a, 0, sizeof( *a ) );
    if( bit_start == offsetof( mad_registers, x86.cpu.cs )*8 ) {
        a->mach.segment = mr->x86.cpu.cs;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.ds )*8 ) {
        a->mach.segment = mr->x86.cpu.ds;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.es )*8 ) {
        a->mach.segment = mr->x86.cpu.es;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.es )*8 ) {
        a->mach.segment = mr->x86.cpu.es;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.ss )*8 ) {
        a->mach.segment = mr->x86.cpu.ss;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.fs )*8 ) {
        a->mach.segment = mr->x86.cpu.fs;
    } else if( bit_start == offsetof( mad_registers, x86.cpu.gs )*8 ) {
        a->mach.segment = mr->x86.cpu.gs;
    } else {
        p = (unsigned_32 *)((unsigned_8 *)mr + (bit_start / BITS_PER_BYTE));
        a->mach.offset = *p;
        a->mach.segment = mr->x86.cpu.ds;
        if( ri->bit_size == 16 ) {
            a->mach.offset &= 0xffff;
        }
    }
    MCAddrSection( a );
    return( MS_OK );
}

const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    return( rsd->togglelist );
}

unsigned        DIGENTRY MIRegSetDisplayToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    unsigned    toggle;
    unsigned    save;
    unsigned    save_on;
    unsigned    save_off;

    toggle = on & off;
    if( rsd == &CPURegSet ) {
        MADState->cpu_toggles ^= toggle;
        MADState->cpu_toggles |= on & ~toggle;
        MADState->cpu_toggles &= ~off | toggle;
        return( MADState->cpu_toggles );
    } else if( rsd == &FPURegSet ) {
        MADState->fpu_toggles ^= toggle;
        MADState->fpu_toggles |= on & ~toggle;
        MADState->fpu_toggles &= ~off | toggle;
        return( MADState->fpu_toggles );
    } else {
        #define TBITS   (MT_HEX|MT_SIGNED)
        save_on = on & ~TBITS;
        on &= TBITS;
        off &= TBITS;
        toggle &= TBITS;
        MADState->mmx_toggles ^= toggle;
        MADState->mmx_toggles |= on & ~toggle;
        MADState->mmx_toggles &= ~off | toggle;
        if( save_on != 0 ) {
            /* only allow one bit to turn on */
            save_on &= ~(save_on & (save_on-1));
            save_off = ~save_on & ~TBITS;
            toggle = save_on & save_off;
            save = MADState->mmx_toggles;
            MADState->mmx_toggles ^= toggle;
            MADState->mmx_toggles |= save_on & ~toggle;
            MADState->mmx_toggles &= ~save_off | toggle;
            if( (MADState->mmx_toggles & ~TBITS) == 0 ) MADState->mmx_toggles = save;
        }
        return( MADState->mmx_toggles );
    }
}

walk_result     DIGENTRY MIRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    const x86_reg_info  *const *list;
    const x86_reg_info  *reg;
    processor_level     cpulevel;
    processor_level     fpulevel;
    walk_result         wr;
    unsigned            level;

    list = (ri==NULL) ? rsd->reglist : ((const x86_reg_info *)ri)->sublist;
    if( list == NULL ) return( WR_CONTINUE );
    level = MCSystemConfig()->cpu;
    if( level >= X86_586 ) {
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
    for( ;; ) {
        reg = *list;
        if( reg == NULL ) break;
        if( reg->cpulevel <= cpulevel || reg->fpulevel <= fpulevel ) {
            wr = wk( &reg->info, reg->sublist != NULL, d );
            if( wr != WR_CONTINUE ) return( wr );
        }
        ++list;
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

void            DIGENTRY MIRegSpecialGet( mad_special_reg sr, const mad_registers *mr, addr_ptr *ma )
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

void            DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *ma )
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

unsigned        DIGENTRY MIRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, unsigned max, char *buff )
{
    const char  *seg;
    const char  *offset;
    unsigned    len;
    unsigned    left;
    unsigned    amount;

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
    }
    len = 0;
    left = max;
    if( left > 0 ) --left;
    if( af == MAF_FULL ) {
        len = strlen( seg );
        amount = len;
        if( amount > left ) amount = left;
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
    if( amount > left ) amount = left;
    memcpy( buff, offset, amount );
    buff += amount;
    left -= amount;
    if( max > 0 ) *buff = '\0';
    return( len );
}


const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
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
        &FPU_cw.info,
    };

    return( list[ci-CI_EAX] );
}

void            DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    mr = mr;
    flags = flags;
    bit_start = bit_start;
    bit_size = bit_size;
}

void            DIGENTRY MIRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    bit_size = bit_size;
    if( flags & RF_GPREG ) MCNotify( MNT_MODIFY_REG, (void *)&CPURegSet );
    if( flags & (RF_FPREG|RF_MMREG) ) {
        MCNotify( MNT_MODIFY_REG, (void *)&FPURegSet );
        MCNotify( MNT_MODIFY_REG, (void *)&MMXRegSet );
    }
    switch( bit_start ) {
    case offsetof( mad_registers, x86.cpu.eip ) * 8:
    case offsetof( mad_registers, x86.cpu.cs  ) * 8:
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case offsetof( mad_registers, x86.cpu.esp ) * 8:
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case offsetof( mad_registers, x86.cpu.ebp ) * 8:
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    case offsetof( mad_registers, x86.cpu.ss  ) * 8:
        MCNotify( MNT_MODIFY_SP, NULL );
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 0:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 2:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 4:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 6:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 8:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 10:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 12:
    case offsetof( mad_registers, x86.fpu.tag ) * 8 + 14:
        MCNotify( MNT_REDRAW_REG, (void *)&FPURegSet );
        break;
    case offsetof( mad_registers, x86.fpu.reg[0] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[1] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[2] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[3] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[4] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[5] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[6] ) * 8:
    case offsetof( mad_registers, x86.fpu.reg[7] ) * 8:
        if( flags & RF_FPREG ) {
            unsigned    index;
            word        shift;
            word        mask;

            index = bit_start - (offsetof( mad_registers, x86.fpu.reg[0] ) * 8);
            index /= sizeof( mr->x86.fpu.reg[0] ) * 8;
            shift = index << 1;
            mask = 3;
            mask <<= shift;
            mr->x86.fpu.tag &= ~mask;
            mr->x86.fpu.tag |= TAG_VALID << shift;
        }
        break;
    }
}
