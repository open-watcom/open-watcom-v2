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


#include "owlpriv.h"
#include <stdarg.h>

extern void DumpString( char * );

#define MAX_LINE_LEN    512

static char *symbolName( owl_symbol_handle sym ) {
//************************************************

    char                *name;

    name = "UNNAMED";
    if( sym->name != NULL ) {
        name = &sym->name->text[ 0 ];
    }
    return( name );
}

static char *sectionName( owl_file_handle file, owl_symbol_handle sym ) {
//***********************************************************************

    char        *name = "UNDEFINED";

    file = file;
    if( sym->section != NULL ) {
        name = &sym->section->name->text[ 0 ];
    }
    return( name );
}

static char *symbolType( owl_symbol_handle sym ) {
//************************************************

    static char *typeName[] = { "function", "object", "section", "file" };
    return( typeName[ sym->type ] );
}

static char *symbolLinkage( owl_symbol_handle sym ) {
//***************************************************

    static char *linkageName[] = { "undefined", "function", "static", "global", "weak" };
    return( linkageName[ sym->linkage ] );
}

static void dumpSym( owl_file_handle file, owl_symbol_handle sym ) {
//******************************************************************

    char        buffer[ MAX_LINE_LEN ];

    sprintf( &buffer[ 0 ], "%s[%x]:\n", symbolName( sym ), sym );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\tsection\t\t%s[%x]\n", sectionName( file, sym ), sym->section );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\ttype\t\t%s[%x]\n", symbolType( sym ), sym->type );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\tlinkage\t\t%s[%x]\n", symbolLinkage( sym ), sym->linkage );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\trelocs\t\t%d\n", sym->num_relocs );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\toffset\t\t%x\n", sym->offset );
    DumpString( &buffer[ 0 ] );
    sprintf( &buffer[ 0 ], "\tflags\t\t%x\n", sym->flags );
    DumpString( &buffer[ 0 ] );
}

void OWLENTRY OWLDumpSymTab( owl_file_handle file ) {
//***************************************************

    owl_symbol_handle   sym;

    for( sym = file->symbol_table->head; sym != NULL; sym = sym->next ) {
        dumpSym( file, sym );
    }
}
