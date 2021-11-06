/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Debug Information Processor (DIP) manager.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <limits.h>
#if defined( __WINDOWS__ )
#elif defined( __NT__ )
#include <windows.h>
#elif defined( __OS2__ )
#include <os2.h>
#endif
#include "walloca.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "dipsys.h"


#define IMAGE_MAP_INIT  16
#define IMAGE_MAP_GROW  16
#define MAX_LOAD_DIPS   32
#define MAX_REG_DIPS    1
#define MAX_DIPS        (MAX_LOAD_DIPS+MAX_REG_DIPS)

#define CH2ICH( hp )    ((imp_cue_handle *)((hp) + 1))
#define IH2IIH( hp )    ((imp_image_handle *)((hp) + 1))
#define SH2ISH( hp )    ((imp_sym_handle *)((hp) + 1))
#define TH2ITH( hp )    ((imp_type_handle *)((hp) + 1))

#define ICH2CH( hp )    (((cue_handle *)(hp))-1)
#define IIH2IH( hp )    (((image_handle *)(hp))-1)
#define ISH2SH( hp )    (((sym_handle *)(hp))-1)
#define ITH2TH( hp )    (((type_handle *)(hp))-1)

#define MK_MH( ii, sm ) ((unsigned_32)((ii)+1) << 16 | (sm))
#define MH_IMAGE( mh )  ((unsigned)((mh)>>16)-1)
#define II2IH( ii )     (((ActProc==NULL)||(ActProc->map_entries<=ii))?NULL:ActProc->map_ih[ii])
#define MH2IH( mh )     (((mh&0xffff0000)==0)?NULL:II2IH(MH_IMAGE(mh)))

#define NO_IMAGE_IDX    ((image_idx)-1)

enum artificial_pointers {
    AP_NEAR2 = 1,
    AP_FAR2,
    AP_NEAR4,
    AP_FAR4,
    AP_MASK = 0x7,
    AP_SHIFT = 3,
    AP_FULL  = 0xe000
};

typedef unsigned_16     image_idx;

struct image_handle {
    dip_imp_routines    *dip;
    image_handle        *next;
    void                *extra;
    image_idx           ii;
    unsigned            : 0;    /* for alignment */
};

struct type_handle {
    image_idx           ii;
    unsigned short      ap;
    unsigned            : 0;    /* for alignment */
};

struct cue_handle {
    image_idx           ii;
    unsigned            : 0;    /* for alignment */
};

struct sym_handle {
    image_idx           ii;
    unsigned            : 0;    /* for alignment */
};

struct process_info {
    image_idx           last_addr_mod_found;
    image_idx           map_entries;
    image_handle        **map_ih;
    image_handle        *list_ih;
    image_handle        **add_ih;
};

static image_idx        LoadingImageIdx = NO_IMAGE_IDX;

static process_info     *ActProc;
static struct {
    dip_imp_routines    *rtns;
    dip_sys_handle      sys_hdl;
}                       LoadedDIPs[MAX_DIPS];

static unsigned         MaxHdlSize[] = {
    #define pick(e,hdl,imphdl,wvimphdl) 0,
    #include "diphndls.h"
    #undef pick
};

static const unsigned_8 MgrHdlSize[] = {
    #define pick(e,hdl,imphdl,wvimphdl) hdl,
    #include "diphndls.h"
    #undef pick
};

char DIPDefaults[] = {
#ifdef USE_FILENAME_VERSION
    "dwarf"  QUOTED( USE_FILENAME_VERSION ) "\0"
    "watcom" QUOTED( USE_FILENAME_VERSION ) "\0"
    "codevi" QUOTED( USE_FILENAME_VERSION ) "\0"
    "mapsym" QUOTED( USE_FILENAME_VERSION ) "\0"
    "export" QUOTED( USE_FILENAME_VERSION ) "\0"
#else
    "dwarf\0"
    "watcom\0"
    "codeview\0"
    "mapsym\0"
    "export\0"
#endif
    "\0"
};

const address    NilAddr = { 0 };

/*
 * Client interface
 */

static dip_client_routines DIPClientInterface = {
    DIP_MAJOR,
    DIP_MINOR,
    sizeof( dip_client_routines ),
    DIGCli( Alloc ),
    DIGCli( Realloc ),
    DIGCli( Free ),
    DIPCli( MapAddr ),
    DIPCli( SymCreate ),
    DIPCli( ItemLocation ),
    DIPCli( AssignLocation ),
    DIPCli( SameAddrSpace ),
    DIPCli( AddrSection ),
    DIGCli( Open ),
    DIGCli( Seek ),
    DIGCli( Tell ),
    DIGCli( Read ),
    DIGCli( Write ),
    DIGCli( Close ),
    DIGCli( Remove ),
    DIPCli( Status ),
    DIPCli( CurrArch ),
    DIGCli( MachineData )
};


/*
 *      Control Routines
 */
dip_status DIPInit( void )
{
    return( DS_OK );
}

static void SetHdlSizes( dip_imp_routines *rtns )
{
    handle_kind hk;
    unsigned    size;

    for( hk = 0; hk < MAX_HK; ++hk ) {
        size = rtns->HandleSize( hk ) + MgrHdlSize[hk];
        if( MaxHdlSize[hk] < size ) {
            MaxHdlSize[hk] = size;
        }
    }
}

dip_status DIPLoad( const char *path )
{
    int         i;
    dip_status  ds;

    for( i = 0; LoadedDIPs[i].rtns != NULL; ++i ) {
        if( i >= MAX_LOAD_DIPS ) {
            return( DS_ERR | DS_TOO_MANY_DIPS );
        }
    }
    ds = DIPSysLoad( path, &DIPClientInterface, &LoadedDIPs[i].rtns, &LoadedDIPs[i].sys_hdl );
    if( ds != DS_OK )
        return( ds );
    if( DIPClientInterface.major != LoadedDIPs[i].rtns->major
      || DIPClientInterface.minor > LoadedDIPs[i].rtns->minor ) {
        DIPSysUnload( &LoadedDIPs[i].sys_hdl );
        LoadedDIPs[i].rtns = NULL;
        return( DS_ERR | DS_INVALID_DIP_VERSION );
    }
    SetHdlSizes( LoadedDIPs[i].rtns );
    return( DS_OK );
}

dip_status DIPRegister( dip_imp_routines *dir )
{
    int     i;

    for( i = MAX_LOAD_DIPS; LoadedDIPs[i].rtns != NULL; ++i ) {
        if( i >= MAX_DIPS ) {
            return( DS_ERR | DS_TOO_MANY_DIPS );
        }
    }
    LoadedDIPs[i].rtns = dir;
    LoadedDIPs[i].sys_hdl = NULL_SYSHDL;
    SetHdlSizes( LoadedDIPs[i].rtns );
    return( DS_OK );
}

void DIPFiniLatest( void )
{
    int     i;

    for( i = MAX_DIPS - 1; i >= 0; --i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->Shutdown();
            LoadedDIPs[i].rtns = NULL;
            DIPSysUnload( &LoadedDIPs[i].sys_hdl );
            break;
        }
    }
}

void DIPFini( void )
{
    int     i;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->Shutdown();
            LoadedDIPs[i].rtns = NULL;
        }
        DIPSysUnload( &LoadedDIPs[i].sys_hdl );
    }
}

size_t DIPHandleSize( handle_kind hk )
{
    return( MaxHdlSize[hk] );
}

size_t DIPHandleSizeWV( handle_kind hk )
{
    return( MgrHdlSize[hk] );
}

dip_status DIPMoreMem( size_t amount )
{
    int     i;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            if( LoadedDIPs[i].rtns->MoreMem( amount ) == DS_OK ) {
                return( DS_OK );
            }
        }
    }
    return( DS_FAIL );
}

void DIPCancel( void )
{
    int                 i;
    image_handle        *ih;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->Cancel();
        }
    }
    if( LoadingImageIdx != NO_IMAGE_IDX ) {
        ih = II2IH( LoadingImageIdx );
        if( ih != NULL ) {
            ih->dip->UnloadInfo( IH2IIH( ih ) );
        }
        LoadingImageIdx = NO_IMAGE_IDX;
    }
}

static image_idx FindImageMapSlot( process_info *p )
{
    image_handle        **new_ih;
    image_idx           new_num;
    image_idx           ii;
    image_idx           j;

    for( ii = 0; ii < p->map_entries; ++ii ) {
        if( p->map_ih[ii] == NULL ) {
            return( ii );
        }
    }
    new_num = p->map_entries + IMAGE_MAP_GROW;
    new_ih = DIGCli( Realloc )( p->map_ih, new_num * sizeof( p->map_ih[0] ) );
    if( new_ih == NULL ) {
        DIPCli( Status )( DS_ERR | DS_NO_MEM );
        return( NO_IMAGE_IDX );
    }
    ii = p->map_entries;
    p->map_entries = new_num;
    p->map_ih = new_ih;
    for( j = ii; j < new_num; ++j )
        new_ih[j] = NULL;
    return( ii );
}

static void DIPCleanupInfo( process_info *p, image_handle *ih )
{
    image_handle        **owner;
    image_handle        *curr;

    DIPCli( ImageUnload )( MK_MH( ih->ii, 0 ) );
    ih->dip->UnloadInfo( IH2IIH( ih ) );
    p->map_ih[ih->ii] = NULL;
    owner = &p->list_ih;
    for( ;; ) {
        curr = *owner;
        if( curr == ih )
            break;
        owner = &curr->next;
    }
    *owner = ih->next;
    if( p->add_ih == &ih->next ) {
        p->add_ih = owner;
    }
    if( ih->ii == p->last_addr_mod_found ) {
        p->last_addr_mod_found = NO_IMAGE_IDX;
    }
    DIGCli( Free )( ih );
}

static void CleanupProcess( process_info *p, int unload )
{
    image_handle        *ih;
    image_handle        *next;

    if( unload ) {
        while( p->list_ih ) {
            DIPCleanupInfo( p, p->list_ih );
        }
    } else {
        for( ih = p->list_ih; ih != NULL; ih = next ) {
            next = ih->next;
            DIGCli( Free )( ih );
        }
    }
    DIGCli( Free )( p->map_ih );
    DIGCli( Free )( p );
}

process_info *DIPCreateProcess( void )
{
    process_info        *p;
    image_handle        *ih;
    image_idx           ii;
    int                 j;

    p = DIGCli( Alloc )( sizeof( process_info ) );
    if( p == NULL ) {
        DIPCli( Status )( DS_ERR | DS_NO_MEM );
        return( NULL );
    }
    p->last_addr_mod_found = NO_IMAGE_IDX;
    p->map_ih = DIGCli( Alloc )( IMAGE_MAP_INIT * sizeof( p->map_ih[0] ) );
    if( p->map_ih == NULL ) {
        DIGCli( Free )( p );
        DIPCli( Status )( DS_ERR | DS_NO_MEM );
        return( NULL );
    }
    p->map_entries = IMAGE_MAP_INIT;
    for( ii = 0; ii < IMAGE_MAP_INIT; ++ii )
        p->map_ih[ii] = NULL;
    p->list_ih = NULL;
    p->add_ih = &p->list_ih;
    for( j = MAX_DIPS - 1; j >= MAX_LOAD_DIPS; --j ) {
        if( LoadedDIPs[j].rtns != NULL ) {
            ii = FindImageMapSlot( p );
            if( ii == NO_IMAGE_IDX ) {
                CleanupProcess( p, 0 );
                DIPCli( Status )( DS_ERR | DS_NO_MEM );
                return( NULL );
            }
            ih = DIGCli( Alloc )( sizeof( image_handle ) );
            if( ih == NULL ) {
                CleanupProcess( p, 0 );
                DIPCli( Status )( DS_ERR | DS_NO_MEM );
                return( NULL );
            }
            p->map_ih[ii] = ih;
            ih->next = *p->add_ih;
            *p->add_ih = ih;
            p->add_ih = &ih->next;
            ih->dip = LoadedDIPs[j].rtns;
            ih->extra = NULL;
            ih->ii = ii;
        }
    }
    p->add_ih = &p->list_ih;
    if( ActProc == NULL )
        ActProc = p;
    return( p );
}

process_info *DIPSetProcess( process_info *new )
{
    process_info        *old;

    old = ActProc;
    ActProc = new;
    return( old );
}

void DIPDestroyProcess( process_info *p )
{
    CleanupProcess( p, 1 );
    if( p == ActProc ) {
        ActProc = NULL;
    }
}

dip_priority DIPPriority( dip_priority prev_priority )
{
    dip_priority    priority;
    dip_priority    curr_priority;
    int             j;

    priority = DIP_PRIOR_MAX + 1;
    for( j = 0; j < MAX_DIPS; ++j ) {
        if( LoadedDIPs[j].rtns == NULL )
            continue;
        curr_priority = LoadedDIPs[j].rtns->priority;
        if( curr_priority > DIP_PRIOR_MAX )
            curr_priority = DIP_PRIOR_MAX;
        if( curr_priority <= prev_priority )
            continue;
        if( curr_priority >= priority )
            continue;
        priority = curr_priority;
    }
    return( priority > DIP_PRIOR_MAX ? 0 : priority );
}

mod_handle DIPLoadInfo( FILE *fp, unsigned extra, dip_priority priority )
{
    image_idx           ii;
    int                 j;
    image_handle        *ih;
    dip_status          ds;

    if( ActProc == NULL ) {
        DIPCli( Status )( DS_ERR | DS_NO_PROCESS );
        return( NO_MOD );
    }
    ii = FindImageMapSlot( ActProc );
    if( ii == NO_IMAGE_IDX )
        return( NO_MOD );
    ih = DIGCli( Alloc )( MaxHdlSize[HK_IMAGE] + extra );
    if( ih == NULL ) {
        DIPCli( Status )( DS_ERR | DS_NO_MEM );
        return( NO_MOD );
    }
    for( j = 0; j < MAX_DIPS; ++j ) {
        if( LoadedDIPs[j].rtns == NULL )
            continue;
        if( LoadedDIPs[j].rtns->priority != priority )
            continue;
        ds = LoadedDIPs[j].rtns->LoadInfo( fp, IH2IIH( ih ) );
        if( ds == DS_OK ) {
            ActProc->map_ih[ii] = ih;
            ih->next = *ActProc->add_ih;
            *ActProc->add_ih = ih;
            ActProc->add_ih = &ih->next;
            ih->dip = LoadedDIPs[j].rtns;
            ih->extra = (unsigned_8 *)ih + MaxHdlSize[HK_IMAGE];
            ih->ii = ii;
            LoadingImageIdx = ii;
            return( MK_MH( ii, 0 ) );
        }
        if( ds & DS_ERR ) {
            break;
        }
    }
    DIGCli( Free )( ih );
    return( NO_MOD );
}

void DIPMapInfo( mod_handle mh, void *d )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih != NULL ) {
        ih->dip->MapInfo( IH2IIH( ih ), d );
    }
    LoadingImageIdx = NO_IMAGE_IDX;
}

void DIPUnloadInfo( mod_handle mh )
{
    image_handle *ih;

    ih = MH2IH( mh );
    if( ih != NULL ) {
        DIPCleanupInfo( ActProc, ih );
    }
}


dip_priority DIPImagePriority( mod_handle mh )
{
    image_handle        *ih;

    if( mh == NO_MOD )
        return( DIP_PRIOR_MAX + 1 );
    ih = MH2IH( mh );
    if( ih == NULL )
        return( DIP_PRIOR_MAX + 1 );
    return( ih->dip->priority );
}

/*
 *      Information Walkers
 */
walk_result DIPWalkImageList( DIP_IMAGE_WALKER *iw, void *d )
{
    walk_result         wr;
    image_handle        *ih;

    wr = WR_CONTINUE;
    if( ActProc != NULL ) {
        for( ih = ActProc->list_ih; ih != NULL; ih = ih->next ) {
            wr = iw( MK_MH( ih->ii, 0 ), d );
            if( wr != WR_CONTINUE ) {
                break;
            }
        }
    }
    return( wr );
}

typedef struct {
    union {
        DIP_MOD_WALKER      *m;
        DIP_TYPE_WALKER     *t;
        DIP_SYM_WALKER      *s;
        DIP_CUE_WALKER      *c;
    }           walk;
    void                *d;
    location_context    *lc;
} walk_glue;

static walk_result DIGCLIENT ModGlue( imp_image_handle *iih, imp_mod_handle imh, void *d )
{
    walk_glue           *wd = d;
    image_handle        *ih;

    ih = IIH2IH( iih );
    return( wd->walk.m( MK_MH( ih->ii, imh ), wd->d ) );
}

static walk_result WalkOneModList( mod_handle mh, void *d )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( WR_STOP );
    return( ih->dip->WalkModList( IH2IIH( ih ), ModGlue, d ) );
}

walk_result DIPWalkModList( mod_handle mh, DIP_MOD_WALKER *mw, void *d )
{
    walk_glue           glue;

    if( ActProc == NULL ) {
        DIPCli( Status )( DS_ERR | DS_NO_PROCESS );
        return( WR_FAIL );
    }
    glue.walk.m = mw;
    glue.d = d;
    glue.lc = NULL;
    if( mh == NO_MOD ) {
        return( DIPWalkImageList( WalkOneModList, &glue ) );
    } else {
        return( WalkOneModList( mh, &glue ) );
    }
}

static walk_result DIGCLIENT TypeGlue( imp_image_handle *iih, imp_type_handle *ith, void *d )
{
    walk_glue   *wd = d;

    /* unused parameters */ (void)iih;

    return( wd->walk.t( ITH2TH( ith ), wd->d ) );
}

walk_result DIPWalkTypeList( mod_handle mh, DIP_TYPE_WALKER *tw, void *d )
{
    image_handle        *ih;
    walk_glue           glue;
    type_handle         *th = walloca( MaxHdlSize[HK_TYPE] );
    walk_result         wr;

    wr = WR_CONTINUE;
    ih = MH2IH( mh );
    if( ih != NULL ) {
        glue.walk.t = tw;
        glue.d = d;
        glue.lc = NULL;
        th->ii = MH_IMAGE( mh );
        th->ap = 0;
        wr = ih->dip->WalkTypeList( IH2IIH( ih ), MH2IMH( mh ), TypeGlue, TH2ITH( th ), &glue );
    }
    return( wr );
}

static walk_result DIGCLIENT SymGlue( imp_image_handle *iih, sym_walk_info swi,
                                                    imp_sym_handle *ish, void *d )
{
    walk_glue   *wd = d;
    sym_handle  *sh;

    /* unused parameters */ (void)iih;

    sh = (ish == NULL) ? NULL: ISH2SH( ish );
    return( wd->walk.s( swi, sh, wd->d ) );
}

static walk_result DoWalkSymList( symbol_source, void *, walk_glue * );

static walk_result GblSymWalk( mod_handle mh, void *d )
{
    return( DoWalkSymList( SS_MODULE, (void *)&mh, d ) );
}

static walk_result DoWalkSymList( symbol_source ss, void *start, walk_glue *wd )
{
    image_handle        *ih;
    sym_handle          *sh = walloca( MaxHdlSize[HK_SYM] );
    image_idx           ii = 0;
    imp_mod_handle      imh;
    mod_handle          mh;
    type_handle         *th;
    sym_handle          *scope_sh;
    walk_result         wr;

    switch( ss ) {
    case SS_MODULE:
        mh = *(mod_handle *)start;
        if( mh == NO_MOD ) {
            return( DIPWalkImageList( GblSymWalk, wd ) );
        }
        ii = MH_IMAGE( mh );
        imh = MH2IMH( mh );
        start = &imh;
        break;
    case SS_SCOPED:
        if( DIPAddrMod( *(address *)start, &mh ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        ii = MH_IMAGE( mh );
        break;
    case SS_TYPE:
        th = start;
        if( th->ap != 0 )
            return( WR_CONTINUE );
        start = TH2ITH( th );
        ii = th->ii;
        break;
    case SS_BLOCK:
        if( DIPAddrMod( ((scope_block *)start)->start, &mh ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        ii = MH_IMAGE( mh );
        break;
    case SS_SCOPESYM:
        scope_sh = (sym_handle *)start;
        start = SH2ISH( scope_sh );
        ii = scope_sh->ii;
        break;
    }
    wr = WR_CONTINUE;
    ih = II2IH( ii );
    if( ih != NULL ) {
        sh->ii = ii;
        if( ih->dip->minor == DIP_MINOR ) {
            wr = ih->dip->WalkSymListEx( IH2IIH( ih ), ss, start, SymGlue, SH2ISH( sh ), wd->lc, wd );
        } else {
            wr = ih->dip->WalkSymList( IH2IIH( ih ), ss, start, SymGlue, SH2ISH( sh ), wd );
        }
    }
    return( wr );
}

walk_result DIPWalkSymListEx( symbol_source ss, void *start, DIP_SYM_WALKER *sw, location_context *lc, void *d )
{
    walk_glue   glue;

    glue.walk.s = sw;
    glue.d = d;
    glue.lc = lc;
    return( DoWalkSymList( ss, start, &glue ) );
}

walk_result DIPWalkSymList( symbol_source ss, void *start, DIP_SYM_WALKER *sw, void *d )
{
    return( DIPWalkSymListEx( ss, start, sw, NULL, d ) );
}

static walk_result DIGCLIENT CueGlue( imp_image_handle *iih, imp_cue_handle *icueh, void *d )
{
    walk_glue   *wd = d;

    /* unused parameters */ (void)iih;

    return( wd->walk.c( ICH2CH( icueh ), wd->d ) );
}

walk_result DIPWalkFileList( mod_handle mh, DIP_CUE_WALKER *cw, void *d )
{
    image_handle        *ih;
    walk_glue           glue;
    cue_handle          *cueh = walloca( MaxHdlSize[HK_CUE] );

    ih = MH2IH( mh );
    if( ih == NULL )
        return( WR_CONTINUE );
    glue.walk.c = cw;
    glue.d = d;
    glue.lc = NULL;
    cueh->ii = MH_IMAGE( mh );
    return( ih->dip->WalkFileList( IH2IIH( ih ), MH2IMH( mh ), CueGlue, CH2ICH( cueh ), &glue ) );
}

/*
 * Image Information
 */
void *DIPImageExtra( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    return( ih == NULL ? NULL : ih->extra );
}

const char *DIPImageName( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    return( ih == NULL ? NULL : ih->dip->dip_name );
}

/*
 * Module Information
 */
size_t DIPModName( mod_handle mh, char *buff, size_t buff_size )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->ModName( IH2IIH( ih ), MH2IMH( mh ), buff, buff_size ) );
}

char *DIPModSrcLang( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( NULL );
    return( ih->dip->ModSrcLang( IH2IIH( ih ), MH2IMH( mh ) ) );
}

dip_status DIPModHasInfo( mod_handle mh, handle_kind hk )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->ModInfo( IH2IIH( ih ), MH2IMH( mh ), hk ) );
}

dip_status DIPModDefault( mod_handle mh, default_kind dk, dig_type_info *ti )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->ModDefault( IH2IIH( ih ), MH2IMH( mh ), dk, ti ) );
}

address DIPModAddr( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( NilAddr );
    return( ih->dip->ModAddr( IH2IIH( ih ), MH2IMH( mh ) ) );
}

/*
 * Type Information
 */
mod_handle DIPTypeMod( type_handle *th )
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    return( MK_MH( ih->ii, ih->dip->TypeMod( IH2IIH( ih ), TH2ITH( th ) ) ) );
}

dip_status DIPTypeInfo( type_handle *th, location_context *lc, dig_type_info *ti )
{
    image_handle        *ih;

    if( th->ap != 0 ) {
        ti->kind = TK_POINTER;
        switch( th->ap & AP_MASK ) {
        case AP_NEAR2:
            ti->modifier = TM_NEAR;
            ti->size = 2;
            break;
        case AP_FAR2:
            ti->modifier = TM_FAR;
            ti->size = 4;
            break;
        case AP_NEAR4:
            ti->modifier = TM_NEAR;
            ti->size = 4;
            break;
        case AP_FAR4:
            ti->modifier = TM_FAR;
            ti->size = 6;
            break;
        }
        ti->deref = false;
        return( DS_OK );
    }
    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->TypeInfo( IH2IIH( ih ), TH2ITH( th ), lc, ti ) );
}

void DIPTypeInit( type_handle *th, mod_handle mh )
{
    th->ii = MH_IMAGE( mh );
    th->ap = 0;
}

dip_status DIPTypePointer( type_handle *base_th, type_modifier tm, unsigned size, type_handle *ptr_th )
{
    if( base_th->ap & AP_FULL )
        return( DS_ERR | DS_TOO_MANY_POINTERS );
    memcpy( ptr_th, base_th, MaxHdlSize[HK_TYPE] );
    ptr_th->ap <<= AP_SHIFT;
    if( tm == TM_NEAR ) {
        if( size == 2 ) {
            ptr_th->ap |= AP_NEAR2;
        } else {
            ptr_th->ap |= AP_NEAR4;
        }
    } else {
        if( size == 4 ) {
            ptr_th->ap |= AP_FAR2;
        } else {
            ptr_th->ap |= AP_FAR4;
        }
    }
    return( DS_OK );
}

dip_status DIPTypeBase( type_handle *th, type_handle *base_th, location_context *lc, location_list *ll )
{
    image_handle        *ih;

    if( th->ap != 0 ) {
        memcpy( base_th, th, MaxHdlSize[HK_TYPE] );
        base_th->ap >>= AP_SHIFT;
        return( DS_OK );
    }
    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    base_th->ii = th->ii;
    base_th->ap = 0;
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->TypeBase( IH2IIH( ih ), TH2ITH( th ), TH2ITH( base_th ), lc, ll ) );
    } else {
        return( ih->dip->OldTypeBase( IH2IIH( ih ), TH2ITH( th ), TH2ITH( base_th ) ) );
    }
}

dip_status DIPTypeAddRef( type_handle *th )
/*****************************************/
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->TypeAddRef( IH2IIH( ih ), TH2ITH( th ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status DIPTypeRelease( type_handle *th )
/******************************************/
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->TypeRelease( IH2IIH( ih ), TH2ITH( th ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status DIPTypeFreeAll( void )
/*******************************/
{
    image_handle        *ih;

    if( ActProc == NULL )
        return( DS_OK );
    for( ih = ActProc->list_ih; ih != NULL; ih = ih->next ) {
        if( ih->dip->minor == DIP_MINOR ) {
            ih->dip->TypeFreeAll( IH2IIH( ih ) );
        }
    }
    return( DS_OK );
}

dip_status DIPTypeArrayInfo( type_handle *th, location_context *lc, array_info *ai, type_handle *index_th )
{
    image_handle        *ih;
    imp_type_handle     *ith;

    if( th->ap != 0 )
        return( DS_ERR | DS_IMPROPER_TYPE );
    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    if( index_th == NULL ) {
        ith = NULL;
    } else {
        index_th->ii = th->ii;
        index_th->ap = 0;
        ith = TH2ITH( index_th );
    }
    return( ih->dip->TypeArrayInfo( IH2IIH( ih ), TH2ITH( th ), lc, ai, ith ) );
}

dip_status DIPTypeProcInfo( type_handle *th, type_handle *parm_th, unsigned num )
{
    image_handle        *ih;

    if( th->ap != 0 )
        return( DS_ERR | DS_IMPROPER_TYPE );
    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    parm_th->ii = th->ii;
    parm_th->ap = 0;
    return( ih->dip->TypeProcInfo( IH2IIH( ih ),
        TH2ITH( th ), TH2ITH( parm_th ), num ) );
}

dip_status DIPTypePtrAddrSpace( type_handle *th, location_context *lc, address *a )
{
    image_handle        *ih;

    if( th->ap != 0 )
        return( DS_FAIL );
    ih = II2IH( th->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->TypePtrAddrSpace( IH2IIH( ih ), TH2ITH( th ), lc, a ) );
}

dip_status DIPTypeThunkAdjust( type_handle *th1, type_handle *th2, location_context *lc, address *a )
{
    image_handle        *ih;

    if( th1->ap != 0 || th2->ap != 0 )
        return( DS_FAIL );
    if( th1->ii != th2->ii )
        return( DS_FAIL );
    ih = II2IH( th1->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->TypeThunkAdjust( IH2IIH( ih ), TH2ITH( th1 ), TH2ITH( th2 ), lc, a ) );
}

int DIPTypeCmp( type_handle *th1, type_handle *th2 )
{
    image_handle        *ih;

    ih = II2IH( th1->ii );
    if( ih == NULL )
        return( 0 );
    if( th1->ii < th2->ii )
        return( -1 );
    if( th1->ii > th2->ii )
        return( 1 );
    return( ih->dip->TypeCmp( IH2IIH( ih ), TH2ITH( th1 ), TH2ITH( th2 ) ) );
}

size_t DIPTypeName( type_handle *th, unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->TypeName( IH2IIH( ih ), TH2ITH( th ), num, tag, buff, buff_size ) );
}

/*
 * Symbol Information
 */
mod_handle DIPSymMod( sym_handle *sh )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    return( MK_MH( ih->ii, ih->dip->SymMod( IH2IIH( ih ), SH2ISH( sh ) ) ) );
}

//NYI: needs to do something for expression names
size_t DIPSymName( sym_handle *sh, location_context *lc, symbol_name_type snt, char *buff, size_t buff_size )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->SymName( IH2IIH( ih ), SH2ISH( sh ), lc, snt, buff, buff_size ) );
}

dip_status DIPSymType( sym_handle *sh, type_handle *th )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    th->ii = sh->ii;
    th->ap = 0;
    return( ih->dip->SymType( IH2IIH( ih ), SH2ISH( sh ), TH2ITH( th ) ) );
}

dip_status DIPSymLocation( sym_handle *sh, location_context *lc, location_list *ll )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->SymLocation( IH2IIH( ih ), SH2ISH( sh ), lc, ll ) );
}

dip_status DIPSymValue( sym_handle *sh, location_context *lc, void *value )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->SymValue( IH2IIH( ih ), SH2ISH( sh ), lc, value ) );
}

dip_status DIPSymInfo( sym_handle *sh, location_context *lc, sym_info *si )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->SymInfo( IH2IIH( ih ), SH2ISH( sh ), lc, si ) );
}

void DIPSymInit( sym_handle *sh, image_handle *ih )
{
    if( ih != NULL ) {
        sh->ii = ih->ii;
    } else {
        sh->ii = NO_IMAGE_IDX;
    }
}

dip_status DIPSymParmLocation( sym_handle *sh, location_context *lc, location_list *ll, unsigned parm )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->SymParmLocation( IH2IIH( ih ), SH2ISH( sh ), lc, ll, parm ) );
}

dip_status DIPSymObjType( sym_handle *sh, type_handle *th, dig_type_info *ti )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    th->ii = sh->ii;
    th->ap = 0;
    return( ih->dip->SymObjType( IH2IIH( ih ), SH2ISH( sh ), TH2ITH( th ), ti ) );
}

dip_status DIPSymObjLocation( sym_handle *sh, location_context *lc, location_list *ll )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    return( ih->dip->SymObjLocation( IH2IIH( ih ), SH2ISH( sh ), lc, ll ) );
}

int DIPSymCmp( sym_handle *sh1, sym_handle *sh2 )
{
    image_handle        *ih;

    ih = II2IH( sh1->ii );
    if( ih == NULL )
        return( 0 );
    if( sh1->ii < sh2->ii )
        return( -1 );
    if( sh1->ii > sh2->ii )
        return( 1 );
    return( ih->dip->SymCmp( IH2IIH( ih ), SH2ISH( sh1 ), SH2ISH( sh2 ) ) );
}

dip_status DIPSymAddRef( sym_handle *sh )
/***************************************/
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->SymAddRef( IH2IIH( ih ), SH2ISH( sh ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status DIPSymRelease( sym_handle *sh )
/****************************************/
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->SymRelease( IH2IIH( ih ), SH2ISH( sh ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status DIPSymFreeAll( void )
/******************************/
{
    image_handle        *ih;

    if( ActProc == NULL )
        return( DS_OK );
    for( ih = ActProc->list_ih; ih != NULL; ih = ih->next ) {
        if( ih->dip->minor == DIP_MINOR ) {
            ih->dip->SymFreeAll( IH2IIH( ih ) );
        }
    }
    return( DS_OK );
}

/*
 * Source Cue Information
 */
mod_handle DIPCueMod( cue_handle *cueh )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    return( MK_MH( ih->ii, ih->dip->CueMod( IH2IIH( ih ), CH2ICH( cueh ) ) ) );
}

size_t DIPCueFile( cue_handle *cueh, char *buff, size_t buff_size )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->CueFile( IH2IIH( ih ), CH2ICH( cueh ), buff, buff_size ) );
}

cue_fileid DIPCueFileId( cue_handle *cueh )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    return( ih->dip->CueFileId( IH2IIH( ih ), CH2ICH( cueh ) ) );
}

dip_status DIPCueAdjust( cue_handle *src_cueh, int adj, cue_handle *dst_cueh )
{
    image_handle        *ih;

    ih = II2IH( src_cueh->ii );
    if( ih == NULL )
        return( DS_ERR | DS_NO_PROCESS );
    dst_cueh->ii = src_cueh->ii;
    return( ih->dip->CueAdjust( IH2IIH( ih ), CH2ICH( src_cueh ), adj, CH2ICH( dst_cueh ) ) );
}

unsigned long DIPCueLine( cue_handle *cueh )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->CueLine( IH2IIH( ih ), CH2ICH( cueh ) ) );
}

unsigned DIPCueColumn( cue_handle *cueh )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    if( ih == NULL )
        return( 0 );
    return( ih->dip->CueColumn( IH2IIH( ih ), CH2ICH( cueh ) ) );
}

address DIPCueAddr( cue_handle *cueh )
{
    image_handle        *ih;

    ih = II2IH( cueh->ii );
    if( ih == NULL )
        return( NilAddr );
    return( ih->dip->CueAddr( IH2IIH( ih ), CH2ICH( cueh ) ) );
}

search_result DIPLineCue( mod_handle mh, cue_fileid id, unsigned long line, unsigned column, cue_handle *cueh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL )
        return( SR_NONE );
    if( MH2IMH( mh ) == IMH_NOMOD ) {
        DIPCli( Status )( DS_ERR | DS_BAD_PARM );
        return( SR_FAIL );
    }
    cueh->ii = ih->ii;
    return( ih->dip->LineCue( IH2IIH( ih ), MH2IMH( mh ), id, line, column, CH2ICH( cueh ) ) );
}

search_result DIPAddrCue( mod_handle mh, address addr, cue_handle *cueh )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( DIPAddrMod( addr, &mh ) == SR_NONE ) {
            return( SR_NONE );
        }
    }
    ih = MH2IH( mh );
    if( ih == NULL )
        return( SR_NONE );
    cueh->ii = ih->ii;
    return( ih->dip->AddrCue( IH2IIH( ih ), MH2IMH( mh ), addr, CH2ICH( cueh ) ) );
}

int DIPCueCmp( cue_handle *cueh1, cue_handle *cueh2 )
{
    image_handle        *ih;

    ih = II2IH( cueh1->ii );
    if( ih == NULL )
        return( 0 );
    if( cueh1->ii < cueh2->ii )
        return( -1 );
    if( cueh1->ii > cueh2->ii )
        return( 1 );
    return( ih->dip->CueCmp( IH2IIH( ih ), CH2ICH( cueh1 ), CH2ICH( cueh2 ) ) );
}

/*
 * Lookup Routines
 */
search_result DIPAddrMod( address a, mod_handle *mh )
{
    image_handle        *ih;
    search_result       sr;
    image_idx           last_found;
    imp_mod_handle      imh;

    if( ActProc != NULL ) {
        /* check the image where the last lookup succeeded first */
        last_found = ActProc->last_addr_mod_found;
        if( last_found != NO_IMAGE_IDX ) {
            ih = ActProc->map_ih[last_found];
            if( ih != NULL ) {
                sr = ih->dip->AddrMod( IH2IIH( ih ), a, &imh );
                if( sr != SR_NONE ) {
                    *mh = MK_MH( last_found, imh );
                    return( sr );
                }
            }
        }
        for( ih = ActProc->list_ih; ih != NULL; ih = ih->next ) {
            if( ih->ii != last_found ) {
                sr = ih->dip->AddrMod( IH2IIH( ih ), a, &imh );
                if( sr != SR_NONE ) {
                    ActProc->last_addr_mod_found = ih->ii;
                    *mh = MK_MH( ih->ii, imh );
                    return( sr );
                }
            }
        }
    }
    *mh = NO_MOD;
    return( SR_NONE );
}

search_result DIPAddrSym( mod_handle mh, address a, sym_handle *sh )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( DIPAddrMod( a, &mh ) == SR_NONE ) {
            return( SR_NONE );
        }
    }
    ih = MH2IH( mh );
    if( ih == NULL )
        return( SR_NONE );
    sh->ii = ih->ii;
    return( ih->dip->AddrSym( IH2IIH( ih ), MH2IMH( mh ), a, SH2ISH( sh ) ) );
}

//NYI: needs to do something for expression names
search_result DIPLookupSymEx( symbol_source ss, void *source, lookup_item *li, location_context *lc, void *d )
{
    image_handle        *ih;
    image_handle        *curr_ih;
    mod_handle          curr_mod;
    mod_handle          save_mod;
    search_result       sr;
    imp_mod_handle      imh;
    type_handle         *th;
    sym_handle          *sh;

    if( ActProc == NULL )
        return( SR_NONE );
    save_mod = li->mod;
    if( save_mod == ILL_MOD )
        return( SR_NONE );
    curr_mod = NO_MOD;
    switch( ss ) {
    case SS_MODULE:
        curr_mod = *(mod_handle *)source;
        imh = MH2IMH( curr_mod );
        source = &imh;
        break;
    case SS_SCOPED:
        DIPAddrMod( *(address *)source, &curr_mod );
        break;
    case SS_TYPE:
        th = (type_handle *)source;
        if( th->ap != 0 )
            return( SR_NONE );
        curr_mod = MK_MH( th->ii, 0 );
        source = TH2ITH( th );
        if( li->mod == NO_MOD )
            li->mod = curr_mod;
        break;
    case SS_BLOCK:
        DIPCli( Status )( DS_ERR | DS_BAD_PARM );
        return( SR_NONE );
    case SS_SCOPESYM:
        sh = (sym_handle *)source;
        curr_mod = MK_MH( sh->ii, 0 );
        source = SH2ISH( sh );
        if( li->mod == NO_MOD )
            li->mod = curr_mod;
        break;
    }
    curr_ih = (curr_mod == NO_MOD) ? NULL : MH2IH( curr_mod );
    ih = (li->mod == NO_MOD) ? curr_ih : MH2IH( li->mod );
    li->mod = IMH2MH( MH2IMH( li->mod ) );
    if( ih != NULL ) {
        if( ih->dip->minor == DIP_MINOR ) {
            sr = ih->dip->LookupSymEx( IH2IIH( ih ), ss, source, li, lc, d );
        } else {
            sr = ih->dip->LookupSym( IH2IIH( ih ), ss, source, li, d );
        }
    } else {
        sr = SR_NONE;
    }
    if( sr == SR_NONE && save_mod == NO_MOD && ss == SS_SCOPED ) {
        curr_ih = ih;
        for( ih = ActProc->list_ih; ih != NULL; ih = ih->next ) {
            if( ih != curr_ih ) {
                if( ih->dip->minor == DIP_MINOR ) {
                    sr = ih->dip->LookupSymEx( IH2IIH( ih ), ss, source, li, lc, d );
                } else {
                    sr = ih->dip->LookupSym( IH2IIH( ih ), ss, source, li, d );
                }
                if( sr != SR_NONE ) {
                    break;
                }
            }
        }
    }
    li->mod = save_mod;
    return( sr );
}

search_result DIPLookupSym( symbol_source ss, void *source, lookup_item *li, void *d )
{
    return( DIPLookupSymEx( ss, source, li, NULL, d ) );
}

search_result DIPAddrScope( mod_handle mh, address addr, scope_block *scope )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( DIPAddrMod( addr, &mh ) == SR_NONE ) {
            return( SR_NONE );
        }
    }
    ih = MH2IH( mh );
    if( ih == NULL )
        return( SR_NONE );
    return( ih->dip->AddrScope( IH2IIH( ih ), MH2IMH( mh ), addr, scope ) );
}

search_result DIPScopeOuter( mod_handle mh, scope_block *in, scope_block *out )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( DIPAddrMod( in->start, &mh ) == SR_NONE ) {
            return( SR_NONE );
        }
    }
    ih = MH2IH( mh );
    if( ih == NULL )
        return( SR_NONE );
    return( ih->dip->ScopeOuter( IH2IIH( ih ), MH2IMH( mh ), in, out ) );
}
