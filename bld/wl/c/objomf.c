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
* Description:  Object file processing routines specific to OMF.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "alloc.h"
#include "pcobj.h"
#include "obj2supp.h"
#include "objnode.h"
#include "objcalc.h"
#include "objio.h"
#include "objcache.h"
#include "wcomdef.h"
#include "cmdline.h"
#include "loadfile.h"
#include "dbgall.h"
#include "objpass1.h"
#include "objstrip.h"
#include "omfreloc.h"
#include "carve.h"
#include "strtab.h"
#include "permdata.h"
#include "virtmem.h"
#include "impexp.h"
#include "objomf.h"

enum dll_entry_type { DLL_RELOC_NAME, DLL_RELOC_ORDINAL };

/* forward declarations */

static void     Pass1Cmd( byte );
static void     ProcTHEADR( void );
static void     Comment( void );
static void     AddNames( void );
static void     ProcSegDef( void );
static void     ProcPubdef( bool static_sym );
static void     UseSymbols( bool static_sym, bool iscextdef );
static void     DefineGroup( void );
static void     ProcLinnum( void );
static void     ProcLxdata( bool islidata );
static void     ProcLxdata( bool islidata );
static void     ProcModuleEnd( void );
static void     ProcAlias( void );
static void     DoLazyExtdef( bool isweak );
static void     ProcVFTableRecord( bool ispure );
static void     ProcVFReference( void );
static void     GetObject( segdata *seg, unsigned_32 obj_offset, bool lidata );

byte            OMFAlignTab[] = {0,0,1,4,8,2,12};



void ResetObjOMF( void )
/**********************/
{
    ObjBuff = NULL;
    EOObjRec = NULL;
}

static unsigned long ProcObj( file_list *file, unsigned long loc, void (*procrtn)( byte ) )
/*****************************************************************************************/
/* Process an object file. */
{
    obj_record          *rec;
    byte                cmd;
    unsigned_16         len;

    RecNum = 0;
    do {
        ObjFormat &= ~FMT_MS_386;   // assume not a Microsoft 386 .obj file
        rec = CacheRead( file, loc, sizeof(obj_record) );
        if( rec == NULL ) {
            EarlyEOF();
            break;
        }
        loc += sizeof( obj_record );
        len = rec->length;
        cmd = rec->command;
        if( procrtn != NULL ) {
            RecNum++;
            ObjBuff = CacheRead( file, loc, len );
            if( ObjBuff == NULL ) {
                EarlyEOF();
                break;
            }
            EOObjRec = ObjBuff + len - 1;       // 1 for the checksum.
            (*procrtn)( cmd );
            if( ObjBuff > EOObjRec ) {
                BadObject();
                break;
            }
        }
        loc += len;
    } while( cmd != CMD_MODEND && cmd != CMD_MODE32 );
    return( loc );
}

static void CheckUninit( void *_seg, void *dummy )
/************************************************/
{
    segnode *seg = _seg;

    dummy = dummy;
    if( (seg->info & SEG_LXDATA_SEEN) == 0 ) {
        seg->entry->isuninit = true;
        if( seg->entry->u1.vm_ptr ) {
            ReleaseInfo( seg->entry->u1.vm_ptr );
            seg->entry->u1.vm_ptr = 0;
        }
    }
}

unsigned long OMFPass1( void )
/***********************************/
// do pass 1 for OMF object files
{
    unsigned long retval;

    PermStartMod( CurrMod );
    if( LinkState & (HAVE_MACHTYPE_MASK & ~HAVE_I86_CODE) ) {
        LnkMsg( WRN+MSG_MACHTYPE_DIFFERENT, "s", CurrMod->f.source->file->name);
    } else {
        LinkState |= HAVE_I86_CODE;
    }
    CurrMod->omfdbg = OMF_DBG_CODEVIEW; // Assume MS style LINNUM records
    retval = ProcObj( CurrMod->f.source, CurrMod->location, &Pass1Cmd );
    IterateNodelist( SegNodes, CheckUninit, NULL );
    ResolveComdats();
    return( retval );
}

static void Pass1Cmd( byte cmd )
/******************************/
/* Process an object record for pass 1 */
{
    bool isstatic;

    isstatic = false;
    switch( cmd ) {
    case CMD_THEADR:
        ProcTHEADR();
        break;
    case CMD_COMENT:
        Comment();
        break;
    case CMD_LLNAME:
    case CMD_LNAMES:
        AddNames();
        break;
    case CMD_SEGD32:
        ObjFormat |= FMT_MS_386;
    case CMD_SEGDEF:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ProcSegDef();
        break;
    case CMD_STATIC_PUBD32:
        ObjFormat |= FMT_MS_386;
    case CMD_STATIC_PUBDEF:
        ProcPubdef( true );
        break;
    case CMD_PUBD32:
        ObjFormat |= FMT_MS_386;
    case CMD_PUBDEF:
        ProcPubdef( false );
        break;
    case CMD_STATIC_EXTDEF:
    case CMD_STATIC_EXTD32:
        isstatic = true;
    case CMD_EXTDEF:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        UseSymbols( isstatic, false );
        break;
    case CMD_CEXTDF:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        UseSymbols( false, true );
        break;
    case CMD_GRPDEF:
        DefineGroup();
        break;
    case CMD_LINN32:
        ObjFormat |= FMT_MS_386;
    case CMD_LINNUM:
        if (CurrMod->omfdbg == OMF_DBG_CODEVIEW)
            ProcLinnum();
        break;
    case CMD_LINS32:
        ObjFormat |= FMT_MS_386;
    case CMD_LINSYM:
        ProcLinsym();
        break;
    case CMD_STATIC_COMDEF:
        isstatic = true;
    case CMD_COMDEF:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ProcComdef( isstatic );
        break;
    case CMD_COMD32:
        ObjFormat |= FMT_MS_386;
    case CMD_COMDAT:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ProcComdat();
        break;
    case CMD_LEDA32:
        ObjFormat |= FMT_MS_386;
    case CMD_LEDATA:
        ProcLxdata( false );
        break;
    case CMD_LIDA32:
        ObjFormat |= FMT_MS_386;
    case CMD_LIDATA:
        ProcLxdata( true );
        break;
    case CMD_FIXU32:
        ObjFormat |= FMT_MS_386;
    case CMD_FIXUP:         /* count the fixups for each seg_leader */
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        DoRelocs();
        ObjFormat &= ~FMT_UNSAFE_FIXUPP;
        break;
    case CMD_MODE32:
        ObjFormat |= FMT_MS_386;
    case CMD_MODEND:
        ProcModuleEnd();
        break;
    case CMD_ALIAS:
        ProcAlias();
        break;
    case CMD_BAKP32:
        ObjFormat |= FMT_MS_386;
    case CMD_BAKPAT:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ProcBakpat();
        break;
    case CMD_NBKP32:
        ObjFormat |= FMT_MS_386;
    case CMD_NBKPAT:
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ProcNbkpat();
        break;
    case CMD_VERNUM:
    case CMD_VENDEXT:
    case CMD_LOCSYM:
    case CMD_TYPDEF:
    case CMD_DEBSYM:
    case CMD_BLKDEF:
    case CMD_BLKD32:
    case CMD_BLKEND:
    case CMD_BLKE32:
        /* ignore the Intel debugging records */
        break;
    case CMD_RHEADR:
    case CMD_REGINT:
    case CMD_REDATA:
    case CMD_RIDATA:
    case CMD_OVLDEF:
    case CMD_ENDREC:
    case CMD_LHEADR:
    case CMD_PEDATA:
    case CMD_PIDATA:
    case CMD_LIBHED:
    case CMD_LIBNAM:
    case CMD_LIBLOC:
    case CMD_LIBDIC:
        LnkMsg( LOC_REC+WRN+MSG_REC_NOT_DONE, "x", cmd );
        break;
    default:
        CurrMod->f.source->file->flags |= INSTAT_IOERR;
        LnkMsg( LOC_REC+ERR+MSG_BAD_REC_TYPE, "x", cmd );
        break;
    }
}

bool IsOMF( file_list *list, unsigned long loc )
/**********************************************/
{
    byte        *rec;

    rec = CacheRead( list, loc, sizeof(unsigned_8) );
    return( rec != NULL && *rec == CMD_THEADR );
}

char *GetOMFName( file_list *list, unsigned long *loc )
/*****************************************************/
{
    obj_record  *rec;
    char        *name;
    unsigned    len;

    rec = CacheRead( list, *loc, sizeof(obj_record) );
    if( rec == NULL )
        return( NULL );
    *loc += sizeof( obj_record );
    len = rec->length;
    name = CacheRead( list, *loc, rec->length );
    *loc += len;
    if( name == NULL )
        return( NULL );
    len = *(unsigned char *)name;        // get actual name length
    return( ChkToString( name + 1, len ) );
}

void OMFSkipObj( file_list *list, unsigned long *loc )
/****************************************************/
{
    *loc = ProcObj( list, *loc, NULL );
}

static void ProcTHEADR( void )
/****************************/
{
    char    name[256];
    int     sym_len;

    if( CurrMod->omfdbg == OMF_DBG_CODEVIEW ) {
        sym_len = *ObjBuff++;
        if( sym_len == 0 ) {
            BadObject();
        }
        memcpy( name, ObjBuff, sym_len );
        name[sym_len] = '\0';
    }
}

static void LinkDirective( void )
/*******************************/
{
    unsigned        directive;
    lib_priority    priority;
    segnode         *seg;

    directive = *ObjBuff++;
    switch( directive ) {
    case LDIR_DEFAULT_LIBRARY:
        if( ObjBuff < EOObjRec ) {
            priority = *ObjBuff++ - '0';
            AddCommentLib( (char *)ObjBuff, EOObjRec - ObjBuff, priority );
        }
        break;
    case LDIR_SOURCE_LANGUAGE:
        DBIP1Source( ObjBuff, EOObjRec );
        break;
    case LDIR_VF_PURE_DEF:
        ProcVFTableRecord( true );
        break;
    case LDIR_VF_TABLE_DEF:
        ProcVFTableRecord( false );
        break;
    case LDIR_VF_REFERENCE:
        ProcVFReference();
        break;
    case LDIR_PACKDATA:
        if( (LinkFlags & PACKDATA_FLAG) == 0 ) {
            PackDataLimit = _GetU32UN( ObjBuff );
            LinkFlags |= PACKDATA_FLAG;
        }
        break;
    case LDIR_OPT_FAR_CALLS:
        seg = (segnode *)FindNode( SegNodes, GetIdx() );
        seg->entry->canfarcall = true;
        seg->entry->iscode = true;
        break;
    case LDIR_FLAT_ADDRS:
        CurrMod->modinfo |= MOD_FLATTEN_DBI;
        break;
    case LDIR_OPT_UNSAFE:
        ObjFormat |= FMT_UNSAFE_FIXUPP;
        break;
    }
}

#define EXPDEF_ORDINAL  0x80

static void ProcExportKeyword( void )
/***********************************/
{
    unsigned_8  flags;
    length_name intname;
    length_name expname;
    ordinal_t   ordinal;

    flags = *ObjBuff++;
    expname.len = *ObjBuff++;
    expname.name = (char *)ObjBuff;
    ObjBuff += expname.len;
    intname.len = *ObjBuff++;
    intname.name = (char *)ObjBuff;
    ObjBuff += intname.len;
    ordinal = 0;
    if( flags & EXPDEF_ORDINAL ) {
        ordinal = GET_U16_UN(ObjBuff);
    }
    HandleExport( &expname, &intname, flags, ordinal );
}

static void ProcImportKeyword( void )
/***********************************/
{
    length_name intname;
    length_name modname;
    length_name extname;
    unsigned_8  info;

    info = *ObjBuff++;
    intname.len = *ObjBuff++;
    intname.name = (char *)ObjBuff;
    ObjBuff += intname.len;
    modname.len = *ObjBuff++;
    modname.name = (char *)ObjBuff;
    ObjBuff += modname.len;
    if( info == DLL_RELOC_NAME ) {
        if( *ObjBuff == 0 ) {   /* use internal name */
            HandleImport( &intname, &modname, &intname, NOT_IMP_BY_ORDINAL );
        } else {
            extname.len = *ObjBuff++;
            extname.name = (char *)ObjBuff;
            ObjBuff += extname.len;
            HandleImport( &intname, &modname, &extname, NOT_IMP_BY_ORDINAL );
        }
    } else {
        HandleImport(&intname, &modname, &extname, GET_U16_UN(ObjBuff) );
    }
}


static void DoMSOMF( void )
/***********************************/
/* Figure out debug info type and handle it accordingly later. */
{
    if( ObjBuff == EOObjRec )
        CurrMod->omfdbg = OMF_DBG_CODEVIEW;    /* Assume MS style */
    else {
//        unsigned_8  version;

        /* version = * */ ObjBuff++;
        if( ObjBuff[0] == 'C' && ObjBuff[1] == 'V' ) {
            CurrMod->omfdbg = OMF_DBG_CODEVIEW;
        } else if( ObjBuff[0] == 'H' && ObjBuff[1] == 'L' ) {
            CurrMod->omfdbg = OMF_DBG_HLL;
        } else {
            CurrMod->omfdbg = OMF_DBG_UNKNOWN;
        }
    }
}

static void Comment( void )
/*************************/
/* Process a comment record. */
{
    unsigned char       attribute;
    unsigned char       class;
    int                 proc;
    unsigned char       which;

    attribute = *ObjBuff++;
    class = *ObjBuff++;

    switch( class ) {
    case CMT_DLL_ENTRY:
        which = *ObjBuff++;
        switch( which ) {
        case MOMF_IMPDEF:
        case MOMF_EXPDEF:
            if( SeenDLLRecord() ) {
                if( which == MOMF_EXPDEF ) {
                    ProcExportKeyword();
                } else {
                    ProcImportKeyword();
                }
            }
            break;
        case MOMF_PROT_LIB:
            if( FmtData.type & MK_WINDOWS ) {
                FmtData.u.os2.is_private_dll = true;
            }
            break;
        }
        break;
    case CMT_WAT_PROC_MODEL:
    case CMT_MS_PROC_MODEL:
        proc = *ObjBuff - '0';
        if( proc > FmtData.cpu_type )
            FmtData.cpu_type = proc;
        break;
    case CMT_DOSSEG:
        LinkState |= DOSSEG_FLAG;
        break;
    case CMT_DEFAULT_LIBRARY:
        AddCommentLib( (char *)ObjBuff, EOObjRec - ObjBuff, LIB_PRIORITY_MAX - 2 );
        break;
    case CMT_LINKER_DIRECTIVE:
        LinkDirective();
        break;
    case CMT_WKEXT:
        DoLazyExtdef( true );
        break;
    case CMT_LZEXT:
        DoLazyExtdef( false );
        break;
    case CMT_EASY_OMF:
        if( memcmp( ObjBuff, EASY_OMF_SIGNATURE, 5 ) == 0 ) {
            ObjFormat |= FMT_EASY_OMF;
        }
        break;
    case CMT_SOURCE_NAME:
        DBIComment();
        break;
    case CMT_MS_OMF:
        DoMSOMF();
        break;
    case 0x80:      /* Code Gen used to put bytes out in wrong order */
        if( attribute == CMT_SOURCE_NAME ) {    /* no longer generated */
            DBIComment();
        } else if( attribute == CMT_LINKER_DIRECTIVE ) {  /* linker directive */
            LinkDirective();
        }
        break;
    }
}

static void ProcAlias( void )
/***************************/
/* process a symbol alias directive */
{
    char        *alias;
    unsigned    aliaslen;
    unsigned    targetlen;
    symbol      *sym;

    for( ; ObjBuff < EOObjRec; ObjBuff += targetlen ) {
        aliaslen = *ObjBuff++;
        alias = (char *)ObjBuff;
        ObjBuff += aliaslen;
        targetlen = *ObjBuff++;
        sym = SymOp( ST_FIND | ST_NOALIAS, alias, aliaslen );
        if( sym == NULL || (sym->info & SYM_DEFINED) == 0 ) {
            MakeSymAlias( alias, aliaslen, (const char *)ObjBuff, targetlen );
        }
    }
}

static void ProcModuleEnd( void )
/*******************************/
{
    byte        frame;
    byte        target;
    unsigned    targetidx;
    segnode     *seg;
    extnode     *ext;
    bool        hasdisp;

    if( StartInfo.user_specd )
        return;
    target = *ObjBuff++;
    if( target & 0x40 ) {
        if( ObjBuff >= EOObjRec )               /* CSet/2 stupidity */
            return;
        target = *ObjBuff++;
        frame = (target >> 4) & 0x7;
        hasdisp = (target & 0x4) == 0;
        target &= 0x3;
        if( frame <= 2 ) {              /* frame requires an index */
            SkipIdx();
        }
        targetidx = GetIdx();
        switch( target ) {
        case TARGET_SEGWD:
            if( StartInfo.type != START_UNDEFED ) {
                LnkMsg( LOC+MILD_ERR+MSG_MULT_START_ADDRS, "12", StartInfo.mod->f.source->file->name, StartInfo.mod->name );
                return;                 // <-------- NOTE: premature return
            }
            seg = (segnode *)FindNode( SegNodes, targetidx );
            StartInfo.type = START_IS_SDATA;
            StartInfo.targ.sdata= seg->entry;
            if( (seg->info & SEG_CODE) && (LinkFlags & STRIP_CODE) ) {
                RefSeg( (segdata *)seg->entry );
            }
            StartInfo.mod = CurrMod;
            break;
        case TARGET_EXTWD:
            ext = (extnode *)FindNode( ExtNodes, targetidx );
            SetStartSym( ext->entry->name );
            break;
        case TARGET_ABSWD:
        case TARGET_GRPWD:
            BadObject();        // no one does these, right???
            break;
        }
        if( hasdisp ) {
            if( ObjFormat & FMT_32BIT_REC ) {
                _TargU32toHost( _GetU32UN( ObjBuff ), StartInfo.off );
            } else {
                _TargU16toHost( _GetU16UN( ObjBuff ), StartInfo.off );
            }
        }
    }
}

static void AddNames( void )
/**************************/
/* Process NAMES record */
{
    int                 name_len;
    list_of_names       **entry;

    DEBUG(( DBG_OLD, "AddNames()" ));
    for( ; ObjBuff < EOObjRec; ObjBuff += name_len ) {
        name_len = *ObjBuff++;
        entry = AllocNode( NameNodes );
        *entry = MakeListName( (char *)ObjBuff, name_len );
    }
}

static void ProcSegDef( void )
/****************************/
/* process a segdef record */
{
    segdata             *sdata;
    segnode             *snode;
    list_of_names       *clname;
    list_of_names       *name;
    byte                acbp;
    unsigned            comb;

    DEBUG(( DBG_OLD, "ProcSegDef()" ));
    sdata = AllocSegData();
    acbp = *ObjBuff++;
    comb = (acbp >> 2) & 7;
    if( comb == COMB_INVALID || comb == COMB_BAD ) {
        sdata->combine = COMBINE_INVALID;
    } else if( comb == COMB_COMMON ) {
        sdata->combine = COMBINE_COMMON;
    } else {
        sdata->combine = COMBINE_ADD;
    }
    sdata->align = OMFAlignTab[acbp >> 5];
    if( ObjFormat & FMT_EASY_OMF ) {   // set USE_32 flag.
        sdata->is32bit = true;
    } else if( acbp & 1 ) {
        sdata->is32bit = true;
    }
    switch( acbp >> 5 ) {
    case ALIGN_ABS:
        sdata->isabs = true;
        _TargU16toHost( _GetU16UN( ObjBuff ), sdata->frame );
        ObjBuff += sizeof( unsigned_16 ) + 1;
        break;
    case ALIGN_LTRELOC:
// in 32 bit object files, ALIGN_LTRELOC is actually ALIGN_4KPAGE
        if( (ObjFormat & FMT_32BIT_REC) || (FmtData.type & MK_RAW) )
            break;
        sdata->align = OMFAlignTab[ALIGN_PARA];
        ObjBuff += 5;   /*  step over ltldat, max_seg_len, grp_offs fields */
        break;
    }
    if( ObjFormat & FMT_32BIT_REC ) {
        if( acbp & 2 ) {
            BadObject();        // we can't handle 4 GB segments properly
            return;
        }
        _TargU32toHost( _GetU32UN( ObjBuff ), sdata->length );
        ObjBuff += sizeof( unsigned_32 );
    } else {
        if( acbp & 2 ) {
            sdata->length = 65536;          // 64k segment
        } else {
            _TargU16toHost( _GetU16UN( ObjBuff ), sdata->length );
        }
        ObjBuff += sizeof( unsigned_16 );
    }
    name = FindName( GetIdx() );
    sdata->u.name = name->name;
    clname = FindName( GetIdx() );
    if( ObjFormat & FMT_EASY_OMF ) {
        SkipIdx();                          // skip overlay name index
        if( ObjBuff < EOObjRec ) {          // the optional attribute field present
            if( (*ObjBuff & 0x4) == 0 ) {       // if USE32 bit not set;
                sdata->is32bit = false;
            }
        }
    }
    sdata->iscode = IsCodeClass( clname->name, strlen(clname->name) );
    snode = AllocNode( SegNodes );
    snode->entry = sdata;
    AllocateSegment( snode, clname->name );
}

static void DefineGroup( void )
/*****************************/
/* Define a group. */
{
    int                 num_segs;
    byte                *anchor;
    segnode             *seg;
    list_of_names       *grp_name;
    grpnode             *newnode;
    group_entry         *group;

    grp_name = FindName( GetIdx() );
    DEBUG(( DBG_OLD, "DefineGroup() - %s", grp_name->name ));
    anchor = ObjBuff;
    num_segs = 0;
    while( ObjBuff < EOObjRec ) {
        if( *ObjBuff++ != GRP_SEGIDX ) {
            BadObject();
            return;
        }
        SkipIdx();/*  skip segment index */
        ++num_segs;
    }
    newnode = AllocNode( GrpNodes );
    group = SearchGroups( grp_name->name );
    if( group == NULL ) {
        if( num_segs == 0 ) {
            newnode->entry = NULL;
            return;                     // NOTE: premature return!
        }
        group = AllocGroup( grp_name->name, &Groups );
    }
    newnode->entry = group;
    ObjBuff = anchor;
    while( ObjBuff < EOObjRec ) {
        ObjBuff++;
        seg = (segnode *)FindNode( SegNodes, GetIdx() );
        AddToGroup( group, seg->entry->u.leader );
    }
}

static void ProcPubdef( bool static_sym )
/***************************************/
/* Define symbols. */
{
    symbol          *sym;
    char            *sym_name;
    segnode         *seg;
    offset          off;
    unsigned        sym_len;
    unsigned_16     frame;
    unsigned_16     segidx;

    DEBUG(( DBG_OLD, "ProcPubdef" ));
    SkipIdx();
    segidx = GetIdx();
    if( segidx != 0 ) {
        seg = (segnode *) FindNode( SegNodes, segidx );
        frame = 0;
    } else {
        seg = NULL;
        _TargU16toHost( _GetU16UN( ObjBuff ), frame );
        ObjBuff += sizeof( unsigned_16 );
    }
    DEBUG(( DBG_OLD, "segidx = %d", segidx ));
    while( ObjBuff < EOObjRec ) {
        sym_len = *ObjBuff++;
        if( sym_len == 0 ) {
            BadObject();
        }
        sym_name = (char *)ObjBuff;
        ObjBuff += sym_len;
        if( ObjFormat & FMT_32BIT_REC ) {
            _TargU32toHost( _GetU32UN( ObjBuff ), off );
            ObjBuff += sizeof( unsigned_32 );
        } else {
            _TargU16toHost( _GetU16UN( ObjBuff ), off );
            ObjBuff += sizeof( unsigned_16 );
        }
        if( static_sym ) {
            sym = SymOp( ST_DEFINE_SYM | ST_STATIC, sym_name, sym_len );
        } else {
            sym = SymOp( ST_DEFINE_SYM, sym_name, sym_len );
        }
        DefineSymbol( sym, seg, off, frame );
        SkipIdx();   /* skip type index */
    }
}

static void DoLazyExtdef( bool isweak )
/*************************************/
/* handle the lazy and weak extdef comments */
{
    extnode     *ext;
    symbol      *sym;
    unsigned    idx;

    while( ObjBuff < EOObjRec ) {
        ext = (extnode *) FindNode( ExtNodes, GetIdx() );
        sym = ext->entry;
        ext->isweak = true;
        idx = GetIdx();
        ext = (extnode *) FindNode( ExtNodes, idx );
        DefineLazyExtdef( sym, ext->entry, isweak );
    }
}

static void *GetVFListStart( void )
/**********************************/
{
    return( ObjBuff );
}

static void SetVFListStart( void *start )
/****************************************/
{
    ObjBuff = start;
}

static bool EndOfVFList( void )
/*****************************/
{
    return( ObjBuff >= EOObjRec );
}

static char *GetVFListName( void )
/*********************************/
{
    list_of_names       *lname;

    lname = FindName( GetIdx() );
    return( lname->name );
}

static void ProcVFTableRecord( bool ispure )
/******************************************/
// process the watcom virtual function table information extension
{
    extnode     *ext;
    symbol      *sym;
    vflistrtns  rtns;

    if( (LinkFlags & VF_REMOVAL) == 0 )
        return;
    ext = (extnode *) FindNode( ExtNodes, GetIdx() );
    sym = ext->entry;
    ext->isweak = true;
    ext = (extnode *) FindNode( ExtNodes, GetIdx() );
    rtns.getstart = GetVFListStart;
    rtns.setstart = SetVFListStart;
    rtns.isend = EndOfVFList;
    rtns.getname = GetVFListName;
    DefineVFTableRecord( sym, ext->entry, ispure, &rtns );
}

static void ProcVFReference( void )
/*********************************/
/* process a vftable reference record */
{
    extnode             *ext;
    segnode             *seg;
    symbol              *sym;
    list_of_names       *lname;
    unsigned            index;

    if( (LinkFlags & VF_REMOVAL) == 0 )
        return;
    index = GetIdx();
    if( index == 0 ) {
        LnkMsg( WRN+LOC+MSG_NOT_COMPILED_VF_ELIM, NULL );
        return;
    }
    ext = (extnode *) FindNode( ExtNodes, index );
    if( LinkFlags & STRIP_CODE ) {
        if( *ObjBuff == 0 ) {   /* it is a comdat index */
            ObjBuff++;
            lname = FindName( GetIdx() );
            sym = FindISymbol( lname->name );
            if( sym == NULL ) {
                sym = MakeWeakExtdef( lname->name, NULL );
            }
            DefineVFReference( sym, ext->entry, true );
        } else {                /* it's a seg idx */
            seg = (segnode *)FindNode( SegNodes, GetIdx() );
            DefineVFReference( seg, ext->entry, false );
        }
    }
    if( (ext->entry->info & SYM_DEFINED) == 0 ) {
        ext->entry->info |= SYM_VF_MARKED;
    }
}

static void UseSymbols( bool static_sym, bool iscextdef )
/*******************************************************/
/* Define all external references. */
{
    list_of_names       *lnptr;
    unsigned            sym_len;
    extnode             *newnode;
    symbol              *sym;
    sym_flags           flags;

    DEBUG(( DBG_OLD, "UseSymbols()" ));
    flags = ST_CREATE | ST_REFERENCE;
    if( static_sym ) {
        flags |= ST_STATIC;
    }
    while( ObjBuff < EOObjRec ) {
        if( iscextdef ) {
            lnptr = FindName( GetIdx() );
            sym = RefISymbol( lnptr->name );
        } else {
            sym_len = *ObjBuff++;
            if( sym_len == 0 ) {
                BadObject();
            }
            sym = SymOp( flags, (char *)ObjBuff, sym_len );
            ObjBuff += sym_len;
        }
        newnode = AllocNode( ExtNodes );
        newnode->entry = sym;
        newnode->isweak = false;
        DefineReference( sym );
        SkipIdx();/*  skip type index */
    }
}

void SkipIdx( void )
/*************************/
/* skip the index */
{
    if( *ObjBuff++ & IS_2_BYTES ) {
        ObjBuff++;
    }
}

unsigned_16 GetIdx( void )
/*******************************/
/* Get an index. */
{
    unsigned_16 index;

    index = *ObjBuff++;
    if( index & IS_2_BYTES ) {
        index = (index & 0x7f) * 256 + *ObjBuff++;
    }
    return( index );
}

list_of_names *FindName( unsigned_16 index )
/**************************************************/
/* Find name of specified index. */
{
    return( *((list_of_names **)FindNode( NameNodes, index ) ) );
}

static void ProcLxdata( bool islidata )
/*************************************/
/* process ledata and lidata records */
{
    segnode     *seg;
    unsigned_32 obj_offset;

    seg = (segnode *) FindNode( SegNodes, GetIdx() );
    seg->entry->u.leader->info |= SEG_LXDATA_SEEN;
    seg->info |= SEG_LXDATA_SEEN;
    if( ObjFormat & FMT_32BIT_REC ) {
        _TargU32toHost( _GetU32UN( ObjBuff ), obj_offset );
        ObjBuff += sizeof( unsigned_32 );
    } else {
        _TargU16toHost( _GetU16UN( ObjBuff ), obj_offset );
        ObjBuff += sizeof( unsigned_16 );
    }
#ifdef _DEVELOPMENT
    if( stricmp( seg->entry->u.leader->segname, "_BSS" ) == 0 ) {
        LnkMsg( LOC_REC+ERR+MSG_INTERNAL, "s", "Initialized BSS found" );
    }
#endif
    GetObject( seg->entry, obj_offset, islidata );
}

static void ProcLinnum( void )
/****************************/
/* do some processing for the linnum record */
{
    segnode     *seg;
    bool        is32bit;

    SkipIdx();          /* don't need the group idx */
    seg = (segnode *) FindNode( SegNodes, GetIdx() );
    if( seg->info & SEG_DEAD )                  /* ignore dead segments */
        return;
    is32bit = (ObjFormat & FMT_32BIT_REC) != 0;
    DBIAddLines( seg->entry, ObjBuff, EOObjRec - ObjBuff, is32bit );
}

static byte *ProcIDBlock( virt_mem *dest, byte *buffer, unsigned_32 iterate )
/***************************************************************************/
/* Process logically iterated data blocks. */
{
    byte            len;
    byte            *anchor;
    unsigned_16     count;
    unsigned_16     inner;
    unsigned_32     rep;

    if( iterate == 0 ) {  // no iterations, so abort.
        return( EOObjRec );
    }
    _TargU16toHost( _GetU16UN( buffer ), count );
    buffer += sizeof( unsigned_16 );
    if( count == 0 ) {
        len = *buffer;
        ++buffer;
        do {
            PutInfo( *dest, buffer, len );
            *dest += len;
        } while( --iterate != 0 );
        buffer += len;
    } else {
        anchor = buffer;
        if( ObjFormat & FMT_MS_386 ) {
            do {
                buffer = anchor;
                inner = count;
                do {
                    _TargU32toHost( _GetU32UN(buffer), rep );
                    buffer += sizeof(unsigned_32);
                    buffer = ProcIDBlock( dest, buffer, rep );
                } while( --inner != 0 );
            } while( --iterate != 0 );
        } else {
            do {
                buffer = anchor;
                inner = count;
                do {
                    _TargU16toHost( _GetU16UN(buffer), rep );
                    buffer += sizeof(unsigned_16);
                    buffer = ProcIDBlock( dest, buffer, rep );
                } while( --inner != 0 );
            } while( --iterate != 0 );
        }
    }
    return( buffer );
}

static void DoLIData( virt_mem start, byte *data, unsigned size )
/***************************************************************/
/* Expand logically iterated data. */
{
    unsigned_32 rep;
    byte        *end_data;

    end_data = data + size;
    for( ; data < end_data; data = ProcIDBlock( &start, data, rep ) ) {
        if( ObjFormat & FMT_MS_386 ) {
            _TargU32toHost( _GetU32UN( data ), rep );
            data += sizeof( unsigned_32 );
        } else {
            _TargU16toHost( _GetU16UN( data ), rep );
            data += sizeof( unsigned_16 );
        }
    }
}

static void GetObject( segdata *seg, unsigned_32 obj_offset, bool lidata )
/*************************************************************************/
/* Load object code. */
{
    unsigned    size;
    virt_mem    start;

    if( seg->isdead || seg->isabs ) {   /* ignore dead or abs segments */
        ObjFormat |= FMT_IGNORE_FIXUPP; /* and any corresponding fixupps */
        return;
    }
    ObjFormat &= ~(FMT_IGNORE_FIXUPP|FMT_IS_LIDATA);
    if( lidata ) {
        ObjFormat |= FMT_IS_LIDATA;
    }
    if( ObjBuff != EOObjRec ) {
        size = EOObjRec - ObjBuff;
        start = seg->u1.vm_ptr + obj_offset;
        if( lidata ) {
            DoLIData( start, ObjBuff, size );
        } else {
            if( size + obj_offset > seg->length ) {
                LnkMsg( LOC_REC+FTL+MSG_OBJ_FILE_ATTR, NULL );
            }
            PutInfo( start, ObjBuff, size );
        }
    }
    SetCurrSeg( seg, obj_offset, NULL );
}
