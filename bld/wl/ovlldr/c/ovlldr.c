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
* Description:  Overlay loader.
*
****************************************************************************/



#include <dos.h>
#include <stddef.h>
#include "ovlstd.h"

extern void __near      NAME( OVLLDR )( void );
extern void __near      NAME( OVLMUNGE )( void * );
extern void __near      NAME( CHPOVLLDR )( void );
#define LOADOVERLAY     NAME( LoadOverlay )

static void MungeVectors( unsigned ovl_num )
//==========================================
// Copy the jmp instruction on top of the call instruction in the vector table
// so future references to this symbol will bypass the overlay manager.
{
    vector_ptr  vect;

    WALK_ALL_VECT( vect ) {
#ifdef OVL_SMALL
        if( vect->sec_num == ovl_num ) {
            if( __OVLDOPAR__ ) {
                vect->call_op = CALL_INSTRUCTION;
                vect->ldr_addr = FP_OFF( NAME( CHPOVLLDR ) )
                      - FP_OFF( &vect->ldr_addr ) - sizeof( vect->ldr_addr );
            } else {
                vect->call_op = vect->jmp_op;
                vect->ldr_addr = vect->target
                       + (offsetof(vector,target) - offsetof(vector,ldr_addr));
            }
        }
#else
        if( vect->u.v.sec_num == ovl_num ) {
            if( __OVLDOPAR__ ) {
                vect->u.v.call_op = CALL_INSTRUCTION;
                vect->u.v.ldr_addr = FP_OFF( NAME( CHPOVLLDR ) )
                  - FP_OFF( &vect->u.v.ldr_addr ) - sizeof( vect->u.v.ldr_addr );
            } else {
                vect->u.m.big_nop = OVV_MOV_AX_AX;
                vect->u.m.test_op = OVV_TEST_OPCODE;
            }
        }
#endif
    }
}

static void DeMungeVectors( unsigned ovl_num )
/********************************************/
// overlay not in memory any more, so demunge vectors.
{
    vector_ptr  vect;

    WALK_ALL_VECT( vect ) {
#ifdef OVL_SMALL
        if( vect->sec_num == ovl_num ) {
            vect->call_op = CALL_INSTRUCTION;
            vect->ldr_addr = FP_OFF( NAME( OVLLDR ) )
              - FP_OFF( &vect->ldr_addr ) - sizeof( vect->ldr_addr );
        }
#else
        if( vect->u.v.sec_num == ovl_num ) {
            vect->u.v.call_op = CALL_INSTRUCTION;
            vect->u.v.ldr_addr = FP_OFF( NAME( OVLLDR ) )
              - FP_OFF( &vect->u.v.ldr_addr ) - sizeof( vect->u.v.ldr_addr );
        }
#endif
    }
}

static void ClearInMemFlags( ovltab_entry_ptr loaded_ovl )
//========================================================
// Turn off the IN_MEM flag in the overlay table for those sections
// that overlap with the loaded section.
{
    ovltab_entry_ptr    ovl;
    unsigned            end;
    ovltab_entry_ptr    anc;

    end = loaded_ovl->start_para + loaded_ovl->num_paras;
    WALK_ALL_OVL( ovl ) {
        if( ovl->flags_anc & FLAG_INMEM ) {
            if( ovl->start_para < end
                && ovl->start_para + ovl->num_paras > loaded_ovl->start_para ) {
                ovl->flags_anc &= ~FLAG_INMEM;
                DeMungeVectors( OVLNUM( ovl ) );
            } else {
                /* If a section has ancestors, they all have to be in memory
                    or else we must unload this section. */
                anc = ovl;
                for( ; anc->flags_anc & OVE_FLAG_ANC_MASK; ) {
                    anc = __OVLTAB__.entries + (anc->flags_anc & OVE_FLAG_ANC_MASK) - 1;
                    if( (anc->flags_anc & FLAG_INMEM) == 0 ) {
                        ovl->flags_anc &= ~FLAG_INMEM;
                        DeMungeVectors( OVLNUM( ovl ) );
                        break;
                    }
                }
            }
        }
    }
}

int __near LOADOVERLAY( unsigned ovl_num )
//======================================
// Load specified overlay.
{
    ovltab_entry_ptr    ovl;
    tiny_ret_t          status;
    tiny_handle_t       fp;
    int                 loaded_something;

    // load overlay and all its ancestors
    loaded_something = 0;
    for( ; ovl_num != 0; ovl_num = ovl->flags_anc & OVE_FLAG_ANC_MASK ) {
        ovl = __OVLTAB__.entries + ovl_num - 1;
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
#endif
        if( (ovl->flags_anc & FLAG_INMEM) == 0 ) {
            loaded_something = 1;
            status = __OpenOvl__( ovl->fname );
            if( TINY_ERROR( status ) )
                __OvlExit__( OVL_OPEN_ERR );
            fp = TINY_INFO( status );
            if( TINY_ERROR( __OvlSeek__( fp, ovl->disk_addr ) ) )
                __OvlExit__( OVL_IO_ERR );
            __OvlCodeLoad__( ovl, fp );
            __OvlRelocLoad__( ovl, fp );
            ClearInMemFlags( ovl );
            ovl->flags_anc |= FLAG_INMEM;
            MungeVectors( ovl_num );
#ifdef OVL_DEBUG
            __OvlMsg__( OVL_LOADED );
        } else {
            __OvlMsg__( OVL_RESIDENT );
#endif
        }
    }
    return( loaded_something );
}

void __near NAME( OVLLOAD )( unsigned ovl_num )
//===========================================
// Load overlay.
{
    if( LOADOVERLAY( ovl_num ) ) {
        NAME( DBG_HOOK )( ovl_num, __OVLISRET__, __OVLCAUSE__ );
    }
}

dos_addr __near NAME( OVLTINIT )( void )
//====================================
// Overlay initialization.
{
    ovltab_entry_ptr    ovl;

    if( __OVLTAB__.prolog.major != OVL_MAJOR_VERSION
        || __OVLTAB__.prolog.minor > OVL_MINOR_VERSION ) {
        __OvlExit__( OVL_BAD_VERSION );
    }
    __OVLFILEPREV__ = 0xFFFF;
    WALK_ALL_OVL( ovl ) {
        ovl->code_handle = ovl->start_para + __OVLTAB__.prolog.delta;
        if( ovl->flags_anc & OVE_FLAG_PRELOAD ) {
            LOADOVERLAY( OVLNUM( ovl ) );
        }
    }
    return( __OVLTAB__.prolog.start );
}
