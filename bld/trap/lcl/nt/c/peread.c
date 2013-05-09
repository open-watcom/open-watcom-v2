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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "stdnt.h"

/*
 * SeekRead - seek to a specified spot in the file, and read some data
 */
BOOL SeekRead( HANDLE handle, DWORD newpos, void *buff, WORD size )
{
    int     rc;
    DWORD   bytes;

    rc = SetFilePointer( handle, newpos, NULL, FILE_BEGIN );
    if( rc == INVALID_SET_FILE_POINTER ) {
        return( FALSE );
    }
    rc = ReadFile( handle, buff, size, &bytes, NULL );
    if( !rc ) {
        return( FALSE );
    }
    if( bytes != size ) {
        return( FALSE );
    }
    return( TRUE );

}

/*
 * GetEXEHeader - get type of EXE
 */
int GetEXEHeader( HANDLE handle, header_info *hi, WORD *stack )
{
    WORD    data;
    WORD    sig;
    DWORD   nh_offset;

    if( !SeekRead( handle, 0x00, &data, sizeof( data ) ) ) {
        return( FALSE );
    }
    if( data != EXE_MZ ) {
        return( FALSE );
    }

    //    if( !SeekRead( handle, 0x18, &data, sizeof( data ) ) ) {
    //      return( FALSE );
    //    }
    //    if( data < 0x40 ) {
    //      return( FALSE );
    //    }

    if( !SeekRead( handle, 0x3c, &nh_offset, sizeof( unsigned_32 ) ) ) {
        return( FALSE );
    }

    if( !SeekRead( handle, nh_offset, &sig, sizeof( sig ) ) ) {
        sig = 0;
    }
    hi->sig = sig;
    if( sig == EXE_PE ) {
        return( SeekRead( handle, nh_offset, &hi->u.peh, sizeof( exe_pe_header ) ) );
    }
#if defined( MD_x86 )
    if( sig == EXE_NE ) {
        if( !SeekRead( handle, nh_offset, &hi->u.neh, sizeof( os2_exe_header ) ) )
        {
            return( FALSE );
        }
        if( hi->u.neh.target == TARGET_WINDOWS ) {
            DWORD       off;
            int         len;
            DWORD       bytes;
            DWORD       pos;

            off = nh_offset + hi->u.neh.resident_off;
            if( SetFilePointer( handle, off, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
                return( FALSE );
            }
            if( !ReadFile( handle, &len, sizeof( len ), &bytes, NULL ) ) {
                return( FALSE );
            }
            if( len > sizeof( hi->modname ) - 1 ) {
                len = sizeof( hi->modname ) - 1;
            }
            if( !ReadFile( handle, hi->modname, len, &bytes, NULL ) ) {
                return( FALSE );
            }
            hi->modname[len] = 0;
            pos = nh_offset + hi->u.neh.segment_off + 
                ( hi->u.neh.adsegnum - 1 ) * sizeof( segment_record ) +
                offsetof( segment_record, min );
            if( !SeekRead( handle, pos, stack, sizeof( *stack ) ) ) {
                return( FALSE );
            }
            *stack += hi->u.neh.stack;
            return( TRUE );
        }
        return( FALSE );
    }
    hi->sig = EXE_MZ;
    return( TRUE );
#elif defined( MD_x64 )
    return( FALSE );
#elif defined( MD_axp ) || defined( MD_ppc )
    return( FALSE );
#else
    #error GetEXEHeader not configured
#endif
}

int GetModuleName( HANDLE fhdl, char *name )
{
    header_info         hi;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               lenread;
    DWORD               export_rva;
    DWORD               i;
    char                buf[_MAX_PATH];
    WORD                stack;
    int                 num_objects;
    DWORD               seek_offset;

    if( !GetEXEHeader( fhdl, &hi, &stack ) ) {
        return( FALSE );
    }
    if( hi.sig != EXE_PE ) {
        return( FALSE );
    }
    seek_offset = SetFilePointer( fhdl, 0, NULL, FILE_CURRENT );
    if( seek_offset == INVALID_SET_FILE_POINTER ) {
        return( FALSE );
    }
    if( IS_PE64( hi.u.peh ) ) {
        export_rva = PE64( hi.u.peh ).table[PE_TBL_EXPORT].rva;
        num_objects = PE64( hi.u.peh ).num_objects;
        seek_offset += PE64( hi.u.peh ).nt_hdr_size + offsetof( pe_header64, magic ) - sizeof( exe_pe_header );
    } else {
        export_rva = PE32( hi.u.peh ).table[PE_TBL_EXPORT].rva;
        num_objects = PE32( hi.u.peh ).num_objects;
        seek_offset += PE32( hi.u.peh ).nt_hdr_size + offsetof( pe_header, magic ) - sizeof( exe_pe_header );
    }
    if( num_objects == 0 ) {
        return( FALSE );
    }
    /* position to begining of object table */
    if( SetFilePointer( fhdl, seek_offset, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( FALSE );
    }
    for( i = 0; i < num_objects; i++ ) {
        if( !ReadFile( fhdl, &obj, sizeof( obj ), &lenread, NULL ) || lenread != sizeof( obj ) ) {
            return( FALSE );
        }
        if( export_rva >= obj.rva && export_rva < obj.rva + obj.physical_size ) {
            break;
        }
    }
    if( i == num_objects ) {
        return( FALSE );
    }
    if( !SeekRead( fhdl, obj.physical_offset + export_rva - obj.rva, &expdir, sizeof( expdir ) ) ) {
        return( FALSE );
    }
    if( !SeekRead( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, buf, _MAX_PATH ) ) {
        return( FALSE );
    }
    if( SetFilePointer( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( FALSE );
    }
    if( !ReadFile( fhdl, buf, _MAX_PATH, &lenread, NULL ) ) {
        return( FALSE );
    }
    memcpy( name, buf, lenread );
    name[lenread] = '\0';
    return( TRUE );
}

#if 0
int CpFile( HANDLE in )
{
    HANDLE  out;
    char    buff[1024];
    DWORD   lenread;
    DWORD   old;
    int     rc;

    out = CreateFile( (LPTSTR)"CP.OUT", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if( out == INVALID_HANDLE_VALUE ) {
        return( 1 );
    }
    old = SetFilePointer( in, 0, NULL, FILE_CURRENT );
    SetFilePointer( in, 0, NULL, FILE_BEGIN );
    rc = 0;
    for( ;; ) {
        if( !ReadFile( in, buff, sizeof( buff ), &lenread, NULL ) ) {
            rc = 2;
            break;
        }
        if( lenread == 0 ) {
            break;
        }
        if( !WriteFile( out, buff, lenread, &lenread, NULL ) ) {
            rc = 3;
            break;
        }
    }
    SetFilePointer( in, old, NULL, FILE_BEGIN );
    CloseHandle( out );
    return( rc );
}
#endif
