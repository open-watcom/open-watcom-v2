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


#include <windows.h>
#include <stdlib.h>
#include "drwatcom.h"
#include "exepe.h"

#define EXE_PE  0x4550
#define EXE_NE  0x454e
#define EXE_MZ  0x5a4d

/*
 * seekRead
 */
static BOOL seekRead( HANDLE fhdl, DWORD offset, void *buf, DWORD size ) {

    DWORD       lenread;

    if( SetFilePointer( fhdl, offset, NULL, FILE_BEGIN ) == -1 ) {
        return( FALSE );
    }
    if( !ReadFile( fhdl, buf, size, &lenread, NULL )  || lenread != size ) {
        return( FALSE );
    }
    return( TRUE );
}

/*
 * getEXEHeader - verify that this is a PE executable and read the header
 */
static BOOL getEXEHeader( HANDLE fhdl, pe_header *hdr ) {

    WORD        sig;
    DWORD       nh_offset;

    if( !seekRead( fhdl, 0x00, &sig, sizeof( sig ) ) ) {
        return( FALSE );
    }
    if( sig != EXE_MZ ) {
        return( FALSE );
    }

    if( !seekRead( fhdl, 0x3c, &nh_offset, sizeof( DWORD ) ) ) {
        return( FALSE );
    }

    if( !seekRead( fhdl, nh_offset, &sig, sizeof( sig ) ) ) {
        return( FALSE );
    }
    if( sig == EXE_PE ) {
        if( !seekRead( fhdl, nh_offset, hdr, sizeof( pe_header ) ) ) {
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );
}

/*
 * GetSegmentList
 */
BOOL GetSegmentList( ModuleNode *node ) {

    pe_header           header;
    pe_object           obj;
    WORD                i;
    DWORD               lenread;

    if( !getEXEHeader( node->fhdl, &header ) ) return( FALSE );
    node->syminfo = MemAlloc( sizeof( SymInfoNode )
                              + header.num_objects * sizeof( DWORD ) );
    node->syminfo->segcnt = header.num_objects;
    for( i=0; i < header.num_objects; i++ ) {
        if( !ReadFile( node->fhdl, &obj, sizeof( obj ), &lenread, NULL )
            || lenread != sizeof( obj ) ) {
            return( FALSE );
        }
        node->syminfo->segoff[i] = obj.rva + node->base;
    }
    return( TRUE );
}

/*
 * GetModuleName - get the name of a module from its Export directory table
 */
char *GetModuleName( HANDLE fhdl ) {

    pe_header           header;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               lenread;
    DWORD               export_rva;
    DWORD               i;
    char                buf[_MAX_PATH];
    char                *ret;

    if( !getEXEHeader( fhdl, &header ) ) return( NULL );
    export_rva = header.table[ PE_TBL_EXPORT ].rva;
    for( i=0; i < header.num_objects; i++ ) {
        if( !ReadFile( fhdl, &obj, sizeof( obj ), &lenread, NULL )
            || lenread != sizeof( obj ) ) {
            return( NULL );
        }
        if( export_rva >= obj.rva && export_rva < obj.rva + obj.physical_size ) {
            break;
        }
    }
    if( i == header.num_objects ) return( NULL );
    if( !seekRead( fhdl, obj.physical_offset, &expdir, sizeof( expdir ) ) ) {
        return( NULL );
    }
    if( !seekRead( fhdl, obj.physical_offset + expdir.name_rva - obj.rva,
                   buf, _MAX_PATH ) ) {
        return( NULL );
    }
    if( SetFilePointer( fhdl, obj.physical_offset + expdir.name_rva - obj.rva,
                        NULL, FILE_BEGIN ) == -1 ) {
        return( NULL );
    }
    if( !ReadFile( fhdl, buf, _MAX_PATH, &lenread, NULL ) ) {
        return( NULL);
    }
    ret = MemAlloc( strlen( buf ) + 1 );
    strcpy( ret, buf );
    return( ret );
}

BOOL GetModuleSize( HANDLE fhdl, DWORD *size ) {

    pe_header           header;

    if( !getEXEHeader( fhdl, &header ) ) return( FALSE );
    *size = header.image_size;
    return( TRUE );
}

ObjectInfo *GetModuleObjects( HANDLE fhdl, DWORD *num_objects ) {

    pe_header           header;
    pe_object           obj;
    ObjectInfo          *ret;
    DWORD               i;
    DWORD               lenread;

    if( !getEXEHeader( fhdl, &header ) ) return( NULL );
    ret = MemAlloc( header.num_objects * sizeof( ObjectInfo ) );
    for( i=0; i < header.num_objects; i++ ) {
        if( !ReadFile( fhdl, &obj, sizeof( obj ), &lenread, NULL ) ) break;
        if( lenread != sizeof( obj ) ) break;
        ret[i].rva = obj.rva;
        strcpy( ret[i].name, obj.name );
    }
    *num_objects = i;
    return( ret );
}
