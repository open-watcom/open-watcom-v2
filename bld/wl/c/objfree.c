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
* Description:  OBJFREE : free linker structures
*
****************************************************************************/

#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "reloc.h"
#include "objnode.h"
#include "overlays.h"
#include "library.h"
#include "virtmem.h"
#include "objcache.h"
#include "loados2.h"
#include "dbgall.h"
#include "objio.h"
#include "wcomdef.h"
#include "objorl.h"
#include "loadfile.h"
#include "ring.h"
#include "carve.h"
#include "strtab.h"
#include "permdata.h"
#include "objpass1.h"
#include "objpass2.h"
#include "objfree.h"

static void FreeAreas( OVL_AREA *area );
static void FreeClasses( class_entry * list );
static void FreeFiles( file_list *list );
static void FreeMods( mod_entry *head );

void FiniLinkStruct( void )
/********************************/
/* Free object processing data structures. */
{
    DEBUG(( DBG_OLD, "FiniLinkStruct()" ));
    BurnNodes();
    FreeTokBuffs();
    CacheFini();
    ObjORLFini();
}


static void FreeSections( section *sec )
/**************************************/
/* Free sections & classes. */
{
    section             *next;
    ORDER_CLASS         *Class, *NextClass;
    ORDER_SEGMENT       *Seg, *NextSeg;

    for( ; sec != NULL; sec = next ) {
        next = sec->next_sect;
        FreeFiles( sec->files );
        if( !(LinkFlags & INC_LINK_FLAG) ) {
            FreeMods( sec->mods );
            FreeClasses( sec->classlist );
        }
        DBISectCleanup( sec );
        FreeAreas( sec->areas );
        ZapHTable(sec->modFilesHashed, LFree);
        // Free up any Order Class entries
        for( Class = sec->orderlist; Class != NULL; Class = NextClass ) {
            NextClass = Class->NextClass;
            if( Class->Name != NULL ) {   // Including members and sucessors
                _LnkFree ( Class->Name );
            }
            if( Class->Copy ) {
                _LnkFree ( Class->SrcName );
            }
            // Order Seg entries can also have members and sucessors
            for( Seg = Class->SegList; Seg != NULL; Seg = NextSeg ) {
                NextSeg = Seg->NextSeg;
                if ( Seg->Name != NULL ) {
                    _LnkFree( Seg->Name );
                }
                _LnkFree ( Seg );
            }
            _LnkFree ( Class );
        }
        _LnkFree( sec );
    }
}

static void FreeAreas( OVL_AREA *area )
/*************************************/
{
    OVL_AREA    *next;

    for( ; area != NULL; area = next ) {
        next = area->next_area;
        FreeSections( area->sections );
    }
}

static void FreeClasses( class_entry * list )
/*******************************************/
{
    class_entry *       next;

    for( ; list != NULL; list = next ) {
        next = list->next_class;
        RingWalk( list->segs, FreeLeader );
        CarveFree( CarveClass, list );
    }
}

void FreeAMod( mod_entry *mod )
/************************************/
{
    FreeObjCache( mod->f.source );
    FreeModEntry( mod );
}

static void FreeMods( mod_entry *head )
/*************************************/
{
    void        *next;

    for( ; head != NULL; head = next ) {
        next = head->n.next_mod;
        FreeAMod( head );
    }
}

static void FreeFiles( file_list *list )
/**************************************/
{
    void        *next;

    for( ; list != NULL; list = next ) {
        next = list->next_file;
        CacheClose( list, 3 );
        if( list->status & STAT_HAS_MEMBER && list->u.member != NULL ) {
            FreeList( list->u.member );
        }
        _PermFree( list );
    }
}


void CleanLinkStruct( void )
/*********************************/
/* free all structures */
{
    if( Root == NULL ) return;  /* haven't finished initializing */
    BurnLibs();
    FreeFiles( ObjLibFiles );
    FreeFiles( Root->files );
    ObjLibFiles = NULL;
    Root->files = NULL;
    if( !(LinkFlags & INC_LINK_FLAG) ) {
        FreeMods( LibModules );
    }
    if( SymFileName != NULL ) {
        _LnkFree( SymFileName );
    }
    if( FmtData.osname != NULL ) {
        _LnkFree( FmtData.osname );
    }
    if (FmtData.resource != NULL) {
        _LnkFree( FmtData.resource );
    }
    FreeRelocInfo();
    FreeGroups( Groups );
    FreeGroups( AbsGroups );
    Groups = NULL;
    AbsGroups = NULL;
    FreeSections( Root );
    DBICleanup();
    Root = NULL;
    if( FmtData.type & MK_REAL_MODE ) {
        FreeOvlStruct();
    }
}

#if defined(_OS2) || defined( _QNXLOAD )
void FreeSegFlags( seg_flags * curr )
/******************************************/
{
    seg_flags * next;

    for( ; curr != NULL; curr = next ) {
        next = curr->next;
        _LnkFree( curr->name );
        _LnkFree( curr );
    }
}
#endif

void FreeObjInfo( void )
/*****************************/
{
    FreeNodes( ExtNodes );
    FreeNodes( SegNodes );
    FreeNodes( GrpNodes );
}

static void FreeAGroup( group_entry *group )
/******************************************/
/* free all memory associated with group */
{
    CarveFree( CarveGroup, group );
}

void FreeGroups( group_entry *head )
/*****************************************/
{
    group_entry *next;

    for( ; head != NULL; head = next ) {
        next = head->next_group;
        FreeAGroup( head );
    }
}
