/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


bool SeekRead( HANDLE h, DWORD newpos, void *buff, WORD size )
/******************************************************************
 * seek to a specified spot in the file, and read some data
 */
{
    DWORD   bytes;

    if( SetFilePointer( h, newpos, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        if( GetLastError() != NO_ERROR ) {
            return( false );
        }
    }
    if( ReadFile( h, buff, size, &bytes, NULL ) == 0 ) {
        return( false );
    }
    if( bytes != size ) {
        return( false );
    }
    return( true );

}

bool GetEXEHeader( HANDLE h, header_info *hi, WORD *stack )
/***************************************************************
 * get type of EXE
 */
{
    WORD    data;
    WORD    signature;
    DWORD   ne_header_off;

    if( !SeekRead( h, 0, &data, sizeof( data ) )
      || data != EXESIGN_DOS ) {
        return( false );
    }

    if( !SeekRead( h, DOS_RELOC_OFFSET, &data, sizeof( data ) )
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( false );
    }

    if( !SeekRead( h, NE_HEADER_OFFSET, &ne_header_off, sizeof( ne_header_off ) ) ) {
        return( false );
    }

    if( !SeekRead( h, ne_header_off, &signature, sizeof( signature ) ) ) {
        signature = 0;
    }
    if( signature == EXESIGN_PE ) {
        DWORD      bytes;

        hi->signature = EXESIGN_PE;
        if( !SeekRead( h, ne_header_off, &hi->u.pehdr, PE_HDR_SIZE )
          || ReadFile( h, (char *)&hi->u.pehdr + PE_HDR_SIZE, PE_OPT_SIZE( hi->u.pehdr ), &bytes, NULL ) == 0 ) {
            return( false );
        }
        return( true );
    }
#if MADARCH & MADARCH_X86
    if( signature == EXESIGN_NE ) {
        hi->signature = EXESIGN_NE;
        if( !SeekRead( h, ne_header_off, &hi->u.nehdr, sizeof( hi->u.nehdr ) ) ) {
            return( false );
        }
        if( hi->u.nehdr.target == TARGET_WINDOWS ) {
            DWORD           off;
            unsigned char   len;
            DWORD           bytes;
            DWORD           pos;

            off = ne_header_off + hi->u.nehdr.resident_off;
            if( !SeekRead( h, off, &len, sizeof( len ) ) ) {
                return( false );
            }
            if( len > sizeof( hi->modname ) - 1 ) {
                len = sizeof( hi->modname ) - 1;
            }
            if( ReadFile( h, hi->modname, len, &bytes, NULL ) == 0 ) {
                return( false );
            }
            hi->modname[len] = 0;
            pos = ne_header_off + hi->u.nehdr.segment_off +
                ( hi->u.nehdr.adsegnum - 1 ) * sizeof( segment_record ) +
                offsetof( segment_record, min );
            if( !SeekRead( h, pos, stack, sizeof( *stack ) ) ) {
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

bool GetModuleName( HANDLE h, char *buff, size_t maxlen )
{
    header_info         hi;
    pe_object           obj;
    pe_export_directory expdir;
    DWORD               lenread;
    DWORD               export_rva;
    DWORD               i;
    WORD                stack;
    int                 num_objects;

    if( !GetEXEHeader( h, &hi, &stack ) ) {
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
        if( ReadFile( h, &obj, sizeof( obj ), &lenread, NULL ) == 0
          || lenread != sizeof( obj ) ) {
            return( false );
        }
        if( export_rva >= obj.rva && export_rva < ( obj.rva + obj.physical_size ) ) {
            break;
        }
    }
    if( i == num_objects ) {
        return( false );
    }
    if( !SeekRead( h, obj.physical_offset + export_rva - obj.rva, &expdir, sizeof( expdir ) ) ) {
        return( false );
    }
    if( !SeekRead( h, obj.physical_offset + expdir.name_rva - obj.rva, buff, maxlen ) ) {
        return( false );
    }
    if( SetFilePointer( h, obj.physical_offset + expdir.name_rva - obj.rva, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        if( GetLastError() != NO_ERROR ) {
            return( false );
        }
    }
    if( maxlen > 0 )
        maxlen--;
    if( ReadFile( h, buff, maxlen, &lenread, NULL ) == 0 ) {
        return( false );
    }
    buff[lenread] = '\0';
    return( true );
}

#if 0
int CpFile( HANDLE hin )
{
    HANDLE  hout;
    char    buff[1024];
    DWORD   lenread;
    DWORD   old;
    int     rc;

    hout = CreateFile( "CP.OUT", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if( hout == INVALID_HANDLE_VALUE ) {
        return( 1 );
    }
    old = SetFilePointer( hin, 0, NULL, FILE_CURRENT );
    SetFilePointer( hin, 0, NULL, FILE_BEGIN );
    rc = 0;
    for( ;; ) {
        if( ReadFile( hin, buff, sizeof( buff ), &lenread, NULL ) == 0 ) {
            rc = 2;
            break;
        }
        if( lenread == 0 ) {
            break;
        }
        if( WriteFile( hout, buff, lenread, &lenread, NULL ) == 0 ) {
            rc = 3;
            break;
        }
    }
    SetFilePointer( hin, old, NULL, FILE_BEGIN );
    CloseHandle( hout );
    return( rc );
}
#endif
