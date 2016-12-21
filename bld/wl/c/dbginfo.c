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

#include "clibext.h"

#define EXE_MAJOR_VERSION   3
#define EXE_MINOR_VERSION   0

#define ADDR_INFO_LIMIT ( 63 * 1024U / sizeof( addrinfo ) )
#define DEMAND_INFO_SPLIT ( 16 * 1024 )

#define NON_SECT_INFO 0x8000

typedef struct {
    unsigned_32 offset;
    unsigned_16 num;
    unsigned_16 size;
} demanddata;

typedef struct odbimodinfo {
    unsigned_32 linelinksize;
    demanddata  types;
    demanddata  locals;
    demanddata  lines;
    unsigned_32 modnum;
    unsigned    dbisourceoffset;
} odbimodinfo;

typedef struct seginfo     {
    struct seginfo      *next;
    unsigned_32         dbioff;
    segheader           head;
    unsigned_32         endoflast;
    seg_leader          *prevlead;
    seg_leader          *final;
    byte                nonsect : 1;
    byte                full : 1;
    byte                finished : 1;
} seginfo;

typedef struct snamelist {        // source name list
    struct snamelist    *next;
    byte                len;        // length of the name
    char                name[1];       // stored WITH a nullchar
} snamelist;

static unsigned_32  DBISize;
static dbgheader    Master;            // rest depend on .obj files.

static snamelist    *DBISourceLang;     // list of source languages

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
/***********************************/
{
    DBISize = sizeof( dbgheader );
    Master.signature = DBG_SIGNATURE;
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
/******************************************/
{
    if( CurrSect->dbg_info == NULL )
        return;
    _PermAlloc( mod->d.o, sizeof( odbimodinfo ) );
    memset( mod->d.o, 0, sizeof( odbimodinfo ) );
}

static void DumpInfo( debug_info *dinfo, void *data, unsigned len )
/*****************************************************************/
{
    PutInfo( dinfo->dump_addr, data, len );
    dinfo->dump_addr += len;
}

static bool FindMatch( size_t len, const void *buff, unsigned *offset )
/*********************************************************************/
// returns false if not found
{
    snamelist   *node;

    *offset = 0;
    for( node = DBISourceLang; node != NULL; node = node->next ) {
        if( node->len == len ) {
            if( memicmp( buff, node->name, len ) == 0 ) {
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
/********************************************************/
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
/*********************************************************/
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
/*************************************/
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
/****************************************/
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
/************************************************/
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
    dinfo->mod.curr.u.vm_offs += strlen( obj->name ) + sizeof( modinfo );
    dinfo->linelinks.size = 0;
}

void ODBIDefClass( class_entry *cl, unsigned_32 size )
/***********************************************************/
{
    debug_info *dinfo;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( cl->flags & CLASS_MS_TYPE ) {
        dinfo->type.curr.u.vm_offs += size;
        dinfo->TypeClass = cl;
    } else if( cl->flags & CLASS_MS_LOCAL ) {
        dinfo->local.curr.u.vm_offs += size;
        dinfo->LocalClass = cl;
    }
}

static int ODBISymIsForGlobalDebugging( symbol *sym, mod_entry *currMod )
/***********************************************************************/
{
    return( (currMod->modinfo & DBI_ONLY_EXPORTS) == 0 && ( (currMod->modinfo & DBI_STATICS) || (sym->info & SYM_STATIC) == 0 ) );
}

void ODBIAddGlobal( symbol *sym )
/**************************************/
{
    debug_info      *dinfo;
    unsigned        add;

    dinfo = CurrSect->dbg_info;
    if( dinfo == NULL )
        return;
    if( ODBISymIsForGlobalDebugging( sym, CurrMod ) ) {
        add = strlen( sym->name );
        if( add > 255 ) {
            LnkMsg( WRN+MSG_SYMBOL_NAME_TOO_LONG, "s", sym->name );
            add = 255;
        }
        dinfo->global.curr.u.vm_offs += add + sizeof( gblinfo );
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
/********************************************/
{
    debug_info      *dptr;

    dptr = sect->dbg_info;
    if( dptr == NULL )
        return;
    if( ( dptr->local.curr.u.vm_offs > 0 ) || ( dptr->type.curr.u.vm_offs > 0 ) || ( dptr->line.curr.u.vm_offs > 0 ) ) {
        dptr->linelinks.curr.u.vm_offs += sizeof( unsigned_32 );
    }
    dptr->locallinks.start = sizeof( sectheader );
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

static void DoName( const char *cname, char *intelname, unsigned_8 len_u8 )
/*************************************************************************/
{
    intelname[0] = len_u8;
    memcpy( intelname + 1, cname, len_u8 );
}

void ODBIGenGlobal( symbol *sym, section *sect )
/******************************************************/
{
    size_t      len;
    unsigned_8  len_u8;
    gblinfo     *data;
    char        *name;
    debug_info  *dptr;

    dptr = sect->dbg_info;
    if( dptr == NULL )
        return;
    if( ODBISymIsForGlobalDebugging( sym, CurrMod ) ) {
        name = sym->name;
        len = strlen( name );
        len_u8 = 255;
        if( len < 255 ) {
            len_u8 = len;
        }
        len = sizeof( gblinfo ) + len_u8;
        data = (gblinfo *)alloca( len );
        _HostU32toTarg( sym->addr.off, data->off );
        _HostU16toTarg( sym->addr.seg, data->seg );
        _HostU16toTarg( dptr->modnum, data->mod_idx );
        data->flags = 0;
        if( sym->info & SYM_STATIC ) {
            data->flags |= DBG_GBL_STATIC;
        }
        if( sym->p.seg != NULL ) {
            if( sym->p.seg->u.leader->info & SEG_CODE ) {
                data->flags |= DBG_GBL_CODE;
            } else {
                data->flags |= DBG_GBL_DATA;
            }
        }
        DoName( name, data->name, len_u8 );
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
/********************************************************/
{
    segheader   seghdr;
    seg_leader  *seg;
    debug_info  *dptr = _dinfo;

    seg = sdata->u.leader;
    if( seg->group == NULL ) {
        seghdr.off = seg->seg_addr.off;
        seghdr.seg = seg->seg_addr.seg;
    } else {
        seghdr.off = seg->group->grp_addr.off + SEG_GROUP_DELTA( seg );
        seghdr.seg = seg->group->grp_addr.seg;
    }
    seghdr.num = seg->num;
    if( CurrSect == NonSect )
        seghdr.num |= NON_SECT_INFO;
    DumpInfo( dptr, &seghdr, sizeof( segheader ) );
    dptr->addr.curr.u.vm_ptr += sizeof( segheader );
}

static void ODBIGenAddrAdd( segdata *sdata, offset delta, offset size, void *_dinfo, bool isnewmod )
/**************************************************************************************************/
{
    addrinfo    addr;
    debug_info  *dptr = _dinfo;

    delta = delta;
    if( isnewmod ) {
        addr.size = size;
        addr.mod_idx = sdata->o.mod->d.o->modnum;
        DumpInfo( dptr, &addr, sizeof( addrinfo ) );
        sdata->addrinfo = dptr->addr.curr.u.vm_ptr - dptr->addr.init.u.vm_ptr;
        dptr->addr.curr.u.vm_ptr += sizeof( addrinfo );
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
/*************************************************/
{
    addrinfo    info;
    segheader   header;

    dptr->addr.size = sizeof( segheader ) + sizeof( addrinfo );
    dptr->addr.curr.u.vm_ptr = DBIAlloc( dptr->addr.size );
    dptr->addr.init.u.vm_ptr = dptr->addr.curr.u.vm_ptr;
    dptr->dump_addr = dptr->addr.curr.u.vm_ptr;
    header.off = 0;
    header.seg = 0;
    header.num = 1;
    DumpInfo( dptr, &header, sizeof( segheader ) );
    dptr->addr.curr.u.vm_ptr += sizeof( segheader );
    info.size = 0;
    info.mod_idx = 0;
    DumpInfo( dptr, &info, sizeof( addrinfo ) );
    dptr->addr.curr.u.vm_ptr += sizeof( addrinfo );
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
    unsigned            linelen;
    ln_off_pair _WCUNALIGNED *pair;
    ln_off_386          tmp_ln;
    unsigned_32         temp;
    unsigned            lineqty;
    debug_info          *dinfo;
    lineseg             lseg;
    segdata             *firstseg;
    unsigned_32         prevoff;
    offset              adjust;
    bool                needsort;
    unsigned            size;
    unsigned            item_size;
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
    cookie = cookie;
    sdata->u.leader->num = 0;
}

static void ODBIAddAddrAdd( segdata *sdata, offset delta, offset size,
                            void *_dinfo, bool isnewmod )
/*******************************************************/
{
    delta = delta;
    size = size;
    if( !isnewmod )
        return;
    ((debug_info *)_dinfo)->addr.size += sizeof( addrinfo );
    sdata->u.leader->num++;
}

void ODBIAddAddrInfo( seg_leader *seg )
/********************************************/
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
        dptr->addr.size += sizeof( segheader );
    }
}

void ODBIFini( section *sect )
/***********************************/
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
    modinfo             *info;
    unsigned_8          len_u8;
    size_t              len;
    char                *name;
    debug_info          *dptr;

    dptr = CurrSect->dbg_info;
    if( ( dptr == NULL ) || (CurrMod->modinfo & DBI_ALL) == 0 )
        return;
    rec = CurrMod->d.o;
    name = CurrMod->name;
    len = strlen( name );
    len_u8 = 255;
    if( len < 255 )
        len_u8 = len;
    len = sizeof( modinfo ) + len_u8;
    info = (modinfo *)alloca( len );
    _HostU16toTarg( rec->types.num, info->types.len );
    _HostU32toTarg( rec->types.offset, info->types.off );
    _HostU16toTarg( rec->locals.num, info->locals.len );
    _HostU32toTarg( rec->locals.offset, info->locals.off );
    _HostU16toTarg( rec->lines.num, info->lines.len );
    _HostU32toTarg( rec->lines.offset, info->lines.off );
    DoName( name, info->name, len_u8 );
    info->language = rec->dbisourceoffset;
    PutInfo( dptr->mod.curr.u.vm_ptr, (char *)info, len );
    dptr->mod.curr.u.vm_ptr += len;
    dptr->modnum++;
}

void ODBISectCleanup( section *sect )
/******************************************/
{
    sect = sect;
    _PermFree( sect->dbg_info );
}

static void DBIWriteInfo( virt_mem stg, unsigned long len )
/*********************************************************/
{
    if( len == 0 )
        return;
    DBISize += len;
    WriteInfoLoad( stg, len );
}

static void DBIWriteLocal( const void *buff, size_t len )
/*******************************************************/
{
    if( len == 0 )
        return;
    DBISize += len;
    WriteLoad( buff, len );
}

static unsigned_16 WriteSegValues( void )
/***************************************/
// write out all possible group segment values
{
    unsigned_16     segarray[2];
    group_entry     *currgrp;
    unsigned_16     *buffer;
    unsigned_16     buflen;

    if( FmtData.type & MK_FLAT ) {
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
    debug_info      *dptr;
    sectheader      header;
    unsigned long   pos;

    dptr = sec->dbg_info;
    if( dptr != NULL ) {
        header.section_size = dptr->addr.start + dptr->addr.size;
        header.mod_offset = dptr->mod.start;
        header.gbl_offset = dptr->global.start;
        header.addr_offset = dptr->addr.start;
        header.section_id = sec->ovl_num;
        DBIWriteLocal( &header, sizeof( sectheader ) );
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
/***************************/
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
    Master.seg_size = WriteSegValues();
    WalkAllSects( WriteDBISecs );
    Master.debug_size = DBISize;
    if( Master.obj_major_ver == 0 )
        Master.obj_major_ver = 1;
    WriteLoad( &Master, sizeof( dbgheader ) );
#ifdef _INT_DEBUG
    if( TraceInfo.sizeadded != TraceInfo.sizegenned ) {
        LnkMsg( WRN+MSG_INTERNAL, "s", "size mismatch in watcom dbi" );
    }
#endif
}
