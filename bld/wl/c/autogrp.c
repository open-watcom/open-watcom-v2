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


static group_entry *    GetAutoGroup( bool );
static void             SortGroup( seg_leader * );

static group_entry *    CurrGroup;
int                     NumGroups;

extern void AutoGroup( void )
/***************************/
{
    AutoGroupSect( Root );
    ProcAllOvl( &AutoGroupSect );
    SortGroupList();
    FindSplitGroups();
    if( NumGroups == 0 ) {
        LnkMsg( FTL+MSG_NO_CODE_OR_DATA, NULL );
    }
    NumberNonAutos();
}

static void AutoGroupSect( section * sec )
/****************************************/
{
    class_entry *           class;

    CurrGroup = NULL;
    for( class = sec->classlist; class != NULL; class = class->next_class ) {
        if( !(class->flags & CLASS_HANDS_OFF) ) {
            PackClass( class, sec );
        }
    }
}

static offset SetSegType( seg_leader * seg )
/******************************************/
// set packlimit if necessary.
{
    offset      limit;

    if( seg == NULL ) return 0xFFFF;
    if( seg->info & SEG_CODE ) {
        if( LinkFlags & PACKCODE_FLAG ) return PackCodeLimit;
    } else {
        if( LinkFlags & PACKDATA_FLAG ) return PackDataLimit;
    }
    if( seg->info & USE_32 ) {
        limit = 0xFFFFFFFF;
    } else if( FmtData.type & MK_WINDOWS ) {   /* windows doesn't like */
        limit = 0xFFF0;             /* large code segments */
    } else {
        limit = 0xFFFF;
    }
    return limit;
}

static bool CanPack( seg_leader *one, seg_leader *two )
/*****************************************************/
{
    if( one->info & SEG_CODE) {
        if( two->combine == COMBINE_INVALID ) return( FALSE );
    } else {
        if( one->align != two->align ) return FALSE;
    }
    if( (one->info & USE_32) != (two->info & USE_32) ) return( FALSE );
    if( one->segflags != two->segflags ) return( FALSE );
    return( TRUE );
}

static void PackClass( class_entry *class, section *sec )
/*******************************************************/
{
    seg_leader *    seg;
    seg_leader *    packstart;
    seg_leader *    anchor;
    offset          size;
    offset          new_size;
    offset          align_size;
    unsigned        num_segs;
    bool            lastseg;    // TRUE iff this should be last seg in group.
    bool            isdata;
    bool            isreadwrite;
    offset          limit;

    isreadwrite = !(class->flags & CLASS_READ_ONLY);
    CurrentSeg = NULL;
    seg = (seg_leader *) RingStep( class->segs, NULL );
    anchor = seg;
    packstart = seg;
    size = 0;
    num_segs = 0;
    lastseg = FALSE;
    if( seg != NULL ) {
        limit = SetSegType( seg );
        isdata = !(seg->info & SEG_CODE);
    }
    while( seg != NULL ) {
        if( seg->group == NULL ) {
            align_size = CAlign(size, seg->align );
            new_size = align_size + seg->size;
            if( seg->size != 0 && (new_size < size
                                        || new_size >= limit
                                        || lastseg
                                        || !CanPack( packstart, seg )) ) {
                PackSegs(packstart, num_segs, size, class, isdata, isreadwrite);
                packstart = seg;
                num_segs = 1;
                if( FmtData.type & MK_REAL_MODE ) {
                    size = (align_size & 0xF) + seg->size;
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
        } else {
            seg->group->section = sec;
            if( !(seg->info & SEG_CODE) ) {
                seg->group->segflags |= SEG_DATA;
            }
            if( isreadwrite ) {
                seg->group->segflags &= ~SEG_READ_ONLY;
            }
            PackSegs( packstart, num_segs, size, class, isdata, isreadwrite );
            packstart = (seg_leader *) RingStep( class->segs, seg );
            num_segs = 0;
            if( FmtData.type & MK_REAL_MODE ) {
                size = (CAlign( size, seg->align ) + seg->size) & 0xF;
            } else {
                size = 0;
            }
            lastseg = FALSE;
            limit = SetSegType( packstart );
        }
        seg = (seg_leader *) RingStep( class->segs, seg );
    }
    PackSegs( packstart, num_segs, size, class, isdata, isreadwrite );
}

static void PackSegs( seg_leader * seg, unsigned num_segs, offset size,
                      class_entry *class, bool isdata, bool isrdwr )
/*********************************************************************/
{
    group_entry *       group;
    bool                fakegroup;

    if( num_segs == 0 ) return;
    fakegroup = size == 0 && CurrGroup != NULL;
    if( fakegroup ) {
        group = CurrGroup;
    } else {
        group = GetAutoGroup( (seg->info & SEG_ABSOLUTE) != 0 );
        if( isdata ) {
            group->segflags |= SEG_DATA;
        }
        if( isrdwr ) {
            group->segflags &= ~SEG_READ_ONLY;
        }
        group->section = seg->class->section;
    }
    while( num_segs != 0 ) {
        if( seg->group == NULL ) {
            seg->group = group;
            if( !fakegroup ) {
                Ring2Append( &group->leaders, seg );
            }
            --num_segs;
        }
        seg = (seg_leader *) RingStep( class->segs, seg );
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
    group->g.grp_relocs = NULL;
}

extern group_entry * AllocGroup( char *name, group_entry ** grp_list )
/********************************************************************/
{
    group_entry *group;
    symbol *    sym;

    group = CarveAlloc( CarveGroup );
    group->leaders = NULL;
    _PermAlloc( sym, sizeof *sym  ); // second class slave citizen
    BasicInitSym(sym);
    sym->name = AddStringTable( &PermStrings, name, strlen(name) + 1 );
    sym->namelen = strlen(name) + 1;
    SET_SYM_TYPE( sym, SYM_GROUP );
    sym->info |= SYM_STATIC;
    group->next_group = NULL;
    InitGroup( group );
    group->sym = sym;
    LinkList( grp_list, group );
    if( strcmp( name, DataGrpName ) == 0 ) {
        DataGroup = group;
    } else if( name == AutoGrpName ) {
        group->isautogrp = 1;
    }
    return group;
}

static group_entry * GetAutoGroup( bool abs_seg )
/***********************************************/
{
    group_entry *    group;
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
    return group;
}

static void SortGroupList( void )
/*******************************/
// Sort the group list by segments within classes.
{
    group_entry *   group;
    unsigned        number;

    NumGroups = 0;
    if( Groups == NULL ) return;
// first, set all of the links in the group list to NULL
    group = Groups;
    while( Groups != NULL ) {
        Groups = Groups->next_group;
        group->next_group = NULL;
        group->leaders = NULL;
        group = Groups;
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
    if( seg->group == NULL ) return;
    if( seg->info & SEG_ABSOLUTE ) return;
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

static bool CheckGroupSplit( seg_leader *leader, section *sect )
/**************************************************************/
{
    return leader->class->section != sect;
}

static void FindSplitGroups( void )
/*********************************/
// a group can be split by accidentally putting parts of it into different
// overlays. This causes all hell to break loose, so this checks to make sure
// that this doesn't happen.
{
    group_entry *   group;

    if( !(FmtData.type & MK_OVERLAYS) ) return;
    group = Groups;
    while( group != NULL ) {
        if( Ring2Lookup( group->leaders, CheckGroupSplit, group->section ) ) {
            LnkMsg( ERR+MSG_OVL_GROUP_SPLIT, "s", group->sym->name );
        }
        group = group->next_group;
    }
}

static void NumberNonAutos( void )
/********************************/
{
    group_entry *       group;
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
