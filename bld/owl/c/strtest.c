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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include "owlpriv.h"

#define BUFFER_SIZE     1024


static void addStrings( owl_string_table *table, char *buffer ) {

    char                *s;
    char                *last;

    last = buffer;
    for( s = buffer; *s; ) {
        if( isspace( *s ) ) {
            *s++ = 0;
            printf( "Inserting: '%s'\n", last );
            OWLStringAdd( table, last );
            while( isspace( *s ) ) s++;
            last = s;
        } else {
            s++;
        }
    }
}

void main( int argc, char *argv[] ) {

    char                buffer[ BUFFER_SIZE ];
    owl_handle          owl;
    owl_file_handle     file;
    owl_string_table    *table;
    owl_client_funcs    funcs = { write, tell, lseek, malloc, free };
    char                *t_buff;

    owl = OWLInit( &funcs, OWL_CPU_PPC );
    file = OWLFileInit( owl, (owl_client_file)NULL, OWL_FORMAT_ELF, OWL_FILE_OBJECT );
    table = OWLStringInit( file );
    while( fgets( buffer, BUFFER_SIZE, stdin ) != NULL ) {
        addStrings( table, buffer );
        OWLStringDump( table );
    }
    t_buff = malloc( OWLStringTableSize( table ) + 1 );
    OWLStringEmit( table, t_buff );
    write( STDOUT_FILENO, t_buff, OWLStringTableSize( table ) );
    free( t_buff );
    OWLStringFini( table );
    OWLFileFini( file );
    OWLFini( owl );
}
