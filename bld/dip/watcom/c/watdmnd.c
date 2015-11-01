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
* Description:  Demand loading of Watcom style debugging information.
*
****************************************************************************/


#include "dipwat.h"
#include <limits.h>
#include "watlcl.h"
#include "watmod.h"
#include "watldsym.h"


/* WD looks for this symbol to determine module bitness */
#if !defined( __WINDOWS__ )
int __nullarea;
#if defined( __WATCOMC__ )
#pragma aux __nullarea "*";
#endif
#endif

typedef struct demand_ctrl {
        struct demand_ctrl      *link;
        pointer_int             *owner;
        void                    (*clear)(void *, void *);
        pointer_int             save;
        unsigned                size;
        unsigned                locks;
        unsigned                time_stamp;
        char                    buff[1];
} demand_ctrl;


#define _demand_size( size ) ( (sizeof( demand_ctrl ) - 1) + size )

static demand_ctrl      *DemandList;
static demand_ctrl      *LastDemand;
static unsigned         LastDmndSize;
static unsigned         TimeStamp;

#define GET_LINK( sect, idx )                                   \
        ((sect)->dmnd_link[(unsigned)(idx) / MAX_LINK_ENTRIES]  \
        [(unsigned)(idx) % MAX_LINK_ENTRIES])

#define RESIDENT                0x00000001UL
#define MK_DMND_PTR( p )        ((demand_ctrl *)( (p) & ~RESIDENT))
#define IS_RESIDENT( p )        ((p) & RESIDENT)
#define DMND_LINK( p )          (IS_RESIDENT(p)?MK_DMND_PTR(p)->save:(p))
#define MK_DMND_OFFSET( p )     (DMND_LINK( p ) >> 1)
#define DMND_SIZE( sect, idx )  ((DMND_LINK(GET_LINK(sect,(idx)+1))     \
                                 - DMND_LINK(GET_LINK(sect,idx))) >> 1)
#define STASH_DMND_PTR( p )     ((pointer_int)(p) | RESIDENT)


/*
 * InfoSize -- return size of demand info section
 */
unsigned InfoSize( imp_image_handle *ii, imp_mod_handle im,
                        unsigned item, unsigned entry )
{
    demand_info         *dmnd;
    section_info        *inf;

    dmnd = &ModPointer( ii, im )->di[item];
    if( entry >= dmnd->u.entries )
        return( 0 );
    entry += dmnd->info_off;
    inf = FindInfo( ii, im );
    return( DMND_SIZE( inf, entry ) );
}


/*
 * InitDemand -- allocate a last chance demand info location
 */

struct walk_demand {
    unsigned long       max_size;
};

static walk_result WlkDmnd( imp_image_handle *ii, imp_mod_handle im, void *d )
{
    struct walk_demand  *wdd = d;
    unsigned long       size;
    unsigned            dmnd;
    unsigned            i;

    for( dmnd = DMND_FIRST; dmnd < DMND_NUM; ++dmnd ) {
        i = 0;
        for( ;; ) {
            size = InfoSize( ii, im, dmnd, i );
            if( size == 0 )
                break;
            if( size > wdd->max_size )
                wdd->max_size = size;
            ++i;
        }
    }
    return( WR_CONTINUE );
}

static void Unload( demand_ctrl *section )
{
    demand_ctrl **owner;

    if( section->owner == NULL )
        return;
    if( section == LastDemand ) {
        if( section->clear != NULL ) {
            section->clear( section->buff, section->buff + section->size );
        }
        *section->owner = section->save;
        section->owner = NULL;
        section->clear = NULL;
        return;
    }
    for( owner = &DemandList; *owner != section; owner = &(*owner)->link )
        ;
    *owner = section->link;
    if( section->clear != NULL ) {
        section->clear( section->buff, section->buff + section->size );
    }
    *section->owner = section->save;
    DCFree( section );
}

dip_status InitDemand( imp_image_handle *ii )
{
    struct walk_demand  d;

    d.max_size = 0;
    MyWalkModList( ii, WlkDmnd, &d );
    if( d.max_size >= (0x10000UL - sizeof( demand_ctrl )) ) {
        DCStatus( DS_ERR|DS_INFO_INVALID );
        return( DS_ERR|DS_INFO_INVALID );
    }
    if( d.max_size <= LastDmndSize )
        return( DS_OK );
    if( LastDemand != NULL ) {
        Unload( LastDemand );
        DCFree( LastDemand );
    }
    LastDmndSize = d.max_size;
    LastDemand = DCAlloc( _demand_size( d.max_size ) );
    if( LastDemand == NULL ) {
        DCStatus( DS_ERR|DS_NO_MEM );
        return( DS_ERR|DS_NO_MEM );
    }
    LastDemand->link = NULL;
    LastDemand->owner = NULL;
    LastDemand->clear = NULL;
    LastDemand->locks = 0;
    return( DS_OK );
}

void FiniDemand( void )
{
    DCFree( LastDemand );
    LastDemand = NULL;
    LastDmndSize = 0;
    TimeStamp = 0;
}

static walk_result WlkClear( imp_image_handle *ii, imp_mod_handle im, void *d )
{
    unsigned            dmnd;
    mod_info            *mp;
    section_info        *sect;
    int                 entry;
    unsigned            real_entry;
    pointer_int         *lnk;

    d = d;
    mp = ModPointer( ii, im );
    sect = FindInfo( ii, im );
    for( dmnd = DMND_FIRST; dmnd < DMND_NUM; ++dmnd ) {
        for( entry = mp->di[dmnd].u.entries-1; entry >= 0; --entry ) {
            real_entry = entry + mp->di[dmnd].info_off;
            lnk = &GET_LINK( sect, real_entry );
            if( IS_RESIDENT( *lnk ) ) {
                Unload( MK_DMND_PTR( *lnk ) );
            }
        }
    }
    return( WR_CONTINUE );
}

void InfoClear( imp_image_handle *ii )
{
    MyWalkModList( ii, WlkClear, NULL );
}

/*
 * InfoUnlock -- arbitrarily set all demand section lock counts to zero
 */

void InfoUnlock( void )
{
    demand_ctrl *section;

    for( section = DemandList; section != NULL; section = section->link ) {
        section->locks = 0;
    }
    if( LastDemand != NULL ) LastDemand->locks = 0;
}


/*
 * InfoLoad -- load demand info
 */

void *InfoLoad( imp_image_handle *ii, imp_mod_handle im, unsigned item,
                unsigned entry, void (*clear)(void *, void *) )
{
    demand_ctrl         *section;
    demand_info         *info;
    section_info        *sect;
    unsigned long       tmpoff;
    pointer_int         *lnk;
    unsigned            size;

    ++TimeStamp;
    if( TimeStamp == 0 ) { /* TimeStamp wrapped */
        TimeStamp = 1;
        for(section = DemandList; section != NULL; section = section->link) {
            section->time_stamp = 0;
        }
    }
    info = &ModPointer( ii, im )->di[ item ];
    if( entry >= info->u.entries )
        return( NULL );
    entry += info->info_off;
    sect = FindInfo( ii, im );
    lnk = &GET_LINK( sect, entry );
    if( IS_RESIDENT( *lnk ) ) {
        section = MK_DMND_PTR( *lnk );
    } else {
        /* section not loaded */
        size = DMND_SIZE( sect, entry );
        if( (LastDemand->owner == NULL || LastDemand->size < size)
            && LastDemand->locks == 0 ) {
            /* keep largest section in LastDemand */
            section = LastDemand;
            Unload( LastDemand );
        } else {
            /* allocate some memory */
            section = DCAlloc( _demand_size( size ) );
            if( section == NULL ) {
                if( LastDemand->locks != 0 )
                    return( NULL );
                /* no memory, use last chance */
                section = LastDemand;
                Unload( LastDemand );
            }
        }
        tmpoff = MK_DMND_OFFSET( *lnk );
        if( InfoRead( sect, tmpoff, size, section->buff ) != DS_OK ) {
            if( section != LastDemand )
                DCFree( section );
            return( NULL );
        }
        section->size = size;
        section->locks = 0;
        section->clear = clear;
        section->owner = lnk;
        section->save  = *lnk;
        *lnk = STASH_DMND_PTR( section );
        if( section != LastDemand ) {
            section->link = DemandList;
            DemandList = section;
        }
    }
    section->time_stamp = TimeStamp; /* for removal priority */
    section->locks++;
    return( section->buff );
}


static void AdjustLockCount( const char *p, int adjust )
{
    demand_ctrl *section;

    section = (demand_ctrl *)( p - offsetof( demand_ctrl, buff ) );
    section->locks += adjust;
}

/*
 * Increment the lock count for a specific demand load section
 */
void InfoSpecLock( const char *p )
{
    AdjustLockCount( p, 1 );
}


/*
 * Decrement the lock count for a specific demand load section
 */
void InfoSpecUnlock( const char *p )
{
    AdjustLockCount( p, -1 );
}


/*
 * InfoRelease -- release the least recently used section
 */

static dip_status ReleaseFromList( void )
{
    demand_ctrl *release;
    demand_ctrl *curr;
    unsigned    low;

    low = UINT_MAX;
    release = NULL;
    for( curr = DemandList; curr != NULL; curr = curr->link ) {
        if( curr->locks == 0 && curr->time_stamp < low ) {
            low = curr->time_stamp;
            release = curr;
        }
    }
    if( release == NULL )
        return( DS_FAIL );
    Unload( release );
    return( DS_OK );
}

dip_status InfoRelease( void )
{
    if( ReleaseFromList() == DS_OK )
        return( DS_OK );
    if( LastDemand != NULL && LastDemand->locks == 0 && LastDemand->clear != NULL ) {
        /* let's hope the clear routine frees up some memory */
        Unload( LastDemand );
        return( DS_OK );
    }
    return( DS_FAIL );
}
