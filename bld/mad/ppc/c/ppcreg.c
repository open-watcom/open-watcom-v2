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
* Description:  PowerPC machine register description.
*
****************************************************************************/


#include <ctype.h>
#include "ppc.h"
#include "ppctypes.h"
#include "madregs.h"


#define BIT_OFF( who ) BYTES2BITS( offsetof( mad_registers, ppc.who ) )
#define IS_FP_BIT(x)   (x >= BIT_OFF(f0) && x < BIT_OFF(f31) + 64)

#define PPC_SWAP_REG_64(x)                          \
    {                                               \
        unsigned_32     temp;                       \
        CONV_BE_32( (x).u._32[I64LO32] );           \
        CONV_BE_32( (x).u._32[I64HI32] );           \
        temp = (x).u._32[I64LO32];                  \
        (x).u._32[I64LO32] = (x).u._32[I64HI32];    \
        (x).u._32[I64HI32] = temp;                  \
    }

enum {
    RS_NONE,
    RS_DWORD,
    RS_WORD,
    RS_DOUBLE,
    RS_MSR,
    RS_XER,
    RS_PCSCR,
    RS_NUM,
};

typedef enum {
    RF_NONE,
    RF_GPREG  = 0x1,
    RF_FPREG  = 0x2,
} register_flags;

#define REG_NAME( name )        const char NAME_##name[] = #name

typedef struct {
    char                name[5];
    unsigned_8          start;
    mad_type_handle     mth;
} sublist_data;

static const sublist_data IntRegSubData[] = {
    { "b0",  0, PPCT_BYTE },
    { "b1",  8, PPCT_BYTE },
    { "b2", 16, PPCT_BYTE },
    { "b3", 24, PPCT_BYTE },
    { "b4", 32, PPCT_BYTE },
    { "b5", 40, PPCT_BYTE },
    { "b6", 48, PPCT_BYTE },
    { "b7", 56, PPCT_BYTE },
    { "h0",  0, PPCT_HWORD },
    { "h1", 16, PPCT_HWORD },
    { "h2", 32, PPCT_HWORD },
    { "h3", 48, PPCT_HWORD },
    { "w0",  0, PPCT_WORD },
    { "w1", 32, PPCT_WORD },
};

#define sublist( id, type, reg_set, base, start, len ) \
    { { NAME_##id,                  \
        PPCT_##type,                \
        BIT_OFF( base ) + start,    \
        len,                        \
        RF_GPREG },                 \
        reg_set##_REG_SET, RS_NONE },

static const ppc_reg_info       *SubList[] =
{
    //NYI:
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

#define PPCT_U32                PPCT_WORD
#define PPCT_U64                PPCT_DWORD
#define XREG_TYPE( bits )       PPCT_U##bits
#define REG_TYPE( bits )        XREG_TYPE( bits )

#define REG_BITS_WORD   32
#define REG_BITS_DWORD  64
#define REG_BITS_DOUBLE 64

/* to avoid relocations to R/W data segments */
#define regpick(id,type,reg_set)    REG_NAME( id );
#define regpicku(u,id,type,reg_set) REG_NAME( id );
#include "ppcregs.h"
#undef regpick
#undef regpicku

const ppc_reg_info RegList[] = {
    #define regpick(id,type,reg_set)    \
        { { NAME_##id,                  \
            PPCT_##type,                \
            BIT_OFF( id ),              \
            REG_BITS_##type,            \
            RF_GPREG },                 \
            reg_set##_REG_SET, RS_##type },
    #define regpicku(u,id,type,reg_set) \
        { { NAME_##id,                  \
            PPCT_##type,                \
            BIT_OFF( u ),               \
            REG_BITS_##type,            \
            RF_GPREG },                 \
            reg_set##_REG_SET, RS_##type },
    #include "ppcregs.h"
    #undef regpick
    #undef regpicku
};

// For 64-bit registers displayed as 32-bit - 32GPRs + sp, lr, iar, ctr, msr
// NB: Relies on the fact that all the DWORD registers are grouped together
// in a single block.
static ppc_reg_info RegListHalf[32 + 5];

static ppc_reg_info             **RegSubList;

static const mad_toggle_strings CPUToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_MEXTENDED,MAD_MSTR_REG_EXTENDED,MAD_MSTR_REG_NORMAL},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};

static const mad_toggle_strings FPUToggleList[] =
{
    {MAD_MSTR_MHEX,MAD_MSTR_HEX,MAD_MSTR_DECIMAL},
    {MAD_MSTR_NIL,MAD_MSTR_NIL,MAD_MSTR_NIL}
};

struct mad_reg_set_data {
    mad_status (*get_piece)( unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value );
    const mad_toggle_strings    *togglelist;
    mad_string                  name;
};

static mad_status       CPUGetPiece( unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value );

static mad_status       FPUGetPiece( unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value );

static const mad_reg_set_data RegSet[] = {
    { CPUGetPiece, CPUToggleList, MAD_MSTR_CPU },
    { FPUGetPiece, FPUToggleList, MAD_MSTR_FPU },
};

unsigned        DIGENTRY MIRegistersSize( void )
{
    return( sizeof( struct ppc_mad_registers ) );
}

mad_status      DIGENTRY MIRegistersHost( mad_registers *mr )
{
#if !defined( __BIG_ENDIAN__ )
    int             i;

    // Currently harcoded for big endian targets - should be dynamic
    // And we really ought to have a 64-bit byte swap routine...

    // Convert GPRs
    for( i = 0; i < 32; i++ ) {
        PPC_SWAP_REG_64( *(&mr->ppc.r0 + i) );
    }
    // Convert FPRs
    for( i = 0; i < 32; i++ ) {
        PPC_SWAP_REG_64( (*(&mr->ppc.f0 + i)).u64 );
    }
    // Convert special registers
    PPC_SWAP_REG_64( mr->ppc.iar );
    PPC_SWAP_REG_64( mr->ppc.msr );
    PPC_SWAP_REG_64( mr->ppc.ctr );
    PPC_SWAP_REG_64( mr->ppc.lr );

    CONV_BE_32( mr->ppc.xer );
    CONV_BE_32( mr->ppc.cr );
    CONV_BE_32( mr->ppc.fpscr );
#endif
    return( MS_OK );
}

mad_status      DIGENTRY MIRegistersTarget( mad_registers *mr )
{
#if !defined( __BIG_ENDIAN__ )
    int             i;

    // Convert GPRs
    for( i = 0; i < 32; i++ ) {
        PPC_SWAP_REG_64( *(&mr->ppc.r0 + i) );
    }
    // Convert FPRs
    for( i = 0; i < 32; i++ ) {
        PPC_SWAP_REG_64( (*(&mr->ppc.f0 + i)).u64 );
    }
    // Convert special registers
    PPC_SWAP_REG_64( mr->ppc.iar );
    PPC_SWAP_REG_64( mr->ppc.msr );
    PPC_SWAP_REG_64( mr->ppc.ctr );
    PPC_SWAP_REG_64( mr->ppc.lr );

    CONV_BE_32( mr->ppc.xer );
    CONV_BE_32( mr->ppc.cr );
    CONV_BE_32( mr->ppc.fpscr );
#endif
    return( MS_OK );
}

walk_result     DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    walk_result wr;

    if( tk & MTK_INTEGER ) {
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
    return( WR_CONTINUE );
}

mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

size_t DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, char *buff, size_t buff_size )
{
    char        str[80];
    size_t      len;

    if( rsd == &RegSet[CPU_REG_SET] ) {
        switch( MCSystemConfig()->cpu ) {
        case PPC_601:
            strcpy( str, "601" );
            break;
        case PPC_603:
            strcpy( str, "603" );
            break;
        case PPC_604:
            strcpy( str, "604" );
            break;
        case PPC_620:
            strcpy( str, "620" );
            break;
        default:
            str[0] = '\0';
            break;
        }
    } else {
        str[0] = '\0';
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

unsigned        DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    rsd = rsd;

    return( 0 );
}

//static char     DescriptBuff[10];

typedef struct {
    unsigned_16         first_reg_idx;
    unsigned_8          num_regs;
    unsigned_8          disp_type;
} reg_display_entry;

static const reg_display_entry CPUNumeric[] = {
    { IDX_r0,           32, PPCT_H_DWORD },
    { IDX_lr,            4, PPCT_H_DWORD },
    { IDX_cr,            2, PPCT_H_WORD },
    { 0,                 0, 0 }
};

static int FindEntry( const reg_display_entry *tbl, unsigned piece,
                                unsigned *idx, mad_type_handle *type )
{
    for( ;; ) {
        if( tbl->num_regs == 0 )
            return( 0 );
        if( tbl->num_regs > piece )
            break;
        piece -= tbl->num_regs;
        ++tbl;
    }
    *idx = tbl->first_reg_idx + piece;
    *type = tbl->disp_type;
    return( 1 );
}


static mad_status       CPUGetPiece( unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value )
{
    unsigned    idx;

    if( !FindEntry( CPUNumeric, piece, &idx, disp_type ) )
        return( MS_FAIL );
    *reg = &RegList[idx].info;
    if( !(MADState->reg_state[CPU_REG_SET] & CT_EXTENDED) ) {
        if( *disp_type == PPCT_H_DWORD ) {
            *disp_type = PPCT_H_WORD;
            *reg = &RegListHalf[idx - IDX_r0].info;
        }
    }
    //NYI: if extended & ~ 64 bit mode, downshift to 32-bit display.
    //     Also, if 64 <=> 32 bit switch, tell client to redraw window
    if( !(MADState->reg_state[CPU_REG_SET] & CT_HEX) ) {
        switch( *disp_type ) {
        case PPCT_H_DWORD:
            *disp_type = PPCT_H_UINT64;
            break;
        case PPCT_H_WORD:
            *disp_type = PPCT_H_ULONG;
            break;
        }
    }
    *descript_p = (*reg)->name;
    *max_descript_p = 0;
    *max_value = 0;
    return( MS_OK );
}

static const reg_display_entry FPUList[] = {
    { IDX_f0,           32, PPCT_H_DOUBLE },
    { IDX_fpscr,         1, PPCT_H_WORD },
    { 0,                 0, 0 }
};

static mad_status       FPUGetPiece( unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value )
{
    unsigned    idx;

    if( !FindEntry( FPUList, piece, &idx, disp_type ) )
        return( MS_FAIL );
    *reg = &RegList[idx].info;
    if( MADState->reg_state[FPU_REG_SET] & FT_HEX ) {
        switch( *disp_type ) {
        case PPCT_H_DOUBLE:
            *disp_type = PPCT_H_HDOUBLE;
            break;
        }
    }
    *descript_p = (*reg)->name;
    *max_descript_p = 0;
    *max_value = 0;
    return( MS_OK );
}

mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                mad_registers const *mr,
                                unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value )
{
    mr = mr;

    return( rsd->get_piece( piece, descript_p, max_descript_p, reg, disp_type, max_value ) );
}

static const mad_modify_list    WordReg = { NULL, PPCT_H_WORD, MAD_MSTR_NIL };
static const mad_modify_list    DWordReg = { NULL, PPCT_H_DWORD, MAD_MSTR_NIL };
static const mad_modify_list    FltReg = { NULL, PPCT_H_DOUBLE, MAD_MSTR_NIL };

mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, int *num_possible_p )
{
    rsd = rsd;

    *num_possible_p = 1;
    switch( ri->type ) {
    case PPCT_H_DOUBLE:
        *possible_p = &FltReg;
        break;
    case PPCT_H_DWORD:
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

    rsd = rsd;

    if( ri->bit_start == BIT_OFF( iar ) ) {
        new_ip = old->ppc.iar;
        //NYI: 64 bit
        new_ip.u._32[I64LO32] += sizeof( unsigned_32 );
        if( new_ip.u._32[I64LO32] != cur->ppc.iar.u._32[I64LO32] ) {
            return( MS_MODIFIED_SIGNIFICANTLY );
        } else if( old->ppc.iar.u._32[I64LO32] != cur->ppc.iar.u._32[I64LO32] ) {
            return( MS_MODIFIED );
        }
    } else {
        p_old = (unsigned_8 *)old + BYTEIDX( ri->bit_start );
        p_cur = (unsigned_8 *)cur + BYTEIDX( ri->bit_start );
        size = ri->bit_size;
        if( size >= BYTES2BITS( 1 ) ) {
            /* it's going to be byte aligned */
            return( memcmp( p_old, p_cur, BITS2BYTES( size ) ) != 0 ? MS_MODIFIED_SIGNIFICANTLY : MS_OK );
        } else {
            mask = (1 << size) - 1;
            #define GET_VAL( w )    ((*p_##w >> BITIDX( ri->bit_start )) & mask)
            return( GET_VAL( old ) != GET_VAL( cur ) ? MS_MODIFIED_SIGNIFICANTLY : MS_OK );
        }
    }
    return( MS_OK );
}

mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, mad_registers const *mr, address *a )
{
    unsigned    bit_start;
    unsigned_64 *p;

    memset( a, 0, sizeof( *a ) );
    bit_start = ri->bit_start;
    if( bit_start == BIT_OFF( iar ) ) {
        a->mach.offset = mr->ppc.iar.u._32[I64LO32];
        return( MS_OK );
    }
    if( IS_FP_BIT( bit_start ) ) {
        return( MS_FAIL );
    }
    p = (unsigned_64 *)((unsigned_8 *)mr + BYTEIDX( bit_start ));
    a->mach.offset = p->u._32[I64LO32];
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
//    unsigned    old;

    toggle = on & off;
    index = (unsigned)( rsd - &RegSet[CPU_REG_SET] );
    bits = &MADState->reg_state[index];
//    old = *bits;
    *bits ^= toggle;
    *bits |= on & ~toggle;
    *bits &= ~off | toggle;
    return( *bits );
}

walk_result     DIGENTRY MIRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    const ppc_reg_info  *curr;
    walk_result         wr;
    unsigned            reg_set;

    if( ri != NULL ) {
        switch( ((ppc_reg_info *)ri)->sublist_code ) {
        case RS_DWORD:
           curr = RegSubList[TYPEIDX( ri->bit_start, unsigned_64 )];
           break;
        default:
            curr = SubList[((ppc_reg_info *)ri)->sublist_code];
            break;
        }
        if( curr != NULL ) {
            for( ; curr->info.name != NULL; ++curr ) {
                wr = wk( &curr->info, 0, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    } else {
        reg_set = (unsigned)( rsd - RegSet );
        for( curr = RegList; curr < &RegList[IDX_LAST_ONE]; ++curr ) {
            if( curr->reg_set == reg_set ) {
                wr = wk( &curr->info, curr->sublist_code != 0, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    }
    return( WR_CONTINUE );
}

void            DIGENTRY MIRegSpecialGet( mad_special_reg sr, mad_registers const *mr, addr_ptr *ma )
{
    ma->segment = 0;
    switch( sr ) {
    case MSR_IP:
        ma->offset = mr->ppc.iar.u._32[I64LO32];
        break;
    case MSR_SP:
        ma->offset = mr->ppc.u1.sp.u._32[I64LO32];
        break;
    case MSR_FP:
        //NYI: can actually float around
        ma->offset = mr->ppc.r31.u._32[I64LO32];
        break;
    }
}

void            DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, addr_ptr const *ma )
{
    switch( sr ) {
    case MSR_IP:
        mr->ppc.iar.u._32[I64LO32] = ma->offset;
        break;
    case MSR_SP:
        mr->ppc.u1.sp.u._32[I64LO32] = ma->offset;
        break;
    case MSR_FP:
        //NYI: can actually float around
        mr->ppc.r31.u._32[I64LO32] = ma->offset;
        break;
    }
}

size_t DIGENTRY MIRegSpecialName( mad_special_reg sr, mad_registers const *mr, mad_address_format af, char *buff, size_t buff_size )
{
    unsigned    idx;
    size_t      len;
    char const  *p;

    af = af; mr = mr;

    switch( sr ) {
    case MSR_IP:
        idx = IDX_iar;
        break;
    case MSR_SP:
        idx = IDX_sp;
        break;
    case MSR_FP:
        idx = IDX_r31;
        break;
    default:
        idx = 0;
        break;
    }
    p = RegList[idx].info.name;
    len = strlen( p );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, p, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}


const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
{
    const mad_reg_info  *reg;

    reg = NULL;
    switch( ci ) {
    case CI_PPC_lr:
        reg = &RegList[IDX_lr].info;
        break;
    case CI_PPC_ctr:
        reg = &RegList[IDX_ctr].info;
        break;
    case CI_PPC_iar:
        reg = &RegList[IDX_iar].info;
        break;
    case CI_PPC_cr:
        reg = &RegList[IDX_cr].info;
        break;
    case CI_PPC_xer:
        reg = &RegList[IDX_xer].info;
        break;
    case CI_PPC_fpscr:
        reg = &RegList[IDX_fpscr].info;
        break;
    default:
        if( ci >= CI_PPC_r0 && ci <= CI_PPC_r31 ) {
            reg = &RegList[ci - CI_PPC_r0 + IDX_r0].info;
        } else if( ci >= CI_PPC_f0 && ci <= CI_PPC_f31 ) {
            reg = &RegList[ci - CI_PPC_f0 + IDX_f0].info;
        }
    }
    return( reg );
}

void            DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    mr = mr; flags = flags; bit_start = bit_start; bit_size = bit_size;
}

void            DIGENTRY MIRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    unsigned    i;
    unsigned    bit_end;

    mr = mr; flags = flags;

    bit_end = bit_start + bit_size;
    #define IN_RANGE( i, bit )  \
      ((bit) >= RegList[i].info.bit_start && (bit) < (unsigned)( RegList[i].info.bit_start + RegList[i].info.bit_size ))
    for( i = 0; i < IDX_LAST_ONE; ++i ) {
        if( IN_RANGE( i, bit_start ) || IN_RANGE( i, bit_end ) ) {
            MCNotify( MNT_MODIFY_REG, (void *)&RegSet[RegList[i].reg_set] );
            break;
        }
    }
    switch( bit_start ) {
    case BIT_OFF( iar ):
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case BIT_OFF( u1 ): // sp
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case BIT_OFF( r31 ): // fp NYI: can float
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    }
}

static mad_status AddSubList( unsigned idx, const sublist_data *sub, unsigned num )
{
    unsigned    i;
    unsigned    j;

    i = TYPEIDX( RegList[idx].info.bit_start, unsigned_64 );
    if( RegSubList[i] != NULL )
        return( MS_OK );
    RegSubList[i] = MCAlloc( sizeof( ppc_reg_info ) * ( num + 1 ) );
    if( RegSubList[i] == NULL )
        return( MS_ERR|MS_NO_MEM );
    memset( RegSubList[i], 0, sizeof( ppc_reg_info ) * ( num + 1 ) );
    for( j = 0; j < num; ++j ) {
        RegSubList[i][j] = RegList[idx];
        RegSubList[i][j].info.name       = sub[j].name;
        RegSubList[i][j].info.type       = sub[j].mth;
        RegSubList[i][j].info.bit_start += sub[j].start;
        RegSubList[i][j].info.bit_size   = (unsigned_8)TypeArray[sub[j].mth].u.b->bits;
        RegSubList[i][j].sublist_code    = RS_NONE;
    }
    return( MS_OK );
}

mad_status RegInit()
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
            curr = TYPEIDX( RegList[i].info.bit_start, unsigned_64 );
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
    RegSubList = MCAlloc( ( max + 1 ) * sizeof( *RegSubList ) );
    if( RegSubList == NULL )
        return( MS_ERR | MS_NO_MEM );
    memset( RegSubList, 0, ( max + 1 ) * sizeof( *RegSubList ) );
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_DWORD:
            ms = AddSubList( i, IntRegSubData, NUM_ELTS( IntRegSubData ) );
            if( ms != MS_OK )
                return( ms );
            break;
        }
    }
    return( MS_OK );
}

void RegFini()
{
    unsigned    i;
    unsigned    max;
    unsigned    curr;

    max = 0;
    for( i = 0; i < NUM_ELTS( RegList ); ++i ) {
        switch( RegList[i].sublist_code ) {
        case RS_DWORD:
            curr = TYPEIDX( RegList[i].info.bit_start, unsigned_64 );
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
