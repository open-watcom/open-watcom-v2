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
* Description:  Produce Watcom style debugging information in load file.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "walloca.h"
#include "linkstd.h"
#include "pcobj.h"
#include "alloc.h"
#include "dbginfo.h"
#include "newmem.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "objnode.h"
#include "loadfile.h"
#include "objcalc.h"
#include "objomf.h"
#include "overlays.h"
#include "specials.h"
#include "ring.h"
#include "dbgcomm.h"
#include "dbgwat.h"
#include "machtype.h"
#include "wdbginfo.h"

#include "clibext.h"


#define DEMAND_INFO_SPLIT   _16KB

#define NON_SECT_INFO 0x8000

typedef struct {
    unsigned_32         offset;
    unsigned_16         num;
    unsigned_16         size;
} demanddata;

typedef struct odbimodinfo {
    unsigned_32         linelinksize;
    demanddata          types;
    demanddata          locals;
    demanddata          lines;
    unsigned_32         modnum;
    unsigned            dbisourceoffset;
} odbimodinfo;

#if 0
typedef struct seginfo {
    struct seginfo      *next;
    unsigned_32         dbioff;
    seg_dbg_info        head;
    unsigned_32         endoflast;
    seg_leader          *prevlead;
    seg_leader          *final;
    byte                nonsect : 1;
    byte                full : 1;
    byte                finished : 1;
} seginfo;
#endif

typedef struct snamelist {              // source name list
    struct snamelist    *next;
    size_t              len;            // length of the name
    char                name[1];        // stored WITH a nullchar
} snamelist;

static unsigned_32          DBISize;
static master_dbg_header    Master;             // rest depend on .obj files.

static snamelist            *DBISourceLang;     // list of source languages

#ifdef _INT_DEBUG
struct {
    offset   sizeadded;
    offset   sizegenned;
} TraceInfo;
#endif

static snamelist *LangAlloc( size_t len, const char *buff )
/*********************************************************/
{
    snamelist *node;

    _PermAlloc( node, sizeof( snamelist ) + len );
    node->len = len;
    memcpy( node->name, buff, len );
    node->name[len] = '\0';
    return( node );
}

void ODBIInit( section *sect )
/****************************/
{
    DBISize = sizeof( master_dbg_header );
    Master.signature = WAT_DBG_SIGNATURE;
    Master.exe_major_ver = EXE_MAJOR_VERSION;
    Master.exe_minor_ver = EXE_MINOR_VERSION;
    Master.obj_major_ver = 0;
    Master.obj_minor_ver = 0;
    DBISourceLang = LangAlloc( 1, "C" );
    DBISourceLang->next = NULL;
    _PermAlloc( sect->dbg_info, sizeof( debug_info ) );
    memset( sect->dbg_info, 0, sizeof( debug_info ) );  //assumes NULL == 0
#ifdef _INT_DEBUG
    memset( &TraceInfo, 0, sizeof( TraceInfo ) );
#endif
}

void ODBIInitModule( mod_entry *mod )
/***********************************/
{
    if( CurrSect->dbg_info == NULL )
        return;
    _PermAlloc( mod->d.o, sizeof( odbimodinfo ) );
    memset( mod->d.o, 0, sizeof( odbimodinfo ) );
}

static void DumpInfo( debug_info *dinfo, const void *data, size_t len )
/*********************************************************************/
{
    PutInfo( dinfo->dump_addr, data, len );
    dinfo->dump_addr += len;
}

static bool FindMatch( size_t len, const char *buff, unsigned *offset )
/*********************************************************************/
// returns false if not found
{
    snamelist   *node;

    *offset = 0;
    for( node = DBISourceLang; node != NULL; node = node->next ) {
        if( node->len == len ) {
            if( strnicmp( buff, node->name, len ) == 0 ) {
                return( true );
            }
        }
        *offset += node->len + 1;     // +1 for NULLCHAR
    }
    return( false );
}

void ODBIP1Source( byte major, byte minor, const char *name, size_t len )
/***********************************************************************/
{
    snamelist   *node;

    if( Master.obj_major_ver == 0 )
        Master.obj_major_ver = major;
    if( major != Master.obj_major_ver ) {
        LnkMsg( LOC+WRN+MSG_CANT_USE_LOCALS, NULL );
        CurrMod->modinfo &= ~( DBI_TYPE | DBI_LOCAL );
    }
    if( minor > Master.obj_minor_ver ) {
        Master.obj_minor_ver = minor;
    }
    if( !FindMatch( len, name, &CurrMod->d.o->dbisourceoffset ) ) {
        node = LangAlloc( len, name );
        node->next = DBISourceLang->next;
        DBISourceLang->next = node;        // keep "C" the first entry.
    }
}

static void DoAddLocal( dbi_section *dbi, offset length )
/*******************************************************/
{
    if( ( dbi->size == 0 ) || ( dbi->size + length > DEMAND_INFO_SPLIT ) ) {
        dbi->curr.u.vm_offs += sizeof( unsigned_32 );
        dbi->size = 0;
    }
    dbi->size += length;
#ifdef _INT_DEBUG
    TraceInfo.sizeadded += length;
#endif
}

void ODBIAddLocal( seg_leader *seg, offset length )
/*************************************************/
{
    debug_info          *dinfo;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( seg->dbgtype == MS_TYPE ) {
        DEBUG(( DBG_DBGINFO, "adding type info %h", length ));
        DoAddLocal( &dinfo->typelinks, length );
    } else if( seg->dbgtype == MS_LOCAL ) {
        DEBUG(( DBG_DBGINFO, "adding local info %h", length ));
        DoAddLocal( &dinfo->locallinks, length );
    }
}

void ODBIP1ModuleScanned( void )
/******************************/
{
    debug_info          *dinfo;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    dinfo->typelinks.size = 0;
    dinfo->locallinks.size = 0;
}

static void DoGenLocal( dbi_section *dsect, dbi_section *dlink, demanddata *dmod, offset length )
/***********************************************************************************************/
{
    unsigned_32 spot;

    if( ( dmod->size == 0 ) || ( dmod->size + length > DEMAND_INFO_SPLIT ) ) {
        spot = dsect->start + ( dsect->curr.u.vm_offs - dsect->init.u.vm_offs );
        PutInfo( dlink->curr.u.vm_ptr, &spot, sizeof( unsigned_32 ) );
        dmod->num++;
        if( dmod->size == 0 ) {
            dmod->offset = dlink->start + ( dlink->curr.u.vm_ptr - dlink->init.u.vm_ptr );
        } else {
            dmod->size = 0;
        }
        dlink->curr.u.vm_ptr += sizeof( unsigned_32 );
    }
    dsect->curr.u.vm_ptr += length;
    dmod->size += length;
#ifdef _INT_DEBUG
    TraceInfo.sizegenned += length;
#endif
}

void ODBIGenLocal( segdata *sdata )
/*********************************/
{
    debug_info          *dinfo;
    odbimodinfo         *minfo;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( sdata->isdead )
        return;
    minfo = CurrMod->d.o;
    if( sdata->u.leader->dbgtype == MS_TYPE ) {
        DEBUG(( DBG_DBGINFO, "genning type info %h", sdata->length ));
        DoGenLocal( &dinfo->type, &dinfo->typelinks, &minfo->types, sdata->length );
    } else if( sdata->u.leader->dbgtype == MS_LOCAL ) {
        DEBUG(( DBG_DBGINFO, "genning local info %h", sdata->length ));
        DoGenLocal( &dinfo->local, &dinfo->locallinks, &minfo->locals, sdata->length );
    }
}

static void ODBIAddLines( lineinfo *info )
/****************************************/
{
    unsigned            lineqty;
    unsigned_32         linesize;
    debug_info          *dinfo;

    dinfo = CurrSect->dbg_info;
    lineqty = DBICalcLineQty( info );
    linesize = lineqty * sizeof( ln_off_386 ) + sizeof( lineseg );
    dinfo->line.curr.u.vm_offs += linesize;
    DoAddLocal( &dinfo->linelinks, linesize );
}

void ODBIP1ModuleFinished( mod_entry *obj )
/*****************************************/
{
    debug_info          *dinfo;

    dinfo = CurrSect->dbg_info;
    if( ( dinfo == NULL ) || (obj->modinfo & DBI_ALL) == 0 )
        return;
    if( MOD_NOT_DEBUGGABLE( obj ) )
        return;
    CurrMod = obj;
    if( CurrMod->modinfo & DBI_LINE ) {
        DBILineWalk( obj->lines, ODBIAddLines );
    }
    Ring2Walk( obj->publist, DBIModGlobal );
    dinfo->mod.curr.u.vm_offs += strlen( obj->name.u.ptr ) + sizeof( mod_dbg_info );
    dinfo->linelinks.size = 0;
}

void ODBIDefClass( class_entry *class, unsigned_32 size )
/*******************************************************/
{
    debug_info *dinfo;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( class->flags & CLASS_MS_TYPE ) {
        dinfo->type.curr.u.vm_offs += size;
        dinfo->TypeClass = class;
    } else if( class->flags & CLASS_MS_LOCAL ) {
        dinfo->local.curr.u.vm_offs += size;
        dinfo->LocalClass = class;
    }
}

static int ODBISymIsForGlobalDebugging( symbol *sym, mod_entry *currMod )
/***********************************************************************/
{
    return( (currMod->modinfo & DBI_ONLY_EXPORTS) == 0 && ( (currMod->modinfo & DBI_STATICS) || (sym->info & SYM_STATIC) == 0 ) );
}

void ODBIAddGlobal( symbol *sym )
/*******************************/
{
    debug_info      *dinfo;
    size_t          len;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( ODBISymIsForGlobalDebugging( sym, CurrMod ) ) {
        len = strlen( sym->name.u.ptr );
        if( len > 255 ) {
            LnkMsg( WRN+MSG_SYMBOL_NAME_TOO_LONG, "s", sym->name.u.ptr );
            len = 255;
        }
        dinfo->global.curr.u.vm_offs += sizeof( v3_gbl_info ) + len;
    }
}

static bool AllocASeg( void *leader, void *group )
/************************************************/
{
    ((seg_leader *)leader)->group = group;
    return( false );
}

static void AllocDBIClasses( class_entry *class )
/***********************************************/
/* Allocate all classes in the list */
{
    group_entry *group;

    for( ; class != NULL; class = class->next_class ) {
        if( class->flags & CLASS_DEBUG_INFO ) {
            group = AllocGroup( AutoGrpName, &DBIGroups );
            group->grp_addr.seg = 0;
            RingLookup( class->segs, AllocASeg, group );
        }
    }
}

void ODBIAddrSectStart( section *sect )
/*************************************/
{
    debug_info      *dptr;

    dptr = sect->dbg_info;
    if( dptr == NULL )
        return;
    if( ( dptr->local.curr.u.vm_offs > 0 ) || ( dptr->type.curr.u.vm_offs > 0 ) || ( dptr->line.curr.u.vm_offs > 0 ) ) {
        dptr->linelinks.curr.u.vm_offs += sizeof( unsigned_32 );
    }
    dptr->locallinks.start = sizeof( section_dbg_header );
    dptr->locallinks.size = dptr->locallinks.curr.u.vm_offs;
    dptr->locallinks.curr.u.vm_ptr = DBIAlloc( dptr->locallinks.curr.u.vm_offs );
    dptr->locallinks.init.u.vm_ptr = dptr->locallinks.curr.u.vm_ptr;

    dptr->typelinks.start = dptr->locallinks.start + dptr->locallinks.size;
    dptr->typelinks.size = dptr->typelinks.curr.u.vm_offs;
    dptr->typelinks.curr.u.vm_ptr = DBIAlloc( dptr->typelinks.curr.u.vm_offs );
    dptr->typelinks.init.u.vm_ptr = dptr->typelinks.curr.u.vm_ptr;

    dptr->linelinks.start = dptr->typelinks.start + dptr->typelinks.size;
    dptr->linelinks.size = dptr->linelinks.curr.u.vm_offs;
    dptr->linelinks.curr.u.vm_ptr = DBIAlloc( dptr->linelinks.curr.u.vm_offs );
    dptr->linelinks.init.u.vm_ptr = dptr->linelinks.curr.u.vm_ptr;

    dptr->local.start = dptr->linelinks.start + dptr->linelinks.size;
    dptr->local.size = dptr->local.curr.u.vm_offs;
    dptr->local.init.u.vm_offs = dptr->local.curr.u.vm_offs;

    dptr->type.start = dptr->local.start + dptr->local.size;
    dptr->type.size = dptr->type.curr.u.vm_offs;
    dptr->type.init.u.vm_offs = dptr->type.curr.u.vm_offs;

    dptr->line.start = dptr->type.start + dptr->type.size;
    dptr->line.size = dptr->line.curr.u.vm_offs;
    dptr->line.curr.u.vm_ptr = DBIAlloc( dptr->line.curr.u.vm_offs );
    dptr->line.init.u.vm_ptr = dptr->line.curr.u.vm_ptr;

    dptr->mod.start = dptr->line.start + dptr->line.size;
    dptr->mod.size = dptr->mod.curr.u.vm_offs;
    dptr->mod.curr.u.vm_ptr = DBIAlloc( dptr->mod.curr.u.vm_offs );
    dptr->mod.init.u.vm_ptr = dptr->mod.curr.u.vm_ptr;

    dptr->global.start = dptr->mod.start + dptr->mod.size;
    dptr->addr.start = dptr->global.start + dptr->global.curr.u.vm_offs;
    dptr->global.size = 0;
    dptr->global.curr.u.vm_ptr = DBIAlloc( dptr->global.curr.u.vm_offs );
    dptr->global.init.u.vm_ptr = dptr->global.curr.u.vm_ptr;

    dptr->addr.curr.u.vm_ptr = DBIAlloc( dptr->addr.size );
    dptr->addr.init.u.vm_ptr = dptr->addr.curr.u.vm_ptr;

    dptr->dump_addr = dptr->global.curr.u.vm_ptr;
    dptr->modnum = -1;

    AllocDBIClasses( sect->classlist );
}

static void DoName( const char *cname, char *intelname, size_t len )
/******************************************************************/
{
    intelname[0] = (char)len;
    memcpy( intelname + 1, cname, len );
}

void ODBIGenGlobal( symbol *sym, section *sect )
/**********************************************/
{
    size_t      len;
    v3_gbl_info *data;
    debug_info  *dptr;

    dptr = sect->dbg_info;
    if( dptr == NULL )
        return;
    if( ODBISymIsForGlobalDebugging( sym, CurrMod ) ) {
        len = strlen( sym->name.u.ptr );
        if( len > 255 ) {
            len = 255;
        }
        data = (v3_gbl_info *)alloca( sizeof( v3_gbl_info ) + len );
        _HostU32toTarg( sym->addr.off, data->addr.offset );
        _HostU16toTarg( sym->addr.seg, data->addr.segment );
        _HostU16toTarg( dptr->modnum, data->mod );
        data->kind = 0;
        if( sym->info & SYM_STATIC ) {
            data->kind |= GBL_KIND_STATIC;
        }
        if( sym->p.seg != NULL ) {
            if( sym->p.seg->u.leader->info & SEG_CODE ) {
                data->kind |= GBL_KIND_CODE;
            } else {
                data->kind |= GBL_KIND_DATA;
            }
        }
        DoName( sym->name.u.ptr, data->name, len );
        len += sizeof( v3_gbl_info );
        DumpInfo( dptr, data, len );
        dptr->global.size += len;
    }
}

void ODBIAddModule( mod_entry *obj, section *sect )
/*************************************************/
{
    debug_info          *dptr;

    dptr = sect->dbg_info;
    if( ( dptr == NULL ) || (obj->modinfo & DBI_ALL) == 0 )
        return;
    dptr->modnum++;
    obj->d.o->modnum = dptr->modnum;
}

static void ODBIGenAddrInit( segdata *sdata, void *_dinfo )
/*********************************************************/
{
    seg_dbg_info    seghdr;
    seg_leader      *seg;
    debug_info      *dptr = _dinfo;

    seg = sdata->u.leader;
    if( seg->group == NULL ) {
        seghdr.base.offset = seg->seg_addr.off;
        seghdr.base.segment = seg->seg_addr.seg;
    } else {
        seghdr.base.offset = seg->group->grp_addr.off + SEG_GROUP_DELTA( seg );
        seghdr.base.segment = seg->group->grp_addr.seg;
    }
    seghdr.count = seg->num;
    if( CurrSect == NonSect )
        seghdr.count |= NON_SECT_INFO;
    DumpInfo( dptr, &seghdr, sizeof( seg_dbg_info ) - sizeof( addr_dbg_info ) );
    dptr->addr.curr.u.vm_ptr += sizeof( seg_dbg_info ) - sizeof( addr_dbg_info );
}

static void ODBIGenAddrAdd( segdata *sdata, offset delta, offset size, void *_dinfo, bool isnewmod )
/**************************************************************************************************/
{
    addr_dbg_info   addr;
    debug_info      *dptr = _dinfo;

    /* unused parameters */ (void)delta;

    if( isnewmod ) {
        addr.size = size;
        addr.mod = sdata->o.mod->d.o->modnum;
        DumpInfo( dptr, &addr, sizeof( addr_dbg_info ) );
        sdata->addrinfo = dptr->addr.curr.u.vm_ptr - dptr->addr.init.u.vm_ptr;
        dptr->addr.curr.u.vm_ptr += sizeof( addr_dbg_info );
    } else {
        sdata->addrinfo = dptr->addr.curr.u.vm_ptr - dptr->addr.init.u.vm_ptr;
    }
}

static void ODBIGenAddrInfo( seg_leader *seg )
/********************************************/
{
    debug_info  *dptr;

    if( CurrSect == NonSect ) {
        dptr = Root->dbg_info;
    } else {
        dptr = CurrSect->dbg_info;
    }
    if( ( dptr == NULL ) || ( seg->num == 0 ) )
        return;
    DBIAddrInfoScan( seg, ODBIGenAddrInit, ODBIGenAddrAdd, dptr );
}

static void WriteBogusAddrInfo( debug_info *dptr )
/************************************************/
{
    seg_dbg_info    header;

    dptr->addr.size = sizeof( seg_dbg_info );
    dptr->addr.curr.u.vm_ptr = DBIAlloc( dptr->addr.size );
    dptr->addr.init.u.vm_ptr = dptr->addr.curr.u.vm_ptr;
    dptr->dump_addr = dptr->addr.curr.u.vm_ptr;
    header.base.offset = 0;
    header.base.segment = 0;
    header.count = 1;
    header.addr[0].size = 0;
    header.addr[0].mod = 0;
    DumpInfo( dptr, &header, sizeof( seg_dbg_info ) );
    dptr->addr.curr.u.vm_ptr += sizeof( seg_dbg_info );
}

void ODBIP2Start( section *sect )
/*******************************/
/* initialize pointers for pass 2 processing */

{
    debug_info          *dptr;

    if( sect == NonSect ) {
        dptr = Root->dbg_info;
    } else {
        dptr = sect->dbg_info;
    }
    if( dptr != NULL ) {
        // if section has no info then write bogus address info
        if( dptr->addr.curr.u.vm_ptr == 0 ) {
            WriteBogusAddrInfo( dptr );
        } else {
            dptr->dump_addr = dptr->addr.curr.u.vm_ptr;
            SectWalkClass( sect, ODBIGenAddrInfo );
        }
        dptr->dump_addr = dptr->line.curr.u.vm_ptr;
        dptr->modnum = 0;
    }
}

static int CmpLn386( const void *a, const void *b )
/*************************************************/
{
    return( ((ln_off_386 _WCUNALIGNED *)a)->off - ((ln_off_386 _WCUNALIGNED *)b)->off );
}

static int CmpLn286( const void *a, const void *b )
/*************************************************/
{
    return( ((ln_off_286 *)a)->off - ((ln_off_286 *)b)->off );
}

static bool CheckFirst( void *_seg, void *_firstseg )
/***************************************************/
{
    segdata *seg = _seg;
    segdata **firstseg = _firstseg;

    if( ( seg->a.delta < (*firstseg)->a.delta )
            && ( seg->addrinfo == (*firstseg)->addrinfo ) ) {
        *firstseg = seg;
    }
    return( false );
}

void ODBIGenLines( lineinfo *info )
/*********************************/
{
    size_t              linelen;
    ln_off_pair _WCUNALIGNED *pair;
    ln_off_386          tmp_ln;
    unsigned_32         temp;
    size_t              lineqty;
    debug_info          *dinfo;
    lineseg             lseg;
    segdata             *firstseg;
    unsigned_32         prevoff;
    offset              adjust;
    bool                needsort;
    size_t              size;
    size_t              item_size;
    segdata             *seg;
    int                 (*CmpLn)( const void *a, const void *b );

    seg = info->seg;

    dinfo = CurrSect->dbg_info;
    if( ( dinfo == NULL ) || (CurrMod->modinfo & DBI_LINE) == 0 )
        return;
    lineqty = DBICalcLineQty( info );
    DoGenLocal( &dinfo->line, &dinfo->linelinks, &CurrMod->d.o->lines, lineqty * sizeof( ln_off_386 ) + sizeof( lineseg ) );
    lseg.segment = seg->addrinfo;
    lseg.num = lineqty;
    DumpInfo( dinfo, &lseg, sizeof( lineseg ) );
/*
    fix the offset so that, together with modinfo.seg, it
    represents the offset of that line in the image
    also when we have multiple segdefs for the same segment, we collapse the
    addr infos, so we have to adjust the line # offset to account for this.
*/
    firstseg = Ring2Step( CurrMod->segs, NULL );
    Ring2Lookup( CurrMod->segs, CheckFirst, &firstseg );
    adjust = seg->a.delta - firstseg->a.delta;
    pair = (ln_off_pair *)info->data;
    prevoff = 0;
    needsort = false;
    if( info->size & LINE_IS_32BIT ) {
        item_size = sizeof( ln_off_386 );
        CmpLn = CmpLn386;
    } else {
        item_size = sizeof( ln_off_286 );
        CmpLn = CmpLn286;
    }
    linelen = info->size & ~LINE_IS_32BIT;
    for( size = linelen; size > 0; size -= item_size ) {
        if( info->size & LINE_IS_32BIT ) {
            pair->_386.off += adjust;
            if( prevoff > pair->_386.off ) {
                needsort = true;
            }
            prevoff = pair->_386.off;
        } else {
            _TargU16toHost( pair->_286.off, temp );
            if( prevoff > temp ) {
                needsort = true;
            }
            prevoff = temp;
        }
        pair = (void *)( (char *)pair + item_size );
    }
    if( needsort ) {
        qsort( info->data, lineqty, item_size, CmpLn );
    }
    if( info->size & LINE_IS_32BIT ) {
        DumpInfo( dinfo, info->data, linelen );
    } else {
        pair = (ln_off_pair *)info->data;
        for( size = linelen; size > 0; size -= item_size ) {
            _TargU16toHost( pair->_286.off, temp );
            _HostU32toTarg( temp + adjust, tmp_ln.off );
            tmp_ln.linnum = pair->_286.linnum;
            // NYI: might have to do some buffering here
            DumpInfo( dinfo, &tmp_ln, sizeof( ln_off_386 ) );
            pair = (void *)( (char *)pair + item_size );
        }
    }
}

static void ODBIAddAddrInit( segdata *sdata, void *cookie )
/*********************************************************/
{
    /* unused parameters */ (void)cookie;

    sdata->u.leader->num = 0;
}

static void ODBIAddAddrAdd( segdata *sdata, offset delta, offset size,
                            void *_dinfo, bool isnewmod )
/*******************************************************/
{
    /* unused parameters */ (void)delta; (void)size;

    if( !isnewmod )
        return;
    ((debug_info *)_dinfo)->addr.size += sizeof( addr_dbg_info );
    sdata->u.leader->num++;
}

void ODBIAddAddrInfo( seg_leader *seg )
/*************************************/
{
    debug_info *dptr;

    if( CurrSect == NonSect ) {
        dptr = Root->dbg_info;
    } else {
        dptr = CurrSect->dbg_info;
    }
    if( dptr == NULL )
        return;
    DBIAddrInfoScan( seg, ODBIAddAddrInit, ODBIAddAddrAdd, dptr );
    if( seg->num > 0 ) {
        dptr->addr.size += sizeof( seg_dbg_info ) - sizeof( addr_dbg_info );
    }
}

void ODBIFini( section *sect )
/****************************/
// write out the final links in the link tables.
{
    debug_info          *dptr;
    unsigned_32         spot;

    if( sect == NonSect ) {
        dptr = Root->dbg_info;
    } else {
        dptr = sect->dbg_info;
    }
    if( dptr != NULL ) {
        if( ( dptr->local.size > 0 ) || ( dptr->type.size > 0 ) || ( dptr->line.size > 0 ) ) {
            spot = dptr->line.start + ( dptr->line.curr.u.vm_ptr - dptr->line.init.u.vm_ptr );
            PutInfo( dptr->linelinks.curr.u.vm_ptr, &spot, sizeof( unsigned_32 ) );
        }
    }
}

void ODBIGenModule( void )
/************************/
{
    odbimodinfo         *rec;
    mod_dbg_info        *info;
    size_t              len;
    debug_info          *dptr;

    dptr = CurrSect->dbg_info;
    if( ( dptr == NULL ) || (CurrMod->modinfo & DBI_ALL) == 0 )
        return;
    rec = CurrMod->d.o;
    len = strlen( CurrMod->name.u.ptr );
    if( len > 255 )
        len = 255;
    info = (mod_dbg_info *)alloca( sizeof( mod_dbg_info ) + len );
    _HostU16toTarg( rec->types.num, info->di[DMND_TYPES].u.entries );
    _HostU32toTarg( rec->types.offset, info->di[DMND_TYPES].info_off );
    _HostU16toTarg( rec->locals.num, info->di[DMND_LOCALS].u.entries );
    _HostU32toTarg( rec->locals.offset, info->di[DMND_LOCALS].info_off );
    _HostU16toTarg( rec->lines.num, info->di[DMND_LINES].u.entries );
    _HostU32toTarg( rec->lines.offset, info->di[DMND_LINES].info_off );
    DoName( CurrMod->name.u.ptr, info->name, len );
    info->language = rec->dbisourceoffset;
    len += sizeof( mod_dbg_info );
    PutInfo( dptr->mod.curr.u.vm_ptr, (char *)info, len );
    dptr->mod.curr.u.vm_ptr += len;
    dptr->modnum++;
}

void ODBISectCleanup( section *sect )
/***********************************/
{
    /* unused parameters */ (void)sect;

    _PermFree( sect->dbg_info );
}

static void DBIWriteInfo( virt_mem stg, unsigned long len )
/*********************************************************/
{
    if( len > 0 ) {
        DBISize += len;
        WriteInfoLoad( stg, len );
    }
}

static void DBIWriteLocal( const void *buff, size_t len )
/*******************************************************/
{
    if( len > 0 ) {
        DBISize += len;
        WriteLoad( buff, len );
    }
}

static unsigned_16 WriteSegValues( void )
/***************************************/
// write out all possible group segment values
{
    unsigned_16     segarray[2];
    group_entry     *currgrp;
    unsigned_16     *buffer;
    unsigned_16     buflen;

    if( FmtData.type & MK_FLAT_OFFS ) {
        segarray[0] = 1;
        DBIWriteLocal( segarray, sizeof( unsigned_16 ) );
        return( sizeof( unsigned_16 ) );
    } else if( FmtData.type & MK_ID_SPLIT ) {
        segarray[0] = CODE_SEGMENT;
        segarray[1] = DATA_SEGMENT;
        DBIWriteLocal( segarray, sizeof( unsigned_16 ) * 2 );
        return( sizeof( unsigned_16 ) * 2 );
    } else {
        buffer = (unsigned_16 *) TokBuff;
        buflen = 0;
        for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
            *buffer++ = currgrp->grp_addr.seg;
            buflen += sizeof( unsigned_16 );
        }
        DBIWriteLocal( TokBuff, buflen );
        return( buflen );
    }
}

static void WriteDBISecs( section *sec )
/**************************************/
{
    debug_info          *dptr;
    section_dbg_header  header;
    unsigned long       pos;

    dptr = sec->dbg_info;
    if( dptr != NULL ) {
        header.section_size = dptr->addr.start + dptr->addr.size;
        header.mod_offset = dptr->mod.start;
        header.gbl_offset = dptr->global.start;
        header.addr_offset = dptr->addr.start;
        header.section_id = sec->ovlref;
        DBIWriteLocal( &header, sizeof( section_dbg_header ) );
        DBIWriteInfo( dptr->locallinks.init.u.vm_ptr, dptr->locallinks.size );
        DBIWriteInfo( dptr->typelinks.init.u.vm_ptr, dptr->typelinks.size );
        DBIWriteInfo( dptr->linelinks.init.u.vm_ptr, dptr->linelinks.size );
        pos = PosLoad();
        if( dptr->LocalClass != NULL ) {
            RingWalk( dptr->LocalClass->segs, WriteLeaderLoad );
        }
        if( dptr->TypeClass != NULL ) {
            RingWalk( dptr->TypeClass->segs, WriteLeaderLoad );
        }
        DBISize += PosLoad() - pos;
        DBIWriteInfo( dptr->line.init.u.vm_ptr, dptr->line.size );
        DBIWriteInfo( dptr->mod.init.u.vm_ptr, dptr->mod.size );
        DBIWriteInfo( dptr->global.init.u.vm_ptr, dptr->global.size );
        DBIWriteInfo( dptr->addr.init.u.vm_ptr, dptr->addr.size );
    }
}

void ODBIWrite( void )
/********************/
/* copy debugging info from extra memory to loadfile */
{
    snamelist   *node;
    snamelist   *next;

    CurrSect = Root;
    Master.lang_size = 0;
    for( node = DBISourceLang; node != NULL; node = next ) {
        next = node->next;
        Master.lang_size += node->len + 1;
        DBIWriteLocal( node->name, node->len + 1 );  // +1 for nullchar
        _PermFree( node );
    }
    DBISourceLang = NULL;
    Master.segment_size = WriteSegValues();
    WalkAllSects( WriteDBISecs );
    Master.debug_size = DBISize;
    if( Master.obj_major_ver == 0 )
        Master.obj_major_ver = 1;
    WriteLoad( &Master, sizeof( master_dbg_header ) );
#ifdef _INT_DEBUG
    if( TraceInfo.sizeadded != TraceInfo.sizegenned ) {
        LnkMsg( WRN+MSG_INTERNAL, "s", "size mismatch in watcom dbi" );
    }
#endif
}
