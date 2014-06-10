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
* Description:  Routines for creating auto groups and packing segments
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "ring.h"
#include "alloc.h"
#include "pcobj.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "specials.h"
#include "overlays.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "objcalc.h"
#include "objnode.h"
#include "objpass1.h"
#include "symmem.h"


static group_entry      *GetAutoGroup( bool );
static void             SortGroup( seg_leader * );
static void             PackSegs( seg_leader *, unsigned );
static void             SortGroupList( void );
static void             FindSplitGroups( void );
static void             NumberNonAutos( void );
static void             AutoGroupSect( section * );


static group_entry      *CurrGroup;
int                     NumGroups;

void AutoGroup( void )
/********************/
{
    WalkAllSects( &AutoGroupSect );
    SortGroupList();
    FindSplitGroups();
    if( NumGroups == 0 ) {
        LnkMsg( FTL+MSG_NO_CODE_OR_DATA, NULL );
    }
    NumberNonAutos();
}

static offset SetSegType( seg_leader *seg )
/*****************************************/
// set packlimit if necessary.
{
    offset      limit;

    if( seg == NULL )
        return( 0xFFFF );
    if( seg->info & SEG_CODE ) {
        if( LinkFlags & PACKCODE_FLAG ) {
            return( PackCodeLimit );
        }
    } else {
        if( LinkFlags & PACKDATA_FLAG ) {
            return( PackDataLimit );
        }
    }
    if( seg->info & USE_32 ) {
        limit = 0xFFFFFFFF;
    } else if( FmtData.type & MK_WINDOWS ) {   /* windows doesn't like */
        limit = 0xFFF0;                        /* large code segments */
    } else {
        limit = 0xFFFF;
    }
    return( limit );
}

static seg_leader *GetNextSeg( section *sec, seg_leader *seg )
/************************************************************/
/*
 * if seg == NULL then get first segment in section
 * if seg != NULL then get next segment after seg
 *
 * segments with debug info are skiped
 *
 * TODO!
 * now it goes through sorted class list
 * it can not handle DOSSEG segment order properly if class contains some segments
 * which are and are not member of DGROUP
 * it should be replaced by sorted segment list
 */
{
    class_entry *class;

    if( seg == NULL ) {
        for( class = sec->classlist; class != NULL; class = class->next_class ) {
            if( !(class->flags & CLASS_DEBUG_INFO) ) {
                 break;
            }
        }
        if( class == NULL ) {
            return( NULL );
        }
    } else {
        class = seg->class;
    }
    for( seg = RingStep( class->segs, seg ); seg == NULL; seg = RingStep( class->segs, seg ) ) {
        for( class = class->next_class; class != NULL; class = class->next_class ) {
            if( !(class->flags & CLASS_DEBUG_INFO) ) {
                 break;
            }
        }
        if( class == NULL ) {
            return( NULL );
        }
    }
    return( seg );
}

static bool CanPack( seg_leader *one, seg_leader *two )
/*****************************************************/
{
    if( one->info & SEG_CODE ) {
        if( two->combine == COMBINE_INVALID ) {
            return( FALSE );
        }
    } else {
        if( one->align != two->align ) {
            return( FALSE );
        }
    }
    if( (one->info & (USE_32 | SEG_CODE)) != (two->info & (USE_32 | SEG_CODE)) )
        return( FALSE );
    if( one->segflags != two->segflags || (two->segflags & SEG_FIXED) )
        return( FALSE );
    if( one->class != two->class && (two->class->flags & CLASS_FIXED) )
        return( FALSE );
    if( one->group != two->group )
        return( FALSE );
    return( TRUE );
}

static void AutoGroupSect( section *sec )
/***************************************/
{
    seg_leader      *seg;
    seg_leader      *packstart;
    offset          size;
    offset          new_size;
    offset          align_size;
    unsigned        num_segs;
    bool            lastseg = FALSE;    // TRUE iff this should be last seg in group.
    offset          limit = 0;

    CurrentSeg = NULL;
    size = 0;
    num_segs = 0;
    packstart = NULL;
    for( seg = NULL; (seg = GetNextSeg( sec, seg )) != NULL; ) {
        if( seg->info & SEG_ABSOLUTE ) {
            PackSegs( seg, 1 );
        } else {
            if( packstart == NULL ) {
                limit = SetSegType( seg );
                packstart = seg;
            }
            align_size = CAlign( size, seg->align );
            new_size = align_size + seg->size;
            if( ( new_size >= limit )      // group overflow 16/32-bit
                || ( new_size < size )     // group overflow 32-bit
                || lastseg
                || !CanPack( packstart, seg ) ) {
                PackSegs( packstart, num_segs );
                packstart = seg;
                num_segs = 1;
                if( FmtData.type & MK_REAL_MODE ) {
                    size = ( align_size & 0xF ) + seg->size;
                } else {
                    size = seg->size;
                }
                lastseg = FALSE;
                limit = SetSegType( seg );
            } else {
                size = new_size;
                ++num_segs;
            }
            if( seg->info & LAST_SEGMENT ) {
                lastseg = TRUE;
            }
        }
    }
    PackSegs( packstart, num_segs );
}

static void PackSegs( seg_leader *seg, unsigned num_segs )
/********************************************************/
{
    group_entry         *group;

    if( num_segs == 0 )
        return;

    if( seg->group != NULL ) {
        group = seg->group;
    } else {
        group = GetAutoGroup( seg->info & SEG_ABSOLUTE );
    }
    group->section = seg->class->section;
    while( num_segs != 0 ) {
        if( seg->group == NULL || seg->group == group ) {
            if( !(seg->info & SEG_CODE) ) {
                group->segflags |= SEG_DATA;
            }
            if( !(seg->class->flags & CLASS_READ_ONLY) ) {
                group->segflags &= ~SEG_READ_ONLY;
            }
            if( seg->class->flags & CLASS_COPY ) {  // If class is copied, mark group accordingly
                group->isdup = TRUE;
            }
            if( seg->group == NULL ) {              // if its not in a group add it to this one
                seg->group = group;
                Ring2Append( &group->leaders, seg );
            }
            --num_segs;
        }
        seg = GetNextSeg( group->section, seg );
    }
}

static void InitGroup( group_entry *group )
/*****************************************/
{
    group->grp_addr.seg = UNDEFINED;
    group->grp_addr.off = 0;
    group->totalsize = 0;
    group->size = 0;
    group->segflags = DEFAULT_GRP_FLAGS;
    group->u.miscflags = 0;
    group->isfree = FALSE;
    group->isautogrp = FALSE;
    group->isdup = FALSE;
    group->g.grp_relocs = NULL;
}

group_entry *AllocGroup( char *name, group_entry ** grp_list )
/********************************************************************/
{
    group_entry *group;
    symbol      *sym;

    group = CarveAlloc( CarveGroup );
    group->leaders = NULL;
    _PermAlloc( sym, sizeof *sym  ); // second class slave citizen
    BasicInitSym( sym );
    sym->namelen_cmp = strlen( name );
    sym->name = AddBufferStringTable( &PermStrings, name, sym->namelen_cmp + 1 );
    SET_SYM_TYPE( sym, SYM_GROUP );
    sym->info |= SYM_STATIC;
    group->next_group = NULL;
    InitGroup( group );
    group->sym = sym;
    LinkList( grp_list, group );
    if( stricmp( name, DataGrpName ) == 0 ) {
        DataGroup = group;
    } else if( name == AutoGrpName ) {
        group->isautogrp = 1;
    }
    return( group );
}

static group_entry *GetAutoGroup( bool abs_seg )
/***********************************************/
{
    group_entry      *group;
    group_entry **   grp_list;

    if( abs_seg ) {
        grp_list = &AbsGroups;
    } else {
        grp_list = &Groups;
    }
    group = AllocGroup( AutoGrpName, grp_list );
    if( !abs_seg ) {
        CurrGroup = group;
    }
    return( group );
}

static void SortGroupList( void )
/*******************************/
// Sort the group list by segments within classes.
{
    group_entry     *group;
    unsigned        number;

    NumGroups = 0;
    if( Groups == NULL )
        return;

    // first, set all of the links in the group list to NULL
    for( group = Groups; group != NULL; group = Groups ) {
        Groups = Groups->next_group;  // Take group out of original ring
        group->next_group = NULL;
        group->leaders = NULL;
        NumGroups++;
    }
    number = NumGroups;
    CurrGroup = NULL;
    WalkLeaders( SortGroup );
    CurrGroup->next_group = NULL;  // break the circular list.
    NumGroups = number;            // save # of groups.
}

static void SortGroup( seg_leader *seg )
/**************************************/
// Go through the classes & segments, and rebuild the group list in sorted form
{
    if( seg->group == NULL )
        return;
    if( seg->info & SEG_ABSOLUTE )
        return;
    Ring2Append( &seg->group->leaders, seg );
    if( seg->group->next_group == NULL ) { // not in the list yet
        if( CurrGroup == NULL ) {
            Groups = CurrGroup = seg->group;
        } else {
            CurrGroup->next_group = seg->group;
            CurrGroup = CurrGroup->next_group;
        }
        // Make the list circular so we have an easy way of telling if a node
        // is in the list.
        CurrGroup->next_group = Groups;
        NumGroups--;
        DbgAssert( NumGroups >= 0 );
    }
}

static bool CheckGroupSplit( void *leader, void *sect )
/*****************************************************/
{
    return( ((seg_leader *)leader)->class->section != (section *)sect );
}

static void FindSplitGroups( void )
/*********************************/
// a group can be split by accidentally putting parts of it into different
// overlays. This causes all hell to break loose, so this checks to make sure
// that this doesn't happen.
{
    group_entry     *group;

    if( FmtData.type & MK_OVERLAYS ) {
        for( group = Groups; group != NULL; group = group->next_group ) {
            if( Ring2Lookup( group->leaders, CheckGroupSplit, group->section ) ) {
                LnkMsg( ERR+MSG_OVL_GROUP_SPLIT, "s", group->sym->name );
            }
        }
    }
}

static void NumberNonAutos( void )
/********************************/
{
    group_entry         *group;
    unsigned            num;

    num = 0;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->isautogrp ) {
            group->num = 0;
        } else {
            num++;
            group->num = num;
        }
    }
}
