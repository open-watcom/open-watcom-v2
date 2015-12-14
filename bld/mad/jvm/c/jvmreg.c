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
#include "jvm.h"
#include "jvmtypes.h"
#include "madregs.h"

#define BIT_OFF( who ) (offsetof( mad_registers, jvm.who ) * BITS_PER_BYTE)

enum {
    RS_NONE,
    RS_NUM,
};

#define REG_NAME( name )        const char NAME_##name[] = #name

/* to avoid relocations to R/W data segments */
#define regpick( name, type, s ) REG_NAME( name );
#include "jvmregs.h"
#undef regpick

#define regpick( name, type, reg_set )  \
        { { NAME_##name,                \
            JVMT_##type,                \
            BIT_OFF( name ),            \
            32,                         \
            RS_NONE },                  \
        },

const jvm_reg_info RegList[] = {
    #include "jvmregs.h"
};

static const mad_toggle_strings CPUToggleList[] =
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

static const mad_reg_set_data RegSet[] = {
    { CPUGetPiece, CPUToggleList, MAD_MSTR_CPU },
};

/*
        Return the size of the structure required to hold a register set.
*/
unsigned        DIGENTRY MIRegistersSize( void )
{
    return( sizeof( struct jvm_mad_registers ) );
}

/*
        Perform any conversions on the register set after it's been
        received from the trap file. Return MS_OK if nothing was actually
        modified, MS_MODIFIED if some changes were made.
*/
mad_status      DIGENTRY MIRegistersHost( mad_registers *mr )
{
    return( MS_OK );
}


/*
        Perform any conversions on the register set required before sending
        it to the trap file (undo the effects of a MIRegistersHost). Return
        MS_OK if nothing was actually changed, MS_MODIFIED otherwise.
*/
mad_status      DIGENTRY MIRegistersTarget( mad_registers *mr )
{
    return( MS_OK );
}

/*
        Call back to the client routine with pointers to the various
        reg set structure(s).
*/
walk_result     DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    walk_result wr;

    if( tk & MTK_INTEGER ) {
        wr = wk( &RegSet[CPU_REG_SET], d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    return( WR_CONTINUE );
}

/*
        Return the name of the register set.
*/
mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    return( rsd->name );
}

/*
        Return the register set level string (8086, 186, 286, etc).
*/
size_t DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, char *buff, size_t buff_size )
{
    if( buff_size > 0 )
        *buff = '\0';
    return( 0 );
}

/*
        Return the register set display grouping. A register set is either
        displayed as a stream of values that the client displays however it
        wishes (return value 0), or as an n X m grid where 'n' is the number
        of rows and 'm' is the number of columns. The grouping is the number
        of columns (return value m).
*/
unsigned        DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    return( 0 );
}



static mad_status       CPUGetPiece( unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value )
{
    if( piece >= sizeof( RegList ) / sizeof( RegList[0] ) )
        return( MS_FAIL );
    *reg = &RegList[piece].info;
    *descript_p = (*reg)->name;
    *max_descript_p = strlen( *descript_p );
    *disp_type = (*reg)->type;
    *max_value = 0;
    return( MS_OK );
}

/*
        Fill in the information to display one register.
*/
mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                const mad_registers *mr,
                                unsigned piece,
                                const char **descript_p,
                                size_t *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                size_t *max_value )
{
    return( rsd->get_piece( piece, descript_p, max_descript_p, reg, disp_type, max_value ) );
}

static const mad_modify_list    DWordReg = { NULL, JVMT_N32_PTR, MAD_MSTR_NIL };

/*
        A particular register has been selected for modification. Indicate
        what are the legal ranges of values for it.
*/
mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, int *num_possible_p )
{
    *num_possible_p = 1;
    *possible_p = &DWordReg;
    return( MS_OK );
}

/*
        Query whether a register value differs between the two register
        sets 'old' and 'cur'. The following return codes are possible:

                MS_OK                           - value is unchanged
                MS_MODIFIED                     - value is different
                MS_MODIFIED_SIGNIFICANTLY       - value has changed
                                                  in a way that the user
                                                  cares about

*/
mad_status DIGENTRY MIRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *cur )
{
    addr_ptr    new_ip;
    unsigned_8  *p_old;
    unsigned_8  *p_cur;
    unsigned    mask;
    unsigned    size;

    if( ri->bit_start == BIT_OFF( pc ) ) {
        new_ip = old->jvm.pc;
        //NYI: find length of instruction
        new_ip.offset += sizeof( unsigned_32 );
        if( new_ip.segment != cur->jvm.pc.segment ||
            new_ip.offset != cur->jvm.pc.offset ) {
            return( MS_MODIFIED_SIGNIFICANTLY );
        } else if( old->jvm.pc.segment != cur->jvm.pc.segment ||
                   old->jvm.pc.offset != cur->jvm.pc.offset ) {
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

/*
        Given a register, fill in 'a' with the address that the register
        points at.
*/
mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    unsigned    bit_start;
    unsigned_32 *p;

    memset( a, 0, sizeof( *a ) );
    bit_start = ri->bit_start;
    p = (unsigned_32 *)((unsigned_8 *)mr + (bit_start / BITS_PER_BYTE));
    a->mach.offset = *p;
    return( MS_OK );
}

/*
        Return the toggle list strings for the register set.
*/
const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    return( rsd->togglelist );
}

/*
        Get/Set the register set toggle bits. For a particular bit, the
        following algorithm is used:

                on      off     result
                --      ---     ------
                0       0       bit left unchanged
                1       0       bit set to 1
                0       1       bit set to 0
                1       1       bit toggled from previous value

        (it operates the same as a JK flip-flop).
        Return the new toggle state.
*/
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
    return( *bits );
}

/*
        Walk a register set/register, calling back the client with
        mad_reg_info structures for all the registers contained in the
        register set/register.

        If 'ri' is non-NULL, walk all the sub registers of the
        indicated register, otherwise walk all the registers of
        the register set 'rsd'.
*/

walk_result     DIGENTRY MIRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    walk_result         wr;

    if( ri == NULL ) {
        wr = wk( &RegList[CPU_REG_SET].info, 0, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    return( WR_CONTINUE );
}

/*
        Get the address pointed at by the following special registers:

                MSR_IP  - instruction pointer
                MSR_SP  - stack pointer
                MSR_FP  - frame pointer
*/
void            DIGENTRY MIRegSpecialGet( mad_special_reg sr, const mad_registers *mr, addr_ptr *ma )
{
    ma->segment = 0;
    switch( sr ) {
    case MSR_IP:
        ma->offset = mr->jvm.pc.offset;
        ma->segment = mr->jvm.pc.segment;
        break;
    case MSR_SP:
        ma->offset = mr->jvm.optop;
        break;
    case MSR_FP:
        ma->offset = mr->jvm.frame.offset;
        break;
    }
}

/*
        Set a special register to the address 'a'.
*/
void            DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *ma )
{
    switch( sr ) {
    case MSR_IP:
        mr->jvm.pc.offset = ma->offset;
        mr->jvm.pc.segment = ma->segment;
        break;
    case MSR_SP:
        mr->jvm.optop = ma->offset;
        break;
    case MSR_FP:
        mr->jvm.frame.offset = ma->offset;
        break;
    }
}

/*
        Get the name of a special register.
*/
size_t DIGENTRY MIRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, char *buff, size_t buff_size )
{
    unsigned    idx;
    size_t      len;
    const char  *p;

    switch( sr ) {
    case MSR_IP:
        idx = IDX_pc;
        break;
    case MSR_SP:
        idx = IDX_optop;
        break;
    case MSR_FP:
        idx = IDX_frame;
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


/*
        Convert a DIP context item enumeration to a MAD mad_reg_info
        pointer.
*/
const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
{
    const mad_reg_info  *reg;

    reg = NULL;
    switch( ci ) {
    case CI_JVM_pc:
        reg = &RegList[IDX_pc].info;
        break;
    case CI_JVM_optop:
        reg = &RegList[IDX_optop].info;
        break;
    case CI_JVM_frame:
        reg = &RegList[IDX_frame].info;
        break;
    case CI_JVM_vars:
        reg = &RegList[IDX_vars].info;
        break;
    }
    return( reg );
}

/*
        The client is about to modify a register (or registers).
*/
void            DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
}

/*
        The client has just finished modifying a register (or registers).
        The MAD should take this chance to notify the client on what displays
        need to be updated. MCNotify should be called for the following

                MNT_MODIFY_REG  - to notify client which register set has
                                  changed (2d parm is register set pointer).
                MNT_MODIFY_IP   - instruction pointer special reg has changed.
                MNT_MODIFY_SP   - stack pointer special reg has changed.
                MNT_MODIFY_FP   - frame pointer special reg has changed.

*/
void            DIGENTRY MIRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    unsigned    bit_end;

    bit_end = bit_start + bit_size;
    MCNotify( MNT_MODIFY_REG, (void *)&RegSet[CPU_REG_SET] );
    switch( bit_start ) {
    case BIT_OFF( pc ):
        MCNotify( MNT_MODIFY_IP, NULL );
        break;
    case BIT_OFF( optop ):
        MCNotify( MNT_MODIFY_SP, NULL );
        break;
    case BIT_OFF( frame ):
        MCNotify( MNT_MODIFY_FP, NULL );
        break;
    }
}

mad_status RegInit()
{
    return( MS_OK );
}

void RegFini()
{
}
