/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "wlib.h"
#include <errno.h>
#include "wio.h"
#include "wlibutil.h"

#include "clibext.h"


libfile ExportListFile;
libfile NewLibrary;

char *MakeTmpName( char * );

void WriteNew( const void *buff, size_t len )
{
    LibWrite( NewLibrary, buff, len );
}

void WriteNewLib( void )
{
    char tmp[_MAX_PATH + 1];
    char *bak,*lib,*out;

    lib = Options.input_name;
    if( Options.output_name != NULL && !IsSameFile( lib, Options.output_name ) ) {
        out = Options.output_name;
    } else {
        out = MakeTmpName( tmp );
    }
    if( Options.export_list_file != NULL ) {
        ExportListFile = LibOpen( Options.export_list_file, LIBOPEN_WRITE );
    } else {
        ExportListFile = NULL;
    }
    NewLibrary = LibOpen( out, LIBOPEN_WRITE );
    if( NewLibrary == NULL ) {
        if( out == tmp ) {
            FatalError( ERR_CANT_OPEN, out, "Cannot create temporary file" );
        } else {
            FatalError( ERR_CANT_OPEN, out, strerror( errno ) );
        }
    }
    WriteFileTable();
    LibClose( NewLibrary );
    if( ExportListFile != NULL ) {
        LibClose( ExportListFile );
    }
    CloseInputLibs();   //closes all input libs
    if( out == tmp ) {
        bak = MakeBakName();
        if( access( bak, F_OK ) == 0 && remove( bak ) != 0 ) {
            FatalError( ERR_CANT_REMOVE, bak );
        }
        if( access( lib, F_OK ) == 0 && rename( lib, bak ) != 0 ) {
            FatalError( ERR_CANT_REPLACE, lib, strerror( errno ) );
        }
        if( rename( tmp, lib ) != 0 ) {
            rename( bak, lib );
            FatalError( ERR_CANT_REPLACE, lib, strerror( errno ) );
        }
        if( Options.no_backup ) {
            remove( bak );
        }
    }
}

void WriteBigEndian32( unsigned_32 num )
{
    CONV_BE_32( num );
    WriteNew( &num, sizeof( num ) );
}

void WriteLittleEndian32( unsigned_32 num )
{
    CONV_LE_32( num );
    WriteNew( &num, sizeof( num ) );
}


void WriteLittleEndian16( unsigned_16 num )
{
    CONV_LE_16( num );
    WriteNew( &num, sizeof( num ) );
}
