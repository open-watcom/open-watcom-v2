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
* Description:  MIPS machine register description.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "mips.h"
#include "mpstypes.h"
#include "madregs.h"

#define BIT_OFF( who ) (offsetof( mad_registers, mips.who ) * BITS_PER_BYTE)

/* Macros to get at GP/FP registers based on their number; useful in loops */
#define TRANS_GPREG_LO( mr, idx ) (*((unsigned_32 *)(&(mr.r0.u._32[I64LO32])) + (2 * idx)))
#define TRANS_GPREG_HI( mr, idx ) (*((unsigned_32 *)(&(mr.r0.u._32[I64HI32])) + (2 * idx)))
#define TRANS_FPREG_LO( mr, idx ) (*((unsigned_32 *)(&(mr.f0.u64.u._32[I64LO32])) + (2 * idx)))
#define TRANS_FPREG_HI( mr, idx ) (*((unsigned_32 *)(&(mr.f0.u64.u._32[I64HI32])) + (2 * idx)))

enum {
    RS_NONE,
    RS_DWORD,
    RS_WORD,
    RS_DOUBLE,
    RS_FPCSR,
    RS_NUM,
};

#define REG_NAME( name )        const char NAME_##name[] = #name

typedef struct {
    char                name[5];
    unsigned_8          start;
    mad_type_handle     mth;
} sublist_data;

static const sublist_data DwordRegSubData[] = {
        { "b0",  0, MIPST_BYTE },
        { "b1",  8, MIPST_BYTE },
        { "b2", 16, MIPST_BYTE },
        { "b3", 24, MIPST_BYTE },
        { "b4", 32, MIPST_BYTE },
        { "b5", 40, MIPST_BYTE },
        { "b6", 48, MIPST_BYTE },
        { "b7", 56, MIPST_BYTE },
        { "h0",  0, MIPST_HALF },
        { "h1", 16, MIPST_HALF },
        { "h2", 32, MIPST_HALF },
        { "h3", 48, MIPST_HALF },
        { "w0",  0, MIPST_WORD },
        { "w1", 32, MIPST_WORD },
};

#define sublist( name, type, reg_set, base, start, len )        \
        { { NAME_##name,                \
            MIPST_##type,               \
            BIT_OFF( base )+start,      \
            len,                        \
            1 },                        \
            reg_set##_REG_SET, RS_NONE },

REG_NAME( rm );
REG_NAME( fi );
REG_NAME( fu );
REG_NAME( fo );
REG_NAME( fz );
REG_NAME( fv );
REG_NAME( ei );
REG_NAME( eu );
REG_NAME( eo );
REG_NAME( ez );
REG_NAME( ev );
REG_NAME( ci );
REG_NAME( cu );
REG_NAME( co );
REG_NAME( cz );
REG_NAME( cv );
REG_NAME( cn );
REG_NAME( c );

static const mips_reg_info      FPCSRSubList[] = {
    sublist( rm, BYTE, FPU, fpcsr,  0, 2 )
    sublist( fi, BYTE, FPU, fpcsr,  2, 1 )
    sublist( fu, BYTE, FPU, fpcsr,  3, 1 )
    sublist( fo, BYTE, FPU, fpcsr,  4, 1 )
    sublist( fz, BYTE, FPU, fpcsr,  5, 1 )
    sublist( fv, BYTE, FPU, fpcsr,  6, 1 )
    sublist( ei, BYTE, FPU, fpcsr,  7, 1 )
    sublist( eu, BYTE, FPU, fpcsr,  8, 1 )
    sublist( eo, BYTE, FPU, fpcsr,  9, 1 )
    sublist( ez, BYTE, FPU, fpcsr, 10, 1 )
    sublist( ev, BYTE, FPU, fpcsr, 11, 1 )
    sublist( ci, BYTE, FPU, fpcsr, 12, 1 )
    sublist( cu, BYTE, FPU, fpcsr, 13, 1 )
    sublist( co, BYTE, FPU, fpcsr, 14, 1 )
    sublist( cz, BYTE, FPU, fpcsr, 15, 1 )
    sublist( cv, BYTE, FPU, fpcsr, 16, 1 )
    sublist( cn, BYTE, FPU, fpcsr, 17, 1 )
    sublist( c,  BYTE, FPU, fpcsr, 23, 1 )
    { NULL }
};

static const mips_reg_info      *SubList[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    FPCSRSubList,
};

#define MIPST_U32               MIPST_WORD
#define MIPST_U64               MIPST_DWORD
#define XREG_TYPE( bits )       MIPST_U##bits
#define REG_TYPE( bits )        XREG_TYPE( bits )

#define REG_BITS_WORD   32
#define REG_BITS_DWORD  64
#define REG_BITS_DOUBLE 64
#define REG_BITS_FPCSR  REG_BITS_WORD

#define RT_DOUBLE       MIPST_DOUBLE
#define RT_DWORD        MIPST_DWORD
#define RT_WORD         REG_TYPE( REG_BITS_WORD )
#define RT_FPCSR        RT_WORD

/* to avoid relocations to R/W data segments */
#define regpick( name, type, s ) REG_NAME( name );
#include "mpsregs.h"
#undef regpick

#define regpick( name, type, reg_set )  \
        { { NAME_##name,                \
            RT_##type,                  \
            BIT_OFF( name ),            \
            REG_BITS_##type,            \
            1 },                        \
            reg_set##_REG_SET, RS_##type },

const mips_reg_info RegList[] = {
    #include "mpsregs.h"
};

// For 64-bit registers displayed as 32-bit - 32GPRs + pc
// NB: Relies on the fact that all the 64-bit registers are grouped together
// in a single block.
static mips_reg_info    RegListHalf[32 + 32 + 1];

static mips_reg_info    **RegSubList;

static const mad_toggle_strings CPUToggleList[] =
{
    { MAD_MSTR_MHEX,        MAD_MSTR_HEX,           MAD_MSTR_DECIMAL },
    { MAD_MSTR_MEXTENDED,   MAD_MSTR_REG_EXTENDED,  MAD_MSTR_REG_NORMAL },
    { MAD_MSTR_MSYMBOLIC,   MAD_MSTR_REG_SYMBOLIC,  MAD_MSTR_REG_NUMERIC },
    { MAD_MSTR_NIL,         MAD_MSTR_NIL,           MAD_MSTR_NIL }
};

static const mad_toggle_strings FPUToggleList[] =
{
    { MAD_MSTR_MHEX,        MAD_MSTR_HEX,           MAD_MSTR_DECIMAL},
    { MAD_MSTR_NIL,         MAD_MSTR_NIL,           MAD_MSTR_NIL}
};

struct mad_reg_set_data {
    mad_status (*get_piece)( unsigned piece, char **descript, unsigned *max_descript, const mad_reg_info **reg, mad_type_handle *disp_type, unsigned *max_value );
    const mad_toggle_strings    *togglelist;
    mad_string                  name;
};

static mad_status       CPUGetPiece( unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value );

static mad_status       FPUGetPiece( unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value );

static const mad_reg_set_data RegSet[] = {
    { CPUGetPiece, CPUToggleList, MAD_MSTR_CPU },
    { FPUGetPiece, FPUToggleList, MAD_MSTR_FPU },
};

unsigned DIGENTRY MIRegistersSize( void )
{
    return( sizeof( struct mips_mad_registers ) );
}

mad_status DIGENTRY MIRegistersHost( mad_registers *mr )
{
#if defined( __BIG_ENDIAN__ )
    unsigned_32     temp;
    int             i;

    // Currently harcoded for little endian targets - should be dynamic
    // And we really ought to have a 64-bit byte swap routine...

    // Convert GPRs
    for( i = 0; i < 32; i++ ) {
        CONV_BE_32( TRANS_GPREG_LO( mr->mips, i ) );
        CONV_BE_32( TRANS_GPREG_HI( mr->mips, i ) );
        temp = TRANS_GPREG_LO( mr->mips, i );
        TRANS_GPREG_LO( mr->mips, i ) = TRANS_GPREG_HI( mr->mips, i );
        TRANS_GPREG_HI( mr->mips, i ) = temp;
    }
    // Convert FPRs
    for( i = 0; i < 32; i++ ) {
        CONV_BE_32( TRANS_FPREG_LO( mr->mips, i ) );
        CONV_BE_32( TRANS_FPREG_HI( mr->mips, i ) );
        temp = TRANS_FPREG_LO( mr->mips, i );
        TRANS_FPREG_LO( mr->mips, i ) = TRANS_FPREG_HI( mr->mips, i );
        TRANS_FPREG_HI( mr->mips, i ) = temp;
    }
    // Convert special registers
    CONV_BE_32( mr->mips.pc.u._32[I64LO32] );
    CONV_BE_32( mr->mips.pc.u._32[I64HI32] );
    temp = mr->mips.pc.u._32[I64LO32];
    mr->mips.pc.u._32[I64LO32] = mr->mips.pc.u._32[I64HI32];
    mr->mips.pc.u._32[I64HI32] = temp;

    CONV_BE_32( mr->mips.lo );
    CONV_BE_32( mr->mips.hi );
    CONV_BE_32( mr->mips.fpcsr );
    CONV_BE_32( mr->mips.fpivr );
#endif
    return( MS_OK );
}

mad_status DIGENTRY MIRegistersTarget( mad_registers *mr )
{
#if defined( __BIG_ENDIAN__ )
    unsigned_32     temp;
    int             i;

    // Convert GPRs
    for( i = 0; i < 32; i++ ) {
        CONV_BE_32( TRANS_GPREG_LO( mr->mips, i ) );
        CONV_BE_32( TRANS_GPREG_HI( mr->mips, i ) );
        temp = TRANS_GPREG_LO( mr->mips, i );
        TRANS_GPREG_LO( mr->mips, i ) = TRANS_GPREG_HI( mr->mips, i );
        TRANS_GPREG_HI( mr->mips, i ) = temp;
    }
    // Convert FPRs
    for( i = 0; i < 32; i++ ) {
        CONV_BE_32( TRANS_FPREG_LO( mr->mips, i ) );
        CONV_BE_32( TRANS_FPREG_HI( mr->mips, i ) );
        temp = TRANS_FPREG_LO( mr->mips, i );
        TRANS_FPREG_LO( mr->mips, i ) = TRANS_FPREG_HI( mr->mips, i );
        TRANS_FPREG_HI( mr->mips, i ) = temp;
    }
    // Convert special registers
    CONV_BE_32( mr->mips.pc.u._32[I64LO32] );
    CONV_BE_32( mr->mips.pc.u._32[I64HI32] );
    temp = mr->mips.pc.u._32[I64LO32];
    mr->mips.pc.u._32[I64LO32] = mr->mips.pc.u._32[I64HI32];
    mr->mips.pc.u._32[I64HI32] = temp;

    CONV_BE_32( mr->mips.lo );
    CONV_BE_32( mr->mips.hi );
    CONV_BE_32( mr->mips.fpcsr );
    CONV_BE_32( mr->mips.fpivr );
#endif
    return( MS_OK );
}

walk_result DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    walk_result wr;

    if( tk & MTK_INTEGER ) {
        wr = wk( &RegSet[CPU_REG_SET], d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    if( tk & MTK_FLOAT ) {
        wr = wk( &RegSet[FPU_REG_SET], d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    return( WR_CONTINUE );
}

mad_string DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

unsigned DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, unsigned max, char *buff )
{
    char        str[80];
    unsigned    len;

    if( rsd == &RegSet[CPU_REG_SET] ) {
        switch( MCSystemConfig()->cpu ) {
        case MIPS_R2000:
            strcpy( str, "R2000" );
            break;
        case MIPS_R3000:
            strcpy( str, "R3000" );
            break;
        case MIPS_R4000:
            strcpy( str, "R4000" );
            break;
        case MIPS_R5000:
            strcpy( str, "R5000" );
            break;
        default:
            str[0] = '\0';
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

unsigned DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    return( 0 );
}

static char     DescriptBuff[10];

typedef struct {
    unsigned_16         first_reg_idx;
    unsigned_8          num_regs;
    unsigned_8          disp_type;
} reg_display_entry;

/* Note that $zero isn't displayed - it'd just waste valuable space */
static const reg_display_entry CPUNumeric[] = {
    { IDX_r1,           31, MIPST_DWORD },
    { IDX_pc,            1, MIPST_DWORD },
    { IDX_lo,            2, MIPST_WORD },
    { 0,                 0, 0 }
};

static const reg_display_entry CPUSymbolic[] = {
    { IDX_at,           31, MIPST_DWORD },
    { IDX_pc,            1, MIPST_DWORD },
    { IDX_lo,            2, MIPST_WORD },
    { 0,                 0, 0 }
};

static int FindEntry( const reg_display_entry *tbl, unsigned piece,
                                unsigned *idx, mad_type_handle *type )
{
    for( ;; ) {
        if( tbl->num_regs == 0 ) return( 0 );
        if( tbl->num_regs > piece ) break;
        piece -= tbl->num_regs;
        ++tbl;
    }
    *idx = tbl->first_reg_idx + piece;
    *type = tbl->disp_type;
    return( 1 );
}


static mad_status CPUGetPiece( unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    unsigned    idx;

    if( MADState->reg_state[CPU_REG_SET] & CT_SYMBOLIC_NAMES ) {
        if( !FindEntry( CPUSymbolic, piece, &idx, disp_type ) ) return( MS_FAIL );
    } else {
        if( !FindEntry( CPUNumeric, piece, &idx, disp_type ) ) return( MS_FAIL );
    }
    *reg = &RegList[idx].info;
    if( !(MADState->reg_state[CPU_REG_SET] & CT_EXTENDED) ) {
        if( *disp_type == MIPST_DWORD ) {
            *disp_type = MIPST_WORD;
            *reg = &RegListHalf[idx - IDX_r0].info;
        }
    }
    //NYI: if extended & ~ 64 bit mode, downshift to 32-bit display.
    //     Also, if 64 <=> 32 bit switch, tell client to redraw window
    if( !(MADState->reg_state[CPU_REG_SET] & CT_HEX) ) {
        switch( *disp_type ) {
        case MIPST_DWORD:
            *disp_type = MIPST_UINT64;
            break;
        case MIPST_WORD:
            *disp_type = MIPST_ULONG;
            break;
        }
    }
    *descript = DescriptBuff;
    *max_descript = 0;
    *max_value = 0;
    strcpy( DescriptBuff, (*reg)->name );
    return( MS_OK );
}

// NYI: FPUs with real 64-bit registers
static const reg_display_entry FPUList[] = {
    { IDX_f0,           16, MIPST_DOUBLE },
    { IDX_fpcsr,         1, RT_WORD },
    { 0,                 0, 0 }
};

static mad_status FPUGetPiece( unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    unsigned    idx;

    if( !FindEntry( FPUList, piece, &idx, disp_type ) ) return( MS_FAIL );
    if( MADState->reg_state[FPU_REG_SET] & FT_HEX ) {
        switch( *disp_type ) {
        case MIPST_DOUBLE:
            *disp_type = MIPST_HDOUBLE;
            break;
        }
    }
    *descript = DescriptBuff;
    *max_descript = 0;
    *max_value = 0;
    *reg = &RegList[idx].info;
    strcpy( DescriptBuff, (*reg)->name );
    return( MS_OK );
}

mad_status DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                mad_registers const *mr,
                                unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    return( rsd->get_piece( piece, descript, max_descript, reg,
                        disp_type, max_value ) );
}

static const mad_modify_list    WordReg = { NULL, MIPST_WORD, MAD_MSTR_NIL };
static const mad_modify_list    DWordReg = { NULL, MIPST_DWORD, MAD_MSTR_NIL };
static const mad_modify_list    FltReg = { NULL, MIPST_DOUBLE, MAD_MSTR_NIL };

mad_status DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, unsigned *num_possible_p )
{
    *num_possible_p = 1;
    switch( ri->type ) {
    case MIPST_DOUBLE:
        *possible_p = &FltReg;
        break;
    case MIPST_DWORD:
        //NYI: have to check for 64-bit mode
        *possible_p = &DWordReg;
        break;
    default:
        *possible_p = &WordReg;
        break;
    }
    return( MS_OK );
}

mad_status DIGENTRY MIRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *cur )
{
    unsigned_64 new_ip;
    unsigned_8  *p_old;
    unsigned_8  *p_cur;
    unsigned    mask;
    unsigned    size;

    if( ri->bit_start == BIT_OFF( pc ) ) {
        new_ip = old->mips.pc;
        //NYI: 64 bit
        new_ip.u._32[I64LO32] += sizeof( unsigned_32 );
        if( new_ip.u._32[I64LO32] != cur->mips.pc.u._32[I64LO32] ) {
            return( MS_MODIFIED_SIGNIFICANTLY );
        } else if( old->mips.pc.u._32[I64LO32] != cur->mips.pc.u._32[I64LO32] ) {
            return( MS_MODIFIED );
        }
    } else {
        p_old = (unsigned_8 *)old + (ri->bit_start / BITS_PER_BYTE);
        p_cur = (unsigned_8 *)cur + (ri->bit_start / BITS_PER_BYTE);
        size = ri->bit_size;
        if( size >= BITS_PER_BYTE ) {
            /* it's going to be byte aligned */
            return( memcmp( p_old, p_cur, size / BITS_PER_BYTE ) != 0 ? MS_MODIFIED_SIGNIFICANTLY : MS_OK );
        } else {
            mask = (1 << size) - 1;
            #define GET_VAL( w ) (((*p_##w >> (ri->bit_start % BITS_PER_BYTE))) & mask)
            return( GET_VAL( old ) != GET_VAL( cur ) ? MS_MODIFIED_SIGNIFICANTLY : MS_OK );
        }
    }
    return( MS_OK );
}

mad_status DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, mad_registers const *mr, address *a )
{
    unsigned    bit_start;
    unsigned_64 *p;

    memset( a, 0, sizeof( *a ) );
    bit_start = ri->bit_start;
    if( bit_start == BIT_OFF( pc ) ) {
        a->mach.offset = mr->mips.pc.u._32[I64LO32];
        return( MS_OK );
    }
    if( bit_start >= BIT_OFF( f0 ) && bit_start < (BIT_OFF( f31 ) + 64) ) {
        return( MS_FAIL );
    }
    p = (unsigned_64 *)((unsigned_8 *)mr + (bit_start / BITS_PER_BYTE));
    a->mach.offset = p->u._32[I64LO32];
    return( MS_OK );
}

const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    return( rsd->togglelist );
}

unsigned DIGENTRY MIRegSetDisplayToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    unsigned    toggle;
    unsigned    *bits;
    unsigned    index;
    unsigned    old;

    toggle = on & off;
    index = rsd - &RegSet[CPU_REG_SET];
    bits = &MADState->reg_state[index];
    old = *bits;
    *bits ^= toggle;
    *bits |= on & ~toggle;
    *bits &= ~off | toggle;
    if( index == CPU_REG_SET && ((old ^ *bits) & CT_SYMBOLIC_NAMES) ) {
        /* We've changed from numeric regs to symbolic or vice versa.
         * Have to force a redraw of the disassembly window.
         */
        MCNotify( MNT_REDRAW_DISASM, NULL );
    }
    return( *bits );
}

walk_result DIGENTRY MIRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    const mips_reg_info *curr;
    walk_result         wr;
    unsigned            reg_set;

    if( ri != NULL ) {
        switch( ((mips_reg_info *)ri)->sublist_code ) {
        case RS_DWORD:
            curr = RegSubList[ri->bit_start / (sizeof( unsigned_64 ) * BITS_PER_BYTE)];
            break;
        default:
            curr = SubList[((mips_reg_info *)ri)->sublist_code];
            break;
        }
        if( curr != NULL ) {
            while( curr->info.name != NULL ) {
                wr = wk( &curr->info, 0, d );
                if( wr != WR_CONTINUE ) return( wr );
                ++curr;
            }
        }
    } else {
        reg_set = rsd - RegSet;
        curr = RegList;
        while( curr < &RegList[ IDX_LAST_ONE ] ) {
            if( curr->reg_set == reg_set ) {
                wr = wk( &curr->info, curr->sublist_code != 0, d );
                if( wr != WR_CONTINUE ) return( wr );
            }
            ++curr;
        }
    }
    return( WR_CONTINUE );
}

void DIGENTRY MIRegSpecialGet( mad_special_reg sr, mad_registers const *mr, addr_ptr *ma )
{
    ma->segment = 0;
    switch( sr ) {
    case MSR_IP:
        ma->offset = mr->mips.pc.u._32[I64LO32];
        break;
    case MSR_SP:
        ma->offset = mr->mips.sp.u._32[I64LO32];
        break;
    case MSR_FP:
        //NYI: may not be used?
        ma->offset = mr->mips.r30.u._32[I64LO32];
        break;
    }
}

void DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, addr_ptr const *ma )
{
    switch( sr ) {
    case MSR_IP:
        mr->mips.pc.u._32[I64LO32] = ma->offset;
        break;
    case MSR_SP:
        mr->mips.sp.u._32[I64LO32] = ma->offset;
        break;
    case MSR_FP:
        //NYI: may not be used?
        mr->mips.r30.u._32[I64LO32] = ma->offset;
        break;
    }
}

unsigned DIGENTRY MIRegSpecialName( mad_special_reg sr, mad_registers const *mr, mad_address_format af, unsigned max, char *buff )
{
    unsigned    idx;
    unsigned    len;
    char const  *p;

    switch( sr ) {
    case MSR_IP:
        idx = IDX_pc;
        break;
    case MSR_SP:
        idx = IDX_sp;
        break;
    case MSR_FP:
        idx = IDX_r30;
        break;
    default:
        idx = 0;
        break;
    }
    p = RegList[idx].info.name;
    len = strlen( p );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, p, max );
        buff[max] = '\0';
    }
    return( len );
}


const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
{
    const mad_reg_info  *reg;

    reg = NULL;
    switch( ci ) {
    case CI_MIPS_pc:
        reg = &RegList[IDX_pc].info;
        break;
    case CI_MIPS_lo:
        reg = &RegList[IDX_lo].info;
        break;
    case CI_MIPS_hi:
        reg = &RegList[IDX_hi].info;
        break;
    default:
        if( ci >= CI_MIPS_r0 && ci <= CI_MIPS_r31 ) {
            reg = &RegList[ci - CI_MIPS_r0 + IDX_r0].info;
        } else if( ci >= CI_MIPS_f0 && ci <= CI_MIPS_f31 ) {
            reg = &RegList[ci - CI_MIPS_f0 + IDX_f0].info;
        }
    }
    return( reg );
}

void DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
}

void DIGENTRY MIRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    unsigned    i;
    unsigned    bit_end;

    bit_end = bit_start + bit_size;
    #define IN_RANGE( i, bit )  \
      ((bit) >= RegList[i].info.bit_start && (bit) < RegList[i].info.bit_start+RegList[i].info.bit_size)
    for( i = 0; i < IDX_LAST_ONE; ++i ) {
        if( (IN_RANGE(i, bit_start) || IN_RANGE( i, bit_end ))) {
            MCNotify( MNT_MODIFY_REG, (void *)&RegSet[RegList[i].reg_set] );
            break;
        }
    }
    switch( bit_start ) {
    case BIT_OFF( pc ):
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case BIT_OFF( sp ):
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case BIT_OFF( r30 ): //NYI: may not be used?
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    }
}

static mad_status AddSubList( unsigned idx, const sublist_data *sub, unsigned num )
{
    unsigned    i;
    unsigned    j;

    i = RegList[idx].info.bit_start / (sizeof( unsigned_64 )*BITS_PER_BYTE);
    if( RegSubList[i] != NULL )
        return( MS_OK );
    RegSubList[i] = MCAlloc( sizeof( mips_reg_info ) * (num+1) );
    if( RegSubList[i] == NULL )
        return( MS_ERR | MS_NO_MEM );
    memset( RegSubList[i], 0, sizeof( mips_reg_info ) * (num+1) );
    for( j = 0; j < num; ++j ) {
        RegSubList[i][j] = RegList[idx];
        RegSubList[i][j].info.name       = sub[j].name;
        RegSubList[i][j].info.type       = sub[j].mth;
        RegSubList[i][j].info.bit_start += sub[j].start;
        RegSubList[i][j].info.bit_size   = TypeArray[ sub[j].mth ].u.b->bits;
        RegSubList[i][j].sublist_code    = RS_NONE;
    }
    return( MS_OK );
}

mad_status RegInit( void )
{
    unsigned    i;
    unsigned    max;
    unsigned    curr;
    unsigned    half_idx = 0;
    mad_status  ms;

    max = 0;
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_DWORD:
            curr = RegList[i].info.bit_start / (sizeof(unsigned_64)*BITS_PER_BYTE);
            if( curr > max )
                max = curr;
            RegListHalf[half_idx] = RegList[i];
#if defined( __BIG_ENDIAN__ )
            // kludge for 64-bit registers displayed as 32-bit - need to
            // skip 32 bits!
            RegListHalf[half_idx].info.bit_start += 32;
#endif
            RegListHalf[half_idx].info.bit_size = 32;
            ++half_idx;
            break;
        }
    }
    RegSubList = MCAlloc( (max+1) * sizeof( *RegSubList ) );
    if( RegSubList == NULL )
        return( MS_ERR | MS_NO_MEM );
    memset( RegSubList, 0, (max+1) * sizeof( *RegSubList ) );
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_DWORD:
            ms = AddSubList( i, DwordRegSubData, NUM_ELTS( DwordRegSubData ) );
            if( ms != MS_OK )
                return( ms );
            break;
        }
    }
    return( MS_OK );
}

void RegFini( void )
{
    unsigned    i;
    unsigned    max;
    unsigned    curr;

    max = 0;
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_DWORD:
            curr = RegList[i].info.bit_start / (sizeof( unsigned_64 ) * BITS_PER_BYTE);
            if( curr > max )
                max = curr;
            break;
        }
    }
    for( i = 0; i <= max; ++i )
        MCFree( RegSubList[i] );
    MCFree( RegSubList );
    RegSubList = NULL;
}
