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
* Description:  Debug Information Processor (DIP) manager.
*
****************************************************************************/


#include <string.h>
#include <limits.h>
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

typedef unsigned_16     image_idx;

struct process_info {
    unsigned            last_addr_mod_found;
    unsigned            map_entries;
    image_handle        **ih_map;
    image_handle        *ih_list;
    image_handle        **ih_add;
};

struct image_handle {
    dip_imp_routines    *dip;
    image_handle        *next;
    void                *extra;
    unsigned            index;
};

enum artificial_pointers {
    AP_NEAR2 = 1,
    AP_FAR2,
    AP_NEAR4,
    AP_FAR4,
    AP_MASK = 0x7,
    AP_SHIFT = 3,
    AP_FULL  = 0xe000
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

#define CLI_HDL( hp, type )     (((type##_handle *)(hp))-1)
#define IMP_HDL( hp, type )     ((imp_##type##_handle *)((hp) + 1))

#define MK_MH( ii, sm )         ((unsigned_32)((ii)+1) << 16 | (sm))
#define MH_IMAGE( mh )          ((unsigned)((mh)>>16)-1)
#define MH_SMOD( mh )           ((unsigned)((mh)&0xffff))
#define II2IH( ii )             ((ActProc==NULL)||(ActProc->map_entries<=ii)?NULL:ActProc->ih_map[ii])
#define MH2IH( mh )             (((mh&0xffff0000)==0)?NULL:II2IH(MH_IMAGE(mh)))

#define NO_IMAGE_IDX    ((unsigned)-1)

static unsigned         LoadingImageIdx = NO_IMAGE_IDX;

static process_info     *ActProc;
static struct {
    dip_imp_routines    *rtns;
    dip_sys_handle      sys_hdl;
}                       LoadedDIPs[MAX_DIPS];
static unsigned         MaxHdlSize[HK_LAST];

static const unsigned_8 MgrHdlOverhead[] = {
    sizeof( image_handle ),
    sizeof( type_handle ),
    sizeof( cue_handle ),
    sizeof( sym_handle )
};


char DIPDefaults[] = {
    "dwarf\0"
    "watcom\0"
    "codeview\0"
    "mapsym\0"
    "export\0"
    "\0"
};

static const address    NilAddr = { 0 };

/*
 * Client interface
 */

/*
 * DCSymCreate is here so that DIPRegister'd functions can call it.
 */
imp_sym_handle *DCSymCreate( imp_image_handle *ii, void *d )
{
    sym_handle  *sh;

    sh = DIPCliSymCreate( d );
    sh->ii = CLI_HDL( ii, image )->index;
    return( IMP_HDL( sh, sym ) );
}

imp_sym_handle * DIGCLIENT XDIPCliSymCreate( imp_image_handle *ii, void *d )
{
    return( DCSymCreate( ii, d ) );
}

dip_client_routines DIPClientInterface = {
    DIP_MAJOR,
    DIP_MINOR_OLD,
    sizeof( dip_client_routines ),
    DIGCliAlloc,
    DIGCliRealloc,
    DIGCliFree,
    DIPCliMapAddr,
    XDIPCliSymCreate,
    DIPCliItemLocation,
    DIPCliAssignLocation,
    DIPCliSameAddrSpace,
    DIPCliAddrSection,
    DIGCliOpen,
    DIGCliSeek,
    DIGCliRead,
    DIGCliWrite,
    DIGCliClose,
    DIGCliRemove,
    DIPCliStatus,
    DIPCliCurrMAD,
    DIGCliMachineData
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

    for( hk = 0; hk < (sizeof(MaxHdlSize) / sizeof(MaxHdlSize[0])); ++hk ) {
        size = rtns->handle_size( hk );
        if( size > MaxHdlSize[hk] ) MaxHdlSize[hk] = size;
    }
}

dip_status DIPLoad( char *path )
{
    unsigned    i;
    dip_status  status;

    for( i = 0; LoadedDIPs[i].rtns != NULL; ++i ) {
        if( i >= MAX_LOAD_DIPS ) {
            return( DS_ERR|DS_TOO_MANY_DIPS );
        }
    }
    status = DIPSysLoad( path, &DIPClientInterface, &LoadedDIPs[i].rtns, &LoadedDIPs[i].sys_hdl );
    if( status != DS_OK ) return( status );
    if( DIPClientInterface.major != LoadedDIPs[i].rtns->major
     || DIPClientInterface.minor > LoadedDIPs[i].rtns->minor ) {
        DIPSysUnload( LoadedDIPs[i].sys_hdl );
        LoadedDIPs[i].rtns = NULL;
        return( DS_ERR|DS_INVALID_DIP_VERSION );
    }
    SetHdlSizes( LoadedDIPs[i].rtns );
    return( DS_OK );
}

dip_status DIPRegister( dip_imp_routines *dir )
{
    unsigned    i;


    for( i = MAX_LOAD_DIPS; LoadedDIPs[i].rtns != NULL; ++i ) {
        if( i >= MAX_DIPS ) {
            return( DS_ERR|DS_TOO_MANY_DIPS );
        }
    }
    LoadedDIPs[i].rtns = dir;
    LoadedDIPs[i].sys_hdl = 0;
    SetHdlSizes( LoadedDIPs[i].rtns );
    return( DS_OK );
}

void DIPFiniLatest( void )
{
    unsigned    i;

    i = MAX_DIPS;
    for( ;; ) {
        --i;
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->shutdown();
            if( LoadedDIPs[i].sys_hdl != 0 ) {
                DIPSysUnload( LoadedDIPs[i].sys_hdl );
            }
            LoadedDIPs[i].rtns = NULL;
            LoadedDIPs[i].sys_hdl = 0;
            return;
        }
        if( i == 0 ) return;
    }
}

void DIPFini( void )
{
    unsigned    i;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->shutdown();
            if( LoadedDIPs[i].sys_hdl != 0 ) {
                DIPSysUnload( LoadedDIPs[i].sys_hdl );
            }
        }
        LoadedDIPs[i].rtns = NULL;
        LoadedDIPs[i].sys_hdl = 0;
    }
}

unsigned DIPHandleSize( handle_kind h )
{
    return( MaxHdlSize[h] + MgrHdlOverhead[h] );
}

dip_status DIPMoreMem( unsigned amount )
{
    unsigned    i;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            if( LoadedDIPs[i].rtns->more_mem( amount ) == DS_OK ) return( DS_OK );
        }
    }
    return( DS_FAIL );
}

void DIPCancel( void )
{
    unsigned            i;
    image_handle        *ih;

    for( i = 0; i < MAX_DIPS; ++i ) {
        if( LoadedDIPs[i].rtns != NULL ) {
            LoadedDIPs[i].rtns->cancel();
        }
    }
    if( LoadingImageIdx != NO_IMAGE_IDX ) {
        ih = II2IH( LoadingImageIdx );
        if( ih != NULL ) {
            ih->dip->unload_info( IMP_HDL( ih, image ) );
        }
        LoadingImageIdx = NO_IMAGE_IDX;
    }
}

unsigned FindImageMapSlot( process_info *p )
{
    image_handle        **new;
    unsigned            new_num;
    unsigned            i;
    unsigned            j;

    for( i = 0; i < p->map_entries; ++i ) {
        if( p->ih_map[i] == NULL ) return( i );
    }
    new_num = p->map_entries + IMAGE_MAP_GROW;
    new = DIGCliRealloc( p->ih_map, new_num * sizeof( p->ih_map[0] ) );
    if( new == NULL ) {
        DIPCliStatus( DS_ERR | DS_NO_MEM );
        return( NO_IMAGE_IDX );
    }
    i = p->map_entries;
    p->map_entries = new_num;
    p->ih_map = new;
    for( j = i; j < new_num; ++j ) new[j] = NULL;
    return( i );
}

static void DIPCleanupInfo( process_info *p, image_handle *ih )
{
    image_handle        **owner;
    image_handle        *curr;

    DIPCliImageUnload( MK_MH( ih->index, 0 ) );
    ih->dip->unload_info( IMP_HDL( ih, image ) );
    p->ih_map[ih->index] = NULL;
    owner = &p->ih_list;
    for( ;; ) {
        curr = *owner;
        if( curr == ih ) break;
        owner = &curr->next;
    }
    *owner = ih->next;
    if( p->ih_add == &ih->next ) {
        p->ih_add = owner;
    }
    if( ih->index == p->last_addr_mod_found ) {
        p->last_addr_mod_found = NO_IMAGE_IDX;
    }
    DIGCliFree( ih );
}

static void CleanupProcess( process_info *p, int unload )
{
    image_handle        *ih;
    image_handle        *next;

    if( unload ) {
        while( p->ih_list ) {
            DIPCleanupInfo( p, p->ih_list );
        }
    } else {
        for( ih = p->ih_list; ih != NULL; ih = next ) {
            next = ih->next;
            DIGCliFree( ih );
        }
    }
    DIGCliFree( p->ih_map );
    DIGCliFree( p );
}

process_info *DIPCreateProcess( void )
{
    process_info        *p;
    image_handle        *ih;
    unsigned            i;
    unsigned            j;

    p = DIGCliAlloc( sizeof( process_info ) );
    if( p == NULL ) {
        DIPCliStatus( DS_ERR|DS_NO_MEM );
        return( NULL );
    }
    p->last_addr_mod_found = NO_IMAGE_IDX;
    p->ih_map = DIGCliAlloc( IMAGE_MAP_INIT * sizeof( p->ih_map[0] ) );
    if( p->ih_map == NULL ) {
        DIGCliFree( p );
        DIPCliStatus( DS_ERR|DS_NO_MEM );
        return( NULL );
    }
    p->map_entries = IMAGE_MAP_INIT;
    for( i = 0; i < IMAGE_MAP_INIT; ++i ) p->ih_map[i] = NULL;
    p->ih_list = NULL;
    p->ih_add = &p->ih_list;
    for( j = MAX_DIPS-1; j >= MAX_LOAD_DIPS; --j ) {
        if( LoadedDIPs[j].rtns != NULL ) {
            i = FindImageMapSlot( p );
            if( i == NO_IMAGE_IDX ) {
                CleanupProcess( p, 0 );
                DIPCliStatus( DS_ERR|DS_NO_MEM );
                return( NULL );
            }
            ih = DIGCliAlloc( sizeof( image_handle ) );
            if( ih == NULL ) {
                CleanupProcess( p, 0 );
                DIPCliStatus( DS_ERR|DS_NO_MEM );
                return( NULL );
            }
            p->ih_map[i] = ih;
            ih->next = *p->ih_add;
            *p->ih_add = ih;
            p->ih_add = &ih->next;
            ih->dip = LoadedDIPs[j].rtns;
            ih->extra = NULL;
            ih->index = i;
        }
    }
    p->ih_add = &p->ih_list;
    if( ActProc == NULL ) ActProc = p;
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
    if( p == ActProc ) ActProc = NULL;
}

unsigned DIPPriority( unsigned old )
{
    unsigned    prio;
    unsigned    curr;
    unsigned    j;

    prio = UINT_MAX;
    for( j = 0; j < MAX_DIPS; ++j ) {
        if( LoadedDIPs[j].rtns == NULL ) continue;
        curr = LoadedDIPs[j].rtns->dip_priority;
        if( curr <= old ) continue;
        if( curr >= prio ) continue;
        prio = curr;
    }
    return( prio == UINT_MAX ? 0 : prio );
}

mod_handle DIPLoadInfo( dig_fhandle file, unsigned extra, unsigned prio )
{
    unsigned            i;
    unsigned            j;
    image_handle        *ih;
    dip_status          ret;

    if( ActProc == NULL ) {
        DIPCliStatus( DS_ERR|DS_NO_PROCESS );
        return( NO_MOD );
    }
    i = FindImageMapSlot( ActProc );
    if( i == NO_IMAGE_IDX ) return( NO_MOD );
    ih = DIGCliAlloc( DIPHandleSize( HK_IMAGE ) + extra );
    if( ih == NULL ) {
        DIPCliStatus( DS_ERR|DS_NO_MEM );
        return( NO_MOD );
    }
    for( j = 0; j < MAX_DIPS; ++j ) {
        if( LoadedDIPs[j].rtns == NULL ) continue;
        if( LoadedDIPs[j].rtns->dip_priority != prio ) continue;
        ret = LoadedDIPs[j].rtns->load_info(file,IMP_HDL(ih,image));
        if( ret == DS_OK ) {
            ActProc->ih_map[i] = ih;
            ih->next = *ActProc->ih_add;
            *ActProc->ih_add = ih;
            ActProc->ih_add = &ih->next;
            ih->dip = LoadedDIPs[j].rtns;
            ih->extra = (unsigned_8 *)ih + DIPHandleSize( HK_IMAGE );
            ih->index = i;
            LoadingImageIdx = i;
            return( MK_MH( i, 0 ) );
        }
        if( ret & DS_ERR ) break;
    }
    DIGCliFree( ih );
    return( NO_MOD );
}

void DIPMapInfo( mod_handle mh, void *d )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih != NULL ) {
        ih->dip->map_info( IMP_HDL( ih, image ), d );
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


unsigned DIPImagePriority( mod_handle mh )
{
    image_handle        *ih;

    if( mh == NO_MOD ) return( DP_MAX+1 );
    ih = MH2IH( mh );
    if( ih == NULL ) return( DP_MAX+1 );
    return( ih->dip->dip_priority );
}

/*
 *      Information Walkers
 */
walk_result WalkImageList( IMAGE_WALKER *iw, void *d )
{
    walk_result         wr;
    image_handle        *ih;

    wr = WR_CONTINUE;
    if( ActProc != NULL ) {
        for( ih = ActProc->ih_list; ih != NULL; ih = ih->next ) {
            wr = iw( MK_MH( ih->index, 0 ), d );
            if( wr != WR_CONTINUE ) break;
        }
    }
    return( wr );
}

typedef struct {
    union {
        MOD_WALKER      *m;
        TYPE_WALKER     *t;
        SYM_WALKER      *s;
        CUE_WALKER      *c;
    }           walk;
    void                *d;
    location_context    *lc;
} walk_glue;

static walk_result DIGCLIENT ModGlue( imp_image_handle *ii, imp_mod_handle im,
                                        void *d )
{
    walk_glue           *wd = d;
    image_handle        *ih;

    ih = CLI_HDL( ii, image );
    return( wd->walk.m( MK_MH( ih->index, im ), wd->d ) );
}

static walk_result WalkOneModList( mod_handle mh, void *d )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( WR_STOP );
    return( ih->dip->walk_mod_list( IMP_HDL( ih, image ), ModGlue, d ) );
}

walk_result WalkModList( mod_handle mh, MOD_WALKER *mw, void *d )
{
    walk_glue           glue;

    if( ActProc == NULL ) {
        DIPCliStatus( DS_ERR|DS_NO_PROCESS );
        return( WR_FAIL );
    }
    glue.walk.m = mw;
    glue.d = d;
    glue.lc = NULL;
    if( mh == NO_MOD ) {
        return( WalkImageList( WalkOneModList, &glue ) );
    } else {
        return( WalkOneModList( mh, &glue ) );
    }
}

static walk_result DIGCLIENT TypeGlue( imp_image_handle *ii,
                                imp_type_handle *it, void *d )
{
    walk_glue   *wd = d;

    ii = ii;
    return( wd->walk.t( CLI_HDL( it, type ), wd->d ) );
}

walk_result WalkTypeList( mod_handle mh, TYPE_WALKER *tw, void *d )
{
    image_handle        *ih;
    walk_glue           glue;
    type_handle         *th = __alloca( DIPHandleSize( HK_TYPE ) );
    walk_result         wr;

    wr = WR_CONTINUE;
    ih = MH2IH( mh );
    if( ih != NULL ) {
        glue.walk.t = tw;
        glue.d = d;
        glue.lc = NULL;
        th->ii = MH_IMAGE( mh );
        th->ap = 0;
        wr = ih->dip->walk_type_list( IMP_HDL( ih, image ), MH_SMOD( mh ),
                                TypeGlue, IMP_HDL( th, type ), &glue );
    }
    return( wr );
}

static walk_result DIGCLIENT SymGlue( imp_image_handle *ii,
                            sym_walk_info swi, imp_sym_handle *is, void *d )
{
    walk_glue   *wd = d;
    sym_handle  *sh;

    ii = ii;
    sh = (is == NULL) ? NULL: CLI_HDL( is, sym );
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
    sym_handle          *sh = __alloca( DIPHandleSize( HK_SYM ) );
    image_idx           ii = 0;
    imp_mod_handle      im;
    mod_handle          mh;
    type_handle         *it;
    sym_handle          *is;
    walk_result         wr;

    switch( ss ) {
    case SS_MODULE:
        mh = *(mod_handle *)start;
        if( mh == NO_MOD ) {
            return( WalkImageList( GblSymWalk, wd ) );
        }
        ii = MH_IMAGE( mh );
        im = MH_SMOD( mh );
        start = &im;
        break;
    case SS_SCOPED:
        if( AddrMod( *(address *)start, &mh ) == SR_NONE ) {
             return( WR_CONTINUE );
        }
        ii = MH_IMAGE( mh );
        break;
    case SS_TYPE:
        it = start;
        if( it->ap != 0 ) return( WR_CONTINUE );
        start = IMP_HDL( it, type );
        ii = it->ii;
        break;
    case SS_BLOCK:
        if( AddrMod( ((scope_block *)start)->start, &mh ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        ii = MH_IMAGE( mh );
        break;
    case SS_SCOPESYM:
        is = start;
        start = IMP_HDL( is, sym );
        ii = is->ii;
        break;
    }
    wr = WR_CONTINUE;
    ih = II2IH( ii );
    if( ih != NULL ) {
        sh->ii = ii;
        if( ih->dip->minor == DIP_MINOR ) {
            wr = ih->dip->walk_sym_list_ex( IMP_HDL( ih, image ), ss, start,
                                    SymGlue, IMP_HDL( sh, sym ), wd->lc, wd );
        } else {
            wr = ih->dip->walk_sym_list( IMP_HDL( ih, image ), ss, start,
                                    SymGlue, IMP_HDL( sh, sym ), wd );
        }
    }
    return( wr );
}

walk_result WalkSymListEx( symbol_source ss, void *start, SYM_WALKER *sw,
                           location_context *lc, void *d )
{
    walk_glue   glue;

    glue.walk.s = sw;
    glue.d = d;
    glue.lc = lc;
    return( DoWalkSymList( ss, start, &glue ) );
}

walk_result WalkSymList( symbol_source ss, void *start, SYM_WALKER *sw, void *d )
{
    return( WalkSymListEx( ss, start, sw, NULL, d ) );
}

static walk_result DIGCLIENT CueGlue( imp_image_handle *ii,
                                imp_cue_handle *ic, void *d )
{
    walk_glue   *wd = d;

    ii = ii;
    return( wd->walk.c( CLI_HDL( ic, cue ), wd->d ) );
}

walk_result WalkFileList( mod_handle mh, CUE_WALKER *cw, void *d )
{
    image_handle        *ih;
    walk_glue           glue;
    cue_handle          *ch = __alloca( DIPHandleSize( HK_CUE ) );

    ih = MH2IH( mh );
    if( ih == NULL ) return( WR_CONTINUE );
    glue.walk.c = cw;
    glue.d = d;
    glue.lc = NULL;
    ch->ii = MH_IMAGE( mh );
    return( ih->dip->walk_file_list( IMP_HDL( ih, image ), MH_SMOD( mh ),
                CueGlue, IMP_HDL( ch, cue ), &glue ) );
}

/*
 * Image Information
 */
void *ImageExtra( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    return( ih == NULL ? NULL : ih->extra );
}

const char *ImageDIP( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    return( ih == NULL ? NULL : ih->dip->dip_name );
}

/*
 * Module Information
 */
unsigned ModName( mod_handle mh, char *result, unsigned max )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( 0 );
    return( ih->dip->mod_name( IMP_HDL( ih, image ), MH_SMOD( mh ),
                        result, max ) );
}

char *ModSrcLang( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( NULL );
    return( ih->dip->mod_src_lang( IMP_HDL( ih, image ),
                MH_SMOD( mh ) ) );
}

dip_status ModHasInfo( mod_handle mh, handle_kind hk )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->mod_info( IMP_HDL( ih, image ),
                MH_SMOD( mh ), hk ) );
}

dip_status ModDefault( mod_handle mh, default_kind dk, dip_type_info *ti )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->mod_default( IMP_HDL( ih, image ),
                MH_SMOD( mh ), dk, ti ) );
}

address ModAddr( mod_handle mh )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( NilAddr );
    return( ih->dip->mod_addr( IMP_HDL( ih, image ),
                MH_SMOD( mh ) ) );
}

/*
 * Type Information
 */
mod_handle TypeMod( type_handle *th )
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    return( MK_MH( ih->index, ih->dip->type_mod( IMP_HDL( ih, image ),
                        IMP_HDL( th, type ) ) ) );
}

dip_status TypeInfo( type_handle *th, location_context *lc, dip_type_info *ti )
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
        return( DS_OK );
    }
    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->tipe_info( IMP_HDL( ih, image ),
                                IMP_HDL( th, type ), lc, ti ) );
}

void *TypeCreate( type_handle *th, mod_handle mh )
{
    th->ii = MH_IMAGE( mh );
    th->ap = 0;
    return( IMP_HDL( th, type ) );
}

dip_status TypePointer( type_handle *base_th, type_modifier tm, unsigned size,
                        type_handle *ptr_th )
{
    if( base_th->ap & AP_FULL ) return( DS_ERR|DS_TOO_MANY_POINTERS );
    memcpy( ptr_th, base_th, DIPHandleSize( HK_TYPE ) );
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

dip_status TypeBase( type_handle *th, type_handle *base_th, location_context *lc, location_list *ll )
{
    image_handle        *ih;

    if( th->ap != 0 ) {
        memcpy( base_th, th, DIPHandleSize( HK_TYPE ) );
        base_th->ap >>= AP_SHIFT;
        return( DS_OK );
    }
    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    base_th->ii = th->ii;
    base_th->ap = 0;
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->type_base( IMP_HDL( ih, image ),
                IMP_HDL( th, type ), IMP_HDL( base_th, type ), lc, ll ) );
    } else {
        return( ih->dip->old_type_base( IMP_HDL( ih, image ),
                IMP_HDL( th, type ), IMP_HDL( base_th, type ) ) );
    }
}

dip_status TypeAddRef( type_handle *th )
/**************************************/
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->type_addref( IMP_HDL( ih, image ),
                IMP_HDL( th, type ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status TypeRelease( type_handle *th )
/**************************************/
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->type_release( IMP_HDL( ih, image ),
                IMP_HDL( th, type ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status TypeFreeAll( void )
/****************************/
{
    image_handle        *ih;

    if( ActProc == NULL ) return( DS_OK );
    for( ih = ActProc->ih_list; ih != NULL; ih = ih->next ) {
        if( ih->dip->minor == DIP_MINOR ) {
            ih->dip->type_freeall( IMP_HDL( ih, image ) );
        }
    }
    return( DS_OK );
}

dip_status TypeArrayInfo( type_handle *th, location_context *lc,
                        array_info *ai, type_handle *index_th )
{
    image_handle        *ih;
    imp_type_handle     *it;

    if( th->ap != 0 ) return( DS_ERR|DS_IMPROPER_TYPE );
    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    if( index_th == NULL ) {
        it = NULL;
    } else {
        index_th->ii = th->ii;
        index_th->ap = 0;
        it = IMP_HDL( index_th, type );
    }
    return( ih->dip->type_array_info( IMP_HDL( ih, image ),
            IMP_HDL( th, type ), lc, ai, it ) );
}

dip_status TypeProcInfo( type_handle *th, type_handle *parm_th, unsigned num )
{
    image_handle        *ih;

    if( th->ap != 0 ) return( DS_ERR|DS_IMPROPER_TYPE );
    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    parm_th->ii = th->ii;
    parm_th->ap = 0;
    return( ih->dip->type_proc_info( IMP_HDL( ih, image ),
        IMP_HDL(th, type), IMP_HDL( parm_th, type ), num ) );
}

dip_status TypePtrAddrSpace( type_handle *th, location_context *lc, address *a )
{
    image_handle        *ih;

    if( th->ap != 0 ) return( DS_FAIL );
    ih = II2IH( th->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->type_ptr_addr_space( IMP_HDL( ih, image ),
        IMP_HDL(th, type), lc, a ) );
}

dip_status TypeThunkAdjust( type_handle *oth, type_handle *mth,
                        location_context *lc, address *a )
{
    image_handle        *ih;

    if( oth->ap != 0 || mth->ap != 0 ) return( DS_FAIL );
    if( oth->ii != mth->ii ) return( DS_FAIL );
    ih = II2IH( oth->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->type_thunk_adjust( IMP_HDL( ih, image ),
        IMP_HDL( oth, type ), IMP_HDL( mth, type ), lc, a ) );
}

int TypeCmp( type_handle *th1, type_handle *th2 )
{
    image_handle        *ih;

    ih = II2IH( th1->ii );
    if( ih == NULL ) return( 0 );
    if( th1->ii != th2->ii ) return( th1->ii - th2->ii );
    return( ih->dip->type_cmp( IMP_HDL( ih, image ),
        IMP_HDL( th1, type ), IMP_HDL( th2, type ) ) );
}

unsigned TypeName( type_handle *th, unsigned num, symbol_type *tag,
                        char *buff, unsigned max )
{
    image_handle        *ih;

    ih = II2IH( th->ii );
    if( ih == NULL ) return( 0 );
    return( ih->dip->type_name( IMP_HDL( ih, image ),
                IMP_HDL( th, type ), num, tag, buff, max ) );
}

/*
 * Symbol Information
 */
mod_handle SymMod( sym_handle *sh )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    return( MK_MH( ih->index, ih->dip->sym_mod( IMP_HDL( ih, image ),
                        IMP_HDL( sh, sym ) ) ) );
}

//NYI: needs to do something for expression names
unsigned SymName( sym_handle *sh, location_context *lc, symbol_name sn,
                        char *name, unsigned max )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( 0 );
    return( ih->dip->sym_name( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, sn, name, max ) );
}

dip_status SymType( sym_handle *sh, type_handle *th )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    th->ii = sh->ii;
    th->ap = 0;
    return( ih->dip->sym_type( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), IMP_HDL( th, type ) ) );
}

dip_status SymLocation( sym_handle *sh, location_context *lc, location_list *ll )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->sym_location( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, ll ) );
}

dip_status SymValue( sym_handle *sh, location_context *lc, void *value )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->sym_value( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, value ) );
}

dip_status SymInfo( sym_handle *sh, location_context *lc, sym_info *si )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->sym_info( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, si ) );
}

dip_status SymParmLocation( sym_handle *sh, location_context *lc,
                        location_list *ll, unsigned parm )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->sym_parm_location( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, ll, parm ) );
}

dip_status SymObjType( sym_handle *sh, type_handle *th, dip_type_info *ti )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    th->ii = sh->ii;
    th->ap = 0;
    return( ih->dip->sym_obj_type( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), IMP_HDL( th, type ), ti ) );
}

dip_status SymObjLocation( sym_handle *sh, location_context *lc,
                        location_list *ll )
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    return( ih->dip->sym_obj_location( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ), lc, ll ) );
}

int SymCmp( sym_handle *sh1, sym_handle *sh2 )
{
    image_handle        *ih;

    ih = II2IH( sh1->ii );
    if( ih == NULL ) return( 0 );
    if( sh1->ii != sh2->ii ) return( sh1->ii - sh2->ii );
    return( ih->dip->sym_cmp( IMP_HDL( ih, image ),
        IMP_HDL( sh1, sym ), IMP_HDL( sh2, sym ) ) );
}

dip_status SymAddRef( sym_handle *sh )
/**************************************/
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->sym_addref( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status SymRelease( sym_handle *sh )
/**************************************/
{
    image_handle        *ih;

    ih = II2IH( sh->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    if( ih->dip->minor == DIP_MINOR ) {
        return( ih->dip->sym_release( IMP_HDL( ih, image ),
                IMP_HDL( sh, sym ) ) );
    } else {
        return( DS_OK );
    }
}

dip_status SymFreeAll( void )
/***************************/
{
    image_handle        *ih;

    if( ActProc == NULL ) return( DS_OK );
    for( ih = ActProc->ih_list; ih != NULL; ih = ih->next ) {
        if( ih->dip->minor == DIP_MINOR ) {
            ih->dip->sym_freeall( IMP_HDL( ih, image ) );
        }
    }
    return( DS_OK );
}

/*
 * Source Cue Information
 */
mod_handle CueMod( cue_handle *ch )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    return( MK_MH( ih->index, ih->dip->cue_mod( IMP_HDL( ih, image ),
                        IMP_HDL( ch, cue ) ) ) );
}

unsigned CueFile( cue_handle *ch, char *file, unsigned max )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    if( ih == NULL ) return( 0 );
    return( ih->dip->cue_file( IMP_HDL( ih, image ),
                IMP_HDL( ch, cue ), file, max ) );
}

cue_file_id CueFileId( cue_handle *ch )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    return( ih->dip->cue_fyle_id( IMP_HDL( ih, image ),
                        IMP_HDL( ch, cue ) ) );
}

dip_status CueAdjust( cue_handle *ch, int adj, cue_handle *ach )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    if( ih == NULL ) return( DS_ERR|DS_NO_PROCESS );
    ach->ii = ch->ii;
    return( ih->dip->cue_adjust( IMP_HDL( ih, image ),
            IMP_HDL( ch, cue ), adj, IMP_HDL( ach, cue ) ) );
}

unsigned long CueLine( cue_handle *ch )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    if( ih == NULL ) return( 0 );
    return( ih->dip->cue_line( IMP_HDL( ih, image ),
                IMP_HDL( ch, cue ) ) );
}

unsigned CueColumn( cue_handle *ch )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    if( ih == NULL ) return( 0 );
    return( ih->dip->cue_column( IMP_HDL( ih, image ),
                IMP_HDL( ch, cue ) ) );
}

address CueAddr( cue_handle *ch )
{
    image_handle        *ih;

    ih = II2IH( ch->ii );
    if( ih == NULL ) return( NilAddr );
    return( ih->dip->cue_addr( IMP_HDL( ih, image ),
                IMP_HDL( ch, cue ) ) );
}

search_result LineCue( mod_handle mh, cue_file_id id, unsigned long line,
                        unsigned column, cue_handle *ch )
{
    image_handle        *ih;

    ih = MH2IH( mh );
    if( ih == NULL ) return( SR_NONE );
    if( MH_SMOD( mh ) == (imp_mod_handle)NO_MOD ) {
        DIPCliStatus( DS_ERR|DS_BAD_PARM );
        return( SR_FAIL );
    }
    ch->ii = ih->index;
    return( ih->dip->line_cue( IMP_HDL( ih, image ),
                MH_SMOD( mh ), id, line, column,
                IMP_HDL( ch, cue ) ) );
}

search_result AddrCue( mod_handle mh, address addr, cue_handle *ch )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( AddrMod( addr, &mh ) == SR_NONE ) return( SR_NONE );
    }
    ih = MH2IH( mh );
    if( ih == NULL ) return( SR_NONE );
    ch->ii = ih->index;
    return( ih->dip->addr_cue( IMP_HDL( ih, image ),
                MH_SMOD( mh ), addr, IMP_HDL( ch, cue ) ) );
}

int CueCmp( cue_handle *ch1, cue_handle *ch2 )
{
    image_handle        *ih;

    ih = II2IH( ch1->ii );
    if( ih == NULL ) return( 0 );
    if( ch1->ii != ch2->ii ) return( ch1->ii - ch2->ii );
    return( ih->dip->cue_cmp( IMP_HDL( ih, image ),
        IMP_HDL( ch1, cue ), IMP_HDL( ch2, cue ) ) );
}

/*
 * Lookup Routines
 */
search_result AddrMod( address a, mod_handle *mh )
{
    image_handle        *ih;
    search_result       sr;
    unsigned            last_found;
    imp_mod_handle      im;

    if( ActProc != NULL ) {
        /* check the image where the last lookup succeeded first */
        last_found = ActProc->last_addr_mod_found;
        if( last_found != NO_IMAGE_IDX ) {
            ih = ActProc->ih_map[last_found];
            if( ih != NULL ) {
                sr = ih->dip->addr_mod( IMP_HDL( ih, image ), a, &im );
                if( sr != SR_NONE ) {
                    *mh = MK_MH( last_found, im );
                    return( sr );
                }
            }
        }
        for( ih = ActProc->ih_list; ih != NULL; ih = ih->next ) {
            if( ih->index != last_found ) {
                sr = ih->dip->addr_mod( IMP_HDL( ih, image ), a, &im );
                if( sr != SR_NONE ) {
                    ActProc->last_addr_mod_found = ih->index;
                    *mh = MK_MH( ih->index, im );
                    return( sr );
                }
            }
        }
    }
    *mh = NO_MOD;
    return( SR_NONE );
}

search_result AddrSym( mod_handle mh, address a, sym_handle *sh )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( AddrMod( a, &mh ) == SR_NONE ) return( SR_NONE );
    }
    ih = MH2IH( mh );
    if( ih == NULL ) return( SR_NONE );
    sh->ii = ih->index;
    return( ih->dip->addr_sym( IMP_HDL( ih, image ),
            MH_SMOD( mh ), a, IMP_HDL( sh, sym ) ) );
}

//NYI: needs to do something for expression names
search_result LookupSymEx( symbol_source ss, void *source,
                        lookup_item *li, location_context *lc, void *d )
{
    image_handle        *ih;
    image_handle        *cih;
    mod_handle          curr_mod;
    mod_handle          save_mod;
    search_result       sr;
    imp_mod_handle      im;
    type_handle         *th;
    sym_handle          *sh;

    if( ActProc == NULL ) return( SR_NONE );
    save_mod = li->mod;
    if( save_mod == ILL_MOD ) return( SR_NONE );
    switch( ss ) {
    case SS_MODULE:
        curr_mod = *(mod_handle *)source;
        im = MH_SMOD( curr_mod );
        source = &im;
        break;
    case SS_SCOPED:
        AddrMod( *(address *)source, &curr_mod );
        break;
    case SS_TYPE:
        th = (type_handle *)source;
        if( th->ap != 0 ) return( SR_NONE );
        curr_mod = MK_MH( th->ii, 0 );
        source = IMP_HDL( th, type );
        if( li->mod == NO_MOD ) li->mod = curr_mod;
        break;
    case SS_BLOCK:
        DIPCliStatus( DS_ERR|DS_BAD_PARM );
        return( SR_NONE );
    case SS_SCOPESYM:
        sh = (sym_handle *)source;
        curr_mod = MK_MH( sh->ii, 0 );
        source = IMP_HDL( sh, sym );
        if( li->mod == NO_MOD ) li->mod = curr_mod;
        break;
    }
    cih = (curr_mod == NO_MOD) ? NULL : MH2IH( curr_mod );
    ih = (li->mod == NO_MOD) ? cih : MH2IH( li->mod );
    li->mod = MH_SMOD( li->mod );
    if( ih != NULL ) {
        if( ih->dip->minor == DIP_MINOR ) {
            sr = ih->dip->lookup_sym_ex( IMP_HDL( ih, image ), ss, source, li, lc, d );
        } else {
            sr = ih->dip->lookup_sym( IMP_HDL( ih, image ), ss, source, li, d );
        }
    } else {
        sr = SR_NONE;
    }
    if( sr == SR_NONE && save_mod == NO_MOD && ss == SS_SCOPED ) {
        cih = ih;
        for( ih = ActProc->ih_list; ih != NULL; ih = ih->next ) {
            if( ih != cih ) {
                if( ih->dip->minor == DIP_MINOR ) {
                    sr = ih->dip->lookup_sym_ex( IMP_HDL( ih, image ),
                            ss, source, li, lc, d );
                } else {
                    sr = ih->dip->lookup_sym( IMP_HDL( ih, image ),
                            ss, source, li, d );
                }
                if( sr != SR_NONE ) break;
            }
        }
    }
    li->mod = save_mod;
    return( sr );
}

search_result LookupSym( symbol_source ss, void *source,
                        lookup_item *li, void *d )
{
    return( LookupSymEx( ss, source, li, NULL, d ) );
}

search_result AddrScope( mod_handle mh, address addr, scope_block *scope )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( AddrMod( addr, &mh ) == SR_NONE ) return( SR_NONE );
    }
    ih = MH2IH( mh );
    if( ih == NULL ) return( SR_NONE );
    return( ih->dip->addr_scope( IMP_HDL( ih, image ),
                MH_SMOD( mh ), addr, scope ) );
}

search_result ScopeOuter( mod_handle mh, scope_block *in, scope_block *out )
{
    image_handle        *ih;

    if( mh == NO_MOD ) {
        if( AddrMod( in->start, &mh ) == SR_NONE ) return( SR_NONE );
    }
    ih = MH2IH( mh );
    if( ih == NULL ) return( SR_NONE );
    return( ih->dip->scope_outer( IMP_HDL( ih, image ),
                MH_SMOD( mh ), in, out ) );
}
