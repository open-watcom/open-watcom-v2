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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wrglbl.h"
#include "wrrdw16.h"
#include "wrmsg.h"
#include "wrmemi.h"
#include "rcrtns.h"
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
typedef struct WRNameInfoStruct {
    uint_16 offset;
    uint_16 length;
    uint_16 flags;
    uint_16 id;
    uint_16 handle;
    uint_16 usage;
} WRNameInfo;

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

long WRReadWin16ExeHeader( FILE *fp, os2_exe_header *header )
{
    bool        old_pos;
    uint_16     offset;
    bool        ok;

    old_pos = -1;

    ok = ( fp != NULL && header != NULL );

    if( ok ) {
        ok = old_pos = !RESSEEK( fp, 0x18, SEEK_SET );
    }

    /* check the reloc offset */
    if( ok ) {
        ResReadUint16( &offset, fp );
        ok = ( offset >= 0x0040 );
    }

    if( ok ) {
        ok = !RESSEEK( fp, OS2_NE_OFFSET, SEEK_SET );
    }

    /* check header offset */
    if( ok ) {
        ResReadUint16( &offset, fp );
        ok = ( offset != 0 );
    }

    if( ok ) {
        ok = !RESSEEK( fp, offset, SEEK_SET );
    }

    if( ok ) {
        ok = ( RESREAD( fp, header, sizeof( os2_exe_header ) ) == sizeof( os2_exe_header ) );
    }

    /* check for valid Win16 EXE */
    if( ok ) {
        ok = WRIsHeaderValidWIN16( header );
    }

    if( old_pos ) {
        ok = ( !RESSEEK( fp, 0x18, SEEK_SET ) && ok );
    }

    if( ok ) {
        return( offset );
    } else {
        return( 0 );
    }
}

bool WRIsHeaderValidWIN16( os2_exe_header *header )
{
    if( header->signature == OS2_SIGNATURE_WORD && header->expver >= 0x300 ) {
        return( true );
    }

    return( false );
}

bool WRWin16HeaderHasResourceTable( os2_exe_header *header )
{
    if( header->resource_off != header->resident_off ) {
        return( true );
    }

    return( false );
}

bool WRLoadWResDirFromWin16EXE( FILE *fp, WResDir *dir )
{
    os2_exe_header  win_header;
    long            offset;
    uint_16         align_shift;
    uint_32         name_offset;
    WResTypeNode    *type_node;
    uint_8          *name_table;
    uint_8          *leftover;
    uint_32         name_table_len;
    uint_32         num_leftover;
    bool            ok;

    ok = ( fp != NULL );

    if( ok ) {
        ok = ( (*dir = WResInitDir()) != NULL );
    }

    if( ok ) {
        ok = ( (offset = WRReadWin16ExeHeader( fp, &win_header )) != 0 );
    }

    /* check if a resource table is present */
    if( ok ) {
        ok = WRWin16HeaderHasResourceTable( &win_header );
        if( !ok ) {
            return( true );
        }
    }

    if( ok ) {
        ok = !RESSEEK( fp, offset, SEEK_SET );
    }

    if( ok ) {
        ok = !RESSEEK( fp, win_header.resource_off, SEEK_CUR );
    }

    if( ok ) {
        ResReadUint16( &align_shift, fp );
        ok = ( align_shift <= 16 );
        if( !ok ) {
            WRDisplayErrorMsg( WR_BADEXE );
        }
    }

    if( ok ) {
        (*dir)->NumResources = 0;
        for( type_node = WRReadWResTypeNodeFromExe( fp, align_shift ); type_node != NULL; type_node = WRReadWResTypeNodeFromExe ( fp, align_shift ) ) {
            type_node->Next = NULL;
            type_node->Prev = (*dir)->Tail;
            if( (*dir)->Tail != NULL ) {
                (*dir)->Tail->Next = type_node;
            }
            if( (*dir)->Head == NULL ) {
                (*dir)->Head = type_node;
            }
            (*dir)->Tail = type_node;
            (*dir)->NumTypes++;
            (*dir)->NumResources += type_node->Info.NumResources;
        }
        name_offset = RESTELL( fp ) - offset - win_header.resource_off;
        ok = WRReadResourceNames( *dir, fp, name_offset );
    }

    if( ok && win_header.expver <= 0x300 ) {
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
    uint_16         type_id;
    uint_16         resource_count;
    uint_32         reserved;
    WResTypeNode    *type_node;
    WResResNode     *res_node;

    ResReadUint16( &type_id, fp );
    if( type_id == 0 ) {
        return( NULL );
    }

    type_node = (WResTypeNode *)MemAlloc( sizeof( WResTypeNode ) );
    if( type_node == NULL ) {
        return( NULL );
    }

    ResReadUint16( &resource_count, fp );
    ResReadUint32( &reserved, fp );

    type_node->Next = NULL;
    type_node->Prev = NULL;
    type_node->Head = NULL;
    type_node->Tail = NULL;
    type_node->Info.NumResources = resource_count;
    type_node->Info.TypeName.IsName = ( (type_id & 0x8000) == 0 );
    type_node->Info.TypeName.ID.Num = (type_id & 0x7fff);

    for( ; resource_count != 0; resource_count-- ) {
        res_node = WRReadWResResNodeFromExe( fp, align_shift );
        if( type_node->Head == NULL ) {
            type_node->Head = res_node;
        }
        if( type_node->Tail != NULL ) {
            type_node->Tail->Next = res_node;
        }
        res_node->Prev = type_node->Tail;
        type_node->Tail = res_node;
    }

    return( type_node );
}

WResResNode *WRReadWResResNodeFromExe( FILE *fp, uint_16 align )
{
    WRNameInfo      name_info;
    uint_32         offset_32;
    uint_32         length_32;
    WResResNode     *rnode;
    WResLangNode    *lnode;

    rnode = (WResResNode *)MemAlloc( sizeof( WResResNode ) );
    if( rnode == NULL ) {
        return( NULL );
    }

    lnode = (WResLangNode *)MemAlloc( sizeof( WResLangNode ) );
    if( lnode == NULL ) {
        MemFree( rnode );
        return( NULL );
    }

    if( RESREAD( fp, &name_info, sizeof( WRNameInfo ) ) != sizeof( WRNameInfo ) ) {
        return( NULL );
    }

    rnode->Next = NULL;
    rnode->Prev = NULL;
    rnode->Head = lnode;
    rnode->Tail = lnode;
    rnode->Info.NumResources = 1;
    rnode->Info.ResName.IsName = ( (name_info.id & 0x8000) == 0 );
    rnode->Info.ResName.ID.Num = (name_info.id & 0x7fff);

    lnode->Next = NULL;
    lnode->Prev = NULL;
    lnode->data = NULL;
    lnode->Info.MemoryFlags = name_info.flags;
    offset_32 = (uint_32)name_info.offset;
    length_32 = (uint_32)name_info.length;
    lnode->Info.Offset = (uint_32)(offset_32 << align);
    lnode->Info.Length = (uint_32)(length_32 << align);
    lnode->Info.lang.lang = DEF_LANG;
    lnode->Info.lang.sublang = DEF_SUBLANG;

    return( rnode );
}

bool WRReadResourceNames( WResDir dir, FILE *fp, uint_32 name_offset )
{
    uint_8      name_len;
    char        *name;
    bool        end_of_names;

    end_of_names = false;

    ResReadUint8( &name_len, fp );

    while( !end_of_names ) {
        if( name_len == 0 ) {
            ResReadUint8( &name_len, fp );
            if( name_len == 0 ) {
                end_of_names = true;
            } else {
                name_offset++;
            }
        } else {
            name = (char *)MemAlloc( name_len + 1 );
            if( RESREAD( fp, name, name_len ) != name_len ) {
                return( false );
            }
            name[name_len] = 0;
            WRSetResName( dir, name_offset, name );
            MemFree( name );
            name_offset = name_offset + name_len + 1;
            ResReadUint8( &name_len, fp );
        }
    }

    return( true );
}

bool WRSetResName( WResDir dir, uint_32 offset, const char *name )
{
    WResTypeNode *type_node;
    WResResNode  *res_node;
    bool         found_one;

    found_one = false;
    for( type_node = dir->Head; type_node != NULL; type_node = type_node->Next ) {
        if( type_node->Info.TypeName.IsName && type_node->Info.TypeName.ID.Num == offset ) {
            type_node = WRRenameWResTypeNode( dir, type_node, name );
            if( type_node == NULL ) {
                return( false );
            } else {
                found_one = true;
            }
        }
        for( res_node = type_node->Head; res_node != NULL; res_node = res_node->Next ) {
            if( res_node->Info.ResName.IsName && res_node->Info.ResName.ID.Num == offset ) {
                res_node = WRRenameWResResNode( type_node, res_node, name );
                if( res_node == NULL ) {
                    return( false );
                } else {
                    found_one = true;
                }
            }
            if( res_node == type_node->Tail ) {
                break;
            }
        }
        if( type_node == dir->Tail ) {
            break;
        }
    }

    return( found_one );
}

WResTypeNode *WRRenameWResTypeNode( WResDir dir, WResTypeNode *type_node, const char *name )
{
    WResTypeNode    *new_type_node;
    size_t          len;

    len = strlen( name );
    new_type_node = (WResTypeNode *)MemAlloc( sizeof( WResTypeNode ) + len - 1 );
    if( new_type_node == NULL ) {
        return( NULL );
    }
    if( dir->Head == type_node ) {
        dir->Head = new_type_node;
    }
    if( dir->Tail == type_node ) {
        dir->Tail = new_type_node;
    }
    new_type_node->Next = type_node->Next;
    new_type_node->Prev = type_node->Prev;
    new_type_node->Head = type_node->Head;
    new_type_node->Tail = type_node->Tail;
    new_type_node->Info.NumResources = type_node->Info.NumResources;
    new_type_node->Info.TypeName.IsName = true;
    new_type_node->Info.TypeName.ID.Name.NumChars = len;
    memcpy( new_type_node->Info.TypeName.ID.Name.Name, name, len );
    if( type_node->Prev != NULL ) {
        type_node->Prev->Next = new_type_node;
    }
    if( type_node->Next != NULL ) {
        type_node->Next->Prev = new_type_node;
    }
    MemFree( type_node );
    return( new_type_node );
}

WResResNode *WRRenameWResResNode( WResTypeNode *type_node,
                                  WResResNode *res_node, const char *name )
{
    WResResNode *new_res_node;
    size_t      len;
    size_t      len1;

    len = strlen( name );
    len1 = len - 1;
    if( len == 0 )
        len1 = 0;
    new_res_node = (WResResNode *)MemAlloc( sizeof( WResResNode ) + len1 );
    if( new_res_node == NULL ) {
        return( NULL );
    }
    if( type_node->Head == res_node ) {
        type_node->Head = new_res_node;
    }
    if( type_node->Tail == res_node ) {
        type_node->Tail = new_res_node;
    }
    new_res_node->Head = res_node->Head;
    new_res_node->Tail = res_node->Tail;
    new_res_node->Next = res_node->Next;
    new_res_node->Prev = res_node->Prev;
    new_res_node->Info.ResName.IsName = true;
    new_res_node->Info.ResName.ID.Name.NumChars = len;
    memcpy( new_res_node->Info.ResName.ID.Name.Name, name, len );
    if( res_node->Prev != NULL ) {
        res_node->Prev->Next = new_res_node;
    }
    if( res_node->Next != NULL ) {
        res_node->Next->Prev = new_res_node;
    }
    MemFree( res_node );
    return( new_res_node );
}

uint_32 WRReadNameTable( WResDir dir, FILE *fp, uint_8 **name_table,
                         uint_32 num_leftover, uint_8 *leftover )
{
    static WResTypeNode *type_node;
    static WResResNode  *res_node;
    static uint_32      res_len;
    static uint_32      res_offset;
    static uint_32      num_read;
    uint_32             len;

    if( dir != NULL ) {
        type_node = WRFindTypeNode( dir, RESOURCE2INT( RT_NAMETABLE ), NULL );
        if( type_node != NULL ) {
            res_node = type_node->Head;
            /* if there are two name tables we ignore all but the first */
            res_len = res_node->Head->Info.Length;
            res_offset = res_node->Head->Info.Offset;
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

    /* If there was data left over but we have a NULL pointer to the
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

    *name_table = (uint_8 *)MemAlloc( len + num_leftover );
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
            *leftover = (uint_8 *)MemAlloc( num_leftover );
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
    WResTypeNode *type_node;
    WResResNode  *res_node;

    for( type_node = dir->Head; type_node != NULL; type_node = type_node->Next ) {
        if( !type_node->Info.TypeName.IsName && type_node->Info.TypeName.ID.Num == entry->type ) {
            for( res_node = type_node->Head; res_node != NULL; res_node = res_node->Next ) {
                if( !res_node->Info.ResName.IsName && res_node->Info.ResName.ID.Num == (entry->id & 0x7fff) ) {
                    if( WRRenameWResResNode( type_node, res_node, entry->name ) == NULL ) {
                        return( false );
                    } else {
                        return( true );
                    }
                }
                if( res_node == type_node->Tail ) {
                    break;
                }
            }
        }
        if( type_node == dir->Tail ) {
            break;
        }
    }

    return( false );
}
