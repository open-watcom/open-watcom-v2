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


#include "wlib.h"

static input_lib *InputLibs;

void AddInputLib( libfile io, char *name )
{
    input_lib   *new;

    new = MemAllocGlobal( sizeof( *new ) + strlen( name ) );
    new->next = InputLibs;
    InputLibs = new;
    new->io = io;
    strcpy( new->name, name );
}

void CloseOneInputLib()
{
    input_lib   *curr;

    for( curr = InputLibs; curr != NULL; curr = curr->next ) {
        if( curr->io != NULL ) {
            LibClose( curr->io );
            curr->io = NULL;
            return;
        }
    }
}

void InitInputLibs()
{
    InputLibs = NULL;
}

void ResetInputLibs()
{
    input_lib   *curr,*next;

    for( curr = InputLibs; curr != NULL; curr = next ) {
        next = curr->next;
        if( curr->io != NULL ) {
            LibClose( curr->io );
        }
        MemFreeGlobal( curr );
    }
    InputLibs = NULL;
}

input_lib *FindInLib( libfile io )
{
    input_lib   *curr;

    for( curr = InputLibs; curr != NULL; curr = curr->next ) {
        if( curr->io == io ) {
            return( curr );
        }
    }
    return( NULL );
}

libfile InLibHandle( input_lib *curr )
{
    if( curr->io == NULL ) {
        curr->io = LibOpen( curr->name, LIBOPEN_BINARY_READ );
    }
    return( curr->io );
}
