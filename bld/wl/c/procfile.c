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


/*
   PROCFILES   : process object file.
*/

#include <string.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "pcobj.h"
#include "library.h"
#include <ar.h>
#include "objnode.h"
#include "obj2supp.h"
#include "fileio.h"
#include "objpass1.h"
#include "overlays.h"
#include "objio.h"
#include "distrib.h"
#include "symtrace.h"
#include "objcache.h"
#include "specials.h"
#include "objorl.h"
#include "objomf.h"
#include "loadfile.h"
#include "dbgall.h"
#include "objfree.h"
#include "cmdline.h"
#include "impexp.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "ring.h"
#include "procfile.h"
#include "hash.h"

static bool             EndOfLib( file_list *, unsigned long );

extern void ProcObjFiles( void )
/******************************/
/* Perform Pass 1 on all object files */
{
    CurrMod = NULL;
    if( LinkFlags & INC_LINK_FLAG) {
        if((LinkFlags & DWARF_DBI_FLAG) == 0 && LinkFlags & ANY_DBI_FLAG) {
            LnkMsg( FTL+MSG_INC_ONLY_SUPPORTS_DWARF, NULL );
        }
        if( LinkFlags & STRIP_CODE ) {
            LnkMsg( FTL+MSG_INC_AND_DCE_NOT_ALLOWED, NULL );
        }
        if( LinkFlags & VF_REMOVAL ) {
            LnkMsg( FTL+MSG_INC_AND_VFR_NOT_ALLOWED, NULL );
        }
    }
    LnkMsg( INF+MSG_LOADING_OBJECT, NULL );
    if( LinkFlags & STRIP_CODE ) {
        LinkState |= CAN_REMOVE_SEGMENTS;
    }
    if( LinkState & GOT_PREV_STRUCTS ) {
        IncLoadObjFiles();
    } else {
        LoadObjFiles( Root );
        if( FmtData.type & MK_OVERLAYS ) {
            OvlPass1();
        }
    }
}

extern void SetupFakeModule( void )
/*********************************/
{
    FakeModule = NewModEntry();
    FakeModule->modinfo = DBI_ALL|MOD_LAST_SEG|MOD_NEED_PASS_2|FMT_PE_XFER;
    FakeModule->name = StringStringTable( &PermStrings, LinkerModule );
    DBIInitModule( FakeModule );

}

extern void LinkFakeModule( void )
/********************************/
{
    // Unlike other modules, fake module goes to the beginning of the list
    FakeModule->n.next_mod = LibModules;
    LibModules = FakeModule;
}

static void CheckNewFile( mod_entry *mod, file_list *list,
                          int AlwaysCheckUsingDate)
/*********************************************************/
{
    time_t      modtime;

    if( !(LinkFlags & GOT_CHGD_FILES) || AlwaysCheckUsingDate) {
        if( QModTime(list->file->name, &modtime) || modtime > mod->modtime ) {
            list->status |= STAT_HAS_CHANGED;
        }
    } else {
        if( FindHTableElem( Root->modFilesHashed, list->file->name ) ) {
            list->status |= STAT_HAS_CHANGED;
        }
    }
}

static void SetStartAddr( void )
/******************************/
{
    mod_entry * mod;

    if( StartInfo.user_specd || !StartInfo.from_inc ) return;
    mod = StartInfo.mod;
    if( mod == NULL ) return;
    if( mod->modinfo & MOD_KILL || mod->f.source->status & STAT_HAS_CHANGED ) {
        ClearStartAddr();
    }
}

static void SetupModule( mod_entry **mod, file_list *list )
/*********************************************************/
{
    char *      fname;
    mod_entry * currmod;

    currmod = *mod;

    CheckNewFile( currmod, list, 0 );
    fname = currmod->f.fname;
    for(;;) {
        currmod->f.source = list;
        currmod = currmod->n.next_mod;
        if( currmod == NULL ) break;
        if( currmod->f.fname != fname ) break;
    }
    *mod = currmod;
}

static void DoIncLibDefs( void )
/******************************/
{
    libnamelist *       lib;

    for( lib = SavedDefLibs; lib != NULL; lib = lib->next ) {
        AddObjLib( lib->name, 1 );
    }
}

static libnamelist * CalcLibBlacklist( void )
/*******************************************/
/* figure out if the user has changed any of the specified libraries, and
 * torch anything after a changed library */
{
    infilelist *        userlibs;
    libnamelist *       oldlibs;

    userlibs = CachedLibFiles;
    oldlibs = SavedUserLibs;
    while( oldlibs != NULL ) {
        if( userlibs == NULL ) return oldlibs;
        if( FNAMECMPSTR(userlibs->name, oldlibs->name) != 0 ) return oldlibs;
        oldlibs = oldlibs->next;
        userlibs = userlibs->next;
    }
    return NULL;
}

static void CheckBlacklist( file_list *list, libnamelist *blacklist )
/*******************************************************************/
{
    unsigned    length;
    unsigned    delta;

    if( list->status & STAT_HAS_CHANGED ) return;
    length = strlen( list->file->name );
    while( blacklist != NULL ) {
        if( length >= blacklist->namelen ) {
            delta = length - blacklist->namelen;
            if( FNAMECMPSTR(blacklist->name,list->file->name + delta) == 0 ) {
                list->status |= STAT_HAS_CHANGED;
                return;
            }
        }
        blacklist = blacklist->next;
    }
}

static void PrepareModList( void )
/********************************/
{
    file_list * list;
    mod_entry * mod;
    mod_entry * curr;
    libnamelist *blacklist;

    mod = Root->mods;
    for( list = Root->files; list != NULL; list = list->next_file ) {
        if( strcmp( list->file->name, mod->f.fname ) == 0 ) {
            SetupModule( &mod, list );
        } else if( mod->n.next_mod != NULL ) {
            if( FNAMECMPSTR( list->file->name, mod->n.next_mod->f.fname ) == 0){
                mod->modinfo |= MOD_KILL;
                mod = mod->n.next_mod;
                SetupModule( &mod, list );
            }
        }
        if( mod == NULL ) break;
    }
    while( mod != NULL ) {
        mod->modinfo |= MOD_KILL;               // no match found
        mod = mod->n.next_mod;
    }
    blacklist = CalcLibBlacklist();
    mod = LibModules;
    while( mod != NULL ) {
        if( mod->f.fname == NULL ) {
            mod->modinfo |= MOD_KILL;
        } else if( !(mod->modinfo & MOD_VISITED) ) {
            list = AddObjLib( mod->f.fname, 128 );
            CheckNewFile( mod, list, 1);
            CheckBlacklist( list, blacklist );
            for( curr = mod->n.next_mod; curr != NULL; curr = curr->n.next_mod){
                if( curr->f.fname == mod->f.fname ) {
                    curr->f.source = list;
                    curr->modinfo |= MOD_VISITED;
                }
            }
            mod->f.source = list;
        }
        mod->modinfo &= ~MOD_VISITED;
        mod = mod->n.next_mod;
    }
    FreeList( SavedUserLibs );
    SavedUserLibs = NULL;
}

static void MarkDefaultSyms( void )
/*********************************/
{
    symbol *sym;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_ALIAS( sym ) && sym->info & SYM_WAS_LAZY ) {
            sym->e.def->info |= SYM_RELOC_REFD;
        }
    }
}

static void MarkRelocs( mod_entry *mod )
/**************************************/
{
    IterateModRelocs( mod->relocs, mod->sizerelocs, RelocMarkSyms );
}

static void KillASym( symbol *sym )
/*********************************/
{
    sym->info |= SYM_KILL;
    if( IS_SYM_IMPORTED( sym ) ) {
        KillDependantSyms( sym );
    }
}

static void KillSyms( mod_entry *mod )
/************************************/
{
    Ring2Walk( mod->publist, KillASym );
}

static void SetAltDefData( symbol *sym )
/**************************************/
{
    if( sym->info & SYM_IS_ALTDEF && sym->info & SYM_COMDAT
                                  && !(sym->info & SYM_HAS_DATA) ) {
        sym->p.seg->data = sym->e.mainsym->p.seg->data;
    }
}

static void FixModAltDefs( mod_entry *mod )
/*****************************************/
{
    Ring2Walk( mod->publist, SetAltDefData );
}

static void IncIterateMods( mod_entry *mod, void (*proc_fn)(mod_entry *),
                            bool dochanged )
/***********************************************************************/
{
    bool haschanged;

    while( mod != NULL ) {
        haschanged = mod->modinfo & MOD_KILL
                        || mod->f.source->status & STAT_HAS_CHANGED;
        if( haschanged == dochanged ) {
            proc_fn( mod );
        }
        mod = mod->n.next_mod;
    }
}

static void AddToModList( mod_entry *mod )
/****************************************/
{
    mod->modinfo |= ObjFormat & FMT_OBJ_FMT_MASK;
    if( CurrMod == NULL ) {
        CurrSect->mods = mod;
    } else {
        CurrMod->n.next_mod = mod;
    }
    mod->n.next_mod = NULL;
    CurrMod = mod;
}

static void SavedPass1( mod_entry *mod )
/**************************************/
{
    ObjFormat = FMT_INCREMENTAL;
    mod->modinfo &= ~FMT_OBJ_FMT_MASK;
    AddToModList( mod );
    ObjPass1();
}

static void ProcessMods( void )
/*****************************/
{
    mod_entry * mod;
    mod_entry * next;
    mod_entry * savemod;
    file_list * list;

    mod = Root->mods;
    Root->mods = NULL;
    for( list = Root->files; list != NULL; list = list->next_file ) {
        if( mod == NULL ) break;
        while( mod->modinfo & MOD_KILL ) {
            next = mod->n.next_mod;
            FreeModEntry( mod );
            mod = next;
            if( mod == NULL ) break;
        }
        if( mod == NULL ) break;
        if( mod->f.source == list ) {
            for(;;) {
                next = mod->n.next_mod;
                if( list->status & STAT_HAS_CHANGED ) {
                    memset( mod, 0, sizeof(mod_entry) );
                    DoPass1( mod, list );
                } else {
                    SavedPass1( mod );
                }
                mod = next;
                if( mod == NULL || mod->f.source != list ) break;
            }
        } else {
            DoPass1( NULL, list );
        }
    }
    while( mod != NULL ) {
        next = mod->n.next_mod;
        FreeModEntry( mod );
        mod = next;
    }
    while( list != NULL ) {
        DoPass1( NULL, list );
        list = list->next_file;
    }
    mod = LibModules;
    savemod = Root->mods;       // pass1 routines will add new mods to this
    Root->mods = NULL;
    CurrMod = NULL;
    while( mod != NULL ) {
        next = mod->n.next_mod;
        if( mod->modinfo & MOD_KILL
                || (mod->f.source != NULL && mod->f.source->status & STAT_HAS_CHANGED )) {
            FreeModEntry( mod );
        } else {
            SavedPass1( mod );
        }
        mod = next;
    }
    LibModules = Root->mods;
    Root->mods = savemod;
}

static void FreeModSegments( mod_entry *mod )
/*******************************************/
{
    mod->publist = NULL;
    Ring2CarveFree( CarveSegData, &mod->segs );
}

static void IncLoadObjFiles( void )
/*********************************/
{
    PrepareModList();
    SetStartAddr();
    MarkDefaultSyms();
    IncIterateMods( Root->mods, MarkRelocs, FALSE );
    IncIterateMods( LibModules, MarkRelocs, FALSE );
    IncIterateMods( Root->mods, FixModAltDefs, FALSE );
    IncIterateMods( LibModules, FixModAltDefs, FALSE );
    IncIterateMods( Root->mods, KillSyms, TRUE );
    IncIterateMods( LibModules, KillSyms, TRUE );
    PurgeSymbols();
    IncIterateMods( Root->mods, FreeModSegments, TRUE );
    IncIterateMods( LibModules, FreeModSegments, TRUE );
    ProcessMods();
    DoIncGroupDefs();
    DoIncLibDefs();
}

extern void LoadObjFiles( section *sect )
/***************************************/
{
    file_list * list;

    CurrSect = sect;
    CurrMod = NULL;
    for( list = sect->files; list != NULL; list = list->next_file ) {
        DoPass1( NULL, list );
    }
}

static member_list * FindMember( file_list *list, char *name )
/************************************************************/
// see if name is in the member list of list
{
    member_list **      memb;
    member_list *       foundmemb;

    foundmemb = NULL;
    memb = &list->u.member;
    while( *memb != NULL ) {
        if( ModNameCompare( name, (*memb)->name ) ) {
            foundmemb = *memb;
            *memb = (*memb)->next;      // remove it from the list.
            break;
        }
        memb = &(*memb)->next;
    }
    return foundmemb;
}

static void DoPass1( mod_entry *next, file_list *list )
/*****************************************************/
/* do pass 1 on the object file */
{
    mod_entry *         old;
    member_list *       member;
    char *              membname;
    unsigned long       loc;
    unsigned long       size;
    unsigned            reclength;
    bool                lastmod;
    bool                ignoreobj;

    loc = 0;
    lastmod = FALSE;
    if( CacheOpen( list ) ) {
        reclength = CheckLibraryType( list, &loc, FALSE );
        for(;;) { /*  there may be more than 1 object module in a file */
            member = NULL;
            ignoreobj = FALSE;
            if( EndOfLib( list, loc ) ) break;
            membname = IdentifyObject( list, &loc, &size );
            if( list->status & STAT_IS_LIB ) {
                if( list->status & STAT_HAS_MEMBER && list->u.member != NULL ) {
                    member = FindMember( list, membname );
                    if( member == NULL ) {
                        ignoreobj = TRUE;
                    } else if( list->u.member == NULL ) {
                        lastmod = TRUE;
                    }
                }
            }
            if( ignoreobj ) {
                _LnkFree( membname );
                if( size != 0 ) {
                    loc += size;
                } else {
                    SkipFile( list, &loc );
                }
            } else {
                if( next == NULL ) {
                    next = NewModEntry();
                }
                next->n.next_mod = NULL;
                next->f.source = list;
                next->modtime = next->f.source->file->modtime;
                if( member != NULL ) {
                    next->modinfo |= member->flags;
                    _LnkFree( member );
                }
                if( !(list->status & STAT_HAS_MEMBER) ) {
                    next->modinfo |= list->status & DBI_MASK;
                    if( list->status & STAT_LAST_SEG ) {
                        next->modinfo |= MOD_LAST_SEG;
                    }
                }
                old = CurrMod;
                AddToModList( next );
                next->location = loc;
                if( membname == NULL ) {
                    membname = ChkStrDup( list->file->name );
                }
                next->name = membname;
                loc = ObjPass1();
                if( list->status & STAT_TRACE_SYMS ) {
                    TraceSymList( CurrMod->publist );
                }
                next = NULL;
            }
            ObjFormat = 0;
            if( list->status & STAT_IS_LIB ) {             // skip padding.
                loc += CalcAlign( loc, reclength );
            } else if( !IS_FMT_OMF(CurrMod->modinfo) ) {
                break;          // can only concat omf.
            }
            if( lastmod || CacheEnd( list, loc ) ) break;
        }
        if( list->u.member != NULL ) {
            LnkMsg( ERR+MSG_CANT_FIND_MEMBER, "12", list->file->name,
                                                    list->u.member->name );
        }
        CacheClose( list, 1 );
    }
    CheckStop();
}

extern char * IdentifyObject( file_list * list, unsigned long *loc,
                              unsigned long *size )
/*****************************************************************/
{
    ar_header * ar_hdr;
    char *      name;

    name = NULL;
    *size = 0;
    if( list->status & STAT_AR_LIB ) {
        ar_hdr = CacheRead( list, *loc, sizeof(ar_header) );
        *loc += sizeof(ar_header);
        name = GetARName( ar_hdr, list );
        *size = GetARValue( ar_hdr->size, AR_SIZE_LEN );
    }
    if( !IsORL( list, *loc ) ) {
        if( IsOMF( list, *loc ) ) {
            ObjFormat |= FMT_OMF;
            name = GetOMFName( list, loc );
        }
    }
    return name;
}

static void BadSkip( file_list *list, unsigned long *loc )
/********************************************************/
{
    list = list;
    loc = loc;
    BadObjFormat();
}

static void (*SkipObjFile[])(file_list *, unsigned long *) = {
        BadSkip, OMFSkipObj, ORLSkipObj, ORLSkipObj };

static void SkipFile( file_list *list, unsigned long *loc )
/*********************************************************/
{
    SkipObjFile[GET_FMT_IDX(ObjFormat)]( list, loc );
}

static bool EndOfLib( file_list *list, unsigned long loc )
/********************************************************/
{
    unsigned_8 *id;

    if( list->status & STAT_OMF_LIB) {
        id = CacheRead( list, loc, sizeof(unsigned_8) );
        return *id == LIB_TRAILER_REC;
    } else {
        return 0;
    }
}

static unsigned long (*CallPass1[])() = {
    BadObjFormat,
    OMFPass1,
    ORLPass1,
    ORLPass1,
    IncPass1,
    BadObjFormat,
    BadObjFormat,
    BadObjFormat
};

extern unsigned long ObjPass1( void )
/***********************************/
/* Pass 1 of 8086 linker. */
{
    unsigned long loc;
    char *        savename;

    DEBUG(( DBG_BASE, "1 : file %s", CurrMod->f.source->file->name ));
    CurrMod->modinfo |= MOD_DONE_PASS_1;
    SymModStart();
    DBIInitModule( CurrMod );
    RelocStartMod();
    P1Start();
    loc = CallPass1[GET_FMT_IDX(ObjFormat)]();
    CollapseLazyExtdefs();
    SymModEnd();
    if( !(CurrMod->modinfo & MOD_GOT_NAME) ) {
        savename = CurrMod->name;
        CurrMod->name = StringStringTable( &PermStrings, savename );
        _LnkFree( savename );
        CurrMod->modinfo |= MOD_GOT_NAME;
    }
    DBIP1ModuleScanned();
    ReleaseNames();
    PermEndMod( CurrMod );
    FreeObjInfo();
    ObjFormat = 0;       //clear flags for processing obj file
    return loc;
}

static bool ResolveVFExtdefs( void )
/**********************************/
/* go through the symbol table, and check if any conditional extdefs
 * should turned into real extdefs */
{
    bool        resolved;
    symbol *    sym;

    resolved = FALSE;
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_VF_REF(sym) ) {
            resolved |= CheckVFList( sym );
        }
    }
    return resolved;
}

extern void ResolveUndefined( void )
/**********************************/
{
    symbol *    sym;
    file_list * lib;
    bool        keepgoing;

    LnkMsg( INF+MSG_SEARCHING_LIBS, NULL );
    if( FmtData.type & MK_OVERLAYS && FmtData.u.dos.distribute ) {
        LinkState |= CAN_REMOVE_SEGMENTS;
        InitModTable();
    }
    CurrSect = Root;
    ResolveVFExtdefs();
    do {
        LinkState &= ~LIBRARIES_ADDED;
        for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
            if( lib->status & STAT_SEEN_LIB ) {
                lib->status |= STAT_OLD_LIB;
            } else {
                lib->status |= STAT_SEEN_LIB;
            }
        }
        for( sym = HeadSym; sym != NULL; sym = sym->link ) {
            if( ((!(sym->info & SYM_DEFINED) && !IS_SYM_WEAK_REF(sym))
                 || (FmtData.type & MK_NOVELL && IS_SYM_IMPORTED(sym)
                    && sym->info & (SYM_REFERENCED | SYM_LOCAL_REF)))
                && !(sym->info & SYM_IS_ALTDEF) ) {
                LibFind( sym->name, (sym->info & SYM_CHECKED) != 0 );
            }
            sym->info |= SYM_CHECKED;
        }
        keepgoing = ResolveVFExtdefs();
    } while( keepgoing || LinkState & LIBRARIES_ADDED );

    BurnLibs();
    PrintBadTraces();
}
