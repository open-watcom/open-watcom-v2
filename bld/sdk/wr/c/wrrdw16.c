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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wrglbl.h"
#include "wrrdw16.h"
#include "wrmsg.h"
#include "rcrtns.h"
#include "exedos.h"
#include "wresdefn.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define NAME_TABLE_MAX   8192

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WRNameTableEntry {
    uint_16 length;
    uint_16 type;
    uint_16 id;
    uint_8  no_idea;
    char    name[1];
} WRNameTableEntry;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool         WRLoadWResDirFromWin16EXE( FILE *, WResDir * );
static bool         WRWin16HeaderHasResourceTable( os2_exe_header * );
static WResTypeNode *WRReadWResTypeNodeFromExe( FILE *, uint_16 );
static WResResNode  *WRReadWResResNodeFromExe( FILE *, uint_16 );
static bool         WRReadResourceNames( WResDir, FILE *, uint_32 );
static bool         WRSetResName( WResDir, uint_32, const char * );
static WResTypeNode *WRRenameWResTypeNode( WResDir, WResTypeNode *, const char * );
static WResResNode  *WRRenameWResResNode( WResTypeNode *, WResResNode *, const char * );
static uint_32      WRReadNameTable( WResDir, FILE *, uint_8 **, uint_32, uint_8 * );
static uint_32      WRUseNameTable( WResDir, uint_8 *, uint_32, uint_8 ** );
static bool         WRSetResNameFromNameTable( WResDir, WRNameTableEntry * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRLoadResourceFromWin16EXE( WRInfo *info )
{
    FILE        *fp;
    bool        ok;

    ok = ( (fp = ResOpenFileRO( info->file_name )) != NULL );

    if( ok ) {
        ok = WRLoadWResDirFromWin16EXE( fp, &info->dir );
    }

    if( fp != NULL ) {
        ResCloseFile( fp );
    }

    return( ok );
}

long WRReadWin16ExeHeader( FILE *fp, os2_exe_header *nehdr )
{
    uint_16     data;
    uint_32     ne_header_off;
    bool        error;

    if( fp == NULL
      || nehdr == NULL )
        return( 0 );
    /*
     * check the reloc offset
     */
    if( RESSEEK( fp, DOS_RELOC_OFFSET, SEEK_SET ) )
        return( 0 );
    error = false;
    data = ResReadUint16( &error, fp );
    if( error
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( 0 );
    }
    /*
     * check header offset
     */
    if( RESSEEK( fp, NE_HEADER_OFFSET, SEEK_SET ) )
        return( 0 );
    ne_header_off = ResReadUint32( &error, fp );
    if( error
      || ne_header_off == 0 ) {
        return( 0 );
    }
    if( RESSEEK( fp, ne_header_off, SEEK_SET ) )
        return( 0 );

    if( RESREAD( fp, nehdr, sizeof( *nehdr ) ) != sizeof( *nehdr ) )
        return( 0 );
    /*
     * check for valid Win16 EXE
     */
    if( !WRIsHeaderValidWIN16( nehdr ) )
        return( 0 );

    return( ne_header_off );
}

bool WRIsHeaderValidWIN16( os2_exe_header *nehdr )
{
    return( nehdr->signature == EXESIGN_NE && nehdr->expver >= 0x300 );
}

bool WRWin16HeaderHasResourceTable( os2_exe_header *nehdr )
{
    return( nehdr->resource_off != nehdr->resident_off );
}

bool WRLoadWResDirFromWin16EXE( FILE *fp, WResDir *dir )
{
    os2_exe_header  nehdr;
    long            ne_header_off;
    uint_16         align_shift;
    uint_32         name_offset;
    WResTypeNode    *typenode;
    uint_8          *name_table;
    uint_8          *leftover;
    uint_32         name_table_len;
    uint_32         num_leftover;
    bool            ok;
    bool            error;

    ok = ( fp != NULL );

    if( ok ) {
        ok = ( (*dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        ok = ( (ne_header_off = WRReadWin16ExeHeader( fp, &nehdr )) != 0 );
    }
    /*
     * check if a resource table is present
     */
    if( ok ) {
        ok = WRWin16HeaderHasResourceTable( &nehdr );
        if( !ok ) {
            return( true );
        }
    }

    if( ok ) {
        ok = !RESSEEK( fp, ne_header_off, SEEK_SET );
    }

    if( ok ) {
        ok = !RESSEEK( fp, nehdr.resource_off, SEEK_CUR );
    }

    if( ok ) {
        error = false;
        align_shift = ResReadUint16( &error, fp );
        if( error
          || align_shift > 16 ) {
            WRDisplayErrorMsg( WR_BADEXE );
            return( false );
        }
    }

    if( ok ) {
        (*dir)->NumResources = 0;
        for( typenode = WRReadWResTypeNodeFromExe( fp, align_shift ); typenode != NULL; typenode = WRReadWResTypeNodeFromExe ( fp, align_shift ) ) {
            typenode->next = NULL;
            typenode->prev = (*dir)->Tail;
            if( (*dir)->Tail != NULL ) {
                (*dir)->Tail->next = typenode;
            }
            if( (*dir)->Head == NULL ) {
                (*dir)->Head = typenode;
            }
            (*dir)->Tail = typenode;
            (*dir)->NumTypes++;
            (*dir)->NumResources += typenode->Info.NumResources;
        }
        name_offset = RESTELL( fp ) - ne_header_off - nehdr.resource_off;
        ok = WRReadResourceNames( *dir, fp, name_offset );
    }

    if( ok
      && nehdr.expver <= 0x300 ) {
        num_leftover = 0;
        leftover = NULL;
        name_table_len = WRReadNameTable( *dir, fp, &name_table, num_leftover, leftover );
        while( name_table_len != 0 ) {
            num_leftover = WRUseNameTable( *dir, name_table, name_table_len, &leftover );
            if( name_table != NULL ) {
                MemFree( name_table );
            }
            name_table_len = WRReadNameTable( NULL, fp, &name_table, num_leftover, leftover );
            if( leftover != NULL ) {
                MemFree( leftover );
                leftover = NULL;
            }
        }
    }

    return( ok );
}

WResTypeNode *WRReadWResTypeNodeFromExe( FILE *fp, uint_16 align_shift )
{
    uint_16                 resource_count;
    resource_type_record    type_info;
    WResTypeNode            *typenode;
    WResResNode             *resnode;

    if( RESREAD( fp, &type_info, sizeof( resource_type_record ) ) != sizeof( resource_type_record ) ) {
        return( NULL );
    }
    if( type_info.type_id == 0 ) {
        return( NULL );
    }
    typenode = MemAlloc( sizeof( WResTypeNode ) );
    if( typenode == NULL )
        return( NULL );

    resource_count = type_info.num_resources;
    typenode->next = NULL;
    typenode->prev = NULL;
    typenode->Head = NULL;
    typenode->Tail = NULL;
    typenode->Info.NumResources = resource_count;
    typenode->Info.TypeName.IsName = ( (type_info.type_id & 0x8000) == 0 );
    typenode->Info.TypeName.ID.Num = (type_info.type_id & 0x7fff);

    for( ; resource_count != 0; resource_count-- ) {
        resnode = WRReadWResResNodeFromExe( fp, align_shift );
        if( typenode->Head == NULL ) {
            typenode->Head = resnode;
        }
        if( typenode->Tail != NULL ) {
            typenode->Tail->next = resnode;
        }
        resnode->prev = typenode->Tail;
        typenode->Tail = resnode;
    }

    return( typenode );
}

WResResNode *WRReadWResResNodeFromExe( FILE *fp, uint_16 align )
{
    resource_record res_info;
    uint_32         offset_32;
    uint_32         length_32;
    WResResNode     *resnode;
    WResLangNode    *langnode;

    resnode = MemAlloc( sizeof( WResResNode ) );
    if( resnode == NULL ) {
        return( NULL );
    }

    langnode = MemAlloc( sizeof( WResLangNode ) );
    if( langnode == NULL ) {
        MemFree( resnode );
        return( NULL );
    }

    if( RESREAD( fp, &res_info, sizeof( resource_record ) ) != sizeof( resource_record ) ) {
        return( NULL );
    }

    resnode->next = NULL;
    resnode->prev = NULL;
    resnode->Head = langnode;
    resnode->Tail = langnode;
    resnode->Info.NumResources = 1;
    resnode->Info.ResName.IsName = ( (res_info.res_id & 0x8000) == 0 );
    resnode->Info.ResName.ID.Num = (res_info.res_id & 0x7fff);

    langnode->next = NULL;
    langnode->prev = NULL;
    langnode->data = NULL;
    langnode->Info.res_flags = res_info.flags;
    offset_32 = (uint_32)res_info.offset;
    length_32 = (uint_32)res_info.length;
    langnode->Info.Offset = (uint_32)(offset_32 << align);
    langnode->Info.Length = (uint_32)(length_32 << align);
    langnode->Info.lang.lang = DEF_LANG;
    langnode->Info.lang.sublang = DEF_SUBLANG;

    return( resnode );
}

bool WRReadResourceNames( WResDir dir, FILE *fp, uint_32 name_offset )
{
    uint_8      name_len;
    char        *name;
    bool        end_of_names;
    bool        error;

    end_of_names = false;

    error = false;
    name_len = ResReadUint8( &error, fp );
    if( error )
        return( false );

    while( !end_of_names ) {
        if( name_len == 0 ) {
            name_len = ResReadUint8( &error, fp );
            if( error )
                return( false );
            if( name_len == 0 ) {
                end_of_names = true;
            } else {
                name_offset++;
            }
        } else {
            name = MemAlloc( name_len + 1 );
            if( RESREAD( fp, name, name_len ) != name_len ) {
                return( false );
            }
            name[name_len] = 0;
            WRSetResName( dir, name_offset, name );
            MemFree( name );
            name_offset = name_offset + name_len + 1;
            name_len = ResReadUint8( &error, fp );
            if( error ) {
                return( false );
            }
        }
    }

    return( true );
}

bool WRSetResName( WResDir dir, uint_32 offset, const char *name )
{
    WResTypeNode *typenode;
    WResResNode  *resnode;
    bool         found_one;

    found_one = false;
    for( typenode = dir->Head; typenode != NULL; typenode = typenode->next ) {
        if( typenode->Info.TypeName.IsName && typenode->Info.TypeName.ID.Num == offset ) {
            typenode = WRRenameWResTypeNode( dir, typenode, name );
            if( typenode == NULL ) {
                return( false );
            } else {
                found_one = true;
            }
        }
        for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
            if( resnode->Info.ResName.IsName && resnode->Info.ResName.ID.Num == offset ) {
                resnode = WRRenameWResResNode( typenode, resnode, name );
                if( resnode == NULL ) {
                    return( false );
                } else {
                    found_one = true;
                }
            }
            if( resnode == typenode->Tail ) {
                break;
            }
        }
        if( typenode == dir->Tail ) {
            break;
        }
    }

    return( found_one );
}

WResTypeNode *WRRenameWResTypeNode( WResDir dir, WResTypeNode *typenode, const char *name )
{
    WResTypeNode    *new_typenode;
    size_t          len;

    len = strlen( name );
    new_typenode = MemAlloc( sizeof( WResTypeNode ) + len - 1 );
    if( new_typenode == NULL ) {
        return( NULL );
    }
    if( dir->Head == typenode ) {
        dir->Head = new_typenode;
    }
    if( dir->Tail == typenode ) {
        dir->Tail = new_typenode;
    }
    new_typenode->next = typenode->next;
    new_typenode->prev = typenode->prev;
    new_typenode->Head = typenode->Head;
    new_typenode->Tail = typenode->Tail;
    new_typenode->Info.NumResources = typenode->Info.NumResources;
    new_typenode->Info.TypeName.IsName = true;
    new_typenode->Info.TypeName.ID.Name.NumChars = len;
    memcpy( new_typenode->Info.TypeName.ID.Name.Name, name, len );
    if( typenode->prev != NULL ) {
        typenode->prev->next = new_typenode;
    }
    if( typenode->next != NULL ) {
        typenode->next->prev = new_typenode;
    }
    MemFree( typenode );
    return( new_typenode );
}

WResResNode *WRRenameWResResNode( WResTypeNode *typenode,
                                  WResResNode *resnode, const char *name )
{
    WResResNode *new_resnode;
    size_t      len;
    size_t      len1;

    len = strlen( name );
    len1 = len - 1;
    if( len == 0 )
        len1 = 0;
    new_resnode = MemAlloc( sizeof( WResResNode ) + len1 );
    if( new_resnode == NULL ) {
        return( NULL );
    }
    if( typenode->Head == resnode ) {
        typenode->Head = new_resnode;
    }
    if( typenode->Tail == resnode ) {
        typenode->Tail = new_resnode;
    }
    new_resnode->Head = resnode->Head;
    new_resnode->Tail = resnode->Tail;
    new_resnode->next = resnode->next;
    new_resnode->prev = resnode->prev;
    new_resnode->Info.ResName.IsName = true;
    new_resnode->Info.ResName.ID.Name.NumChars = len;
    memcpy( new_resnode->Info.ResName.ID.Name.Name, name, len );
    if( resnode->prev != NULL ) {
        resnode->prev->next = new_resnode;
    }
    if( resnode->next != NULL ) {
        resnode->next->prev = new_resnode;
    }
    MemFree( resnode );
    return( new_resnode );
}

uint_32 WRReadNameTable( WResDir dir, FILE *fp, uint_8 **name_table,
                         uint_32 num_leftover, uint_8 *leftover )
{
    static WResTypeNode *typenode;
    static WResResNode  *resnode;
    static uint_32      res_len;
    static uint_32      res_offset;
    static uint_32      num_read;
    uint_32             len;

    if( dir != NULL ) {
        typenode = WRFindTypeNode( dir, RESOURCE2INT( RT_NAMETABLE ), NULL );
        if( typenode != NULL ) {
            resnode = typenode->Head;
            /*
             * if there are two name tables we ignore all but the first
             */
            res_len = resnode->Head->Info.Length;
            res_offset = resnode->Head->Info.Offset;
            if( RESSEEK( fp, res_offset, SEEK_SET ) ) {
                return( 0 );
            }
        } else {
            res_len = 0;
        }
        num_read = 0;
    }

    if( num_read == res_len ) {
        return( 0 );
    }

    *name_table = NULL;

    if( num_read + NAME_TABLE_MAX < res_len ) {
        len = NAME_TABLE_MAX;
    } else {
        len = res_len - num_read;
    }

    num_read += len;
    /*
     * If there was data left over but we have a NULL pointer to the
     * leftover data then we are in a very bad situation.
     * What we shall do is skip the left over data.
     * If the seek to skip the data fails then we are totally SNAFU
     * and must abort the reading of the name resource!!
     */
    if( num_leftover != 0 && leftover == NULL ) {
        if( RESSEEK( fp, num_leftover, SEEK_CUR ) ) {
            return( 0 );
        }
        num_read += num_leftover;
        num_leftover = 0;
    }

    *name_table = MemAlloc( len + num_leftover );
    if( *name_table == NULL ) {
        return( 0 );
    }

    if( num_leftover != 0 ) {
        memcpy( *name_table, leftover, num_leftover );
    }

    if( RESREAD( fp, *name_table + num_leftover, len ) != len ) {
        /* hmmmm... the read failed */
    }

    return( len + num_leftover );
}

uint_32 WRUseNameTable( WResDir dir, uint_8 *name_table, uint_32 len, uint_8 **leftover )
{
    WRNameTableEntry    *entry;
    uint_32             name_pos;
    uint_32             num_leftover;

    num_leftover = 0;
    for( name_pos = 0; name_pos < len; name_pos += entry->length ) {
        entry = (WRNameTableEntry *)(name_table + name_pos);
        if( entry->length == 0 ) {
            break;
        }
        if( name_pos + entry->length > len ) {
            num_leftover = len - name_pos;
            *leftover = MemAlloc( num_leftover );
            if( *leftover != NULL ) {
                memcpy( *leftover, entry, num_leftover );
            } else {
                /* leftover alloc failed! */
            }
            break;
        }
        WRSetResNameFromNameTable( dir, entry );
    }

    return( num_leftover );
}

bool WRSetResNameFromNameTable( WResDir dir, WRNameTableEntry *entry )
{
    WResTypeNode *typenode;
    WResResNode  *resnode;

    for( typenode = dir->Head; typenode != NULL; typenode = typenode->next ) {
        if( !typenode->Info.TypeName.IsName && typenode->Info.TypeName.ID.Num == entry->type ) {
            for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
                if( !resnode->Info.ResName.IsName && resnode->Info.ResName.ID.Num == (entry->id & 0x7fff) ) {
                    if( WRRenameWResResNode( typenode, resnode, entry->name ) == NULL ) {
                        return( false );
                    } else {
                        return( true );
                    }
                }
                if( resnode == typenode->Tail ) {
                    break;
                }
            }
        }
        if( typenode == dir->Tail ) {
            break;
        }
    }

    return( false );
}
