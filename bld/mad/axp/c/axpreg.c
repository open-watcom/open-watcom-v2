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


#include <string.h>
#include <ctype.h>
#include "axp.h"
#include "axptypes.h"
#include "madregs.h"


#define BIT_OFF(who)    (offsetof( mad_registers, axp.who ) * BITS_PER_BYTE)
#define IS_FP_BIT(x)    (x < BIT_OFF(f31) + 64)

enum {
    RS_NONE,
    RS_INT,
    RS_FLT,
    RS_FPCR,
    RS_NT_SOFTFPCR,
    RS_NT_PSR,
    RS_NUM,
};

typedef enum {
    RF_NONE,
    RF_GPREG  = 0x1,
    RF_FPREG  = 0x2,
} register_flags;

#define REG_NAME(id)        static const char NAME_##id[] = #id
#define REG_NAME2(id,name)  static const char NAME_##id[] = #name

typedef struct {
    char                name[5];
    unsigned_8          start;
    mad_type_handle     mth;
} sublist_data;

static const sublist_data IntRegSubData[] = {
    { "b0",  0, AXPT_BYTE },
    { "b1",  8, AXPT_BYTE },
    { "b2", 16, AXPT_BYTE },
    { "b3", 24, AXPT_BYTE },
    { "b4", 32, AXPT_BYTE },
    { "b5", 40, AXPT_BYTE },
    { "b6", 48, AXPT_BYTE },
    { "b7", 56, AXPT_BYTE },
    { "w0",  0, AXPT_WORD },
    { "w1", 16, AXPT_WORD },
    { "w2", 32, AXPT_WORD },
    { "w3", 48, AXPT_WORD },
    { "l0",  0, AXPT_LWORD },
    { "l1", 32, AXPT_LWORD },
};

static const sublist_data FltRegSubData[] = {
    { "t", 0, AXPT_DOUBLE },
    { "g", 0, AXPT_RG_FLOAT },
};

#define sublist( id, type, reg_set, base, start, len ) \
    { { NAME_##id,                  \
        AXPT_##type,                \
        BIT_OFF( base ) + start,    \
        len,                        \
        RF_GPREG },                 \
        PAL_all, reg_set##_REG_SET, RS_NONE },

REG_NAME( invd );
REG_NAME( dzed );
REG_NAME( ovfd );
REG_NAME( inv );
REG_NAME( dze );
REG_NAME( ovf );
REG_NAME( unf );
REG_NAME( ine );
REG_NAME( iov );
REG_NAME( dyn );
REG_NAME( sum );

static const axp_reg_info       FPCRSubList[] = {
    sublist( invd, BYTE, FPU, fpcr, 49, 1 )
    sublist( dzed, BYTE, FPU, fpcr, 50, 1 )
    sublist( ovfd, BYTE, FPU, fpcr, 51, 1 )
    sublist( inv,  BYTE, FPU, fpcr, 52, 1 )
    sublist( dze,  BYTE, FPU, fpcr, 53, 1 )
    sublist( ovf,  BYTE, FPU, fpcr, 54, 1 )
    sublist( unf,  BYTE, FPU, fpcr, 55, 1 )
    sublist( ine,  BYTE, FPU, fpcr, 56, 1 )
    sublist( iov,  BYTE, FPU, fpcr, 57, 1 )
    sublist( dyn,  BYTE, FPU, fpcr, 58, 2 )
    sublist( sum,  BYTE, FPU, fpcr, 63, 1 )
    { NULL }
};

REG_NAME( mode );
REG_NAME( ie );
REG_NAME( irql );

static const axp_reg_info       NTPSRSubList[] = {
    sublist( mode, BYTE, CPU, pal.nt.psr, 0, 1 )
    sublist( ie,   BYTE, CPU, pal.nt.psr, 1, 1 )
    sublist( irql, BYTE, CPU, pal.nt.psr, 2, 3 )
    { NULL }
};

static const axp_reg_info       *SubList[] =
{
    NULL,
    NULL,
    NULL,
    FPCRSubList,
    NULL,                       // NT SOFTFPCR
    NTPSRSubList
};

#define AXPT_U32                AXPT_LWORD
#define AXPT_U64                AXPT_QWORD
#define XREG_TYPE( bits )       AXPT_U##bits
#define REG_TYPE( bits )        XREG_TYPE( bits )

#define REG_BITS_INT    64
#define REG_BITS_FLT    64
#define REG_BITS_FPCR   REG_BITS_INT
#define REG_BITS_U32    32

#define RT_INT          REG_TYPE( REG_BITS_INT )
#define RT_FLT          AXPT_DOUBLE
#define RT_FPCR         RT_INT
#define RT_U32          AXPT_U32

/* to avoid relocations to R/W data segments */
#define regpick(id,type,reg_set)    REG_NAME(id);
#define regpicku(u,id,type,reg_set) REG_NAME(id);
#define palpick(pal,id)             REG_NAME2(pal##_##id,id);
#include "axpregs.h"
#undef regpick
#undef regpicku
#undef palpick

#define PR_nt_fir                       \
        { { NAME_nt_fir,                \
            RT_INT,                     \
            BIT_OFF( pal.nt.fir ),      \
            REG_BITS_INT,               \
            RF_GPREG },                 \
            PAL_nt, 0, RS_NONE }

#define PR_nt_softfpcr                  \
        { { NAME_nt_softfpcr,           \
            RT_INT,                     \
            BIT_OFF( pal.nt.softfpcr ), \
            REG_BITS_INT,               \
            RF_GPREG },                 \
            PAL_nt, 0, RS_NT_SOFTFPCR }

#define PR_nt_psr                       \
        { { NAME_nt_psr,                \
            RT_U32,                     \
            BIT_OFF( pal.nt.psr ),      \
            REG_BITS_U32,               \
            RF_GPREG },                 \
            PAL_nt, 0, RS_NT_PSR }

const axp_reg_info RegList[] = {
    #define regpick(id,type,reg_set)    \
        { { NAME_##id,                  \
            RT_##type,                  \
            BIT_OFF( id ),              \
            REG_BITS_##type,            \
            RF_GPREG },                 \
            PAL_all, reg_set##_REG_SET, RS_##type },
    #define regpicku(u,id,type,reg_set) \
        { { NAME_##id,                  \
            RT_##type,                  \
            BIT_OFF( u ),               \
            REG_BITS_##type,            \
            RF_GPREG },                 \
            PAL_all, reg_set##_REG_SET, RS_##type },
    #define palpick(pal,id)     PR_##pal##_##id,
    #include "axpregs.h"
    #undef regpick
    #undef regpicku
    #undef palpick
};

static axp_reg_info             **RegSubList;

static const mad_toggle_strings CPUToggleList[] =
{
    {MAD_MSTR_MHEX, MAD_MSTR_HEX, MAD_MSTR_DECIMAL},
    {MAD_MSTR_MEXTENDED, MAD_MSTR_REG_EXTENDED, MAD_MSTR_REG_NORMAL},
    {MAD_MSTR_MSYMBOLIC, MAD_MSTR_REG_SYMBOLIC, MAD_MSTR_REG_NUMERIC},
    {MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL}
};

static const mad_toggle_strings FPUToggleList[] =
{
    {MAD_MSTR_MHEX, MAD_MSTR_HEX, MAD_MSTR_DECIMAL},
    {MAD_MSTR_MVAX, MAD_MSTR_VAX, MAD_MSTR_IEEE},
    {MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL}
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

axp_pal CurrPAL;


mad_status GetPData( addr_off off, axp_pdata *pdata )
{
    address     a;

    memset( &a, 0, sizeof( a ) );
    a.mach.offset = off;
    MCMachineData( a, AXPMD_PDATA, 0, NULL, sizeof( *pdata ), pdata );
    if( pdata->pro_end_addr.u._32[0] < pdata->beg_addr.u._32[0]
     || pdata->pro_end_addr.u._32[0] >= pdata->end_addr.u._32[0] ) {
        /*
           This is a procedure with different exception handlers for
           different portions - pro_end_addr is the address of the
           start of the procedure in this case.
        */
        a.mach.offset = pdata->pro_end_addr.u._32[0];
        MCMachineData( a, AXPMD_PDATA, 0, NULL, sizeof( *pdata ), pdata );
    }
    return( MS_OK );
}

int VariableFrame( addr_off off )
{
    axp_pdata   pdata;
    unsigned_32 ins;
    address     a;

    if( GetPData( off, &pdata ) != MS_OK ) return( 0 );
    if( pdata.pro_end_addr.u._32[0] == pdata.beg_addr.u._32[0] ) {
        return( 0 );
    }
    memset( &a, 0, sizeof( a ) );
    a.mach.offset = pdata.pro_end_addr.u._32[0] - sizeof( ins );
    MCReadMem( a, sizeof( ins ), &ins );
    return( ins == INS_MOV_SP_FP );
}

unsigned        DIGENTRY MIRegistersSize( void )
{
    return( sizeof( struct axp_mad_registers ) );
}

mad_status      DIGENTRY MIRegistersHost( mad_registers *mr )
{
    /* stash for the register walker */
    CurrPAL = mr->axp.active_pal;
    return( MS_OK );
}

mad_status      DIGENTRY MIRegistersTarget( mad_registers *mr )
{
    mr = mr;
    return( MS_OK );
}

walk_result     DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
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

mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

unsigned        DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, char *buff, unsigned buff_len )
{
    const char  *str;
    unsigned    len;

    str = "";
    if( rsd == &RegSet[CPU_REG_SET] ) {
        switch( MCSystemConfig()->cpu ) {
        case AXP_21064:
            str = "21064";
            break;
        case AXP_21164:
            str = "21164";
            break;
        }
    }
    len = strlen( str );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, str, buff_len );
        buff[buff_len] = '\0';
    }
    return( len );
}

unsigned        DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    rsd = rsd;
    return( 0 );
}

static char     DescriptBuff[10];

typedef struct {
    unsigned_16         first_reg_idx;
    unsigned_8          num_regs;
    unsigned_8          disp_type;
} reg_display_entry;

static const reg_display_entry CPUNumeric[] = {
    { IDX_r0,           31, RT_INT },
    { IDX_nt_fir,        1, RT_INT },
    { IDX_nt_psr,        1, AXPT_U32 },
    { 0,                 0, 0 }
};

static const reg_display_entry CPUSymbolic[] = {
    { IDX_v0,           31, RT_INT },
    { IDX_nt_fir,        1, RT_INT },
    { IDX_nt_psr,        1, AXPT_U32 },
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


static mad_status       CPUGetPiece( unsigned piece,
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
    if( !(MADState->reg_state[CPU_REG_SET] & CT_EXTENDED) ) {
        if( *disp_type == AXPT_QWORD ) *disp_type = AXPT_LWORD;
    }
    if( !(MADState->reg_state[CPU_REG_SET] & CT_HEX) ) {
        switch( *disp_type ) {
        case AXPT_QWORD:
            *disp_type = AXPT_UINT64;
            break;
        case AXPT_LWORD:
            *disp_type = AXPT_ULONG;
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

static const reg_display_entry FPUList[] = {
    { IDX_f0,           31, AXPT_DOUBLE },
    { IDX_fpcr,          1, RT_INT },
    { IDX_nt_softfpcr,   1, RT_INT },
    { 0,                 0, 0 }
};

static mad_status       FPUGetPiece( unsigned piece,
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
        case AXPT_DOUBLE:
            *disp_type = AXPT_HDOUBLE;
            break;
        }
    }
    if( MADState->reg_state[FPU_REG_SET] & FT_G_FLOAT ) {
        switch( *disp_type ) {
        case AXPT_DOUBLE:
            *disp_type = AXPT_RG_FLOAT;
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

mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                const mad_registers *mr,
                                unsigned piece,
                                char **descript,
                                unsigned *max_descript,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    mr = mr;
    return( rsd->get_piece( piece, descript, max_descript, reg, disp_type, max_value ) );
}

static const mad_modify_list    IntReg = { NULL, REG_TYPE( REG_BITS_INT ), MAD_MSTR_NIL };
static const mad_modify_list    FltReg = { NULL, AXPT_DOUBLE, MAD_MSTR_NIL };

mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, int *num_possible_p )
{
    rsd = rsd;
    switch( ri->bit_start ) {
    case BIT_OFF( u31 ):
    case BIT_OFF( f31 ):
        *possible_p = NULL;
        *num_possible_p = 0;
        return( MS_FAIL );
    }
    *num_possible_p = 1;
    if( ri->type == AXPT_DOUBLE ) {
        *possible_p = &FltReg;
    } else {
        *possible_p = &IntReg;
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

    rsd = rsd;
    if( ri->bit_start == BIT_OFF( pal.nt.fir ) ) {
        new_ip = old->axp.pal.nt.fir;
        //NYI: 64 bit
        new_ip.u._32[0] += sizeof( unsigned_32 );
        if( new_ip.u._32[0] != cur->axp.pal.nt.fir.u._32[0] ) {
            return( MS_MODIFIED_SIGNIFICANTLY );
        } else if( old->axp.pal.nt.fir.u._32[0] != cur->axp.pal.nt.fir.u._32[0] ) {
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

mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    unsigned    bit_start;
    unsigned_32 *p;

    memset( a, 0, sizeof( *a ) );
    bit_start = ri->bit_start;
    if( bit_start == BIT_OFF( pal.nt.fir ) ) {
        a->mach.offset = mr->axp.pal.nt.fir.u._32[0];
        return( MS_OK );
    }
    if( IS_FP_BIT( bit_start ) ) {
        return( MS_FAIL );
    }
    if( bit_start >= BIT_OFF( pal ) ) {
        return( MS_FAIL );
    }
    p = (unsigned_32 *)((unsigned_8 *)mr + (bit_start / BITS_PER_BYTE));
    a->mach.offset = *p;
    return( MS_OK );
}

const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    return( rsd->togglelist );
}

unsigned        DIGENTRY MIRegSetDisplayToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
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
        /*
           We've changed from numeric regs to symbolic or vis-versa.
           Have to force a redraw of the disassembly window.
        */
        MCNotify( MNT_REDRAW_DISASM, NULL );
    }
    return( *bits );
}

walk_result     DIGENTRY MIRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    const axp_reg_info  *curr;
    walk_result         wr;
    unsigned            reg_set;

    if( ri != NULL ) {
        switch( ((axp_reg_info *)ri)->sublist_code ) {
        case RS_INT:
        case RS_FLT:
            curr = RegSubList[ri->bit_start / ( sizeof( axpreg ) * BITS_PER_BYTE )];
            break;
        default:
            curr = SubList[((axp_reg_info *)ri)->sublist_code];
            break;
        }
        if( curr != NULL ) {
            while( curr->info.name != NULL ) {
                wr = wk( &curr->info, 0, d );
                if( wr != WR_CONTINUE )
                    return( wr );
                ++curr;
            }
        }
    } else {
        reg_set = rsd - RegSet;
        curr = RegList;
        while( curr < &RegList[ IDX_LAST_ONE ] ) {
            if( curr->reg_set == reg_set && (curr->pal == PAL_all || curr->pal == CurrPAL) ) {
                wr = wk( &curr->info, curr->sublist_code != 0, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
            ++curr;
        }
    }
    return( WR_CONTINUE );
}

void    DIGENTRY MIRegSpecialGet( mad_special_reg sr, const mad_registers *mr, addr_ptr *ma )
{
    ma->segment = 0;
    switch( sr ) {
    case MSR_IP:
        /* doesn't matter what PAL is in control since always first field */
        ma->offset = mr->axp.pal.nt.fir.u._32[0];
        break;
    case MSR_SP:
        ma->offset = mr->axp.u30.sp.u64.u._32[0];
        break;
    case MSR_FP:
        if( VariableFrame( mr->axp.pal.nt.fir.u._32[0] ) ) {
            ma->offset = mr->axp.u15.fp.u64.u._32[0];
        } else {
            ma->offset = mr->axp.u30.sp.u64.u._32[0];
        }
        break;
    }
}

void    DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *ma )
{
    switch( sr ) {
    case MSR_IP:
        /* doesn't matter what PAL is in control since always first field */
        mr->axp.pal.nt.fir.u._32[0] = ma->offset;
        break;
    case MSR_SP:
        mr->axp.u30.sp.u64.u._32[0] = ma->offset;
        break;
    case MSR_FP:
        if( VariableFrame( mr->axp.pal.nt.fir.u._32[0] ) ) {
            mr->axp.u15.fp.u64.u._32[0] = ma->offset;
        } else {
            mr->axp.u30.sp.u64.u._32[0] = ma->offset;
        }
        break;
    }
}

unsigned    DIGENTRY MIRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, char *buff, unsigned buff_len )
{
    unsigned    idx;
    unsigned    len;
    const char  *p;

    af = af;
    idx = 0;
    switch( sr ) {
    case MSR_IP:
        switch( mr->axp.active_pal ) {
        case PAL_nt:
            idx = IDX_nt_fir;
            break;
        case PAL_unix:
        case PAL_vms:
            //NYI: later
            //break;
        default:
            /* 'cause we gotta have one */
            idx = IDX_nt_fir;
            break;
        }
        break;
    case MSR_SP:
        idx = IDX_sp;
        break;
    case MSR_FP:
        if( VariableFrame( mr->axp.pal.nt.fir.u._32[0] ) ) {
            idx = IDX_fp;
        } else {
            idx = IDX_sp;
        }
        break;
    }
    p = RegList[idx].info.name;
    len = strlen( p );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, p, buff_len );
        buff[buff_len] = '\0';
    }
    return( len );
}


const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
{
    const mad_reg_info  *reg;

    reg = NULL;
    if( ci == CI_AXP_fir ) {
        reg = &RegList[IDX_nt_fir].info;
    } else if( ci >= CI_AXP_r0 && ci <= CI_AXP_r31 ) {
        reg = &RegList[ci - CI_AXP_r0 + IDX_r0].info;
    } else if( ci >= CI_AXP_f0 && ci <= CI_AXP_f31 ) {
        reg = &RegList[ci - CI_AXP_f0 + IDX_f0].info;
    }
    return( reg );
}

void            DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    bit_size = bit_size;
    bit_start = bit_start;
    flags = flags;
    mr = mr;
}

void            DIGENTRY MIRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    unsigned    i;
    unsigned    bit_end;

    mr = mr;
    flags = flags;
    memset( &mr->axp.u31, 0, sizeof( mr->axp.u31 ) );
    memset( &mr->axp.f31, 0, sizeof( mr->axp.f31 ) );
    bit_end = bit_start + bit_size;
    #define IN_RANGE( i, bit )  \
      ((bit) >= RegList[i].info.bit_start && (bit) < RegList[i].info.bit_start+RegList[i].info.bit_size)
    for( i = 0; i < IDX_LAST_ONE; ++i ) {
        if( (IN_RANGE(i, bit_start) || IN_RANGE( i, bit_end ))
         && (RegList[i].pal == PAL_all || RegList[i].pal == mr->axp.active_pal) ) {
            MCNotify( MNT_MODIFY_REG, (void *)&RegSet[RegList[i].reg_set] );
            break;
        }
    }
    switch( bit_start ) {
    case BIT_OFF( pal.nt.fir ):
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case BIT_OFF( u30 ): // sp
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case BIT_OFF( u15 ): // fp
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    }
}

static mad_status AddSubList( unsigned idx, const sublist_data *sub, unsigned num )
{
    unsigned    i;
    unsigned    j;

    i = RegList[idx].info.bit_start / (sizeof( axpreg )*BITS_PER_BYTE);
    if( RegSubList[i] != NULL )
        return( MS_OK );
    RegSubList[i] = MCAlloc( sizeof( axp_reg_info ) * (num+1) );
    if( RegSubList[i] == NULL )
        return( MS_ERR|MS_NO_MEM );
    memset( RegSubList[i], 0, sizeof( axp_reg_info ) * (num+1) );
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
    mad_status  ms;

    max = 0;
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_INT:
        case RS_FLT:
            curr = RegList[i].info.bit_start / ( sizeof( axpreg ) * BITS_PER_BYTE );
            if( curr > max )
                max = curr;
            break;
        }
    }
    RegSubList = MCAlloc( ( max + 1 ) * sizeof( *RegSubList ) );
    if( RegSubList == NULL )
        return( MS_ERR | MS_NO_MEM );
    memset( RegSubList, 0, ( max + 1 ) * sizeof( *RegSubList ) );
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_INT:
            ms = AddSubList( i, IntRegSubData, NUM_ELTS( IntRegSubData ) );
            if( ms != MS_OK )
                return( ms );
            break;
        case RS_FLT:
            ms = AddSubList( i, FltRegSubData, NUM_ELTS( FltRegSubData ) );
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
        case RS_INT:
        case RS_FLT:
            curr = RegList[i].info.bit_start / (sizeof(axpreg)*BITS_PER_BYTE);
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
