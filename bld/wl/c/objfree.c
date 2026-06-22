/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


void FiniLinkStruct( void )
/********************************/
/* Free object processing data structures. */
{
    DEBUG(( DBG_OLD, "FiniLinkStruct()" ));
    BurnNodes();
    FreeTokBuffs();
    CacheFini();
    FiniORLObj();
}

static void FreeClasses( class_entry *class )
/*******************************************/
{
    class_entry     *next_class;

    for( ; class != NULL; class = next_class ) {
        next_class = class->next;
        RingWalk( class->segs, FreeLeader );
        CarveFree( CarveClass, class );
    }
}

void FreeAMod( mod_entry *mod )
/************************************/
{
    FreeObjCache( mod->u1.source );
    FreeModEntry( mod );
}

static void FreeMods( mod_entry *head )
/*************************************/
{
    void        *next;

    for( ; head != NULL; head = next ) {
        next = head->u.next;
        FreeAMod( head );
    }
}

static void FreeFiles( file_list *file )
/**************************************/
{
    void        *next;

    for( ; file != NULL; file = next ) {
        next = file->next;
        CacheClose( file, 3 );
        if( (file->flags_file & STAT_HAS_MEMBER) && file->u.member != NULL ) {
            FreeList( file->u.member );
        }
        _PermFree( file );
    }
}

static void FreeAreas( AREASECT *area );

static void FreeSections( section *sect )
/****************************************
 * Free sections & classes.
 */
{
    section             *next;
    ORDER_CLASS         *Class, *NextClass;
    ORDER_SEGMENT       *Seg, *NextSeg;

    for( ; sect != NULL; sect = next ) {
        next = sect->next;
        FreeFiles( sect->files );
        if( (LinkFlags & LF_INC_LINK_FLAG) == 0 ) {
            FreeMods( sect->mods );
            FreeClasses( sect->classes );
        }
        DBISectCleanup( sect );
        FreeAreas( sect->areas );
        ZapHTable( sect->modFilesHashed, MemFree );
        // Free up any Order Class entries
        for( Class = sect->orderlist; Class != NULL; Class = NextClass ) {
            NextClass = Class->NextClass;
            if( Class->Name != NULL ) {   // Including members and sucessors
                MemFree ( Class->Name );
            }
            if( Class->Copy ) {
                MemFree ( Class->SrcName );
            }
            // Order Seg entries can also have members and sucessors
            for( Seg = Class->SegList; Seg != NULL; Seg = NextSeg ) {
                NextSeg = Seg->NextSeg;
                if( Seg->Name != NULL ) {
                    MemFree( Seg->Name );
                }
                MemFree ( Seg );
            }
            MemFree ( Class );
        }
        MemFree( sect );
    }
}

static void FreeAreas( AREASECT *area )
/*************************************/
{
    AREASECT    *next_area;

    for( ; area != NULL; area = next_area ) {
        next_area = area->next;
        FreeSections( area->sections );
    }
}


void CleanLinkStruct( void )
/*********************************/
/* free all structures */
{
    if( Root == NULL )
        return;  /* haven't finished initializing */
    BurnLibs();
    FreeFiles( ObjLibFiles );
    FreeFiles( Root->files );
    ObjLibFiles = NULL;
    Root->files = NULL;
    if( (LinkFlags & LF_INC_LINK_FLAG) == 0 ) {
        FreeMods( LibModules );
    }
    if( SymFileName != NULL ) {
        MemFree( SymFileName );
    }
    if( FmtData.osname != NULL ) {
        MemFree( FmtData.osname );
    }
    if( FmtData.resource != NULL ) {
        MemFree( FmtData.resource );
    }
    FreeRelocInfo();
    FreeGroups( Groups );
    FreeGroups( AbsGroups );
    Groups = NULL;
    AbsGroups = NULL;
    FreeSections( Root );
    DBICleanup();
    Root = NULL;
#ifdef _EXE
    if( FmtData.type & MK_DOS ) {
        FreeOverlaySupp();
    }
#endif
}

#if defined( _OS2 ) || defined( _QNX )
void FreeSegFlags( xxx_seg_flags *curr )
/**************************************/
{
    xxx_seg_flags   *next;

    for( ; curr != NULL; curr = next ) {
        next = curr->next;
        MemFree( curr->name );
        MemFree( curr );
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

void FreeGroups( group_entry *group )
/***********************************/
{
    group_entry *next;

    for( ; group != NULL; group = next ) {
        next = group->next;
        FreeAGroup( group );
    }
}
