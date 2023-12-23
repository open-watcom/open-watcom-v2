/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "exedos.h"
#include "exepe.h"
#include "digcli.h"


/*
 * seekRead
 */
static bool seekRead( FILE *fp, DWORD offset, void *buf, DWORD size )
{
    if( DIGCli( Seek )( fp, offset, DIG_SEEK_ORG ) ) {
        return( false );
    }
    if( DIGCli( Read )( fp, buf, size ) != size ) {
        return( false );
    }
    return( true );
}

/*
 * getEXEHeader - verify that this is a PE executable and read the header
 */
static bool getEXEHeader( FILE *fp, pe_exe_header *pehdr )
{
    unsigned_16     data;
    unsigned_32     ne_header_off;

    if( !seekRead( fp, 0, &data, sizeof( data ) )
      || data != EXESIGN_DOS ) {
        return( false );
    }

    if( !seekRead( fp, NE_HEADER_OFFSET, &ne_header_off, sizeof( ne_header_off ) ) ) {
        return( false );
    }
    if( DIGCli( Seek )( fp, ne_header_off, DIG_SEEK_ORG ) ) {
        return( false );
    }
    if( DIGCli( Read )( fp, pehdr, PE_HDR_SIZE ) != PE_HDR_SIZE
      || ( pehdr->signature != EXESIGN_PE )
      || DIGCli( Read )( fp, (char *)pehdr + PE_HDR_SIZE, PE_OPT_SIZE( *pehdr ) ) != PE_OPT_SIZE( *pehdr ) ) {
        return( false );
    }
    return( true );
}

/*
 * GetSegmentList
 */
bool GetSegmentList( ModuleNode *node )
{
    pe_exe_header       pehdr;
    pe_object           obj;
    unsigned            i;
    unsigned            num_objects;

    if( !getEXEHeader( node->fp, &pehdr ) )
        return( false );
    num_objects = pehdr.fheader.num_objects;
    node->syminfo = MemAlloc( sizeof( SymInfoNode ) + num_objects * sizeof( SegInfo ) );
    node->syminfo->segcnt = num_objects;
    for( i = 0; i < num_objects; i++ ) {
        if( DIGCli( Read )( node->fp, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( false );
        }
        node->syminfo->seginfo[i].segoff = obj.rva + node->base;
        if ( obj.flags & PE_OBJ_CODE ){
            node->syminfo->seginfo[i].code = TRUE;
        } else {
            node->syminfo->seginfo[i].code = FALSE;
        }
    }
    return( true );
}

/*
 * GetModuleName - get the name of a module from its Export directory table
 */
char *GetModuleName( FILE *fp )
{
    pe_exe_header       pehdr;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               export_rva;
    char                buf[_MAX_PATH];
    char                *ret;
    unsigned            i;
    unsigned            num_objects;

    if( !getEXEHeader( fp, &pehdr ) )
        return( NULL );
    export_rva = PE_DIRECTORY( pehdr, PE_TBL_EXPORT ).rva;
    num_objects = pehdr.fheader.num_objects;
    for( i = 0; i < num_objects; i++ ) {
        if( DIGCli( Read )( fp, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( NULL );
        }
        if( export_rva >= obj.rva && export_rva < obj.rva + obj.physical_size ) {
            break;
        }
    }
    if( i == num_objects )
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

bool GetModuleSize( FILE *fp, DWORD *size )
{
    pe_exe_header       pehdr;

    if( !getEXEHeader( fp, &pehdr ) )
        return( false );
    *size = PE( pehdr, image_size );
    return( true );
}

ObjectInfo *GetModuleObjects( FILE *fp, unsigned *objects_num )
{
    pe_exe_header       pehdr;
    pe_object           obj;
    ObjectInfo          *ret;
    unsigned            i;
    unsigned            num_objects;

    if( !getEXEHeader( fp, &pehdr ) )
        return( NULL );
    num_objects = pehdr.fheader.num_objects;
    ret = MemAlloc( num_objects * sizeof( ObjectInfo ) );
    for( i = 0; i < num_objects; i++ ) {
        if( DIGCli( Read )( fp, &obj, sizeof( obj ) ) != sizeof( obj ) )
            break;
        ret[i].rva = obj.rva;
        strcpy( ret[i].name, obj.name );
    }
    *objects_num = i;
    return( ret );
}
