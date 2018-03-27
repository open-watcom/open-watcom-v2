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


#include "drwatcom.h"
#include "exepe.h"
#include "digcli.h"


#define EXE_PE  0x4550
#define EXE_NE  0x454e
#define EXE_MZ  0x5a4d

/*
 * seekRead
 */
static BOOL seekRead( FILE *fp, DWORD offset, void *buf, DWORD size )
{
    if( DIGCli( Seek )( fp, offset, DIG_ORG ) ) {
        return( FALSE );
    }
    if( DIGCli( Read )( fp, buf, size ) != size ) {
        return( FALSE );
    }
    return( TRUE );
}

/*
 * getEXEHeader - verify that this is a PE executable and read the header
 */
static BOOL getEXEHeader( FILE *fp, pe_header *hdr )
{
    WORD        sig;
    DWORD       nh_offset;

    if( !seekRead( fp, 0x00, &sig, sizeof( sig ) ) ) {
        return( FALSE );
    }
    if( sig != EXE_MZ ) {
        return( FALSE );
    }

    if( !seekRead( fp, 0x3c, &nh_offset, sizeof( DWORD ) ) ) {
        return( FALSE );
    }

    if( !seekRead( fp, nh_offset, &sig, sizeof( sig ) ) ) {
        return( FALSE );
    }
    if( sig == EXE_PE ) {
        if( !seekRead( fp, nh_offset, hdr, sizeof( pe_header ) ) ) {
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );
}

/*
 * GetSegmentList
 */
BOOL GetSegmentList( ModuleNode *node )
{
    pe_header           header;
    pe_object           obj;
    WORD                i;

    if( !getEXEHeader( node->fp, &header ) )
        return( FALSE );
    node->syminfo = MemAlloc( sizeof( SymInfoNode ) + header.num_objects * sizeof( SegInfo ) );
    node->syminfo->segcnt = header.num_objects;
    for( i = 0; i < header.num_objects; i++ ) {
        if( DIGCli( Read )( node->fp, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( FALSE );
        }
        node->syminfo->seginfo[i].segoff = obj.rva + node->base;
        if ( obj.flags & PE_OBJ_CODE ){
            node->syminfo->seginfo[i].code = TRUE;
        } else {
            node->syminfo->seginfo[i].code = FALSE;
        }
    }
    return( TRUE );
}

/*
 * GetModuleName - get the name of a module from its Export directory table
 */
char *GetModuleName( FILE *fp )
{
    pe_header           header;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               export_rva;
    DWORD               i;
    char                buf[_MAX_PATH];
    char                *ret;

    if( !getEXEHeader( fp, &header ) )
        return( NULL );
    export_rva = header.table[ PE_TBL_EXPORT ].rva;
    for( i = 0; i < header.num_objects; i++ ) {
        if( DIGCli( Read )( fp, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( NULL );
        }
        if( export_rva >= obj.rva && export_rva < obj.rva + obj.physical_size ) {
            break;
        }
    }
    if( i == header.num_objects )
        return( NULL );
    if( !seekRead( fp, obj.physical_offset + export_rva - obj.rva , &expdir, sizeof( expdir ) ) ) {
        return( NULL );
    }
    if( !seekRead( fp, obj.physical_offset + expdir.name_rva - obj.rva, buf, _MAX_PATH ) ) {
        return( NULL );
    }
    ret = MemAlloc( strlen( buf ) + 1 );
    strcpy( ret, buf );
    return( ret );
}

BOOL GetModuleSize( FILE *fp, DWORD *size )
{
    pe_header           header;

    if( !getEXEHeader( fp, &header ) )
        return( FALSE );
    *size = header.image_size;
    return( TRUE );
}

ObjectInfo *GetModuleObjects( FILE *fp, DWORD *num_objects )
{
    pe_header           header;
    pe_object           obj;
    ObjectInfo          *ret;
    DWORD               i;

    if( !getEXEHeader( fp, &header ) )
        return( NULL );
    ret = MemAlloc( header.num_objects * sizeof( ObjectInfo ) );
    for( i = 0; i < header.num_objects; i++ ) {
        if( DIGCli( Read )( fp, &obj, sizeof( obj ) ) != sizeof( obj ) )
            break;
        ret[i].rva = obj.rva;
        strcpy( ret[i].name, obj.name );
    }
    *num_objects = i;
    return( ret );
}
