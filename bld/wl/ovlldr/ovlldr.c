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



/* OVLLDR:       Overlay loader.
*/

#include <dos.h>
#include <stddef.h>
#include "ovlstd.h"

extern void             NAME( OVLLDR )(void);
extern void near        NAME( OVLMUNGE )(void *);
extern void             (far * far NAME( DBG_HOOK ) )();
extern void near        NAME( CHPOVLLDR )( void );
#define LOADOVERLAY     NAME( LoadOverlay )

static void ClearInMemFlags( ovltab_entry *loaded_ovl )
//=====================================================
// Turn off the IN_MEM flag in the overlay table for those sections
// that overlap with the loaded section.
{
    ovltab_entry *  ovl;
    int             ovl_num;
    unsigned        end;
    ovltab_entry *  anc;
    int             anc_num;

    end = loaded_ovl->start_para + loaded_ovl->num_paras;
    ovl_num = 1;
    ovl = __OVLTAB__.entries;
    for(;;) {
        if( ovl->flags_anc == OVLTAB_TERMINATOR ) break;
        if( ovl->flags_anc & FLAG_INMEM ) {
            if( ovl->start_para < end
                && ovl->start_para + ovl->num_paras > loaded_ovl->start_para ) {
                ovl->flags_anc &= ~FLAG_INMEM;
                DeMungeVectors( ovl_num );
            } else {
                /* If a section has ancestors, they all have to be in memory
                    or else we must unload this section. */
                anc = ovl;
                for( ;; ) {
                    anc_num = anc->flags_anc & FLAG_ANC_MASK;
                    if( anc_num == 0 ) break;
                    anc = &__OVLTAB__.entries[ anc_num - 1 ];
                    if( (anc->flags_anc & FLAG_INMEM) == 0 ) {
                        ovl->flags_anc &= ~FLAG_INMEM;
                        DeMungeVectors( ovl_num );
                        break;
                    }
                }
            }
        }
        ovl++;
        ovl_num++;
    }
}

int near LOADOVERLAY( unsigned int ovl_num )
//==========================================
// Load specified overlay.
{
    ovltab_entry *  ovl;
    tiny_ret_t      status;
    tiny_handle_t   fp;
    int             loaded_something;

    loaded_something = 0;
    while( ovl_num != 0 ) { // load overlay and all its ancestors
        ovl = &__OVLTAB__.entries[ ovl_num - 1 ];
#ifdef OVL_DEBUG
        __OvlMsg__( OVL_SECTION );
        __OvlNum__( ovl_num );
#endif
        if( (ovl->flags_anc & FLAG_INMEM) == 0 ) {
            loaded_something = 1;
            status = __OpenOvl__( ovl->fname );
            if( TINY_ERROR( status ) ) __OvlExit__( OVL_OPEN_ERR );
            fp = TINY_INFO( status );
            status = __OvlSeek__( fp, ovl->disk_addr );
            if( !TINY_OK( status ) ) {
                __OvlExit__( OVL_IO_ERR );
            }
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
        ovl_num = ovl->flags_anc & FLAG_ANC_MASK;
    }
    return( loaded_something );
}

static void MungeVectors( int ovl_num )
//=====================================
// Copy the jmp instruction on top of the call instruction in the vector table
// so future references to this symbol will bypass the overlay manager.
{
    vector *    vect;

    vect = &__OVLSTARTVEC__;
    while( vect < &__OVLENDVEC__ ) {
#ifdef OVL_SMALL
        if( vect->sec_num == ovl_num ) {
            if( __OVLDOPAR__ ) {
                vect->call_op = CALL_INSTRUCTION;
                vect->ldr_addr = (unsigned) NAME( CHPOVLLDR )
                                             - (unsigned)(&vect->ldr_addr) - 2;
            } else {
                vect->call_op = vect->jmp_op;
                vect->ldr_addr = vect->target
                       + (offsetof(vector,target) - offsetof(vector,ldr_addr));
#else
        if( vect->u.v.sec_num == ovl_num ) {
            if( __OVLDOPAR__ ) {
                vect->u.v.call_op = CALL_INSTRUCTION;
                vect->u.v.ldr_addr = FP_OFF( NAME( CHPOVLLDR ) )
                                             - FP_OFF(&vect->u.v.ldr_addr) - 2;
            } else {
                vect->u.m.big_nop = MOV_AX_AX;
                vect->u.m.test_op = TEST_OPCODE;
#endif
            }
        }
        vect++;
    }
}

static void DeMungeVectors( int ovl_num )
/***************************************/
// overlay not in memory any more, so demunge vectors.
{
    vector *    vect;

    vect = &__OVLSTARTVEC__;
    while( vect < &__OVLENDVEC__ ) {
#ifdef OVL_SMALL
        if( vect->sec_num == ovl_num ) {
            vect->call_op = CALL_INSTRUCTION;
            vect->ldr_addr = (unsigned)NAME(OVLLDR)
                                       - (unsigned)(&vect->ldr_addr) - 2;
#else
        if( vect->u.v.sec_num == ovl_num ) {
            vect->u.v.call_op = CALL_INSTRUCTION;
            vect->u.v.ldr_addr = FP_OFF( NAME( OVLLDR ) )
                                 - FP_OFF(&vect->u.v.ldr_addr) - 2;
#endif
        }
        vect++;
    }
}

void near NAME( OVLLOAD )( int ovl_num )
//======================================
// Load overlay.
{
    if( LOADOVERLAY( ovl_num ) ) {
        NAME( DBG_HOOK )( ovl_num, __OVLISRET__, __OVLCAUSE__ );
    }
}

dos_addr near NAME( OVLTINIT )( void )
//====================================
// Overlay initialization.
{
    ovltab_entry        *ovl;
    unsigned int        ovl_num;

    if( __OVLTAB__.prolog.major != OVL_MAJOR_VERSION
        || __OVLTAB__.prolog.minor > OVL_MINOR_VERSION ) {
        __OvlExit__( OVL_BAD_VERSION );
    }
    __OVLFILEPREV__ = 0xFFFF;
    ovl_num = 1;
    ovl = __OVLTAB__.entries;
    while( ovl->flags_anc != OVLTAB_TERMINATOR ) {
        ovl->code_handle = ovl->start_para + __OVLTAB__.prolog.delta;
        if( ovl->flags_anc & FLAG_PRELOAD ) {
            LOADOVERLAY( ovl_num );
        }
        ovl_num++;
        ovl++;
    }
    return( __OVLTAB__.prolog.start );
}
