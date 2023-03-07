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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "madconf.h"
#include "stdnt.h"

/*
 * SeekRead - seek to a specified spot in the file, and read some data
 */
bool SeekRead( HANDLE handle, DWORD newpos, void *buff, WORD size )
{
    int     rc;
    DWORD   bytes;

    if( SetFilePointer( handle, newpos, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    rc = ReadFile( handle, buff, size, &bytes, NULL );
    if( !rc ) {
        return( false );
    }
    if( bytes != size ) {
        return( false );
    }
    return( true );

}

/*
 * GetEXEHeader - get type of EXE
 */
bool GetEXEHeader( HANDLE handle, header_info *hi, WORD *stack )
{
    WORD    data;
    WORD    sig;
    DWORD   nh_offset;

    if( !SeekRead( handle, 0x00, &data, sizeof( data ) ) ) {
        return( false );
    }
    if( data != EXE_MZ ) {
        return( false );
    }

    //    if( !SeekRead( handle, 0x18, &data, sizeof( data ) ) ) {
    //      return( false );
    //    }
    //    if( data < 0x40 ) {
    //      return( false );
    //    }

    if( !SeekRead( handle, 0x3c, &nh_offset, sizeof( unsigned_32 ) ) ) {
        return( false );
    }

    if( !SeekRead( handle, nh_offset, &sig, sizeof( sig ) ) ) {
        sig = 0;
    }
    hi->sig = sig;
    if( sig == EXE_PE ) {
        return( SeekRead( handle, nh_offset, &hi->u.peh, sizeof( exe_pe_header ) ) );
    }
#if MADARCH & MADARCH_X86
    if( sig == EXE_NE ) {
        if( !SeekRead( handle, nh_offset, &hi->u.neh, sizeof( os2_exe_header ) ) ) {
            return( false );
        }
        if( hi->u.neh.target == TARGET_WINDOWS ) {
            DWORD           off;
            unsigned char   len;
            DWORD           bytes;
            DWORD           pos;

            off = nh_offset + hi->u.neh.resident_off;
            if( SetFilePointer( handle, off, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
                return( false );
            }
            if( !ReadFile( handle, &len, sizeof( len ), &bytes, NULL ) ) {
                return( false );
            }
            if( len > sizeof( hi->modname ) - 1 ) {
                len = sizeof( hi->modname ) - 1;
            }
            if( !ReadFile( handle, hi->modname, len, &bytes, NULL ) ) {
                return( false );
            }
            hi->modname[len] = 0;
            pos = nh_offset + hi->u.neh.segment_off +
                ( hi->u.neh.adsegnum - 1 ) * sizeof( segment_record ) +
                offsetof( segment_record, min );
            if( !SeekRead( handle, pos, stack, sizeof( *stack ) ) ) {
                return( false );
            }
            *stack += hi->u.neh.stack;
            return( true );
        }
        return( false );
    }
    hi->sig = EXE_MZ;
    return( true );
#elif MADARCH & MADARCH_X64
    /* unused parameters */ (void)stack;
    return( false );
#elif MADARCH & (MADARCH_AXP | MADARCH_PPC)
    /* unused parameters */ (void)stack;
    return( false );
#else
    #error GetEXEHeader not configured
#endif
}

bool GetModuleName( HANDLE fhdl, char *name )
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
        return( false );
    }
    if( hi.sig != EXE_PE ) {
        return( false );
    }
    seek_offset = SetFilePointer( fhdl, 0, NULL, FILE_CURRENT );
    if( seek_offset == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    export_rva = PE_DIRECTORY( hi.u.peh, PE_TBL_EXPORT ).rva;
    if( IS_PE64( hi.u.peh ) ) {
        num_objects = PE64( hi.u.peh ).num_objects;
        seek_offset += PE64( hi.u.peh ).nt_hdr_size + offsetof( pe_header64, magic ) - sizeof( exe_pe_header );
    } else {
        num_objects = PE32( hi.u.peh ).num_objects;
        seek_offset += PE32( hi.u.peh ).nt_hdr_size + offsetof( pe_header, magic ) - sizeof( exe_pe_header );
    }
    if( num_objects == 0 ) {
        return( false );
    }
    /* position to begining of object table */
    if( SetFilePointer( fhdl, seek_offset, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    for( i = 0; i < num_objects; i++ ) {
        if( !ReadFile( fhdl, &obj, sizeof( obj ), &lenread, NULL ) || lenread != sizeof( obj ) ) {
            return( false );
        }
        if( export_rva >= obj.rva && export_rva < obj.rva + obj.physical_size ) {
            break;
        }
    }
    if( i == num_objects ) {
        return( false );
    }
    if( !SeekRead( fhdl, obj.physical_offset + export_rva - obj.rva, &expdir, sizeof( expdir ) ) ) {
        return( false );
    }
    if( !SeekRead( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, buf, _MAX_PATH ) ) {
        return( false );
    }
    if( SetFilePointer( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    if( !ReadFile( fhdl, buf, _MAX_PATH, &lenread, NULL ) ) {
        return( false );
    }
    memcpy( name, buf, lenread );
    name[lenread] = '\0';
    return( true );
}

#if 0
int CpFile( HANDLE in )
{
    HANDLE  out;
    char    buff[1024];
    DWORD   lenread;
    DWORD   old;
    int     rc;

    out = CreateFile( "CP.OUT", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
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
