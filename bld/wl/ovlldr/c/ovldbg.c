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
* Description:  Overlay debugger support.
*
****************************************************************************/


#include <stddef.h>
#include <dos.h>
#if defined( OVL_MULTITHREAD )
#include "novlldr.h"
#elif defined( OVL_WHOOSH )
#include "novlldr.h"
#else
#include "ovlstd.h"
#endif
#include "ovlldr.h"
#include "trpovl.h"


typedef unsigned char byte;

#ifdef OVL_WHOOSH
typedef struct {
    unsigned    location;
    unsigned    section;
    unsigned    bitsize;
} ovl_dbg_info;

extern ovl_dbg_info _CODE_BASED __OVLDBGINFO__;
#else
extern unsigned     _CODE_BASED __BankStack__;
#endif


static int GetSizeOverlays( void )
/********************************/
// This returns the size of the overlay state table.
{
    unsigned    number;

    number = __OVLTABEND__ - __OVLTAB__.entries;
#ifdef OVL_WHOOSH
    __OVLDBGINFO__.bitsize = ( number + 7 ) / 8;
#endif
    return( ( ( number + 7 ) / 8 ) + sizeof( unsigned ) + 1 );
}

static bool GetSectionData( ovl_address __far *data )
/***************************************************/
{
    unsigned            number;
    ovltab_entry_ptr    ovl;

    number = __OVLTABEND__ - __OVLTAB__.entries;
    if( ( data->sect_id > number ) || ( data->sect_id == 0 ) )
        return( false );
    ovl = __OVLTAB__.entries + data->sect_id - 1;
    data->sect_id = ovl->num_paras;
#ifdef OVL_WHOOSH
    data->mach.segment = __OVLTAB__.prolog.delta + __OVLSTARTPARA__;
#else
    data->mach.segment = ovl->code_handle;
#endif
    data->mach.offset = 0;
    return( true );
}

static bool SaveOvlState( unsigned char __far *data )
/***************************************************/
// this fills a bit array with the status of the overlays
// 1 means overlay in memory, 0 means overlay on disk
{
    ovltab_entry_ptr    ovl;
    unsigned char       mask;
    unsigned char       loaded;
    unsigned char __far *savedata;

    savedata = data;
    mask = 1;
    loaded = 0;
    WALK_ALL_OVL( ovl ) {
        if( ovl->flags_anc & FLAG_INMEM ) {
            loaded |= mask;
        }
        *data = loaded;
        if( mask == 0x80 ) {
            mask = 1;
            loaded = 0;
            ++data;
        } else {
            mask <<= 1;
        }
    }
    if( mask != 1 )
        ++data;
#ifndef OVL_WHOOSH
    *(unsigned __far *)data = __BankStack__;
#else
    *data = 1;
    if( __OVLFLAGS__ & DBGAREA_VALID ) {
        savedata += ( __OVLDBGINFO__.section - 1 ) / 8;
        *savedata |= 1 << ( __OVLDBGINFO__.section - 1 ) % 8;
    }
#endif
    return( true );
}

static bool RestoreOvlState( unsigned char __far *data )
/******************************************************/
// set the overlay state to match the given vector.
{
    ovltab_entry_ptr    ovl;
    unsigned char       mask;
#ifdef OVL_WHOOSH
    unsigned            ovlnum;
    unsigned            flags_save;
    unsigned            code_save;
    unsigned_16         start_save;

/* If we want to load a section for the debugger to look at, do it in a special
 * area so it doesn't cause the overlay loader to go tracing through the
 * debugger stack. (or run out of memory) */
    if( *( data + __OVLDBGINFO__.bitsize ) == 0 ) {
        ovlnum = 1;
        for( ; *data == 0; ++data ) {
            ovlnum += 8;
        }
        for( mask = *data >> 1; mask != 0; mask >>= 1 ) {
            ovlnum++;
        }
        ovl = __OVLTAB__.entries + ovlnum - 1;
        if( (ovl->flags_anc & FLAG_INMEM) == 0 ) {
            if( (__OVLFLAGS__ & DBGAREA_VALID) && ( __OVLDBGINFO__.section != ovlnum ) ) {
                __OVLTAB__.entries[__OVLDBGINFO__.section - 1].flags_anc |= FLAG_CHANGED;
            }
            __OVLDBGINFO__.section = ovlnum;
            code_save = ovl->code_handle;
            flags_save = ovl->flags_anc;
            start_save = ovl->start_para;
            ovl->code_handle = __OVLDBGINFO__.location;
            ovl->start_para = __OVLSTARTPARA__;
            __LoadSectionCode__( ovl );
            ovl->code_handle = code_save;
            ovl->flags_anc = flags_save | FLAG_CHANGED;
            ovl->start_para = start_save;
            __OVLFLAGS__ |= DBGAREA_LOADED | DBGAREA_VALID;
        }
        return( true );
    }
#endif
    mask = 1;
    WALK_ALL_OVL( ovl ) {
#ifndef OVL_WHOOSH
        if( (ovl->flags_anc & FLAG_INMEM) == 0 && (*data & mask) ) {
            NAME( LoadOverlay )( OVLNUM( ovl ) );
        }
#else
        if( (ovl->flags_anc & FLAG_INMEM) == 0 != (*data & mask) == 0 ) {
            /* our overlay state doesn't match the one given to us... so
               we tell the debugger to forget it. */
            return( false );
        }
#endif
        if( mask == 0x80 ) {
            mask = 1;
            ++data;
        } else {
            mask <<= 1;
        }
    }
#ifndef OVL_WHOOSH
    if( mask != 1 )
        ++data;
    if( *(unsigned __far *)data != 0 )
        __BankStack__ = *(unsigned __far *)data;
#else
    __OVLFLAGS__ &= ~DBGAREA_VALID;
#endif
    return( true );
}

static bool CheckVecAddr( ovl_address __far *data )
/*************************************************/
// check if the address stored in data is a vector, returning true if it is.
{
    vector_ptr          vect;
#ifdef OVL_WHOOSH
    ovltab_entry_ptr    ovl;
#endif

    if( data->mach.segment != FP_SEG( __OVLSTARTVEC__ ) )
        return( false );
    vect = (vector_ptr)data->mach.offset;
    if( FP_OFF( vect ) < FP_OFF( __OVLSTARTVEC__ ) )
        return( false );
    if( !OVLVEC_OK( vect ) )
        return( false );
    if( ( FP_OFF( vect ) - FP_OFF( __OVLSTARTVEC__ ) ) % sizeof( vector ) != 0 )
        return( false );
#ifdef OVL_SMALL
    data->mach.segment = FP_SEG( vect );
    data->mach.offset = vect->target + FP_OFF( &vect->target ) + sizeof( vect->target );
    data->sect_id = vect->sec_num;
#elif defined( OVL_WHOOSH )
    if( vect->u.i.cs_over == OVV_CS_OVERRIDE ) {
        data->sect_id = ((ovltab_entry_ptr)vect->u.i.tab_addr - __OVLTAB__.entries) + 1;
        data->mach.segment = vect->target.seg;
    } else {
        data->sect_id = vect->u.v.sec_num;
        ovl = __OVLTAB__.entries + vect->u.v.sec_num - 1;
        if( ( __OVLFLAGS__ & DBGAREA_VALID ) && ( data->sect_id == __OVLDBGINFO__.section ) ) {
            data->mach.segment = __OVLDBGINFO__.location;
        } else if( ovl->flags_anc & FLAG_DBG_SECT_LOAD ) {
            data->mach.segment = __OVLDBGINFO__.location;
        } else if( ovl->flags_anc & FLAG_RET_TRAP ) {
            ret_trap_ptr        rt;

            rt = MK_FP( ovl->code_handle, 0 );
            data->mach.segment = rt->old_code_handle;
        } else if( ovl->code_handle != 0 ) {
            data->mach.segment = ovl->code_handle;
        } else {
            data->mach.segment = __OVLSTARTPARA__ + __OVLTAB__.prolog.delta;
        }
    }
    data->mach.offset = vect->target.off;
#else
    data->mach.segment = vect->target.seg;
    data->mach.offset = vect->target.off;
    data->sect_id = vect->u.v.sec_num;
#endif
    return( true );
}

#ifdef OVL_WHOOSH

static bool GetChangedSections( ovl_address __far *data )
/*******************************************************/
/* return true if a section changed. return the section number and the new
 * segment in that memory pointed to by data */
{
    ovltab_entry_ptr    ovl;
    unsigned            ovl_num;

     if( ( __OVLFLAGS__ & DBGAREA_LOADED ) && ( __OVLFLAGS__ & DBGAREA_VALID ) ) {
        ovl_num = __OVLDBGINFO__.section;
        if( ovl_num == data->sect_id ) {
            __OVLFLAGS__ &= ~DBGAREA_LOADED;
            data->sect_id = 0;
        } else {
            data->sect_id = ovl_num;
            data->mach.offset = 0;
            data->mach.segment = __OVLDBGINFO__.location;
            __OVLTAB__.entries[ovl_num - 1].flags_anc &= ~FLAG_CHANGED;
            __OVLTAB__.entries[ovl_num - 1].flags_anc |= FLAG_DBG_SECT_LOAD;
            return( true );
        }
    }
    for( ovl = __OVLTAB__.entries + data->sect_id; OVLTAB_OK( ovl ); ++ovl ) {
        if( ovl->flags_anc & FLAG_CHANGED ) {
            ovl->flags_anc &= ~( FLAG_CHANGED | FLAG_DBG_SECT_LOAD );
            data->sect_id = OVLNUM( ovl );
            data->mach.offset = 0;
            if( ovl->flags_anc & FLAG_RET_TRAP ) {
                ret_trap_ptr    rt;

                rt = MK_FP( ovl->code_handle, 0 );
                data->mach.segment = rt->old_code_handle;
                return( true );
            }
            data->mach.segment = ovl->code_handle;
            return( true );
        }
    }
    return( false );
}

unsigned __near __OVLMAXSECT__( void )
/***********************************/
// This returns the size of the largest overlay section.
{
    ovltab_entry_ptr    ovl;
    unsigned            max;

    max = 0;
    WALK_ALL_OVL( ovl ) {
        if( ovl->num_paras > max ) {
            max = ovl->num_paras;
        }
    }
    return( max );
}
#endif

int __far GNAME( DBG_HANDLER )( int service, void __far *data )
/*************************************************************/
{
    int ret;

    ret = 0;
    switch( service ) {
    case OVLDBG_GET_OVERLAY_STATE:
        ret = SaveOvlState( data );
        break;
    case OVLDBG_SET_OVERLAY_STATE:
        ret = RestoreOvlState( data );
        break;
    case OVLDBG_TRANSLATE_VECTOR_ADDR:
        ret = CheckVecAddr( data );
        break;
    case OVLDBG_GET_STATE_SIZE:
        ret = GetSizeOverlays();
        break;
    case OVLDBG_TRANSLATE_RETURN_ADDR:
        ret = GNAME( CheckRetAddr )( data );
        break;
    case OVLDBG_GET_OVL_TBL_ADDR:
        *(void __far *__far *)data = &__OVLTAB__;
        break;
#ifdef OVL_WHOOSH
    case OVLDBG_GET_MOVED_SECTION:
        ret = GetChangedSections( data );
        break;
#endif
    case OVLDBG_GET_SECTION_DATA:
        ret = GetSectionData( data );
        break;
    }
    return( ret );
}
