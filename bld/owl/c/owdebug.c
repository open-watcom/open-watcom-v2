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

/*
 * NOTE: This is all horribly cheesy and COFF specific, but I am past the
 * point of caring. ELF can bite me.
 */

static void outLineNum( owl_func_handle func, owl_line_num line, owl_offset addr ) {
//**********************************************************************************
// NB: want line numbers relative to the start of the function

    coff_line_num       rec;

    assert( func->section->linenum_buffer != NULL );
    rec.ir.RVA = addr;
    rec.line_number = ( ( line == 0 ) ? 0 : ( line - func->x.func->first_line ) );
    OWLBufferWrite( func->section->linenum_buffer, (const char *)&rec, sizeof( coff_line_num ) );
    func->section->num_linenums += 1;
}

owl_func_handle OWLENTRY OWLDebugFuncBegin( owl_section_handle section, owl_symbol_handle func, owl_line_num first, owl_offset start ) {
//**************************************************************************************************************************************

    owl_func_info      *info;

    assert( func != NULL );
    if( section->linenum_buffer == NULL ) {
        section->linenum_buffer = OWLBufferInit( section->file );
    }
    info = _ClientAlloc( section->file, sizeof( owl_func_info ) );
    func->x.func = info;
    info->start = start;
    info->end = 0;
    info->first_line = first;
    info->last_line = 0;
    info->linenum_offset = OWLBufferTell( section->linenum_buffer );
    info->num_lines = 1; // first entry
    info->head = NULL;
    info->lnk = &info->head;
    outLineNum( func, 0, 0 );
    _Log((section->file, "OWLDebugFuncBegin( %x, %x, %d, %x ) -> %x\n", section, func, first, start ));
    return( func );
}

void OWLENTRY OWLDebugFuncLine( owl_func_handle func, owl_line_num line, owl_offset addr ) {
//******************************************************************************************

    _Log((func->section->file, "OWLDebugFuncLine( %x, %d, %x )\n", func, line, addr ));
    outLineNum( func, line, addr );
    func->x.func->num_lines += 1;
}

void OWLENTRY OWLDebugFuncFile( owl_func_handle func, char const *name  ) {
//*************************************************************************
    owl_func_info     *info;
    owl_func_file     *new;
    int                size;

    info = func->x.func;
    _Log((func->section->file, "OWLDebugFuncFile( %x, %s )\n", func, name ));
    size = strlen( name );
    new = _ClientAlloc( func->section->file, sizeof( owl_func_file )+size );
    strcpy( new->name, name );
    new->num_lines = info->num_lines; // save prev
    info->num_lines = 0;
    new->next = NULL;
    *info->lnk = new;
    info->lnk = &new->next;
}

void OWLENTRY OWLDebugFuncEnd( owl_func_handle func, owl_line_num last, owl_offset end ) {
//****************************************************************************************

    _Log((func->section->file, "OWLDebugFuncEnd( %x, %d, %x )\n", func, last, end ));
    func->x.func->end = end;
    func->x.func->last_line = last;
}
