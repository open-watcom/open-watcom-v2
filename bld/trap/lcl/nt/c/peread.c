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
#include "stdnt.h"


bool SeekRead( HANDLE handle, DWORD newpos, void *buff, WORD size )
/******************************************************************
 * seek to a specified spot in the file, and read some data
 */
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

bool GetEXEHeader( HANDLE handle, header_info *hi, WORD *stack )
/***************************************************************
 * get type of EXE
 */
{
    WORD    data;
    WORD    signature;
    DWORD   ne_header_off;

    if( !SeekRead( handle, 0, &data, sizeof( data ) )
      || data != EXESIGN_DOS ) {
        return( false );
    }

    if( !SeekRead( handle, DOS_RELOC_OFFSET, &data, sizeof( data ) )
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( false );
    }

    if( !SeekRead( handle, NE_HEADER_OFFSET, &ne_header_off, sizeof( ne_header_off ) ) ) {
        return( false );
    }

    if( !SeekRead( handle, ne_header_off, &signature, sizeof( signature ) ) ) {
        signature = 0;
    }
    if( signature == EXESIGN_PE ) {
        DWORD      bytes;

        hi->signature = EXESIGN_PE;
        if( !SeekRead( handle, ne_header_off, &hi->u.pehdr, PE_HDR_SIZE )
          || !ReadFile( handle, (char *)&hi->u.pehdr + PE_HDR_SIZE, PE_OPT_SIZE( hi->u.pehdr ), &bytes, NULL ) ) {
            return( false );
        }
        return( true );
    }
#if MADARCH & MADARCH_X86
    if( signature == EXESIGN_NE ) {
        hi->signature = EXESIGN_NE;
        if( !SeekRead( handle, ne_header_off, &hi->u.nehdr, sizeof( hi->u.nehdr ) ) ) {
            return( false );
        }
        if( hi->u.nehdr.target == TARGET_WINDOWS ) {
            DWORD           off;
            unsigned char   len;
            DWORD           bytes;
            DWORD           pos;

            off = ne_header_off + hi->u.nehdr.resident_off;
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
            pos = ne_header_off + hi->u.nehdr.segment_off +
                ( hi->u.nehdr.adsegnum - 1 ) * sizeof( segment_record ) +
                offsetof( segment_record, min );
            if( !SeekRead( handle, pos, stack, sizeof( *stack ) ) ) {
                return( false );
            }
            *stack += hi->u.nehdr.stack;
            return( true );
        }
        return( false );
    }
    hi->signature = EXESIGN_DOS;
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

bool GetModuleName( HANDLE fhdl, char *buff, size_t maxlen )
{
    header_info         hi;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               lenread;
    DWORD               export_rva;
    DWORD               i;
    WORD                stack;
    int                 num_objects;

    if( !GetEXEHeader( fhdl, &hi, &stack ) ) {
        return( false );
    }
    if( hi.signature != EXESIGN_PE ) {
        return( false );
    }
    export_rva = PE_DIRECTORY( hi.u.pehdr, PE_TBL_EXPORT ).rva;
    num_objects = hi.u.pehdr.fheader.num_objects;
    if( num_objects == 0 ) {
        return( false );
    }
    memset( &obj, 0, sizeof( obj ) );
    for( i = 0; i < num_objects; i++ ) {
        if( !ReadFile( fhdl, &obj, sizeof( obj ), &lenread, NULL ) || lenread != sizeof( obj ) ) {
            return( false );
        }
        if( export_rva >= obj.rva && export_rva < ( obj.rva + obj.physical_size ) ) {
            break;
        }
    }
    if( i == num_objects ) {
        return( false );
    }
    if( !SeekRead( fhdl, obj.physical_offset + export_rva - obj.rva, &expdir, sizeof( expdir ) ) ) {
        return( false );
    }
    if( !SeekRead( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, buff, maxlen ) ) {
        return( false );
    }
    if( SetFilePointer( fhdl, obj.physical_offset + expdir.name_rva - obj.rva, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    if( maxlen > 0 )
        maxlen--;
    if( !ReadFile( fhdl, buff, maxlen, &lenread, NULL ) ) {
        return( false );
    }
    buff[lenread] = '\0';
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
