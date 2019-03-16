/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "owlpriv.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFFER_SIZE     1024

static int owl_write( owl_client_file f, const char *buff, size_t size )
{
    return( fwrite( buff, 1, size, f ) != size );
}

static long owl_tell( owl_client_file f )
{
    return( ftell( f ) );
}

static int owl_seek( owl_client_file f, long offset, int where )
{
    return( fseek( f, offset, where ) );
}

void main( int argc, char *argv[] ) {

    owl_handle          owl;
    owl_file_handle     file;
    owl_client_funcs    funcs = { owl_write, owl_tell, owl_seek, malloc, free };
    owl_buffer_handle   buffer;
    char                data[ BUFFER_SIZE ];

    owl = OWLInit( &funcs, OWL_CPU_PPC );
    file = OWLFileInit( owl, "test", stdout, OWL_FORMAT_ELF, OWL_FILE_OBJECT );
    buffer = OWLBufferInit( file );
    while( 1 ) {
        memset( data, 0, sizeof( data ) );
        if( fgets( data, sizeof( data ), stdin ) == NULL )
            break;
        OWLBufferWrite( buffer, data, strlen( data ) );
    }
    OWLBufferEmit( buffer );
    OWLBufferFini( buffer );
    // OWLFileFini( file );
    // OWLFini( owl );
}
