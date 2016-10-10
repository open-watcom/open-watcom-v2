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
* Description:  Utilities for processing creation of NE format files,
*               used by 16-bit OS/2 and Windows.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "wio.h"
#include "linkstd.h"
#include "ring.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "reloc.h"
#include "exeos2.h"
#include "exeflat.h"
#include "exepe.h"
#include "exedos.h"
#include "loadfile.h"
#include "specials.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "objcalc.h"
#include "fileio.h"
#include "objfree.h"
#include "objpass1.h"
#include "dbgall.h"
#include "objstrip.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "loadpe.h"
#include "impexp.h"
#include "wres.h"
#include "rcstrblk.h"
#include "rcstr.h"
#include "loados2.h"

#include "clibext.h"

#define STUB_ALIGN 16

typedef struct FullResourceRecord {
    struct FullResourceRecord   *Next;
    struct FullResourceRecord   *Prev;
    resource_record             Info;
} FullResourceRecord;

typedef struct FullTypeRecord {
    struct FullTypeRecord   *Next;
    struct FullTypeRecord   *Prev;
    FullResourceRecord      *Head;
    FullResourceRecord      *Tail;
    resource_type_record    Info;
} FullTypeRecord;

typedef struct ExeResDir {
    uint_16             ResShiftCount;
    uint_16             NumTypes;
    uint_16             NumResources;
    uint_16             TableSize;
    FullTypeRecord      *Head;
    FullTypeRecord      *Tail;
} ExeResDir;

typedef struct ResTable {
    ExeResDir       Dir;
    StringsBlock    Str;
} ResTable;


static  uint_8          DosStub[] = {
        0x4D, 0x5A, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD,
        0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21 };

static unsigned long WriteOS2Relocs( group_entry *group )
/*******************************************************/
/* write all relocs associated with group to the file */
{
    unsigned long relocsize;
    unsigned long relocnum;

    relocsize = RelocSize( group->g.grp_relocs );
    relocnum = relocsize / sizeof( os2_reloc_item );
    if( relocnum == 0 )
        return( 0 );
    WriteLoad( &relocnum, 2 );
    DumpRelocList( group->g.grp_relocs );
    return( relocsize );
}

static void WriteOS2Data( unsigned_32 stub_len, os2_exe_header *exe_head )
/************************************************************************/
/* copy code from extra memory to loadfile. */
{
    group_entry         *group;
    unsigned            group_num;
    unsigned long       off;
    segment_record      segrec;
    unsigned_32         seg_addr;
    unsigned long       relocsize;

    DEBUG(( DBG_BASE, "Writing data" ));

    group_num = 0;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        segrec.info = group->segflags;
        // write segment
        segrec.min = MAKE_EVEN( group->totalsize );
        segrec.size = MAKE_EVEN( group->size );
        if( segrec.size != 0 ) {
            off = NullAlign( 1 << FmtData.u.os2.segment_shift );
            seg_addr = off >> FmtData.u.os2.segment_shift;
            if( seg_addr > 0xffff ) {
                LnkMsg( ERR+MSG_ALIGN_TOO_SMALL, NULL );
            };
            segrec.address = (unsigned_16)seg_addr;
            WriteGroupLoad( group );
            NullAlign( 2 );         // segment must be even length
            relocsize = WriteOS2Relocs( group );
            if( relocsize != 0 ) {
                segrec.info |= SEG_RELOC;
            }

        } else {
            segrec.address = 0;
        }

        // write to segment table
        off = PosLoad();
        SeekLoad( exe_head->segment_off + stub_len +
                            group_num * sizeof( segment_record ) );
        WriteLoad( &segrec, sizeof( segment_record ) );
        SeekLoad( off );
        group_num++;
    }
}

static void AddLLItemAtEnd( void *head, void *tail, void *item )
/**************************************************************/
{
    struct dllist {
        struct dllist *next;
        struct dllist *prev;
        // SomeDataType some_data;
    } **h, **t, *i;

    h = head; t = tail; i = item;

    i->next = NULL;
    if( *h == NULL ) {
        *h = *t = i;
        i->next = NULL; // prev ptr
    } else {
        i->prev = *t; // prev ptr
        (*t)->next = i;  // next ptr
        *t = i;
    }
}

static uint_16 findResOrTypeName( ResTable *restab, WResID *name )
/******************************************************************/
{
    uint_16     name_id;
    int_32      str_offset;

    if( name->IsName ) {
        str_offset = StringBlockFind( &restab->Str, &name->ID.Name );
        if( str_offset == -1 ) {
            name_id = 0;
        } else {
            name_id = str_offset + restab->Dir.TableSize;
        }
    } else {
        name_id = name->ID.Num | 0x8000;
    }

    return( name_id );
}

static FullTypeRecord *addExeTypeRecord( ResTable *restab,
                            WResTypeInfo *type )
/**********************************************************/
{
    FullTypeRecord      *exe_type;

    _ChkAlloc( exe_type, sizeof( FullTypeRecord ) );

    exe_type->Info.reserved = 0;
    exe_type->Info.num_resources = type->NumResources;
    exe_type->Info.type = findResOrTypeName( restab, &(type->TypeName) );
    exe_type->Head = NULL;
    exe_type->Tail = NULL;
    exe_type->Next = NULL;
    exe_type->Prev = NULL;

    AddLLItemAtEnd( &(restab->Dir.Head), &(restab->Dir.Tail), exe_type );

    return( exe_type );
}

static void addExeResRecord( ResTable *restab, FullTypeRecord *type,
                            WResID *name, uint_16 mem_flags,
                            uint_16 exe_offset, uint_16 exe_length )
/********************************************************************/
{
    FullResourceRecord          *exe_res;

    _ChkAlloc( exe_res, sizeof( FullResourceRecord ) );
    exe_res->Info.offset = exe_offset;
    exe_res->Info.length = exe_length;
    exe_res->Info.flags = mem_flags;
    exe_res->Info.reserved = 0;
    exe_res->Info.name = findResOrTypeName( restab, name );
    exe_res->Next = NULL;
    exe_res->Prev = NULL;
    AddLLItemAtEnd( &(type->Head), &(type->Tail), exe_res );
}

static FullTypeRecord *findExeTypeRecord( ResTable *restab,
                            WResTypeInfo *type )
/***********************************************************/
{
    FullTypeRecord      *exe_type;
    StringItem16        *exe_type_name;

    for( exe_type = restab->Dir.Head; exe_type != NULL;
                exe_type = exe_type->Next ) {
        if( type->TypeName.IsName && (exe_type->Info.type & 0x8000) == 0 ) {
            /* if they are both names */
            exe_type_name = (StringItem16 *) ((char *) restab->Str.StringBlock +
                            (exe_type->Info.type - restab->Dir.TableSize));
            if( exe_type_name->NumChars == type->TypeName.ID.Name.NumChars
                && !memicmp( exe_type_name->Name, type->TypeName.ID.Name.Name,
                            exe_type_name->NumChars ) ) break;
        } else if( !(type->TypeName.IsName) && (exe_type->Info.type & 0x8000) ) {
            /* if they are both numbers */
            if( type->TypeName.ID.Num == (exe_type->Info.type & ~0x8000) ) {
                break;
            }
        }
    }
    if( exe_type == NULL ) {              /* this is a new type */
        exe_type = addExeTypeRecord( restab, type );
    }
    return( exe_type );
} /* findExeTypeRecord */

static void FreeResTable( ResTable *restab )
/*******************************************/
{
    FullTypeRecord              *type;
    FullTypeRecord              *type_next;
    FullResourceRecord          *res;
    FullResourceRecord          *res_next;

    for( type = restab->Dir.Head; type != NULL; type = type_next ) {
        type_next = type->Next;
        for( res = type->Head; res != NULL; res = res_next ) {
            res_next = res->Next;
            _LnkFree( res );
        }
        _LnkFree( type );
    }

    restab->Dir.Head = NULL;
    restab->Dir.Tail = NULL;
}

static void WriteResTable( ResTable *restab )
/*******************************************/
{
    FullTypeRecord      *exe_type;
    FullResourceRecord *exe_res;
    uint_16             zero;

    WriteLoad( &FmtData.u.os2.segment_shift, sizeof( uint_16 ) );
    for( exe_type = restab->Dir.Head; exe_type != NULL;
                                        exe_type = exe_type->Next) {
        WriteLoad( &(exe_type->Info), sizeof( resource_type_record ) );
        for( exe_res = exe_type->Head; exe_res != NULL;
                                        exe_res = exe_res->Next ) {
            WriteLoad( &(exe_res->Info) , sizeof( resource_record ));
        }
    }
    zero = 0;
    WriteLoad( &zero, sizeof( uint_16 ) );
    WriteLoad( restab->Str.StringBlock, restab->Str.StringBlockSize );
}

static void CopyResData( int resHandle, int len )
/***********************************************/
{
    char buff[512];

    for( ;len > 512; len -= 512 ) {
        QRead( resHandle, buff, 512, FmtData.resource );
        WriteLoad( buff, 512 );
    }
    QRead( resHandle, buff, len, FmtData.resource );
    WriteLoad( buff, len);
}

static void WriteOS2Resources( int reshandle, WResDir inRes, ResTable *outRes )
/*****************************************************************************/
{
    int shift_count = FmtData.u.os2.segment_shift;
    int align = 1 << shift_count;
    int outRes_off;
    WResDirWindow       wind;
    FullTypeRecord      *exe_type;
    WResResInfo         *res;
    WResLangInfo        *lang;

    if( inRes == NULL ) return;
    outRes_off = NullAlign(align) >> shift_count;
    /* walk through the WRes directory */
    exe_type = NULL;
    wind = WResFirstResource( inRes );
    while( !WResIsEmptyWindow( wind ) ) {
        lang = WResGetLangInfo( wind );

        if( WResIsFirstResOfType( wind ) ) {
            exe_type = findExeTypeRecord( outRes, WResGetTypeInfo( wind ) );
        }
        res = WResGetResInfo( wind );
        addExeResRecord( outRes, exe_type, &(res->ResName),
                        lang->MemoryFlags, outRes_off,
                        (lang->Length + align - 1) >> shift_count );
        QSeek( reshandle, lang->Offset, FmtData.resource );
        CopyResData( reshandle, lang->Length );
        NullAlign( align );
        outRes_off += (lang->Length + align - 1) >> shift_count;

        wind = WResNextResource( wind, inRes );
    }
    NullAlign(align);
}


static unsigned long WriteTabList( name_list *val, unsigned long *count,
                                   bool upper)
/***********************************************************************/
{
    name_list           *node;
    unsigned long       off;
    unsigned long       i;
    int                 j;

    i = 0;
    off = 0;
    for( node = val; node != NULL; node = node->next ) {
        ++i;
        WriteLoad( &(node->len), sizeof( unsigned char ) );  // NOTE:little endian
        if( upper ) {
            for( j = node->len-1; j >= 0; --j ) {
                node->name[j] = toupper( node->name[j] );
            }
        }
        WriteLoad( node->name, node->len );
        off += node->len + 1;
    }
    *count = i;
    return( off );
}

unsigned long ImportProcTable( unsigned long *count )
/**********************************************************/
{
    return( WriteTabList( FmtData.u.os2.imp_tab_list, count,
                          (LinkFlags & CASE_FLAG) == 0 ) );
}

unsigned long ImportModTable( unsigned long *count )
/*********************************************************/
{
    return( WriteTabList( FmtData.u.os2.mod_ref_list, count, false ) );
}

static unsigned long ImportNameTable( void )
/******************************************/
{
    unsigned long count;
    unsigned long size;

    PadLoad( 1 );
    size = 1;
    size += ImportProcTable( &count );
    size += ImportModTable( &count );
    return( size );
}

static unsigned long ModRefTable( void )
/**************************************/
/* count total number of groups */
{
    name_list           *node;
    name_list           *inode;
    unsigned long       nodenum;
    unsigned long       off;

    if( FmtData.type & MK_OS2_16BIT ) {
        off = 1;
        for( inode = FmtData.u.os2.imp_tab_list; inode != NULL;
                                                 inode = inode->next ) {
            off += inode->len + 1;
        }
    } else {
        off = 0;
    }
    nodenum = 0;
    for( node = FmtData.u.os2.mod_ref_list; node != NULL; node = node->next ) {
        WriteLoad( &off, sizeof( unsigned_16 ) );
        off += node->len + 1;
        nodenum++;
    }
    return( nodenum );
}

unsigned long ResNonResNameTable( bool dores )
/********************************************/
/* NOTE: this routine assumes INTEL byte ordering (in the use of namelen) */
{
    entry_export    *exp;
    unsigned        namelen;
    unsigned long   size;
    const char      *name;

    size = 0;
    if( dores ) {
        if( FmtData.u.os2.res_module_name != NULL ) {
            name = FmtData.u.os2.res_module_name;
            namelen = strlen( name );
        } else {
            name = GetBaseName( Root->outfile->fname, 0, &namelen );
        }
    } else {     /* in non-resident names table */
        if( FmtData.u.os2.description != NULL ) {
            name = FmtData.u.os2.description;
        } else if( FmtData.type & MK_OS2_16BIT ) {
            name = Root->outfile->fname;
        } else {
            name = "";
        }
        namelen = strlen( name );
    }
    if( dores || namelen > 0 ) {
        WriteLoad( &namelen, 1 );
        WriteLoad( name, namelen );
        PadLoad( 2 );
        size += namelen + 3;
    }
    if( dores && FmtData.u.os2.res_module_name != NULL ) {
        _LnkFree( FmtData.u.os2.res_module_name );
        FmtData.u.os2.res_module_name = NULL;
    }
    if( !dores && FmtData.u.os2.description != NULL ) {
        _LnkFree( FmtData.u.os2.description );
        FmtData.u.os2.description = NULL;
    }
    for( exp = FmtData.u.os2.exports; exp != NULL; exp = exp->next ) {
        if( !exp->isexported ) continue;
        if( exp->isanonymous ) continue;
        if( (dores && exp->isresident) || (!dores && !exp->isresident) ) {
            if( (LinkFlags & CASE_FLAG) == 0 ) {
                strupr( exp->name );
            }
            namelen = strlen( exp->name );
            WriteLoad( &namelen, 1 );
            WriteLoad( exp->name, namelen );
            WriteLoad( &(exp->ordinal), 2 );
            size += namelen + 3;
            if( !exp->isprivate ) {
                if( exp->impname != NULL ) {
                    AddImpLibEntry(exp->impname, exp->name, NOT_IMP_BY_ORDINAL);
                } else {
                    AddImpLibEntry( exp->sym->name, NULL, exp->ordinal );
                }
            }
        }
    }
    if( size > 0 ) {
        PadLoad( 1 );
        ++size;
    }
    return( size );
}


/*
 * NOTE: The routine DumpFlatEntryTable in LOADFLAT.C is very similar to this
 *       one, however there are a enough differences to preclude the use
 *       of one routine to dump both tables. Therefore any logic bugs that
 *       occur in this routine will likely have to be fixed in the other
 *       one as well.
 */
static unsigned long DumpEntryTable( void )
/*****************************************/
/* Dump the entry table to the file */
{
    entry_export    *start;
    entry_export    *place;
    entry_export    *prev;
    unsigned_16     prevord;
    unsigned long   size;
    unsigned        gap;
    unsigned        entries;
    bundle_prefix   prefix;
    union {
        fixed_record    f;
        movable_record  m;
    }               bundle_item;

    size = 0;
    start = FmtData.u.os2.exports;
    if( start != NULL ) {
        prevord = 0;
        for( place = start; place != NULL; ) {
            gap = place->ordinal - prevord;
            if( gap > 1 ) {  // fill in gaps in ordinals.
                gap--;       // fix 'off by 1' problem.
                prefix.number = 0xFF;
                prefix.type = 0x00;   // Null bundles.
                for( ; gap > 0xFF; gap -= 0xFF ) {
                    WriteLoad( &prefix, sizeof( bundle_prefix ) );
                    size += 2;
                }
                prefix.number = (unsigned_8) gap;
                WriteLoad( &prefix, sizeof( bundle_prefix ) );
                size += 2;
            }
            // now get a bundle of ordinals.
            entries = 1;
            prev = start = place;
            for( place = place->next; place != NULL; place = place->next ) {
                if( entries >= 0xff ) break;
                if( start->ismovable ) {
                    if( !place->ismovable )break;
                } else {
                    if( place->addr.seg != start->addr.seg ) break;
                }
                if( place->ordinal - prev->ordinal > 1 ) {
                    break;    // ordinal can't be put in this bundle.
                }
                entries++;
                prev = place;
            }
            if( start->ismovable ) {
                prefix.type = MOVABLE_ENTRY_PNT;
                bundle_item.m.reserved = 0x3fcd; /* int 0x3f */
                size += entries * sizeof( movable_record );
            } else {
                prefix.type = start->addr.seg;     // fixed segment records.
                size += entries * sizeof( fixed_record );
            }
            prevord = prev->ordinal;
            prefix.number = (unsigned_8) entries;
            WriteLoad( &prefix, sizeof( bundle_prefix ) );
            size += 2;
            for( ; entries > 0; --entries ) {
                bundle_item.f.info = (start->iopl_words << IOPL_WORD_SHIFT);
                if( start->isexported ) {
                    bundle_item.f.info |= ENTRY_EXPORTED;
                    if( FmtData.u.os2.flags & SHARABLE_DGROUP ) {
                        bundle_item.f.info |= ENTRY_SHARED;
                    }
                }
                if( start->ismovable ) {
                    if( start->addr.seg > 0xFF ) {
                        LnkMsg( WRN+MSG_BAD_MOVABLE_SEG_NUM, NULL );
                    }
                    bundle_item.m.entrynum = start->addr.seg;
                    bundle_item.m.entry = start->addr.off;
                    WriteLoad( &bundle_item, sizeof( movable_record ) );
                } else {
                    bundle_item.f.entry = start->addr.off;
                    WriteLoad( &bundle_item, sizeof( fixed_record ) );
                }
                start = start->next;
            }
        }
    }
    PadLoad( 2 );
    return( size + 2 );
}

void ChkOS2Data( void )
/**********************/
{
    SetSegFlags( (seg_flags *) FmtData.u.os2.os2_seg_flags );
    FmtData.u.os2.os2_seg_flags = NULL;
}

#define DEF_SEG_ON (SEG_PURE|SEG_READ_ONLY|SEG_CONFORMING|SEG_MOVABLE|SEG_DISCARD|SEG_RESIDENT|SEG_CONTIGUOUS|SEG_NOPAGE)
#define DEF_SEG_OFF (SEG_PRELOAD|SEG_INVALID)

static void CheckGrpFlags( void *_leader )
/****************************************/
{
    seg_leader     *leader = _leader;
    unsigned_16     sflags;

    sflags = leader->segflags;
    // if any of these flags are on, turn it on for the entire group.
    leader->group->segflags |= sflags & DEF_SEG_OFF;
    // if any of these flags off, make sure they are off in the group.
    leader->group->segflags &= sflags & DEF_SEG_ON | ~DEF_SEG_ON;
    if( (sflags & SEG_LEVEL_MASK) == SEG_LEVEL_2 ) {
        /* if any are level 2 then all have to be. */
        leader->group->segflags &= ~SEG_LEVEL_MASK;
        leader->group->segflags |= SEG_LEVEL_2;
    }
}

static void SetGroupFlags( void )
/*******************************/
// This goes through the groups, setting the flag word to be compatible with
// the flag words that are specified in the segments.
{
    group_entry     *group;

    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        group->segflags |= DEF_SEG_ON;
        Ring2Walk( group->leaders, CheckGrpFlags );
        /* for some insane reason, level 2 segments must be marked as
            movable */
        if( (group->segflags & SEG_LEVEL_MASK) == SEG_LEVEL_2 ) {
            group->segflags |= SEG_MOVABLE;
        }
    }
}

void ChkOS2Exports( void )
/*******************************/
// NOTE: there is a continue in this loop!
{
    symbol          *symptr;
    entry_export    *exp;
    group_entry     *group;
    unsigned        num_entries;

    SetGroupFlags();            // NOTE: there is a continue in this loop!
    num_entries = 0;
    for( exp = FmtData.u.os2.exports; exp != NULL; exp = exp->next ) {
        num_entries++;
        symptr = exp->sym;
        if( IS_SYM_ALIAS( symptr ) ) {
            symptr = UnaliasSym( ST_FIND, symptr );
            if( symptr == NULL || (symptr->info & SYM_DEFINED) == 0 ) {
                LnkMsg( ERR+MSG_EXP_SYM_NOT_FOUND, "s", exp->sym->name );
                continue;               // <----- DANGER weird control flow!
            } else if( exp->sym->info & SYM_WAS_LAZY ) {
                LnkMsg( WRN+MSG_EXP_SYM_NOT_FOUND, "s", exp->sym->name );
            }
            // Keep the import name. If an alias is exported, we want the
            // alias name in the import lib, not the substitute name
            if( exp->impname == NULL ) {
                exp->impname = ChkStrDup( exp->sym->name );
            }

            exp->sym = symptr;
        }
        if( (symptr->info & SYM_DEFINED) == 0 ) {
            LnkMsg( ERR+MSG_EXP_SYM_NOT_FOUND, "s", symptr->name );
        } else {
            exp->addr = symptr->addr;
            if( symptr->p.seg == NULL || IS_SYM_IMPORTED(symptr) ) {
                if( FmtData.type & MK_OS2_FLAT ) {
                    // MN: Create a forwarder - add a special flag?
                    // Currently DumpFlatEntryTable() in loadflat.c will
                    // recognize a forwarder by segment == 0xFFFF
                } else {
                    LnkMsg( ERR+MSG_CANT_EXPORT_ABSOLUTE, "S", symptr );
                }
            } else {
                group = symptr->p.seg->u.leader->group;
                if( FmtData.type & MK_OS2_FLAT ) {
                    exp->addr.off -= group->grp_addr.off;
                } else if( FmtData.type & MK_PE ) {
                    exp->addr.off += (group->linear - group->grp_addr.off);
                }
                if( group->segflags & SEG_MOVABLE ) {
                    exp->ismovable = true;
                }
            }
        }
    }   // NOTE: there is a continue in this loop!
    AssignOrdinals();    /* make sure all exports have ordinals */
    if( (FmtData.type & MK_WIN_VXD) && ( num_entries != 1 ) ) {
        LnkMsg( FTL+MSG_VXD_INCORRECT_EXPORT, NULL );
    }
}

void PhoneyStack( void )
/*****************************/
// signal that we will be making a fake stack later on.
{
    FmtData.u.os2.flags |= PHONEY_STACK_FLAG;
}

static WResDir InitNEResources(int *resHandle, ResTable *outRes)
/**************************************************************/
{
    WResDir     inRes;
    bool        dup_discarded;
    bool        error;

    if( FmtData.resource ) {
        *resHandle = QOpenR( FmtData.resource );
        inRes = WResInitDir();
        error = WResReadDir( *resHandle, inRes, &dup_discarded );
        if( error ) {
            LnkMsg( WRN+MSG_PROBLEM_IN_RESOURCE, NULL );
            inRes = NULL;
        }

        outRes->Dir.NumTypes = WResGetNumTypes( inRes );
        outRes->Dir.NumResources = WResGetNumResources( inRes );
        outRes->Dir.TableSize = outRes->Dir.NumTypes * sizeof( resource_type_record ) +
                            outRes->Dir.NumResources * sizeof( resource_record ) +
                            2 * sizeof( uint_16 );
        /* the 2 uint_16 are the resource shift count and the type 0 record */
        outRes->Dir.Head = NULL;
        outRes->Dir.Tail = NULL;
        StringBlockBuild( &outRes->Str, inRes, false );
    } else {
        inRes = NULL;
    }
    return( inRes );
}

static void FiniNEResources( int resHandle, WResDir inRes, ResTable *outRes )
/***************************************************************************/
{
    if( inRes != NULL ) {
        if( outRes->Str.StringBlock != NULL ) {
            _LnkFree( outRes->Str.StringBlock );
            outRes->Str.StringBlock = NULL;
        }
        if( outRes->Str.StringList != NULL ) {
            _LnkFree( outRes->Str.StringList );
            outRes->Str.StringList = NULL;
        }
        WResFreeDir( inRes );
        QClose( resHandle, FmtData.resource );
    }
}


static uint_32 ComputeResourceSize( WResDir dir )
/***********************************************/
{
    uint_32         length;
    WResDirWindow   wind;
    WResLangInfo    *res;

    if( dir == NULL ) {
        return( 0 );
    }
    length = 0;
    wind = WResFirstResource( dir );
    while( !WResIsEmptyWindow( wind ) ) {
        res = WResGetLangInfo( wind );
        length += res->Length;
        wind = WResNextResource( wind, dir );
    }
    return( length );
}

#define MAX_DGROUP_SIZE (64*1024UL)

void FiniOS2LoadFile( void )
/***************************/
/* terminate writing of load file */
{
    os2_exe_header      exe_head;
    unsigned long       temp;
    unsigned_16         adseg;
    group_entry         *group;
    unsigned_32         stub_len;
    unsigned_32         dgroup_size;
    unsigned_32         dgroup_total;
    unsigned long       size;
    entry_export        *exp;
    unsigned long       imageguess;     // estimated length of the image
    unsigned            pad_len;
    WResDir             inRes;          // Directory of resources to read
    int                 resHandle = 0;  // Handle for resources file
    ResTable            outRes;         // Resources to go out

    stub_len = Write_Stub_File( STUB_ALIGN );
    temp = sizeof( os2_exe_header );
    exe_head.segment_off = temp;
    SeekLoad( stub_len + sizeof( os2_exe_header ) );
    adseg = 0;
    exe_head.segments = 0;
    dgroup_size = 0;
    imageguess = 0;
    if( DataGroup != NULL ) {
        adseg = DataGroup->grp_addr.seg;
        if( DataGroup->segflags & SEG_PURE ) {
            FmtData.u.os2.flags |= SHARABLE_DGROUP;
        }
        if( StackSegPtr != NULL ) {
            if( DataGroup->totalsize - DataGroup->size < StackSize ) {
                StackSize = DataGroup->totalsize - DataGroup->size;
                DataGroup->totalsize = DataGroup->size;
            } else {
                DataGroup->totalsize -= StackSize;
            }
        }
        dgroup_size = DataGroup->totalsize;
    }
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        imageguess += group->size;
        exe_head.segments++;
    }
    temp += exe_head.segments * sizeof( segment_record );
    inRes = InitNEResources(&resHandle, &outRes);
    exe_head.resource_off = temp;
    if( inRes ) {
        exe_head.resource = outRes.Dir.NumResources;
        temp += outRes.Dir.TableSize;
        temp += outRes.Str.StringBlockSize;
    } else {
        exe_head.resource = 0;
    }
    exe_head.resident_off = temp;
    SeekLoad( stub_len+temp );
    temp += ResNonResNameTable( true );  // true - do resident table.
    exe_head.module_off = temp;
    exe_head.modrefs = ModRefTable();
    temp += exe_head.modrefs * sizeof( unsigned_16 );
    exe_head.import_off = temp;
    temp += ImportNameTable();
    exe_head.entry_off = temp;
    size = DumpEntryTable();
    exe_head.entry_size = size;
    temp += size;
    temp += stub_len;
    exe_head.nonres_off = temp;
    exe_head.nonres_size = ResNonResNameTable( false );  // false = do non-res.
    temp += exe_head.nonres_size;
    /*
     * if no segment shift specified, figure out the best one, assuming that
     * the maximum padding will happen every time.
     */
    if( FmtData.u.os2.segment_shift == 0 ) {
        imageguess += temp + (unsigned long)Root->relocs * sizeof( os2_reloc_item )
                     + stub_len + exe_head.segments * 3;
        pad_len = binary_log( (imageguess >> 16) << 1 );
        imageguess += ((1 << pad_len) - 1) * exe_head.segments;
        imageguess += ComputeResourceSize( inRes ); // inRes may be 0
        FmtData.u.os2.segment_shift = binary_log( (imageguess >> 16) << 1 );
        if( FmtData.u.os2.segment_shift == 0 ) {
            FmtData.u.os2.segment_shift = 1;     // since microsoft thinks 0 == 9
        }
    }
    exe_head.gangstart = NullAlign( 1 << FmtData.u.os2.segment_shift ) >>
                         FmtData.u.os2.segment_shift;
    WriteOS2Data( stub_len, &exe_head );
    WriteOS2Resources( resHandle, inRes, &outRes );
    exe_head.gangstart = 0;
    exe_head.ganglength = 0;
    if( inRes ) {
        SeekLoad( exe_head.resource_off + stub_len );
        WriteResTable( &outRes );
        FreeResTable( &outRes );
    }
    SeekEndLoad( 0 );
    FiniNEResources( resHandle, inRes, &outRes );
    DBIWrite();
    exe_head.signature = OS2_SIGNATURE_WORD;
    exe_head.version = 0x0105;          /* version 5.1 */
    exe_head.chk_sum = 0L;
    exe_head.info = 0;
    if( FmtData.type & MK_WINDOWS ) {
        exe_head.target = TARGET_WINDOWS;
    } else {
        exe_head.target = TARGET_OS2;
    }
    if( FmtData.u.os2.flags & PROTMODE_ONLY ) {
        exe_head.info |= OS2_PROT_MODE_ONLY;
    }
    if( FmtData.u.os2.flags & SINGLE_AUTO_DATA ) {
        exe_head.info |= OS2_SINGLE_AUTO;
    } else if( FmtData.u.os2.flags & MULTIPLE_AUTO_DATA ){
        exe_head.info |= OS2_MULT_AUTO;
    } else {
        adseg = 0;    // no automatic data segment.
    }
    if( FmtData.u.os2.flags & PM_NOT_COMPATIBLE ) {
        exe_head.info |= OS2_NOT_PM_COMPATIBLE;
    } else if( FmtData.u.os2.flags & PM_APPLICATION ) {
        exe_head.info |= OS2_PM_APP;
    } else {
        exe_head.info |= OS2_PM_COMPATIBLE;
    }
    if( FmtData.u.os2.is_private_dll ) {
        exe_head.info |= WIN_PRIVATE_DLL;
    }
    if( FmtData.dll ) {
        exe_head.info |= OS2_IS_DLL;
        if( FmtData.u.os2.flags & INIT_INSTANCE_FLAG ) {
            exe_head.info |= OS2_INIT_INSTANCE;
        }
    }
    exe_head.adsegnum = adseg;
    exe_head.heap = FmtData.u.os2.heapsize;
/*
 * the microsoft linker for windows will generate a stack even if no stack
 * segment has been explicitly specified. (as long as the user specifies a stack
 * size). Since microsoft's windows libraries rely on this, we have to mimic
 * it.
*/
    if( FmtData.u.os2.flags & PHONEY_STACK_FLAG ) {
        exe_head.SP = 0;
        exe_head.stacknum = adseg;
    } else {
        exe_head.SP = 0;
        //exe_head.SP = StackAddr.off;
        if( StackAddr.seg != UNDEFINED ) {
            exe_head.stacknum = StackAddr.seg;
        } else {
            exe_head.stacknum = 0;
        }
    }
    exe_head.stack = StackSize;
    if( dgroup_size + exe_head.heap > MAX_DGROUP_SIZE ) {
        LnkMsg( WRN+MSG_HEAP_TOO_BIG, NULL );
        if( dgroup_size < MAX_DGROUP_SIZE ) {
            exe_head.heap = MAX_DGROUP_SIZE - dgroup_size;
        }
    }
    exe_head.IP = StartInfo.addr.off;
    if( StartInfo.type == START_UNDEFED ) {
        exe_head.entrynum = 0;
    } else {
        exe_head.entrynum = StartInfo.addr.seg;
    }
    exe_head.align = FmtData.u.os2.segment_shift;
    exe_head.movable = 0;
    for( exp = FmtData.u.os2.exports; exp != NULL; exp = exp->next ) {
        if( exp->ismovable ) {
            exe_head.movable++;
        }
    }
    exe_head.otherflags = 0;
    if( FmtData.u.os2.flags & LONG_FILENAMES ) {
        exe_head.otherflags = OS2_LONG_FILE_NAMES;
    }
    if( FmtData.u.os2.flags & PROPORTIONAL_FONT ) {
        exe_head.otherflags |= WIN_CLEAN_MEMORY | WIN_PROPORTIONAL_FONT;
    } else if( FmtData.u.os2.flags & CLEAN_MEMORY ) {
        exe_head.otherflags |= WIN_CLEAN_MEMORY;
    }
    if( exe_head.ganglength ) {
        exe_head.otherflags |= WIN_GANGLOAD_PRESENT;
    }
    exe_head.swaparea = 0;
    if( FmtData.type & MK_WINDOWS ) {
        if( FmtData.ver_specified ) {
            exe_head.expver = (FmtData.major << 8) | (FmtData.minor & 0xFF);
        } else {
            exe_head.expver = 0x300;
        }
    } else {
        exe_head.expver = 0;
    }
    /* Check default data segment size. On OS/2, data segment + heap + stack
     * may be up to 64K. On Windows, the max is about 0xfffe. Windows may also
     * tweak the default heap/stack size so this check isn't bulletproof.
     */
    dgroup_total = dgroup_size + exe_head.stack + exe_head.heap;
    if( FmtData.type & MK_WINDOWS ) {
        if( dgroup_total > (MAX_DGROUP_SIZE - 3) ) {
            LnkMsg( FTL+MSG_DEFDATA_TOO_BIG, "l",
                    dgroup_total - MAX_DGROUP_SIZE + 3 );
        }
    } else {
        if( dgroup_total > MAX_DGROUP_SIZE ) {
            LnkMsg( FTL+MSG_DEFDATA_TOO_BIG, "l",
                    dgroup_total - MAX_DGROUP_SIZE );
        }
    }

    if( LinkState & LINK_ERROR ) {
        exe_head.info |= OS2_LINK_ERROR;
    }
    SeekLoad( stub_len );
    WriteLoad( &exe_head, sizeof( os2_exe_header ) );
}

void FreeImpNameTab( void )
/********************************/
{
    FmtData.u.os2.mod_ref_list = NULL;  /* these are permalloc'd */
    FmtData.u.os2.imp_tab_list = NULL;
}

static unsigned DoExeName( void )
/*******************************/
/* make up the "program is %f" string, and put it in tokbuff.*/
{
    char        rc_buff[RESOURCE_MAX_SIZE];
    unsigned    msgsize;

    Msg_Get( MSG_IS_A_EXE, rc_buff );
    msgsize = FmtStr( TokBuff, TokSize, rc_buff );
    TokBuff[msgsize++] = '\r';
    TokBuff[msgsize++] = '\n';
    TokBuff[msgsize] = '$';     /* end of string for int 21 fn. 9 */
    return( msgsize + 1 );
}

unsigned_32 GetStubSize( void )
/************************************/
/* return the size of the stub file (unaligned) */
{
    unsigned_32     stub_len = 0;
    f_handle        the_file;
    dos_exe_header  dosheader;
    unsigned_32     read_len;
    unsigned_32     reloc_size;
    unsigned_32     code_start;

    if( FmtData.u.os2.no_stub ) {
        return( 0 );
    }
    stub_len = sizeof( DosStub ) + DoExeName();
    if( FmtData.u.os2.stub_file_name != NULL && stricmp( FmtData.u.os2.stub_file_name, Root->outfile->fname ) != 0 ) {
        the_file = FindPath( FmtData.u.os2.stub_file_name );
        if( the_file != NIL_FHANDLE ) {
            QRead( the_file, &dosheader, sizeof( dos_exe_header ), FmtData.u.os2.stub_file_name );
            if( dosheader.signature == DOS_SIGNATURE ) {
                if( dosheader.mod_size == 0 ) {
                    read_len = 512;
                } else {
                    read_len = dosheader.mod_size;
                }
                code_start = dosheader.hdr_size * 16ul;
                read_len += (dosheader.file_size - 1) * 512ul - code_start;
                // make sure reloc_size is a multiple of 16.
                reloc_size = MAKE_PARA( dosheader.num_relocs * 4ul );
                dosheader.hdr_size = 4 + reloc_size/16;
                stub_len = read_len + dosheader.hdr_size * 16ul;
            }
            QClose( the_file, FmtData.u.os2.stub_file_name );
        }
    }
    return( stub_len );
}

static unsigned WriteDefStub( unsigned_32 stub_align )
/****************************************************/
/* write the default stub to the executable file */
{
    unsigned            msgsize;
    unsigned            fullsize;
    unsigned_32         *stubend;

    msgsize = DoExeName();
    fullsize = ROUND_UP( msgsize + sizeof( DosStub ), stub_align );
    stubend = (unsigned_32 *)(DosStub + 0x3c);
    *stubend = fullsize;
    WriteLoad( DosStub, sizeof( DosStub ) );
    WriteLoad( TokBuff, msgsize );
    PadLoad( fullsize - msgsize - sizeof( DosStub ) );
    return( fullsize );
}

unsigned_32 Write_Stub_File( unsigned_32 stub_align )
/***************************************************/
{
    unsigned_32     stub_len;
    f_handle        the_file;
    dos_exe_header  dosheader;
    unsigned_32     read_len;
    unsigned        amount;
    unsigned_32     reloc_size;
    unsigned_16     num_relocs;
    unsigned_32     the_reloc;
    unsigned_32     code_start;

    if( FmtData.u.os2.no_stub ) {
        stub_len = 0;
    } else if( FmtData.u.os2.stub_file_name == NULL ) {
        stub_len = WriteDefStub( stub_align );
    } else if( stricmp( FmtData.u.os2.stub_file_name, Root->outfile->fname ) == 0 ) {
        LnkMsg( ERR+MSG_STUB_SAME_AS_LOAD, NULL );
        stub_len = WriteDefStub( stub_align );
    } else {
        the_file = FindPath( FmtData.u.os2.stub_file_name );
        if( the_file == NIL_FHANDLE ) {
            LnkMsg( WRN+MSG_CANT_OPEN_NO_REASON, "s", FmtData.u.os2.stub_file_name );
            return( WriteDefStub( stub_align ) );   // NOTE: <== a return here.
        }
        QRead( the_file, &dosheader, sizeof( dos_exe_header ), FmtData.u.os2.stub_file_name );
        if( dosheader.signature != DOS_SIGNATURE ) {
            LnkMsg( ERR + MSG_INV_STUB_FILE, NULL );
            stub_len = WriteDefStub( stub_align );
        } else {
            if( dosheader.mod_size == 0 ) {
                read_len = 512;
            } else {
                read_len = dosheader.mod_size;
            }
            QSeek( the_file, dosheader.reloc_offset, FmtData.u.os2.stub_file_name );
            dosheader.reloc_offset = 0x40;
            code_start = dosheader.hdr_size * 16ul;
            read_len += (dosheader.file_size - 1) * 512ul - code_start;
            // make sure reloc_size is a multiple of 16.
            reloc_size = MAKE_PARA( dosheader.num_relocs * 4ul );
            dosheader.hdr_size = 4 + reloc_size/16;
            stub_len = read_len + dosheader.hdr_size * 16ul;
            dosheader.file_size = (stub_len + 511) >> 9;  // round up.
            dosheader.mod_size = stub_len % 512;
            WriteLoad( &dosheader, sizeof( dos_exe_header ) );
            PadLoad( 0x3c - sizeof( dos_exe_header ) );
            stub_len = ROUND_UP( stub_len, stub_align );
            WriteLoad( &stub_len, sizeof( unsigned_32 ) );
            for(num_relocs = dosheader.num_relocs;num_relocs > 0;num_relocs--) {
                QRead( the_file, &the_reloc, sizeof( unsigned_32 ), FmtData.u.os2.stub_file_name );
                WriteLoad( &the_reloc, sizeof( unsigned_32 ) );
                reloc_size -= sizeof( unsigned_32 );
            }
            if( reloc_size != 0 ) {    // need padding
                PadLoad( reloc_size );
            }
            QSeek( the_file, code_start, FmtData.u.os2.stub_file_name );
            for( ; read_len > 0; read_len -= amount ) {
                if( read_len < TokSize ) {
                    amount = read_len;
                } else {
                    amount = TokSize;
                }
                QRead( the_file, TokBuff, amount, FmtData.u.os2.stub_file_name );
                WriteLoad( TokBuff, amount );
            }
            stub_len = NullAlign( stub_align );
        }
        QClose( the_file, FmtData.u.os2.stub_file_name );
        _LnkFree( FmtData.u.os2.stub_file_name );
        FmtData.u.os2.stub_file_name = NULL;
    }
    return( stub_len );
}
