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

    while( sec != NULL ) {
        FreeFiles( sec->files );
        if( !(LinkFlags & INC_LINK_FLAG) ) {
            FreeMods( sec->mods );
            FreeClasses( sec->classlist );
        }
        DBISectCleanup( sec );
        FreeAreas( sec->areas );
        ZapHTable(sec->modFilesHashed, LFree);
        Class = sec->orderlist;
        while( Class != NULL ) {   // Free up any Order Class entries
            if( Class->Name != NULL ) {   // Including members and sucessors
                _LnkFree ( Class->Name );
            }
            if( Class->Copy ) {
                _LnkFree ( Class->SrcName );
            }
            Seg = Class->SegList;
            while ( Seg != NULL ) {  // Order Seg emtries can also have members and sucessors
                if ( Seg->Name != NULL ) {
                    _LnkFree( Seg->Name );
                }
                NextSeg = Seg->NextSeg;
                _LnkFree ( Seg );
                Seg = NextSeg;
            }
            NextClass = Class->NextClass;
            _LnkFree ( Class );
            Class = NextClass;
        }
        next = sec->next_sect;
        _LnkFree( sec );
        sec = next;
    }
}

static void FreeAreas( OVL_AREA *area )
/*************************************/
{
    OVL_AREA    *next;

    while( area != NULL ) {
        next = area->next_area;
        FreeSections( area->sections );
        area = next;
    }
}

static void FreeClasses( class_entry * list )
/*******************************************/
{
    class_entry *       next;

    while( list != NULL ) {
        next = list->next_class;
        RingWalk( list->segs, FreeLeader );
        CarveFree( CarveClass, list );
        list = next;
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
    void *      temp;

    while( head != NULL ) {
        temp = head->n.next_mod;
        FreeAMod( head );
        head = temp;
    }
}

static void FreeFiles( file_list *list )
/**************************************/
{
    void *      temp;

    while( list != NULL ) {
        temp = list->next_file;
        CacheClose( list, 3 );
        if( list->status & STAT_HAS_MEMBER && list->u.member != NULL ) {
            FreeList( list->u.member );
        }
        _PermFree( list );
        list = temp;
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

    while( curr != NULL ) {
        next = curr->next;
        _LnkFree( curr->name );
        _LnkFree( curr );
        curr = next;
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

    while( head != NULL ) {
        next = head->next_group;
        FreeAGroup( head );
        head = next;
    }
}
