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


#include <wlib.h>

void BadLibrary( char *name )
/***************************/
{
    FatalError( ERR_BAD_LIBRARY, name );
}


void AllocFNameTab( char *name, libfile io, arch_header *arch )
/*************************************************************/
{
    MemFree( arch->fnametab );
    GetFileContents( name, io, arch, &arch->fnametab );
}

void AllocFFNameTab( char *name, libfile io, arch_header *arch )
/**************************************************************/
{
    MemFree( arch->ffnametab );
    GetFileContents( name, io, arch, &arch->ffnametab );
    arch->nextffname = arch->ffnametab;
    arch->lastffname = arch->nextffname + arch->size;
}


void LibWalk( libfile io, char *name, void (*rtn)( arch_header *, libfile io ) )
/**********************************************************************/
{
    ar_header           ar;
    arch_header         arch;
    file_offset         bytes_read;
    int                 dict_count;
    file_offset         pos;

    dict_count = 0;
    arch.fnametab = NULL;
    arch.ffnametab = NULL;
    for( ;; ) {
        bytes_read = LibRead( io, &ar, AR_HEADER_SIZE );
        if( bytes_read == 0 ) break;
        if( bytes_read != AR_HEADER_SIZE ) {
            BadLibrary( name );
        }
        if( strncmp( ar.header_ident, AR_HEADER_IDENT, AR_HEADER_IDENT_LEN ) ) {
            BadLibrary( name );
        }
        GetARHeaderValues( &ar, &arch );
        pos = LibTell( io );
        if( ar.name[0] == '/' && ar.name[1] == ' ' && ar.name[2] == ' ' ) {
            // Ignore symbol table.
/*
            dict_count++;
            if( dict_count == 2 ) {
                error = readDict( &arch );
            } else {
                error = MoveAheadFrom( &arch );
                updateNewArchive( &arch );
            }
*/
        } else if( ar.name[0] == '/' && ar.name[1] == '/' && ar.name[2] == ' ' ) {
            AllocFNameTab( name, io, &arch );
        } else if( ar.name[0] == '/' && ar.name[1] == '/' && ar.name[2] == '/' ) {
            AllocFFNameTab( name, io, &arch );
        } else {
            arch.name = GetARName( &ar, &arch );
            arch.ffname = GetFFName( &arch );
            rtn( &arch, io );
            MemFree( arch.name );
            MemFree( arch.ffname );
        }
        if( arch.size & 1 ) ++arch.size;
        LibSeek( io, pos + arch.size, SEEK_SET );
    }
    MemFree( arch.fnametab );
    MemFree( arch.ffnametab );
}
