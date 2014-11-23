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
#include "xmp.h"
#include "xmptypes.h"
#include "madregs.h"

enum {
    RS_NONE,
    RS_NUM,
};

/*
        Return the size of the structure required to hold a register set.
*/
unsigned        DIGENTRY MIRegistersSize( void )
{
    //NYI:
    return( 0 );
}

/*
        Perform any conversions on the register set after it's been
        received from the trap file. Return MS_OK if nothing was actually
        modified, MS_MODIFIED if some changes were made.
*/
mad_status      DIGENTRY MIRegistersHost( mad_registers *mr )
{
    //NYI:
    return( MS_OK );
}


/*
        Perform any conversions on the register set required before sending
        it to the trap file (undo the effects of a MIRegistersHost). Return
        MS_OK if nothing was actually changed, MS_MODIFIED otherwise.
*/
mad_status      DIGENTRY MIRegistersTarget( mad_registers *mr )
{
    //NYI:
    return( MS_OK );
}

/*
        Call back to the client routine with pointers to the various
        reg set structure(s).
*/
walk_result     DIGENTRY MIRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    //NYI:
    return( WR_CONTINUE );
}

/*
        Return the name of the register set.
*/
mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data *rsd )
{
    //NYI:
    return( MAD_MSTR_NIL );
}

/*
        Return the register set level string (8086, 186, 286, etc).
*/
unsigned        DIGENTRY MIRegSetLevel( const mad_reg_set_data *rsd, char *buff, unsigned buff_size )
{
    //NYI:
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

/*
        Fill in the information to display one register.
*/
mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *rsd,
                                const mad_registers *mr,
                                unsigned piece,
                                const char **descript_p,
                                unsigned *max_descript_p,
                                const mad_reg_info **reg,
                                mad_type_handle *disp_type,
                                unsigned *max_value )
{
    //NYI:
    return( MS_FAIL );
}

/*
        A particular register has been selected for modification. Indicate
        what are the legal ranges of values for it.
*/
mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_modify_list **possible_p, int *num_possible_p )
{
    //NYI:
    return( MS_FAIL );
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
    //NYI:
    return( MS_FAIL );
}

/*
        Given a register, fill in 'a' with the address that the register
        points at.
*/
mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    //NYI:
    return( MS_FAIL );
}

/*
        Return the toggle list strings for the register set.
*/
const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    //NYI:
    return( 0 );
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
    //NYI:
    index = 0; // rsd - &RegSet[CPU_REG_SET];
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
    //NYI:
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
    //NYI:
}

/*
        Set a special register to the address 'a'.
*/
void            DIGENTRY MIRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *ma )
{
    //NYI:
}

/*
        Get the name of a special register.
*/
unsigned        DIGENTRY MIRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, char *buff, unsigned buff_size )
{
    //NYI:
    return( 0 );
}


/*
        Convert a DIP context item enumeration to a MAD mad_reg_info
        pointer.
*/
const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item ci )
{
    //NYI:
    return( NULL );
}

/*
        The client is about to modify a register (or registers).
*/
void            DIGENTRY MIRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    //NYI:
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
    //NYI:
}
