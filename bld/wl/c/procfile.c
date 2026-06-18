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
* Description:  PROCFILES   : process object file.
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include "pcobj.h"
#include "library.h"
#include "ar.h"
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
#include "loadpe.h"


static void BadSkip( const file_list *file, unsigned long *loc )
/**************************************************************/
{
    /* unused parameters */ (void)file; (void)loc;

    BadObjFormat();
}

static struct {
    void (*SkipObj)( const file_list *, unsigned long * );
    unsigned long (*Pass1)( void );
} Process[] = {
    /* SkipObj       Pass1                                                                  */
    { BadSkip,      BadObjFormat },     /* FMT_PE_XFER      .obj is PE xfer code segment    */
    { OMFSkipObj,   OMFPass1 },         /* FMT_OMF          .obj is an OMF object file      */
    { ORLSkipObj,   ORLPass1 },         /* FMT_COFF         .obj is a COFF object file      */
    { ORLSkipObj,   ORLPass1 },         /* FMT_ELF          .obj is an ELF object file      */
    { BadSkip,      IncPass1 },         /* FMT_INCREMENTAL  .obj is saved inc. linking info */
    { BadSkip,      BadObjFormat },     /* unused                                           */
    { BadSkip,      BadObjFormat },     /* unused                                           */
    { BadSkip,      BadObjFormat }      /* unused                                           */
};

void SetupFakeModule( void )
/*********************************/
{
    if( FmtData.type & MK_PE ) {
        FakeModule = NewModEntry();
        FakeModule->modinfo = DBI_ALL | MOD_LAST_SEG | MOD_NEED_PASS_2 | FMT_PE_XFER;
        FakeModule->name.u.ptr = AddStringStringTable( &PermStrings, LinkerModule );
        DBIInitModule( FakeModule );
    }
}

void LinkFakeModule( void )
/********************************/
{
    if( FmtData.type & MK_PE ) {
        // Unlike other modules, fake module goes to the beginning of the list
        FakeModule->n.next_mod = LibModules;
        LibModules = FakeModule;
    }
}

static void CheckNewFile( mod_entry *mod, file_list *file,
                          int AlwaysCheckUsingDate)
/*********************************************************/
{
    time_t      modtime;

    if( (LinkFlags & LF_GOT_CHGD_FILES) == 0 || AlwaysCheckUsingDate ) {
        if( QModTime( file->infile->name.u.ptr, &modtime ) || modtime > mod->modtime ) {
            file->flags |= STAT_HAS_CHANGED;
        }
    } else {
        if( FindHTableElem( Root->modFilesHashed, file->infile->name.u.ptr ) ) {
            file->flags |= STAT_HAS_CHANGED;
        }
    }
}

static void SetStartAddr( void )
/******************************/
{
    mod_entry   *mod;

    if( StartInfo.user_specd || !StartInfo.from_inc )
        return;
    mod = StartInfo.mod;
    if( mod == NULL )
        return;
    if( (mod->modinfo & MOD_KILL) || (mod->f.source->flags & STAT_HAS_CHANGED) ) {
        ClearStartAddr();
    }
}

static void SetupModule( mod_entry **mod, file_list *file )
/*********************************************************/
{
    char        *fname;
    mod_entry   *currmod;

    currmod = *mod;

    CheckNewFile( currmod, file, 0 );
    fname = currmod->f.fname.u.ptr;
    for( ;; ) {
        currmod->f.source = file;
        currmod = currmod->n.next_mod;
        if( currmod == NULL )
            break;
        if( currmod->f.fname.u.ptr != fname ) {
            break;
        }
    }
    *mod = currmod;
}

static void DoIncLibDefs( void )
/******************************/
{
    libnamelist         *lib;

    for( lib = SavedDefLibs; lib != NULL; lib = lib->next ) {
        AddObjLib( lib->name, lib->priority );
    }
}

static libnamelist *CalcLibBlacklist( void )
/*******************************************/
/* figure out if the user has changed any of the specified libraries, and
 * torch anything after a changed library */
{
    infilelist          *userlibs_infile;
    libnamelist         *oldlibs;

    userlibs_infile = CachedLibFiles;
    for( oldlibs = SavedUserLibs; oldlibs != NULL; oldlibs = oldlibs->next ) {
        if( userlibs_infile == NULL )
            return( oldlibs );
        if( FNAMECMPSTR( userlibs_infile->name.u.ptr, oldlibs->name ) != 0 )
            return( oldlibs );
        userlibs_infile = userlibs_infile->next;
    }
    return( NULL );
}

static void CheckBlacklist( file_list *file, libnamelist *blacklist )
/*******************************************************************/
{
    size_t      length;
    size_t      length_b;
    size_t      delta;

    if( file->flags & STAT_HAS_CHANGED )
        return;
    length = strlen( file->infile->name.u.ptr );
    for( ; blacklist != NULL; blacklist = blacklist->next ) {
        length_b = strlen( blacklist->name );
        if( length >= length_b ) {
            delta = length - length_b;
            if( FNAMECMPSTR( blacklist->name, file->infile->name.u.ptr + delta ) == 0 ) {
                file->flags |= STAT_HAS_CHANGED;
                return;
            }
        }
    }
}

static void PrepareModList( void )
/********************************/
{
    file_list   *file;
    mod_entry   *mod;
    mod_entry   *curr;
    libnamelist *blacklist;

    mod = Root->mods;
    for( file = Root->files; file != NULL && mod != NULL; file = file->next_file ) {
        if( strcmp( file->infile->name.u.ptr, mod->f.fname.u.ptr ) == 0 ) {
            SetupModule( &mod, file );
        } else if( mod->n.next_mod != NULL ) {
            if( FNAMECMPSTR( file->infile->name.u.ptr, mod->n.next_mod->f.fname.u.ptr ) == 0 ) {
                mod->modinfo |= MOD_KILL;
                mod = mod->n.next_mod;
                SetupModule( &mod, file );
            }
        }
    }
    for( ; mod != NULL; mod = mod->n.next_mod ) {
        mod->modinfo |= MOD_KILL;               // no match found
    }
    blacklist = CalcLibBlacklist();
    for( mod = LibModules; mod != NULL; mod = mod->n.next_mod ) {
        if( mod->f.fname.u.ptr == NULL ) {
            mod->modinfo |= MOD_KILL;
        } else if( (mod->modinfo & MOD_VISITED) == 0 ) {
            file = AddObjLib( mod->f.fname.u.ptr, LIB_PRIORITY_MID );
            CheckNewFile( mod, file, 1);
            CheckBlacklist( file, blacklist );
            for( curr = mod->n.next_mod; curr != NULL; curr = curr->n.next_mod){
                if( curr->f.fname.u.ptr == mod->f.fname.u.ptr ) {
                    curr->f.source = file;
                    curr->modinfo |= MOD_VISITED;
                }
            }
            mod->f.source = file;
        }
        mod->modinfo &= ~MOD_VISITED;
    }
    FreeList( SavedUserLibs );
    SavedUserLibs = NULL;
}

static void MarkDefaultSyms( void )
/*********************************/
{
    symbol *sym;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_ALIAS( sym ) && (sym->info & SYM_WAS_LAZY) ) {
            sym->e.def->info |= SYM_RELOC_REFD;
        }
    }
}

static void MarkRelocs( mod_entry *mod )
/**************************************/
{
    IterateModRelocs( mod->relocs, mod->sizerelocs, RelocMarkSyms );
}

static void KillASym( void *_sym )
/********************************/
{
    symbol *sym = _sym;

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

static void SetAltDefData( void *_sym )
/*************************************/
{
    symbol *sym = _sym;

    if( (sym->info & SYM_IS_ALTDEF) && IS_SYM_COMDAT( sym ) && (sym->info & SYM_HAS_DATA) == 0 ) {
        sym->p.seg->u1.vm_ptr = sym->e.mainsym->p.seg->u1.vm_ptr;
    }
}

static void FixModAltDefs( mod_entry *mod )
/*****************************************/
{
    Ring2Walk( mod->publist, SetAltDefData );
}

static void IncIterateMods( mod_entry *mod, void (*proc_fn)(mod_entry *), bool dochanged )
/****************************************************************************************/
{
    bool haschanged;

    for( ; mod != NULL; mod = mod->n.next_mod ) {
        haschanged = (mod->modinfo & MOD_KILL) || (mod->f.source->flags & STAT_HAS_CHANGED);
        if( haschanged == dochanged ) {
            proc_fn( mod );
        }
    }
}

static void AddToModList( mod_entry *mod )
/****************************************/
{
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
    ObjFileFormat = FMT_INCREMENTAL;
    mod->modinfo &= ~FMT_OBJ_FMT_MASK;
    mod->modinfo |= FMT_INCREMENTAL;
    AddToModList( mod );
    ObjPass1();
}

static void FreeModSegments( mod_entry *mod )
/*******************************************/
{
    mod->publist = NULL;
    Ring2CarveFree( CarveSegData, &mod->segs );
}

static member_list *FindMember( file_list *file, char *name )
/***********************************************************/
// see if name is in the member list of list
{
    member_list         **member;
    member_list         *found;

    found = NULL;
    for( member = &file->u.member; *member != NULL; member = &(*member)->next ) {
        if( ModNameCompare( name, (*member)->name ) ) {
            found = *member;
            *member = (*member)->next;      // remove it from the list.
            break;
        }
    }
    return( found );
}

static bool EndOfLib( file_list *file, unsigned long loc )
/********************************************************/
{
    unsigned_8 *id;

    if( file->flags & STAT_OMF_LIB ) {
        id = CacheRead( file, loc, sizeof( unsigned_8 ) );
        return( *id == LIB_TRAILER_REC );
    } else {
        return( false );
    }
}

static void DoPass1( mod_entry *next, file_list *file )
/*****************************************************/
/* do pass 1 on the object file */
{
    member_list         *member;
    char                *membname;
    unsigned long       loc;
    unsigned long       size;
    unsigned_16         reclength;
    bool                lastmod;
    bool                ignoreobj;

    loc = 0;
    lastmod = false;
    if( CacheOpen( file ) ) {
        reclength = (unsigned_16)CheckLibraryType( file, &loc, false );
        for( ;; ) { /*  there may be more than 1 object module in a file */
            member = NULL;
            ignoreobj = false;
            if( EndOfLib( file, loc ) )
                break;
            membname = IdentifyObject( file, &loc, &size );
            if( file->flags & STAT_IS_LIB ) {
                if( (file->flags & STAT_HAS_MEMBER) && file->u.member != NULL ) {
                    member = FindMember( file, membname );
                    if( member == NULL ) {
                        ignoreobj = true;
                    } else if( file->u.member == NULL ) {
                        lastmod = true;
                    }
                }
            }
            if( ignoreobj ) {
                MemFree( membname );
                if( size != 0 ) {
                    loc += size;
                } else {
                    Process[GET_FMT_IDX( ObjFileFormat )].SkipObj( file, &loc );
                }
            } else {
                if( next == NULL ) {
                    next = NewModEntry();
                }
                next->n.next_mod = NULL;
                next->f.source = file;
                next->modtime = next->f.source->infile->modtime;
                next->modinfo |= ObjFileFormat & FMT_OBJ_FMT_MASK;
                if( member != NULL ) {
                    next->modinfo |= member->flags;
                    MemFree( member );
                }
                if( (file->flags & STAT_HAS_MEMBER) == 0 ) {
                    next->modinfo |= file->flags & DBI_MASK;
                    if( file->flags & STAT_LAST_SEG ) {
                        next->modinfo |= MOD_LAST_SEG;
                    }
                }
                AddToModList( next );
                next->location = loc;
                if( membname != NULL ) {
                    next->name.u.ptr = AddStringStringTable( &PermStrings, membname );
                    MemFree( membname );
                } else {
                    next->name.u.ptr = file->infile->name.u.ptr;
                }
                loc = ObjPass1();
                if( file->flags & STAT_TRACE_SYMS ) {
                    TraceSymList( CurrMod->publist );
                }
                next = NULL;
            }
            ObjFileFormat = 0;
            ObjFormat = 0;
            if( file->flags & STAT_IS_LIB ) {      // skip library padding.
                unsigned_16 modulus;

                modulus = (unsigned_16)( loc % reclength );
                if( modulus != 0 ) {
                    loc += reclength - modulus;     // go to library boundary.
                }
            } else if( !IS_FMT_OMF( CurrMod->modinfo ) ) {
                break;          // can only concat omf.
            }
            if( lastmod || CacheIsEnd( file, loc ) ) {
                break;
            }
        }
        if( file->u.member != NULL ) {
            LnkMsg( ERR+MSG_CANT_FIND_MEMBER, "12", file->infile->name.u.ptr, file->u.member->name );
        }
        CacheClose( file, 1 );
    }
    CheckStop();
}

static void ProcessMods( void )
/*****************************/
{
    mod_entry   *mod;
    mod_entry   *next;
    mod_entry   *savemod;
    file_list   *file;

    mod = Root->mods;
    Root->mods = NULL;
    for( file = Root->files; file != NULL && mod != NULL; file = file->next_file ) {
        for( ; mod != NULL; mod = next ) {
            next = mod->n.next_mod;
            if( mod->modinfo & MOD_KILL ) {
                FreeModEntry( mod );
            } else if( mod->f.source != file ) {
                DoPass1( NULL, file );
                break;
            } else {
                if( file->flags & STAT_HAS_CHANGED ) {
                    memset( mod, 0, sizeof( mod_entry ) );
                    DoPass1( mod, file );
                } else {
                    SavedPass1( mod );
                }
            }
        }
    }
    for( ; mod != NULL; mod = next ) {
        next = mod->n.next_mod;
        FreeModEntry( mod );
    }
    for( ; file != NULL; file = file->next_file ) {
        DoPass1( NULL, file );
    }
    savemod = Root->mods;       // pass1 routines will add new mods to this
    Root->mods = NULL;
    CurrMod = NULL;
    for( mod = LibModules; mod != NULL; mod = next ) {
        next = mod->n.next_mod;
        if( (mod->modinfo & MOD_KILL)
                || mod->f.source != NULL && (mod->f.source->flags & STAT_HAS_CHANGED) ) {
            FreeModEntry( mod );
        } else {
            SavedPass1( mod );
        }
    }
    LibModules = Root->mods;
    Root->mods = savemod;
}

static void IncLoadObjFiles( void )
/*********************************/
{
    PrepareModList();
    SetStartAddr();
    MarkDefaultSyms();
    IncIterateMods( Root->mods, MarkRelocs, false );
    IncIterateMods( LibModules, MarkRelocs, false );
    IncIterateMods( Root->mods, FixModAltDefs, false );
    IncIterateMods( LibModules, FixModAltDefs, false );
    IncIterateMods( Root->mods, KillSyms, true );
    IncIterateMods( LibModules, KillSyms, true );
    PurgeSymbols();
    IncIterateMods( Root->mods, FreeModSegments, true );
    IncIterateMods( LibModules, FreeModSegments, true );
    ProcessMods();
    DoIncGroupDefs();
    DoIncLibDefs();
}

void ProcObjFiles( void )
/******************************/
/* Perform Pass 1 on all object files */
{
    CurrMod = NULL;
    if( LinkFlags & LF_INC_LINK_FLAG ) {
        if( (LinkFlags & LF_DWARF_DBI_FLAG) == 0 && (LinkFlags & LF_ANY_DBI_FLAG) ) {
            LnkMsg( FTL+MSG_INC_ONLY_SUPPORTS_DWARF, NULL );
        }
        if( LinkFlags & LF_STRIP_CODE ) {
            LnkMsg( FTL+MSG_INC_AND_DCE_NOT_ALLOWED, NULL );
        }
        if( LinkFlags & LF_VF_REMOVAL ) {
            LnkMsg( FTL+MSG_INC_AND_VFR_NOT_ALLOWED, NULL );
        }
    }
    LnkMsg( INF+MSG_LOADING_OBJECT, NULL );
    if( LinkFlags & LF_STRIP_CODE ) {
        LinkState |= LS_CAN_REMOVE_SEGMENTS;
    }
    if( LinkState & LS_GOT_PREV_STRUCTS ) {
        IncLoadObjFiles();
    } else {
        LoadObjFiles( Root );
#ifdef _EXE
        if( FmtData.type & MK_OVERLAYS ) {
            OvlPass1();
        }
#endif
    }
}

void LoadObjFiles( section *sect )
/********************************/
{
    file_list   *file;

    CurrSect = sect;
    CurrMod = NULL;
    for( file = sect->files; file != NULL; file = file->next_file ) {
        DoPass1( NULL, file );
    }
}

char *IdentifyObject( const file_list *file, unsigned long *loc, unsigned long *size )
/************************************************************************************/
{
    ar_header       *ar_hdr;
    char            *name;
    unsigned long   ar_loc;

    name = NULL;
    *size = 0;
    ar_loc = 0;
    if( file->flags & STAT_AR_LIB ) {
        ar_loc = __ROUND_UP_SIZE_EVEN( *loc );     /* AR headers are word aligned. */
        ar_hdr = CacheRead( file, ar_loc, sizeof( ar_header ) );
        ar_loc += sizeof( ar_header );
        name = GetARName( ar_hdr, file, &ar_loc );
        *size = GetARValue( ar_hdr->size, AR_SIZE_LEN );
        *loc = ar_loc;
    }
    switch( FileTypeORL( file, *loc ) ) {
    case ORL_ELF:
        ObjFileFormat |= FMT_ELF;
        break;
    case ORL_COFF:
        ObjFileFormat |= FMT_COFF;
        break;
    case ORL_OMF:
        ObjFileFormat |= FMT_OMF;
        if( (file->flags & STAT_IS_LIB) == STAT_OMF_LIB ) {
            name = GetOMFName( file, loc );
        }
        break;
    default:
        break;
    }
    return( name );
}

unsigned long ObjPass1( void )
/****************************/
/* Pass 1 of 8086 linker. */
{
    unsigned long loc;

    DEBUG(( DBG_BASE, "1 : file = %s, module = %s", CurrMod->f.source->infile->name.u.ptr, CurrMod->name.u.ptr ));
    CurrMod->modinfo |= MOD_DONE_PASS_1;
    SymModStart();
    DBIInitModule( CurrMod );
    RelocStartMod();
    P1Start();
    loc = Process[GET_FMT_IDX( ObjFileFormat )].Pass1();
    CollapseLazyExtdefs();
    SymModEnd();
    DBIP1ModuleScanned();
    ReleaseNames();
    PermEndMod( CurrMod );
    FreeObjInfo();
    ObjFileFormat = 0;
    ObjFormat = 0;       //clear flags for processing obj file
    return( loc );
}

static bool ResolveVFExtdefs( void )
/**********************************/
/* go through the symbol table, and check if any conditional extdefs
 * should turned into real extdefs */
{
    bool        resolved;
    symbol      *sym;

    resolved = false;
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_VF_REF( sym ) ) {
            resolved |= CheckVFList( sym );
        }
    }
    return( resolved );
}

void ResolveUndefined( void )
/**********************************/
{
    symbol      *sym;
    file_list   *file;
    bool        keepgoing;

    LnkMsg( INF+MSG_SEARCHING_LIBS, NULL );
#ifdef _EXE
    if( (FmtData.type & MK_OVERLAYS) && FmtData.u.dos.distribute ) {
        LinkState |= LS_CAN_REMOVE_SEGMENTS;
        DistribInitMods();
    }
#endif
    CurrSect = Root;
    ResolveVFExtdefs();
    do {
        LinkState &= ~LS_LIBRARIES_ADDED;
        for( file = ObjLibFiles; file != NULL; file = file->next_file ) {
            if( file->flags & STAT_SEEN_LIB ) {
                file->flags |= STAT_OLD_LIB;
            } else {
                file->flags |= STAT_SEEN_LIB;
            }
        }
        for( sym = HeadSym; sym != NULL; sym = sym->link ) {
            if( ( (sym->info & SYM_DEFINED) == 0 && !IS_SYM_WEAK_REF( sym )
                || (FmtData.type & MK_NOVELL) && IS_SYM_IMPORTED( sym )
                  && (sym->info & (SYM_REFERENCED | SYM_LOCAL_REF)) )
              && (sym->info & SYM_IS_ALTDEF) == 0 ) {
                LibFind( sym->name.u.ptr, (sym->info & SYM_CHECKED) != 0 );
            }
            sym->info |= SYM_CHECKED;
        }
        keepgoing = ResolveVFExtdefs();
    } while( keepgoing || (LinkState & LS_LIBRARIES_ADDED) );

    BurnLibs();
    PrintBadTraces();
}

void ProcLocalImports( void )
/***************************/
{
#ifdef _OS2
    symbol  *sym;

    if( FmtData.type & MK_PE ) {
        for( sym = HeadSym; sym != NULL; sym = sym->link ) {
            if( (sym->info & SYM_DEFINED) == 0 && !IS_SYM_WEAK_REF( sym ) && (sym->info & SYM_IS_ALTDEF) == 0 ) {
                ImportPELocalSym( sym );
            }
        }
    }
#endif
}

void FreeLocalImports( void )
/***************************/
{
#ifdef _OS2
    FreePELocalImports();
#endif
}
