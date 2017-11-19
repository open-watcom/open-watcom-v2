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


#include "commonui.h"
#include <string.h>
#include <io.h>
#include "wrglbl.h"
#include "wrrdwnt.h"
#include "wrmsg.h"
#include "wrmemi.h"
#include "rcrtns.h"

#include "clibext.h"


/* forward declarations */
bool WRReadResourceEntry( WResFileID fid, uint_32 offset, resource_entry *res_entry );

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define PE_OFFSET                 0x3c

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool     WRWinNTHeaderHasResourceTable( exe_pe_header * );
static int      WRCalcObjTableOffset( WResFileID, exe_pe_header * );
static bool     WRReadNTObjectTable( WResFileID, exe_pe_header *, pe_object ** );
static bool     WRLoadWResDirFromWinNTEXE( WResFileID, WResDir * );
static bool     WRHandleWinNTTypeDir( WResFileID, WResDir *, uint_32 );
static bool     WRHandleWinNTTypeEntry( WResFileID, WResDir *, resource_dir_entry *, bool );
static bool     WRHandleWinNTNameDir( WResFileID, WResDir *, WResID *, uint_32 );
static bool     WRHandleWinNTNameEntry( WResFileID, WResDir *, WResID *, resource_dir_entry *, bool );
static bool     WRHandleWinNTLangIDDir( WResFileID, WResDir *, WResID *, WResID *, uint_32 );
static bool     WRHandleWinNTLangIDEntry( WResFileID, WResDir *, WResID *, WResID *, resource_dir_entry * );
static bool     WRReadResourceHeader( WResFileID, uint_32, resource_dir_header *, resource_dir_entry ** );
static WResID   *WRGetUniCodeWResID( WResFileID, uint_32 );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static uint_32 res_offset = 0;
static uint_32 res_rva    = 0;

#define WR_MAP_DATA_RVA( rva )  (uint_32)((rva - res_rva) + res_offset)
#define WR_MAP_RES_RVA( rva )   (uint_32)(rva + res_offset)

bool WRLoadResourceFromWinNTEXE( WRInfo *info )
{
    WResFileID  fid;
    bool        ok;

    ok = ( (fid = ResOpenFileRO( info->file_name )) != NULL );

    if( ok ) {
        ok = WRLoadWResDirFromWinNTEXE( fid, &info->dir );
    }

    if( fid != NULL ) {
        ResCloseFile( fid );
    }

    return( ok );
}

long int WRReadWinNTExeHeader( WResFileID fid, exe_pe_header *header )
{
    bool        old_pos;
    uint_16     offset;
    bool        ok;

    old_pos = false;

    ok = ( fid != NULL && header != NULL );

    if( ok ) {
        ok = old_pos = !RESSEEK( fid, 0x18, SEEK_SET );
    }

    /* check the reloc offset */
    if( ok ) {
        ResReadUint16( &offset, fid );
        ok = ( offset >= 0x0040 );
    }

    if( ok ) {
        ok = !RESSEEK( fid, PE_OFFSET, SEEK_SET );
    }

    /* check header offset */
    if( ok ) {
        ResReadUint16( &offset, fid );
        ok = ( offset != 0 );
    }

    if( ok ) {
        ok = !RESSEEK( fid, offset, SEEK_SET );
    }

    if( ok ) {
        ok = ( RESREAD( fid, &PE32( *header ), sizeof( pe_header ) ) == sizeof( pe_header ) );
        if( ok && IS_PE64( *header ) ) {
            ok = !RESSEEK( fid, offset, SEEK_SET );
            if( ok ) {
                ok = ( RESREAD( fid, &PE64( *header ), sizeof( pe_header64 ) ) == sizeof( pe_header64 ) );
            }
        }
    }

    /* check for valid Win32 EXE */
    if( ok ) {
        ok = WRIsHeaderValidWINNT( header );
    }

    if( old_pos ) {
        ok = ( !RESSEEK( fid, 0x18, SEEK_SET ) && ok );
    }

    if( !ok ) {
        WRDisplayErrorMsg( WR_INVALIDNTEXE );
        offset = 0;
    }

    return( offset );
}

int WRCalcObjTableOffset( WResFileID fid, exe_pe_header *hdr )
{
    uint_16  pe_offset;
    int      offset;
    bool     ok;

    ok = !ResSeek( fid, PE_OFFSET, SEEK_SET );

    if( ok ) {
        ResReadUint16( &pe_offset, fid );
        ok = ( pe_offset != 0 );
    }

    if ( ok ) {
        if( IS_PE64( *hdr ) ) {
            offset = pe_offset + PE64( *hdr ).nt_hdr_size + offsetof( pe_header64, magic );
        } else {
            offset = pe_offset + PE32( *hdr ).nt_hdr_size + offsetof( pe_header, magic );
        }
    } else {
        offset = 0;
    }

    return( offset );
}

bool WRReadNTObjectTable( WResFileID fid, exe_pe_header *hdr, pe_object **ot )
{
    size_t  size;
    int     ot_offset;

    ot_offset = WRCalcObjTableOffset( fid, hdr );
    if( ot_offset == 0 || ResSeek( fid, ot_offset, SEEK_SET ) ) {
        return( false );
    }
    if( IS_PE64( *hdr ) ) {
        size = sizeof( pe_object ) * PE64( *hdr ).num_objects;
    } else {
        size = sizeof( pe_object ) * PE32( *hdr ).num_objects;
    }
    *ot = (pe_object *)MemAlloc( size );
    if( *ot != NULL ) {
        if( RESREAD( fid, *ot, size ) != size ) {
            MemFree( *ot );
            *ot = NULL;
        }
    }

    return( *ot != NULL );
}

bool WRIsHeaderValidWINNT( exe_pe_header *header )
{
    /* at some point will we have to check the CPUTYPE ????!!!! */
    if( IS_PE64( *header ) ) {
        return( PE64( *header ).signature == PE_SIGNATURE );
    } else {
        return( PE32( *header ).signature == PE_SIGNATURE );
    }
}

bool WRWinNTHeaderHasResourceTable( exe_pe_header *header )
{
    int                 num_tables;
    pe_hdr_table_entry  *table;

    if( IS_PE64( *header ) ) {
        num_tables = PE64( *header ).num_tables;
        table = PE64( *header ).table;
    } else {
        num_tables = PE32( *header ).num_tables;
        table = PE32( *header ).table;
    }
    return( num_tables > PE_TBL_RESOURCE && table[PE_TBL_RESOURCE].rva != 0 && table[PE_TBL_RESOURCE].size != 0 );
}

bool WRLoadWResDirFromWinNTEXE( WResFileID fid, WResDir *dir )
{
    exe_pe_header       nt_header;
    pe_object           *otable;
    uint_32             physical_size;
    uint_32             physical_offset;
    int                 i;
    bool                ok;
    unsigned_32         resource_rva;

    ok = ( fid != NULL );

    if( ok ) {
        ok = ( (*dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        ok = ( WRReadWinNTExeHeader( fid, &nt_header ) != 0 );
    }

    /* check if a resource table is present */
    if( ok ) {
        ok = WRWinNTHeaderHasResourceTable( &nt_header );
        if( !ok ) {
            WRDisplayErrorMsg( WR_EXENORES );
            return( TRUE );
        }
    }

    /* read NT object table */
    otable = NULL;
    if( ok ) {
        ok = WRReadNTObjectTable( fid, &nt_header, &otable );
    }

    /* find resource object in object table */
    resource_rva = 0;
    if( ok ) {
        int         num_objects;
        unsigned_32 file_align;

        physical_size = 0;
        physical_offset = 0;
        if( IS_PE64( nt_header ) ) {
            resource_rva = PE64( nt_header ).table[PE_TBL_RESOURCE].rva;
            num_objects = PE64( nt_header ).num_objects;
            file_align = PE64( nt_header ).file_align;
        } else {
            resource_rva = PE32( nt_header ).table[PE_TBL_RESOURCE].rva;
            num_objects = PE32( nt_header ).num_objects;
            file_align = PE32( nt_header ).file_align;
        }
        for( i = 0; i < num_objects; i++ ) {
            if( otable[i].rva == resource_rva ) {
                physical_size = otable[i].physical_size;
                physical_offset = otable[i].physical_offset;
                break;
            }
        }
        ok = ( physical_size != 0 && physical_offset != 0 && (physical_size % file_align) == 0 && (physical_offset % file_align) == 0 );
    }

    if( otable != NULL ) {
        MemFree( otable );
    }

    /* read the resource information */
    if( ok ) {
        res_offset = physical_offset;
        res_rva = resource_rva;
        ok = WRHandleWinNTTypeDir( fid, dir, physical_offset );
    }

    return( ok );
}

bool WRHandleWinNTTypeDir( WResFileID fid, WResDir *dir, uint_32 offset )
{
    resource_dir_header rd_hdr;
    resource_dir_entry  *rd_entry;
    int                 i;
    bool                ok;

    ok = WRReadResourceHeader( fid, offset, &rd_hdr, &rd_entry );

    if( ok ) {
        for( i = 0; i < rd_hdr.num_name_entries; i++ ) {
            WRHandleWinNTTypeEntry( fid, dir, &rd_entry[i], true );
        }
        for( i = rd_hdr.num_name_entries; i < rd_hdr.num_name_entries + rd_hdr.num_id_entries; i++ ) {
            WRHandleWinNTTypeEntry( fid, dir, &rd_entry[i], false );
        }
        MemFree( rd_entry );
    }

    return( ok );
}

bool WRHandleWinNTTypeEntry( WResFileID fid, WResDir *dir, resource_dir_entry *rd_entry, bool is_name )
{
    WResID  *type;
    bool    ok;

    /* verify the id_name */
    if( ( (rd_entry->id_name & PE_RESOURCE_MASK_ON) != 0 ) ^ is_name ) {
        WRDisplayErrorMsg( WR_BADIDDISCARDTYPE );
        return( FALSE );
    }

    if( is_name ) {
        type = WRGetUniCodeWResID( fid, PE_RESOURCE_MASK & rd_entry->id_name );
    } else {
        type = WResIDFromNum( rd_entry->id_name );
    }

    ok = ( type != NULL );

    if( ok ) {
        /* verify that entry_rva is a subdir */
        ok = ( (rd_entry->entry_rva & PE_RESOURCE_MASK_ON) != 0 );
        if( !ok ) {
            WRDisplayErrorMsg( WR_BADRVADISCARDTYPE );
        }
    }

    if( ok ) {
        ok = WRHandleWinNTNameDir( fid, dir, type, rd_entry->entry_rva & PE_RESOURCE_MASK );
    }

    if( type != NULL ) {
        WResIDFree( type );
    }

    return( ok );
}

bool WRHandleWinNTNameDir( WResFileID fid, WResDir *dir, WResID *type, uint_32 rva )
{
    resource_dir_header rd_hdr;
    resource_dir_entry  *rd_entry;
    uint_32             offset;
    int                 i;
    bool                ok;

    offset = WR_MAP_RES_RVA( rva );

    ok = WRReadResourceHeader( fid, offset, &rd_hdr, &rd_entry );

    if( ok ) {
        for( i = 0; i < rd_hdr.num_name_entries; i++ ) {
            WRHandleWinNTNameEntry( fid, dir, type, &rd_entry[i], true );
        }
        for( i = rd_hdr.num_name_entries; i < rd_hdr.num_name_entries + rd_hdr.num_id_entries; i++ ) {
            WRHandleWinNTNameEntry( fid, dir, type, &rd_entry[i], false );
        }
        MemFree( rd_entry );
    }

    return( ok );
}

bool WRHandleWinNTNameEntry( WResFileID fid, WResDir *dir, WResID *type,
                            resource_dir_entry *rd_entry, bool is_name )
{
    WResLangType    def_lang;
    resource_entry  res_entry;
    uint_32         offset;
    WResID          *name;
    bool            add_now;
    bool            ok;

    def_lang.lang = DEF_LANG;
    def_lang.sublang = DEF_SUBLANG;

    /* verify the id_name */
    if( ( (rd_entry->id_name & PE_RESOURCE_MASK_ON) != 0 ) ^ is_name ) {
        WRDisplayErrorMsg( WR_BADIDDISCARDNAME );
        return( FALSE );
    }

    if( is_name ) {
        name = WRGetUniCodeWResID( fid, PE_RESOURCE_MASK & rd_entry->id_name );
    } else {
        name = WResIDFromNum( rd_entry->id_name );
    }

    ok = ( name != NULL );

    if( ok ) {
        /* is the entry_rva is a subdir */
        if( rd_entry->entry_rva & PE_RESOURCE_MASK_ON ) {
            add_now = false;
            ok = WRHandleWinNTLangIDDir( fid, dir, type, name, rd_entry->entry_rva & PE_RESOURCE_MASK );
        } else {
            /* will this to happen often ???? */
            add_now = true;
            offset = WR_MAP_RES_RVA( rd_entry->entry_rva );
            ok = WRReadResourceEntry( fid, offset, &res_entry );
        }
    }

    if( ok ) {
        if( add_now ) {
            offset = WR_MAP_RES_RVA( res_entry.data_rva );
            ok = !WResAddResource( type, name, 0, offset, res_entry.size,
                                   *dir, &def_lang, NULL );
        }
    }

    if( name != NULL ) {
        WResIDFree( name );
    }

    return( ok );
}

bool WRHandleWinNTLangIDDir( WResFileID fid, WResDir *dir,
                            WResID *type, WResID *name, uint_32 rva )
{
    resource_dir_header rd_hdr;
    resource_dir_entry  *rd_entry;
    uint_32             offset;
    bool                ok;
    int                 i;

    offset = WR_MAP_RES_RVA( rva );

    ok = WRReadResourceHeader( fid, offset, &rd_hdr, &rd_entry );

    if( ok ) {
        for( i = 0; i < rd_hdr.num_name_entries; i++ ) {
            WRHandleWinNTLangIDEntry( fid, dir, type, name, &rd_entry[i] );
        }
        for( i = rd_hdr.num_name_entries; i < rd_hdr.num_name_entries + rd_hdr.num_id_entries; i++ ) {
            WRHandleWinNTLangIDEntry( fid, dir, type, name, &rd_entry[i] );
        }
        /* until more info is available only look for the first id entry */
        MemFree( rd_entry );
    }

    return( ok );
}

bool WRHandleWinNTLangIDEntry( WResFileID fid, WResDir *dir, WResID *type,
                              WResID *name, resource_dir_entry *rd_entry )
{
    WResLangType    lang;
    resource_entry  res_entry;
    uint_32         offset;
    bool            ok;

    ok = ( name != NULL );

    if( ok ) {
        /* is the entry_rva is a subdir */
        if( rd_entry->entry_rva & PE_RESOURCE_MASK_ON ) {
            WRDisplayErrorMsg( WR_BADLANGDISCARDNAME );
            ok = false;
        } else {
            offset = WR_MAP_RES_RVA( rd_entry->entry_rva );
            ok = WRReadResourceEntry( fid, offset, &res_entry );
        }
    }

    if( ok ) {
        offset = WR_MAP_DATA_RVA( res_entry.data_rva );
        lang.lang = HIWORD( rd_entry->id_name );
        lang.sublang = LOBYTE( LOWORD( rd_entry->id_name ) );
        ok = !WResAddResource( type, name, 0, offset, res_entry.size, *dir, &lang, NULL );
    }

    return( ok );
}

bool WRReadResourceHeader( WResFileID fid, uint_32 offset,
                          resource_dir_header *rd_hdr,
                          resource_dir_entry **rd_entry )
{
    bool    ok;
    size_t  rde_size;

    *rd_entry = NULL;

    /* if offset is zero don't perform the seek to beginning of resource directory */
    ok = ( offset == 0 || !RESSEEK( fid, offset, SEEK_SET ) );

    /* read the resource directory header */
    if( ok ) {
        ok = ( RESREAD( fid, rd_hdr, sizeof( resource_dir_header ) ) == sizeof( resource_dir_header ) );
    }

    if( ok ) {
        rde_size = sizeof( resource_dir_entry ) * ( rd_hdr->num_name_entries + rd_hdr->num_id_entries );
        *rd_entry = (resource_dir_entry *)MemAlloc( rde_size );
        ok = ( *rd_entry != NULL );
    }

    if( ok ) {
        ok = ( RESREAD( fid, *rd_entry, rde_size ) == rde_size );
    }

    if( !ok && *rd_entry != NULL ) {
        MemFree( *rd_entry );
    }

    return( ok );
}

WResID *WRGetUniCodeWResID( WResFileID fid, uint_32 rva )
{
    bool    old_pos;
    uint_32 offset;
    uint_16 len;
    bool    ok;
    char    *unistr;
    WResID  *id;
    int     i;

    offset = WR_MAP_RES_RVA( rva );

    unistr = NULL;

    /* seek to the location of the Unicode string */
    ok = old_pos = !RESSEEK( fid, offset, SEEK_SET );

    /* read the Unicode string */
    if( ok ) {
        ResReadUint16( &len, fid );
        len *= 2;
        unistr = (char *)MemAlloc( len + 2 );
        ok = ( unistr != NULL );
    }

    if( ok ) {
        unistr[len] = 0;
        unistr[len + 1] = 0;
        ok = ( RESREAD( fid, unistr, len ) == len );
    }

    if( ok ) {
        for( i = 0; i < len + 2; i += 2 ) {
            unistr[i / 2] = unistr[i];
        }
        ok = ( (id = WResIDFromStr( unistr )) != NULL );
    }

#if 0
    if( old_pos ) {
        ok = ( !RESSEEK( fid, offset, SEEK_SET ) && ok );
    }
#endif

    if( unistr != NULL ) {
        MemFree( unistr );
    }

    if( ok ) {
        return( id );
    } else {
        return( NULL );
    }
}

bool WRReadResourceEntry( WResFileID fid, uint_32 offset, resource_entry *res_entry )
{
    bool ok;

    /* if offset is zero don't perform the seek to beginning of resource directory */
    ok = ( offset == 0 || !ResSeek( fid, offset, SEEK_SET ) );

    /* read the resource entry */
    if( ok ) {
        ok = ( RESREAD( fid, res_entry, sizeof( resource_entry ) ) == sizeof( resource_entry ) );
    }

    return( ok );
}
