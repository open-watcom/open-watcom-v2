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


#include <wwindows.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrrdw16.h"
#include "wrmsg.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define NAME_TABLE_MAX   8192
#define WR_RT_NAMETABLE  15

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
static int          WRLoadWResDirFromWin16EXE( WResFileID, WResDir * );
static int          WRIsHeaderValidWIN16( os2_exe_header * );
static int          WRWin16HeaderHasResourceTable( os2_exe_header * );
static WResTypeNode *WRReadWResTypeNodeFromExe( WResFileID, uint_16 );
static WResResNode  *WRReadWResResNodeFromExe( WResFileID, uint_16 );
static int          WRReadResourceNames( WResDir, WResFileID, uint_32 );
static int          WRSetResName( WResDir, uint_32, char * );
static WResTypeNode *WRRenameWResTypeNode( WResDir, WResTypeNode *, char * );
static WResResNode  *WRRenameWResResNode( WResTypeNode *, WResResNode *, char * );
static uint_32      WRReadNameTable( WResDir, WResFileID, uint_8 **, uint_32, uint_8 * );
static uint_32      WRUseNameTable( WResDir, uint_8 *, uint_32, uint_8 ** );
static int          WRSetResNameFromNameTable( WResDir, WRNameTableEntry * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

int WRLoadResourceFromWin16EXE( WRInfo *info )
{
    WResFileID  file_handle;
    int         ok;

    ok = ((file_handle = ResOpenFileRO( info->file_name )) != -1);

    if( ok ) {
        ok = WRLoadWResDirFromWin16EXE( file_handle, &info->dir );
    }

    if( file_handle != -1 ) {
        ResCloseFile( file_handle );
    }

    return( ok );
}

long int WRReadWin16ExeHeader( WResFileID file_handle, os2_exe_header *header )
{
    long int    old_pos;
    uint_16     offset;
    int         ok;

    old_pos = -1;

    ok = (file_handle != -1 && header != NULL);

    if( ok ) {
        ok = ((old_pos = ResSeek( file_handle, 0x18, SEEK_SET )) != -1);
    }

    /* check the reloc offset */
    if( ok ) {
        ResReadUint16( &offset, file_handle );
        ok = (offset >= 0x0040);
    }

    if( ok ) {
        ok = (ResSeek( file_handle, OS2_NE_OFFSET, SEEK_SET ) != -1);
    }

    /* check header offset */
    if( ok ) {
        ResReadUint16( &offset, file_handle );
        ok = (offset != 0x0000);
    }

    if( ok ) {
        ok = (ResSeek( file_handle, offset, SEEK_SET ) != -1);
    }

    if( ok ) {
        ok = (read( file_handle, header, sizeof( os2_exe_header ) ) ==
              sizeof( os2_exe_header ));
    }

    /* check for valid Win16 EXE */
    if( ok ) {
        ok = WRIsHeaderValidWIN16( header );
    }

    if( old_pos != -1 ) {
        ok = (ResSeek( file_handle, old_pos, SEEK_SET ) != -1 && ok);
    }

    if( ok ) {
        return( offset );
    } else {
        return( 0 );
    }
}

int WRIsHeaderValidWIN16( os2_exe_header *header )
{
    if( header->signature == OS2_SIGNATURE_WORD && header->expver >= 0x300 ) {
        return( TRUE );
    }

    return( FALSE );
}

int WRWin16HeaderHasResourceTable( os2_exe_header *header )
{
    if( header->resource_off != header->resident_off ) {
        return( TRUE );
    }

    return( FALSE );
}

int WRLoadWResDirFromWin16EXE( WResFileID file_handle, WResDir *dir )
{
    os2_exe_header  win_header;
    long int        offset;
    uint_16         align_shift;
    uint_32         name_offset;
    WResTypeNode    *type_node;
    uint_8          *name_table;
    uint_8          *leftover;
    uint_32         name_table_len;
    uint_32         num_leftover;
    int             ok;

    ok = (file_handle != -1);

    if( ok ) {
        ok = ((*dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        ok = ((offset = WRReadWin16ExeHeader( file_handle, &win_header )) != 0);
    }

    /* check if a resource table is present */
    if( ok ) {
        ok = WRWin16HeaderHasResourceTable( &win_header );
        if( !ok ) {
            return( TRUE );
        }
    }

    if( ok ) {
        ok = (ResSeek( file_handle, offset, SEEK_SET ) != -1);
    }

    if( ok ) {
        ok = (ResSeek( file_handle, win_header.resource_off, SEEK_CUR ) != -1);
    }

    if( ok ) {
        ResReadUint16( &align_shift, file_handle );
        ok = (align_shift <= 16);
        if( !ok ) {
            WRDisplayErrorMsg( WR_BADEXE );
        }
    }

    if( ok ) {
        (*dir)->NumResources = 0;
        type_node = WRReadWResTypeNodeFromExe( file_handle, align_shift );
        while( type_node != NULL ) {
            type_node->Next = NULL;
            type_node->Prev = (*dir)->Tail;
            if( (*dir)->Tail != NULL ) {
                (*dir)->Tail->Next = type_node;
            }
            if ( (*dir)->Head == NULL ) {
                (*dir)->Head = type_node;
            }
            (*dir)->Tail = type_node;
            (*dir)->NumTypes++;
            (*dir)->NumResources += type_node->Info.NumResources;
            type_node = WRReadWResTypeNodeFromExe ( file_handle, align_shift );
        }
        name_offset = tell( file_handle ) - offset - win_header.resource_off;
        ok = WRReadResourceNames( *dir, file_handle, name_offset );
    }

    if( ok && win_header.expver <= 0x300 ) {
        num_leftover = 0;
        leftover = NULL;
        name_table_len = WRReadNameTable( *dir, file_handle,
                                          &name_table, num_leftover, leftover );
        while( name_table_len != 0 ) {
            num_leftover = WRUseNameTable( *dir, name_table, name_table_len, &leftover );
            if( name_table != NULL ) {
                WRMemFree( name_table );
            }
            name_table_len = WRReadNameTable( NULL, file_handle, &name_table,
                                              num_leftover, leftover );
            if( leftover != NULL ) {
                WRMemFree( leftover );
                leftover = NULL;
            }
        }
    }

    return( ok );
}

WResTypeNode *WRReadWResTypeNodeFromExe( WResFileID file_handle, uint_16 align_shift )
{
    uint_16         type_id;
    uint_16         resource_count;
    uint_32         reserved;
    WResTypeNode    *type_node;
    WResResNode     *res_node;

    ResReadUint16( &type_id, file_handle );
    if( type_id == 0x0000 ) {
        return( NULL );
    }

    type_node = (WResTypeNode *)WRMemAlloc( sizeof( WResTypeNode ) );
    if( type_node == NULL ) {
        return( NULL );
    }

    ResReadUint16( &resource_count, file_handle );
    ResReadUint32( &reserved, file_handle );

    type_node->Next = NULL;
    type_node->Prev = NULL;
    type_node->Head = NULL;
    type_node->Tail = NULL;
    type_node->Info.NumResources = resource_count;
    if( type_id & 0x8000 ) {
        type_node->Info.TypeName.IsName = FALSE;
    } else {
        type_node->Info.TypeName.IsName = TRUE;
    }
    type_node->Info.TypeName.ID.Num = (type_id & 0x7fff);

    for( ; resource_count != 0; resource_count-- ) {
        res_node = WRReadWResResNodeFromExe( file_handle, align_shift );
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

WResResNode *WRReadWResResNodeFromExe( WResFileID file, uint_16 align )
{
    WRNameInfo      name_info;
    uint_32         offset_32;
    uint_32         length_32;
    WResResNode     *rnode;
    WResLangNode    *lnode;

    rnode = (WResResNode *)WRMemAlloc( sizeof( WResResNode ) );
    if( rnode == NULL ) {
        return( NULL );
    }

    lnode = (WResLangNode *)WRMemAlloc( sizeof( WResLangNode ) );
    if( lnode == NULL ) {
        WRMemFree( rnode );
        return( NULL );
    }

    if( read( file, &name_info, sizeof( WRNameInfo ) ) != sizeof( WRNameInfo ) ) {
        return( NULL );
    }

    rnode->Next = NULL;
    rnode->Prev = NULL;
    rnode->Head = lnode;
    rnode->Tail = lnode;
    rnode->Info.NumResources = 1;
    if( name_info.id & 0x8000 ) {
        rnode->Info.ResName.IsName = FALSE;
    } else {
        rnode->Info.ResName.IsName = TRUE;
    }
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

int WRReadResourceNames( WResDir dir, WResFileID file_handle, uint_32 name_offset )
{
    uint_8      name_len;
    char        *name;
    int         end_of_names;

    end_of_names = FALSE;

    ResReadUint8( &name_len, file_handle );

    while( !end_of_names ) {
        if( name_len == 0 ) {
            ResReadUint8( &name_len, file_handle );
            if( name_len == 0 ) {
                end_of_names = TRUE;
            } else {
                name_offset++;
            }
        } else {
            name = (char *)WRMemAlloc( name_len + 1 );
            if( read( file_handle, name, name_len ) != name_len ) {
                return( FALSE );
            }
            name[name_len] = 0;
            WRSetResName( dir, name_offset, name );
            WRMemFree( name );
            name_offset = name_offset + name_len + 1;
            ResReadUint8( &name_len, file_handle );
        }
    }

    return( TRUE );
}

int WRSetResName( WResDir dir, uint_32 offset, char *name )
{
    WResTypeNode *type_node;
    WResResNode  *res_node;
    int          found_one;

    found_one = FALSE;
    type_node = dir->Head;
    while( type_node != NULL ) {
        if( type_node->Info.TypeName.IsName && type_node->Info.TypeName.ID.Num == offset ) {
            type_node = WRRenameWResTypeNode( dir, type_node, name );
            if( type_node == NULL ) {
                return( FALSE );
            } else {
                found_one = TRUE;
            }
        }
        res_node = type_node->Head;
        while( res_node != NULL ) {
            if( res_node->Info.ResName.IsName && res_node->Info.ResName.ID.Num == offset ) {
                res_node = WRRenameWResResNode( type_node, res_node, name );
                if( res_node == NULL ) {
                    return( FALSE );
                } else {
                    found_one = TRUE;
                }
            }
            if( res_node == type_node->Tail ) {
                break;
            }
            res_node = res_node->Next;
        }
        if( type_node == dir->Tail ) {
            break;
        }
        type_node = type_node->Next;
    }

    return( found_one );
}

WResTypeNode *WRRenameWResTypeNode( WResDir dir, WResTypeNode *type_node, char *name )
{
    WResTypeNode    *new_type_node;
    int             len;

    len = strlen( name );
    new_type_node = (WResTypeNode *)WRMemAlloc( sizeof( WResTypeNode ) + len - 1 );
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
    new_type_node->Info.TypeName.IsName = TRUE;
    new_type_node->Info.TypeName.ID.Name.NumChars = len;
    memcpy( new_type_node->Info.TypeName.ID.Name.Name, name, len );
    if( type_node->Prev != NULL ) {
        type_node->Prev->Next = new_type_node;
    }
    if( type_node->Next != NULL ) {
        type_node->Next->Prev = new_type_node;
    }
    WRMemFree( type_node );
    return( new_type_node );
}

WResResNode *WRRenameWResResNode( WResTypeNode *type_node,
                                  WResResNode *res_node, char *name )
{
    WResResNode *new_res_node;
    int         len;

    len = strlen( name );
    new_res_node = (WResResNode *)WRMemAlloc( sizeof( WResResNode ) + max( 0, len - 1 ) );
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
    new_res_node->Info.ResName.IsName = TRUE;
    new_res_node->Info.ResName.ID.Name.NumChars = len;
    memcpy( new_res_node->Info.ResName.ID.Name.Name, name, len );
    if( res_node->Prev != NULL ) {
        res_node->Prev->Next = new_res_node;
    }
    if( res_node->Next != NULL ) {
        res_node->Next->Prev = new_res_node;
    }
    WRMemFree( res_node );
    return( new_res_node );
}

uint_32 WRReadNameTable( WResDir dir, WResFileID file_handle, uint_8 **name_table,
                         uint_32 num_leftover, uint_8 *leftover )
{
    static WResTypeNode *type_node;
    static WResResNode  *res_node;
    static uint_32      res_len;
    static uint_32      res_offset;
    static uint_32      num_read;
    uint_32             len;

    if( dir != NULL ) {
        type_node = WRFindTypeNode( dir, (uint_16)WR_RT_NAMETABLE, NULL );
        if( type_node != NULL ) {
            res_node = type_node->Head;
            /* if there are two name tables we ignore all but the first */
            res_len = res_node->Head->Info.Length;
            res_offset = res_node->Head->Info.Offset;
            if( ResSeek( file_handle, res_offset, SEEK_SET ) == -1 ) {
                return( FALSE );
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
        if( ResSeek( file_handle, num_leftover, SEEK_CUR ) == -1 ) {
            return( 0 );
        }
        num_read += num_leftover;
        num_leftover = 0;
    }

    *name_table = (uint_8 *)WRMemAlloc( len + num_leftover );
    if( *name_table == NULL ) {
        return( FALSE );
    }

    if( num_leftover != 0 ) {
        memcpy( *name_table, leftover, num_leftover );
    }

    if( read( file_handle, *name_table + num_leftover, len ) != len ) {
        /* hmmmm... the read failed */
    }

    return( len + num_leftover );
}

uint_32 WRUseNameTable( WResDir dir, uint_8 *name_table, uint_32 len, uint_8 **leftover )
{
    WRNameTableEntry    *entry;
    uint_32             name_pos;
    uint_32             num_leftover;

    name_pos = 0;
    num_leftover = 0;
    while( name_pos < len ) {
        entry = (WRNameTableEntry *)(name_table + name_pos);
        if( entry->length == 0 ) {
            break;
        }
        if( name_pos + entry->length > len ) {
            num_leftover = len - name_pos;
            *leftover = (uint_8 *)WRMemAlloc( num_leftover );
            if( *leftover != NULL ) {
                memcpy( *leftover, entry, num_leftover );
            } else {
                /* leftover alloc failed! */
            }
            break;
        }
        WRSetResNameFromNameTable( dir, entry );
        name_pos += entry->length;
    }

    return( num_leftover );
}

int WRSetResNameFromNameTable( WResDir dir, WRNameTableEntry *entry )
{
    WResTypeNode *type_node;
    WResResNode  *res_node;

    type_node = dir->Head;
    while( type_node != NULL ) {
        if( !type_node->Info.TypeName.IsName &&
            type_node->Info.TypeName.ID.Num == entry->type ) {
            res_node = type_node->Head;
            while( res_node != NULL ) {
                if( !res_node->Info.ResName.IsName &&
                    res_node->Info.ResName.ID.Num == (entry->id & 0x7fff) ) {
                    if( WRRenameWResResNode( type_node, res_node, entry->name ) == NULL ) {
                        return( FALSE );
                    } else {
                        return( TRUE );
                    }
                }
                if( res_node == type_node->Tail ) {
                    break;
                }
                res_node = res_node->Next;
            }
        }
        if( type_node == dir->Tail ) {
            break;
        }
        type_node = type_node->Next;
    }

    return( FALSE );
}
