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


// OVLDBG:       Overlay debugger support.

#include <stddef.h>
#include <dos.h>
#if defined(OVL_MULTITHREAD)
#include "movlldr.h"
#elif defined(OVL_WHOOSH)
#include "novlldr.h"
#else
#include "ovlstd.h"
#endif

#ifndef FALSE
#define FALSE       0
#define TRUE        (!FALSE)
#endif

typedef struct {
    void    far *addr;
    short   sect;
} ovl_addr;

typedef unsigned char byte;

#if defined( OVL_WHOOSH )
extern int  near __LoadNewOverlay__( int );
extern int  near __LoadSectionCode__( ovltab_entry far * );
extern int  near __NCheckRetAddr__( void far * );
#else
extern int  near NAME( LoadOverlay )( int );
extern int  near NAME( CheckRetAddr )( ovl_addr far * );
#endif

#ifndef OVL_WHOOSH
extern unsigned  far    __BankStack__;
#else
typedef struct {
    unsigned    location;
    unsigned    section;
    unsigned    bitsize;
} ovl_dbg_info;

extern ovl_dbg_info far __OVLDBGINFO__;
#endif

enum {
    GET_SIZE_OVERLAYS,
    SAVE_OVL_STATE,
    RESTORE_OVL_STATE,
    CHK_VEC_ADDR,
    CHK_RET_ADDR,
    GET_OVL_TBL_ADDR,
    GET_CHANGED_SECTIONS,
    GET_SECTION_DATA
};

static int GetSizeOverlays( void )
/********************************/
// This returns the size of the overlay state table.
{
    unsigned    number;

    number = (ovltab_entry far *)&__OVLTABEND__ - __OVLTAB__.entries;
#ifdef OVL_WHOOSH
    __OVLDBGINFO__.bitsize = (number+7) / 8;
#endif
    return( ((number+7) / 8) + sizeof( unsigned ) + 1 );
}

static int GetSectionData( ovl_addr far * data )
/**********************************************/
{
    unsigned            number;
    unsigned            seg;
    ovltab_entry far *  ovl;

    number = (ovltab_entry far *)&__OVLTABEND__ - __OVLTAB__.entries;
    if( data->sect > number || data->sect <= 0 ) return 0;
    ovl = &__OVLTAB__.entries[data->sect - 1];
    data->sect = ovl->num_paras;
#ifdef OVL_WHOOSH
    seg = __OVLTAB__.prolog.delta + __OVLSTARTPARA__;
#else
    seg = ovl->code_handle;
#endif
    data->addr = MK_FP( seg, 0 );
    return 1;
}

static int SaveOvlState( char far * data )
/****************************************/
// this fills a bit array with the status of the overlays
// 1 means overlay in memory, 0 means overlay on disk
{
    ovltab_entry far *  ovl;
    unsigned char       mask;
    unsigned char       loaded;
    char far *          savedata;

    savedata = data;
    mask = 1;
    loaded = 0;
    ovl = &__OVLTAB__.entries;
    while( FP_OFF( ovl ) != FP_OFF( &__OVLTABEND__ ) ) {
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
        ovl++;
    }
    if( mask != 1 ) ++data;
#ifndef OVL_WHOOSH
    *(unsigned *)data = __BankStack__;
#else
    *data = 1;
    if( __OVLFLAGS__ & DBGAREA_VALID ) {
        savedata += (__OVLDBGINFO__.section - 1) / 8;
        *savedata |= 1 << (__OVLDBGINFO__.section - 1) % 8;
    }
#endif
    return( TRUE );
}

static int RestoreOvlState( char far * data )
/*******************************************/
// set the overlay state to match the given vector.
{
    ovltab_entry far *  ovl;
    unsigned char       mask;
    int                 ovlnum;
#ifdef OVL_WHOOSH
    unsigned            flags_save;
    unsigned            code_save;
    unsigned_16         start_save;

/* If we want to load a section for the debugger to look at, do it in a special
 * area so it doesn't cause the overlay loader to go tracing through the
 * debugger stack. (or run out of memory) */
    if( *(data + __OVLDBGINFO__.bitsize) == 0 ) {
        ovlnum = 1;
        while( *data == 0 ) {
            ovlnum += 8;
            data++;
        }
        mask = *data >> 1;
        while( mask != 0 ) {
            mask >>= 1;
            ovlnum++;
        }
        ovl = &__OVLTAB__.entries[ ovlnum - 1 ];
        if( !(ovl->flags_anc & FLAG_INMEM) ) {
            if( ( __OVLFLAGS__ & DBGAREA_VALID )
                             && ( __OVLDBGINFO__.section != ovlnum ) ) {
                __OVLTAB__.entries[__OVLDBGINFO__.section - 1].flags_anc
                                                             |= FLAG_CHANGED;
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
        return( TRUE );
    }
#endif
    ovlnum = 1;
    mask = 1;
    ovl = &__OVLTAB__.entries;
    while( FP_OFF( ovl ) != FP_OFF( &__OVLTABEND__ ) ) {
#ifndef OVL_WHOOSH
        if( !(ovl->flags_anc & FLAG_INMEM) && (*data & mask) ) {
            NAME( LoadOverlay )( ovlnum );
        }
#else
        if( ( ( ovl->flags_anc & FLAG_INMEM ) == 0 ) !=
                                                ( ( *data & mask ) == 0 ) ) {
            /* our overlay state doesn't match the one given to us... so
               we tell the debugger to forget it. */
            return( FALSE );
        }
#endif
        if( mask == 0x80 ) {
            mask = 1;
            ++data;
        } else {
            mask <<= 1;
        }
        ++ovlnum;
        ++ovl;
    }
#ifndef OVL_WHOOSH
    if( mask != 1 ) ++data;
    if( *(unsigned *)data != 0 ) __BankStack__ = *(unsigned *)data;
#else
    __OVLFLAGS__ &= ~DBGAREA_VALID;
#endif
    return( TRUE );
}

static int CheckVecAddr( ovl_addr far * data )
/**********************************************/
// check if the address stored in data is a vector, returning TRUE if it is.
{
    char far *      address;
    vector far *    vect;
    unsigned        addr;
#ifdef OVL_WHOOSH
    ovltab_entry *  ovl;
#endif

    address = data->addr;
    if( FP_SEG( address ) != FP_SEG( &__OVLSTARTVEC__ ) ) return( FALSE );
    addr = FP_OFF( address );
    if( addr < FP_OFF( &__OVLSTARTVEC__ ) ) return( FALSE );
    if( addr >= FP_OFF( &__OVLENDVEC__ ) ) return( FALSE );
    addr -= FP_OFF( &__OVLSTARTVEC__ );
    if( addr % sizeof( vector ) != 0 ) return( FALSE );
    vect = (vector *)address;
#ifdef OVL_SMALL
    data->addr = MK_FP(FP_SEG(address),vect->target+(unsigned)&vect->target+2);
    data->sect = vect->sec_num;
#elif defined( OVL_WHOOSH )
    if( vect->u.i.cs_over == CS_OVERRIDE ) {
        data->sect = (vect->u.i.tab_addr - FP_OFF(__OVLTAB__.entries))
                                                    / sizeof(ovltab_entry) + 1;
        data->addr = MK_FP( vect->target.seg, vect->target.off );
    } else {
        data->sect = vect->u.v.sec_num;
        ovl = &__OVLTAB__.entries[ vect->u.v.sec_num - 1 ];
        if( ( __OVLFLAGS__ & DBGAREA_VALID )
                         && ( data->sect == __OVLDBGINFO__.section ) ) {
            addr = __OVLDBGINFO__.location;
        } else if( ovl->flags_anc & FLAG_DBG_SECT_LOAD ) {
            addr = __OVLDBGINFO__.location;
        } else if( ovl->flags_anc & FLAG_RET_TRAP ) {
            ret_trap_ptr        rt;

            rt = MK_FP( ovl->code_handle, 0 );
            addr = rt->old_code_handle;
        } else if( ovl->code_handle != 0 ) {
            addr = ovl->code_handle;
        } else {
            addr = __OVLSTARTPARA__ + __OVLTAB__.prolog.delta;
        }
        data->addr = MK_FP( addr, vect->target.off);
    }
#else
    data->addr = MK_FP( vect->target.seg, vect->target.off );
    data->sect = vect->u.v.sec_num;
#endif
    return( TRUE );
}

#ifdef OVL_WHOOSH

static int GetChangedSections( ovl_addr far *data )
/*************************************************/
/* return TRUE if a section changed. return the section number and the new
 * segment in that memory pointed to by data */
{
    ovltab_entry *  ovl;
    unsigned        ovl_num;

     if( __OVLFLAGS__ & DBGAREA_LOADED && __OVLFLAGS__ & DBGAREA_VALID ) {
        ovl_num = __OVLDBGINFO__.section;
        if( ovl_num == data->sect ) {
            __OVLFLAGS__ &= ~DBGAREA_LOADED;
            data->sect = 0;
        } else {
            data->sect = ovl_num;
            data->addr = MK_FP( __OVLDBGINFO__.location, 0 );
            __OVLTAB__.entries[ovl_num - 1].flags_anc &= ~FLAG_CHANGED;
            __OVLTAB__.entries[ovl_num - 1].flags_anc |= FLAG_DBG_SECT_LOAD;
            return( TRUE );
        }
    }
    ovl_num = data->sect + 1;
    ovl = &__OVLTAB__.entries[ ovl_num - 1 ];
    while( FP_OFF( ovl ) != FP_OFF( &__OVLTABEND__ ) ) {
        if( ovl->flags_anc & FLAG_CHANGED ) {
            ovl->flags_anc &= ~( FLAG_CHANGED | FLAG_DBG_SECT_LOAD );
            data->sect = ovl_num;
            if( ovl->flags_anc & FLAG_RET_TRAP ) {
                ret_trap_ptr    rt;

                rt = MK_FP( ovl->code_handle, 0 );
                data->addr = MK_FP( rt->old_code_handle, 0 );
                return( TRUE );
            }
            data->addr = MK_FP( ovl->code_handle, 0 );
            return( TRUE );
        }
        ++ovl;
        ++ovl_num;
    }
    return( FALSE );
}

unsigned near __OVLMAXSECT__( void )
/***********************************/
// This returns the size of the largest overlay section.
{
    ovltab_entry far *  ovl;
    unsigned            max;

    max = 0;
    ovl = &__OVLTAB__.entries;
    while( FP_OFF( ovl ) != FP_OFF( &__OVLTABEND__ ) ) {
        if( ovl->num_paras > max ) {
            max = ovl->num_paras;
        }
        ++ovl;
    }
    return( max );
}
#endif

#if defined( OVL_WHOOSH )
int far __NDBG_HANDLER__( int service, void far *data )
#else
int far NAME( DBG_HANDLER )( int service, void far *data )
#endif
/****************************************************/
{
    int ret;

    ret = FALSE;
    switch( service ) {
    case SAVE_OVL_STATE:
        ret = SaveOvlState( data );
        break;
    case RESTORE_OVL_STATE:
        ret = RestoreOvlState( data );
        break;
    case CHK_VEC_ADDR:
        ret = CheckVecAddr( data );
        break;
    case GET_SIZE_OVERLAYS:
        ret = GetSizeOverlays();
        break;
    case CHK_RET_ADDR:
#ifndef OVL_WHOOSH
        ret = NAME( CheckRetAddr )( data );
#else
        ret = __NCheckRetAddr__( data );
#endif
        break;
    case GET_OVL_TBL_ADDR:
        *(void far *far *)data = &__OVLTAB__;
        break;
#ifdef OVL_WHOOSH
    case GET_CHANGED_SECTIONS:
        ret = GetChangedSections( data );
        break;
#endif
    case GET_SECTION_DATA:
        ret = GetSectionData( data );
        break;
    }
    return( ret );
}
